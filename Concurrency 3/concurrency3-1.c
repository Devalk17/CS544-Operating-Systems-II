// Author: Deval Kaku
// ONID: 933342527
// Concurrency 3: Part 1
// CS544 Operating Systems 2 Spring 2018

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

pthread_mutex_t mutex_1, mutex_2;

int current = 0;
int random_func(int min, int max) {
    return rand() % (max + 1 - min) + min;
}


void *processes_func() {
    while (1) {
        int exec_time = 0;
        pthread_mutex_lock(&mutex_1);
        // Creating new proocess
        exec_time = random_func(1, 7);
        current++;
        printf("Running time for new process is %d seconds. Total number of process is %d.\n", exec_time, current);

        // to check the number of processes
        if (current == 3) {
            printf("Wait for three processes to finish.\n");
        } else {
          pthread_mutex_unlock(&mutex_1);
        }

        // proccessing period
        sleep(exec_time);
        printf("Number %d process is finished.\n", current--);

        pthread_mutex_lock(&mutex_2);
        // to make sure all three procceses are finished before new proccess
        if (current == 0) {
            printf("All three finished. Available for new proccess\n");
            pthread_mutex_unlock(&mutex_1);
        }
        exec_time = random_func(2, 6);
        pthread_mutex_unlock(&mutex_2);
        sleep(exec_time);
    }
}



int main() {
    srand(time(NULL));

    // initializing threads
    pthread_t thread_array[3];
    pthread_mutex_init(&mutex_1, NULL);
    pthread_mutex_init(&mutex_2, NULL);

    // create threads
    int i;
    for (i = 0; i < 3; i++) {
        pthread_create(&thread_array[i], NULL, processes_func, NULL);
    }

    // wait for created thread to exit
    for (i = 0; i < 3; i++) {
        pthread_join(thread_array[i], NULL);
    }

    return 0;
}