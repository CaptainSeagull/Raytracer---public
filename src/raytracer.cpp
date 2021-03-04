
//
// Interface
struct Material {
    F32 specular; // 0 is pure diffuse ("chalk"), 1 is pure specular ("mirror")
    V3 emit_colour;
    V4 ref_colour;
};
internal Material create_material(V3 emit_colour, V4 ref_colour, F32 specular);

struct Plane {
    V3  n;
    F32 d;
    Int mat_i;
};
internal Plane create_plane(V3 n, F32 d, Int mat_i);

struct Sphere {
    V3  p;
    F32 r;
    Int mat_i;
};
internal Sphere create_sphere(V3 p, F32 r, Int mat_i);

// TODO: Is it better to render faces normally or pre-triangulated? Do some investigation into the performance difference.
struct Face {
    V3  *p;
    Int p_len;
    V3  n;
    F32 d;

    Int mat_i;
};
internal Face create_face(Memory *memory, V3 *p, Int p_len, Int mat_i);

struct Camera {
    V3 p;
    V3 x;
    V3 y;
    V3 z;
};
internal Camera create_camera(V3 p, V3 pd, V3 r, V3 s) {
    Camera camera = {};
    M4x4 transform = iden();

    transform = scale(transform, s);

    // TODO: Is the != 0 optimisation worth it? Seems unlikely it'll ever really be true...
    if(r.x != 0) { transform *= x_rotation(LANE_DEGREES_TO_RADIANS(r.x)); }
    if(r.y != 0) { transform *= y_rotation(LANE_DEGREES_TO_RADIANS(r.y)); }
    if(r.z != 0) { transform *= z_rotation(LANE_DEGREES_TO_RADIANS(r.z)); }

    // This is a raytracer (not a rasteriser) so for the camera transform we actually want to transform it like an object (so columns
    // are the axis NOT rows). So transpose the matrix and use the columns.
    transform = transpose(transform);

    // Rotation (grab cols NOT rows since we transposed the matrix).
    camera.x = extract_col(transform, 0).xyz;
    camera.y = extract_col(transform, 1).xyz;
    camera.z = extract_col(transform, 2).xyz;

    // Translation
    V3 local_amount_to_move = transform * pd;
    camera.p = p + local_amount_to_move;

    return(camera);
}


struct Scene {
    Int material_count;
    Material *materials;

    Int plane_count;
    Plane *planes;

    Int sphere_count;
    Sphere *spheres;

    Int face_count;
    Face *faces;
};

internal Void render_scene(Scene *scene, Camera *camera, Image *image, Int core_count, Int max_bounce_count, Int rays_per_pixel, U64 random_seed);

//
// Implementation

struct Work_Order {
    Scene *scene;
    Camera *camera;
    Image *image;

    Int x_min;
    Int y_min;

    Int one_past_x_max;
    Int one_past_y_max;

    Lane_U32 entropy;
};

struct Work_Queue {
    Int work_order_count;
    Work_Order *work_orders;

    Int rays_per_pixel;
    Int max_bounce_count;

    volatile U64 next_work_order_index;
    volatile U64 bounces_computed;
    volatile U64 loops_computed;
    volatile U64 tile_retire_count;
};

internal Material create_material(V3 emit_colour, V4 ref_colour, F32 specular) {
    Material m;
    m.emit_colour = emit_colour;
    m.ref_colour = ref_colour;
    m.specular = specular;

    return(m);
}

internal Plane create_plane(V3 n, F32 d, Int mat_i) {
    Plane p;
    p.n = n;
    p.d = d;
    p.mat_i = mat_i;

    return(p);
}

internal Sphere create_sphere(V3 p, F32 r, Int mat_i) {
    Sphere s;
    s.p = p;
    s.r = r;
    s.mat_i = mat_i;

    return(s);
}

internal Face create_face(Memory *memory, V3 *p, Int p_len, Int mat_i) {
    ASSERT(p_len >= 3);

    Face face = {};
    face.p_len = p_len;
    face.p = (V3 *)memory_push(memory, Memory_Index_permanent, sizeof(V3) * (p_len + 1));
    for(Int i = 0; (i < p_len); ++i) {
        face.p[i] = v3(p[i]);
    }
    face.p[p_len] = p[0]; // Write the start onto the end.
    face.n = normalise(cross(p[1] - p[0], p[2] - p[0]));
    face.d = inner(face.n, face.p[0]);
    face.mat_i = mat_i;

    // TODO: ASSERT all the points are on the same plane.

    return(face);
}

