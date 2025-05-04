/**
 * Based on a project from @conorbros
 */

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>

#define DEFAULT_PATH "/usr/share/openipc"

char **paths;
size_t paths_c = 0;

/**
 * @brief Adds a path to the list of paths to look for executables in
 * 
 * @param path 
 */
void add_path(char *path)
{
    paths[paths_c] = (char *)malloc(sizeof(char) * strlen(path) + 1);
    strcpy(paths[paths_c++], path);
    paths = realloc(paths, (paths_c + 1) * sizeof(char *));
}

/**
 * @brief Initializes the list of paths and adds the DEFAULT_PATH
 * 
 */
void init_paths()
{
    paths = (char **)malloc(sizeof(char *) * 2);
    paths[paths_c] = (char *)malloc(sizeof(char) * strlen(DEFAULT_PATH) + 1);
    strcpy(paths[paths_c++], DEFAULT_PATH);
}

/**
 * @brief Empties the list of paths and frees memory
 * 
 */
void clear_paths()
{
    paths = realloc(paths, sizeof(char *));
    paths_c = 0;
}

void free_paths()
{
    for (int i = 0; i < paths_c; i++)
    {
        free(paths[i]);
    }
    free(paths);
}

/**
 * @brief Frees all memory in a heap allocated char array
 * 
 * @param arr array to free
 * @param c count of elements in array
 */
void free_char_arr(char **arr, size_t c)
{
    for (size_t i = 0; i < c; i++)
    {
        free(arr[i]);
    }
    free(arr);
}

/**
 * @brief Prints an error msg to stderr
 * 
 */
