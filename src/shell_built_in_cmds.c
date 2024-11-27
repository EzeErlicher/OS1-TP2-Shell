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
