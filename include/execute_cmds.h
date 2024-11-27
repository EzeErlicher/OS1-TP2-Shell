
#include <stdlib.h>

void handle_signal(int signal);

void execute_pipe_commands(char* commands[], int n_commands, int background);

// Function to expand environment variables
void expand_env_variables(char* command);

// Modify execute_command to include environment variable expansion
void execute_command(char* command, int background);