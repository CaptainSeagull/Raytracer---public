//
// Crappy bitmap loader... write bitmaps in BMP Version 3 (Microsoft Windows 3.x) without compression.

// From: https://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header and https://www.fileformat.info/format/bmp/egff.htm

#pragma pack(push, 1)
struct Bitmap_Header {
    // BMP_File_Header (2.x)
    U16 file_type; // Header used to identify BMP file. Magic number must be 0x42 0x4D
    U32 file_size; // Size (total size?)
    U16 reserved1;
    U16 reserved2;
    U32 bitmap_offset; // Starting address of pixel array

    // Bitmap_Header (3.x)
    U32 size;
    S32 width;
    S32 height;
    U16 planes;
    U16 bits_per_pixel;
    U32 compression;
    U32 size_of_bitmap;

    // TODO: Unused.
    S32 horz_resolution;
    S32 vert_resolution;
    U32 colours_used;
    U32 colours_important;
};
#pragma pack(pop)

struct Image {
    Int width;
    Int height;
    U32 *pixels;
};

internal Void write_image_to_disk(Memory *memory, Image *image, String file_name) {
    U64 output_pixel_size = image->width * image->height * sizeof(U32); // TODO: Should this not be sizeof U32...?

    Bitmap_Header header = {};
    header.file_type = 0x4D42;
    header.file_size = sizeof(header) + output_pixel_size;
    header.bitmap_offset = sizeof(Bitmap_Header);
    header.size = sizeof(header) - 14;
    header.width = image->width;
    header.height = image->height;
    header.planes = 1;
    header.bits_per_pixel = 32;
    //header.compression = 0; // BI_RGB
    header.size_of_bitmap = output_pixel_size;

    U64 to_write_size = sizeof(Bitmap_Header) + output_pixel_size;
    U8 *to_write = (U8 *)memory_push(memory, Memory_Index_temp, to_write_size);
    defer { memory_pop(memory, to_write); };
    memcpy(to_write, &header, sizeof(header));
    memcpy(to_write + sizeof(header), image->pixels, output_pixel_size);

    Bool success = system_write_to_file(file_name, to_write, to_write_size);
    ASSERT(success);
}

internal U32 *get_pixel_pointer(Image *image, Int x, Int y) {
    U32 *res = image->pixels + (y * image->width) + x;
    return(res);
}
