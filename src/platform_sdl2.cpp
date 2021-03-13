// SDL2 version of the platform code.

internal File system_read_file(Memory *memory, U32 memory_index_to_use, String fname, Bool null_terminate/*= false*/) {
    File res = {};

    ASSERT(fname.len < 1024);
    Char buf[1024] = {};
    memcpy(buf, fname.e, fname.len);

    FILE *f = fopen(buf, "rb");
    if(f) {
        fseek(f, 0, SEEK_END);
        U64 size = ftell(f);
        fseek(f, 0, SEEK_SET);

        U32 fsize32 = safe_truncate_size_64(size);
        ASSERT(fsize32 == size);
        Void *file_memory = memory_push(memory, memory_index_to_use, (null_terminate) ? fsize32 + 1 : fsize32);

        fread(file_memory, size, 1, f);
        fclose(f);

        res.e = (Char *)file_memory;
        res.size = size;
        res.fname = fname;
    }

    return(res);
}

internal Bool system_write_file(String fname, U8 *data, U64 size) {
    // TODO: Implement
    ASSERT(0);
    return(false);
}

internal U64 system_locked_add(U64 volatile *a, U64 b) {
    // TODO: Implement properly
    Int r = *a;
    *a += b;
    return(r);
}

internal Void system_create_thread(Void *p) {
    ASSERT(0);
    // TODO: Implement
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

    Void *all_memory = malloc(total_size);
    //defer { free(all_memory); }; // TODO: Disabled for performance

    ASSERT(all_memory);
    if(all_memory) { // Not much we can do if this fails...
        Uintptr group_inputs[] = { permanent_size, temp_size, world_object_size, json_parse_size, bitmap_size };
        Memory memory = create_memory_base(all_memory, group_inputs, ARRAY_COUNT(group_inputs));

        Int window_width = 640;
        Int window_height = 480;

        // SDL init
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window *wnd = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
        SDL_Renderer *renderer = SDL_CreateRenderer(wnd, -1, 0);

        API api = {};
        api.bitmap_memory = memory_push(&memory, Memory_Index_bitmap, 4096 * 2160 * 4);
        ASSERT(api.bitmap_memory);

        api.init = true;
        api.image_size_change = true;
        api.running = true;
        api.memory = &memory;
        api.axis_scale = v3(1);
        api.default_rays_per_pixel = 8;
        api.current_scene_i = 4;
        api.randomish_seed = rand();
        api.window_width = window_width;
        api.window_height = window_height;
        api.core_count = 1; // TODO: Hardcoded.

        Scene scene = {};

        Bool quit = false;
        while(!quit) {
            SDL_Event event;
            SDL_WaitEvent(&event);

            switch (event.type) {
                case SDL_QUIT: { quit = true; } break;
            }

            handle_input_and_render(&api, &scene);

            if(!api.init) {
                SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(api.bitmap_memory, window_width, window_height, 32, 4 * window_width, 0, 0, 0, 0);
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
                defer {
                    SDL_DestroyTexture(texture);
                    SDL_FreeSurface(surface);
                };

                SDL_Rect dstrect = { 0, 0, window_width, window_height };
                SDL_RenderCopy(renderer, texture, 0, &dstrect);
                SDL_RenderPresent(renderer);

                api.image_size_change = false;
            } else {
                api.init = false;
            }
        }

        //SDL_DestroyRenderer(renderer);
        //SDL_DestroyWindow(wnd);
        //SDL_Quit();

        free(all_memory);
    }
    return(0);
}