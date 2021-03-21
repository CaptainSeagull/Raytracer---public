internal Void init_scene_0(API *api, Scene *scene) {
    Memory *memory = api->memory;

    // Exported from SketchUp.

    File file = api->cb.read_file(memory, Memory_Index_temp, "test.json", true);
    if(file.size > 0) {
        String json_as_string = create_string(file.e, file.size);
        create_scene_from_json(api, memory, json_as_string, scene);

        memory_pop(memory, file.e);
    }

    scene->spheres[scene->sphere_count++] = create_sphere(v3(0, 0, -1), 0.1f, 1); // TODO: Add a test shere
    api->camera_rotation.x = 10; // TODO: Hardcoded!
}

internal Void init_scene_1(API *api, Scene *scene) {
    Memory *memory = api->memory;

    Material *materials = scene->materials;
    Texture *textures = scene->textures;
    Plane *planes = scene->planes;
    Sphere *spheres = scene->spheres;
    Face *faces = scene->faces;
    Textured_Face *textured_faces = scene->textured_faces;

    Int mat_i = 0;
    materials[mat_i++] = create_material(v3( 0.3f, 0.4f, 0.5f), v4(0.00f, 0.00f, 0.00f, 1.0f), 0.00f); // 0
    materials[mat_i++] = create_material(v3( 0.0f, 0.0f, 0.0f), v4(0.5f,  0.50f, 0.50f, 1.0f), 0.00f); // 1
    materials[mat_i++] = create_material(v3( 0.0f, 0.0f, 0.0f), v4(0.70f, 0.50f, 0.30f, 1.0f), 0.00f); // 2
    materials[mat_i++] = create_material(v3(20.0f, 0.0f, 0.0f), v4(0.00f, 0.00f, 0.00f, 1.0f), 0.00f); // 3
    materials[mat_i++] = create_material(v3( 0.0f, 0.0f, 0.0f), v4(0.40f, 0.80f, 0.20f, 0.5f), 0.70f); // 4
    materials[mat_i++] = create_material(v3( 0.0f, 0.0f, 0.0f), v4(0.20f, 0.80f, 0.90f, 1.0f), 0.85f); // 5
    materials[mat_i++] = create_material(v3( 0.0f, 0.0f, 0.0f), v4(0.95f, 0.95f, 0.95f, 1.0f), 1.00f); // 6

    Int tex_i = 0;

    Int plane_i = 0;
    planes[plane_i++] = create_plane(v3(0, 0, 1), 0, 1);

    Int sphere_i = 0;
    spheres[sphere_i++] = create_sphere(v3( 0,  0, 0), 1.0f, 2);
    spheres[sphere_i++] = create_sphere(v3( 3, -2, 0), 1.0f, 3);
    spheres[sphere_i++] = create_sphere(v3(-2, -1, 2), 1.0f, 4);
    spheres[sphere_i++] = create_sphere(v3( 1, -2, 3), 1.0f, 5);
    spheres[sphere_i++] = create_sphere(v3(-2,  3, 0), 2.0f, 6);

    Int face_i = 0;

    Int textured_face_i = 0;

    api->camera_rotation.x = -65;

    api->pos = v3(0, -10, 5);

    scene->material_count = mat_i;
    scene->texture_count = tex_i;
    scene->plane_count = plane_i;
    scene->sphere_count = sphere_i;
    scene->face_count = face_i;
    scene->textured_face_count = textured_face_i;
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
        V2 p[4];
        p[0] = v2( 0,  0);
        p[1] = v2( 0, 20);
        p[2] = v2(10, 20);
        p[3] = v2(10,  0);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), v3(-9, -10, -20), v3(0, 90, 0), v3(1, 1, 1), 2);
    }

    {
        V2 p[4];
        p[0] = v2(10,  0);
        p[1] = v2(10, 20);
        p[2] = v2( 0, 20);
        p[3] = v2( 0,  0);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), v3(-9, -10, 20), v3(0, 90, 0), v3(1, 1, 1), 2);
    }

    api->pos = v3(-20, -5, 5);
    api->camera_rotation.x = 275;
    api->camera_rotation.z = 90;

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
    spheres[sphere_i++] = create_sphere(v3(0, 0, 0), 1.0f, 4);

    Int face_i = 0;

    {
        V2 p[4];
        p[0] = v2( 0,  0);
        p[1] = v2( 0, 20);
        p[2] = v2(10, 20);
        p[3] = v2(10,  0);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), v3(-9, -10, -20), v3(0, 90, 0), v3(1, 1, 1), 2);
    }

    {
        V2 p[4];
        p[0] = v2(0, 0);
        p[1] = v2(0, 2);
        p[2] = v2(2, 2);
        p[3] = v2(2, 0);

        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), v3(-1, -1, -1), v3(0, 0, 0), v3(1, 1, 1), 1);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), v3(1, -1, -1), v3(0, 0, 0), v3(1, 1, 1), 1);

        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), v3(-1, -1, -1), v3(90, 0, 0), v3(1, 1, 1), 1);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), v3( 1, -1, -1), v3(90, 0, 0), v3(1, 1, 1), 1);
    }
    {
        V2 p[4];
        p[0] = v2(2, 0);
        p[1] = v2(2, 2);
        p[2] = v2(0, 2);
        p[3] = v2(0, 0);

        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), v3(-1, -1, 1), v3(0, 90, 0), v3(1, 1, 1), 1);

        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), v3(-1, -1, 1), v3(90, 0, 0), v3(1, 1, 1), 1);
        faces[face_i++] = create_face(memory, p, ARRAY_COUNT(p), v3( 1, -1, 1), v3(90, 0, 0), v3(1, 1, 1), 1);
    }
    api->pos = v3(-20, -10, 5);
    api->camera_rotation.x = 290;
    api->camera_rotation.z = 60;

    scene->material_count = mat_i;
    scene->plane_count = plane_i;
    scene->sphere_count = sphere_i;
    scene->face_count = face_i;
}


