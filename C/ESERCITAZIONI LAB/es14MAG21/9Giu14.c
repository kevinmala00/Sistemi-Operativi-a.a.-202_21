#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

typedef int pipe_t[2];

int waiting(const char *str, int err){
        int status;
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

    /*contiene il numero dei file passati come parametri*/
    int N=argc-1;

    /*controllo sul numero dei parametri, devono essere almeno 2*/
    if(N<2){
        printf("errore: devi passare almeno due parametri\n");
        exit(1);
    }

    /*controllo sul tipo di parametri devono essere file*/
    for(int i=0; i<N; i++){
        int fd = open(argv[i+1],O_RDONLY);
        if(fd<0){
            printf("errore: file %s non apribile\n",argv[i+1]);
            exit(2);
        }
    }

    /*array dinamico di N pipe per comunicazione padre-figlio*/
    pipe_t *piped=calloc(N, sizeof(*piped));
    if(!piped){
        printf("errore: allocazione di memoria non riuscita\n");
        exit(3);
    }

    /*creo le N pipe*/
    for(int i=0; i<N; i++){
        if(pipe(piped[i])<0){
            printf("errore: creazione della pipe non riuscita\n");
            exit(4);
        }
    }

    printf("PID padre: %d inizio a generare gli %d figli\n", getpid(), N);

    /*process identifier utilizzato per le fork*/
    int pid;

    /*pipe di comunicazione figlio nipote*/
    pipe_t p;

    /*generazione dei figli*/
    for(int i=0; i<N; i++){
        if((pid=fork())<0){
            printf("errore: creazione del processo figlio non riuscita\n");
            exit(5);
        }

        /*processo figlio*/
        if(pid==0){
            printf("PID figlio %d: %d\t file: %s\n", i, getpid(), argv[i+1]);

            /*chiusura delle pipe da parte del figlio, deve rimanere aperta solo la parte [1]
            di indice corrispondente al figlio j=i
            rimane aperta solamente la piped[i][1]*/
            for(int j=0; j<N; j++){
                close(piped[j][0]);
                if(j!=i){
                    close(piped[j][1]);
                }
            }

            /*creazione pipe tra figlio e nipote*/
            if(pipe(p)<0){
                printf("errore: creazione pipe figlio-nipote non riuscita\n");
                exit(-1);
            }

            /*creazione processo nipote*/
            if((pid=fork())<0){
                printf("errore: creazione processo nipote non riuscita\n");
                exit(-2);
            }

            /*codice processo nipote*/
            if(pid==0){
                printf("nipote del figlio %d PID: %d\t file: %s\n", i, getpid(), argv[i+1]);

                /*chiusura pipe lato lettura non utilizzata dal nipote tra il figlio  e il padre*/
                close(piped[i][1]);

                /*devo chiudere lo standard output del nipote per simulare il piping dei comandi
                in modo che ciò che viene scritto dall'esecuzione del wc raggiunga il figlio*/
                close(1);
                dup(p[1]);

                close(0);
                open(argv[i+1], O_RDONLY);

                /*chiusura di ogni lato della pipe p*/
                close(p[0]);
                close(p[1]);

                /*esecuzione comando wc -l*/
                execlp("wc","wc", "-l", NULL);

                /*se si giunge in questa sezione errore*/
                exit(-1);
            }
        
            
            /*codice figlio*/
            {
            /*chiusura dell lato scrittura con il nipote*/
            close(p[1]);

            /*lunghezza del file passato al nipote ricevuta dalla pipe*/
            int l;

            /*stringa contentente il risultato letto dalla pipe*/
            char str[17];
            int k;
            /*lettura  della pipe tra figlio e nipote*/
               for (k=0; read(p[0],&str[k], sizeof(*str)); k++){
               }
            
            str[k+1]='\0';
            /*converto la str con atoi*/
            l=atoi(str);
            

            
            /*comunica al padre il valore calcolato*/
            write(piped[i][1], &l, sizeof(l));

            /*il figlio aspetta il nipote*/
            int ritorno=waiting("nipote", -5);
            exit(ritorno);
            }
            
        }          
    }

    /*codice padre*/
    /*chiusura di tutti i lati della scrittura con i figli*/
    for(int j=0; j<N; j++){
        close(piped[j][1]);
    }

    int l;
    long s=0;
    /*lettura in ordine delle infomazioni*/
    for(int i=0; i<N;i++){
        read(piped[i][0], &l, sizeof(l));
        printf("filgio di indice %d ha scritto %d\n", i, l);
        s+=l;
    }
    printf("i figli hanno ritornato una somma pari a %ld\n", s);

    /*il padre deve aspettare tutti i figli*/
    for(int j=0; j<N; j++){
        waiting("figlio",5);
    }
    return EXIT_SUCCESS;
}