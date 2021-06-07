#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#define MSGSIZE 5
char inpbuf [MSGSIZE];

int main(int argc, char **argv){
    int j, piped[2];
    char mess[MSGSIZE];

    if(argc != 2){
        perror("errore: devi passare un parametro\n");
        exit(1);
    }
    
    if(pipe(piped)<0){
        printf("errore nella pipe\n");
        exit(2);
    }
    int pid=fork();
    if(pid<0){
        printf("errore nella fork\n");
        exit(3); 
    }
    if(pid==0){
        /*figlio*/
        int fd;
        if((fd = open(argv[1], O_RDONLY)) < 0){
                printf("Errore in apertura file %s\n", argv[1]);
                exit(-1); /* torniamo al padre un -1 che sara' interpretato identificato come errore */
        }
        printf("Figlio %d sta per iniziare a scrivere una serie di messaggi, ognuno di lunghezza %d,sulla pipe dopo averli letti dal file passato come parametro\n", getpid(), MSGSIZE);
        j=0; /* il figlio inizializza la sua variabile j per contare i messaggi che ha mandato al padre */
        while (read(fd, mess, MSGSIZE)){ /* il contenuto del file e' tale che in mess ci saranno 4 caratteri e il terminatore di linea */
            /* il padre ha concordato con il figlio che gli mandera' solo stringhe e quindi dobbiamo sostituire il terminatore di linea con il terminatore di stringa */
            mess[MSGSIZE-1]='\0';
            write (piped[1], mess, MSGSIZE);
            j++;
        }
        printf("Figlio %d scritto %d messaggi sulla pipe\n", getpid(), j);
        exit(0);
    }
    /* padre */
    printf("Padre %d sta per iniziare a leggere i messaggi dalla pipe\n", getpid());
    j=0; /* il padre inizializza la sua variabile j per verificare quanti messaggi ha mandato il figlio*/
    while (read ( piped[0], inpbuf, MSGSIZE)){
        /* dato che il figlio gli ha inviato delle stringhe, il padre le puo' scrivere direttamente con una printf */
        printf ("%d: %s\n", j, inpbuf);
        j++;
    }
    printf("Padre %d letto %d messaggi dalla pipe\n", getpid(), j);
    /* padre aspetta il figlio */
    int status;
    int pidFiglio = wait(&status);
    if (pidFiglio < 0){
        printf("Errore wait\n");
        exit(5);
    }
    if ((status & 0xFF) != 0)
        printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
    else{
        int ritorno=(int)((status >> 8) & 0xFF);
        printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
    }
    return EXIT_SUCCESS;
}
