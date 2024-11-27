#include "execute_cmds.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PIPE_PATH "/tmp/monitor_pipe"
#define BUFFER_SIZE 1024

int job_id = 1;    // Contador para IDs de trabajos en segundo plano
pid_t fg_pid = -1; // PID de el proceso en primer plano actual(foreground)

void handle_signal(int signal)
{
    if (fg_pid > 0)
    {
        kill(fg_pid, signal); // envía la señal al proceso en primer plano

        // retorna si la señal es SIGTSTP para que la shell pueda seguir ejectuando comandos
        if (signal == SIGTSTP)
        {
            return;
        }
    }
}

void execute_pipe_commands(char* commands[], int n_commands, int background)
{
    int pipe_fds[2];
    int in_fd = 0; // Input inicial para el primer comando (stdin)
    pid_t pid;

    for (int i = 0; i < n_commands; i++)
    {
        // Crea un pipe para el comando actual, excepto el último
        if (i < n_commands - 1 && pipe(pipe_fds) == -1)
        {
            perror("Error al crear el pipe");
            return;
        }

        // Fork
        if ((pid = fork()) == 0)
        {
            // Redirige el input
            if (in_fd != 0)
            {
                dup2(in_fd, 0);
                close(in_fd);
            }

            // Redirige el output excepto para el último comando
            if (i < n_commands - 1)
            {
                dup2(pipe_fds[1], 1);
                close(pipe_fds[0]);
                close(pipe_fds[1]);
            }

            // Prepara los argumentos para execvp
            char* args[BUFFER_SIZE];
            char* token = strtok(commands[i], " ");
            int j = 0;
            while (token != NULL && j < 1024 - 1)
            {
                args[j++] = token;
                token = strtok(NULL, " ");
            }
            args[j] = NULL;

            if (execvp(args[0], args) == -1)
            {
                perror("Error al ejecutar el programa");
                exit(EXIT_FAILURE);
            }
        }
        else if (pid < 0)
        {
            perror("Error al crear el proceso hijo");
            return;
        }
        else
        {
            if (background && i == n_commands - 1)
            {
                printf("[%d] %d\n", job_id++, pid);
            }
            else
            {
                fg_pid = pid; // actualiza el PID del proceso en primer plano
            }

            // Cierra los file descriptors usados
            close(pipe_fds[1]);
            in_fd = pipe_fds[0];
        }
    }

    if (!background)
    {
        waitpid(pid, NULL, 0);
        fg_pid = -1;
    }
}

// Función para expandir variables de entorno
void expand_env_variables(char* command)
{
    char expanded_command[BUFFER_SIZE] = {0};
    char* token = strtok(command, " ");
    char* var;

    while (token != NULL)
    {
        if (token[0] == '$')
        {
            var = token + 1;           // Skip the '$'
            char* value = getenv(var); // Obtiene el valor de la variable
            if (value != NULL)
            {
                strcat(expanded_command, value);
            }
        }
        else
        {
            strcat(expanded_command, token);
        }

        strcat(expanded_command, " "); // añade espacios entre tokens
        token = strtok(NULL, " ");
    }

    // quita los trailing spaces
    expanded_command[strlen(expanded_command) - 1] = '\0';
    strcpy(command, expanded_command); // copia al comando original
}

void execute_command(char* command, int background)
{
    expand_env_variables(command);

    char* pipe_commands[BUFFER_SIZE];
    int n_commands = 0;
    char* token = strtok(command, "|");

    while (token != NULL && n_commands < 1024)
    {
        pipe_commands[n_commands++] = token;
        token = strtok(NULL, "|");
    }

    if (n_commands > 1)
    {
        execute_pipe_commands(pipe_commands, n_commands, background);
        return;
    }

    // Handling redirection
    char* input_file = NULL;
    char* output_file = NULL;
    char* args[BUFFER_SIZE];
    int i = 0;

    // Chequea por redirección de input
    char* redirect_in = strchr(command, '<');
    if (redirect_in)
    {
        *redirect_in = '\0';                       // Termina el comando en NULL
        input_file = strtok(redirect_in + 1, " "); // Obtiene el nombre del archivo input
    }

    // Chequea por redirección de output
    char* redirect_out = strchr(command, '>');
    if (redirect_out)
    {
        *redirect_out = '\0';
        output_file = strtok(redirect_out + 1, " ");
    }

    // Prepare arguments for execvp
    token = strtok(command, " ");
    while (token != NULL && i < 1024 - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == 0)
    {
        if (input_file)
        {
            int in_fd = open(input_file, O_RDONLY);
            if (in_fd == -1)
            {
                perror("Error al abrir el archivo de entrada");
                exit(EXIT_FAILURE);
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        if (output_file)
        {
            int out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd == -1)
            {
                perror("Error al abrir el archivo de salida");
                exit(EXIT_FAILURE);
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        if (execvp(args[0], args) == -1)
        {
            perror("Error al ejecutar el programa");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid < 0)
    {
        perror("Error al crear el proceso hijo");
    }
    else
    {
        if (background)
        {
            printf("[%d] %d\n", job_id++, pid);
        }
        else
        {
            fg_pid = pid; // Setea PID en primer plano
            waitpid(pid, NULL, 0);
            fg_pid = -1; // limpia PID de primer plano cuando el proceso se completa
        }
    }
}