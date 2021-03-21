// Linux platform code

// TODO: Need to handle window resizing properly

internal File system_read_file(Memory *memory, U32 memory_index_to_use, String fname, Bool null_terminate/*= false*/) {
    File res = {};

    // TODO: Use temp buffer here!
    ASSERT(fname.len < 1024);
    Char buf[1024] = {};
    memcpy(buf, fname.e, fname.len);

    int file_desc = open(buf, O_RDONLY);
    if(file_desc != -1) {
        U64 size64 = lseek(file_desc, 0, SEEK_END);
        U32 size32 = (U64)size64;
        ASSERT(size64 == size32);
        lseek(file_desc, 0, SEEK_SET);

        Void *file_memory = memory_push(memory, memory_index_to_use, (null_terminate) ? size32 + 1 : size32);
        read(file_desc, file_memory, size32);

        if(file_memory) {
            res.fname = fname;
            res.size = size32;
            res.e = (Char *)file_memory;

        }
    }

    return(res);
}

internal Bool system_write_file(String fname, U8 *data, U64 size) {
    // TODO: Implement
    ASSERT(0);
    return(false);
}

internal U64 system_locked_add(U64 volatile *a, U64 b) {
    int r = __sync_fetch_and_add(a, b);
    return(r);
}

internal Void *linux_worker_thread(void *p) {
    worker_thread_callback(p);
    return(0);
}

internal Void system_create_thread(Void *p) {
    pthread_t thread;
    pthread_create(&thread, 0, linux_worker_thread, p);
    // TODO: Close thread handle like Windows?
}

internal F32 linux_get_seconds_elapsed(U64 perf_count_frequency, U64 start, U64 end) {
    F32 r = ((F32)(end - start) / (F32)perf_count_frequency);
    return(r);
}

internal Void linux_sleep(U32 ms) {
    struct timespec elapsed, tv;

    elapsed.tv_sec = ms / 1000;
    elapsed.tv_nsec = (ms % 1000) * 1000000;
    tv.tv_sec = elapsed.tv_sec;
    tv.tv_nsec = elapsed.tv_nsec;
    int err = nanosleep(&tv, &elapsed);
    // TODO - Check error
}

internal U64 linux_get_processor_timestamp(Void) {
    U64 res = __rdtsc();
    return(res);
}

internal Key linux_key_to_our_key(KeySym key) {
    Key res = key_unknown;
    switch(key) {
        case XK_Up:    { res = key_up;    } break;
        case XK_Down:  { res = key_down;  } break;
        case XK_Left:  { res = key_left;  } break;
        case XK_Right: { res = key_right; } break;

        case XK_space:  { res = key_space;  } break;
        case XK_Escape: { res = key_escape; } break;

        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': {
            res = (Key)((Int)key + ('A' - 'a'));  // To upper
        } break;

        case '0': { res = (Key)0; } break;
        case '1': { res = (Key)1; } break;
        case '2': { res = (Key)2; } break;
        case '3': { res = (Key)3; } break;
        case '4': { res = (Key)4; } break;
        case '5': { res = (Key)5; } break;
        case '6': { res = (Key)6; } break;
        case '7': { res = (Key)7; } break;
        case '8': { res = (Key)8; } break;
        case '9': { res = (Key)9; } break;
    }

    return(res);
}

