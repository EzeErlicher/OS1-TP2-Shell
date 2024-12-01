#include "shell_built_in_cmds.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

void get_prompt(char* prompt, size_t size)
{
    struct passwd* pw = getpwuid(getuid());
    const char* user = pw->pw_name;

    char hostname[BUFFER_SIZE / 4];
    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        perror("Error getting hostname");
        return;
    }

    char cwd[BUFFER_SIZE];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error getting current working directory");
        return;
    }

    snprintf(prompt, size, "%s@%s:%s$ ", user, hostname, cwd);
}

void change_directory(char* path)
{
    char current_dir[BUFFER_SIZE];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror("Error getting current working directory");
        return;
    }

    if (path == NULL || strcmp(path, "") == 0)
    {
        printf("Error: No se especificó un directorio.\n");
        return;
    }
    else if (strcmp(path, "-") == 0)
    {
        char* oldpwd = getenv("OLDPWD");
        if (oldpwd)
        {
            if (chdir(oldpwd) == -1)
            {
                perror("Error changing directory to OLDPWD");
                return;
            }
            printf("%s\n", oldpwd);
            return;
        }
        else
        {
            printf("Error: OLDPWD no está definido.\n");
            return;
        }
    }

    if (chdir(path) == 0)
    {
        setenv("OLDPWD", current_dir, 1);
        char new_dir[BUFFER_SIZE];
        if (getcwd(new_dir, sizeof(new_dir)) == NULL)
        {
            perror("Error getting new working directory");
            return;
        }
        setenv("PWD", new_dir, 1);
    }
    else
    {
        perror("Error al cambiar de directorio");
    }
}

void clear_screen()
{
    // Ignore the return value of the escape sequence
    (void)printf("\033[H\033[J"); // Escape sequence to clear screen
}

// Function to read and display configuration files
void show_config_file(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening configuration file");
        return;
    }

    printf("Contenido de %s:\n", file_path);
    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline if present
        line[strcspn(line, "\n")] = 0;
        printf("%s\n", line);
    }
    fclose(file);
}

// Recursive function to explore the directory and search for .config or .json files
void explore_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        // Skip . and .. directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Full path to the file or directory
        char full_path[BUFFER_SIZE];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat statbuf;
        if (stat(full_path, &statbuf) == 0) {
            // If it's a directory, recurse
            if (S_ISDIR(statbuf.st_mode)) {
                explore_directory(full_path);
            } else {
                // If it's a file, check for .config or .json extensions
                if (strstr(entry->d_name, ".config") || strstr(entry->d_name, ".json")) {
                    printf("Archivo de configuración encontrado: %s\n", full_path);
                    show_config_file(full_path);
                }
            }
        }
    }
    closedir(dir);
}

// Command to start the exploration of a given directory
void explore_config_files(const char *dir_path) {
    printf("Explorando el directorio: %s en busca de archivos '.config' o '.json'\n", dir_path);
    explore_directory(dir_path);
}