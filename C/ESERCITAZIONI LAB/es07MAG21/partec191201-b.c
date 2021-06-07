#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

typedef struct{
    char c;
    long occ;
} elem;

long contaoccorrenze(int fd, char c){
    char buffer;
    long occ=0;
    int nread;
    while(read(fd, &buffer, 1)){
        if(buffer == c){
        occ++;
        }
    }
    return occ;
}

int main(int argc, char **argv){

    if(argc<=3){
        printf("errore: devi passare almeno parametro\n");
        exit(1);
    }

    int fd=open(argv[1], O_RDONLY);
    if(fd<0){
        printf("errore nell'apertura del file\n");
        exit(2);
    }

    int N=argc-2;
    for(int i=2;i<N;i++){
        if(argv[i][1]!='\0'){
            printf("errore: parmaetro %d non è singolo carattere\n", i-1);
            exit(3);
        }
    }

    int piped[2];
    if(pipe(piped)<0){
        printf("errorre: nella creaziome della pipe\n");
        exit(4);
    }

    for(int i=0;i<=N;i++){
        int pid=fork();

        if(pid<0){
            printf("errorre nella fork\n");
            exit(4);
        }

        /*filgio*/
        if(pid==0){
            long occ=contaoccorrenze(fd,argv[i+2][0]);
            close(piped[0]);
            elem s;
            s.c=argv[i+2][0];
            s.occ=occ;
            write(piped[0], &s, sizeof(s));
            exit(0);
        }
    }
    /*padre*/
    int pidf, status;
    for(int i=0; i<argc-2; i++){
        printf("inizo lettura\n");

        elem s1;
        while(read(piped[0], &s1, sizeof(s1))){
            printf("carattere: %c presente %ld volte\n", s1.c, s1.occ);
        }

        if((pidf=wait(&status))<0){
            printf("errore nella wait\n");
            exit(5);
        }
        if((status & 0XFF) != 0){
            printf("errore figlio temrinato in modo anomalo\n");
            exit(6);
        }
        int exitf= (status>>8) & 0XFF;
        printf("terminato figlio con PID:%d, \tritornato: %d\n",pidf,exitf);
    }
}