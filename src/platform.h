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

    key_A = 65, // 'A'
    key_B,
    key_C,
    key_D,
    key_E,
    key_F,
    key_G,
    key_H,
    key_I,
    key_J,
    key_K,
    key_L,
    key_M,
    key_N,
    key_O,
    key_P,
    key_Q,
    key_R,
    key_S,
    key_T,
    key_U,
    key_V,
    key_W,
    key_X,
    key_Y,
    key_Z,

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

    V2 previous_mouse_pos;
    V2 mouse_pos;

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
internal File system_read_file(Memory *memory, U32 memory_index_to_use, String fname, Bool null_terminate = false);
internal Bool system_write_file(String fname, U8 *data, U64 size);
internal U64 system_locked_add(U64 volatile *a, U64 b);
internal Void system_create_thread(Void *p);

// Callbacks
internal Void worker_thread_callback(Void *p);
internal Void handle_input_and_render(API *api, struct Scene *scene);