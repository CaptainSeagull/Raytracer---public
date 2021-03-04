#include <windows.h> // Can we remove this?

// TODO: Globals... :-(
internal_global BITMAPINFO global_bitmap_info;
internal_global API *global_api;

internal File system_read_entire_file(Memory *memory, U32 memory_index_to_use, String fname, Bool null_terminate) {
    File res = {0};

    ASSERT(fname.len < 1024);
    Char buf[1024] = {};
    memcpy(buf, fname.e, fname.len);

    HANDLE fhandle = CreateFileA(buf, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fsize;
        if(GetFileSizeEx(fhandle, &fsize)) {
            DWORD fsize32 = safe_truncate_size_64(fsize.QuadPart);
            Void *file_memory = memory_push(memory, memory_index_to_use, (null_terminate) ? fsize32 + 1 : fsize32);

            DWORD bytes_read = 0;
            if(ReadFile(fhandle, file_memory, fsize32, &bytes_read, 0)) {
                if(bytes_read != fsize32) {
                    ASSERT(0);
                } else {
                    res.fname = fname; // TODO: Change to full path not relative.
                    res.size = fsize32;
                    res.e = (Char *)file_memory;
                    res.e[res.size] = 0;
                }
            }
        }

        CloseHandle(fhandle);
    }

    return(res);
}

internal Bool system_write_to_file(String fname, U8 *data, U64 size) {
    Bool res = false;

    ASSERT(fname.len < 1024);
    Char buf[1024] = {};
    memcpy(buf, fname.e, fname.len);

    HANDLE fhandle = CreateFileA(buf, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {
        defer { CloseHandle(fhandle); };

        DWORD fsize32;
#if ENVIRONMENT32
        fsize32 = file.size;
#else
        fsize32 = safe_truncate_size_64(size);
#endif
        DWORD bytes_written = 0;
        if(WriteFile(fhandle, data, fsize32, &bytes_written, 0)) {
            if(bytes_written != fsize32) {
                ASSERT(0);
            } else {
                res = true;
            }
        }

    }

    return(res);
}

internal Int system_get_processor_count(Void) {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    U32 res = info.dwNumberOfProcessors;

    return(res);
}

internal U64 system_locked_add(U64 volatile *a, U64 b) {
    U64 r = InterlockedExchangeAdd64((S64 volatile *)a, b);
    return(r);
}

internal DWORD WINAPI win32_internal_worker_thread(Void *p) {
    worker_thread_callback(p);
    return(0);
}

internal Void system_create_thread(Void *p) {
    DWORD thread_id;
    HANDLE h = CreateThread(0, 0, &win32_internal_worker_thread, p, 0, &thread_id);
    CloseHandle(h);
}

internal Void win32_internal_update_window(HDC dc, RECT  *wnd_rect) {
    Int wnd_w = wnd_rect->right - wnd_rect->left;
    Int wnd_h = wnd_rect->bottom - wnd_rect->top;

    // TODO: Bitblt may be faster than StretchDIBits
    StretchDIBits(dc,
                  0, 0, global_api->bitmap_width, global_api->bitmap_height,
                  0, 0, wnd_w, wnd_h,
                  global_api->bitmap_memory, &global_bitmap_info, DIB_RGB_COLORS, SRCCOPY);
}

internal LRESULT CALLBACK win32_internal_window_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param) {
    LRESULT res = 0;
    switch(msg) {
        case WM_QUIT: case WM_CLOSE: { global_api->running = false; } break;

        case WM_SIZE: {
            RECT cr;
            GetClientRect(wnd, &cr);
            Int w = cr.right - cr.left;
            Int h = cr.bottom - cr.top;

            // May be faster to allocate new memory / free old memory. But works for now.
            if(global_api->bitmap_memory) {
                zero(global_api->bitmap_memory, 4096 * 2160 * 4);
            }

            global_bitmap_info.bmiHeader.biSize = sizeof(global_bitmap_info.bmiHeader);
            global_bitmap_info.bmiHeader.biWidth = w;
            global_bitmap_info.bmiHeader.biHeight = h;
            global_bitmap_info.bmiHeader.biPlanes = 1;
            global_bitmap_info.bmiHeader.biBitCount = 32;
            global_bitmap_info.bmiHeader.biCompression = BI_RGB;
            global_bitmap_info.bmiHeader.biSizeImage = 0;
            global_bitmap_info.bmiHeader.biXPelsPerMeter = 0;
            global_bitmap_info.bmiHeader.biYPelsPerMeter = 0;
            global_bitmap_info.bmiHeader.biClrUsed = 0;
            global_bitmap_info.bmiHeader.biClrImportant = 0;

            global_api->bitmap_width = w;
            global_api->bitmap_height = h;

            global_api->image_size_change = true;
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(wnd, &ps);
            RECT cr;
            GetClientRect(wnd, &cr);
            win32_internal_update_window(dc, &cr);
            EndPaint(wnd, &ps);
        } break;

        default: {
            res = DefWindowProcA(wnd, msg, w_param, l_param);
        }
    }

    return(res);
};

