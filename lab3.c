//
// Created by jerry on 17-11-17.
//

#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

int main() {

    // init semaphore
    unsigned int nsems = 1;
    sem_t s1;
    sem_init(&s1, 1, nsems);

    pid_t pid = fork();
    if (pid == -1) {
        printf("Fork failed");
        return 1;
    } else if (pid == 0) {
        // child process

    } else {
        // parent process

    }
}