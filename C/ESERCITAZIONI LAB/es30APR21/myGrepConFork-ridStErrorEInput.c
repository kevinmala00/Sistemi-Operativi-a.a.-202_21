#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
/* FILE: myGrepConFork.c */
int main (int argc, char** argv)
{
    if (argc != 3){
        printf("Errore nel numero di parametri che devono essere due (stringa da cercare e nome del file dove cercare)\n");
        exit(1);
    }

    int pid=fork();
    if(pid<0){
        printf("errore nella fork\n");
        exit(2);
    }

    /*figlio*/
    if(pid==0){
        printf("pid figlio: %d\n",getpid());

        /*ridirigo stdout e stder in /dev/null*/
        close(1);
        open("/dev/null", O_WRONLY);
        close(2);
        open("/dev/null", O_WRONLY);
        close(0);
        open(argv[2],O_RDONLY);

        execlp("grep","grep", argv[1], argv[2], (char *)0);

        /*esegue qui solo in caso di errore, facciamo ritornare -1 al padre
        se no ritorn l'esisto della grep*/

        exit(-1);
    }
    int status;
    int pidf=wait(&status);

    if(pidf<0){
        printf("errore wait\n");
        exit(4);
    }

    if((status & 0XFF) != 0){
        printf("errore figlio temrinato in modo anomalo\n");
        exit(5);
   }
   int exitf= (status>>8) & 0XFF;
   printf("il figlio con pid: %d ha ritornato %d\n", pidf, exitf);

   return EXIT_SUCCESS;
}