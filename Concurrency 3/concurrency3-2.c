// Author: Deval Kaku
// ONID: 933342527
// Concurrency 3: Part 1
// CS544 Operating Systems 2 Spring 2018
// Reference: https://github.com/quinnsam/cs444/blob/master/concurrency3/con3.c


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t mutex_insert;
pthread_mutex_t mutex_search;
pthread_mutex_t mutex_delete;

struct single_link_list {
    int num;
    struct single_link_list *next;
}*head;

// ==================================================
int random_func(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

int count_element() {
    struct single_link_list *temp;
    int i = 0;

    temp = head;
    while (temp != NULL) {
        temp = temp->next;
        i++;
    }
    return i;
}

void inserter () {
    struct single_link_list *temp, **tail;
    int new_value;
    while (1) {
        if (!pthread_mutex_lock(&mutex_insert)) {
            temp = (struct single_link_list *)malloc(sizeof(struct single_link_list));
            new_value = rand() % 9 + 1;

            printf("Insert: %d\n", new_value);
            temp->num = new_value;
            temp->next = NULL;
            tail = &head;
            if (head == NULL) {
                head = temp;
            } else {
                while (*tail != NULL) {
                    tail = &((*tail)->next);
                }
                *tail = temp;
            }

            pthread_mutex_unlock(&mutex_insert);
            sleep(random_func(3,6));
        }
    }
}

void searcher () {
    struct single_link_list *temp;
    while (1) {
        if (count_element() > 0 && !pthread_mutex_trylock(&mutex_search)){
            temp = head;
            if (temp == NULL){
                printf("No elements in the list\n");
                continue;
            } else {
                while (temp != NULL){
                    printf("[%d] ", temp->num);
                    temp = temp->next;
                }
                printf("\n");
            }
            pthread_mutex_unlock(&mutex_search);
        }
        sleep(1);
    }
}

void deleter () {
    int delete_val;
    struct single_link_list *temp, *prev;
    while (1) {
        if (count_element() > 0 && !pthread_mutex_trylock(&mutex_insert)) {
            if(!pthread_mutex_trylock(&mutex_search)) {
                temp = head;
                delete_val = rand() % 9 + 1;
                while (temp != NULL) {
                    if(temp->num == delete_val){
                        printf("Delete: %d\n", delete_val);
                        if (temp == head) {
                            head = temp->next;
                            free(temp);
                            break;
                        } else {
                            prev->next = temp->next;
                            free(temp);
                            break;
                        }
                    } else {
                        prev = temp;
                        temp = temp->next;
                    }
                }
                pthread_mutex_unlock(&mutex_search);
            }
            pthread_mutex_unlock(&mutex_insert);
        }
        sleep(random_func(2,5));
    }
}

// ==================================================
int main() {
    srand(time(NULL));

    struct single_link_list *new_list;
    new_list = (struct single_link_list *)malloc(sizeof(struct single_link_list));
    new_list -> num = rand() % 9 + 1;
    head = new_list;
    head->next = NULL;

    // thread initial
    pthread_t thread_search[3];
    pthread_t thread_insert[3];
    pthread_t thread_delete[3];

    int i = 0;
    // create three threads for each
    for(i = 0; i < 3; ++i){
        if(pthread_create(&thread_insert[i], NULL, (void *)inserter, (void *)NULL)) {
            fprintf(stderr, "Error creating inserter!!\n");
            return -1;
        }

        if(pthread_create(&thread_search[i], NULL, (void *)searcher, (void *)NULL)) {
            fprintf(stderr, "Error creating searcher!!\n");
            return -1;
        }

        if(pthread_create(&thread_delete[i], NULL, (void *)deleter, (void *)NULL)) {
            fprintf(stderr, "Error creating deleter!!\n");
            return -1;
        }
    }

    // wait for created thread to exit
    for(i = 0; i < 3; ++i){
        pthread_join(thread_search[i], NULL);
        pthread_join(thread_insert[i], NULL);
        pthread_join(thread_delete[i], NULL);
    }

    return 0;
}