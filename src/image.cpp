//
// Crappy bitmap loader... write bitmaps in BMP Version 3 (Microsoft Windows 3.x) without compression.

// From: https://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header and https://www.fileformat.info/format/bmp/egff.htm

struct Image {
    Int width;
    Int height;
    U32 *pixels;
};

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
    U32 compression; // NOTE: This is NOT compression... if this is 3 we need to use the colour mask to determine the RGBA slots...
    U32 size_of_bitmap;

    // TODO: Unused.
    S32 horz_resolution;
    S32 vert_resolution;
    U32 colours_used;
    U32 colours_important;
};
#pragma pack(pop)

internal Void write_image_to_disk(API *api, Memory *memory, Image *image, String file_name) {
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
    memcpy(to_write, &header, sizeof(header));
    memcpy(to_write + sizeof(header), image->pixels, output_pixel_size);

    Bool success = api->cb.write_file(file_name, to_write, to_write_size);
    ASSERT(success);

    memory_pop(memory, to_write);
}

internal Image load_image(API *api, Memory *memory, String file_name) {
    Image img = {};

    File raw_bitmap = api->cb.read_file(memory, Memory_Index_temp, file_name, false);

    ASSERT(raw_bitmap.size > 0);
    if(raw_bitmap.size > 0) {
        Bitmap_Header *header = (Bitmap_Header *)raw_bitmap.e;
        ASSERT((header->file_type == 0x4D42) &&
               (header->planes == 1) &&
               (header->compression == 0));

        Void *data_start = (U8 *)raw_bitmap.e + header->bitmap_offset;

        Void *bitmap_memory = memory_push(memory, Memory_Index_permanent, header->width * header->height * sizeof(U32));

        img.width = header->width;
        img.height = header->height;
        img.pixels = (U32 *)bitmap_memory;

        switch(header->bits_per_pixel) {
            case 32: { memcpy(img.pixels, data_start, raw_bitmap.size - header->bitmap_offset);  } break;
            case 24: {
                U8 *src_at = (U8 *)data_start;
                U32 *dst_at = (U32 *)img.pixels;
                for(U64 pixel_i = 0; (pixel_i < raw_bitmap.size - header->bitmap_offset); pixel_i += 3) {
                    U8 *r = src_at + 0;
                    U8 *g = src_at + 1;
                    U8 *b = src_at + 2;

                    *dst_at = (0xFF << 24) | (*b << 16) | (*g << 8) | (*r << 0);
                    dst_at += 1;
                    src_at += 3;
                }
            } break;
        }

        //write_image_to_disk(memory, &img, "readme/test.bmp"); // Write the loaded in bitmap back to disk. Was used for testing 24-bits per pixel.
        memory_pop(memory, raw_bitmap.e);
    }

    return(img);
}