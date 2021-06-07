#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

typedef int pipe_t[2];  /*definisco il tipo pipe_t come vettore di 2 interi*/

typedef struct{
    int c1;     /*corrisponde al indice dal processo*/
    int c2;     /*cosrrisponde al numero di caratteri per linea*/
}elem;


int main(int argc, char** argv){

    int N;              /*numero di file passati*/
    int H;              /*ultimo parametro passato numero compreso tra ]0,255[*/
    pipe_t *pipes;      /*pipe di comunicazione P0 -> P1 -> ..PN-1 -> padre*/
    int *pid_vet;        /*vettore che conterrà i pid dei figli*/
    int fd;             /*file descriptor*/


    N = argc-2;

    /*controllo numero di parametri*/
    if(argc - 1 < 3){
        printf("errore: devi passare almeno 3 parametri\n");
        exit(1);
    }

    /*controllo sul tipo di parametri*/
    H = atoi(argv[argc-1]);
    if(H<=0||H>=255){
        printf("errore: devi pasare un numero compreso tra ]0,255[\n");
        exit(1);
    }

    /*allocazione memoria per il vettore di pipe*/
    pipes=(pipe_t*)malloc(N*sizeof(*pipes));
        if(!pipes){
        printf("errore: allocazione memoria non riuscita\n");
        exit(2);
    }

    /*creazione delle N pipe*/
    for(int i=0; i<N; i++){
        if(pipe(pipes[i])<0){
            printf("errore: creazione della pipe %i non riuscita\n",i);
            exit(3);
        }
    }

    /*allocazione memoria vettore di pid*/
    pid_vet=(int*)malloc(N*sizeof(*pid_vet));
    if(!pid_vet){
        printf("errore: allocazione memoria riuscita\n");
        exit(2);
    }

    /*allocazione memoria vettore di struct*/
    /*generazione degli N figli*/
    for(int i=0; i<N, i++){
        if((pid_vet[i]=fork())<0){
            printf("errore: fork non riuscita\n");
            exit(4);
        }

        /*codice figlio*/
        if(pipes[i]==0){

            /*chiusura delle pipe non utilizzate*/
            for(int j=0; j<N, j++){
                if(j!=i){
                    close[j][i]
                }
            }
        }
    }






}