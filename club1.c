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

// Shared variable
double balance = 0;

int main() {
    int i;

    // Initialize mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Define the type to be pthread
    pthread_t thread[NUMTHREAD];
    long t = 0;

    // Create threads for two club officers and three club members
    for (t = 0; t < NUM_OFFICERS; t++) {
        pthread_create(&thread[t], NULL, officer, (void *)t);
    }
    for (t = NUM_OFFICERS; t < NUMTHREAD; t++) {
        pthread_create(&thread[t], NULL, member, (void *)t);
    }

    // Wait for them to finish before exiting
    for (i = 0; i < NUMTHREAD; i++) {
        pthread_join(thread[i], NULL);
    }

    // Destroy mutex and condition variable
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    pthread_exit(NULL);
}

void *member(void *threadid) {
    double deposit;
    long id = (long)threadid;
    int j = 0;

    // For MAX iterations
    while (j < MAX) {
        // Generate a value for a deposit of up to $1000
        deposit = (double)(rand() % 100);

        // Lock the mutex before accessing the shared balance
        pthread_mutex_lock(&mutex);

        // Update the balance
        balance += deposit;
        printf("member %ld deposited: %10.2f; balance is now %10.2f\n", id, deposit, balance);
        fflush(stdout);
        j++;

        // Signal any waiting officers that money has been deposited
        pthread_cond_signal(&cond);

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        // Randomly sleep for a random amount of time
        if (rand() % 100 >= 50)
            sleep(rand() % 2);
    }

    pthread_exit(NULL);
}

void *officer(void *threadid) {
    double debit;
    long id = (long)threadid;
    int i = 0;

    while (i < MAX) {
        // Generate a random amount to withdraw
        debit = (double)(rand() % 60);

        // Lock the mutex before accessing the shared balance
        pthread_mutex_lock(&mutex);

        // Wait if the account doesn't have enough money to withdraw
        while (balance - debit < LIMIT) {
            pthread_cond_wait(&cond, &mutex);
        }

        // Update the balance
        balance -= debit;
        printf("officer %ld withdrew %10.2f; balance is now %10.2f\n", id, debit, balance);
        fflush(stdout);
        i++;

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        // Randomly sleep for a random amount of time
        if (rand() % 100 >= 50)
            sleep(rand() % 2);
    }

    pthread_exit(NULL);
}
