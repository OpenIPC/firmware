/*
 * YAML Configurator
 *
 * This program parses a subset of YAML configuration files and supports:
 *
 * 1. Mappings and nested mappings (indented key/value pairs)
 *    Example:
 *      network:
 *        wifi:
 *          ssid: MyNetwork
 *          password: secret
 *
 * 2. Sequences – both dash notation and inline lists
 *    Dash notation:
 *      ports:
 *        - 80
 *        - 443
 *
 *    Inline list:
 *      ports_inline: [80,443,8080]
 *
 * 3. Inline mappings
 *    Example:
 *      credentials: {user:admin,password:secret}
 *
 * 4. Block scalar literals (using the '|' notation)
 *    Example:
 *      description: |
 *        This is a multi-line
 *        description text.
 *
 * Command-line arguments:
 *    -i <file>          Specify the YAML file to parse.
 *    -g <key>           Get the value at the dot-separated key path.
 *                       If the node is a container, its entire inline
 *                       representation is printed (e.g. [1,2,3]).
 *    -s <key> <value>   Set value at the dot-separated key path using inline style for lists.
 *                       (e.g. -s list.key "[1,2,3]" saves the list inline)
 *    -S <key> <value>   Set value at the dot-separated key path using dash notation for lists.
 *    -d <key>           Delete the node at the dot-separated key path.
 *
 * When using -s/-S and -d, changes are saved back to the file.
 * Leading dots (e.g. ".fpv.enabled") are allowed.
 *
 * If no operation (-g, -s, -S or -d) is specified, a sanity check is performed:
 * the YAML file is parsed and the complete structure is dumped in a pretty format.
 *
 * Example usage:
 *    ./configurator -i config.yaml
 *    ./configurator -i config.yaml -g network.wifi.ssid
 *    ./configurator -i config.yaml -s credentials.password newsecret
 *    ./configurator -i config.yaml -S list.key "[1,2,3,4]"   (dash notation for lists)
 *    ./configurator -i config.yaml -d network.ethernet
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>

typedef enum {
    YAML_NODE_SCALAR,
    YAML_NODE_MAPPING,
    YAML_NODE_SEQUENCE
} YAMLNodeType;

typedef struct YAMLNode {
    char *key;                   /* For mapping nodes; NULL for sequence items */
    char *value;                 /* For scalar values */
    YAMLNodeType type;
    struct YAMLNode **children;
    size_t num_children;
    int force_inline;            /* For sequences: if nonzero, dump inline as [a,b,c] */
} YAMLNode;

YAMLNode *current_block_literal = NULL;
int block_literal_base_indent = -1;

YAMLNode *create_node(const char *key, const char *value) {
    YAMLNode *node = malloc(sizeof(YAMLNode));
    if (!node) { perror("malloc"); exit(EXIT_FAILURE); }
    node->key = key ? strdup(key) : NULL;
    node->value = value ? strdup(value) : NULL;
    node->type = (value ? YAML_NODE_SCALAR : YAML_NODE_MAPPING);
    node->children = NULL;
    node->num_children = 0;
    node->force_inline = 0;
    return node;
}

void add_child(YAMLNode *parent, YAMLNode *child) {
    parent->children = realloc(parent->children, sizeof(YAMLNode*) * (parent->num_children + 1));
    if (!parent->children) { perror("realloc"); exit(EXIT_FAILURE); }
    parent->children[parent->num_children++] = child;
}

void free_node(YAMLNode *node) {
    if (!node) return;
    if (node->key) free(node->key);
    if (node->value) free(node->value);
    for (size_t i = 0; i < node->num_children; i++) {
        free_node(node->children[i]);
    }
    free(node->children);
    free(node);
}

