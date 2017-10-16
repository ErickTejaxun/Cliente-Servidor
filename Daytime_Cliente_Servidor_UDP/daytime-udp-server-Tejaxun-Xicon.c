/*Practica 1, Tejaxn, Xicn Erick*/
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SIZEBUFF 512

char buffer[SIZEBUFF]; // Mensaje a enviar.
char mensaje_salida[SIZEBUFF];    // Mensaje recibido.
FILE *fichero; // Fichero temporal con datos del servidor, fecha y hora.
int err;
int socket_=0;// Descriptor del socket.

char temporal[100];
/*daytime-udp-server-Apell1-Apell2 [-p puerto-servidor]*/


int main(int argc, char *argv[])
{
	struct sockaddr_in cliente;// Direccion del cliente.

	struct sockaddr_in mydireccion;// Direccion de este mensaje_salida.
	mydireccion.sin_family= AF_INET;
	mydireccion.sin_addr.s_addr = INADDR_ANY;

	/*Asignamos el puerto.*/
	if(argc<3)// Si el usuario no indica el puerto se asigna automaticamente.
	{
		struct servent *s=getservbyname("daytime","udp");
		mydireccion.sin_port=s->s_port;
	}
	else // Si el usuario indica el puerto.
	{
		int auxiliar;
		sscanf(argv[2],"%d",&auxiliar);
		mydireccion.sin_port=htons(auxiliar);
	}

	socket_=socket(AF_INET,SOCK_DGRAM,0);
	if(socket_<0) // Si hubo un error al crear el socket, abortamos el programa.
	{
		perror("Socket()");
		exit(EXIT_FAILURE);
	}

	if(bind(socket_,(struct sockaddr *)&mydireccion,sizeof(mydireccion))<0)
	{
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		socklen_t tamanho = sizeof(struct sockaddr_in);
		if(recvfrom(socket_,buffer,SIZEBUFF,0,(struct sockaddr *)&cliente,&tamanho)<0)
		{
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}
		printf("-----Nueva Conexion-------\n");
		printf("Cliente: Puerto:%i\t Direccion:%s\t",cliente.sin_port,inet_ntoa(cliente.sin_addr));

		/*Limpiamos los buffers*/
		int contador=0;
		for(contador=0;contador<100;contador++){temporal[contador]='\0';}
		for(contador=0;contador<SIZEBUFF;contador++){buffer[contador]='\0';}
		/*Primero obtenemos el nombre del mensaje_salida*/
		gethostname(temporal,10); // Guardamos el nombre del host.

		/*Luego la hora*/
		system ("date> /tmp/tt.txt");
		fichero = fopen("/tmp/tt.txt","r");
		if (fgets(buffer,SIZEBUFF,fichero)==NULL) 
		{
			printf("Error en system(), en fopen(), o en fgets()\n");
			exit(EXIT_FAILURE);
		}
		printf("DateTime:%s\n",buffer);
		strcat(temporal,"\t");
		strcat(temporal,buffer);
		/*enviamos el mensaje;*/
		if(sendto(socket_, temporal,100,0,(struct sockaddr *)&cliente,sizeof(cliente))<0)
		{
			perror("sendto()");
			exit(EXIT_FAILURE);
		}
	}


	return 0;
}


