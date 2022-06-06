#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <signal.h>

#define BUFFER_SIZE 20
pid_t* child; //store pids globally
char* gates; //store argv[1][] values to global gates[]
int count=0;
int k=0;
int usr1=0,usr2=0,term=0,ch=0;
int status;

pid_t term_child;

void s_usr1(int signum){
    usr1=1;
}
void s_usr2(int signum){
    usr2=1;
}

void s_term(int signum){
    term=1;
}

void check_child(int signum){
    ch=1; 
}
//vohthtikh sunarthsh poy vriskei kai epistrefei to index enos terminated paidiou
int terminated_pids(pid_t pid, pid_t* arr, int length){
    int index = -1;
    for(int i = 0; i < length; i++){
        if(arr[i] == pid){
            index = i;
            break;
        }
    }
    if(index == -1){
        fprintf(stderr, "The PID we were looking for was not found.\n");
        term = 1;
    }
    return index;
}

int main(int argc, char **argv){
    
    if(argc == 1 || argc > 2){
        fprintf(stderr, "Usage: %s -states of gates-\n", argv[0]);
        exit (1);
        return 1;
    }
        
    if(strcmp(argv[1], "--help" ) == 0){
         fprintf(stdout, "Usage: %s -states of gates-\n", argv[0]);
         exit(0);
         return 0;
     }
    
    signal(SIGUSR1, s_usr1);
    signal(SIGUSR2, s_usr2);
    signal(SIGTERM, s_term);
    signal(SIGCHLD, check_child);
        
    count = strlen(argv[1]); //xreiazomaste ton arithmo twn sunolikwn chars sto sring
    gates = malloc(count* sizeof(char)); //allocate memory for global arrays
    child = malloc(count* sizeof(pid_t)); //alliws segmentation fault

    while(argv[1][k] != '\0'){ //copy ton pinaka argv[1][] ston gates[]
        gates[k] = argv[1][k];
        k++;
    }
    
    for(int i=0; i<count; i++){
        child[i] = fork();
        if(child[i] == -1){
            fprintf(stderr, "Parent failed to create child!\n");
            exit(1);
            return 1;
        }
        else if(child[i] == 0){
            char buf[BUFFER_SIZE];
            snprintf(buf, BUFFER_SIZE, "%d", i);
            char* args[] = {"./child", buf, &gates[i], NULL};
            // path, gate number, gates t or f, null terminated
            // for child's state we need the gate number and gates state
            if(execv(args[0], args) == -1){
                fprintf(stderr, "EXECV Failed from child!\n");
                exit(1);
                return 1;
            }
        }
        else {
                printf("[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'.\n", getpid(), i, child[i], gates[i]);
        }
    }
    
    while(true){
    if(usr1 == 1){
        for(int i=0; i<count; i++){
            if(kill(child[i],SIGUSR1) == -1){
                fprintf(stderr,"Signal USR1 gone wrong.\n");
            }
        }
        usr1=0;
    }
    else if(usr2 == 1){
        for(int i=0; i<count; i++){
            if(kill(child[i], SIGUSR2) == -1){
                fprintf(stderr,"Signal USR2 gone wrong.\n");
            }
        }
        usr2=0;
    }
    else if(term == 1){
        for(int i=0; i<count; i++){
            if(kill(child[i],SIGTERM) == -1){
                fprintf(stderr,"TERMINATION gone wrong.\n");
            }
            else{
                printf("[PARENT/PID=%d] Waiting for %d children to exit.\n", getpid(), count-i);
                waitpid(child[i],&status,0);
            }
        }
        printf("[PARENT/PID=%d] All children exited, terminating as well.\n", getpid());

        free(child);
        free(gates);
        exit(0);
    }
    else if(ch == 1){
       while((term_child = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0 ){
           //success 0, error -1, cchanged pid
           // wnohang returns immediately if no child has excited
           // wuntraced return if a child has stopped but not traced via ptrace()
            int i = terminated_pids(term_child, &child[i], count); 
            if (i == -1){
                kill(term_child, SIGTERM);
                waitpid(child[i], &status, 0);
                term = 1;
                break;  
            } //elegxos an to i = -1 tote kati paei lathos
            if(WIFSTOPPED(status)==true){
                if(kill(term_child, SIGCONT) == -1){
                    fprintf(stderr, "WIFSTOPPED-CONTINUE error.\n");
                    continue;
                }
                printf("[PARENT/PID=%d] Child %d with PID=%d continued.\n", getpid(), i, term_child);
            }
            else if((WIFEXITED(status)==true) || (WIFSIGNALED(status)==true)){
                child[i] = fork();
                if(child[i] == -1){
                    fprintf(stderr, "Failed to create NEW child!\n");
                    continue;
                }
                else if(child[i] == 0){
                    //fork ok -> ksana args & execv
                    char buf[BUFFER_SIZE];
                    snprintf(buf, BUFFER_SIZE, "%d", i);
                    char* args[] = { "./child", buf, &gates[i], NULL };
                    if(execv(args[0], args) == -1){
                        fprintf(stderr, "EXECV Failed from NEW child!\n");
                        exit(1);
                    }
                }
                else{
                    //p code
                    printf("[PARENT/PID=%d] Created new child for gate %d (PID %d) and initial state \'%c\'\n", getpid(), i, child[i], gates[i]);
                }
            }
            }
            ch = 0;
    }
    }
    return 0;
}
