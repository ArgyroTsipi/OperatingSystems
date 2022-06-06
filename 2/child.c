#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

int t = 0;
int tttime = 0;
char* gateno;
bool state = 0;
int flag_usr1=0,flag_usr2=0,flag_term=0, flag_alarm=0;

void handle_usr2(int signum){
    flag_usr2 = 1;
}

void handle_usr1(int signum){
    flag_usr1 = 1;
}

void handle_term(int signum){
    flag_term = 1;
}

void handle_alrm(int signum){
    flag_alarm = 1;
   // alarm(5);
   }

int main(int argc, char** argv){
    time_t start = time(0); //The time(0) function returns the current time in seconds.
    //1o orisma to path
    //2o orisma to gate number
    gateno = argv[1];
    //3o orisma to gate state (true or false) g[i]
    //ean to g[i] einai t tote state = 0, alliws state = 1
    // strncmp == 0 the contents of the two strings are equal
    //printf("%s",argv[2]);
    if(strncmp(argv[2], "t", sizeof(char)) == 0){
        state = 0;
    }
    else if(strncmp(argv[2], "f", sizeof(char)) == 0){
        state = 1;
    }
    else{
        fprintf(stderr, "Wrong argument value!\n");
        exit(1);
        return 1;
    }
        
    signal(SIGUSR1, handle_usr1);
    signal(SIGUSR2, handle_usr2);
    signal(SIGTERM, handle_term);
    signal(SIGALRM, handle_alrm);
    
  //  kill(getpid(), SIGALRM);
    
    alarm(1);
    //sleep(15);
    //printf("%ld\n", start);
    while(true){
        
    t = time(0) - start;

    if(flag_usr2 == 1){
    if(state == 0){
        printf("[GATE=%s/PID=%d/TIME=%ds] The gates are open!\n", gateno, getpid(), t-1);
    }
    else{
        printf("[GATE=%s/PID=%d/TIME=%ds] The gates are closed!\n", gateno, getpid(), t-1);
        }
    flag_usr2 = 0;
    sleep(15);
    }
    else if(flag_usr1 == 1){
        state = !state;
        if(state == 0){
            printf("[GATE=%s/PID=%d/TIME=%ds] The gates are open!\n", gateno, getpid(), t-1);
        }
        else{
            printf("[GATE=%s/PID=%d/TIME=%ds] The gates are closed!\n", gateno, getpid(), t-1);
            }
        flag_usr1 = 0;
        sleep(10);
    }
    else if(flag_term == 1){
        printf("Child with PID=%d terminated successfully with exit status code O!\n", getpid());
        exit(0);
        flag_term = 0;
    }
    else if(flag_alarm == 1){
        if(state == 0){
            printf("[GATE=%s/PID=%d/TIME=%ds] The gates are open!\n", gateno, getpid(), tttime);
        }
        else{
            printf("[GATE=%s/PID=%d/TIME=%ds] The gates are closed!\n", gateno, getpid(), tttime);
            }

        tttime += 15;
        sleep(15);
        //flag_alarm = 0;
    }

    }
    return 0;
}

