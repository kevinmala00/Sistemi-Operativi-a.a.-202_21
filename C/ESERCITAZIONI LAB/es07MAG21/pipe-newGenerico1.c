#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#define MAX 512

int main(int argc, char **argv){

    if(argc!=2){
        printf("errore: devi passare 1 parametro\n");
        exit(1);
    }

    int piped[2];
    if (pipe(piped)<0){
        printf("errore: nella creazione della pipe\n");
        exit(2);
    }

    int pid= fork();
    if(pid<0){
        printf("errore nella fork\n");
        exit(3);
    }
    
    if(pid==0){
        /*figlio*/
        close(piped[0]);
        int fd=open(argv[1], O_RDONLY);
        if(fd<0){
            printf("errore nell'apertura del file\n");
            exit(-1);
        }

        printf("figlio con PID: %d inizia scrittura\n", getpid());

        int i=0;
        char c;
        char str[MAX];
        int count=0;
        while(read(argv[1],&c,1)){
            if(c=='\n'){
                str[count]='\0';
                write(piped[1],&count,sizeof(count));
                write(piped[1],str,count);
                count=0;
                i++;
            }
            str[count]=c;
            count++;
        }

        printf("il figlio PID: %d ha scritto %d linee\n",getpid(),i);
        exit(0);

    }

    /*padre*/
    close(piped[1]);
     printf("padre con PID: %d inizia lettura\n", getpid());
    int j=0;
    int lun;
    char str1[MAX];
    while(read(piped[0],&lun,sizeof(lun))){
        read(piped[0], str1, lun);
        printf("%d: %s\n",j,str1);
        j++;
    }
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
