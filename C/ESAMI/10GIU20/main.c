#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdint.h>
#include <ctype.h>
#include <sys/stat.h>

#define PERM 0644
typedef int pipe_t[2];          /*definisco il tipo pipe_t come vettore di 2 interi, contiene il fd delle pipe*/

/*funzione di attesa
    @param
        const char *str: contiene il livello del processo che si sta chiudendo (es: "figlio", "nipote"...)
        int indice: contiene il valore da ritornare in caso di errore
    @return 
        int : che corrisponde al valore ritornato dal processo in chiusura
    */
int waiting(const char *str, int index){
        /*contiene i 2 bytes ritornati dal processo atteso con la wait*/
        int status;

        /*contiene il valore di ritorno del processo appena terminato*/
        int ritorno;
    	int pid = wait(&status);
		if (pid < 0)
		{
		printf("Errore: problemi nella wait di attesa dei processi con gerarchia %s\n", str);
		exit(5);
		}

		if ((status & 0xFF) != 0)
    		printf("%s con pid %d terminato in modo anomalo\n",str, pid);
    		else
		        { 
                    ritorno=(int)((status >> 8) &	0xFF);
                    if(ritorno==255){
                        printf("attenzione ci sono stati dei problemi durante l'esecuzione del processo figlio con indice %d\n", index);
                    } 
                    printf("Il %s di indice %d con pid=%d ha ritornato %d\n", str, index, pid, ritorno);
		        }
        return ritorno;
}

/*VALORI DI RITORNO:
    -1: errore generico nei figli
    1: errore nel numero di parametri passati
    2: errore nel tipo di parametri
    3: errore in allocazione della memoria
    4: errore nella creazione delle pipe
    5: errore nella wait
    6: errore nella fork (generazionme dei processi)
    7: errore nell'apertura/creazione di file nel padre
    8: errore nella lettura dalla pipe
    9: errore nella scrittura dalla pipe
*/
int main(int argc, char **argv){
    
    int numparam;               /*contiene il numero di parametri che devono essere passati*/
    int Q;                      /*numero di figli da generare, numero di file passati*/
    pipe_t *pipe_pf;            /*vettore di pipes comunicazione padre figlio*/
    int i, q;                   /*contatore*/
    int pid;                    /*ontenente i pid dei figli*/
    int fd;                     /*file descriptor dei file*/
    char linea[10];             /*contiene la linea letta e trasmessa dai filgi al padre*/
    int count=0;                /*conto delle righe scritte*/
    int fdp;                    /* file descriptor file padre */
    /*controllo sul numero di parametri >= 2*/
    numparam=argc-1;
    if(numparam<2){
        printf("errore: numero di parametri passati errato\n");
        exit(1);
    }

    Q=argc-1;
    /*alloco la memoria per il vettore di pipe comunicazione padre-figlio*/
    pipe_pf=malloc(Q*sizeof(*pipe_pf));
    if(!pipe_pf){
        printf("errore: allocazione della memoria non riuscita\n");
        exit(3);
    }

    /*creo le pipe*/
    for(q=0; q<Q; q++){
        if(pipe(pipe_pf[q])!=0){
            printf("errore: creazione delle pipe non riuscita\n");
            exit(4);
        }
    }

    /*creo il file camilla*/
    fdp=creat("Camilla", PERM);
	if (fdp < 0){
       	printf("Errore creazione file Camilla\n"); 
	    exit(7);
    }


    /*generazione dei processi figli*/
    /* creazione figli */
	for (q=0; q < Q; q++)
	{
		if((pid=fork())<0){
            printf("errore: generazione del figlio non risucita\n");
            exit(6);
        }

        /*codice figlio*/
		if(pid==0){
            printf("sono il figlio di indice %i \tpid:%d \tfile:%s \n", q, getpid(), argv[q+1]);
            
            /*chiusura pipe inutilizzate
                il figlio di indice q deve scrivere al padre
                chiude tutte le letture 
                lascia aperta la propria scrittura
            */
			for (i=0; i < Q; i++){
				close(pipe_pf[i][0]);
				if (i != q)
					close(pipe_pf[i][1]);
			}
			 
            /*apertura del file associato*/
            fd=open(argv[q+1], O_RDONLY);
            if(fd<0){
                printf("errore: apertura del file %s non riuscita\n", argv[q+1]);
                exit(-1);
            }

			i=0;	
			while (read(fd,&(linea[i]),sizeof(*linea)))
			{	
                		if (linea[i] == '\n'){
                        	if (isdigit(linea[0]) && (i+1 < 10)){	
	  					        write(pipe_pf[q][1],linea,i+1*sizeof(*linea));
						        count++; 
					        }
			 		    i=0; 
				}
				else
					i++;
			}
			exit(count);
		}
	}
    //codice padre
    /*chiusura delle pipe
        chiudo tutti i lati delle scritture 
        tengo aperto tutti i lati delle letture
    */
    for(q=0; q<Q; q++){
        close(pipe_pf[q][1]);
    }

    //leggo dai figli
    for(q=0; q<Q; q++){
        i=0;
        while(read(pipe_pf[q][0], &linea[i],sizeof(*linea))){
            if(linea[i]=='\n'){
                linea[i+1]='\0';
                printf("il figlio di inidce: %d ha letto dal file %s la linea %s\n", q, argv[q+1], linea);
                write(fdp,linea,i);
                i=0;
            }
            else
                i++;
        }
    }

    //il padre aspetta i figli 
    for(q=0; q<Q; q++){
        waiting("figlio", q);
    }
    exit(0);
}