internal Void init_scene_4(API *api, Scene *scene) {
    Memory *memory = api->memory;

    Material *materials = scene->materials;
    Texture *textures = scene->textures;
    Plane *planes = scene->planes;
    Sphere *spheres = scene->spheres;
    Face *faces = scene->faces;
    Textured_Face *textured_faces = scene->textured_faces;

    Int mat_i = 0;
    materials[mat_i++] = create_material(v3( 0.3f, 0.4f, 0.5f), v4(0.00f, 0.00f, 0.00f, 1.0f), 0.00f); // 0

    Int tex_i = 0;
    textures[tex_i++] = create_texture(v3(0), load_image(api, memory, "texture.bmp"), 1.0f);

    Int textured_face_i = 0;

    // TODO: Currently a bug if face bounds are not 1-1.
    // TODO: Translation param isn't working 100% correctly... (looks correct but ASSRRTs tell me it's slightly off)
    // TODO: Scaling doesn't work at all...

    {
        V2 p[4];
        p[0] = v2(1, 0);
        p[1] = v2(1, 1);
        p[2] = v2(0, 1);
        p[3] = v2(0, 0);

        // TODO: Having rotation/translation will currently cause an ASSERT!
        textured_faces[textured_face_i++] = create_textured_face(memory, p, ARRAY_COUNT(p), v3(1, 2, 0), v3(6, 45, 5), v3(1, 1, 1), 0);
    }

    {
        V2 p[4];
        p[0] = v2(2, 1);
        p[1] = v2(2, 2);
        p[2] = v2(1, 2);
        p[3] = v2(1, 1);

        // TODO: Having rotation/translation will currently cause an ASSERT!
        textured_faces[textured_face_i++] = create_textured_face(memory, p, ARRAY_COUNT(p), v3(0, 0, 0), v3(0, 45, 0), v3(1, 1, 1), 0);
    }

    {
        V2 p[4];
        p[0] = v2(1, 0);
        p[1] = v2(1, 1);
        p[2] = v2(0, 1);
        p[3] = v2(0, 0);

        // TODO: Having rotation/translation will currently cause an ASSERT!
        textured_faces[textured_face_i++] = create_textured_face(memory, p, ARRAY_COUNT(p), v3(2, 2, 0), v3(76, 45, 10), v3(1, 1, 1), 0);
    }

    {
        V2 p[4];
        p[0] = v2(1, 0);
        p[1] = v2(1, 1);
        p[2] = v2(0, 1);
        p[3] = v2(0, 0);

        textured_faces[textured_face_i++] = create_textured_face(memory, p, ARRAY_COUNT(p), v3(0, 0, 0), v3(0, 11, 76), v3(1, 1, 1), 0);
    }
    {
        V2 p[4];
        p[0] = v2(3, 0);
        p[1] = v2(3, 1);
        p[2] = v2(2, 1);
        p[3] = v2(2, 0);

        textured_faces[textured_face_i++] = create_textured_face(memory, p, ARRAY_COUNT(p), v3(0, 0, 0), v3(90, 0, 0), v3(1, 1, 1), 0);
    }

    {
        V2 p[4];
        p[0] = v2(1, 0);
        p[1] = v2(1, 1);
        p[2] = v2(0, 1);
        p[3] = v2(0, 0);

        textured_faces[textured_face_i++] = create_textured_face(memory, p, ARRAY_COUNT(p), v3(0, 0, 0), v3(0, 45, 0), v3(1, 1, 1), 0);
    }

    {
        V2 p[4];
        p[0] = v2(2, 0);
        p[1] = v2(2, 1);
        p[2] = v2(1, 1);
        p[3] = v2(1, 0);

        textured_faces[textured_face_i++] = create_textured_face(memory, p, ARRAY_COUNT(p), v3(0, 0, 0), v3(0, 0, 45), v3(1, 1, 1), 0);
    }

    {
        V2 p[4];
        p[0] = v2(5, 0);
        p[1] = v2(5, 1);
        p[2] = v2(4, 1);
        p[3] = v2(4, 0);

        textured_faces[textured_face_i++] = create_textured_face(memory, p, ARRAY_COUNT(p), v3(0, 0, 0), v3(0, 20, 45), v3(1, 1, 1), 0);
    }

    api->camera_rotation.x = -65;

    api->pos = v3(0, -10, 5);

    scene->material_count = mat_i;
    scene->texture_count = tex_i;
    scene->textured_face_count = textured_face_i;
}

