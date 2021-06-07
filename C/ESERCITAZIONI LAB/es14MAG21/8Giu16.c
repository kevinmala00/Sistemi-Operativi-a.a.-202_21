#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#define MODE 666

/*funzione di attesa
    @param
        const char *str: contiene il livello del processo che si sta chiudendo (es: "figlio", "nipote"...)
        int err: contiene il valore da ritornare in caso di errore

    @return 
        int : che corrisponde al valore ritornato dal processo in chiusura
    */
int waiting(const char *str, int err){
        /*contiene i 2 bytes ritornati dal processo atteso con la wait*/
        int status;

        /*contiene il valore di ritorno del processo appena terminato*/
        int ritorno;
    	int pid = wait(&status);
		if (pid < 0)
		{
		printf("Errore: problemi nella wait\n");
		exit(err);
		}

		if ((status & 0xFF) != 0)
    		printf("%s con pid %d terminato in modo anomalo\n",str, pid);
    		else
		        { 
                    ritorno=(int)((status >> 8) &	0xFF); 
		            if (ritorno!=0)
 			            printf("Il %s con pid=%d ha ritornato %d si sono riscontrati errori\n", str, pid, ritorno);
		            else  
                        printf("Il %s con pid=%d ha ritornato %d\n", str, pid, ritorno);
		        }
        return ritorno;
}

int main(int argc, char **argv){


    /*controllo sul numero dei parametri, devono essere almeno 2*/
    if(argc<=4){
        printf("errore: devi passare almeno quattro parametri\n");
        exit(1);
    }
    /*contiene il numero dei file passati come parametri*/
    int N=argc-2;

    /*controllo sul tipo di parametri 
    i primi N devono essere file*/
    for(int i=0; i<N; i++){
        int fd = open(argv[i+1],O_RDONLY);
        if(fd<0){
            printf("errore: impossibile aprire il file %s\n",argv[i+1]);
            exit(2);
        }
    }

    /*l'ulitmo deve essere singolo carattere intero*/
    if((argv[argc-1][1])!=0){
        printf("errore: devi passare l'ultimo parametro come singolo carattere numerico\n");
        exit(2);
    }

    /*contiene l'ultimo parametro, numero*/
    int H=atoi(argv[argc-1][1]);

    /*controllo che sia numerico compreso tra ]0,255[*/
    if(H<=0||H>=255){
        printf("errrore: il numero deve essere compreso tra ]0,255[\n");
        exit(3);
    }
    
    srand(time(NULL));

    /*creo il file temporaneo*/
    creat("/tmp/creato", MODE);

    printf("padre (PID: %d) inia a generare %n figli\n", getpid(), N);

    /*vettore di pipe per comunicazione tr padre e gli N figli*/
    int **piped=calloc(N, sizeof(int[2]));
    if(!piped){
        printf("errore: allocazione di memoria non riuscita\n");
        exit(4);
    }

    /*creo le pipe*/
    for(int i=0; i<N; i++){
        if(pipe(piped[i])<0){
            printf("errore: creazione della pipe tra il padre e il figlio di indice %d non riuscita\n",i);
            exit(5);
        }       
    }
    /*contiene il process id ottenuto dalla fork*/
    int pid;

    /*ciclo per la creazione dei processi figli*/
    for(int i=0; i<N;i++){
        /*contiene la posizione in argv del file da assegnare al figlio di indice i*/
        int k=i+1;
        if((pid=fork())<0){
            pritnf("errore: creazione del processo figlio non riuscita\n");
            exit(6);
        }

        /*codice figlio*/
        if(pid==0){
            printf("sono il figlio %d con PID: %d\t file: %s\n", i,getpid(),argv[k]);
            
            /*contien il singolo carattere letto*/
            char c;
            /*inizia la lettura*/

        }

    }
}