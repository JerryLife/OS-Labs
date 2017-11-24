//
// Created by jerry on 17-11-17.
//

#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <tkPort.h>
#include <sys/shm.h>

#define BUF_SIZE 20
#define MEM_SIZE 1024

int main() {

    // init semaphore
    sem_t *space = mmap(NULL, sizeof *space, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t *data = mmap(NULL, sizeof *data, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(space, 1, BUF_SIZE);      // space resource
    sem_init(data, 1, 0);              // data resource

    key_t key = 12345;
    int shm_id = shmget(key, MEM_SIZE * BUF_SIZE, 0666 | IPC_CREAT);
    void *buf = shmat(shm_id, NULL, 0);

    pid_t pid = fork();
    if (pid == -1) {
        printf("Fork failed\n");
        return 1;
    } else if (pid == 0) {
        // write process
        char* fileName = "../example.png";
        int write_id = 0;
        size_t fileSize;

        FILE* hWrite = fopen(fileName, "rb");
        if (hWrite == NULL) {
            printf("Cannot open file \"%s\"\n", fileName);
            exit(0);
        }
        fseek(hWrite, 0, SEEK_END); // seek to end of file
        fileSize = (size_t)ftell(hWrite); // get current file pointer
        fseek(hWrite, 0, SEEK_SET);

        // get size of the file
        printf("Start writing memory\n");
        size_t rst = 0;
        void *read_buf = malloc(MEM_SIZE);

        // send size of a file
        sem_wait(space);
        memcpy(buf, &fileSize, sizeof(size_t));
        sem_post(data);
        printf("Size of file sent is %ld\n", fileSize);

        write_id = 1;
        size_t send_size = 0;
        while (1) {
            sem_wait(space);                   // p(space)
            rst = fread(read_buf, 1, MEM_SIZE, hWrite);
            if (fileSize - send_size <= MEM_SIZE) {
                memcpy(buf + write_id * MEM_SIZE, read_buf, fileSize - send_size);
                printf("All %ld sent\n", fileSize);
                sem_post(data);
                break;
            }
            else memcpy(buf + write_id * MEM_SIZE, read_buf, MEM_SIZE);
            sem_post(data);                   // v(data)
            send_size += MEM_SIZE;
            printf("%ld sent to buf[%d]\n", send_size, write_id);
            printf("Send bits: %lx\n", *(long*)read_buf);
            fseek(hWrite, send_size, SEEK_SET);
            write_id = (write_id + 1) % (BUF_SIZE);
        }
        printf("Finish writing memory\n");
        fclose(hWrite);
    } else {
        // read process
        char* newFileName = "../new.png";
        int read_id = 0;
        size_t rcvFileSize;

        FILE *hRead = fopen(newFileName, "ab");
        if (hRead == NULL) {
            printf("Cannot open file\"%s\"\n", newFileName);
            exit(0);
        }

        sem_wait(data);     // pdata
        memcpy(&rcvFileSize, buf, sizeof(size_t));
        sem_post(space);
        printf("Expected file size is %ld\n", rcvFileSize);

        printf("Start reading memory\n");
        read_id = 1;
        size_t rcv_size = 0;
        while (1) {
            sem_wait(data);                    // p(data)
            if (rcvFileSize - rcv_size <= MEM_SIZE) {
                fwrite(buf + read_id * MEM_SIZE, 1, rcvFileSize - rcv_size, hRead);
                printf("All %ld received\n", rcvFileSize);
                sem_post(space);                // v(space)
                break;
            }
            else fwrite(buf + read_id * MEM_SIZE, 1, MEM_SIZE, hRead);
            sem_post(space);                   // v(space)
            rcv_size += MEM_SIZE;
            printf("%ld received from buf[%d]\n", rcv_size, read_id);
            read_id = (read_id + 1) % BUF_SIZE;
        }
        printf("Finish reading memory\n");

        fclose(hRead);
    }

    sem_close(data);
    sem_close(space);
}