internal Void debug_output_testing(API *api, Memory *memory, Debug_Node *root) {
    Debug_Node *a = add_child_to_node(memory, &root, Debug_Node_Type_header, "A");
    Debug_Node *b = add_child_to_node(memory, &root, Debug_Node_Type_header, "B");
    Debug_Node *c = add_child_to_node(memory, &root, Debug_Node_Type_header, "C");

    Debug_Node *b1 = add_child_to_node(memory, &b, Debug_Node_Type_internal, "B1");
    Debug_Node *b2 = add_child_to_node(memory, &b, Debug_Node_Type_internal, "B2");

    Debug_Node *b21 = add_child_to_node(memory, &b1, Debug_Node_Type_internal, "B21");
    Debug_Node *b22 = add_child_to_node(memory, &b1, Debug_Node_Type_internal, "B22");
    Debug_Node *b23 = add_child_to_node(memory, &b1, Debug_Node_Type_internal, "B23");
    Debug_Node *b24 = add_child_to_node(memory, &b1, Debug_Node_Type_internal, "B24");

    String string_to_output = print_node_to_string(memory, Memory_Index_debug_output, root);
    api->cb.write_file("debug_output.txt", (U8 *)string_to_output.e, string_to_output.len);
}

#if USE_OPENCL
internal Void opencl_test(Memory *memory) {
    Int vector_size = 1024;

    Char const *saxpy_kernel =
        "__kernel void saxpy_kernel(float alpha,\n"
        "                           __global float *a,\n"
        "                           __global float *b,\n"
        "                           __global float *c) {\n"
        "    int index = get_global_id(0);"
        "    c[index] = alpha * a[index] + b[index];"
        "}";

    F32 alpha = 2.0f;
    F32 *a = (F32 *)memory_push(memory, Memory_Index_temp, sizeof(F32) * vector_size);
    F32 *b = (F32 *)memory_push(memory, Memory_Index_temp, sizeof(F32) * vector_size);
    F32 *c = (F32 *)memory_push(memory, Memory_Index_temp, sizeof(F32) * vector_size);

    for(Int i = 0; (i < vector_size); ++i) {
        a[i] = i;
        b[i] = vector_size - i;
        c[i] = -1;
    }

    cl_int status = 0;

    // Get platform and device information
    cl_uint num_platforms;
    status = clGetPlatformIDs(0, 0, &num_platforms);
    cl_platform_id *platforms = (cl_platform_id *)memory_push(memory, Memory_Index_temp, sizeof(cl_platform_id) * num_platforms);
    status = clGetPlatformIDs(num_platforms, platforms, 0);

    // Get the devices list and choose the device to run on
    cl_uint num_devices;
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, 0, &num_devices);
    cl_device_id *device_list = (cl_device_id *)memory_push(memory, Memory_Index_temp, sizeof(cl_device_id) * num_devices);
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, num_devices, device_list, 0);

    // Create openCL context for each device
    cl_context context = clCreateContext(0, num_devices, device_list, 0, 0, &status);

    // Create command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_list[0], 0, &status);

    // Create memory buffers on the device
    cl_mem a_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY,  vector_size * sizeof(float), 0, &status);
    cl_mem b_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY,  vector_size * sizeof(float), 0, &status);
    cl_mem c_clmem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, vector_size * sizeof(float), 0, &status);

    // Copy buffer a and b to device
    status = clEnqueueWriteBuffer(command_queue, a_clmem, CL_TRUE, 0, vector_size * sizeof(float), a, 0, 0, 0);
    status = clEnqueueWriteBuffer(command_queue, b_clmem, CL_TRUE, 0, vector_size * sizeof(float), b, 0, 0, 0);

    // Create program from kernel source
    cl_program program = clCreateProgramWithSource(context, 1, &saxpy_kernel, 0, &status);

    // Build the program
    status = clBuildProgram(program, 1, device_list, 0, 0, 0);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "saxpy_kernel", &status);

    // Set the arguments of the kernel
    status = clSetKernelArg(kernel, 0, sizeof(float), (void *)&alpha);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&a_clmem);
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&b_clmem);
    status = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&c_clmem);

    // Execute the OpenCL kernel
    U64 global_size = vector_size;
    U64 local_size = 64;
    status = clEnqueueNDRangeKernel(command_queue, kernel, 1, 0, &global_size, &local_size, 0, 0, 0);

    // Read the cl memory back into c
    status = clEnqueueReadBuffer(command_queue, c_clmem, CL_TRUE, 0, vector_size * sizeof(F32), c, 0, 0, 0);

    // Wait for commands to complete
    status = clFlush(command_queue);
    status = clFinish(command_queue);

    // TODO: Release everything

    int k = 0;
}
#endif