internal Key win32_internal_win_key_to_our_key(WPARAM k) {
    Key res = key_unknown;
    switch(k) {
        case VK_CONTROL: { res = key_ctrl;   } break;
        case VK_SHIFT:   { res = key_shift;  } break;
        //case VK_ALT:     { res = key_alt;    } break;
        case VK_SPACE:   { res = key_space;  } break;
        case VK_ESCAPE:  { res = key_escape; } break;

        case VK_LEFT:    { res = key_left;  } break;
        case VK_RIGHT:   { res = key_right; } break;
        case VK_UP:      { res = key_up;    } break;
        case VK_DOWN:    { res = key_down;  } break;

        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
        case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z': {
            res = (Key)k;
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

internal F32 win32_internal_get_seconds_elapsed(LARGE_INTEGER start, LARGE_INTEGER end, int64_t perf_cnt_freq) {
    F32 r = (F32)(end.QuadPart - start.QuadPart) / (F32)perf_cnt_freq;
    return(r);
}

internal LARGE_INTEGER win32_internal_get_wall_clock(Void) {
    LARGE_INTEGER res = {};
    QueryPerformanceCounter(&res);

    return(res);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    Int res = 0xFF;

    Uintptr start_buffer_size = sizeof(Memory_Group) * 2; // * 2 is just for padding.

    // TODO: Tweek these sizes as-nessessary.
    U64 permanent_size = MEGABYTES(64);
    U64 temp_size = MEGABYTES(64);
    U64 world_object_size = MEGABYTES(64);
    U64 json_parse_size = MEGABYTES(64);
    U64 bitmap_size = (4096 * 2160 * 4) + 8; // 4K is max bitmap size! + 8 for alignment padding.
    U64 total_size = start_buffer_size + permanent_size + temp_size + world_object_size + json_parse_size + bitmap_size;

    ASSERT((Memory_Index_permanent == 0) && (Memory_Index_temp == 1) && (Memory_Index_world_objects == 2) &&
           (Memory_Index_json_parse == 3) && (Memory_Index_bitmap == 4)); // Has to be guaranteed

    Void *all_memory = VirtualAlloc(0, total_size, MEM_COMMIT, PAGE_READWRITE);
    //defer { VirtualFree(all_memory, 0, MEM_RELEASE); }; // TODO: Disabled for performance

    ASSERT(all_memory);
    if(all_memory) { // Not much we can do if this fails...
        Uintptr group_inputs[] = { permanent_size, temp_size, world_object_size, json_parse_size, bitmap_size };
        Memory memory = create_memory_base(all_memory, group_inputs, ARRAY_COUNT(group_inputs));

        Bool successfully_parsed_command_line = false;

        // Get the command line arguments.
        Char *cmdline = GetCommandLineA();
        Int args_len = string_length(cmdline);

        // Count number of arguments.
        Int original_cnt = 1;
        Bool in_quotes = false;
        for(U64 i = 0; (i < args_len); ++i) {
            if(cmdline[i] == '"') {
                in_quotes = !in_quotes;
            } else if(cmdline[i] == ' ') {
                if(!in_quotes) {
                    ++original_cnt;
                }
            }
        }

        // Create copy of args.
        Char *arg_cpy = (Char *)memory_push(&memory, Memory_Index_permanent, (sizeof(Char) * (args_len + 1)));
        if(arg_cpy) {
            string_copy(arg_cpy, cmdline);

            for(Int i = 0; (i < args_len); ++i) {
                if(arg_cpy[i] == '"') {
                    in_quotes = !in_quotes;
                } else if(arg_cpy[i] == ' ') {
                    if(!in_quotes) {
                        arg_cpy[i] = 0;
                    }
                }
            }

            // Setup pointers.
            in_quotes = false;
            U64 mem_size = original_cnt * 2;
            Int argc = 1;
            Char **argv = (Char **)memory_push(&memory, Memory_Index_permanent, (sizeof(Char *) * mem_size));
            if(!argv) {
                // TODO: Did not have enough memory!
            } else {
                Char **cur = argv;
                *cur++ = arg_cpy;
                for(Int i = 0; (i < args_len); ++i) {
                    if(!arg_cpy[i]) {
                        Char *str = arg_cpy + i + 1;
                        if(!string_contains(str, '*')) {
                            *cur = str;
                            ++cur;
                            ++argc;
                        } else {
                            WIN32_FIND_DATA find_data = {0};
                            HANDLE fhandle = FindFirstFile(str, &find_data);

                            if(fhandle != INVALID_HANDLE_VALUE) {
                                do {
                                    if(argc + 1 >= mem_size) {
                                        ASSERT(0); // TODO: Don't use malloc so can't realloc... just bail?
                                    }

                                    *cur = find_data.cFileName;
                                    ++cur;
                                    ++argc;
                                } while(FindNextFile(fhandle, &find_data));
                            }
                        }
                    }
                }

                successfully_parsed_command_line = true;
                res = 0; // 0 is success
            }
        }

        // _Real_ entry point for program...
        if(successfully_parsed_command_line) {
            API api = {};
            api.bitmap_memory = memory_push(&memory, Memory_Index_bitmap, 4096 * 2160 * 4);
            ASSERT(api.bitmap_memory);
            global_api = &api;

            LARGE_INTEGER perf_cnt_freq_res;
            QueryPerformanceFrequency(&perf_cnt_freq_res);
            U64 perf_cnt_freq = perf_cnt_freq_res.QuadPart;

            WNDCLASS wnd_class = {};
            wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            wnd_class.hInstance = hInstance;
            wnd_class.lpszClassName = TEXT("Some text for something?");
            wnd_class.lpfnWndProc = win32_internal_window_proc;

            // TODO: Is this part correct?
            Int frame_rate = 60;
            Int game_update_hz = frame_rate;
            F32 target_seconds_per_frame = 1.0f / (F32)game_update_hz;
            F32 target_ms_per_frame = 16.66f; //(1.0f / (F32)frame_rate) * 1000.0f;

            // To make the frame rate more granular.
            {
                HMODULE winmmdll = LoadLibraryA("winmm.dll");
                if(winmmdll) {
                    typedef MMRESULT TimeBeginPeriod_t(uint32_t uPeriod);
                    TimeBeginPeriod_t *winm_timeBeginPeriod = (TimeBeginPeriod_t *)GetProcAddress(winmmdll, "timeBeginPeriod");
                    if(winm_timeBeginPeriod) {
                        winm_timeBeginPeriod(1);
                    }

                    FreeLibrary(winmmdll);
                }
            }

            if(RegisterClassA(&wnd_class)) {
                Int win_width = 1920 / 2;
                Int win_height = 1080 / 2;
                HWND wnd = CreateWindowExA(0, wnd_class.lpszClassName, "Raytracer",
                                           WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
                                           win_width, win_height, 0, 0, hInstance, 0);

                if((wnd) && (wnd != INVALID_HANDLE_VALUE)) {
                    LARGE_INTEGER last_counter = win32_internal_get_wall_clock();
                    LARGE_INTEGER flip_wall_clock = win32_internal_get_wall_clock();

                    float ms_per_frame = 16.6666f;

                    api.init = true;
                    api.image_size_change = true;
                    api.running = true;
                    api.memory = &memory;
                    api.axis_scale = v3(1);
                    api.default_rays_per_pixel = 8;
                    api.current_scene_i = 1;

                    api.randomish_seed = win32_internal_get_wall_clock().QuadPart;
                    api.randomish_seed = xorshift64(&api.randomish_seed);

                    SYSTEM_INFO info = {};
                    GetSystemInfo(&info);
                    api.core_count = info.dwNumberOfProcessors;;

                    Scene scene = {};

                    F32 seconds_elapsed_for_last_frame = 0;
                    while(api.running) {
                        // TODO: Do raytracer in separate thread and only have main thread process window messages?

                        for(Int i = 0; (i < ARRAY_COUNT(api.key)); ++i) {
                            api.previous_key[i] = api.key[i];
                        }

                        // Process pending messages
                        {
                            MSG msg;
                            while(PeekMessageA(&msg, wnd, 0, 0, PM_REMOVE)) {
                                switch(msg.message) {
                                    case WM_QUIT: case WM_CLOSE: { api.running = false; } break; // TODO: Does this have to be here and inside the windowproc?

                                    case WM_KEYDOWN: { api.key[win32_internal_win_key_to_our_key(msg.wParam)] = 1.0f; } break;
                                    case WM_KEYUP:   { api.key[win32_internal_win_key_to_our_key(msg.wParam)] = 0.0f; } break;

                                    case WM_LBUTTONDOWN: { api.key[key_mouse_left]   = 1.0f; } break;
                                    case WM_MBUTTONDOWN: { api.key[key_mouse_middle] = 1.0f; } break;
                                    case WM_RBUTTONDOWN: { api.key[key_mouse_right]  = 1.0f; } break;

                                    case WM_LBUTTONUP: { api.key[key_mouse_left]   = 0.0f; } break;
                                    case WM_MBUTTONUP: { api.key[key_mouse_middle] = 0.0f; } break;
                                    case WM_RBUTTONUP: { api.key[key_mouse_right]  = 0.0f; } break;

                                    default: {
                                        TranslateMessage(&msg);
                                        DispatchMessageA(&msg);
                                    } break;
                                }
                            }
                        }

                        // Actual rendering
                        {
                            HDC dc = GetDC(wnd);
                            defer { ReleaseDC(wnd, dc); };

                            RECT cr;
                            GetClientRect(wnd, &cr);

#if INTERNAL
                            {
                                // TODO: This part is a little flakey... why does it need to be called before handle_input_and_render?
                                LOGFONT lf = {};
                                lf.lfHeight = 20;
                                string_copy(&lf.lfFaceName[0], "Courier New");
                                HFONT font = CreateFontIndirect(&lf);
                                defer { DeleteObject(font); };
                                HGDIOBJ oldFont = SelectObject(dc, font);
                                Char buf[256] = {};
                                stbsp_snprintf(buf, ARRAY_COUNT(buf), "MS %.2fms | Rays per pixel %d | Lane Width %d | Scene %d",
                                               seconds_elapsed_for_last_frame, api.current_rays_per_pixel, LANE_WIDTH, api.current_scene_i);
                                TextOut(dc, 10, 10, buf, string_length(buf));
                            }
#endif

                            api.window_width = cr.right - cr.left;;
                            api.window_height = cr.bottom - cr.top;;

                            handle_input_and_render(&api, &scene);
                            if(!api.init) {
                                api.image_size_change = false;
                            }
                            api.init = false;

                            win32_internal_update_window(dc, &cr);
                        }

                        //SwapBuffers(GetDC(wnd)); // TODO: Is this nessessary with GDI or only OpenGL?

                        // Frame rate stuff, not that we ever hit a frame...
                        {
                            F32 seconds_elapsed_for_frame = win32_internal_get_seconds_elapsed(last_counter,
                                                                                               win32_internal_get_wall_clock(),
                                                                                               perf_cnt_freq);
                            if(seconds_elapsed_for_frame < target_seconds_per_frame) {
                                DWORD sleepms = (DWORD)(1000.0f * (target_seconds_per_frame - seconds_elapsed_for_frame));
                                if(sleepms > 0) {
                                    Sleep(sleepms);
                                }

                                F32 test_seconds_elapsed_for_frame = win32_internal_get_seconds_elapsed(last_counter,
                                                                                                        win32_internal_get_wall_clock(),
                                                                                                        perf_cnt_freq);
                                if(test_seconds_elapsed_for_frame < target_seconds_per_frame) {
                                    // TODO: Missed sleep
                                }

                                while(seconds_elapsed_for_frame < target_seconds_per_frame) {
                                    seconds_elapsed_for_frame = win32_internal_get_seconds_elapsed(last_counter,
                                                                                                   win32_internal_get_wall_clock(),
                                                                                                   perf_cnt_freq);
                                }
                            } else {
                                // TODO: Missed Frame Rate!
                            }

                            LARGE_INTEGER end_counter = win32_internal_get_wall_clock();
                            ms_per_frame = 1000.0f * win32_internal_get_seconds_elapsed(last_counter, end_counter, perf_cnt_freq);
                            last_counter = end_counter;

                            flip_wall_clock = win32_internal_get_wall_clock();
                            seconds_elapsed_for_last_frame = seconds_elapsed_for_frame;
                        }

                    }
                }
            }
        }
    }

    return(res);
}
