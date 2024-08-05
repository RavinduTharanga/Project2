#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX 100         // number of iterations for each member or officer
#define BUFLEN 24
#define NUMTHREAD 5     // number of threads
#define LIMIT 5         // low balance limit
#define NUM_OFFICERS 2
#define NUM_MEMBERS 3

void *member(void *threadid);
void *officer(void *threadid);

// Synchronization-related variables
pthread_mutex_t mutex;
pthread_cond_t cond;

typedef struct {    // maintains withdrawal queue for officers (quiz question 8)
    int id;
    double amount;
} OfficerQueue;

int i = 0, j = 0;

/* shared variable */
double balance = 0;

OfficerQueue withdrawalq[MAX];
int qfront = 0, qrear = 0, qsize = 0;

int main() {
    int i;

    /* initializing mutex and condition variable*/
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    /* define the type to be pthread */
    pthread_t thread[NUMTHREAD];
    long t = 0;

    // Create threads for two club officers and three club members
    for (t = 0; t < NUM_OFFICERS; t++) {
        pthread_create(&thread[t], NULL,(void*)officer, (void *)t);
    }
    for (t = NUM_OFFICERS; t < NUMTHREAD; t++) {
        pthread_create(&thread[t], NULL, (void*)member, (void *)t);
    }

    /* wait for them to finish before exiting */
    for (i = 0; i < NUMTHREAD; i++) {
        pthread_join(thread[i], NULL);
    }

    /* destroying mutex and condition variable */
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    pthread_exit(NULL);
}

void *member(void *threadid) {
    double deposit;
    long id = (long)threadid;

    // For MAX iterations
    while (j < MAX) {
        // generate a value for a deposit of up to $100
        deposit = (double)(rand() % 100);

        // lock the mutex before accessing the shared balance
        pthread_mutex_lock(&mutex);

        balance += deposit;
        printf("member %ld deposited: %10.2f; balance is now %10.2f\n", id, deposit, balance);
        fflush(stdout);
        j++;

        // signal any waiting officers that money has been deposited
        pthread_cond_broadcast(&cond);

        // unlock the mutex
        pthread_mutex_unlock(&mutex);

        if (rand() % 100 >= 50)
            sleep(rand() % 2);
    }

    pthread_exit(NULL);
}

void *officer(void *threadid) {
    double debit;
    long id = (long)threadid;

    while (i < MAX) {
        /* generate a random amt to withdraw */
        debit = (double)(rand() % 100);

        // lock the mutex before accessing the shared balance
        pthread_mutex_lock(&mutex);

        withdrawalq[qrear].id = id;
        withdrawalq[qrear].amount = debit;
        qrear = (qrear + 1)%MAX;
        qsize+=1;

        while (qsize > 0) {
            OfficerQueue req = withdrawalq[qfront];
            if (balance - req.amount >= LIMIT){
                balance -= req.amount;
                printf("officer %d withdrew %10.2f; balance is now %10.2f\n", req.id, req.amount, balance);
                qfront = (qfront+1)%MAX;
                qsize-=1;
                pthread_cond_signal(&cond);
                break;
            } else {
                pthread_cond_wait(&cond, &mutex);
            }
        }

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        // Randomly sleep for a random amount of time
        if (rand() % 100 >= 50)
            sleep(rand() % 2);
    }

    pthread_exit(NULL);
}
