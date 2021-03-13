
//
// Interface
struct Material {
    F32 specular; // 0 is pure diffuse ("chalk"), 1 is pure specular ("mirror")
    V3 emit_colour;
    V4 ref_colour;
};
internal Material create_material(V3 emit_colour, V4 ref_colour, F32 specular);

struct Texture {
    F32 specular;
    V3 emit_colour;
    Image image;
};
internal Texture create_texture(V3 emit_colour, Image image, F32 specular);

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
    V3  *pts;
    Int p_len;
    V3  n;
    F32 d;

    Int mat_i;
};
internal Face create_face(Memory *memory, V3 *p, Int p_len, Int mat_i);
internal Face create_face(Memory *memory, V2 *p, Int p_len, V3 translation, V3 rotation, V3 sc, Int mat_i);

// TODO: This structure is larger than it has to be, may end up blowing over cache lines.
struct Textured_Face {
    // TODO: Could move this to the end, require Texture_Faces be allocated, then point this to just past the end. Would keep the
    //       memory more local.
    V3  *pts;
    Int p_len;

    V3  n;
    F32 d;

    V3 translation;
    V3 rotation;
    V3 scale;

    // TODO: Do I need to store these in world-space? Could I store 2 V2's in local space?
    V3 top_left;
    V3 bottom_right;

    // TODO: Add different modes. Repeated, stretched, etc...?
    Int tex_i;
};
internal Textured_Face create_textured_face(Memory *memory, V2 *p, Int p_len, V3 rot, Int tex_i);

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

    Int texture_count;
    Texture *textures;

    Int plane_count;
    Plane *planes;

    Int sphere_count;
    Sphere *spheres;

    Int face_count;
    Face *faces;

    Int textured_face_count;
    Textured_Face *textured_faces;
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

internal Texture create_texture(V3 emit_colour, Image image, F32 specular) {
    Texture t;
    t.emit_colour = emit_colour;
    t.image = image;
    t.specular = specular;

    return(t);
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
    face.pts = (V3 *)memory_push(memory, Memory_Index_permanent, sizeof(V3) * (p_len + 1));
    for(Int i = 0; (i < p_len); ++i) {
        face.pts[i] = p[i];
    }

    face.pts[p_len] = face.pts[0]; // Write the start onto the end.

    face.n = normalise(cross(face.pts[1] - face.pts[0], face.pts[2] - face.pts[0]));
    face.d = inner(face.n, face.pts[0]);
    face.mat_i = mat_i;

    // TODO: ASSERT all the points are on the same plane?

    return(face);
}

internal Face create_face(Memory *memory, V2 *p, Int p_len, V3 translation, V3 rotation, V3 sc, Int mat_i) {
    ASSERT(p_len >= 3);

    Face face = {};

    // Create the rotation matrix, used to create the points.
    M4x4 rotation_transform = iden();
    rotation_transform *= x_rotation(LANE_DEGREES_TO_RADIANS(rotation.x));
    rotation_transform *= y_rotation(LANE_DEGREES_TO_RADIANS(rotation.y));
    rotation_transform *= z_rotation(LANE_DEGREES_TO_RADIANS(rotation.z));

    face.p_len = p_len;
    face.pts = (V3 *)memory_push(memory, Memory_Index_permanent, sizeof(V3) * (p_len + 1));
    for(Int point_i = 0; (point_i < p_len); ++point_i) {
        face.pts[point_i] = rotation_transform * (v3(p[point_i].x, p[point_i].y, 0.0f) + translation);
    }
    face.pts[p_len] = face.pts[0]; // Write the start onto the end.

    face.n = normalise(cross(face.pts[1] - face.pts[0], face.pts[2] - face.pts[0]));
    face.d = inner(face.n, face.pts[0]);
    face.mat_i = mat_i;

    return(face);
}

