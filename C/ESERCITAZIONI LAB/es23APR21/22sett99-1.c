#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv){
	int fd;
	char c;
	if (argc -1 != 3){
		printf("errore: devi passare esattamente tre parametri");
		exit(1);
	}

	if((fd=open(argv[1], O_RDWR))<0){
		printf("errore: file non esistente o problemi nell'apertura");
			exit(2);
	}

	if(strlen(argv[2]) != 1){
		printf("errore il secondo parametro deve essere un singolo carattere");
		exit(3);
	}

	if(strlen(argv[3]) != 1){
                printf("errore il terzo parametro deve essere un singolo carattere");
                exit(3);
        }
	
	char sost = argv[3][0];
	while(read(fd,&c,1)){
		if(c==argv[2][0]){
			lseek(fd,-1L, SEEK_CUR);
			write(fd, &sost, 1);
		}
	}
	return EXIT_SUCCESS;
}
