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
typedef struct{
    int c1;             /*pid nipote*/
    int c2;             /*dimensione linea*/
    char c3[250];       /*linea*/
}struttura;

/*funzione di attesa
    @param
        const char *str: contiene il livello del processo che si sta chiudendo (es: "figlio", "nipote"...)
        int indice: contiene l'indice del processo che si sta aspettando, se è negativo allora vi è un solo processo
    @return 
        int : che corrisponde al valore ritornato dal processo in chiusura
    */
int waiting(const char *str, int index){
        /*contiene i 2 bytes ritornati dal processo atteso con la wait*/
        int status;

        /*contiene il valore di ritorno del processo appena terminato*/
        int ritorno;
    	int pid = wait(&status);
		if (pid < 0){
		    printf("Errore: problemi nella wait di attesa dei processi con gerarchia %s\n", str);
            if(!strcmp(str,"figlio")){
                exit(5);
            }
            if(!strcmp(str,"nipote")){
                exit(-1);
            }
		}

		if ((status & 0xFF) != 0)
    		printf("%s con pid %d terminato in modo anomalo\n",str, pid);
    	else{ 
            ritorno=(int)((status >> 8) &	0xFF);
            if(index<0){
                if(ritorno==255){
                printf("attenzione ci sono stati dei problemi durante l'esecuzione del processo %s\n", str);
                }
                printf("Il %s con pid=%d ha ritornato %d\n", str, pid, ritorno);
            }else{
                if(ritorno==255){
                printf("attenzione ci sono stati dei problemi durante l'esecuzione del processo %s con indice %d\n", str, index);
                }
                printf("Il %s di indice %d con pid=%d ha ritornato %d\n", str, index, pid, ritorno);
            }
        }
        return ritorno;
}

int main(int argc, char  **argv){
    
    pipe_t *pf;      /*pipe padrefiglio*/
    pipe_t fn;      /*pipe figlio nipote*/
    int pid;        /*pid dei figli generati*/
    int pidnip;     /*pid nipote*/
    int N;     /*numero di processi da generare*/
    int i;      /*indice dei figli*/
    int j;      /*contatore*/
    int fd;         /*file descriptor file da anilizzare*/
    struttura elemp;    /*struct su cui il padre salva i dati letti dai figli*/
    struttura elemf;       /*struct su cui salva i dati il figlio da inviare al padre*/
    char linea[250];        /*contiene la linea letta da figlio dalla pipe fn*/

    /*controllo numero di parametri*/
    if(argc-1<3){
        printf("errore: numero di parametri passati incorretto\n");
        exit(1);
    }

    /*calcoliamo il numero di processi da generare*/
    N=argc-1;

    /*allocazione memoria pf*/
    pf=malloc(N*sizeof(*pf));
    if(!pf){
        printf("errore: allocazione memoria non riusicta\n");
        exit(3);
    }

    /*creazione pipe pf*/
    for(i=0;i<N;i++){
        if((pipe(pf[i]))<0){
            printf("errore: creazione della pipe non riuscita\n");
            exit(4);
        }
    }

    /*generazione dei figli*/
    for(i=0;i<N;i++){
        pid=fork();
        if(pid<0){
            printf("errore: generazione del processo non riuscita\n");
            exit(6);
        }

        /*codice figlio*/
        if(pid==0){
            /*chiusura pipe pf
                tutti i lati di lettura dal padre
                tutti i lati di scrittura tranne quello dell'indice col padre*/
            for(j=0;j<N;j++){
                close(pf[j][0]);
                if(j!=i){
                    close(pf[j][1]);
                }
            }
            
            /*creazione pipe f*/
            if((pipe(fn))<0){
                printf("errore: creazione della pipe non riuscita\n");
                exit(-1);
            }

            /*generazione processo nipote*/
            pidnip=fork();
            if(pidnip<0){
                printf("errore: generazione del processo non riuscita\n");
                exit(-1);
            }
                /*codice nipote*/
                if(pidnip==0){
                    /*chiusura pipe inutiliazzate
                    pipe rimasta aperta tra figlio e padre e pipe di lettura dal figlio*/
                    close(pf[i][1]);

                    /*chiudo stdout e uso la dup per indirizzare il sort sulla pipe del figlio*/
                    close(1);
                    dup(fn[1]);

                    close(fn[0]);
                    close(fn[1]);


                    execlp("sort", "sort", "-f", argv[i+1], (char*)0);

                    /*se si torna qui vi è stato un errore*/
                    perror("erorre: esecuzione del comando sort non riuscito\n");
                    exit(-1);
                }
            /*codice figlio*/
            close(fn[1]);

            elemf.c1=pidnip;
            /*legge la solo la prima linea scritta dal nipote*/
            j=0;
            while(read(fn[0], &(elemf.c3[j]), 1)){
                if(elemf.c3[j]=='\n'){
                    elemf.c2=j+1;
                    break;
                }else{
                    j++;
                }
            }

            /*invio la struct al padre*/
            write(pf[i][1], &elemf, sizeof(struttura));

            /*attesa del nipote*/
            waiting("nipote", -5);
            exit(elemf.c2-1);
        }

   }

    /*codice padre*/
    /*chiusura lati pipe inutilizzati
        lati scrittura pf*/
    for(i=0;i<N;i++){
        close(pf[i][1]);
    }

    /*legge le strutture dai figli*/
    for(i=0; i<N;i++){
        read(pf[i][0], &elemp, sizeof(struttura));
        elemp.c3[elemp.c2]='\0';
        printf("struttura n:%d{\npid nipote: %d, \nlunghezza: %d, \nlinea: %s\n} riferita al file %s\n\n",i,elemp.c1,elemp.c2,elemp.c3,argv[i+1]);
    }

    for(i=0;i<N;i++){
        waiting("figlio", i);
    }

    return 0;
}