/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
internal U64 xorshift64(U64 *a) {
    *a ^= *a << 13;
    *a ^= *a >> 7;
    *a ^= *a << 17;

    return(*a);
}

struct Hit_Info {
    Lane_F32 distance;
    Lane_U32 mat_i;
    Lane_V3 normal;
};

internal Hit_Info hit_info(Void) {
    Hit_Info r;
    r.distance = lane_f32(LANE_FLT_MAX);
    r.mat_i = lane_u32(0);
    r.normal = lane_v3_zero();

    return(r);
}

internal Bool render_tile(Work_Queue *queue) {
    Bool res = false;
    U64 work_order_index = system_locked_add(&queue->next_work_order_index, 1);
    if(work_order_index < queue->work_order_count) {
        res = true;

        Work_Order *order = queue->work_orders + work_order_index;

        Image *image = order->image;
        Int x_min = order->x_min;
        Int y_min = order->y_min;
        Int one_past_x_max = order->one_past_x_max;
        Int one_past_y_max = order->one_past_y_max;
        F32 film_dist = 1.0f;

        Scene *scene = order->scene;
        Camera *camera = order->camera;
        Int rays_per_pixel = queue->rays_per_pixel;
        Int max_bounce_count = queue->max_bounce_count;
        Lane_U32 entropy_series = order->entropy;

        V2 film = v2(1.0f, 1.0f);
        if     (image->width > image->height) { film.h = film.w * ((F32)image->height / (F32)image->width ); }
        else if(image->height > image->width) { film.w = film.h * ((F32)image->width  / (F32)image->height); }

        V2 half_film = v2(film.w * 0.5f,  film.h * 0.5f);

        V3 film_centre = camera->p - (film_dist * camera->z);

        V2 half_pix = v2(0.5f / (F32)image->width, 0.5f / (F32)image->height);
        //F32 half_pix.w = 0.5f / (F32)image->width;
        //F32 half_pix.h = 0.5f / (F32)image->height;

        Int total_bounces_computed = 0;
        Int total_loops_computed = 0;

        for(Int y = y_min; (y < one_past_y_max); ++y) {
            U32 *out = get_pixel_pointer(image, x_min, y);

            F32 film_y = -1.0f + (2.0f * ((F32)y / (F32)image->height));
            for(Int x = x_min; (x < one_past_x_max); ++x) {
                F32 film_x = -1.0f + (2.0f * ((F32)x / (F32)image->width));

                V3 output_final_colour = v3(0);

                // Start of SIMD raytracing!
                Lane_V3 final_colour = lane_v3_zero();
                Lane_U32 bounces_computed = lane_u32(0);
                U64 loops_computed = 0;
                Lane_V3 camera_x = lane_v3(camera->x);
                Lane_V3 camera_y = lane_v3(camera->y);
                Lane_V3 camera_z = lane_v3(camera->z);
                Lane_V3 camera_p = lane_v3(camera->p);

                Int ray_count_over_lane_width = (rays_per_pixel / LANE_WIDTH);
                ASSERT((ray_count_over_lane_width * LANE_WIDTH) == rays_per_pixel);

                F32 contribution_per_sample = 1.0f / (F32)rays_per_pixel;
                for(Int ray_i = 0; (ray_i < ray_count_over_lane_width); ++ray_i) {
                    Lane_V2 offset;
#if RANDOM_SAMPLE_ANTI_ALIASING
                    // Grab the centre of the pixel and offset random (still within the pixel) to create an anti-aliasing effect.
                    offset.x = film_x + random_bilateral(&entropy_series) * half_pix.w;
                    offset.y = film_y + random_bilateral(&entropy_series) * half_pix.h;
#else
                    // Just grab the centre of the pixel
                    offset.x = lane_f32(film_x + half_pix.w);
                    offset.y = lane_f32(film_y + half_pix.h);
#endif

                    // TODO: film_centre, film_p etc... are kinda crappy names
                    Lane_V3 film_p = film_centre + ((offset.x * half_film.w * camera_x) + (offset.y * half_film.h * camera_y));
                    Lane_V3 ro = camera_p;
                    Lane_V3 rd = normalise_or_zero(film_p - camera_p);

                    Lane_F32 tolerance = lane_f32(0.0001f);
                    Lane_F32 min_hit_distance = lane_f32(0.001f);
                    Lane_F32 total_hit_distance = lane_f32(LANE_FLT_MAX);

                    Lane_V3 sample = lane_v3_zero();
                    Lane_V3 attenuation = lane_v3(1.0f);

                    Lane_U32 lane_mask = lane_u32(0xFFFFFFFF);

                    // TODO: Investigate a bounding-volume hierarchy.
                    // TODO: Monte-carlo termination?

                    for(Int bounce_count = 0; (bounce_count < max_bounce_count); ++bounce_count) {
                        Hit_Info hit = hit_info();

                        Lane_U32 lane_increment = lane_u32(1);
                        bounces_computed += (lane_increment & lane_mask);
                        loops_computed += LANE_WIDTH;

                        //
                        // Plane
                        for(Int plane_i = 0; (plane_i < scene->plane_count); ++plane_i) {
                            // TODO: Handle back-planes

                            // Ray   -> r0+trd
                            // Plane -> Np+d=0
                            // Subsituting the Ray into "p" and solving for t we get:
                            //   -> t = -d - N*ro / N*rd

                            Plane *plane = &scene->planes[plane_i];

                            Lane_V3 n = lane_v3(plane->n);

                            Lane_F32 denom = inner(n, rd);
                            Lane_U32 denom_mask = ((denom < -tolerance) | (denom > tolerance));
                            if(!lane_mask_is_zeroed(denom_mask)) { // Ray is perpendicular to plane.
                                Lane_F32 d = lane_f32(plane->d);

                                Lane_F32 t = (-d - inner(n, ro)) / denom;
                                Lane_U32 t_mask = ((t > min_hit_distance) & (t < hit.distance));

                                Lane_U32 hit_mask = (denom_mask & t_mask);
                                if(!lane_mask_is_zeroed(hit_mask)) {
                                    // TODO: Handle transparency!
                                    Lane_U32 plane_mat_i = lane_u32(plane->mat_i);

                                    conditional_assign(hit_mask, &hit.distance, t);
                                    conditional_assign(hit_mask, &hit.mat_i, plane_mat_i);
                                    conditional_assign(hit_mask, &hit.normal, n);
                                }
                            }
                        }

                        //
                        // Face
                        for(Int face_i = 0; (face_i < scene->face_count); ++face_i) {
                            // TODO: Handle backfaces.

                            // For the initial test check if the ray is perpendicular to the face's normal.
                            // Ray                       -> r0+trd
                            // Plane from face's normal: -> Np+d=0
                            // Subsituting the Ray into "p" and solving for t we get:
                            //   -> t = -d - N*ro / N*rd
                            // If abs(t) > tolerance we're not perpendicular and have a chance to hit.

                            Face *face = &scene->faces[face_i];

                            Lane_V3 n = lane_v3(face->n);

                            Lane_F32 denom = inner(n, rd);
                            Lane_U32 denom_mask = ((denom < -tolerance) | (denom > tolerance));
                            if(!lane_mask_is_zeroed(denom_mask)) {
                                Lane_F32 d = lane_f32(face->d);

                                Lane_F32 t = (-d - inner(n, ro)) / denom;
                                Lane_U32 t_mask = ((t > min_hit_distance) & (t < hit.distance));
                                if(!lane_mask_is_zeroed(t_mask)) {
                                    Lane_V3 ray_hit_pt = ro + t * rd; // Where the ray hits the plane the triangle's on.

                                    Lane_U32 running_hit_mask = lane_u32(0xFFFFFFFF);
                                    // "Inside-outside" test.
                                    // From:
                                    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
                                    // https://www.scratchapixel.com/code.php?id=9&origin=/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle

                                    // TODO: This loop may make things slow. Using triangulated... triangles may make it faster
                                    //       since we're iterating over a known length. Maybe have a Triangle and Quad struct? Could just
                                    //       reimplement OpenGL in software while I'm at it...
                                    for(Int edge_i = 0, p_len = face->p_len; (edge_i < p_len); ++edge_i) {
                                        Lane_V3 v0 = lane_v3(face->p[edge_i + 0]);
                                        Lane_V3 v1 = lane_v3(face->p[edge_i + 1]);

                                        Lane_V3 edge = v1 - v0;
                                        Lane_V3 vp0 = ray_hit_pt - v0;
                                        Lane_V3 c = cross(edge, vp0);

                                        running_hit_mask = running_hit_mask & (inner(n, c) > 0); // p is on the right side

                                        if(lane_mask_is_zeroed(running_hit_mask)) {
                                            break; // for edge_i
                                        }
                                    }
#if 1
                                    Lane_U32 hit_mask = denom_mask & t_mask & running_hit_mask;
#else
                                    Lane_U32 hit_mask = denom_mask & t_mask; // Turn into a plane
#endif
                                    if(!lane_mask_is_zeroed(hit_mask)) {
                                        Lane_U32 mat_i = lane_u32(face->mat_i);

                                        conditional_assign(hit_mask, &hit.distance, t);
                                        conditional_assign(hit_mask, &hit.mat_i, mat_i);
                                        conditional_assign(hit_mask, &hit.normal, n);
                                    }
                                }
                            }
                        }

                        //
                        // Sphere
                        for(Int sphere_i = 0; (sphere_i < scene->sphere_count); ++sphere_i) {
                            // Ray    -> ro + trd
                            // Sphere -> p*p - r*r = 0
                            // Subsituting the ray into "p" and solving we get:
                            //   -> t*t(rd*rd) + r(2rd*ro) + ro*ro - r*r = 0
                            // which we can solve for t using the quadratic equation:
                            //   -> t = -b +- (sqrt(b*b - 4*a*c) / 2*a)
                            // We end up with 2 values for t (the "in" and "out" of the sphere for the ray), so use the closest value.

                            Sphere *sphere = &scene->spheres[sphere_i];

                            Lane_V3 sphere_p = lane_v3(sphere->p);
                            Lane_F32 sphere_r = lane_f32(sphere->r);

                            Lane_V3 sphere_relative_ray_origin = (ro - sphere_p);

                            // We normalise rd so it's length should always be 1. So just comment out calculations including a.
                            //Lane_F32 a = lane_f32(1.0f);//inner(rd, rd);
                            Lane_F32 b = lane_f32(2.0f) * inner(rd, sphere_relative_ray_origin);
                            Lane_F32 c = inner(sphere_relative_ray_origin, sphere_relative_ray_origin) - (sphere_r * sphere_r);

                            Lane_F32 root_term = square_root((b * b) - (4.0f /* * a */ * c));

                            Lane_U32 root_mask = (root_term > tolerance);
                            if(!lane_mask_is_zeroed(root_mask)) { // Hit/miss the sphere
                                Lane_F32 denom = lane_f32(2.0f) /* * a*/;
                                Lane_F32 tp = (-b + root_term) / denom;
                                Lane_F32 tn = (-b - root_term) / denom;

                                Lane_F32 t = tp;
                                Lane_U32 pick_mask = ((tn > min_hit_distance) & (tn < tp));
                                conditional_assign(pick_mask, &t, tn);

                                Lane_U32 t_mask = ((t > min_hit_distance) & (t < hit.distance));
                                Lane_U32 hit_mask = (root_mask & t_mask);

                                if(!lane_mask_is_zeroed(hit_mask)) {
                                    Lane_U32 sphere_mat_i = lane_u32(sphere->mat_i);
                                    Lane_V4 mat_ref_colour = GATHER_V4(scene->materials, sphere_mat_i, ref_colour);

                                    Lane_V3 sphere_normal = normalise_or_zero(t * rd + sphere_relative_ray_origin);
                                    Lane_V3 sphere_tangent = normalise_or_zero(cross(lane_v3(0, 0, 1), sphere_normal));

                                    conditional_assign(hit_mask, &hit.distance, t);
                                    conditional_assign(hit_mask, &hit.mat_i, sphere_mat_i);
                                    conditional_assign(hit_mask, &hit.normal, sphere_normal);
                                }
                            }
                        }

                        // Crappy gather...
                        // If the lane_mask has failed for this lane then we gather v3(0, 0, 0) then don't add it to the final sample
                        Lane_V3 mat_emit_colour = lane_mask & GATHER_V3(scene->materials, hit.mat_i, emit_colour);
                        Lane_F32 mat_specular = GATHER_F32(scene->materials, hit.mat_i, specular);
                        Lane_V4 mat_ref_colour = GATHER_V4(scene->materials, hit.mat_i, ref_colour);

                        Lane_F32 light_power = lane_f32(1.0f); // TODO; How is this calculated?
                        sample += hadamard(attenuation, mat_emit_colour) * light_power;

                        lane_mask &= (hit.mat_i != 0);

                        if(!lane_mask_is_zeroed(lane_mask)) {
                            // Because we're going backwards we want the attenuation to increase with each bounce as it heads towards
                            // the emitter (either a light source or the sky).

                            total_hit_distance += hit.distance;

                            Lane_F32 denom = 4.0f * LANE_PI32 * (total_hit_distance * total_hit_distance);
                            // TODO: Test denom for 0?
                            Lane_F32 cos_attenuation = inner(-rd, hit.normal) / lane_min(lane_f32(1.0f), denom);

                            attenuation = hadamard(attenuation, lane_max(cos_attenuation, 0) * mat_ref_colour.rgb);

                            ro += (hit.distance * rd);

                            // If the object is transparent then let ref_colour.a/1.0 % of rays pass through it.
                            Lane_U32 weighted_transparency_mask = (random_unilateral(&entropy_series) <= mat_ref_colour.a);
                            if(!lane_mask_is_zeroed(weighted_transparency_mask)) {
                                Lane_V3 reflection = rd - (2.0f * inner(rd, hit.normal) * hit.normal);
                                Lane_V3 random_bounce = normalise_or_zero(hit.normal + lane_v3(random_bilateral(&entropy_series),
                                                                                               random_bilateral(&entropy_series),
                                                                                               random_bilateral(&entropy_series)));
                                // Use lerp so specular sufaces bounce exactly, but "chalky" surfaces less so.
                                conditional_assign(weighted_transparency_mask, &rd, normalise_or_zero(lerp(mat_specular, random_bounce, reflection)));
                            }
                        } else {
                            break;
                        }
                    }

                    final_colour += contribution_per_sample * sample;
                }
                // End of SIMD raytracing!

                total_bounces_computed += lane_horizontal_add(bounces_computed);
                total_loops_computed += loops_computed;
                output_final_colour = lane_horizontal_add(final_colour);

                V4 srgb = linear_to_srgb(output_final_colour);

                U32 bmp_value = ((round_f32_to_u32(srgb.a) << 24) |
                                 (round_f32_to_u32(srgb.r) << 16) |
                                 (round_f32_to_u32(srgb.g) <<  8) |
                                 (round_f32_to_u32(srgb.b) <<  0));

                *out++ = bmp_value;
            }
        }

        system_locked_add(&queue->bounces_computed, total_bounces_computed);
        system_locked_add(&queue->loops_computed, total_loops_computed);
        system_locked_add(&queue->tile_retire_count, 1);
    }

    return(res);
}

