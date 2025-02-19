#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  TCP mode: %s <port> <address> <command> [args...]\n", argv[0]);
        fprintf(stderr, "  UDP mode: %s --udp <port> <command> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Check for UDP mode
    if (strcmp(argv[1], "--udp") == 0) {
        // UDP mode: argv[2] is the port, argv[3] is the command
        int port = atoi(argv[2]);

        // Create UDP socket
        int udp_sock;
        if ((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("UDP socket creation failed");
            exit(EXIT_FAILURE);
        }
        int opt = 1;
        if (setsockopt(udp_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            perror("setsockopt SO_REUSEADDR failed");
            close(udp_sock);
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in udp_addr;
        memset(&udp_addr, 0, sizeof(udp_addr));
        udp_addr.sin_family = AF_INET;
        udp_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to 0.0.0.0
        udp_addr.sin_port = htons(port);

        if (bind(udp_sock, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0) {
            perror("UDP bind failed");
            close(udp_sock);
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "Listening for UDP packets on 0.0.0.0:%d\n", port);

        // Create pipes for redirecting child's stdin and capturing child's stdout.
        // pipe_in: parent writes UDP data -> child's stdin.
        // pipe_out: child writes stdout -> parent reads and prints.
        int pipe_in[2], pipe_out[2];
        if (pipe(pipe_in) < 0) {
            perror("pipe for stdin failed");
            close(udp_sock);
            exit(EXIT_FAILURE);
        }
        if (pipe(pipe_out) < 0) {
            perror("pipe for stdout failed");
            close(udp_sock);
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            close(udp_sock);
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            // Close the parent's ends of the pipes.
            close(pipe_in[1]);
            close(pipe_out[0]);

            // Replace child's stdin with pipe_in[0]
            if (dup2(pipe_in[0], STDIN_FILENO) < 0) {
                perror("dup2 for stdin failed");
                exit(EXIT_FAILURE);
            }
            // Replace child's stdout with pipe_out[1]
            if (dup2(pipe_out[1], STDOUT_FILENO) < 0) {
                perror("dup2 for stdout failed");
                exit(EXIT_FAILURE);
            }
            // Optionally also send stderr to stdout.
            dup2(pipe_out[1], STDERR_FILENO);

            // Close the now duplicated descriptors.
            close(pipe_in[0]);
            close(pipe_out[1]);

            // Execute the requested command.
            // In UDP mode, the command starts at argv[3].
            execvp(argv[3], &argv[3]);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        // Parent process: close the child's ends of the pipes.
        close(pipe_in[0]);
        close(pipe_out[1]);

        // Use select() to multiplex between the UDP socket and the child's output.
        fd_set readfds;
        int maxfd = (udp_sock > pipe_out[0]) ? udp_sock : pipe_out[0];

        while (1) {
            FD_ZERO(&readfds);
            FD_SET(udp_sock, &readfds);
            FD_SET(pipe_out[0], &readfds);

            int ret = select(maxfd + 1, &readfds, NULL, NULL, NULL);
            if (ret < 0) {
                if (errno == EINTR)
                    continue;
                perror("select failed");
                break;
            }

            // Check for incoming UDP packets.
            if (FD_ISSET(udp_sock, &readfds)) {
                char buffer[4096];
                struct sockaddr_in sender_addr;
                socklen_t sender_len = sizeof(sender_addr);
                ssize_t recv_len = recvfrom(udp_sock, buffer, sizeof(buffer), 0,
                                            (struct sockaddr *)&sender_addr, &sender_len);
                if (recv_len < 0) {
                    perror("recvfrom failed");
                } else {
                    // Forward the UDP data to the child's stdin.
                    ssize_t written = write(pipe_in[1], buffer, recv_len);
                    if (written < 0) {
                        perror("write to child's stdin failed");
                    }
                }
            }

            // Check if there is output from the child.
            if (FD_ISSET(pipe_out[0], &readfds)) {
                char outbuf[4096];
                ssize_t count = read(pipe_out[0], outbuf, sizeof(outbuf));
                if (count < 0) {
                    perror("read from child's stdout failed");
                } else if (count == 0) {
                    // End-of-file: child closed its stdout.
                    break;
                } else {
                    // Print child's output to stdout.
                    if (write(STDOUT_FILENO, outbuf, count) < 0) {
                        perror("write to stdout failed");
                    }
                }
            }
        }

        // Cleanup: close file descriptors and wait for child termination.
        close(pipe_in[1]);
        close(pipe_out[0]);
        close(udp_sock);
        wait(NULL);
        exit(EXIT_SUCCESS);
    } else {
        // TCP mode: Expect arguments: <port> <address> <command> [args...]
        if (argc < 4) {
            fprintf(stderr, "Usage: %s <port> <address> <command> [args...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        int port = atoi(argv[1]);
        int sock_fd, conn_fd;
        struct sockaddr_in server_addr, peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);

        if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }

        int opt = 1;
        setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(argv[2]);
        server_addr.sin_port = htons(port);

        if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("bind failed");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        if (listen(sock_fd, 1) < 0) {
            perror("listen failed");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "Waiting for connection on %s:%d\n", argv[2], port);

        if ((conn_fd = accept(sock_fd, (struct sockaddr *)&peer_addr, &peer_addr_len)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "Connection accepted\n");

        close(sock_fd);
        // Replace stdin and stdout with the accepted connection.
        dup2(conn_fd, STDIN_FILENO);
        dup2(conn_fd, STDOUT_FILENO);
        close(conn_fd);

        execvp(argv[3], argv + 3);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
}