void print_err()
{
    char error_message[30] = "Sorry, unknown command\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

/**
 * @brief Searches the shell's current paths for an executable matching the cmd
 * 
 * @param cmd 
 * @return char* full path of the executable
 */
char *search_paths(char *cmd)
{
    char *res = NULL;
    for (size_t i = 0; i < paths_c; i++)
    {
        size_t len = strlen(cmd) + strlen(paths[i]) + 3;
        char *full_path = (char *)malloc(sizeof(char) * len);
        sprintf(full_path, "%s/%s", paths[i], cmd);

        if (access(full_path, X_OK) == 0)
        {
            res = (char *)malloc(sizeof(char) * (strlen(full_path) + 1));
            strcpy(res, full_path);
            free(full_path);
            break;
        }
        free(full_path);
    }
    return res;
}

/**
 * @brief Executes the command with arguments
 * 
 * @param cmd name of binary to execute
 * @param args arguments to pass to executed program
 * @param arg_c count of argument array
 * @param redirect file to redirect stdout and stderr to (can be NULL)
 * @return pid_t pid of process running the command
 */
pid_t exec_cmd(char *cmd, char **args, int arg_c, char *redirect)
{
    char *path = search_paths(cmd);
    if (path == NULL)
    {
        print_err();
        return 0;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0)
    {
        if (redirect != NULL)
        {
            FILE *redirect_file;
            redirect_file = fopen(redirect, "w");
            if (redirect_file == NULL)
            {
                print_err();
                exit(1);
            }
            dup2(fileno(redirect_file), STDOUT_FILENO);
            dup2(fileno(redirect_file), STDERR_FILENO);
        }
        if (execv(path, args) == -1)
        {
            perror("execv");
            exit(1);
        }
    }
    free(path);
    return pid;
}
/**
 * @brief Gets the path to redirect output to. Will assign NULL to char array if no redirect specified. Function will return 0 for no errors, 1 if there is errors;
 * 
 * @param str 
 * @param redirect
 * @return int 0 for fine, 1 for errors
 */
int get_redirect_file(char *str, char **redirect)
{
    size_t o = 0;
    int res = 0;
    size_t len = strlen(str) + 1;
    bool has_output = false;
    int token_index = -1;

    for (int i = 0; i < len; i++)
    {

        if (str[i] == '>' && i == 0)
        {
            return 1;
        }
        else if (str[i] == '>')
        {
            token_index = i;
            has_output = true;
            *redirect = (char *)malloc(sizeof(char) * (len));
            if (str[i + 1] == ' ') // If user has put a space in between '>' and the filename, move to first char in filename
            {
                i++;
            }
            continue;
        }

        if (has_output && o > 1 && str[i] == ' ')
        {
            free(*redirect);
            *redirect = NULL;

            print_err();
            exit(0);
        }
        else if (has_output)
        {
            (*redirect)[o] = str[i];
            o++;
        }
    }
    if (token_index != -1)
    {
        str[token_index] = 0;
    }

    return res;
}

/**
 * @brief Parses and runs a string containing a cmd and optional parameters
 * 
 * @param cmd_str 
 * @return pid_t pid of process running command
 */
pid_t handle_cmd(char *cmd_str)
{
    pid_t res = 0;
    char *redirect = NULL;
    if (get_redirect_file(cmd_str, &redirect) != 0)
    {
        print_err();
        return 1;
    }

    char *cmd = NULL;
    cmd = strtok(cmd_str, " ");
    if (cmd == NULL)
    {
        return res;
    }

    char **args = (char **)malloc(sizeof(char *) * 2);
    args[0] = strdup(cmd);
    size_t args_n = 1;

    char *token = NULL;
    token = strtok(NULL, " ");
    while (token != NULL)
    {
        args[args_n] = (char *)malloc(sizeof(char) * strlen(token) + 1);
        strcpy(args[args_n++], token);

        args = (char **)realloc(args, (args_n + 1) * sizeof(char *));
        token = strtok(NULL, " ");
    }

    // null terminate the arg array
    args = realloc(args, (args_n + 1) * sizeof(char *));
    args[args_n++] = NULL;

    if (strcmp(cmd, "cd") == 0) // Check if commands are built-in commands
    {

        if (args_n != 3) // If there is less 2 args (program name[0] and directory[1]) cannot run
        {
            print_err();
            res = 0;
        }
        else
        {
            if (chdir(args[1]) == -1)
            {
                print_err();
            }
            res = 0;
        }
    }
    else if (strcmp(cmd, "path") == 0)
    {
        clear_paths();

        for (size_t i = 1; i < args_n - 1; i++) // -1 because arg array is null terminated
        {
            add_path(args[i]);
        }
        res = 0;
    }
    else // Else the command is a standard one
    {
        res = exec_cmd(cmd, args, args_n, redirect);
    }

    free_char_arr(args, args_n);
    free(token);

    return res;
}

int handle_line(char *line)
{
    if (strcmp(line, "exit") == 0)
    {
        return 1;
    }

    char *buffer = NULL;
    char **cmds = (char **)malloc(sizeof(char *) * 2);

    buffer = strtok(line, "&");
    size_t cmds_c = 0;

    while (buffer != NULL)
    {

        if (buffer[strlen(buffer) - 1] == ' ') // If last char is a space remove it
        {
            buffer[strlen(buffer) - 1] = 0;
        }

        if (buffer[0] == ' ') // If first char is a space remove it
        {
            buffer++;
        }

        // Put string containing cmd and parameters into array
        cmds[cmds_c] = (char *)malloc(sizeof(char) * strlen(buffer) + 1);
        strcpy(cmds[cmds_c++], buffer);
        cmds = (char **)realloc(cmds, (cmds_c + 1) * sizeof(char *));
        buffer = strtok(NULL, "&");
    }

    // Start all processes with `handle_cmd` function
    for (size_t i = 0; i < cmds_c; i++)
    {
        handle_cmd(cmds[i]);
    }

    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0) // Wait for all child processes to finish before continuing
    {
    }

    free_char_arr(cmds, cmds_c);
    return 0;
}

void interactive_mode()
{
    char *buffer = NULL;
    size_t len = 0;
    while (1)
    {
        printf("keychain:~/jail> ");
        fflush(stdout);

        getline(&buffer, &len, stdin);
        if (strcmp(buffer, "exit\n") == 0)
        {
            break;
        }

        if (buffer[strlen(buffer) - 1] == '\n') // Replace the newline with a null char
        {
            buffer[strlen(buffer) - 1] = 0;
        }

        if (handle_line(buffer) == 1)
        {
            break;
        }
    }
    free(buffer);
}

void batch_mode(FILE *fp)
{
    char *buffer = NULL;
    size_t len = 0;
    while (getline(&buffer, &len, fp) > 0)
    {
        if (buffer[strlen(buffer) - 1] == '\n') // Replace the newline with a null char
        {
            buffer[strlen(buffer) - 1] = 0;
        }

        if (handle_line(buffer) == 1)
        {
            break;
        }
    }

    free(buffer);
}

int main(int argc, char *argv[])
{
    FILE *input;

    // Init the array of paths that
    // the shell with look for executables in
    init_paths();

    if (argc > 1) // If arg is supplied use batch mode
    {
        input = fopen(argv[1], "r");
        if (input == NULL || argc > 2) // If the input file doesn't exist or multiple input file arguments were given
        {
            print_err();
            exit(1);
        }

        batch_mode(input);
    }
    else // else use interactive mode
    {
        interactive_mode();
    }

    free_paths();
    exit(0);
}
