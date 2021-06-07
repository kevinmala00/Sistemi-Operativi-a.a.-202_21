#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char **argv){

/*controllo il numero di parametri=1*/
if(argc != 2){
    printf("errore devi passare esattamente un parametro\n");
    exit(1);
}

/*controllo sul tipo di parametro int ]0,255[*/
int par1= atoi(argv[1]);

if (par1<=0 || par1>=255){
    printf("errore il primo parametro deve essere 0 < num < 255\n");
    exit(2);
}

printf("pid corrente(padre): %d,\nparam1: %d\n\n", getpid(), par1);
int pid;
for(int i=0; i<par1; i++){
    
    if((pid=fork())<0){
        printf("errore nella fork\n");
        exit(3);
    }
    else{
        if(pid==0){
            printf("\tpid figlio %d: %d\n", i, getpid());
            exit(i);
        }
    }
}

printf("\n\n");

int pidf, status;
for(int i=0; i<par1; i++){
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