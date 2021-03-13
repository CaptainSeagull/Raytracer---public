enum Node_Type {
    Node_Type_unknown,
    Node_Type_root,
    Node_Type_number_tag,
    Node_Type_string_tag,
    Node_Type_object,
    Node_Type_identifier,
    Node_Type_array,
    Node_Type_number,
    Node_Type_string,

    Node_Type_count
};

struct Node {
    Token token;
    Node_Type type;

    Node *child;
    Node *next;
};

#define NODE_FOREACH(_node_name, root) for(Node *_node_name = (root); (_node_name); _node_name = _node_name->next)

internal Token blank_token() {
    Token r = {};
    return(r);
}

internal Node *new_node(Memory *memory, Node_Type type, Token token = blank_token()) {
    Node *node = (Node *)memory_push(memory, Memory_Index_json_parse, sizeof(Node));
    ASSERT(node);

    if(node) {
        node->token = token;
        node->type = type;
    }

    return(node);
}

internal Node *find_end_node(Node *node) {
    if(node) {
        while(node->next) {
            node = node->next;
        }
    }

    return(node);
}

internal Void internal_add_new_node(Node **parent, Node *child) {
    if((*parent)->child) {
        Node *end_node = find_end_node((*parent)->child);
        end_node->next = child;
    } else {
        (*parent)->child = child;
    }
}

internal Node *add_child_to_node(Memory *memory, Node **parent, Node_Type type, Token token = blank_token()) {
    Node *node = new_node(memory, type, token);
    internal_add_new_node(parent, node);

    return(node);
}

internal String node_type_to_string(Node_Type node_type) {
    String r = {};
    switch(node_type) {
        case Node_Type_root: { r = "root"; } break;
        case Node_Type_object: { r = "object"; } break;
        case Node_Type_identifier: { r = "identifier"; } break;
        case Node_Type_array: { r = "array"; } break;
        case Node_Type_number: { r = "number"; } break;
        case Node_Type_string: { r = "string"; } break;
        case Node_Type_number_tag: { r = "number_tag"; } break;
        case Node_Type_string_tag: { r = "string_tag"; } break;
    }

    return(r);
}

internal Token create_token(String string, Token_Type type = Token_Type_unknown) {
    Token res = {};
    res.e = string.e;
    res.len = string.len;
    res.type = type;

    return(res);
}

#define set(dst, v, n) set_(((Void *)(dst)), (v), (n))
internal Void *set_(Void *dst, Byte v, Uintptr n) {
    Byte *dest8 = (Byte *)dst;
    for(Uintptr i = 0; (i < n); ++i, ++dest8) {
        *dest8 = (Byte)v;
    }

    return(dst);
}

internal U64 internal_print_tree(Memory *memory, Node *node, Char *buf_root, Char *buf, U64 buf_size, Bool print_type) {
    U64 used = 0;
    if(node) {
        Token type_token = create_token(node_type_to_string(node->type));
        Token token = node->token;
        used += stbsp_snprintf(buf + used, (buf_size - used), " -- ", token.len, token.e);
        if(node->token.len) {
            used += stbsp_snprintf(buf + used, (buf_size - used), " %.*s ", token.len, token.e);
        }
        if(print_type) {
            used += stbsp_snprintf(buf + used, (buf_size - used), "<%.*s>", type_token.len, type_token.e);
        }

        if(node->child) {
            used += internal_print_tree(memory, node->child, buf_root, buf + used, buf_size - used, print_type);
        }

        if(node->next) {
            // Seek back to the start of the line (before the previous \n). Also special case in case this is the first line with no
            // previous \n.
            Char *tmp = buf;
            while(*--tmp != '\n') {
                if(tmp == buf_root) { break; }
            }

            U64 indent_to_use = (buf - tmp) - 1;
            Char *space_buffer = (Char *)memory_push(memory, Memory_Index_temp, indent_to_use); // TODO: Use memory arena
            set(space_buffer, ' ', indent_to_use);


            if(indent_to_use > 0) {
                used += stbsp_snprintf(buf + used, (buf_size - used), "\n%.*s", indent_to_use, space_buffer);
            } else {
                used += stbsp_snprintf(buf + used, (buf_size - used), "\n");
            }
            memory_pop(memory, space_buffer);

            used += internal_print_tree(memory, node->next, buf_root, buf + used, buf_size - used, print_type);
        }
    }

    return(used);
}

internal Int get_tree_height(Node *node, Int height = 0) {
    Int left_height = height;
    Int right_height = height;

    if(node) {
        if(node->next) {
            ++left_height;
            left_height = get_tree_height(node->next, left_height);
        }
        if(node->child) {
            ++right_height;
            right_height = get_tree_height(node->child, right_height);
        }
    }

    return MAX(left_height, right_height);
}

