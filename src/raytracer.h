
struct Material {
    F32 specular; // 0 is pure diffuse ("chalk"), 1 is pure specular ("mirror")
    V3 emit_colour;
    V4 ref_colour;
};

struct Texture {
    F32 specular;
    V3 emit_colour;
    Image image;
};

struct Plane {
    V3  n;
    F32 d;
    Int mat_i;
};

struct Sphere {
    V3  p;
    F32 r;
    Int mat_i;
};

// TODO: Is it better to render faces normally or pre-triangulated? Do some investigation into the performance difference.
struct Face {
    V3  *pts;
    Int p_len;
    V3  n;
    F32 d;

    Int mat_i;
};

// TODO: This structure is larger than it has to be, may end up blowing over cache lines.
struct Textured_Face {
    // TODO: Could move this to the end, require Texture_Faces be allocated, then point this to just past the end. Would keep the
    //       memory more local.
    V3  *pts;
    Int p_len;

    V3  n;
    F32 d;

    V3 translation;
    V3 rotation;
    V3 scale;

    // TODO: Do I need to store these in world-space? Could I store 2 V2's in local space?
    V3 top_left;
    V3 bottom_right;

    // TODO: Add different modes. Repeated, stretched, etc...?
    Int tex_i;
};

struct Camera {
    V3 p;
    V3 x;
    V3 y;
    V3 z;
};

struct Scene {
    Int material_count;
    Material *materials;

    Int texture_count;
    Texture *textures;

    Int plane_count;
    Plane *planes;

    Int sphere_count;
    Sphere *spheres;

    Int face_count;
    Face *faces;

    Int textured_face_count;
    Textured_Face *textured_faces;
};
