
struct Win32_Work_Queue_Entry {
    Void (*cb)(Void *data);
    Void *data;
};

struct Win32_Work_Queue {
    U32 volatile completion_goal;
    U32 volatile completion_count;

    U32 volatile next_entry_to_write;
    U32 volatile next_entry_to_read;

    HANDLE semaphore;

    Win32_Work_Queue_Entry *entries;
    Int entry_count;
};

typedef Void Handle_Input_And_Render(API *, Scene *);
struct Win32_Loaded_Code {
    HMODULE dll;
    FILETIME dll_last_write_time;
    Handle_Input_And_Render *handle_input_and_render;
};

struct Win32_Debug_Window {
    Bool success;
    HWND hwnd;

    Int width, height;

    BITMAPINFO bitmap_info;
    Void *bitmap_memory;
    Int bitmap_width;
    Int bitmap_height;
    Int image_size_change;
    Bool image_size_changed;
};

struct Win32_API {
    Win32_Work_Queue queue;
};