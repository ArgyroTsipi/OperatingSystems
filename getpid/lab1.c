#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 64
#define PERMS 0644

bool FileExists(char* filename){
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int main(int argc, char **argv){
    
    if(argc == 1 || argc > 2){
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
        return 1;
    }
    
    
   if(strcmp(argv[1], "--help" ) == 0){
        fprintf(stdout, "Usage: %s filename\n", argv[0]);
        exit(0);
        return 0;
    }
        
        
    pid_t c1;
    int fd, status; /*katastash eksodou ths c1*/
    char buf_ch[200];
    char buf_par[200];
    c1 = fork();
    
    
/************************************************child's code error**************************************************/

    if(c1 == -1){
        fprintf(stderr, "child error\n");
        status = -1;
        exit(1);
    }

/************************************************child's code success***********************************************/

    else if(c1 == 0){
        
    
        fd = open(argv[1], O_CREAT | O_APPEND | O_WRONLY, 0644);
        
        if(fd == -1){
            fprintf(stderr, "child's open function\n");
            exit(1);
        }

        snprintf(buf_ch, BUFFER_SIZE, "[CHILD] getpid()=%d, getppid()=%d\n", getpid(), getppid());
        
        if(write(fd, buf_ch, strlen(buf_ch)) != strlen(buf_ch)){
            fprintf(stderr, "child's write function\n");
            exit(1);
            }
        close(fd);
    }
    else {

/******************************************************parent's code***************************************************/
       // wait(&status);
        
/*  */   if(FileExists(argv[1])){
            fprintf(stderr, "Error: %s already exists\n", argv[1]);
            exit(1);
            return 1;
        }
/* */   else{
            fd = open(argv[1], O_CREAT | O_APPEND | O_WRONLY, 0644);
        
            if(fd == -1){
                fprintf(stderr, "parent's open function\n");
                exit(1);
            }
        
            wait(&status);
        
            //size_t nbytes = strlen(buf_par);
            snprintf(buf_par, BUFFER_SIZE, "[PARENT] getpid()=%d, getppid()=%d\n", getpid(), getppid());

            //write(fd, buf_par, strlen(buf_par));
            if(write(fd, buf_par, strlen(buf_par)) != strlen(buf_par)) {
                fprintf(stderr, "parent's write function\n");
                exit(1);
            }
        
            close (fd);
            exit(0);
/* */   }
   }
    return 0;
}
/* write() attempts to write nbyte of data to the object referenced by the
         descriptor fildes from the buffer pointed to by buf. */

