#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char **argv){
    if(argc != 2){
        perror("errore: devi passare un parametro\n");
        exit(1);
    }
    int fd1=open(argv[1], O_RDONLY);
    if(fd1<0){
        printf("errore nella open\n");
        exit(2);
    }else{
        printf("fd1= %d\n", fd1);
    }

    int fd2=open(argv[1], O_RDONLY);
    if(fd2<0){
        printf("errore nella open\n");
        exit(3);
    }else{
        printf("fd2= %d\n", fd2);
    }

    close(fd1);
    
    int piped[2];
    if(pipe(piped)<0){
        printf("errore nella pipe\n");
        exit(5);
    }else{
        printf("piped[0]= %d\n",piped[0]);
        printf("piped[1]= %d\n",piped[1]);
    }
    return EXIT_SUCCESS;
}