// Practica tema 8, Tejaxun Xicon Erick Roberto.

#include<stdio.h>
#include<sys/types.h>



#define type 8
#define code 0
#define sequence 0

int descriptor_socket=0;
int main(int argc, char * argv[])
{

	if(argc<2) // Verificamos si se ha ingresado la direccion IP
	{
		printf("./miping direccion-ip [-v]\n");
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in dir_dest;
	dir_dest.sin_family = AF_INET;
	struct hos	
}
