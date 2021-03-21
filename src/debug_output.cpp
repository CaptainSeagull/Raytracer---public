enum Debug_Node_Type {
    Debug_Node_Type_unknown,
    Debug_Node_Type_root,
    Debug_Node_Type_header, // TODO: Debug
    Debug_Node_Type_internal, // TODO: Debug

    Debug_Node_Type_count,
};

struct Debug_Node {
    Debug_Node_Type type;
    String string;

    Debug_Node *next;
    Debug_Node *child;
};

internal Debug_Node create_node(Debug_Node_Type type, String string = "", Debug_Node *next = 0, Debug_Node *child = 0) {
    Debug_Node r = {};
    r.type = type;
    r.string = string;
    r.next = next;
    r.child = child;

    return(r);
}

internal Debug_Node *new_node(Memory *memory, Debug_Node_Type type, String string = "") {
    Debug_Node *node = (Debug_Node *)memory_push(memory, Memory_Index_debug_output, sizeof(Debug_Node));
    ASSERT(node);

    if(node) {
        node->string = string;
        node->type = type;
    }

    return(node);
}

internal Debug_Node *find_end_node(Debug_Node *node) {
    if(node) {
        while(node->next) {
            node = node->next;
        }
    }

    return(node);
}

internal Debug_Node *find_first_node_of_type(Debug_Node *node, Debug_Node_Type type) {
    Debug_Node *res = 0;
    while(node) {
        if(node->type == type) {
            res = node;
            break; // while
        }

        node = node->next;
    }

    return(node);
}

internal Void internal_add_new_node(Debug_Node **parent, Debug_Node *child) {
    if((*parent)->child) {
        Debug_Node *end_node = find_end_node((*parent)->child);
        end_node->next = child;
    } else {
        (*parent)->child = child;
    }
}

internal Debug_Node *add_child_to_node(Memory *memory, Debug_Node **parent, Debug_Node_Type type, String string = "") {
    Debug_Node *node = new_node(memory, type, string);
    internal_add_new_node(parent, node);

    return(node);
}

internal String node_type_to_string(Debug_Node_Type node_type) {
    String res = {};
    switch(node_type) {
        case Debug_Node_Type_root: { res = "Debug_Node_Type_root"; } break;
        case Debug_Node_Type_header: { res = "Debug_Node_Type_header"; } break;
        case Debug_Node_Type_internal: { res = "Debug_Node_Type_internal"; } break;

        default: { ASSERT(0); }
    }

    if(res.len > 0) {
        Int prefix_length = sizeof("Debug_Node_Type_") - 1;
        res.e = res.e + prefix_length;
        res.len -= prefix_length;
    }


    return(res);
}

internal U64 internal_print_tree(Memory *memory, Debug_Node *node, Char *buf, U64 buf_size) {
    // TODO: Windows newlines? Who really cares...
    U64 used = 0;
    if(node) {
        String type = node_type_to_string(node->type);
        String string = node->string;
        used += stbsp_snprintf(buf + used, (buf_size - used), " -- ", string.len, string.e);
        if(node->string.len) {
            used += stbsp_snprintf(buf + used, (buf_size - used), " %.*s ", string.len, string.e);
        }
        used += stbsp_snprintf(buf + used, (buf_size - used), "<%.*s>", type.len, type.e);

        if(node->child) {
            used += internal_print_tree(memory, node->child, buf + used, buf_size - used);
        }

        if(node->next) {
            Char *tmp = buf;
            while(*--tmp != '\n');

            U64 indent_to_use = (buf - tmp) - 1;
            Char *space_buffer = (Char *)memory_push(memory, Memory_Index_debug_output, indent_to_use);
            memset(space_buffer, ' ', indent_to_use);

            if(indent_to_use > 0) {
                used += stbsp_snprintf(buf + used, (buf_size - used), "\n%.*s", indent_to_use, space_buffer);
            } else {
                used += stbsp_snprintf(buf + used, (buf_size - used), "\n");
            }
            memory_pop(memory, space_buffer);

            used += internal_print_tree(memory, node->next, buf + used, buf_size - used);
        }
    }

    return(used);
}

internal String print_node_to_string(Memory *memory, Memory_Index memory_index, Debug_Node *node) {
    U64 buf_size = (256 * 256);
    Char *buf = (Char *)memory_push(memory, memory_index, buf_size);
    U64 used = 0;

    used += stbsp_snprintf(buf + used, (buf_size - used), "Tree output\n\n");

    U64 bytes_written = internal_print_tree(memory, node, buf + used, buf_size - used) + used;

    U64 string_len = string_length(buf);
    ASSERT(string_len == bytes_written); // TODO: Fix me!

    String res = create_string(buf, string_len);

    return(res);
}
