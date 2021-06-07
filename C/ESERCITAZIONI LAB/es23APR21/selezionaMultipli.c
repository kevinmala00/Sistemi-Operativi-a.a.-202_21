#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

void selezionamultipli(int fd, int num){
	char c;
	int pos =0;
	while(read(fd,&c,1)){
			if(pos >= num){
				if(pos%num == 0){
				printf("pos: %d-esima, elemento: %c\n",pos,c);
				}
			}
			pos++;
	}
}


int main(int argc, char **argv){

	if(argc -1 != 2){
		printf("errore: devi passare esattamente due parametri\n");
		exit(1);
	}

	int fd;
	if((fd=open(argv[1],O_RDONLY))<0){
		printf("errore: file no esistente o proiblei in apertura\n");
		exit(2);
	}
	
	if(strlen(argv[2])!=1){
		printf("errore: il secondo parametro deve essere un unico carattere numerico\n");
		exit(3);
	}
	
	int num = argv[2][0] -'0';
	selezionamultipli(fd,num);

	close(fd);
}
