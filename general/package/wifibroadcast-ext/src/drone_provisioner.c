#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <stdint.h>

#define DEFAULT_SERVER_IP "10.5.99.2"
#define DEFAULT_SERVER_PORT 5555
#define BUFFER_SIZE 8192
#define DEFAULT_LISTEN_DURATION 60  // seconds

// Define directories and file paths for BIND and FLASH commands.
#define BIND_DIR "/tmp/bind"
#define BIND_FILE "/tmp/bind/bind.tar.gz"
#define FLASH_DIR "/tmp/flash"
#define FLASH_FILE "/tmp/flash/flash.tar.gz"

// Exit code definitions.
#define EXIT_ERR    1
#define EXIT_BIND   2
#define EXIT_UNBIND 3
#define EXIT_FLASH  4
#define EXIT_BACKUP 5  // For BACKUP command

// Global flag for debug output.
static int debug_enabled = 0;

// Global bandwidth limit in bits/second (default: 1,000,000 bits/s = 1 Mbit/s).
static double g_bw_limit_bits = 1000000.0;

/*--------------------------------------------------
 * Helper Functions
 *--------------------------------------------------*/

// Print debug messages if debug is enabled.
void debug_print(const char *fmt, ...) {
    if (!debug_enabled)
        return;
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "DEBUG: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}

// Print usage help.
void print_help() {
    fprintf(stderr, "Usage: wfb_bind_rcv [OPTIONS]\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --ip <address>          Set server IP address (default: %s)\n", DEFAULT_SERVER_IP);
    fprintf(stderr, "  --port <number>         Set server port (default: %d)\n", DEFAULT_SERVER_PORT);
    fprintf(stderr, "  --listen-duration <sec> Set duration to listen (default: %d seconds)\n", DEFAULT_LISTEN_DURATION);
    fprintf(stderr, "  --force-listen          Continue listening even after a terminating command\n");
    fprintf(stderr, "  --bw-limit <bits/s>     Bandwidth limit in bits/sec (default: 1000000)\n");
    fprintf(stderr, "  --debug                 Enable debug output\n");
    fprintf(stderr, "  --help                  Show this help message\n");
}

// Ensure that a directory exists.
void ensure_directory(const char *dir) {
    struct stat st = {0};
    if (stat(dir, &st) == -1) {
        if (mkdir(dir, 0777) != 0) {
            fprintf(stderr, "ERR\tFailed to create directory: %s\n", dir);
            exit(EXIT_ERR);
        }
    }
}

// Read entire file into a dynamically allocated buffer (including newlines).
char *read_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return strdup("Failed to read file");
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = malloc(fsize + 1);
    if (!buffer) {
        fclose(fp);
        return strdup("Memory allocation error");
    }
    size_t nread = fread(buffer, 1, fsize, fp);
    fclose(fp);

    buffer[nread] = '\0';
    return buffer;
}

// Base64 decode the input string and write the decoded data to a file.
int base64_decode_and_save_to(const char *input, size_t input_length, const char *dir, const char *file) {
    ensure_directory(dir);
    FILE *output_file = fopen(file, "wb");
    if (!output_file) {
        fprintf(stderr, "ERR\tFailed to open output file: %s\n", file);
        return 1;
    }
    unsigned char decode_buffer[BUFFER_SIZE];
    int val = 0, valb = -8;
    size_t out_len = 0;
    for (size_t i = 0; i < input_length; i++) {
        char c = input[i];
        if (c == '=' || c == '\n' || c == '\r')
            continue;
        char *pos = strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", c);
        if (pos == NULL)
            continue;
        val = (val << 6) + (pos - "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
        valb += 6;
        if (valb >= 0) {
            decode_buffer[out_len++] = (val >> valb) & 0xFF;
            valb -= 8;
        }
        if (out_len >= BUFFER_SIZE) {
            fwrite(decode_buffer, 1, out_len, output_file);
            out_len = 0;
        }
    }
    if (out_len > 0) {
        fwrite(decode_buffer, 1, out_len, output_file);
    }
    fclose(output_file);
    return 0;
}

// Return elapsed time (in seconds) since 'start', using monotonic clock.
static double elapsed_time_sec(const struct timespec *start) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    double seconds = (double)(now.tv_sec - start->tv_sec);
    double nsecs   = (double)(now.tv_nsec - start->tv_nsec) / 1e9;
    return seconds + nsecs;
}

