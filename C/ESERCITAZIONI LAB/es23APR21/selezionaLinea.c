#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

void selezionalinea(int fd, int num){
    char c;
    int count=0,lung;
    char *str= malloc(sizeof(*str));
    while(1){
        for(int i=0; read(fd, &c, 1);i++){
            if(c=='\n'){
                break;
            }
            if(!str){
                lung=1;
            }
            else{
                lung =strlen(str);
            }
         
            str=realloc(str,lung+1);
            str[i]=c;
        }
        count++;

        if(!str){
            break;
        }
        

        if(count==num){
            puts(str);
            free(str);
            return;
        }

        str=NULL;
    }
    puts("linea non esistente\n");
}

int main(int argc, char **argv){

    if(argc -1!=2){
        printf("errore: devi passare esattamente due parametri\n");
        exit(2);
    }

    int fd;
    char *filename= argv[1];
    if((fd = open(filename, O_RDONLY))<0){
        printf("errore: file non esistente o errori in apertura\n");
        exit(2);
    }


    int num=atoi(argv[2]);
    if(num<=0){
        printf("errore: il secondo parametro deve esser un numero strettamente posistivo\n");
        exit(3);
    }

    selezionalinea(fd, num);

    close(fd);
    return EXIT_SUCCESS;
}