internal String print_node_to_string(Memory *memory, Node *node) {
    U64 buf_size = (256 * 256);
    Char *buf = (Char *)memory_push(memory, Memory_Index_json_parse, buf_size);

    U64 bytes_written = internal_print_tree(memory, node, buf, buf, buf_size, true);

    U64 string_len = string_length(buf);
    ASSERT(string_len == bytes_written); // TODO: Fix me!

    String res = create_string(buf, string_len);

    return(res);
}

internal Void print_node(Memory *memory, String path, Node *node) {
    String string_to_output = print_node_to_string(memory, node);

    system_write_file(path, (U8 *)string_to_output.e, string_to_output.len);
}

internal Void internal_parse(Memory *memory, Tokenizer *tokenizer, Node *parent) {
    Token token = get_token(tokenizer);
    Bool should_loop = true;
    do {
        switch(token.type) {
            case Token_Type_open_brace: {
                Node *node = add_child_to_node(memory, &parent, Node_Type_object);
                internal_parse(memory, tokenizer, node);
            } break;

            case Token_Type_close_brace: {
                ASSERT(parent->type == Node_Type_object);
                should_loop = false;
            } break;

            case Token_Type_open_bracket: {
                Node *node = add_child_to_node(memory, &parent, Node_Type_array);
                internal_parse(memory, tokenizer, node);
            } break;

            case Token_Type_close_bracket: {
                ASSERT(parent->type == Node_Type_array);
                should_loop = false;
            } break;

            case Token_Type_identifier: {
                //Node *node = add_child_to_node(memory, &parent, Node_Type_object, token);
                if(peak_token(tokenizer).type == Token_Type_colon) { eat_token(tokenizer); }

                Token next_token = peak_token(tokenizer);
                switch(next_token.type) {
                    case Token_Type_open_brace: {
                        eat_token(tokenizer);
                        Node *node = add_child_to_node(memory, &parent, Node_Type_object, token);
                        internal_parse(memory, tokenizer, node);
                    } break;

                    case Token_Type_open_bracket: {
                        eat_token(tokenizer);
                        Node *node = add_child_to_node(memory, &parent, Node_Type_array, token);
                        internal_parse(memory, tokenizer, node);
                    } break;

                    case Token_Type_number: {
                        Node *node = add_child_to_node(memory, &parent, Node_Type_number_tag, token);
                        add_child_to_node(memory, &node, Node_Type_number, next_token);
                        eat_token(tokenizer);
                    } break;

                    case Token_Type_string: {
                        Node *node = add_child_to_node(memory, &parent, Node_Type_string_tag, token);
                        add_child_to_node(memory, &node, Node_Type_string, next_token);
                        eat_token(tokenizer);
                    } break;

                    case Token_Type_minus: {
                        Node *node = add_child_to_node(memory, &parent, Node_Type_number_tag, token);
                        eat_token(tokenizer);
                        Token next_next_token = get_token(tokenizer); // TODO: Fucking name...
                        ASSERT(next_next_token.type == Token_Type_number);
                        --next_next_token.e;
                        ++next_next_token.len;
                        add_child_to_node(memory, &node, Node_Type_number);

                        eat_token(tokenizer);
                    } break;

                    default: { ASSERT(0); }
                }

            } break;

            case Token_Type_minus: {
                Token next_token = get_token(tokenizer);
                ASSERT(next_token.type == Token_Type_number);
                --next_token.e;
                ++next_token.len;
                Node *node = add_child_to_node(memory, &parent, Node_Type_number, next_token);
            } break;

            case Token_Type_number: {
                Node *node = add_child_to_node(memory, &parent, Node_Type_number, token);
            } break;

            case Token_Type_string: {
                Node *node = add_child_to_node(memory, &parent, Node_Type_string, token);
            } break;
        }

        if(should_loop) {
            token = get_token(tokenizer);
        }
    } while(should_loop && token.type != Token_Type_end_of_stream);
}

internal String token_to_string(Token t) {
    String res = create_string((t.e) ? t.e : "", t.len);
    return(res);
}

internal Bool operator==(Token t, String s) {
    Bool r = string_compare(token_to_string(t), s);
    return(r);
}

internal Bool operator==(String s, Token t) {
    Bool r = string_compare(token_to_string(t), s);
    return(r);
}

internal V3 node_to_v3(Node *node) {
    V3 r = {};
    Int i = 0;
    NODE_FOREACH(child, node->child) {
        r.e[i++] = string_to_float(token_to_string(child->token)).v;
    }
    ASSERT(i == 3);

    return(r);
}

