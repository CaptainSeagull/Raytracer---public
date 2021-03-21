#include "common_includes.cpp"

#include <windows.h>
#include <gl/gl.h>
#include "platform_win32.h"

// TODO: Go through this file and tidy it up.
// TODO: Controller support would be fun!
// TODO: Add the platform code as callbacks on the API struct?
// TODO: Maybe initially try to render with OpenGL and, if that fails, default to the GDI version

// TODO: Globals... :-(
internal_global BITMAPINFO global_bitmap_info;
internal_global API *global_api;
internal_global GLuint global_gl_blit_texture_handle;

#if DEBUG_WINDOW
    internal_global Win32_Debug_Window *global_debug_window;
#endif

internal File win32_read_file(Memory *memory, U32 memory_index_to_use, String fname, Bool null_terminate) {
    File res = {};

    // TODO: Use temp buffer here!
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

internal Bool win32_write_file(String fname, U8 *data, U64 size) {
    Bool res = false;

    ASSERT(fname.len < 1024);
    Char buf[1024] = {};
    memcpy(buf, fname.e, fname.len);

    HANDLE fhandle = CreateFileA(buf, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
    if(fhandle != INVALID_HANDLE_VALUE) {

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

        CloseHandle(fhandle);
    }

    return(res);
}

internal Int win32_get_processor_count(Void) {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    U32 res = info.dwNumberOfProcessors;

    return(res);
}

internal U64 win32_locked_add(U64 volatile *a, U64 b) {
    U64 r = InterlockedExchangeAdd64((S64 volatile *)a, b);
    return(r);
}

internal Void win32_update_window(HDC dc, RECT  *wnd_rect, Void *bitmap_memory, BITMAPINFO *bitmap_info,
                                  Int bitmap_width, Int bitmap_height) {
    Int wnd_w = wnd_rect->right - wnd_rect->left;
    Int wnd_h = wnd_rect->bottom - wnd_rect->top;

#if USE_OPENGL_WINDOW

    // Could be moved to platform-independent code.
    {
        glViewport(0, 0, wnd_w, wnd_h);

        glBindTexture(GL_TEXTURE_2D, global_gl_blit_texture_handle);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bitmap_width, bitmap_height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap_memory);

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

    SwapBuffers(dc);
#else
    // TODO: Bitblt may be faster than StretchDIBits
    StretchDIBits(dc,
                  0, 0, bitmap_width, bitmap_height,
                  0, 0, wnd_w, wnd_h,
                  bitmap_memory, bitmap_info, DIB_RGB_COLORS, SRCCOPY);
#endif
}

internal LRESULT CALLBACK win32_window_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param) {
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
                zero(global_api->bitmap_memory, FOUR_K);
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
            win32_update_window(dc, &cr, global_api->bitmap_memory, &global_bitmap_info, global_api->bitmap_width, global_api->bitmap_height);
            EndPaint(wnd, &ps);
        } break;

        default: {
            res = DefWindowProcA(wnd, msg, w_param, l_param);
        }
    }

    return(res);
};

#if DEBUG_WINDOW
internal LRESULT CALLBACK win32_debug_window_proc(HWND wnd, UINT msg, WPARAM w_param, LPARAM l_param) {
    LRESULT res = 0;
    switch(msg) {
        case WM_QUIT: case WM_CLOSE: { global_api->running = false; } break;

        case WM_SIZE: {
            RECT cr;
            GetClientRect(wnd, &cr);
            Int w = cr.right - cr.left;
            Int h = cr.bottom - cr.top;

            // May be faster to allocate new memory / free old memory. But works for now.
            if(global_debug_window->bitmap_memory) {
                zero(global_debug_window->bitmap_memory, FOUR_K);
            }

            global_debug_window->bitmap_info.bmiHeader.biSize = sizeof(global_debug_window->bitmap_info.bmiHeader);
            global_debug_window->bitmap_info.bmiHeader.biWidth = w;
            global_debug_window->bitmap_info.bmiHeader.biHeight = h;
            global_debug_window->bitmap_info.bmiHeader.biPlanes = 1;
            global_debug_window->bitmap_info.bmiHeader.biBitCount = 32;
            global_debug_window->bitmap_info.bmiHeader.biCompression = BI_RGB;
            global_debug_window->bitmap_info.bmiHeader.biSizeImage = 0;
            global_debug_window->bitmap_info.bmiHeader.biXPelsPerMeter = 0;
            global_debug_window->bitmap_info.bmiHeader.biYPelsPerMeter = 0;
            global_debug_window->bitmap_info.bmiHeader.biClrUsed = 0;
            global_debug_window->bitmap_info.bmiHeader.biClrImportant = 0;

            global_debug_window->bitmap_width = w;
            global_debug_window->bitmap_height = h;

            global_debug_window->image_size_change = true;
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(wnd, &ps);
            RECT cr;
            GetClientRect(wnd, &cr);
            //win32_update_window(dc, &cr);
            EndPaint(wnd, &ps);
        } break;

        default: {
            res = DefWindowProcA(wnd, msg, w_param, l_param);
        }
    }

    return(res);
};
#endif

