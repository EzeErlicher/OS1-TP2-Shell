#include <stdio.h>

void get_prompt(char* prompt, size_t size);

void change_directory(char* path);

void clear_screen();

// Function to read and display configuration files
void show_config_file(const char *file_path);

// Recursive function to explore the directory and search for .config or .json files
void explore_directory(const char *dir_path);

// Command to start the exploration of a given directory
void explore_config_files(const char *dir_path);