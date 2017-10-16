/*Practica 1, Tejaxn, Xicn Erick*/
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <string.h>

#define SIZEBUFF 1024
char mensaje_recibido[SIZEBUFF];
int socket_=0; // Descriptor del socket;

int main(int argc, char *argv[])
{
	if(argc<2)
	{
		printf("./daytime-udp-client direccion.ip.server [-p puerto_server]\n");
		exit(EXIT_FAILURE);
	}

	socket_= socket(AF_INET,SOCK_DGRAM,0);//Creamos un socket para la conexin.
	if(socket_<0) //Verificamos que no haya error en el socket.
	{
		    perror("socket()");
		        exit(EXIT_FAILURE);
	}
	struct sockaddr_in myaddr; //Direccion del host cliente.
	myaddr.sin_family= AF_INET;
	myaddr.sin_addr.s_addr= INADDR_ANY;
	/*myaddr.sin_port=0;
	 * inet_aton(argv[1],&myaddr.sin_addr);*/

	struct sockaddr_in servidor; //Direccion del servidor.
	servidor.sin_family = AF_INET;
	inet_aton(argv[1],&servidor.sin_addr);
	if(argc>3)
	{
		int auxiliar;
		printf("%s-- \n",argv[3]);
		sscanf(argv[3],"%d",&auxiliar);
		printf("%s-- \n",argv[3]);
		servidor.sin_port=htons(auxiliar);
	}
	else
	{
		struct servent *s=getservbyname("daytime","udp");
		servidor.sin_port=s->s_port;
		//printf("%s  %d ----\n",s->s_name,htons(s->s_port));
	}

	if(bind(socket_, (struct sockaddr *) &myaddr, sizeof(myaddr))<0) //Si hay error al enlazar, abortamos el programa.
	{
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	char *mensaje="Hola\0";
	if(sendto(socket_,mensaje,sizeof(mensaje),0,(struct sockaddr *)&servidor,sizeof(servidor))<0)
	{
		perror("sendto()");
		exit(EXIT_FAILURE);
	}
	socklen_t tamanho = sizeof(struct sockaddr_in);
	if(recvfrom(socket_,mensaje_recibido,100,0,(struct sockaddr *)&servidor,&tamanho)<0)
	{
		perror("recvfrom()");
		exit(EXIT_FAILURE);
	}

	printf("%s\n",mensaje_recibido);
	close(socket_);
	return 0;
}


