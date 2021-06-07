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
    8: errore nella lettura 
    9: errore nella scrittura 
*/
int main(int argc, char **argv){
    
    int B;      /*numero di figli da generare*/
    int L;      /*dimensione in byte del file da analizzare, multiplo di B*/
    int q;      /*indice dei figli*/
    int i;      /*contatore*/
    int *pid;    /*pid dei figli*/
    pipe_t *p;      /*pipe di comunicazione padre figlio*/
    int numparam=argc-1;   /*contiene il numero di parametri che devono essere passati*/
    int fdc;    /*file creato dal padre*/
    int fd;     /*file descriptor del file passato da anlaizzare*/
    char *file;   /*contiene il nome dell file da creare*/
    char *est=".Chiara";    /*contiene l'estensione del nuovo file*/
    char *linea;        /*contiene i byte letti dai figli*/

    /*controllo sul numero di parametri
    deve essere esattamente 3*/
    if(numparam!=3){
        printf("errore: numero di parametri errato\n");
        exit(1);
    }

    B=atoi(argv[2]);
    L=atoi(argv[3]);
    if(L<=0||B<=0){
        printf("errore: parametri passati incorretti\n");
        exit(2);
    }

    /*definisco la stringa contenente il nome del file da creare*/
    file =malloc(sizeof(*file));
    file= strcat(file,argv[1]);
    file = strcat(file,est);

    /* allocazione pid */
	if ((pid=(int *)malloc(B*sizeof(int))) == NULL)
	{
        	printf("Errore allocazione pid\n");
        	exit(3);
	}

    /*creo il file*/
    fdc= open (file, O_CREAT | O_WRONLY, PERM);
    if(fdc<0){
        printf("errore: creazione del file non riuscita\n");
        exit(7);
    }

    /*alloco la memoria per le pipe*/
    p=malloc(B*sizeof(*p));
    if(!p){
        printf("errore: allocazione della memoria non riuscita\n"),
        exit(3);
    }


    /*creo le pipe*/
    for(q=0;q<B;q++){
        if(pipe(p[q])<0){
            printf("errore: creazione delle pipe non risucita\n");
            exit(4);
        }
    }

    /*generazione dei figli*/
    for(q=0; q<B; q++){
        if((pid[q]=fork())<0){
            printf("errore: generazione dei processi non riuscita\n");
            exit(6);
        }

        /*codice figlio*/
        if(pid[q]==0){
            printf("figlio %d pid: %d byte [%d, %d]\n", q, getpid(), q*(L/B), ((q+1)*(L/B))-1);
            /*chiusura lati pipe inutiliazzati
                chiudo tutti i lati di lettura
                lascio aperto il prorio lato di scrittura*/
            for(i=0;i<B;i++){
                close(p[i][0]);
                if(i!=q){
                    close(p[i][1]);
                }
            }

            /*apertura del file*/
            fd=open(argv[1], O_RDONLY);
            if(fd<0){
                printf("erorre: errore nell'apertura del file nel figlio\n");
                exit(-1);
            }

            /*alloco la memoria per la linea*/
            linea=malloc((L/B)*sizeof(*linea));
            if(!linea){
                printf("erorre: allocazioje memoria non riuscita\n");
                exit(3);
            }

            /*mi sposto al primo byte da leggere*/
            lseek(fd,(long)q*L/B,SEEK_SET);

			read(fd,linea,L/B); 		
	  		write(p[q][1],&linea[L/B-1],1);
            exit(L/B);
        }
    }

    /*codice padre*/
    char ch;    /*carattere letto dalla pipe*/
    /*chiusura pipe non utilizzate
        legge da tutti i figli ma non scrive a nessuno
    */
    for(q=0;q<B;q++){
         close(p[q][1]);
    }

    for(q=0;q<B;q++){
        read(p[q][0], &ch, 1);
        write(fdc, &ch, 1);
    }

    for(q=0; q<B; q++){
        waiting("figlio", q);
    }

    exit(0);
}