internal Void init_scene_0(API *api, Scene *scene) {
    Memory *memory = api->memory;

    // Exported from SketchUp.

    // TODO: Has a bug when being reloaded using numkeys.
    File file = system_read_entire_file(memory, Memory_Index_temp, "test.json", true);
    if(file.size > 0) {
        String json_as_string = create_string(file.e, file.size);
        create_scene_from_json(memory, json_as_string, scene);
    }

    memory_pop(memory, file.e);

    scene->spheres[scene->sphere_count++] = create_sphere(v3(0, 0, -1), 0.1f, 1); // TODO: Add a test shere
    api->camera_rotation.x = 10; // TODO: Hardcoded!
}

internal Void init_scene_1(API *api, Scene *scene) {
    Memory *memory = api->memory;

    Material *materials = scene->materials;
    Plane *planes = scene->planes;
    Sphere *spheres = scene->spheres;
    Face *faces = scene->faces;

    Int mat_i = 0;
    materials[mat_i++] = create_material(v3( 0.3f, 0.4f, 0.5f), v4(0.00f, 0.00f, 0.00f, 1.0f), 0.00f); // 0
    materials[mat_i++] = create_material(v3( 0.0f, 0.0f, 0.0f), v4(0.5f,  0.50f, 0.50f, 1.0f), 0.00f); // 1
    materials[mat_i++] = create_material(v3( 0.0f, 0.0f, 0.0f), v4(0.70f, 0.50f, 0.30f, 1.0f), 0.00f); // 2
    materials[mat_i++] = create_material(v3(20.0f, 0.0f, 0.0f), v4(0.00f, 0.00f, 0.00f, 1.0f), 0.00f); // 3
    materials[mat_i++] = create_material(v3( 0.0f, 0.0f, 0.0f), v4(0.40f, 0.80f, 0.20f, 0.5f), 0.70f); // 4
    materials[mat_i++] = create_material(v3( 0.0f, 0.0f, 0.0f), v4(0.20f, 0.80f, 0.90f, 1.0f), 0.85f); // 5
    materials[mat_i++] = create_material(v3( 0.0f, 0.0f, 0.0f), v4(0.95f, 0.95f, 0.95f, 1.0f), 1.00f); // 6

    Int plane_i = 0;
    planes[plane_i++] = create_plane(v3(0, 0, 1), 0, 1);

    Int sphere_i = 0;
    spheres[sphere_i++] = create_sphere(v3( 0,  0, 0), 1.0f, 2);
    spheres[sphere_i++] = create_sphere(v3( 3, -2, 0), 1.0f, 3);
    spheres[sphere_i++] = create_sphere(v3(-2, -1, 2), 1.0f, 4);
    spheres[sphere_i++] = create_sphere(v3( 1, -2, 3), 1.0f, 5);
    spheres[sphere_i++] = create_sphere(v3(-2,  3, 0), 2.0f, 6);

    Int face_i = 0;
    {
        V3 p[4];
        p[0] = v3( 1, 1,  3);
        p[1] = v3(-1, 1,  1);
        p[2] = v3(-1, 2,  1);
        p[3] = v3( 1, 2,  3);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), 3);
    }

    api->camera_rotation.x = -65;

    api->pos = v3(0, -10, 5);

    scene->material_count = mat_i;
    scene->plane_count = plane_i;
    scene->sphere_count = sphere_i;
    scene->face_count = face_i;
}

