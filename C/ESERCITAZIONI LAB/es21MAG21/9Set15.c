#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

typedef int pipe_t[2];  /*definisco il tipo pipe_t come vettore di 2 interi*/

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
		if (pid < 0){
		    printf("Errore: problemi nella wait\n");
		    exit(err);
		}

		if ((status & 0xFF) != 0)
    		printf("%s con pid %d terminato in modo anomalo\n",str, pid);
    	else{ 
                ritorno=(int)((status >> 8) & 0xFF); 
		        if (ritorno!=0)
 			            printf("Il %s con pid=%d ha ritornato %d si sono riscontrati errori\n", str, pid, ritorno);
		        else  
                        printf("Il %s con pid=%d ha ritornato %d\n", str, pid, ritorno);
		        }
        return ritorno;
}

int main(int argc, char** argv){

    int N;              /*numero di file passati*/
    pipe_t *pipe_fp;    /*pipe di comunicazione figlio padre*/
    pipe_t *pipe_pf;    /*pipe di comunicazione padre figlio*/
    int *pid_vet;        /*vettore che conterrà i pid dei figli*/
    int fd;             /*file descriptor*/
    int *confrontocaratteri;          /*contiene 1 o 0 per ogni figlio che dindica se il apdre deve indicare di leggere*/
    char mess;          /*messaggio dal padre*/
    char s = 's';        /*carattere da inviare al figlio, s continua n fermati*/
    char c;             /*carattere letto dal figlio dal file*/
    char car;           /*carattere letto dal padre dal file*/
    N = argc-2;

    /*controllo sul numero di parametri*/
    if(argc -1 <3){
        printf("errore:devi passare almeno 3 parametri\n");
        exit(1);
    }

    /*allocazione del vettore dinamico contenente le N pipe di comunicazione figlio_padre*/
    pipe_fp=calloc(N, sizeof(*pipe_fp));
    if(!pipe_fp){
        printf("errore: allocazione di memoria pipe figlio padre non riuscita\n");
        exit(2);
    }

    /*allocazione del vettore dinamico contenente le N pipe di comunicazione padre_figlio*/
    pipe_pf=calloc(N, sizeof(*pipe_pf));
    if(!pipe_pf){
        printf("errore: allocazione di memoria  pipe padrefilgio non riuscita\n");
        exit(3);
    }

    /*creazione delle N pipe figlio_padre*/
    for(int i=0; i<N; i++){
        if(pipe(pipe_fp[i])<0){
            printf("errore: creazione della pipe  figlio padre non riuscita\n");
            exit(4);
        }
    }

    /*creazione delle N pipe padre_figlio*/
    for(int i=0; i<N; i++){
        if(pipe(pipe_pf[i])<0){
            printf("errore: creazione della pipe figlio padre non riuscita\n");
            exit(5);
        }
    }
    
    /*alloco il vettore di pid*/
    pid_vet=malloc(N*sizeof(*pid_vet));
    if(!pid_vet){
        printf("errore: allocazione di memoria vettore di pid non riuscita\n");
        exit(6);
    }

    /*alloco il vettore di confronto*/
    confrontocaratteri=malloc(N*sizeof(*confrontocaratteri));
    if(!confrontocaratteri){
        printf("errore: allocazione di memoria vettore confronto non riuscita\n");
        exit(7);
    }

    /*inizialmente setto tutti i valori a 1*/
    confrontocaratteri=memset(confrontocaratteri, 1, N*sizeof(*confrontocaratteri));

    /*apertura del file assegnato al padre*/
    fd=open(argv[argc-1],O_RDONLY);
    if(fd<0){
        printf("errore: apertura file %s non riuscita\n",argv[argc-1]);
        exit(8);
    }

    /*creo gli N figli salvando i loro pid*/
    for(int i=0; i<N; i++){
        if((pid_vet[i]=fork())<0){
            printf("errore: creazione del processo figlio %d non rieuscita\n", i);
            exit(9);
        }

        /*codice figlio*/
        if(pid_vet[i]==0){
            printf("sono il figlio %d con PID: %d\tfile: %s\n",i,getpid(),argv[i+1]);

            /*chiusura pipe
            figlio padre
                tutte lato lettura, 
                tutte tranne il mio indice in lato scrittur
            padre figlio
                tutte lato scrittura
                tutte tranne mio indice lato lettura
            */
            for(int j=0; j<N; j++){
               close(pipe_fp[j][0]);
               close(pipe_pf[j][1]);
               if(j!=i){
                    close(pipe_fp[j][1]);
                    close(pipe_pf[j][0]);
               }
           }
           
            /*apertura file*/
            fd=open(argv[i+1],O_RDONLY);
            if(fd<0){
               printf("FIGLIO indice %d: errore: apertura del file %s non riuscita\n",i,argv[i+1]);
               exit(-1);
           }
            /*inizio lettura se messaggio dal padre è s continuo, n mi fermo*/
            while(read(pipe_pf[i][0], &mess, 1)){
                //printf("\tPROCESSO di indice %d e pid=%d letto carattere %c dal padre\n",i,getpid(),mess);
                if(mess=='n'){
                    break;
                }
                read(fd,&c,1);
                write(pipe_fp[i][1],&c,1);
                //printf("\tPROCESSO di indice %d e pid=%d scritto al padre carattere %c\n",i,getpid(),c);
            }
            /*terminata la lettura dei caratteri del file esco con successo, 
            oppure ricevuto il messaggio 'n'*/
            exit(0);
        }
    }      

    /*codice padre*/
    /*chiusura delle pipe
    padre figlio 
        tutte lato lettura
    figlio padre
        tutte lato scrittura
    */  

    for(int i=0;i<N;i++){
        close(pipe_fp[i][1]);
        close(pipe_pf[i][0]);
    }

    /*leggo da file e confornto con quelli dei figli se diverso lo segno nel vettore confornto caratteri*/
    while(read(fd,&car,1)){
        //printf("PADRE letto carattere %c dal file\n",car);
        for(int i=0; i<N; i++){
            if(confrontocaratteri[i]){
                write(pipe_pf[i][1], &s, 1);
                /*leggo dal figlio*/
                read(pipe_fp[i][0],&c,1);
                //printf("\tPADRE letto dal figlio di indice %d carattere %c\n", i,c);
                if(c!=car){
                    confrontocaratteri[i]=0;
                }
            }
        }
    }

    /*ammazzo tutti i figli con confrotocaratteri a 0*/
    for(int i=0; i<N; i++){
        if(!confrontocaratteri[i]){
            if(kill(pid_vet[i],SIGKILL)==-1){
                printf("errore: kill del figlio %d fallita, vuol dire che era già terminato\n",i);
            }
        }else{
            /*faccio terminale con successo i figli con confrontocaratteri =1*/
            s='n';
            write(pipe_pf[i][1], &s, sizeof(s));
        }
    }

    for(int i=0; i<N; i++){
        waiting("figlio", 10);
    }
    exit(0);
}