// Bandwidth-limited write: writes data in chunks, sleeping if we exceed bits/sec.
static void throttle_write(FILE *fp, const char *data, size_t data_len, double bits_per_sec) {
    const size_t chunk_size = 4096;
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    double bytes_per_sec = bits_per_sec / 8.0;
    size_t total_sent = 0;

    while (total_sent < data_len) {
        size_t remain = data_len - total_sent;
        size_t send_now = (remain < chunk_size) ? remain : chunk_size;
        
        size_t n = fwrite(data + total_sent, 1, send_now, fp);
        if (n < send_now) {
            // If we failed to write the full chunk, likely connection closed or error
            total_sent += n;
            break;
        }
        
        total_sent += n;
        fflush(fp);  // ensure timely sending

        // Calculate how much we've sent vs. how much we *should* have sent
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed = (double)(now.tv_sec - start.tv_sec) +
                         (double)(now.tv_nsec - start.tv_nsec) / 1e9;
        double allowed_bytes = bytes_per_sec * elapsed;

        if (total_sent > allowed_bytes) {
            double excess_bytes = total_sent - allowed_bytes;
            double wait_time = excess_bytes / bytes_per_sec; // in seconds
            if (wait_time > 0) {
                usleep((useconds_t)(wait_time * 1e6));
            }
        }
    }
}

// Execute a system command and capture its output in a dynamically allocated string.
char *execute_command(const char *cmd) {
    FILE *fp = popen(cmd, "r");
    if (!fp)
        return NULL;
    
    size_t size = 4096;
    char *output = malloc(size);
    if (!output) {
        pclose(fp);
        return NULL;
    }
    output[0] = '\0';
    size_t len = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        size_t buffer_len = strlen(buffer);
        if (len + buffer_len + 1 > size) {
            size = (len + buffer_len + 1) * 2;
            char *temp = realloc(output, size);
            if (!temp) {
                free(output);
                pclose(fp);
                return NULL;
            }
            output = temp;
        }
        strcpy(output + len, buffer);
        len += buffer_len;
    }
    pclose(fp);
    return output;
}

// Base64 encode a given binary buffer.
char *base64_encode(const unsigned char *data, size_t input_length) {
    static const char encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const int mod_table[] = {0, 2, 1};
    size_t output_length = 4 * ((input_length + 2) / 3);
    char *encoded_data = malloc(output_length + 1);
    if (!encoded_data)
        return NULL;
    size_t i, j;
    for (i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;
        uint32_t triple = (octet_a << 16) | (octet_b << 8) | (octet_c);
        encoded_data[j++] = encoding_table[(triple >> 18) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 12) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 6) & 0x3F];
        encoded_data[j++] = encoding_table[triple & 0x3F];
    }
    // Pad as needed
    for (i = 0; i < (size_t)mod_table[input_length % 3]; i++) {
        encoded_data[output_length - 1 - i] = '=';
    }
    encoded_data[output_length] = '\0';
    return encoded_data;
}

/*--------------------------------------------------
 * Command Handler Declarations
 *--------------------------------------------------*/

typedef int (*command_handler)(const char *arg, FILE *client_file, int force_listen);

/*
 * Each command handler sends a reply to the connected peer.
 * If the command should cause the program to terminate (and force_listen is false),
 * the handler returns the exit code to use (nonzero). Otherwise, it returns 0.
 */

