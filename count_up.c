#include <stdio.h>
#include <unistd.h>  // For the sleep() function

int main() {
    for (int i = 0; i <= 10; i++) {
        printf("%d\n", i);
        sleep(1);  // Pause for 1 second
    }
    return 0;
}