//
// Main callback from platform.
extern "C" Void handle_input_and_render(API *api, Scene *scene) {

    Memory *memory = api->memory;
    ASSERT(memory->error == Memory_Arena_Error_success);

#if USE_OPENCL
    opencl_test(memory);
#endif
    // Debug testing
    {
#if DEBUG_WINDOW_FLAG
        Debug_Node root = create_node(Debug_Node_Type_root);
        debug_output_testing(memory, &root);
#endif
    }

    //
    // Scene swapping
    {
        Int scene_count = 5;
        for(Int i = 0; (i < scene_count); ++i) {
            if(api->key[i] && !api->previous_key[i]) {
                api->current_scene_i = i;
                api->init = true;
                zero(scene, sizeof(scene));
                memory_clear_entire_group(memory, Memory_Index_world_objects);
                break;
            }
        }
    }

    //
    // Init
    if(api->init) {

        Int max_material_count = 32;
        Int max_texture_count = 32;
        Int max_plane_count = 32;
        Int max_sphere_count = 32;
        Int max_face_count = 32;
        Int max_textured_face_count = 32;

        scene->materials = (Material *)memory_push(memory, Memory_Index_world_objects, sizeof(Material) * max_material_count);
        scene->textures = (Texture *)memory_push(memory, Memory_Index_world_objects, sizeof(Texture) * max_texture_count);
        scene->planes = (Plane *)memory_push(memory, Memory_Index_world_objects, sizeof(Plane) * max_plane_count);
        scene->spheres = (Sphere *)memory_push(memory, Memory_Index_world_objects, sizeof(Sphere) * max_sphere_count);
        scene->faces = (Face *)memory_push(memory, Memory_Index_world_objects, sizeof(Face) * max_face_count);
        scene->textured_faces = (Textured_Face *)memory_push(memory, Memory_Index_world_objects, sizeof(Textured_Face) * max_textured_face_count);

        ASSERT((scene->materials) && (scene->textures) && (scene->planes) && (scene->spheres) && (scene->faces) && (scene->textured_faces));

        Int scene_i = api->current_scene_i;

        // Reset these to defaults.
        api->pos = v3(0);
        api->camera_rotation = v3(0);
        api->axis_scale = v3(1);

        switch(scene_i) {
            // TODO: Reloading scene 0 doesn't work... probably related to the fact it's parsing JSON with the world's worst
            //       JSON parser.

            case 0: { init_scene_0(api, scene); } break;
            case 1: { init_scene_1(api, scene); } break;
            case 2: { init_scene_2(api, scene); } break;
            case 3: { init_scene_3(api, scene); } break;
            case 4: { init_scene_4(api, scene); } break;

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

        if(api->key[key_mouse_middle]) {
            F32 stabaliser = 1.0f;

            if(api->previous_mouse_pos.x > api->mouse_pos.x) {
                cam_changed = true;
                api->camera_rotation.z += (api->mouse_pos.x + api->previous_mouse_pos.x) * stabaliser;
            } else if(api->previous_mouse_pos.x < api->mouse_pos.x) {
                cam_changed = true;
                api->camera_rotation.z -= (api->mouse_pos.x + api->previous_mouse_pos.x) * stabaliser;
            }

            if(api->previous_mouse_pos.y > api->mouse_pos.y) {
                cam_changed = true;
                api->camera_rotation.x += (api->mouse_pos.y + api->previous_mouse_pos.y) * stabaliser;
            } else if(api->previous_mouse_pos.y < api->mouse_pos.y) {
                cam_changed = true;
                api->camera_rotation.x -= (api->mouse_pos.y + api->previous_mouse_pos.y) * stabaliser;
            }
        }

        V3 amount_to_move = v3(0);
        if(api->key[key_mouse_left]) {
            F32 stabaliser = 0.25f;

            if(api->previous_mouse_pos.x > api->mouse_pos.x) {
                cam_changed = true;
                amount_to_move.x += (api->mouse_pos.x + api->previous_mouse_pos.x) * stabaliser;
            } else if(api->previous_mouse_pos.x < api->mouse_pos.x) {
                cam_changed = true;
                amount_to_move.x -= (api->mouse_pos.x + api->previous_mouse_pos.x) * stabaliser;
            }

            if(api->previous_mouse_pos.y > api->mouse_pos.y) {
                cam_changed = true;
                amount_to_move.y -= (api->mouse_pos.y + api->previous_mouse_pos.y) * stabaliser;
            } else if(api->previous_mouse_pos.y < api->mouse_pos.y) {
                cam_changed = true;
                amount_to_move.y += (api->mouse_pos.y + api->previous_mouse_pos.y) * stabaliser;
            }
        }

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
            render_scene(api, scene, &camera, &image, api->core_count, max_bounce_count, api->current_rays_per_pixel, api->randomish_seed);
        }

        // Save current bitmap is J is pressed.
        if(api->key['J'] && !api->previous_key['J']) {
            write_image_to_disk(api, memory, &image, "output/test.bmp");
        }
    }
}