internal Key win32_key_to_our_key(WPARAM k) {
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

internal F32 win32_get_seconds_elapsed(LARGE_INTEGER start, LARGE_INTEGER end, int64_t perf_cnt_freq) {
    F32 r = (F32)(end.QuadPart - start.QuadPart) / (F32)perf_cnt_freq;
    return(r);
}

internal LARGE_INTEGER win32_get_wall_clock(Void) {
    LARGE_INTEGER res = {};
    QueryPerformanceCounter(&res);

    return(res);
}

internal Void win32_get_window_dimension(HWND wnd, Int *w, Int *h) {
    RECT cr;
    GetClientRect(wnd, &cr);
    *w = (cr.right - cr.left);
    *h = (cr.bottom - cr.top);
}

#if DEBUG_WINDOW
internal Void win32_create_debug_window(Win32_Debug_Window *debug_window, HINSTANCE instance, Memory *memory) {
    global_debug_window = debug_window;

    WNDCLASS wnd_class = {};
    wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wnd_class.hInstance = instance;
    wnd_class.lpszClassName = TEXT("class2");
    wnd_class.lpfnWndProc = win32_debug_window_proc;

    if(RegisterClassA(&wnd_class)) {
        debug_window->width = 800;
        debug_window->height = 480;
        debug_window->hwnd = CreateWindowExA(0, wnd_class.lpszClassName, "Debug!",
                                             WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_POPUP, 900, 0,
                                             debug_window->width, debug_window->height, 0, 0, 0, 0);
        if(debug_window->hwnd && debug_window->hwnd != INVALID_HANDLE_VALUE) {
            debug_window->bitmap_memory = memory_push(memory, Memory_Index_debug_bitmap, FOUR_K);
            debug_window->success = true;
        }
    }
}
#endif

internal Void win32_init_opengl(HWND window) {
    HDC dc = GetDC(window);

    PIXELFORMATDESCRIPTOR desired_pfd = {};
    desired_pfd.nSize = sizeof(desired_pfd);
    desired_pfd.nVersion = 1;
    desired_pfd.iPixelType = PFD_TYPE_RGBA;
    desired_pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER; // TODO: PFD_DOUBLEBUFFER can have issues with streaming...
    desired_pfd.cColorBits = 32; // TODO: Or 24 or 32?
    desired_pfd.cAlphaBits = 8;
    desired_pfd.iLayerType = PFD_MAIN_PLANE;

    Int suggested_pfd_i = ChoosePixelFormat(dc, &desired_pfd);
    PIXELFORMATDESCRIPTOR suggested_pfd;
    DescribePixelFormat(dc, suggested_pfd_i, sizeof(suggested_pfd), &suggested_pfd);
    SetPixelFormat(dc, suggested_pfd_i, &suggested_pfd);

    HGLRC rc = wglCreateContext(dc);
    if(wglMakeCurrent(dc, rc)) {
        glGenTextures(1, &global_gl_blit_texture_handle);
    } else {
        ASSERT(0); // TODO: Something went wrong.
    }

    ReleaseDC(window, dc);
}

//
// Threading
internal Bool win32_add_work_queue_entry(API *api, Void *data, Void (*cb)(Void *data)) {
    Bool res = false;

    Win32_API *win32_api = (Win32_API *)api->platform_specific;
    Win32_Work_Queue *queue = &win32_api->queue;

    // % entry_count because it's a circular buffer!
    Int new_next_entry_to_write = (queue->next_entry_to_write + 1) % queue->entry_count;

    // This is a circular buffer, so these should never match!
    if(new_next_entry_to_write != queue->next_entry_to_read) {
        res = true;

        Win32_Work_Queue_Entry *entry = &queue->entries[queue->next_entry_to_write];
        entry->cb = cb;
        entry->data = data;

        ++queue->completion_goal;

        _WriteBarrier(); _mm_sfence();

        queue->next_entry_to_write = new_next_entry_to_write;
        ReleaseSemaphore(queue->semaphore, 1, 0);
    }

    return(res);
}

// Returns true if we actually did any work.
internal Bool win32_do_next_work_queue_entry(Win32_Work_Queue *queue) {
    Bool res = false;

    // Store next_entry_to_read and DON'T read it again in case the value's changed by other threads. We do a InterlockedCompareExchange
    // below to validate it's still correct before actually doing any work.
    Int original_next_entry_to_read = queue->next_entry_to_read;
    Int new_entry_to_read = (original_next_entry_to_read + 1) % queue->entry_count; // % entry_count because it's a circular buffer!

    // Check if we've caught-up to the write cursor.
    if(original_next_entry_to_read != queue->next_entry_to_write) {
        res = true;

        // Do a thread-safe exchange and see if we get back the original.
        Int idx = InterlockedCompareExchange((LONG volatile *)&queue->next_entry_to_read, new_entry_to_read, original_next_entry_to_read);
        if(idx == original_next_entry_to_read) {
            Win32_Work_Queue_Entry *entry = &queue->entries[idx];
            entry->cb(entry->data);

            InterlockedIncrement((LONG volatile *)&queue->completion_count);
        }
    }

    return(res);
}

internal Void win32_complete_all_work(API *api) {
    Win32_API *win32_api = (Win32_API *)api->platform_specific;
    Win32_Work_Queue *queue = &win32_api->queue;

    while(queue->completion_goal != queue->completion_count) {
        win32_do_next_work_queue_entry(queue);
    }

    queue->completion_goal = 0;
    queue->completion_count = 0;
}

internal DWORD WINAPI win32_thread_proc(Void *p) {
    Win32_Work_Queue *queue = (Win32_Work_Queue *)p;
    for(;;) {
        if(!win32_do_next_work_queue_entry(queue)) {
            WaitForSingleObjectEx(queue->semaphore, INFINITE, 0);
        }
    }

    //return(0);
}

//
// Code for reloading Raytracer dll
internal FILETIME win32_get_last_write_time(Char *fname) {
    FILETIME res = {};
    WIN32_FIND_DATA find_data = {};
    HANDLE find_handle = FindFirstFileA(fname, &find_data);
    if(find_handle != INVALID_HANDLE_VALUE) {
        res = find_data.ftLastWriteTime;
        FindClose(find_handle);
    }

    return(res);
}

internal Win32_Loaded_Code win32_load_code(Char *source_fname, Char *temp_fname) {
    Win32_Loaded_Code res = {};

    res.dll_last_write_time = win32_get_last_write_time(source_fname);
    CopyFile(source_fname, temp_fname, FALSE);

    res.dll = LoadLibraryA(temp_fname);
    ASSERT(res.dll);
    if(res.dll) {
        res.handle_input_and_render = (Handle_Input_And_Render *)GetProcAddress(res.dll, "handle_input_and_render");
        ASSERT(res.handle_input_and_render);
    }

    return(res);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    int res = 0xFF;

    Uintptr start_buffer_size = sizeof(Memory_Group) * 2; // * 2 is just for padding.

    // TODO: Tweek these sizes as-nessessary.
    U64 permanent_size = MEGABYTES(128);
    U64 temp_size = MEGABYTES(256);
    U64 world_object_size = MEGABYTES(64);
    U64 json_parse_size = MEGABYTES(64);
    U64 bitmap_size = FOUR_K + 8; // + 8 for alignment padding.
    U64 debug_bitmap_size = 0;
#if DEBUG_WINDOW
    debug_bitmap_size = (FOUR_K) + 8;
#endif
    U64 debug_output_size = 0;
#if INTERNAL
    debug_output_size = MEGABYTES(64);
#endif
    U64 total_size = start_buffer_size + permanent_size + temp_size + world_object_size + json_parse_size + bitmap_size +
                     debug_bitmap_size + debug_output_size;

    ASSERT((Memory_Index_permanent == 0) && (Memory_Index_temp == 1) && (Memory_Index_world_objects == 2) &&
           (Memory_Index_json_parse == 3) && (Memory_Index_bitmap == 4) && (Memory_Index_debug_bitmap == 5) &&
           (Memory_Index_debug_output == 6)); // Has to be guaranteed

    Void *all_memory = VirtualAlloc(0, total_size, MEM_COMMIT, PAGE_READWRITE);

    ASSERT(all_memory);
    if(all_memory) { // Not much we can do if this fails...
        Uintptr group_inputs[] = { permanent_size, temp_size, world_object_size, json_parse_size, bitmap_size,
                                   debug_bitmap_size, debug_output_size
                                 };
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
                ASSERT(0); // TODO: Did not have enough memory!
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
            api.bitmap_memory = memory_push(&memory, Memory_Index_bitmap, FOUR_K);
            ASSERT(api.bitmap_memory);
            global_api = &api;

            LARGE_INTEGER perf_cnt_freq_res;
            QueryPerformanceFrequency(&perf_cnt_freq_res);
            U64 perf_cnt_freq = perf_cnt_freq_res.QuadPart;

            WNDCLASS wnd_class = {};
            wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            wnd_class.hInstance = hInstance;
            wnd_class.lpszClassName = TEXT("Some text or something?");
            wnd_class.lpfnWndProc = win32_window_proc;

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
                Int target_wnd_width = 1920 / 3;
                Int target_wnd_height = 1080 / 3;
                HWND wnd = CreateWindowExA(0, wnd_class.lpszClassName, "Raytracer",
                                           WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
                                           target_wnd_width, target_wnd_height, 0, 0, hInstance, 0);

#if USE_OPENGL_WINDOW
                win32_init_opengl(wnd);
#endif

#if DEBUG_WINDOW
                Win32_Debug_Window debug_window = {};
                win32_create_debug_window(&debug_window, hInstance, &memory);
                ASSERT(debug_window.success);
#endif

                Int wnd_width = 0, wnd_height = 0;
                win32_get_window_dimension(wnd, &wnd_width, &wnd_height);

                if((wnd) && (wnd != INVALID_HANDLE_VALUE)) {
                    LARGE_INTEGER last_counter = win32_get_wall_clock();
                    LARGE_INTEGER flip_wall_clock = win32_get_wall_clock();

                    float ms_per_frame = 16.6666f;

                    // TODO: Instead of assuming we're running from the data directory and going relative from there could I find the path
                    // to the current EXE and load the DLLs from there?
                    Char *source_dll_full = "../build/Raytracer.dll";
                    Char *temp_dll_full = "../build/Raytracer_temp.dll";
                    Win32_Loaded_Code code = win32_load_code(source_dll_full, temp_dll_full);

                    Win32_API win32_api = {};
                    win32_api.queue.entry_count = 2048;
                    win32_api.queue.entries = (Win32_Work_Queue_Entry *)memory_push(&memory, Memory_Index_permanent,
                                                                                    sizeof(Win32_Work_Queue_Entry) * win32_api.queue.entry_count);
                    ASSERT(win32_api.queue.entries);
                    api.platform_specific = (Void *)&win32_api;

                    api.max_work_queue_count = win32_api.queue.entry_count;
                    api.init = true;
                    api.image_size_change = true;
                    api.running = true;
                    api.memory = &memory;
                    api.axis_scale = v3(1);
                    api.default_rays_per_pixel = 8;
                    api.current_scene_i = 1;

                    api.cb.read_file = win32_read_file;
                    api.cb.write_file = win32_write_file;
                    api.cb.locked_add = win32_locked_add;

                    api.cb.add_work_queue_entry = win32_add_work_queue_entry;
                    api.cb.complete_all_work = win32_complete_all_work;

                    api.randomish_seed = win32_get_wall_clock().QuadPart;
                    api.randomish_seed = xorshift64(&api.randomish_seed);

                    SYSTEM_INFO info = {};
                    GetSystemInfo(&info);
                    api.core_count = info.dwNumberOfProcessors;

                    for(Int i = 0; (i < api.core_count); ++i) {
                        HANDLE h = CreateThread(0, 0, win32_thread_proc, &win32_api.queue, 0, 0);
                        ASSERT(h && h != INVALID_HANDLE_VALUE);
                        CloseHandle(h);
                    }

                    Scene scene = {};

                    F32 seconds_elapsed_for_last_frame = 0;
                    while(api.running) {
                        // TODO: Do raytracer in separate thread and only have main thread process window messages?

                        // Unload the raytracer DLL then reload it in.
                        FILETIME new_write_time = win32_get_last_write_time(source_dll_full);
                        if(CompareFileTime(&new_write_time, &code.dll_last_write_time) != 0) {
                            if(code.dll) {
                                FreeLibrary(code.dll);

                                code.dll = 0;
                                code.handle_input_and_render = 0;
                            }

                            code = win32_load_code(source_dll_full, temp_dll_full);
                        }


                        for(Int i = 0; (i < ARRAY_COUNT(api.key)); ++i) {
                            api.previous_key[i] = api.key[i];
                        }

                        // Process pending messages
                        {
                            MSG msg;
                            while(PeekMessageA(&msg, wnd, 0, 0, PM_REMOVE)) {
                                switch(msg.message) {
                                    case WM_QUIT: case WM_CLOSE: { api.running = false; } break; // TODO: Does this have to be here and inside the windowproc?

                                    case WM_KEYDOWN: { api.key[win32_key_to_our_key(msg.wParam)] = 1.0f; } break;
                                    case WM_KEYUP:   { api.key[win32_key_to_our_key(msg.wParam)] = 0.0f; } break;

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

#if DEBUG_WINDOW
                            {
                                MSG _msg;
                                while(PeekMessageA(&_msg, debug_window.hwnd, 0, 0, PM_REMOVE)) {
                                    TranslateMessage(&_msg);
                                    DispatchMessageA(&_msg);
                                }
                            }
#endif
                        }

                        // Actual rendering
                        {
                            HDC dc = GetDC(wnd);

                            RECT cr;
                            GetClientRect(wnd, &cr);

                            {
                                POINT pt;
                                GetCursorPos(&pt);
                                ScreenToClient(wnd, &pt);

                                api.previous_mouse_pos = api.mouse_pos;

                                F32 pos_x = (F32)pt.x / (F32)wnd_width;
                                F32 pos_y = (F32)pt.y / (F32)wnd_height;

                                // Only store this if we're inside the window
                                if((pos_x >= 0 && pos_x <= 1) && (pos_y >= 0 && pos_y <= 1)) {
                                    api.mouse_pos.x = clamp01(pos_x);
                                    api.mouse_pos.y = clamp01(pos_y);
                                }
                            }

#if DEBUG_WINDOW
                            {
                                HDC debug_dc = GetDC(debug_window.hwnd);
                                // TODO: This part is a little flakey... why does it need to be called before handle_input_and_render?
                                LOGFONT lf = {};
                                lf.lfHeight = 20;
                                string_copy(&lf.lfFaceName[0], "Courier New");
                                HFONT font = CreateFontIndirect(&lf);
                                HGDIOBJ oldFont = SelectObject(debug_dc, font);
                                DeleteObject(font);
                                Char buf[256] = {};
                                stbsp_snprintf(buf, ARRAY_COUNT(buf), "MS %.3fms | Rays per pixel %d | Lane Width %d | Scene %d | %f %f",
                                               seconds_elapsed_for_last_frame, api.current_rays_per_pixel, LANE_WIDTH, api.current_scene_i,
                                               api.mouse_pos.x, api.mouse_pos.y);
                                TextOut(debug_dc, 10, 10, buf, string_length(buf));
                            }
#endif

                            api.window_width = cr.right - cr.left;;
                            api.window_height = cr.bottom - cr.top;;

                            code.handle_input_and_render(&api, &scene);
                            if(!api.init) { api.image_size_change = false; }
                            api.init = false;

                            win32_update_window(dc, &cr, api.bitmap_memory, &global_bitmap_info, api.bitmap_width, api.bitmap_height);

                            ReleaseDC(wnd, dc);
                        }

                        //SwapBuffers(GetDC(wnd)); // TODO: Is this nessessary with GDI or only OpenGL?

                        // Frame rate stuff, not that we ever hit a frame...
                        {
                            F32 seconds_elapsed_for_frame = win32_get_seconds_elapsed(last_counter,
                                                                                      win32_get_wall_clock(),
                                                                                      perf_cnt_freq);
                            if(seconds_elapsed_for_frame < target_seconds_per_frame) {
                                DWORD sleepms = (DWORD)(1000.0f * (target_seconds_per_frame - seconds_elapsed_for_frame));
                                if(sleepms > 0) {
                                    Sleep(sleepms);
                                }

                                F32 test_seconds_elapsed_for_frame = win32_get_seconds_elapsed(last_counter,
                                                                                               win32_get_wall_clock(),
                                                                                               perf_cnt_freq);
                                if(test_seconds_elapsed_for_frame < target_seconds_per_frame) {
                                    // TODO: Missed sleep
                                }

                                while(seconds_elapsed_for_frame < target_seconds_per_frame) {
                                    seconds_elapsed_for_frame = win32_get_seconds_elapsed(last_counter,
                                                                                          win32_get_wall_clock(),
                                                                                          perf_cnt_freq);
                                }
                            } else {
                                // TODO: Missed Frame Rate!
                            }

                            LARGE_INTEGER end_counter = win32_get_wall_clock();
                            ms_per_frame = 1000.0f * win32_get_seconds_elapsed(last_counter, end_counter, perf_cnt_freq);
                            last_counter = end_counter;

                            flip_wall_clock = win32_get_wall_clock();
                            seconds_elapsed_for_last_frame = seconds_elapsed_for_frame;
                        }

                    }
                }
            }
        }

        //VirtualFree(all_memory, 0, MEM_RELEASE); // Disabled for performance
    }

    return(res);
}
