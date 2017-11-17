//
// Created by jerry on 17-11-17.
//

#include <iostream>
#include <thread>
#include<sys/sem.h>
#include<semaphore.h>

using namespace std;

static volatile int a = 0;
static volatile bool stop = false;
static volatile int num = 0;

// define a semaphore
key_t key = 2333;
int nsems = 1;
sem_t s1, s2;
int ret1 = sem_init(&s1, 0, 1);
int ret2 = sem_init(&s1, 0, 1);

void add() {
    while (num++ < 100) {
        sem_wait(&s1);
        a += num;
        cout << num << endl;
        this_thread::sleep_for(chrono::milliseconds(20));
        sem_post(&s2);
    }
    stop = true;
}

void print() {
    while (!stop) {
        sem_wait(&s2);
        cout << a << endl;
        this_thread::sleep_for(chrono::milliseconds(20));
        sem_post(&s1);
    }
}

int main() {
    thread thread_add(&add);
    thread thread_print(&print);

    thread_add.join();
    thread_print.join();

    return 0;
}