// TODO: Faces should be rotated around a known point (top-left or origin).
internal Textured_Face create_textured_face(Memory *memory, V2 *p, Int p_len, V3 translation, V3 rotation, V3 sc, Int tex_i) {
    // TODO: Scale isn't supported right now.

    ASSERT(p_len >= 3);

    Textured_Face face = {};

    // Transform
    face.translation = translation;
    face.rotation = rotation;
    face.scale = sc;

    // Create the rotation matrix, used to create the points.
    M4x4 rotation_transform = iden();
    rotation_transform *= x_rotation(LANE_DEGREES_TO_RADIANS(rotation.x));
    rotation_transform *= y_rotation(LANE_DEGREES_TO_RADIANS(rotation.y));
    rotation_transform *= z_rotation(LANE_DEGREES_TO_RADIANS(rotation.z));

    // Add the points in global coords.
    face.p_len = p_len;
    face.pts = (V3 *)memory_push(memory, Memory_Index_permanent, sizeof(V3) * (p_len + 1));
    for(Int point_i = 0; (point_i < p_len); ++point_i) {
        face.pts[point_i] = rotation_transform * (v3(p[point_i].x, p[point_i].y, 0.0f) + face.translation);
    }
    face.pts[p_len] = face.pts[0]; // Set the last point to be the first again.

    // Calculate min/max bounds for the face. We calculate this in 2D then transform to guarantee the bounds are in the
    // same plane as the face.
    {
        V2 min = v2(LANE_FLT_MAX);
        V2 max = v2(LANE_FLT_MIN);
        for(Int point_i = 0; (point_i < p_len); ++point_i) {
            V2 translated_p = p[point_i] + translation.xy;

            // TODO: Should this be the top_left/bottom_right point or the total bounds?
            min.x = MIN(translated_p.x, min.x);
            min.y = MIN(translated_p.y, min.y);

            max.x = MAX(translated_p.x, max.x);
            max.y = MAX(translated_p.y, max.y);
        }

        ASSERT((min.x < LANE_FLT_MAX) && (min.y < LANE_FLT_MAX));
        ASSERT((max.x > LANE_FLT_MIN) && (max.y > LANE_FLT_MIN));

        // TODO: top_left and bottom_right are backwards :-(
        face.top_left     = rotation_transform * v3(max.x, max.y, 0.0f);
        face.bottom_right = rotation_transform * v3(min.x, min.y, 0.0f);
    }

#if INTERNAL
    {
        // Test we can un-transform the face and get the same results.
        F32 tolerance = 0.001f;
        for(Int point_i = 0; (point_i < face.p_len); ++point_i) {
            M4x4 inverse_trans = iden();
            inverse_trans *= z_rotation(LANE_DEGREES_TO_RADIANS(-face.rotation.z));
            inverse_trans *= y_rotation(LANE_DEGREES_TO_RADIANS(-face.rotation.y));
            inverse_trans *= x_rotation(LANE_DEGREES_TO_RADIANS(-face.rotation.x));

            V3 local_pt = (inverse_trans * face.pts[point_i]) - face.translation;
            V2 diff = v2(local_pt.x - p[point_i].x,
                         local_pt.y - p[point_i].y);
            ASSERT((diff.x     > -tolerance) && (diff.x     < tolerance));
            ASSERT((diff.y     > -tolerance) && (diff.y     < tolerance));
            ASSERT((local_pt.z > -tolerance) && (local_pt.z < tolerance));
        }
    }
#endif

    // Normal, distance-from-origin, and texture-index.
    face.n = normalise(cross(face.pts[1] - face.pts[0], face.pts[2] - face.pts[0]));
    face.d = inner(face.n, face.pts[0]);
    face.tex_i = tex_i;

    return(face);
}


/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
internal U64 xorshift64(U64 *a) {
    *a ^= *a << 13;
    *a ^= *a >> 7;
    *a ^= *a << 17;

    return(*a);
}

enum Type {
    Type_unknown,
    Type_plane,
    Type_sphere,
    Type_face,
    Type_textured_face,
    Type_count
};
struct Hit_Info {
    Lane_F32 distance;
    Lane_V3 normal;

    Lane_U32 mat_i;

    // Specific to textures
    Lane_U32 tex_i;
    Lane_U32 hit_texture_mask;