// VERSION: reply with version info.
int cmd_version(const char *arg, FILE *client_file, int force_listen) {
    (void)arg;
    (void)force_listen;
    fprintf(client_file, "OK\tOpenIPC bind v0.1\n");
    fflush(client_file);
    return 0;
}

// BIND: decode base64 input and save to BIND_FILE.
int cmd_bind(const char *arg, FILE *client_file, int force_listen) {
    if (arg == NULL || strlen(arg) == 0) {
        fprintf(client_file, "ERR\tMissing argument for BIND command\n");
        fflush(client_file);
        return 0;
    }
    debug_print("Received BIND command with base64 length: %zu\n", strlen(arg));
    if (base64_decode_and_save_to(arg, strlen(arg), BIND_DIR, BIND_FILE) == 0) {
        fprintf(client_file, "OK\n");
        fflush(client_file);
        if (!force_listen)
            return EXIT_BIND;
    } else {
        fprintf(client_file, "ERR\tFailed to process data for BIND\n");
        fflush(client_file);
    }
    return 0;
}

// FLASH: decode base64 input and save to FLASH_FILE.
int cmd_flash(const char *arg, FILE *client_file, int force_listen) {
    if (arg == NULL || strlen(arg) == 0) {
        fprintf(client_file, "ERR\tMissing argument for FLASH command\n");
        fflush(client_file);
        return 0;
    }
    debug_print("Received FLASH command with base64 length: %zu\n", strlen(arg));
    if (base64_decode_and_save_to(arg, strlen(arg), FLASH_DIR, FLASH_FILE) == 0) {
        fprintf(client_file, "OK\n");
        fflush(client_file);
        if (!force_listen)
            return EXIT_FLASH;
    } else {
        fprintf(client_file, "ERR\tFailed to process data for FLASH\n");
        fflush(client_file);
    }
    return 0;
}

// UNBIND: execute the system command "firstboot".
int cmd_unbind(const char *arg, FILE *client_file, int force_listen) {
    (void)arg;
    debug_print("Received UNBIND command\n");
    int ret = system("firstboot");
    if (ret == -1) {
        fprintf(client_file, "ERR\tFailed to execute UNBIND command\n");
    } else if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0) {
        fprintf(client_file, "OK\tUNBIND executed successfully\n");
        fflush(client_file);
        if (!force_listen)
            return EXIT_UNBIND;
    } else {
        fprintf(client_file, "ERR\tUNBIND command returned error code %d\n", WEXITSTATUS(ret));
    }
    fflush(client_file);
    return 0;
}

/*
 * INFO: now we only read the file /etc/vtx_info.yaml.
 * We preserve newlines as they are read, base64-encode them directly,
 * and then send them with bandwidth throttling if needed.
 */
int cmd_info(const char *arg, FILE *client_file, int force_listen) {
    (void)arg;
    (void)force_listen;
    debug_print("Received INFO command\n");

    // Read content from /etc/vtx_info.yaml directly
    char *info_content = read_file("/etc/vtx_info.yaml");
    if (!info_content) {
        info_content = strdup("Failed to read /etc/vtx_info.yaml");
    }

    // Base64-encode the raw data (including any newlines)
    char *encoded_response = base64_encode((unsigned char*)info_content, strlen(info_content));
    if (encoded_response) {
        size_t enc_len = strlen(encoded_response);

        fprintf(client_file, "OK\t");
        fflush(client_file);

        // Throttle the base64 data
        throttle_write(client_file, encoded_response, enc_len, g_bw_limit_bits);

        fprintf(client_file, "\n");
        fflush(client_file);

        free(encoded_response);
    } else {
        fprintf(client_file, "ERR\tFailed to encode response\n");
        fflush(client_file);
    }

    free(info_content);
    return 0;
}

