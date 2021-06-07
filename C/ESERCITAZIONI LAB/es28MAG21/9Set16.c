/*librerie*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define PERM 0644               /*definisco i diritti da utilizzare nella creazione dei file*/

typedef int pipe_t[2];          /*definisco il tipo pipe_t come vettore di 2 interi, contiene il fd delle pipe*/

typedef struct{
    char v1;    /*carattere*/
    long v2;    /*occorrenze*/
}s_occ;

/*funzione di attesa
    @param
        const char *str: contiene il livello del processo che si sta chiudendo (es: "figlio", "nipote"...)
        int err: contiene il valore da ritornare in caso di errore
    @return 
        int : che corrisponde al valore ritornato dal processo in chiusura
    */
int waiting(const char *str){
        /*contiene i 2 bytes ritornati dal processo atteso con la wait*/
        int status;

        /*contiene il valore di ritorno del processo appena terminato*/
        int ritorno;
    	int pid = wait(&status);
		if (pid < 0)
		{
		printf("Errore: problemi nella wait di atttesa dei processi con gerarchia %s\n", str);
		exit(5);
		}

		if ((status & 0xFF) != 0)
    		printf("%s con pid %d terminato in modo anomalo\n",str, pid);
    		else
		        { 
                    ritorno=(int)((status >> 8) &	0xFF); 
                    printf("Il %s con pid=%d ha ritornato %d\n", str, pid, ritorno, ritorno);
		        }
        return ritorno;
}

void bubbleSort(s_occ *v, int dim){ 
    int i; 
    bool ordinato = false;
    s_occ tmp;
    while (dim>1 && !ordinato){ 
        ordinato = true; /* hp: è ordinato */
        for (i=0; i<dim-1; i++){
            if (v[i].v2>v[i+1].v2){
                tmp=v[i];
                v[i]=v[i+1];
                v[i+1]=tmp;
                ordinato = false;
            }
        }
        dim--;
    }
}

/*VALORI DI RITORNO:
    -1: errore generico nei figli
    1: errore nel numero di parametri passati
    2: errore nel tipo di parametri
    3: errore in allocazione della memoria
    4: errore nella creazione delle pipe
    5: errore nella wait
    6: errore nella fork (generazionme dei processi)
    7: errore nell'apertura del file nel padre
    8: errore nella lettura dalla pipe
    9: errore nella scrittura dalla pipe
*/
int main(int argc, char** argv){

    int numparam;               /*contiene il numero di parametri che devono essere passati*/
    int N;                      /*numero di figli da generare, numero di file passati*/
    pipe_t pipes[26];           /*pipeline comunicazione tra figli*/
    int pid_vet[26];            /*vettore contenente i pid dei figli generati*/
    int fd;                     /*file descriptor dei file passato ai figli*/
    int i,j;                    /*contatori*/
    char C;                     /*carattere assegnato dai figli*/
    char c;                     /*carettere letto dal file*/
    long curr;                  /*conteggio occorrenze corrente*/
    s_occ s_vett[26];           /*vettore di struct*/
    int nr;                     /*bytes letti sia dai figli che dal padre dalla pipe*/
    int ns;                     /*bytes scritti sulla pipe dai figli*/

    numparam=1;

    /*controllo sul numero di parametri*/
    if(argc-1!=numparam){
        printf("errore: devi passare %d parametro\n", numparam);
        exit(1);
    }
    
    N=26;

    /*PIPELINE*/
    /*creazione delle N pipe comunicazione tra figli*/
    for(i=0; i<N; i++){
        if(pipe(pipes[i])<0){
            printf("errore: creazione della pipeline non riuscita\n");
            exit(4);
        }
    }

    /*generazione dei figli*/
    for(i=0; i<N; i++){
        if((pid_vet[i]=fork())<0){
            printf("errore: creazione processo figlio %d non riuscita\n",i);
            exit(6);
        }

        /*codice FIGLIO*/
        if(pid_vet[i]==0){
            C='a'+ i;
            curr=0;
            printf("sono il figlio di indice %d e pid: %d carattere: %c\n", i, getpid(), C);
            /*chiusura delle pipe non utilizzate*/
            for(j=0;j<N;j++){
                if(j!=i){
                    close(pipes[j][1]);
                }
                if(i==0 || j != i-1){
                    close(pipes[j][0]);
                }
            }

            /*apertura del file*/
            fd=open(argv[1],O_RDONLY);
            if(fd<0){
                printf("errore: apertura file non riuscita\n");
                exit(-1);
            }

            while(read(fd,&c,sizeof(c))){
                if(c==C)
                    curr++;
            }

            /*leggo dalla pipe la struct tranne che dal filgio 0*/
            if(i!=0){
                nr=read(pipes[i-1][0],s_vett,N*sizeof(s_occ));
                if(nr!=N*sizeof(s_occ)){
                    printf("errore: figlio problema nella lettura dalla pipe\n");
                    exit(-1);
                }
            }
            s_vett[i].v1=C;
            s_vett[i].v2=curr;

            ns=write(pipes[i][1], s_vett, N*sizeof(s_occ));
            if(ns!=N*sizeof(s_occ)){
                printf("errore: figlio problema nella scrittura dalla pipe\n");
                exit(-1);
            }
            exit(c);
        }
    }

    /*codice PADRE*/
    /*chiusura dei lati delle pipe non utilizzati*/
    for(i=0; i<N;i++){
        close(pipes[i][1]);
        if(i!=N-1){
            close(pipes[i][0]);
        }
    }
    /*lettura dalla pipe ultimo figlio*/
    nr=read(pipes[N-1][0],s_vett,N*sizeof(s_occ));
    if(nr!=N*sizeof(s_occ)){
        printf("errore: padre problema nella lettura dalla pipe\n");
        exit(8);
    }
    bubbleSort(s_vett, N);
    for(i=0;i<N;i++){
        printf("il figlio\tindice: %d\tpid: %d \t{carattere %c \t%ld occorrenze}\n", s_vett[i].v1-'a', pid_vet[s_vett[i].v1-'a'],s_vett[i].v1, s_vett[i].v2);
    }

    /*ATTESA DEI FIGLI*/
    for(i=0; i<N; i++){
        waiting("figlio");
    }

    exit(0);
}