    Lane_V3 top_left;
    Lane_V3 bottom_right;

    Lane_V3 translation; // TODO: Unused
    Lane_V3 rotation;
    Lane_V3 scale; // TODO: Unused
};

internal Hit_Info hit_info(Void) {
    Hit_Info r = {};
    r.distance = lane_f32(LANE_FLT_MAX);
    r.normal = lane_v3_zero();

    r.mat_i = lane_u32(0);

    r.tex_i = lane_u32(0);
    r.hit_texture_mask = lane_u32(0);

    return(r);
}

internal Lane_V4 texture_gather(Texture *textures, Lane_U32 texture_indices, Lane_V3 hit_point, Hit_Info hit) {
    // TODO: We need to take the actual face orientation into account. So this is not correct...

    Lane_U32 mask = hit.hit_texture_mask;

    // Calculate the inverse transform.
    Lane_M4x4 inverse_trans = lane_iden();

    inverse_trans *= z_rotation(LANE_DEGREES_TO_RADIANS(-hit.rotation.z));
    inverse_trans *= y_rotation(LANE_DEGREES_TO_RADIANS(-hit.rotation.y));
    inverse_trans *= x_rotation(LANE_DEGREES_TO_RADIANS(-hit.rotation.x));

    // Get the unrotated face bounds, and move the point into the same space.
    Lane_V3 top_left        = (inverse_trans * hit.top_left);
    Lane_V3 bottom_right    = (inverse_trans * hit.bottom_right);
    Lane_V3 local_hit_point = (inverse_trans * hit_point);

#if INTERNAL
    // Assert we've actually rotated the face so the Z is 0(-ish).
    {
        // TODO: This ASSERT gets thrown sometimes (in O2 mode...) if I swap to another scene then swap back... :-(
        //F32 tolerance = 0.0001f;
        F32 tolerance = 0.01f;
        for(Int lane_i = 0; (lane_i < LANE_WIDTH); ++lane_i) {
            if(extract(mask, lane_i)) {
                ASSERT((extract(top_left.z,        lane_i) > -tolerance) && (extract(top_left.z,        lane_i) < tolerance));
                ASSERT((extract(bottom_right.z,    lane_i) > -tolerance) && (extract(bottom_right.z,    lane_i) < tolerance));
                ASSERT((extract(local_hit_point.z, lane_i) > -tolerance) && (extract(local_hit_point.z, lane_i) < tolerance));
            }
        }
    }
#endif

    // This is the pct x,y for where the ray hit the face. 0-1 scale. Clamp to 0-1 because the floating-point math sometimes comes
    // out as 1.0001, which gets used to index into the bitmap array (reading bogus memory).
    Lane_F32 pct_x = lane_clamp01(lerp(local_hit_point.x, top_left.x, bottom_right.x));
    Lane_F32 pct_y = lane_clamp01(lerp(local_hit_point.y, top_left.y, bottom_right.y));

    Lane_F32 image_width = lane_f32_from_u32(GATHER_U32(textures, texture_indices, image.width));
    Lane_F32 image_height = lane_f32_from_u32(GATHER_U32(textures, texture_indices, image.height));

    // The X/Y hit mapped onto the image.
    Lane_F32 hit_x = lane_f32_floor(image_width * pct_x);
    Lane_F32 hit_y = lane_f32_floor(image_height * pct_y);

    // Offset into the image for the pixel we want to access.
    Lane_U32 pixel_pointer_offset = lane_u32_from_f32((hit_y * image_width) + hit_x);

    Lane_V4 output_colour = lane_v4_zero();

    {
        V4 result_buffer[MAX_LANE_WIDTH] = {};
        for(Int lane_i = 0; (lane_i < LANE_WIDTH); ++lane_i) {
            // TODO: Removing this if might be possible, but I'd need to recalculate the hit_point inside this method. Right now
            //       we default to FLT_MAX for the distance and keep it at that if there's no hit (per lane). So if we remove the if we end
            //       up calculating the hit_point miles away from where it actually is, then out-of-bounds index into the image pixel array.
            if(extract(mask, lane_i)) {
                U32 texture_i = extract(texture_indices, lane_i);
                U32 *pixel_pointer = textures[texture_i].image.pixels + extract(pixel_pointer_offset, lane_i);
                U8 *current_pixel = (U8 *)pixel_pointer;

                F32 r = (F32)current_pixel[0] / 255.0f;
                F32 g = (F32)current_pixel[1] / 255.0f;
                F32 b = (F32)current_pixel[2] / 255.0f;
                F32 a = (F32)current_pixel[3] / 255.0f;

                F32 tolerance = 1.0f + 0.0001f;
                ASSERT((r <= tolerance) && (g <= tolerance) && (b <= tolerance) && (a <= tolerance));

                result_buffer[lane_i] = v4(b, g, r, a);
            }
        }

        // TODO: This is dumb...
        output_colour.r = lane_f32(result_buffer[0].r, result_buffer[1].r, result_buffer[2].r, result_buffer[3].r,
                                   result_buffer[4].r, result_buffer[5].r, result_buffer[6].r, result_buffer[7].r);
        output_colour.g = lane_f32(result_buffer[0].g, result_buffer[1].g, result_buffer[2].g, result_buffer[3].g,
                                   result_buffer[4].g, result_buffer[5].g, result_buffer[6].g, result_buffer[7].g);
        output_colour.b = lane_f32(result_buffer[0].b, result_buffer[1].b, result_buffer[2].b, result_buffer[3].b,
                                   result_buffer[4].b, result_buffer[5].b, result_buffer[6].b, result_buffer[7].b);
        output_colour.a = lane_f32(result_buffer[0].a, result_buffer[1].a, result_buffer[2].a, result_buffer[3].a,
                                   result_buffer[4].a, result_buffer[5].a, result_buffer[6].a, result_buffer[7].a);
    }

    return(output_colour);
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
        if     (image->width  > image->height) { film.h = film.w * ((F32)image->height / (F32)image->width ); }
        else if(image->height > image->width ) { film.w = film.h * ((F32)image->width  / (F32)image->height); }

        V2 half_film = v2(film.w * 0.5f,  film.h * 0.5f);

        V3 screen_centre = camera->p - (film_dist * camera->z);

        V2 half_pixel = v2(0.5f / (F32)image->width, 0.5f / (F32)image->height);

        U64 total_bounces_computed = 0;
        U64 total_loops_computed = 0;

        for(Int y = y_min; (y < one_past_y_max); ++y) {
            U32 *out = image->pixels + (y * image->width) + x_min;

            F32 screen_y = -1.0f + (2.0f * ((F32)y / (F32)image->height));
            for(Int x = x_min; (x < one_past_x_max); ++x) {
                F32 screen_x = -1.0f + (2.0f * ((F32)x / (F32)image->width));

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
                    offset.x = screen_x + random_bilateral(&entropy_series) * half_pixel.w;
                    offset.y = screen_y + random_bilateral(&entropy_series) * half_pixel.h;
#else
                    // Just grab the centre of the pixel
                    offset.x = lane_f32(screen_x + half_pixel.w);
                    offset.y = lane_f32(screen_y + half_pixel.h);
#endif

                    Lane_V3 screen_p = screen_centre + ((offset.x * half_film.w * camera_x) + (offset.y * half_film.h * camera_y));
                    Lane_V3 ro = camera_p;
                    Lane_V3 rd = normalise_or_zero(screen_p - camera_p);

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
                                        Lane_V3 v0 = lane_v3(face->pts[edge_i + 0]);
                                        Lane_V3 v1 = lane_v3(face->pts[edge_i + 1]);

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
                        // Textured Face
                        for(Int face_i = 0; (face_i < scene->textured_face_count); ++face_i) {
                            // TODO: Handle backfaces.

                            // For the initial test check if the ray is perpendicular to the face's normal.
                            // Ray                       -> r0+trd
                            // Plane from face's normal: -> Np+d=0
                            // Subsituting the Ray into "p" and solving for t we get:
                            //   -> t = -d - N*ro / N*rd
                            // If abs(t) > tolerance we're not perpendicular and have a chance to hit.

                            Textured_Face *face = &scene->textured_faces[face_i];

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
                                        Lane_V3 v0 = lane_v3(face->pts[edge_i + 0]);
                                        Lane_V3 v1 = lane_v3(face->pts[edge_i + 1]);

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
                                        Lane_U32 tex_i = lane_u32(face->tex_i);

                                        conditional_assign(hit_mask, &hit.distance, t);
                                        conditional_assign(hit_mask, &hit.mat_i, lane_u32(1)); // TODO: Hardcode for scene!
                                        conditional_assign(hit_mask, &hit.tex_i, tex_i);
                                        conditional_assign(hit_mask, &hit.normal, n);
                                        conditional_assign(hit_mask, &hit.hit_texture_mask, lane_u32(0xFFFFFFFF));

                                        conditional_assign(hit_mask, &hit.top_left, lane_v3(face->top_left));
                                        conditional_assign(hit_mask, &hit.bottom_right, lane_v3(face->bottom_right));

                                        conditional_assign(hit_mask, &hit.translation, lane_v3(face->translation));
                                        conditional_assign(hit_mask, &hit.rotation, lane_v3(face->rotation));
                                        conditional_assign(hit_mask, &hit.scale, lane_v3(face->scale));
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

                        // Gather material, then try to gather texture if nessessary. If this is a texture we initially gather material 0.
                        Lane_V4 mat_ref_colour = GATHER_V4(scene->materials, hit.mat_i, ref_colour);
                        if(!lane_mask_is_zeroed(hit.hit_texture_mask)) {
                            // This will potential be slow, so only do if nessessary.
                            Lane_V3 hit_point = ro + (hit.distance * rd);

#if 0
                            // TODO: This isn't correct.
                            Lane_F32 pct_x = lerp(hit_point.x, hit.top_left.x, hit.bottom_right.x);
                            Lane_F32 pct_y = lerp(hit_point.y, hit.top_left.y, hit.bottom_right.y);
                            Lane_F32 pct_z = lerp(hit_point.z, hit.top_left.z, hit.bottom_right.z);
                            conditional_assign(hit.hit_texture_mask, &mat_ref_colour, lane_v4(pct_y, pct_x, pct_z, lane_f32(1.0f)));
#else
                            Lane_V4 tex_ref_colour = texture_gather(scene->textures, hit.tex_i, hit_point, hit);
                            tex_ref_colour *= 2.0f; // TODO: Why * 2???
                            // TODO: We're assigning textures to emit and ref colour for now...
                            conditional_assign(hit.hit_texture_mask, &mat_emit_colour, tex_ref_colour.rgb);
                            conditional_assign(hit.hit_texture_mask, &mat_ref_colour, tex_ref_colour);
#endif

                        }

                        Lane_F32 light_power = lane_f32(1.0f); // TODO; How is this calculated?
                        sample += hadamard(attenuation, mat_emit_colour) * light_power;

                        lane_mask &= (hit.mat_i != 0);

                        if(!lane_mask_is_zeroed(lane_mask)) {
                            // Because we're going backwards we want the attenuation to increase with each bounce as it heads towards
                            // the emitter (either a light source or the sky).

                            total_hit_distance += hit.distance;

                            Lane_F32 denom = 4.0f * LANE_PI32 * (total_hit_distance * total_hit_distance);
                            // TODO: Test denom for 0?
                            Lane_F32 cos_attenuation = inner(-rd, hit.normal) / min(lane_f32(1.0f), denom);

                            attenuation = hadamard(attenuation, max(cos_attenuation, 0) * mat_ref_colour.rgb);

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

                total_bounces_computed += horizontal_add(bounces_computed);
                total_loops_computed += loops_computed;
                output_final_colour = horizontal_add(final_colour);

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
    ASSERT((scene) && (scene->material_count > 0));
    ASSERT(camera);
    ASSERT(image);
    ASSERT(core_count > 0);
    ASSERT(max_bounce_count > 0);
    //ASSERT(rays_per_pixel % LANE_WIDTH);;

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
