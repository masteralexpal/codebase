/*
  https://preshing.com/20120515/memory-reordering-caught-in-the-act/
  http://imsweb.club/code/memory_reordering.c
*/
#include "pthread.h"
#include "semaphore.h"
#include "stdio.h"
#ifdef COMP_BAR
#define compiler_reordering() asm volatile("" ::: "memory")
#else
#define compiler_reordering()
#endif
// smp_mb()
#ifdef MEM_BAR
#define mem_barrier() asm volatile("mfence" ::: "memory")
#else
#define mem_barrier()
#endif

sem_t beginSema1;
sem_t beginSema2;
sem_t endSema;

int X, Y;
int r1, r2;

void* thread1Func(void* param) {
  for (;;)  // Loop indefinitely
  {
    sem_wait(&beginSema1);  // Wait for signal from main thread

    // ----- THE TRANSACTION! -----
    X = 1;
    compiler_reordering();  // Prevent compiler reordering
    mem_barrier();
    r1 = Y;

    sem_post(&endSema);  // Notify transaction complete
  }
  return NULL;  // Never returns
};

void* thread2Func(void* param) {
  for (;;)  // Loop indefinitely
  {
    sem_wait(&beginSema2);  // Wait for signal from main thread

    // ----- THE TRANSACTION! -----
    Y = 1;
    compiler_reordering();  // Prevent compiler reordering
    mem_barrier();
    r2 = X;

    sem_post(&endSema);  // Notify transaction complete
  }
  return NULL;  // Never returns
};

int main() {
  // Initialize the semaphores
  sem_init(&beginSema1, 0, 0);
  sem_init(&beginSema2, 0, 0);
  sem_init(&endSema, 0, 0);

  // Spawn the threads
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, thread1Func, NULL);
  pthread_create(&thread2, NULL, thread2Func, NULL);

  // Repeat the experiment ad infinitum
  int detected = 0;
  for (int iterations = 1;; iterations++) {
    // Reset X and Y
    X = 0;
    Y = 0;
    // Signal both threads
    sem_post(&beginSema1);
    sem_post(&beginSema2);
    // Wait for both threads
    sem_wait(&endSema);
    sem_wait(&endSema);
    // Check if there was a simultaneous reorder
    if (r1 == 0 && r2 == 0) {
      detected++;
      printf("%d reorders detected after %d iterations\n", detected,
             iterations);
    }
  }
  return 0;  // Never returns
}