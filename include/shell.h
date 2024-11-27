#include <unistd.h>

/** Obtiene e imprime: usuario@hostname:directorio_actual$ */
void get_prompt(char* prompt, size_t size);

void change_directory(char* path);

void clear_screen();

void handle_signal(int signal);

void execute_pipe_commands(char* commands[], int n_commands, int background);

void expand_env_variables(char* command);

void execute_command(char* command, int background);