// BACKUP command - unchanged from previous, with throttled output if large
int cmd_backup(const char *arg, FILE *client_file, int force_listen) {
    (void)arg;  // BACKUP has no base64 input
    debug_print("Received BACKUP command\n");

    // 1. Run generate_backup.sh
    int ret = system("generate_backup.sh");
    if (ret == -1) {
        fprintf(client_file, "ERR\tFailed to execute generate_backup.sh\n");
        fflush(client_file);
        return 0;  // continue listening
    }
    if (!WIFEXITED(ret) || WEXITSTATUS(ret) != 0) {
        fprintf(client_file, "ERR\tgenerate_backup.sh returned error code %d\n", WEXITSTATUS(ret));
        fflush(client_file);
        return 0;
    }

    // 2. Check if /tmp/backup/backup.tar.gz exists
    struct stat st;
    if (stat("/tmp/backup/backup.tar.gz", &st) != 0) {
        fprintf(client_file, "ERR\tBackup file not found\n");
        fflush(client_file);
        return 0;
    }

    // 3. Read the file into memory (binary)
    FILE *fp = fopen("/tmp/backup/backup.tar.gz", "rb");
    if (!fp) {
        fprintf(client_file, "ERR\tFailed to open /tmp/backup/backup.tar.gz\n");
        fflush(client_file);
        return 0;
    }
    unsigned char *buffer = malloc(st.st_size);
    if (!buffer) {
        fclose(fp);
        fprintf(client_file, "ERR\tMemory allocation error\n");
        fflush(client_file);
        return 0;
    }
    size_t read_count = fread(buffer, 1, st.st_size, fp);
    fclose(fp);
    if (read_count != (size_t)st.st_size) {
        free(buffer);
        fprintf(client_file, "ERR\tFailed to read backup file\n");
        fflush(client_file);
        return 0;
    }

    // 4. Base64-encode the file contents
    char *encoded_data = base64_encode(buffer, read_count);
    free(buffer);
    if (!encoded_data) {
        fprintf(client_file, "ERR\tBase64 encoding failed\n");
        fflush(client_file);
        return 0;
    }

    // 5. Respond with "OK\t" then throttled base64 data, then newline
    fprintf(client_file, "OK\t");
    fflush(client_file);

    throttle_write(client_file, encoded_data, strlen(encoded_data), g_bw_limit_bits);

    fprintf(client_file, "\n");
    fflush(client_file);

    free(encoded_data);

    // 6. If not force_listen, terminate with EXIT_BACKUP
    if (!force_listen) {
        return EXIT_BACKUP;  // = 5
    }
    return 0;
}

/*--------------------------------------------------
 * Command Dispatch
 *--------------------------------------------------*/

typedef struct {
    const char *name;
    command_handler handler;
} command_entry;

command_entry commands[] = {
    { "VERSION", cmd_version },
    { "BIND",    cmd_bind    },
    { "FLASH",   cmd_flash   },
    { "UNBIND",  cmd_unbind  },
    { "INFO",    cmd_info    },  // updated to read /etc/vtx_info.yaml
    { "BACKUP",  cmd_backup  },
    { NULL,      NULL        }   // sentinel
};

/*
 * Dispatch a command based on the command lookup table.
 * Returns a nonzero exit code if the command requests termination; otherwise returns 0.
 */
int handle_command(const char *cmd, const char *arg, FILE *client_file, int force_listen) {
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(cmd, commands[i].name) == 0) {
            return commands[i].handler(arg, client_file, force_listen);
        }
    }
    fprintf(client_file, "ERR\tUnknown command\n");
    fflush(client_file);
    return 0;
}

/*--------------------------------------------------
 * Main
 *--------------------------------------------------*/

