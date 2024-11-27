#include "shell_built_in_cmds.h"
#include "unity.h"
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Set up and tear down functions (optional, but useful if setup or cleanup is needed)
void setUp(void)
{
    // Runs before each test
}

void tearDown(void)
{
    // Runs after each test
}

// Test for get_prompt function
void test_get_prompt(void)
{
    char prompt[1024];
    get_prompt(prompt, sizeof(prompt));

    struct passwd* pw = getpwuid(getuid());
    const char* user = pw->pw_name;

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        perror("Error getting hostname");
    }

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error getting current working directory");
    }

    char expected_prompt[2048];
    snprintf(expected_prompt, sizeof(expected_prompt), "%s@%s:%s$ ", user, hostname, cwd);

    TEST_ASSERT_EQUAL_STRING(expected_prompt, prompt);
}

// Test change_directory function with valid path
void test_change_directory_to_home(void)
{
    char* home_dir = getenv("HOME");
    TEST_ASSERT_NOT_NULL_MESSAGE(home_dir, "HOME environment variable is not set");

    change_directory(home_dir);

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error getting current working directory");
    }
    TEST_ASSERT_EQUAL_STRING(home_dir, cwd);
}

// Test change_directory function with invalid path
void test_change_directory_invalid_path(void)
{
    change_directory("/nonexistent_path");

    // Test if the current directory remains unchanged (stays at previous directory)
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error getting current working directory");
    }
    char* expected_dir = getenv("PWD");
    TEST_ASSERT_EQUAL_STRING(expected_dir, cwd);
}

// Test change_directory with "-" argument to go to OLDPWD
void test_change_directory_to_oldpwd(void)
{
    char original_dir[1024];
    if (getcwd(original_dir, sizeof(original_dir)) == NULL)
    {
        perror("Error getting current working directory");
    }

    // Change to home and then back to original
    change_directory(getenv("HOME"));
    setenv("OLDPWD", original_dir, 1); // manually set OLDPWD to the original directory
    change_directory("-");

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error getting current working directory");
    }
    TEST_ASSERT_EQUAL_STRING(original_dir, cwd);
}

// Test clear_screen function
void test_clear_screen(void)
{
    // Redirect stdout to a buffer
    FILE* fp = freopen("/tmp/test_output", "w+", stdout);
    TEST_ASSERT_NOT_NULL_MESSAGE(fp, "Failed to redirect stdout");

    clear_screen();

    fflush(stdout);         // flush the output to file
    fseek(fp, 0, SEEK_SET); // move to start of the file

    // Read and check content for ANSI clear screen sequence
    char buffer[16];
    if (fread(buffer, sizeof(char), 16, fp) == 0)
    {
        perror("Error reading from file");
    }
    TEST_ASSERT_EQUAL_STRING("\033[H\033[J", buffer);

    // Close and reset stdout
    fclose(fp);
    if (freopen("/dev/tty", "w", stdout) == NULL)
    {
        perror("Failed to reset stdout");
    }
}

// Main function to run all tests
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_get_prompt);
    RUN_TEST(test_change_directory_to_home);
    RUN_TEST(test_change_directory_invalid_path);
    RUN_TEST(test_change_directory_to_oldpwd);
    RUN_TEST(test_clear_screen);
    return UNITY_END();
}
