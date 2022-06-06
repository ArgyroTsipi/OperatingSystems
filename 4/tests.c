/**
 * \file sem.c
 * \author Aggelos Kolaitis <neoaggelos@gmail.com>
 * \date 2022-05-23
 * \brief Implementation skeleton for semaphore library.
 */

// Standard Library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <pthread.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <unistd.h>

// Semaphore library
#include "sem.h"

/**
 * \brief sigalrm_handler is used in tests where we verify that the
 * semaphore_wait call will block forever.
 */
void sigalrm_handler(int _signum) { exit(0); }

/**
 * \brief Test that semaphores are initialized to 0, and that reading on a zero
 * semaphore blocks forever.
 *
 * Possible reasons for failure:
 * - Semaphore is not initialized to 0
 * - Semaphore wait does not block even if semaphore is empty
 */
void test_wait_without_signal() {
  pipe_semaphore_t *s = new_semaphore();

  signal(SIGALRM, sigalrm_handler);
  alarm(1);

  // this should never be reached
  semaphore_wait(s, 1);
}

/**
 * \brief Test that we wait on the semaphore blocks if the current semaphore
 * value is less than the one we expect.
 *
 * Possible reasons for failure:
 * - Semaphore is not initialized to 0
 * - Semaphore wait proceeds even if the final value of the semaphore is
 *   negative
 */
void test_wait_without_enough_signal() {
  pipe_semaphore_t *s = new_semaphore();

  signal(SIGALRM, sigalrm_handler);
  alarm(1);

  semaphore_signal(s, 1);
  semaphore_wait(s, 2);

  // this should never be reached
  exit(1);
}

/**
 * \brief Test that signaling and waiting on a semaphore works OK
 *
 * Possible reasons for failure:
 * - Signal operation does not increment the semaphore
 */
void test_single_process() {
  pipe_semaphore_t *s = new_semaphore();

  semaphore_signal(s, 1);
  semaphore_wait(s, 1);

  exit(0);
}

/**
 * \brief Test locks are shared between processes.
 *
 * Possible reasons for failure:
 * - Semaphore state is not shared between processes.
 */
void test_multi_process() {
  pipe_semaphore_t *s = new_semaphore();

  pid_t p = fork();
  if (p < 0) {
    perror("fork");
    exit(-1);
  }

  if (p > 0) {
    semaphore_signal(s, 1);
    semaphore_signal(s, 1);
    wait(NULL);
  } else {
    semaphore_wait(s, 2);
  }
  exit(0);
}

/**
 * \brief Test that we can increment and decrement the semaphore in consecutive
 * operations
 *
 * Possible reasons for failure:
 * - Implementation is using a boolean lock instead of a counter value
 */
void test_many_signals() {
  pipe_semaphore_t *s = new_semaphore();

  semaphore_signal(s, 1);
  semaphore_signal(s, 1);

  semaphore_wait(s, 1);
  semaphore_wait(s, 1);

  exit(0);
}

/**
 * \brief Test that all processes waiting on the semaphore will eventually
 * run.
 *
 * Possible reasons for failure:
 * - Implementation of the wait operation decrements the counter more than it
 *   should.
 */
void test_liveness() {
  pipe_semaphore_t *s = new_semaphore();
  semaphore_signal(s, 2);

  pid_t p = fork();
  if (p < 0) {
    perror("fork");
    exit(-1);
  }

  if (p > 0) {
    semaphore_wait(s, 1);
    wait(NULL);
  } else {
    semaphore_wait(s, 1);
  }

  exit(0);
}

typedef struct {
  pthread_t pthread;
  int *counter;
  pipe_semaphore_t *counterLock;
  int step;
} my_thread;

void *pthread_loop(void *_data) {
  my_thread *data = (my_thread *)_data;
  for (int i = 0; i < 10000; i++) {
    semaphore_wait(data->counterLock, 10);
    *(data->counter) += data->step;
    semaphore_signal(data->counterLock, 10);
  }
  return NULL;
}

my_thread *init_thread(int *counter, pipe_semaphore_t *lock, int step) {
  my_thread *p = malloc(sizeof(my_thread));

  p->counter = counter;
  p->counterLock = lock;
  p->step = step;

  return p;
}

/**
 * \brief Test that our semaphore has proper locking semantics when used
 * with shared memory. The test will create two threads that operate on the
 * same variable. Arithmetic operations are not atomic, so if the semaphore
 * is lacking proper lock semantics this fails.
 *
 * Possible reasons for failure:
 * - Signal and wait operations are not atomic
 * - Signal operation returns without ensuring semaphore counter has incremented
 * - Wait operation returns without ensuring semaphore counter has decremented
 *   and remains positive.
 */
void test_loop() {
  pipe_semaphore_t *s = new_semaphore();
  int counter = 0;

  semaphore_signal(s, 10);

  my_thread *t1 = init_thread(&counter, s, 1);
  my_thread *t2 = init_thread(&counter, s, -1);

  pthread_create(&(t1->pthread), NULL, *pthread_loop, (void *)t1);
  pthread_create(&(t2->pthread), NULL, *pthread_loop, (void *)t2);

  pthread_join(t1->pthread, NULL);
  pthread_join(t2->pthread, NULL);

  if (counter != 0) {
    exit(1);
  }
  exit(0);
}

#define TEST(s)                                                                \
  {                                                                            \
    if (strncmp(argv[1], (#s), strlen(#s)) == 0) {                             \
      printf("Running test %s\n", #s);                                         \
      s();                                                                     \
    }                                                                          \
  }

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [test_name]\n", argv[0]);
    exit(-1);
  }

  TEST(test_single_process);
  TEST(test_many_signals);
  TEST(test_multi_process);
  TEST(test_loop);
  TEST(test_liveness);
  TEST(test_wait_without_signal);
  TEST(test_wait_without_enough_signal);
}
