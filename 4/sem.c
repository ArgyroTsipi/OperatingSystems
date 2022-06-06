/**
 * \file sem.c
 * \author Aggelos Kolaitis <neoaggelos@gmail.com>
 * \date 2022-05-23
 * \brief Implementation skeleton for semaphore library.
 */

// Standard Library
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Semaphore library
#include "sem.h"
//int lock = 0;
pipe_semaphore_t *new_semaphore() {
  // TODO: allocate memory for semaphore, initialize internal state
   pipe_semaphore_t* s;
   s->pipes[0] = malloc(2*sizeof(int)); // allocate memory
   s->pipes[1] = malloc(2*sizeof(int));
 
    if(pipe(s->pipes) == -1) // pipe
    {
        perror("pipe");
        exit(1);
        return NULL; // return NULL on error
    }
        
  return s; // return a valid semaphore on success
}

int semaphore_wait(pipe_semaphore_t *s, int val) {

    // wait reads & decrements the value of the semaphore by val & blocks until this is possible.
  
    // TODO: implement semaphore wait
    
    //while(lock != 0);
    //lock = 1;
    
    if(s == NULL) {
        perror("s");
    }
    
    if(val <= 0){
        perror("negative value");
    }
    
    char buff[val];
    // if val > pipes characters then wait until it reads them all
    int result = 0;
    int counter = 0; // counter to count the total number of chars we read
    
    while(counter < val){
        
        result = read(s->pipes[0], &buff[val], val-counter);
        
        if(result == -1){
            perror("read");
        }
        
        counter = counter + result;
    }
    return 0;
}

//The entry to the critical section is handled by the wait() function. Critical section - shared variable : lock before change & free afterwards

// The exit from a critical section is controlled by the signal() function.

int semaphore_signal(pipe_semaphore_t *s, int val){
    
    // signal increments the value of the semaphore, unblocking.
  // TODO: implement semaphore signal
    if(s == NULL) {
        perror("s");
    }
    char buff[val];
    for(int i=0; i<val; i++){
        buff[i] = '1';
    }
    // A race condition occurs when multiple processes or threads read and write
    int result = write(s->pipes[1], &buff[val], val); // reads val chars from pipe
    
    if(result == -1){
        perror("write");
    }
    //lock = 0;
  return 0;
}

// remainder section
