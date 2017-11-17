#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

#define size 50

int stop = 0;

void interruptHandler(int sig) {
    if (sig == SIGINT) {
        stop = 1;
    }
}

void child1Handler(int sig) {
    if (sig == SIGUSR1) {
        printf("Child Process 1 is killed by parents\n");
        exit(0);
    }
}

void child2Handler(int sig) {
    if (sig == SIGUSR2) {
        printf("Child Process 2 is killed by parents\n");
        exit(0);
    }
}

int main() {
    int num = 0;
    char buf_send[size], buf_rcv[size];
    int pipefd[2];

    pid_t pid, pid1;

    pipe(pipefd);

    // parent process
    pid = fork();

    if (pid == 0) {
        signal(SIGINT, SIG_IGN);
        signal(SIGUSR1, child1Handler);
        // child process 1
        while (1) {
            sprintf(buf_send, "I send you %d times\n", num);
            num++;
            write(pipefd[1], buf_send, strlen(buf_send));
            sleep(1);
        }
    } else {
        pid1 = fork();
        if (pid1 == 0) {
            // child process 2
            signal(SIGINT, SIG_IGN);
            signal(SIGUSR2, child2Handler);

            while (1) {
                read(pipefd[0], buf_rcv, strlen(buf_rcv));
                printf("%s", buf_rcv);
                fflush(stdout);
            }
        } else {
            // parent process
            signal(SIGINT, interruptHandler);
            while(!stop);
            kill(pid1, SIGUSR2);
            kill(pid, SIGUSR1);
            close(pipefd[0]);
            close(pipefd[1]);
            sleep(1);
            printf("Parent Process is killed\n");
            exit(0);
        }
    }

    return 0;
}