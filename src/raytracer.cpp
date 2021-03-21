
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

//
// Implementation

struct Render_Tile_Data {
    Int min_x;
    Int min_y;

    Int one_past_max_x;
    Int one_past_max_y;

    Lane_U32 entropy;

    Int rays_per_pixel;
    Int max_bounce_count;
    Scene *scene;
    Camera *camera;
    Image *image;

    Platform_Callbacks *cb;

    U64 volatile bounces_computed;
    U64 volatile loops_computed;
};

internal Material create_material(V3 emit_colour, V4 ref_colour, F32 specular) {
    Material m;
    m.emit_colour = emit_colour;
    m.ref_colour = ref_colour;
    m.specular = specular;

    return(m);
}

internal Texture create_texture(V3 emit_colour, Image image, F32 specular) {
    ASSERT(image.pixels && image.width > 0 && image.height > 0);

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

internal Void render_tile(Void *d);

internal Void render_scene(API *api, Scene *scene, Camera *camera, Image *image,
                           Int core_count, Int max_bounce_count, Int rays_per_pixel, U64 random_seed) {
    ASSERT((scene) && (scene->material_count > 0));
    ASSERT(camera);
    ASSERT(image);
    ASSERT(core_count > 0);
    ASSERT(max_bounce_count > 0);
    //ASSERT(rays_per_pixel % LANE_WIDTH);

    //MessageBox(0, "", "", MB_OK);

    Memory *memory = api->memory;

    Int width = image->width;
    Int height = image->height;

    Int tile_width = width / core_count;
    Int tile_height = tile_width;

    Int tile_count_x = (width + tile_width - 1) / tile_width;
    Int tile_count_y = (height + tile_height - 1) / tile_height;
    U64 total_tile_count = (tile_count_x * tile_count_y);

    Render_Tile_Data *render_tile_data = (Render_Tile_Data *)memory_push(memory, Memory_Index_temp, sizeof(Render_Tile_Data) * api->max_work_queue_count);
    ASSERT(render_tile_data);

    Render_Tile_Data *at = render_tile_data;

    Int cur = 0;

    for(Int tile_y = 0; (tile_y < tile_count_y); ++tile_y) {
        Int min_y = tile_y * tile_height;
        Int one_past_max_y = MIN(min_y + tile_height, image->height);

        for(Int tile_x = 0; (tile_x < tile_count_x); ++tile_x) {
            Int min_x = tile_x * tile_width;
            Int one_past_max_x = MIN(min_x + tile_width, image->width);

            at->min_x = min_x;
            at->min_y = min_y;

            at->one_past_max_x = one_past_max_x;
            at->one_past_max_y = one_past_max_y;

            at->rays_per_pixel = rays_per_pixel;
            at->max_bounce_count = max_bounce_count;
            at->scene = scene;
            at->camera = camera;
            at->image = image;

            at->cb = &api->cb;

            at->entropy = lane_u32(xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                   xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                   xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                   xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                   xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                   xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                   xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed),
                                   xorshift64(&random_seed) + tile_x * xorshift64(&random_seed) + tile_y * xorshift64(&random_seed));

            Bool success = api->cb.add_work_queue_entry(api, at, render_tile);
            ASSERT(success);

            ++at; ++cur;
        }
    }

    api->cb.complete_all_work(api);

    memory_pop(memory, render_tile_data);
}
