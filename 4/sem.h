/**
 * \file sem.h
 * \author Aggelos Kolaitis <neoaggelos@gmail.com>
 * \date 2022-05-23
 * \brief Header file with definitions for semaphore library.
 */

#ifndef _SEM_H_INCLUDED
#define _SEM_H_INCLUDED

struct pipe_semaphore_t {
  // TODO: member fields for internal state of semaphore ...
    int pipes[2];
};

typedef struct pipe_semaphore_t pipe_semaphore_t;

/**
 * \brief Initialize a new semaphore. The initial value of the semaphore is 0.
 *
 * \return a valid semaphore on success, NULL on error.
 */
pipe_semaphore_t *new_semaphore();

/**
 * \brief Semaphore wait (V) decrements the value of the semaphore by val, and
 * blocks until this is possible.
 *
 * \note If semaphore_wait fails with a non-recoverable error, it does not have
 * to guarantee that the semaphore is in a consistent state after returning.
 *
 * \param sem The semaphore to wait on
 * \param val Decrement the semaphore by this value
 *
 * \return 0 on success, -1 on error
 */
int semaphore_wait(pipe_semaphore_t *sem, int val);

/**
 * \brief Semaphore signal (P) increments the value of the semaphore,
 * unblocking.

 * \note If semaphore_wait fails with a non-recoverable error, it does not have
 * to guarantee that the semaphore is in a consistent state after returning.
 *
 * \param sem The semaphore to signal
 * \param val Increment the semaphore by this value
 *
 * \return 0 on success, -1 on error
 */
int semaphore_signal(pipe_semaphore_t *sem, int val);

#endif /* _SEM_H_INCLUDED */
