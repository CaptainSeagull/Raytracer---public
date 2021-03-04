struct File {
    Char *e;
    Uintptr size;
    String fname;
};

enum Key : Int {
    key_unknown = -1,

    // 0-9 are matched by index,

    key_ctrl = 11,
    key_shift,
    key_alt,
    key_space,
    key_escape,

    key_left,
    key_right,
    key_up,
    key_down,

    key_mouse_left,
    key_mouse_middle,
    key_mouse_right,

    left_stick_x,
    left_stick_y,

    right_stick_x,
    right_stick_y,

    dpad_right,
    dpad_up,
    dpad_down,
    dpad_left,

    start,
    back,

    left_shoulder,
    right_shoulder,
    left_thumb,
    right_thumb,

    controller_a,
    controller_b,
    controller_x,
    controller_y,

    key_a = 65, // 'A'
    key_b,
    key_c,
    key_d,
    key_e,
    key_f,
    key_g,
    key_h,
    key_i,
    key_j,
    key_k,
    key_l,
    key_m,
    key_n,
    key_o,
    key_p,
    key_q,
    key_r,
    key_s,
    key_t,
    key_u,
    key_v,
    key_w,
    key_x,
    key_y,
    key_z,

    key_cnt = 128
};

struct API {
    F32 previous_key[256];
    F32 key[256];

    Memory *memory;
    Bool init;
    Bool running;
    F32 dt;
    F32 seconds_elapsed_for_last_frame;

    Int window_width;
    Int window_height;
    Int core_count;

    Int bitmap_width;
    Int bitmap_height;
    Void *bitmap_memory;

    // TODO: Instead of adding stuff here, just add a void * to separate out the platform/raytracer code more.

    Bool image_size_change;

    U64 randomish_seed;

    V3 pos;
    V3 camera_rotation;
    V3 axis_scale;

    Int current_scene_i;
    Int current_rays_per_pixel;
    Int default_rays_per_pixel;
};

// Platform services
internal File system_read_entire_file(Memory *memory, U32 memory_index_to_use, String fname, Bool null_terminate = false);
internal Bool system_write_to_file(String fname, U8 *data, U64 size);
internal U64 system_locked_add(U64 volatile *a, U64 b);
internal Void system_create_thread(Void *p);

// Callbacks
internal Void worker_thread_callback(Void *p);
internal Void handle_input_and_render(API *api, struct Scene *scene);