internal V4 node_to_v4(Node *node) {
    V4 r = {};
    Int i = 0;
    NODE_FOREACH(child, node->child) {
        r.e[i++] = string_to_float(token_to_string(child->token)).v;
    }
    ASSERT(i == 4);

    return(r);
}

internal F32 node_to_float(Node *node) {
    String_To_Float_Result r = string_to_float(token_to_string(node->child->token));
    ASSERT(r.success);
    return(r.v);
}

internal Int node_to_int(Node *node) {
    String_To_Int_Result r = string_to_int(token_to_string(node->child->token));
    ASSERT(r.success);
    return(r.v);
}

internal Void iterate_and_create_scene(Memory *memory, Scene *scene, Node *node) {
    NODE_FOREACH(cur, node) {
        if(cur->type == Node_Type_object) {
#if 0
            // TODO: We store the camera information in API now. So this part doesn't work.
            if(cur->token == "camera") {
                NODE_FOREACH(camera_field, cur->child) {
                    if(camera_field->token == "position") {
                        scene->camera.p = node_to_v3(camera_field);
                    }

                    else if(camera_field->token == "x") {
                        scene->camera.x = node_to_v3(camera_field);
                    }

                    else if(camera_field->token == "y") {
                        scene->camera.y = node_to_v3(camera_field);
                    }

                    else if(camera_field->token == "z") {
                        scene->camera.z = node_to_v3(camera_field);
                    }
                }
            }
#endif
        }

        else if(cur->type == Node_Type_array) {
            if(cur->token == "materials") {
                NODE_FOREACH(material_object, cur->child) {
                    Material *material = &scene->materials[scene->material_count++];
                    NODE_FOREACH(material_field, material_object->child) {
                        if(material_field->token == "emit_colour") {
                            material->emit_colour = node_to_v3(material_field);
                        }

                        else if(material_field->token == "ref_colour") {
                            material->ref_colour = node_to_v4(material_field);
                        }

                        else if(material_field->token == "specular") {
                            material->specular = node_to_float(material_field);
                        }
                    }
                }
            }

            else if(cur->token == "planes") {
                NODE_FOREACH(plane_object, cur->child) {
                    Plane *plane = &scene->planes[scene->plane_count++];
                    NODE_FOREACH(plane_field, plane_object->child) {
                        if(plane_field->token == "distance_from_origin") {
                            plane->d = node_to_float(plane_field);
                        }

                        else if(plane_field->token == "normal") {
                            plane->n = node_to_v3(plane_field);
                        }

                        else if(plane_field->token == "material_index") {
                            plane->mat_i = node_to_int(plane_field);
                        }
                    }
                }
            }

            else if(cur->token == "spheres") {
                NODE_FOREACH(sphere_object, cur->child) {
                    Sphere *sphere = &scene->spheres[scene->sphere_count++];
                    NODE_FOREACH(sphere_field, sphere_object->child) {
                        if(sphere_field->token == "position") {
                            sphere->p = node_to_v3(sphere_field);
                        }

                        else if(sphere_field->token == "radius") {
                            sphere->r = node_to_float(sphere_field);
                        }

                        else if(sphere_field->token == "material_index") {
                            sphere->mat_i = node_to_int(sphere_field);
                        }
                    }
                }
            }

            else if(cur->token == "faces") {
                NODE_FOREACH(face_object, cur->child) {
                    V3 p[12] = {};
                    U32 p_len = 0;
                    U32 mat_i = 0;
                    NODE_FOREACH(face_field, face_object->child) {
                        if(face_field->token == "points") {
                            NODE_FOREACH(point_node, face_field->child) {
                                p[p_len++] = node_to_v3(point_node);
                            }
                        }

                        else if(face_field->token == "material_index") {
                            mat_i = node_to_int(face_field);
                        }
                    }

                    // TODO: Use create_xxx for the other objects as well, not just faces?
                    ASSERT(p_len < ARRAY_COUNT(p));
                    scene->faces[scene->face_count++] = create_face(memory, p, p_len, mat_i);
                }
            }
        }

        if(cur->child) {
            iterate_and_create_scene(memory, scene, cur->child);
        }
    }
}

internal Void create_scene_from_json(Memory *memory, String json, Scene *scene) {
    // TODO: json HAS to be null terminated for this to work. A version of create_tokenizer which takes a string + length would be useful.
    Tokenizer tokenizer = create_tokenizer(json.e);

    Node root = {};
    root.type = Node_Type_root;

    internal_parse(memory, &tokenizer, &root);

#if INTERNAL
    // Debug code to print the tree to disk.
    print_node(memory, "output/tree.txt", &root);
#endif

    iterate_and_create_scene(memory, scene, &root);

    memory_clear_entire_group(memory, Memory_Index_json_parse);
}