/* Parse an inline sequence of the form "[item1,item2,...]". */
YAMLNode *parse_inline_sequence(const char *str) {
    YAMLNode *node = create_node(NULL, NULL);
    node->type = YAML_NODE_SEQUENCE;
    node->force_inline = 1;  /* Inline parsed list defaults to inline style */
    size_t len = strlen(str);
    if (len < 2) return node;
    char *inner = strndup(str + 1, len - 2);
    char *token, *saveptr;
    token = strtok_r(inner, ",", &saveptr);
    while (token) {
        while (*token && isspace((unsigned char)*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) { *end = '\0'; end--; }
        YAMLNode *child = create_node(NULL, token);
        child->type = YAML_NODE_SCALAR;
        add_child(node, child);
        token = strtok_r(NULL, ",", &saveptr);
    }
    free(inner);
    return node;
}

/* Parse an inline mapping of the form "{key1:value1,key2:value2,...}".
   This parser avoids splitting on commas that are inside inline sequences. */
YAMLNode *parse_inline_mapping(const char *str) {
    YAMLNode *node = create_node(NULL, NULL);
    node->type = YAML_NODE_MAPPING;
    size_t len = strlen(str);
    if (len < 2) return node;
    char *inner = strndup(str + 1, len - 2);
    int inner_len = strlen(inner);
    int token_start = 0, bracket_level = 0;
    for (int i = 0; i <= inner_len; i++) {
        char c = inner[i];
        if (c == '[') { bracket_level++; }
        else if (c == ']') { if (bracket_level > 0) bracket_level--; }
        if ((c == ',' && bracket_level == 0) || c == '\0') {
            int token_len = i - token_start;
            if (token_len > 0) {
                char *pair = strndup(inner + token_start, token_len);
                char *colon = strchr(pair, ':');
                if (colon) {
                    *colon = '\0';
                    char *k = pair;
                    char *v = colon + 1;
                    while (*k && isspace((unsigned char)*k)) k++;
                    char *kend = k + strlen(k) - 1;
                    while (kend >= k && isspace((unsigned char)*kend)) { *kend = '\0'; kend--; }
                    while (*v && isspace((unsigned char)*v)) v++;
                    char *vend = v + strlen(v) - 1;
                    while (vend >= v && isspace((unsigned char)*vend)) { *vend = '\0'; vend--; }
                    YAMLNode *child = NULL;
                    if (v[0] == '[') {
                        child = parse_inline_sequence(v);
                        free(child->key);
                        child->key = strdup(k);
                    } else if (v[0] == '{') {
                        child = parse_inline_mapping(v);
                        free(child->key);
                        child->key = strdup(k);
                    } else {
                        child = create_node(k, v);
                    }
                    child->type = (child->value ? YAML_NODE_SCALAR : YAML_NODE_MAPPING);
                    add_child(node, child);
                }
                free(pair);
            }
            token_start = i + 1;
        }
    }
    free(inner);
    return node;
}

/* Print a YAML node inline to the specified file pointer.
   Scalars print their value; sequences print as "[item,item,...]"; mappings as "{key:value,...}". */
void print_inline_yaml(FILE *f, const YAMLNode *node) {
    if (!node) return;
    if (node->type == YAML_NODE_SCALAR) {
        if (node->value)
            fprintf(f, "%s", node->value);
    } else if (node->type == YAML_NODE_SEQUENCE) {
        fprintf(f, "[");
        for (size_t i = 0; i < node->num_children; i++) {
            print_inline_yaml(f, node->children[i]);
            if (i < node->num_children - 1)
                fprintf(f, ",");
        }
        fprintf(f, "]");
    } else if (node->type == YAML_NODE_MAPPING) {
        fprintf(f, "{");
        for (size_t i = 0; i < node->num_children; i++) {
            if (node->children[i]->key)
                fprintf(f, "%s:", node->children[i]->key);
            print_inline_yaml(f, node->children[i]);
            if (i < node->num_children - 1)
                fprintf(f, ",");
        }
        fprintf(f, "}");
    }
}

/* Print inline representation to stdout (for -g). */
void print_inline(const YAMLNode *node) {
    print_inline_yaml(stdout, node);
}

/* Pretty-print the entire YAML tree (used for sanity checking). */
void print_yaml(const YAMLNode *node, int depth) {
    for (int i = 0; i < depth; i++) printf("  ");
    if (node->key)
        printf("%s: ", node->key);
    if (node->value)
        printf("%s", node->value);
    if (node->num_children > 0)
        printf(" {%s}", (node->type == YAML_NODE_SEQUENCE ? "sequence" : "mapping"));
    printf("\n");
    for (size_t i = 0; i < node->num_children; i++) {
        print_yaml(node->children[i], depth + 1);
    }
}

/* Standard parse_line() that updates the in-memory tree from one line of YAML. */
void parse_line(const char *line, int indent, YAMLNode *current_parent, int line_number) {
    if (line[0] == '-') {
        const char *value_start = line + 1;
        while (*value_start == ' ') value_start++;
        YAMLNode *node = create_node(NULL, value_start);
        node->type = YAML_NODE_SCALAR;
        if (current_parent->type != YAML_NODE_SEQUENCE)
            current_parent->type = YAML_NODE_SEQUENCE;
        add_child(current_parent, node);
    } else {
        const char *colon = strchr(line, ':');
        if (!colon) {
            fprintf(stderr, "Error at line %d: Missing ':' in mapping: %s\n", line_number, line);
            exit(EXIT_FAILURE);
        }
        size_t key_len = colon - line;
        char *key = strndup(line, key_len);
        const char *val_start = colon + 1;
        while (*val_start == ' ') val_start++;
        YAMLNode *node = NULL;
        if (*val_start != '\0') {
            if (strcmp(val_start, "|") == 0) {
                node = create_node(key, "");
                node->type = YAML_NODE_SCALAR;
                current_block_literal = node;
                block_literal_base_indent = indent + 1;
            } else if (val_start[0] == '[') {
                node = parse_inline_sequence(val_start);
                free(node->key);
                node->key = strdup(key);
            } else if (val_start[0] == '{') {
                node = parse_inline_mapping(val_start);
                free(node->key);
                node->key = strdup(key);
            } else {
                node = create_node(key, val_start);
            }
        } else {
            node = create_node(key, NULL);
            node->type = YAML_NODE_MAPPING;
        }
        add_child(current_parent, node);
        free(key);
    }
}

void parse_yaml(FILE *f, YAMLNode *root) {
    char line[1024];
    YAMLNode *stack[10] = { 0 };
    int current_level = 0;
    int line_number = 0;
    stack[0] = root;
    while (fgets(line, sizeof(line), f)) {
        line_number++;
        line[strcspn(line, "\n")] = '\0';
        int line_indent = 0;
        while (line[line_indent] == ' ') line_indent++;
        if (current_block_literal) {
            if (line_indent >= block_literal_base_indent) {
                char *text = line + block_literal_base_indent;
                char *old_val = current_block_literal->value;
                char *new_val = NULL;
                asprintf(&new_val, "%s%s\n", old_val ? old_val : "", text);
                free(current_block_literal->value);
                current_block_literal->value = new_val;
                continue;
            } else {
                current_block_literal = NULL;
            }
        }
        if (line[0] == '\0' || line[0] == '#')
            continue;
        int level = line_indent / 2;
        if (level > current_level) {
            current_level = level;
            stack[current_level] = stack[current_level - 1]->children[stack[current_level - 1]->num_children - 1];
        } else if (level < current_level) {
            current_level = level;
        }
        YAMLNode *current_parent = stack[current_level];
        parse_line(line + line_indent, line_indent, current_parent, line_number);
    }
}

YAMLNode *find_node(YAMLNode *node, const char *path) {
    while (*path == '.') { path++; }
    char *path_copy = strdup(path);
    char *token, *saveptr;
    token = strtok_r(path_copy, ".", &saveptr);
    YAMLNode *current = node;
    while (token && current) {
        if (current->type != YAML_NODE_MAPPING) { current = NULL; break; }
        int found = 0;
        for (size_t i = 0; i < current->num_children; i++) {
            if (current->children[i]->key && strcmp(current->children[i]->key, token) == 0) {
                current = current->children[i];
                found = 1;
                break;
            }
        }
        if (!found) { current = NULL; break; }
        token = strtok_r(NULL, ".", &saveptr);
    }
    free(path_copy);
    return current;
}

YAMLNode *find_or_create_node(YAMLNode *node, const char *path) {
    while (*path == '.') { path++; }
    char *path_copy = strdup(path);
    char *token, *saveptr;
    token = strtok_r(path_copy, ".", &saveptr);
    YAMLNode *current = node;
    while (token) {
        YAMLNode *child = NULL;
        for (size_t i = 0; i < current->num_children; i++) {
            if (current->children[i]->key && strcmp(current->children[i]->key, token) == 0) {
                child = current->children[i];
                break;
            }
        }
        if (!child) {
            child = create_node(token, NULL);
            child->type = YAML_NODE_MAPPING;
            add_child(current, child);
        }
        current = child;
        token = strtok_r(NULL, ".", &saveptr);
    }
    free(path_copy);
    return current;
}

int delete_node_at_path(YAMLNode *node, const char *path) {
    while (*path == '.') { path++; }
    char *path_copy = strdup(path);
    char *token, *saveptr;
    token = strtok_r(path_copy, ".", &saveptr);
    YAMLNode *current = node;
    YAMLNode *parent = NULL;
    char *last_token = NULL;
    while (token && current) {
        parent = current;
        last_token = token;
        token = strtok_r(NULL, ".", &saveptr);
        if (token) {
            int found = 0;
            for (size_t i = 0; i < parent->num_children; i++) {
                if (parent->children[i]->key && strcmp(parent->children[i]->key, last_token) == 0) {
                    current = parent->children[i];
                    found = 1;
                    break;
                }
            }
            if (!found) { free(path_copy); return 0; }
        }
    }
    if (!parent || !last_token) { free(path_copy); return 0; }
    for (size_t i = 0; i < parent->num_children; i++) {
        if (parent->children[i]->key && strcmp(parent->children[i]->key, last_token) == 0) {
            free_node(parent->children[i]);
            for (size_t j = i; j < parent->num_children - 1; j++) {
                parent->children[j] = parent->children[j + 1];
            }
            parent->num_children--;
            parent->children = realloc(parent->children, sizeof(YAMLNode*) * parent->num_children);
            free(path_copy);
            return 1;
        }
    }
    free(path_copy);
    return 0;
}

/* Dump a YAML node to file.
 * For a sequence: if force_inline is true, dump it inline ([a,b,c]);
 * otherwise, dump using dash notation.
 */
void dump_yaml_node(FILE *f, const YAMLNode *node, int indent) {
    if (indent == 0 && node->key && strcmp(node->key, "root") == 0) {
        for (size_t i = 0; i < node->num_children; i++) {
            dump_yaml_node(f, node->children[i], indent);
        }
        return;
    }
    for (int i = 0; i < indent; i++) fputc(' ', f);
    if (node->key)
        fprintf(f, "%s:", node->key);
    if (node->value) {
        if (strchr(node->value, '\n')) {
            fprintf(f, " |\n");
            char *val_copy = strdup(node->value);
            char *line = strtok(val_copy, "\n");
            while (line) {
                for (int i = 0; i < indent + 2; i++) fputc(' ', f);
                fprintf(f, "%s\n", line);
                line = strtok(NULL, "\n");
            }
            free(val_copy);
        } else {
            fprintf(f, " %s", node->value);
        }
    }
    if (node->num_children > 0) {
        if (node->type == YAML_NODE_SEQUENCE && node->force_inline) {
            fprintf(f, " ");
            print_inline_yaml(f, node);
            fprintf(f, "\n");
            return;
        }
        fprintf(f, "\n");
        if (node->type == YAML_NODE_MAPPING) {
            for (size_t i = 0; i < node->num_children; i++) {
                dump_yaml_node(f, node->children[i], indent + 2);
            }
        } else if (node->type == YAML_NODE_SEQUENCE) {
            for (size_t i = 0; i < node->num_children; i++) {
                for (int j = 0; j < indent + 2; j++) fputc(' ', f);
                fprintf(f, "- ");
                if (node->children[i]->value && node->children[i]->num_children == 0) {
                    fprintf(f, "%s\n", node->children[i]->value);
                } else {
                    fprintf(f, "\n");
                    dump_yaml_node(f, node->children[i], indent + 4);
                }
            }
        }
    } else {
        fprintf(f, "\n");
    }
}

void save_yaml(const char *filename, const YAMLNode *root) {
    FILE *f = fopen(filename, "w");
    if (!f) { perror("fopen for writing"); exit(EXIT_FAILURE); }
    dump_yaml_node(f, root, 0);
    fclose(f);
}

void handle_signal(int sig) {
    fprintf(stderr, "\nReceived signal %d, exiting gracefully...\n", sig);
    exit(EXIT_FAILURE);
}

void usage(const char *progname) {
    fprintf(stderr, "Usage: %s -i <file> [ -g <key> | -s <key> <value> | -S <key> <value> | -d <key> ]\n", progname);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -i <file>          YAML file to parse\n");
    fprintf(stderr, "  -g <key>           Get value at the dot-separated key path (outputs inline representation)\n");
    fprintf(stderr, "  -s <key> <value>   Set value at the dot-separated key path using inline style for lists\n");
    fprintf(stderr, "                     (e.g. -s list.key \"[1,2,3]\" saves the list inline)\n");
    fprintf(stderr, "  -S <key> <value>   Set value at the dot-separated key path using dash notation for lists\n");
    fprintf(stderr, "  -d <key>           Delete node at the dot-separated key path\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        usage(argv[0]);
    }
    signal(SIGINT, handle_signal);
    char *filename = NULL, *get_path = NULL, *set_path = NULL, *set_value = NULL, *delete_path = NULL;
    int opt;
    int set_dash = 0; /* 0 = inline style (-s), 1 = dash notation (-S) */
    while ((opt = getopt(argc, argv, "i:g:s:S:d:")) != -1) {
        switch (opt) {
            case 'i':
                filename = optarg;
                break;
            case 'g':
                get_path = optarg;
                break;
            case 's':
                set_path = optarg;
                if (optind < argc) {
                    set_value = argv[optind++];
                } else {
                    fprintf(stderr, "Error: -s requires a key and a value.\n");
                    usage(argv[0]);
                }
                set_dash = 0;
                break;
            case 'S':
                set_path = optarg;
                if (optind < argc) {
                    set_value = argv[optind++];
                } else {
                    fprintf(stderr, "Error: -S requires a key and a value.\n");
                    usage(argv[0]);
                }
                set_dash = 1;
                break;
            case 'd':
                delete_path = optarg;
                break;
            default:
                usage(argv[0]);
        }
    }
    if (!filename) {
        fprintf(stderr, "Error: No input file specified.\n");
        usage(argv[0]);
    }
    FILE *f = fopen(filename, "r");
    if (!f) { perror("fopen"); exit(EXIT_FAILURE); }
    YAMLNode *root = create_node("root", NULL);
    root->type = YAML_NODE_MAPPING;
    parse_yaml(f, root);
    fclose(f);

    if (get_path) {
        YAMLNode *node = find_node(root, get_path);
        if (node) {
            if (node->type == YAML_NODE_SCALAR && node->value)
                printf("%s\n", node->value);
            else {
                print_inline(node);
                printf("\n");
            }
            fflush(stdout);
        } else {
            printf("Node not found.\n");
        }
    } else if (set_path) {
        YAMLNode *node = find_or_create_node(root, set_path);
        if (node) {
            if (set_value[0] == '[') {
                if (node->value) { free(node->value); node->value = NULL; }
                for (size_t i = 0; i < node->num_children; i++) {
                    free_node(node->children[i]);
                }
                free(node->children);
                YAMLNode *new_list = parse_inline_sequence(set_value);
                node->children = new_list->children;
                node->num_children = new_list->num_children;
                node->type = YAML_NODE_SEQUENCE;
                node->force_inline = (set_dash ? 0 : 1);
                new_list->children = NULL;
                free(new_list);
            } else if (set_value[0] == '{') {
                if (node->value) { free(node->value); node->value = NULL; }
                for (size_t i = 0; i < node->num_children; i++) {
                    free_node(node->children[i]);
                }
                free(node->children);
                YAMLNode *new_map = parse_inline_mapping(set_value);
                node->children = new_map->children;
                node->num_children = new_map->num_children;
                node->type = YAML_NODE_MAPPING;
                new_map->children = NULL;
                free(new_map);
            } else {
                if (node->value) free(node->value);
                for (size_t i = 0; i < node->num_children; i++) {
                    free_node(node->children[i]);
                }
                free(node->children);
                node->children = NULL;
                node->num_children = 0;
                node->value = strdup(set_value);
                node->type = YAML_NODE_SCALAR;
            }
            printf("Value set at '%s'.\n", set_path);
            save_yaml(filename, root);
            printf("Changes saved to file '%s'.\n", filename);
        } else {
            printf("Could not set value at '%s'.\n", set_path);
        }
    } else if (delete_path) {
        int result = delete_node_at_path(root, delete_path);
        if (result) {
            printf("Node '%s' deleted.\n", delete_path);
            save_yaml(filename, root);
            printf("Changes saved to file '%s'.\n", filename);
        } else {
            printf("Node '%s' not found.\n", delete_path);
        }
    } else {
        /* Sanity check: no operation specified, so dump the parsed structure */
        printf("YAML configuration parsed successfully. Dumping structure:\n");
        print_yaml(root, 0);
    }
    free_node(root);
    return EXIT_SUCCESS;
}