internal Void init_scene_2(API *api, Scene *scene) {
    Memory *memory = api->memory;

    Material *materials = scene->materials;
    Plane *planes = scene->planes;
    Sphere *spheres = scene->spheres;
    Face *faces = scene->faces;

    Int mat_i = 0;
    materials[mat_i++] = create_material(v3( 0.3f,  0.4f, 0.5f), v4(0.00f, 0.00f, 0.00f, 1.00f), 0.00f); // 0
    materials[mat_i++] = create_material(v3( 0.0f,  0.0f, 0.0f), v4(0.5f,  0.50f, 0.50f, 1.00f), 0.00f); // 1
    materials[mat_i++] = create_material(v3( 0.0f,  0.0f, 0.0f), v4(0.5f,  0.50f, 0.50f, 1.00f), 1.00f); // 2
    materials[mat_i++] = create_material(v3(10.0f,  0.0f, 0.0f), v4(0.00f, 0.00f, 0.00f, 1.00f), 0.00f); // 3
    materials[mat_i++] = create_material(v3( 0.0f, 10.0f, 0.0f), v4(0.00f, 0.00f, 0.00f, 1.00f), 0.00f); // 4
    materials[mat_i++] = create_material(v3( 0.0f,  0.0f, 0.0f), v4(0.5f,  0.50f, 0.50f, 0.01f), 0.00f); // 5

    Int plane_i = 0;
    planes[plane_i++] = create_plane(v3(0, 0, 1), 0, 1);

    Int sphere_i = 0;
    spheres[sphere_i++] = create_sphere(v3( 1,  2, 0), 1.0f, 3);
    spheres[sphere_i++] = create_sphere(v3(-1,  0, 0), 1.0f, 4);

    Int face_i = 0;
    {
        V3 p[4];
        p[0] = v3( 1, 2,  3);
        p[1] = v3(-1, 2,  1);
        p[2] = v3(-1, 1,  1);
        p[3] = v3( 1, 1,  3);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), 5);
    } {
        V3 p[4];
        p[0] = v3( 1,  0,  3);
        p[1] = v3(-1,  0,  1);
        p[2] = v3(-1, -1,  1);
        p[3] = v3( 1, -1,  3);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), 1);
    } {
        V3 p[4];
        p[0] = v3(-1, -1,  3);
        p[1] = v3( 1, -1,  1);
        p[2] = v3( 1,  0,  1);
        p[3] = v3(-1,  0,  3);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), 1);
    }

    // Mirrors
    {
        V3 p[4];
        p[0] = v3(-7, -5,  6);
        p[1] = v3(-7, -5,  0);
        p[2] = v3( 7, -5,  0);
        p[3] = v3( 7, -5,  6);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), 2);
    } {
        V3 p[4];
        p[0] = v3( 7, 5,  6);
        p[1] = v3( 7, 5,  0);
        p[2] = v3(-7, 5,  0);
        p[3] = v3(-7, 5,  6);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), 2);
    }

    api->pos = v3(-20, -10, 5);
    api->camera_rotation.x = 290;
    api->camera_rotation.z = 60;

    scene->material_count = mat_i;
    scene->plane_count = plane_i;
    scene->sphere_count = sphere_i;
    scene->face_count = face_i;
}


internal Void init_scene_3(API *api, Scene *scene) {
    Memory *memory = api->memory;

    Material *materials = scene->materials;
    Plane *planes = scene->planes;
    Sphere *spheres = scene->spheres;
    Face *faces = scene->faces;

    Int mat_i = 0;
    materials[mat_i++] = create_material(v3( 0.3f,  0.4f, 0.5f), v4(0.00f, 0.00f, 0.00f, 1.00f), 0.00f); // 0
    materials[mat_i++] = create_material(v3( 0.0f,  0.0f, 0.0f), v4(0.5f,  0.50f, 0.50f, 1.00f), 0.00f); // 1
    materials[mat_i++] = create_material(v3( 0.0f,  0.0f, 0.0f), v4(0.5f,  0.50f, 0.50f, 1.00f), 1.00f); // 2
    materials[mat_i++] = create_material(v3(10.0f,  0.0f, 0.0f), v4(0.00f, 0.00f, 0.00f, 1.00f), 0.00f); // 3
    materials[mat_i++] = create_material(v3( 0.0f, 40.0f, 0.0f), v4(0.00f, 0.00f, 0.00f, 1.00f), 0.00f); // 4
    materials[mat_i++] = create_material(v3( 0.0f,  0.0f, 0.0f), v4(0.5f,  0.50f, 0.50f, 0.01f), 0.00f); // 5

    Int plane_i = 0;
    planes[plane_i++] = create_plane(v3(0, 0, 1), 0, 1);

    Int sphere_i = 0;
    spheres[sphere_i++] = create_sphere(v3(-11, 0, 0), 1.0f, 4);

    Int face_i = 0;
    {
        V3 p[4];
        p[0] = v3(-12, -1.0, 2);
        p[1] = v3(-12, -1.0, -1);
        p[2] = v3(-8,  -1.0, -1);
        p[3] = v3(-8,  -1.0, 2);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), 2);
    }
    {
        V3 p[4];
        p[0] = v3(-10,  1.0f, 2);
        p[1] = v3(-10,  1.0f, -1);
        p[2] = v3(-12, 1.0f, -1);
        p[3] = v3(-12, 1.0f, 2);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), 2);
    }
    {
        V3 p[4];
        p[0] = v3(12, -1.0f, 2);
        p[1] = v3(12, -1.0f, -1);
        p[2] = v3(12,  1.0f, -1);
        p[3] = v3(12,  1.0f, 2);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), 2);
    }
    {
        V3 p[4];
        p[0] = v3(-10,  -1, -2);
        p[1] = v3(-12, -1, -2);
        p[2] = v3(-12,  1, -2);
        p[3] = v3(-10,   1, -2);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), 2);
    }

    api->pos = v3(-20, -10, 5);
    api->camera_rotation.x = 290;
    api->camera_rotation.z = 60;

    scene->material_count = mat_i;
    scene->plane_count = plane_i;
    scene->sphere_count = sphere_i;
    scene->face_count = face_i;
}