int main(int argc, char *argv[]) {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int listen_duration = DEFAULT_LISTEN_DURATION;
    char server_ip[INET_ADDRSTRLEN] = DEFAULT_SERVER_IP;
    int server_port = DEFAULT_SERVER_PORT;
    int force_listen = 0;  // Default: terminate on a successful terminating command.

    int exit_code = 0;   
    int command_terminated = 0;

    // Parse command-line arguments.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        } else if (strcmp(argv[i], "--ip") == 0 && i + 1 < argc) {
            strncpy(server_ip, argv[i + 1], INET_ADDRSTRLEN);
            i++;
        } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            server_port = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--listen-duration") == 0 && i + 1 < argc) {
            listen_duration = atoi(argv[i + 1]);
            if (listen_duration <= 0) {
                fprintf(stderr, "ERR\tInvalid listen duration\n");
                exit(EXIT_ERR);
            }
            i++;
        } else if (strcmp(argv[i], "--force-listen") == 0) {
            force_listen = 1;
        } else if (strcmp(argv[i], "--debug") == 0) {
            debug_enabled = 1;
        } else if (strcmp(argv[i], "--bw-limit") == 0 && i + 1 < argc) {
            // parse bandwidth limit in bits/s
            g_bw_limit_bits = atof(argv[i + 1]);
            if (g_bw_limit_bits < 1.0) {
                // enforce minimal limit if invalid
                g_bw_limit_bits = 1.0;
            }
            i++;
        } else {
            fprintf(stderr, "ERR\tInvalid argument: %s\n", argv[i]);
            exit(EXIT_ERR);
        }
    }

    fprintf(stderr, "INFO\tStarting server on %s:%d for %d seconds\n",
            server_ip, server_port, listen_duration);
    fprintf(stderr, "INFO\tBandwidth limit: %.0f bits/s\n", g_bw_limit_bits);

    // Ensure directories for BIND and FLASH exist (created unconditionally).
    ensure_directory(BIND_DIR);
    ensure_directory(FLASH_DIR);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_ERR);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(server_fd);
        exit(EXIT_ERR);
    }

    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL) failed");
        close(server_fd);
        exit(EXIT_ERR);
    }
    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL, O_NONBLOCK) failed");
        close(server_fd);
        exit(EXIT_ERR);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(server_fd);
        exit(EXIT_ERR);
    }

    if (listen(server_fd, 5) == -1) {
        perror("Listening failed");
        close(server_fd);
        exit(EXIT_ERR);
    }

    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    while (1) {
        double diff = elapsed_time_sec(&start_time);
        if (diff >= listen_duration) {
            fprintf(stderr, "INFO\tListen duration expired\n");
            break;
        }
        if (command_terminated) {
            fprintf(stderr, "INFO\tA command requested termination\n");
            break;
        }

        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(100000);
                continue;
            } else {
                perror("Accept failed");
                usleep(100000);
                continue;
            }
        }
        fprintf(stderr, "INFO\tClient connected\n");

        {
            int client_flags = fcntl(client_fd, F_GETFL, 0);
            if (client_flags != -1) {
                client_flags &= ~O_NONBLOCK;
                fcntl(client_fd, F_SETFL, client_flags);
            }
        }

        FILE *client_file = fdopen(client_fd, "r+");
        if (!client_file) {
            perror("fdopen failed");
            close(client_fd);
            continue;
        }

        char *line = NULL;
        size_t linecap = 0;
        while (getline(&line, &linecap, client_file) != -1) {
	size_t len = strlen(line);
	/* Remove any trailing newline and carriage return characters */
	while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
	    line[len - 1] = '\0';
	    len--;
	}

            char *cmd = line;
            char *arg = NULL;
            char *sep = strpbrk(line, " \t");
            if (sep != NULL) {
                *sep = '\0';
                arg = sep + 1;
                while (*arg == ' ' || *arg == '\t')
                    arg++;
                if (*arg == '\0')
                    arg = NULL;
            }

            int ret = handle_command(cmd, arg, client_file, force_listen);
            if (ret != 0) {
                exit_code = ret;
                command_terminated = 1;
                break;
            }
        }
        free(line);
        fclose(client_file);
        fprintf(stderr, "INFO\tClient disconnected\n");

        if (command_terminated)
            break;
    }

    close(server_fd);
    exit(exit_code);
}