internal Void render_scene(Scene *scene, Camera *camera, Image *image,
                           Int core_count, Int max_bounce_count, Int rays_per_pixel, U64 random_seed) {
    Int width = image->width;
    Int height = image->height;

    Int tile_width = width / core_count;
    Int tile_height = tile_width;

    Int tile_count_x = (width + tile_width - 1) / tile_width;
    Int tile_count_y = (height + tile_height - 1) / tile_height;
    U64 total_tile_count = (tile_count_x * tile_count_y);

    Work_Queue queue = {};

    queue.max_bounce_count = max_bounce_count;

    Work_Order work_orders[256] = {};
    ASSERT(total_tile_count < ARRAY_COUNT(work_orders));
    queue.work_orders = &work_orders[0];

    queue.rays_per_pixel = rays_per_pixel;

    for(Int tile_y = 0; (tile_y < tile_count_y); ++tile_y) {
        Int min_y = tile_y * tile_height;
        Int one_past_max_y = MIN(min_y + tile_height, image->height);

        for(Int tile_x = 0; (tile_x < tile_count_x); ++tile_x) {
            Int min_x = tile_x * tile_width;
            Int one_past_max_x = MIN(min_x + tile_width, image->width);

            Work_Order *order = queue.work_orders + queue.work_order_count++;
            ASSERT(queue.work_order_count <= total_tile_count);

            order->scene = scene;
            order->camera = camera;
            order->image = image;
            order->x_min = min_x;
            order->y_min = min_y;
            order->one_past_x_max = one_past_max_x;
            order->one_past_y_max = one_past_max_y;

            // TODO: Replace this with real entropy!
            Lane_U32 entropy = lane_u32(xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                        xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                        xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                        xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                        xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                        xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                        xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                        xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed));
            order->entropy = entropy;
        }
    }
    ASSERT(queue.work_order_count == total_tile_count);

    // TODO: Use Windows fence here.
    // Fence may not be nessessary if create_thread fences on the system.You go
    system_locked_add(&queue.next_work_order_index, 0);

    // TODO: Would be better to just leave threads always running and reuse them rather than create them per-scene.
    // Assume this thread is core 0.
    for(Int core_index = 1; (core_index < core_count); ++core_index) {
        system_create_thread(&queue);
    }

    // Actual render loop.
    while(queue.tile_retire_count < total_tile_count) {
        if(render_tile(&queue)) {
            // Still going!
        }
    }
}

// Thread  callback from platform code.
internal Void worker_thread_callback(Void *p) {
    Work_Queue *queue = (Work_Queue *)p;
    while(render_tile(queue)) {};
}