//
// Main callback from platform.
internal Void handle_input_and_render(API *api, Scene *scene) {
    Memory *memory = api->memory;

    //
    // Scene swapping
    {
        Int scene_count = 4;
        for(Int i = 0; (i < scene_count); ++i) {
            if(api->key[i] && !api->previous_key[i]) {
                api->current_scene_i = i;
                api->init = true;
                memory_clear_entire_group(memory, Memory_Index_world_objects);
                break;
            }
        }
    }

    //
    // Init
    if(api->init) {

        Int max_material_count = 32;
        Int max_plane_count = 32;
        Int max_sphere_count = 32;
        Int max_face_count = 32;

        scene->materials = (Material *)memory_push(memory, Memory_Index_world_objects, sizeof(Material) * max_material_count);
        scene->planes = (Plane *)memory_push(memory, Memory_Index_world_objects, sizeof(Plane) * max_plane_count);
        scene->spheres = (Sphere *)memory_push(memory, Memory_Index_world_objects, sizeof(Sphere) * max_sphere_count);
        scene->faces = (Face *)memory_push(memory, Memory_Index_world_objects, sizeof(Face) * max_face_count);

        ASSERT((scene->materials) && (scene->planes) && (scene->spheres) && (scene->faces));

        Int scene_i = api->current_scene_i;

        switch(scene_i) {
            // TODO: Reloading scene 0 doesn't work... probably related to the fact it's parsing JSON with the world's worst
            //       JSON parser.

            case 0: { init_scene_0(api, scene); } break;
            case 1: { init_scene_1(api, scene); } break;
            case 2: { init_scene_2(api, scene); } break;
            case 3: { init_scene_3(api, scene); } break;

            default: { ASSERT(0); }
        }

        ASSERT((scene->material_count < max_material_count) &&
               (scene->plane_count < max_plane_count) &&
               (scene->sphere_count < max_sphere_count) &&
               (scene->face_count < max_face_count));

    } else {
        //
        // Main update

        Bool cam_changed = false;

        // Rotation/translation/scale of camera.
        F32 rot_speed = 1.0f;
        if     (api->key[key_up])    { cam_changed = true; api->camera_rotation.x -= rot_speed; }
        else if(api->key[key_down])  { cam_changed = true; api->camera_rotation.x += rot_speed; }
        if     (api->key[key_left])  { cam_changed = true; api->camera_rotation.z -= rot_speed; }
        else if(api->key[key_right]) { cam_changed = true; api->camera_rotation.z += rot_speed; }
        if     (api->key['P'])       { cam_changed = true; api->camera_rotation.y -= rot_speed; }
        else if(api->key['L'])       { cam_changed = true; api->camera_rotation.y += rot_speed; }

        V3 amount_to_move = v3(0);
        F32 movement_speed = 0.25;
        if     (api->key['W']) { cam_changed = true; amount_to_move.z -= movement_speed; }
        else if(api->key['S']) { cam_changed = true; amount_to_move.z += movement_speed; }
        if     (api->key['A']) { cam_changed = true; amount_to_move.x -= movement_speed; }
        else if(api->key['D']) { cam_changed = true; amount_to_move.x += movement_speed; }
        if     (api->key['Q']) { cam_changed = true; amount_to_move.y -= movement_speed; }
        else if(api->key['E']) { cam_changed = true; amount_to_move.y += movement_speed; }

        F32 scale_speed = 0.1f;
        if     (api->key['R']) { cam_changed = true; api->axis_scale.x += scale_speed; }
        else if(api->key['F']) { cam_changed = true; api->axis_scale.x -= scale_speed; }
        if     (api->key['T']) { cam_changed = true; api->axis_scale.y += scale_speed; }
        else if(api->key['G']) { cam_changed = true; api->axis_scale.y -= scale_speed; }
        if     (api->key['Y']) { cam_changed = true; api->axis_scale.z += scale_speed; }
        else if(api->key['H']) { cam_changed = true; api->axis_scale.z -= scale_speed; }

        Camera camera = create_camera(api->pos, amount_to_move, api->camera_rotation, api->axis_scale);
        api->pos = camera.p;

        Image image;
        image.width = api->window_width;
        image.height = api->window_height;
        image.pixels = (U32 *)api->bitmap_memory;

        Int max_bounce_count = 32;

        Bool rays_per_pixel_change = api->key[key_space] != api->previous_key[key_space];
        if(api->key[key_space]) {
            api->current_rays_per_pixel *= 2.0f;
            rays_per_pixel_change = true;
        } else {
            api->current_rays_per_pixel = api->default_rays_per_pixel;
        }

        // If the scene hasn't changed then we don't need to rerender the image.
        // TODO: Some issue with rendering text through GDI when skipping this. Only render text if INTERNAL is specified so just always
        //       rerender if that's defined.
        Bool require_rerender = (cam_changed) || (api->image_size_change) || (rays_per_pixel_change) || (INTERNAL);
        if(require_rerender) {
            render_scene(scene, &camera, &image, api->core_count, max_bounce_count, api->current_rays_per_pixel, api->randomish_seed);
        }

        // Save current bitmap is J is pressed.
        if(api->key['J'] && !api->previous_key['J']) {
            write_image_to_disk(memory, &image, "output/test.bmp");
        }
    }
}