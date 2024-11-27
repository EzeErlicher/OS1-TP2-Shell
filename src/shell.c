#include "shell.h"
#include "execute_cmds.h"
#include "monitor_cmds.h"
#include "shell_built_in_cmds.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PIPE_PATH "/tmp/monitor_pipe"
#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    char prompt[BUFFER_SIZE];
    FILE* input_file = NULL;

    // Abre el pipe para lectura
    if (mkfifo(PIPE_PATH, 0666) == -1 && errno != EEXIST) {
        perror("Failed to create FIFO");
        exit(EXIT_FAILURE);
    }

    int pipe_fd = open(PIPE_PATH, O_RDONLY | O_NONBLOCK); // Non-blocking read

    // Signal handlers
    signal(SIGINT, handle_signal);
    signal(SIGTSTP, handle_signal);
    signal(SIGQUIT, handle_signal);

    if (argc > 1) {
        input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
            perror("Error al abrir el archivo de comandos");
            return 1;
        }
    }

    while (1) {
        char command[BUFFER_SIZE];

        // Lee del archivo pasado como argumento
        if (input_file) {
            if (fgets(command, sizeof(command), input_file) == NULL) {
                break; // End of file
            }
        } else {
            // solo imprime el prompt si no se esta leyendo un archivo
            if (isatty(fileno(stdin)) && isatty(fileno(stdout))) { // Checkea si stdin and stdout son terminales
                get_prompt(prompt, sizeof(prompt));
                usleep(100000);
                printf("%s", prompt);
            }

            if (fgets(command, sizeof(command), stdin) == NULL) {
                break;
            }
        }

        command[strcspn(command, "\n")] = 0; // se elimina newline character

        int background = 0;
        if (command[strlen(command) - 1] == '&') {
            background = 1;
            command[strlen(command) - 1] = '\0'; // se elimina '&'
        }

        // Built-in commands
        if (strncmp(command, "cd ", 3) == 0) {
            change_directory(command + 3);
        } else if (strcmp(command, "cd") == 0) {
            change_directory("");
        } else if (strcmp(command, "clr") == 0) {
            clear_screen();
        } else if (strcmp(command, "quit") == 0) {
            break;
        } else if (strcmp(command, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(command, "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(command, "status_monitor") == 0) {
            status_monitor();
        } else if (strncmp(command, "explore_config ", 16) == 0) {
            // Extract the directory from the command and call the explore function
            explore_config_files(command + 16);
        } else {
            execute_command(command, background);
        }
    }

    if (input_file) {
        fclose(input_file);
    }

    unlink(PIPE_PATH);

    printf("Cerrando shell...\n");
    return 0;
}
