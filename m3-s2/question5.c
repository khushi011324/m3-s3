#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <chrono>
#include <vector>
#include <random>

using namespace std;
using namespace std::chrono;

#define NUM_THREADS 4  // Number of threads

// Global variables
unsigned long size = 100000000;
int *v1, *v2, *v3;

// Function to generate random vector
void randomVector(int vector[], unsigned long size) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 100);

    for (unsigned long i = 0; i < size; ++i) {
        vector[i] = dis(gen);
    }
}

// Function executed by each thread
void* threadFunction(void* threadID) {
    long tid = (long)threadID;
    unsigned long start = tid * (size / NUM_THREADS);
    unsigned long end = start + (size / NUM_THREADS);

    // Add corresponding elements of v1 and v2
    for (unsigned long i = start; i < end; ++i) {
        v3[i] = v1[i] + v2[i];
    }

    pthread_exit(NULL);
}

int main() {
    // Allocate memory for vectors
    v1 = new int[size];
    v2 = new int[size];
    v3 = new int[size];

    // Generate random vectors
    randomVector(v1, size);
    randomVector(v2, size);

    // Initialize pthread variables
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    void* status;

    // Initialize and set thread detached attribute
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Start the timer
    auto start_time = high_resolution_clock::now();

    // Create threads
    for (long t = 0; t < NUM_THREADS; ++t) {
        int rc = pthread_create(&threads[t], &attr, threadFunction, (void*)t);
        if (rc) {
            cout << "Error: Unable to create thread, " << rc << endl;
            exit(-1);
        }
    }

    // Wait for all threads to complete
    pthread_attr_destroy(&attr);
    for (long t = 0; t < NUM_THREADS; ++t) {
        int rc = pthread_join(threads[t], &status);
        if (rc) {
            cout << "Error: Unable to join thread, " << rc << endl;
            exit(-1);
        }
    }

    // Stop the timer
    auto stop_time = high_resolution_clock::now();

    // Compute the duration
    auto duration = duration_cast<microseconds>(stop_time - start_time);

    cout << "Time taken by function: " << duration.count() << " microseconds" << endl;

    // Free memory
    delete[] v1;
    delete[] v2;
    delete[] v3;

    pthread_exit(NULL);
}
