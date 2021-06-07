#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

long contaoccorrenze(const char *filename, char c){
    char buffer;
    long occ=0;
    int nread;
    int fd=open(filename,O_RDONLY);
    while(read(fd, &buffer, 1)){
        if(buffer == c){
        occ++;
        }
    }
    return occ;
}

int main(int argc, char **argv){

    /*controllo il numero di parametri=1*/
    if(argc < 4){
        printf("errore devi passare almeno 3 parametri\n");
        exit(1);
    }

    /*controllo sul tipo di parametri, N file 1 carattere singolo*/

    for(int i=1; i<argc; i++){
        int fd=open(argv[i], O_RDONLY);
        if(fd<0){
            printf("errore nell'apertura del file numero %d\n",i);
            exit(2);
        }
    }

    if(argv[argc][1]!=0){
        printf("l'ultimo parametro deve essere un singolo carattere\n");
        exit(3);
    }
    char c=argv[argc][0];

    printf("pid corrente(padre): %d\n\n", getpid());

    /*vi sono argc-2 file passati quindi da generare argc-2 processi figli*/
    int pid;
    for(int i=0; i<argc-2; i++){
    
        if((pid=fork())<0){
            printf("errore nella fork\n");
            exit(3);
        }
        else{
            if(pid==0){
                long occ=contaoccorrenze(argv[i+1],c);
                exit(occ);
            }
        }
    }

    int pidf, status;
    for(int i=0; i<argc-2; i++){
        if((pidf=wait(&status))<0){
            printf("errore nella wait\n");
            exit(4);
        }
        if((status & 0XFF) != 0){
            printf("errore figlio temrinato in modo anomalo\n");
            exit(5);
        }
        int exitf= (status>>8) & 0XFF;
        printf("terminato figlio con PID:%d, \tritornato: %d\n",pidf,exitf);
    }

    return EXIT_SUCCESS;
}   
