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

}struttura;

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

int main(int argc, char const **argv[]){
    
    pipe_t *pf;      /*pipe padrefiglio*/
    int pid;        /*pid dei figli generati*/
    int N;     /*numero di processi da generare*/
    int i;      /*indice dei figli*/
    int j;      /*contatore*/
    int fd;         /*file descriptor file da anilizzare*/

    /*controllo numero di parametri*/
    if(argc-1< ){
        printf("errore: numero di parametri passati incorretto\n");
        exit(1);
    }

    /*calcoliamo il numero di processi da generare*/
    N= ;

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

        /*CODICE FIGLIO*/
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


            
            /*eseguo*/


            /*scrivo sulla pipe con il padre*/

            /*ritorno un valore al padre*/
            exit();
        }

   }

    /*CODICE PADRE*/
    /*chiusura lati pipe inutilizzati
        lati scrittura pf*/
    for(i=0;i<N;i++){
        close(pf[i][1]);
    }

    /*leggo dalle pipe*/
    for(i=0; i<N;i++){
        
    }

    /*aspetto i figlii*/
    for(i=0;i<N;i++){
        waiting("figlio", i);
    }

    return 0;
}
