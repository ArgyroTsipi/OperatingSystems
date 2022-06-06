#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <sys/select.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

bool isNumber(char number[]){
    int i=0;
    if(number[0] == '-')
        i=1;
    for(; number[i]!=0; i++){
        if(!isdigit(number[i]))
            return false;
    }
    return true;
}

int main(int argc, char** argv){
    
    if(argc == 1 || argc > 3){
        fprintf(stderr, "Usage: %s <nChildren> [--random] [--round-robin]", argv[0]);
        exit(1);
        return 1;
    }
    

    //create pipe before fork
    int fd[2];
    if(pipe(fd) == -1){
        fprintf(stderr, "Pipe failure!\n");
        exit(1);
        return 1;
    }
   
   
    int N = atoi(argv[1]);
    char bufferOfIs[N];
    
   // bufferOfIs = malloc(N*sizeof(char));
    pid_t pidArr[N];
    int value;

    //i == arithmish paidiou
    //fork children
    
    for(int i=0; i<N; i++){
        
        pidArr[i] = fork();
        
        if(pidArr[i] == 0){ //child reads from pipe
            bufferOfIs[i] = i;

            for(;;){
                read(fd[0], &value, sizeof(int));
                printf("[Child %d] [%d] Child received %d!\n", i, getpid(), value);
                value++;
                sleep(5);
                write(fd[1], &value, sizeof(int));
                printf("[Child %d] [%d] Child Finished hard work, writing back %d\n", i, getpid(), value);
            }
            close(fd[1]);
            exit(0);
        }
        else if(pidArr[i] == -1){
            perror("fork!\n");
        }
        else {
            //parent's code
            while(1){
                fd_set rdfds;
                int maxfd;
                
                FD_ZERO(&rdfds);   // we must initialize before each call to select
                FD_SET(STDIN_FILENO, &rdfds);  // select will check for input from stdin
                FD_SET(fd[0], &rdfds); // select will check for input from pipe
                
                // select only considers file descriptors that are smaller than maxfd
                maxfd = MAX(STDIN_FILENO, fd[0]) + 1;
                
                // wait until any of the input file descriptors are ready to receive
                int ready_fds = select(maxfd, &rdfds, NULL, NULL, NULL);
                
                if (ready_fds == -1) {
                    perror("select");
                    continue;          // just try again
                }
                // user has typed something, we can read from stdin without blocking
                if (FD_ISSET(STDIN_FILENO, &rdfds)) {
                   
                    char buffer[101]; /***********************************************/

                    int n_read = read(STDIN_FILENO, buffer, 100);   // error checking!
                    
                    if(n_read == -1){
                        perror("n_read!\n");
                    }
                    
                    buffer[n_read] = '\0';                          // why?
                   
                    // New-line is also read from the stream, discard it.
                    if (n_read > 0 && buffer[n_read-1] == '\n') {
                        buffer[n_read-1] = '\0';
                    }
                    
                    printf("Got user input: '%s'\n", buffer);
                    
                    if (n_read >= 4 && strncmp(buffer, "exit", 4) == 0) {
                        // user typed 'exit', kill child and exit properly
                        for(int i=N; i>=0; i--){
                            kill(pidArr[i], SIGTERM);
                            printf("Waiting for child %d to exit!\n", i);
                        }                       // error checking!
                        wait(NULL);
                        printf("All children terminated!\n");// error checking!
                        exit(0);
                    }
                    else if (n_read >= 4 && strncmp(buffer, "help", 4) == 0) {
                        // user typed 'help'
                        printf("Type a number to send job to a child!\n");
                        //exit(0);
                    }
                    else if(isNumber(argv[1])  && (strncmp(argv[2],"--random", 8) == 0)){
                       // int num = atoi(argv[1]);
                        int randomChildIndex;
                        srand(time(NULL));
                        randomChildIndex = rand() % num;
                        printf("[Parent] Assigned %d to child %d\n", value, randomChildIndex);
                        write(fd[1], &value, sizeof(int));
                    }
                    else if((isNumber(argv[1]) && strncmp(argv[2],"--round-robin", 13) == 0) || (argc == 2 && isNumber(argv[1]))){
                        int num = atoi(argv[1]);
                        int randomChildIndex;
                        //me kuklikh epanafora round robin & default
                        printf("[Parent] Assigned %d to child %d\n", num, randomChildIndex); // or STDIN_FILENO
                        write(fd[1], &num, sizeof(int));
                    }
                    else {
                        printf("Type a number to send job to a child!");
                    }
                }
                // someone has written bytes to the pipe, we can read without blocking
                if (FD_ISSET(fd[0], &rdfds)) {
                    int val;
                    read(fd[0], &val, sizeof(int));                 // error checking!
                    printf("Got input from pipe: '%d'\n", val);
                }
            }
        }
    }
    
}
