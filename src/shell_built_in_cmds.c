
#include "shell_built_in_cmds.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void get_prompt(char* prompt, size_t size)
{
    struct passwd* pw = getpwuid(getuid());
    const char* user = pw->pw_name;

    char hostname[BUFFER_SIZE / 4];
    gethostname(hostname, sizeof(hostname));

    char cwd[BUFFER_SIZE];
    getcwd(cwd, sizeof(cwd));

    snprintf(prompt, size, "%s@%s:%s$ ", user, hostname, cwd);
}

void change_directory(char* path)
{
    char current_dir[BUFFER_SIZE];
    getcwd(current_dir, sizeof(current_dir));

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
            chdir(oldpwd);
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
        getcwd(new_dir, sizeof(new_dir));
        setenv("PWD", new_dir, 1);
    }
    else
    {
        perror("Error al cambiar de directorio");
    }
}

void clear_screen()
{
    printf("\033[H\033[J"); // Escape sequence para limpiar la pantalla
}