int main(int argc, char **argv) {

    // TODO: Tweek these sizes as-nessessary.
    Uintptr start_buffer_size = sizeof(Memory_Group) * 2; // * 2 is just for padding.

    U64 permanent_size = MEGABYTES(64);
    U64 temp_size = MEGABYTES(64);
    U64 world_object_size = MEGABYTES(64);
    U64 json_parse_size = MEGABYTES(64);
    U64 bitmap_size = (4096 * 2160 * 4) + 8; // 4K is max bitmap size! + 8 for alignment padding.
    U64 total_size = start_buffer_size + permanent_size + temp_size + world_object_size + json_parse_size + bitmap_size;

    ASSERT((Memory_Index_permanent == 0) && (Memory_Index_temp == 1) && (Memory_Index_world_objects == 2) &&
           (Memory_Index_json_parse == 3) && (Memory_Index_bitmap == 4)); // Has to be guaranteed

    Void *all_memory = mmap(0, total_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    ASSERT(all_memory);
    if(all_memory) { // Not much we can do if this fails...
        Uintptr group_inputs[] = { permanent_size, temp_size, world_object_size, json_parse_size, bitmap_size };
        Memory memory = create_memory_base(all_memory, group_inputs, ARRAY_COUNT(group_inputs));

        API api = {};
        api.bitmap_memory = memory_push(&memory, Memory_Index_bitmap, FOUR_K);
        ASSERT(api.bitmap_memory);

        api.init = true;
        api.image_size_change = true;
        api.running = true;
        api.memory = &memory;
        api.axis_scale = v3(1);
        api.default_rays_per_pixel = 8;
        api.current_scene_i = 4;

        api.randomish_seed = linux_get_processor_timestamp();
        api.randomish_seed = xorshift64(&api.randomish_seed);

        api.core_count = get_nprocs();

        F32 frame_rate = 16.6f;

        api.window_width = 640;
        api.window_height = 480;

        api.bitmap_width = api.window_width;
        api.bitmap_height = api.window_height;

        Display *display = XOpenDisplay(0);
        if(display) {
            GLint attrib_list[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
            XVisualInfo *visual_info = glXChooseVisual(display, 0, attrib_list);
            if(visual_info) {
                Window root_window = DefaultRootWindow(display);
                Colormap colour_map = XCreateColormap(display, root_window, visual_info->visual, AllocNone);

                XSetWindowAttributes set_window_attributes = {};
                set_window_attributes.colormap = colour_map;
                set_window_attributes.event_mask = ExposureMask | KeyPressMask;

                Window win = XCreateWindow(display, root_window, 0, 0, api.window_width, api.window_height, 0,
                                           visual_info->depth, InputOutput, visual_info->visual,
                                           CWColormap | CWEventMask, &set_window_attributes);

                if(win) {
                    XMapWindow(display, win);
                    XStoreName(display, win, "Raytracer");

                    GLXContext gl_context = glXCreateContext(display, visual_info, 0, GL_TRUE);
                    XSelectInput(display, win, KeyPressMask | KeyReleaseMask);
                    if(gl_context) {
                        glXMakeCurrent(display, win, gl_context);

                        F32 game_update_hz = (F32)frame_rate;
                        U64 last_counter = linux_get_processor_timestamp();
                        U64 flip_wall_clock = linux_get_processor_timestamp();
                        F32 target_seconds_per_frame = 1.0f / (F32)game_update_hz;
                        F32 ms_per_frame = 0.0f;

                        GLuint blit_texture_handle;
                        glGenTextures(1, &blit_texture_handle);

                        Scene scene = {};

                        U64 perf_count_frequency = 1000000000;

                        api.running = true;
                        while(api.running) {
                            for(Int i = 0; (i < ARRAY_COUNT(api.key)); ++i) {
                                api.previous_key[i] = api.key[i];
                            }

                            F32 dt = ms_per_frame;

                            while(XPending(display)) {
                                XEvent event;
                                XNextEvent(display, &event);

                                if(KeyPress == event.xkey.type) {
                                    KeySym key = XKeycodeToKeysym(display, event.xkey.keycode, 0); // TODO: Deprecated method
                                    api.key[linux_key_to_our_key(key)] = 1.0f;
                                } else if(KeyRelease == event.xkey.type) {
                                    KeySym key = XKeycodeToKeysym(display, event.xkey.keycode, 0); // TODO: Deprecated metho
                                    api.key[linux_key_to_our_key(key)] = 0.0f;
                                }
                            }

                            //XWindowAttributes win_attribs = {};
                            //XGetWindowAttributes(display, win, &win_attribs);

                            handle_input_and_render(&api, &scene);
                            if(!api.init) { api.image_size_change = false; }
                            api.init = false;

                            {

                                // Could be moved to platform-independent code.
                                {
                                    Int bitmap_width = api.bitmap_width;
                                    Int bitmap_height = api.bitmap_height;
                                    Void *bitmap_memory = api.bitmap_memory;
                                    Int window_width = api.window_width;
                                    Int window_height = api.window_height;

                                    glViewport(0, 0, window_width, window_height);

                                    glBindTexture(GL_TEXTURE_2D, blit_texture_handle);

                                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bitmap_width, bitmap_height, 0,
                                                 GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap_memory);

                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP);
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP);

                                    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

                                    glEnable(GL_TEXTURE_2D); // TODO: Move above glBindTexture?
#if INTERNAL
                                    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
#else
                                    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
#endif
                                    glClear(GL_COLOR_BUFFER_BIT);

                                    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

                                    glMatrixMode(GL_PROJECTION);
                                    M4x4 proj = m4x4(safe_div(2.0f, bitmap_width), 0,                             0, -1,
                                                     0,                            safe_div(2.0f, bitmap_height), 0, -1,
                                                     0,                            0,                             1,  0,
                                                     0,                            0,                             0,  1);
                                    glLoadMatrixf(transpose(proj).f);

                                    glMatrixMode(GL_TEXTURE); glLoadIdentity();


                                    glBegin(GL_TRIANGLES);

                                    // TODO: How to handle aspect ratio - black bars or crop?

                                    V2 min = v2(0, 0);
                                    V2 max = v2(bitmap_width, bitmap_height);

                                    V4 colour = v4(1, 1, 1, 1);
                                    glColor4f(colour.r, colour.g, colour.g, colour.a);

                                    glTexCoord2f(0, 0); glVertex2f(min.x, min.y);
                                    glTexCoord2f(1, 0); glVertex2f(max.x, min.y);
                                    glTexCoord2f(1, 1); glVertex2f(max.x, max.y);

                                    glTexCoord2f(0, 0); glVertex2f(min.x, min.y);
                                    glTexCoord2f(1, 1); glVertex2f(max.x, max.y);
                                    glTexCoord2f(0, 1); glVertex2f(min.x, max.y);
                                    glEnd();
                                }
                            }
                            //api.init = false;

                            glXSwapBuffers(display, win);

                            F32 seconds_elapsed_for_frame = linux_get_seconds_elapsed(perf_count_frequency,
                                                                                      last_counter,
                                                                                      linux_get_processor_timestamp());
                            if(seconds_elapsed_for_frame < target_seconds_per_frame) {
                                F32 test_seconds_elapsed_for_frame;
                                U32 sleepms = (U32)(1000.0f * (target_seconds_per_frame - seconds_elapsed_for_frame));
                                if(sleepms > 0) {
                                    linux_sleep(sleepms);
                                }

                                test_seconds_elapsed_for_frame = linux_get_seconds_elapsed(perf_count_frequency,
                                                                                           last_counter,
                                                                                           linux_get_processor_timestamp());
                                if(test_seconds_elapsed_for_frame < target_seconds_per_frame) {
                                    // TODO(Jonny): Log missed Sleep here
                                }

                                while(seconds_elapsed_for_frame < target_seconds_per_frame) {
                                    seconds_elapsed_for_frame = linux_get_seconds_elapsed(perf_count_frequency,
                                                                                          last_counter,
                                                                                          linux_get_processor_timestamp());
                                }
                            } else {
                                // Missed Frame Rate!
                            }

                            U64 end_counter = linux_get_processor_timestamp();
                            ms_per_frame = 1000.0f * linux_get_seconds_elapsed(perf_count_frequency,
                                                                               last_counter, end_counter);
                            last_counter = end_counter;

                            flip_wall_clock = linux_get_processor_timestamp();
                        }
                    }
                }
            }
        }

        //munmap(all_memory, total_size);
    }
}