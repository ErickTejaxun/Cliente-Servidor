/*Practica tema 8, Tejaxun Xicon Erick Roberto*/

#include "estructuras.h"
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<signal.h>	//Para uso de signal.
#include<string.h>	//Para concatenar cadenas.
#define SIZE 1024


int socket_this=0;	//Socket de este servicio.
int socket_cliente=0;	//Socket para algun cliente.
int enlace=0;
int apertura=0;
char Mensaje[SIZE];
char buffer[SIZE];
FILE *fichero;

void signal_handler(int signal)//Metodo para terminal el proceso de forma correcta.
{
	if(signal==SIGINT)
	{
		printf("\n Apagando servidor y cerrando sockets \n");
		shutdown(socket_this,SHUT_RDWR);
		shutdown(socket_cliente,SHUT_RDWR);
		close(socket_this);
		close(socket_cliente);
		exit(1);
	}

}

void (*signal(int signum,void(*signal_handler)(int)))(int);

int main(int argc,char *argv[])
{
	ECHORequest echoRequest;
	ECHOResponse echoRespone;


	signal(SIGINT,signal_handler);
	struct sockaddr_in mydireccion;		//Estructura con los datos de este servicio.
	mydireccion.sin_family=AF_INET;		//Familia ipV4
	mydireccion.sin_addr.s_addr=INADDR_ANY;	//Para que obtenga la direccion ip automaticamente.
	if(argc<=1)		//Verificamos si se ha ingresado el numero de puerto, sino le asignamos el del servidor Daytime.
	{
		struct servent *servicio=getservbyname("daytime","tcp");
		mydireccion.sin_port=servicio->s_port;
		printf("Puerto del servicio \t%i\n",mydireccion.sin_port);
	}
	else
	{
		int auxiliar=0;
		auxiliar=atoi(argv[2]);
		mydireccion.sin_port=htons(auxiliar);
	}

	socket_this=socket(AF_INET,SOCK_STREAM,0);
	if(socket_this<0) // S9i no se pudo crear el socket, abortamos el programa.
	{
		perror("socket()");
		exit(EXIT_FAILURE);
	}
	printf("ID socket\t%i\n",socket_this);
	printf("No. puerto\t%i\n",mydireccion.sin_port);
	enlace=bind(socket_this,(struct sockaddr *)&mydireccion,sizeof(mydireccion));
	if(enlace<0) // Si no se pudo crear el enlace, abortamos el programa.
	{
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	printf("ID enlace \t%i\n",enlace);
	apertura=listen(socket_this,10);

	if(apertura<0)  // Si ha habido error al crear la apertura de escucha, se aborta el programa.
	{
		perror("listen()");
		exit(EXIT_FAILURE);
	}
	printf("ID apertura de escucha \t%i\n",apertura);

	struct sockaddr_in direccion_cliente; //Aca se guardaran los datos del cliente.
	socklen_t addrsize;
	addrsize=sizeof(struct sockaddr_in);
	while(1)
	{
		addrsize=sizeof(struct sockaddr_in);
		if((socket_cliente= accept(socket_this,(struct sockaddr *)&direccion_cliente,&addrsize))<0)
		{
			perror("accept()");
			exit(EXIT_FAILURE);
		}
		int contador=0;
		for(contador=0;contador<SIZE;contador++){Mensaje[contador]='\0';} //LImpiar cadena.
		printf("Direccion cliente:\t%s\n",inet_ntoa(direccion_cliente.sin_addr));
		gethostname(Mensaje,10);
		system("date> /tmp/tt.txt");
		fichero=fopen("/tmp/tt.txt","r");
		if(fgets(buffer,SIZE,fichero)==NULL)
		{
			printf("Error en system, en fopen o en fgets\n");
			exit(EXIT_FAILURE);
		}
		//printf("%s \n",buffer);
		strcat(Mensaje," \t");
		strcat(Mensaje,buffer);
		printf("%s\n",Mensaje);
		fclose(fichero);
		send(socket_cliente,Mensaje,SIZE,0);
		//shutdown(socket_cliente,SHUT_RDWR);
		//close(socket_cliente);
	}

	return 0;
}

