#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>



int main(int argc, char **argv){
    int ppid = getpid();
    printf("il pid del processo padre è: %d\n", getpid());

    int pid;
    if((pid = fork())<0){
        printf("errore nella fork\n");
        exit(1);
    }

    /*figlio*/
    if(pid==0){
        printf("pid processo figlio: %d\n", getpid());
        printf("il pid del processo padre è: %d\n", getppid());
        int num=rand()%100;
        exit(num);
    }

    /*padre*/
    int pidfiglio, status;
    if((pidfiglio=wait(&status))<0){
        printf("errore nelle wait\n");
        exit(2);
    }

    if(pid!=pidfiglio){
        printf("il pid della wait non corrisposnde al pid della fork\n");
        exit(3);
    }

    printf("terminato figlio con pid: %d\n",pidfiglio);
    if((status & 0XFF)!=0){
        printf("figlio terminato in modo anomalo\n");
        exit(4);
    }

    /*il valore di ritorno del filgio è nel byte alto, abbiamo già controllato che il byte basso è 0 quindi facciamo shift*/
    int exitfiglio= status >> 8;
    exitfiglio &= 0XFF;
    printf("il figlio ha ritornato: %d\n",exitfiglio);
    return EXIT_SUCCESS;
}

