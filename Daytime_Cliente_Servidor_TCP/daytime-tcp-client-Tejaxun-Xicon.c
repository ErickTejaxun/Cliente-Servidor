
/*Practica tema 6, Tejaxun Xicon Erick Roberto*/
#include<stdio.h>//Impresion en pantalla.
#include<sys/socket.h>  //Socket
#include<sys/types.h>//
#include<errno.h>//Impresion de errores.
#include<stdlib.h>//Conversion
#include<netinet/ip.h>
#include<netdb.h>
#include<netinet/in.h>  //Conversion de ips
#include<arpa/inet.h>   
#include<sys/unistd.h>    //Gethostbyname

#define SIZE 1024
char buffer[SIZE];
int socket_=0;            //Descriptor del socket.
int  enlace=0;
/*struct servent* getservbyname(char *name, char *type);*/


int main(int argc, char* argv[])
{
	if(argc<2)
	{
		printf("cliente IP-Servidor [-p puerto]\n");
		exit(EXIT_FAILURE);
	}

	socket_=socket(AF_INET,SOCK_STREAM,0); //Creamos el socket.
	if(socket_<0)  //En caso de que no se pueda crear el socket, aboratamos el programa.
	{
		perror("socket()\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in mydireccion; //Direccion de este host.
	mydireccion.sin_family = AF_INET;  //Familia ipv4.
	mydireccion.sin_addr.s_addr= INADDR_ANY ;   // Para que obtenga la direccion ip automaticamente.
	mydireccion.sin_port= 0;
	struct sockaddr_in servidor;//Datos del servidor, dados por el usuario.
	servidor.sin_family= AF_INET;//socket ipv4
	/*servidor.sin_port=32;*/
	inet_aton(argv[1],&servidor.sin_addr);
	if(argc<4)
	{
		struct servent *servicio=getservbyname("daytime","tcp");
		servidor.sin_port=servicio->s_port;
		/*printf("Puerto del servicio datytime.....\t%i\n",);*/
	}
	else
	{
		int auxiliar=0;
		sscanf(argv[3],"%d",&auxiliar);//Convertimos de caracter, a entero.
		/*printf("-p\t%i\n",htons(auxiliar));*/
		servidor.sin_port=htons(auxiliar); //Casteamos el dato ingresado. Puerto
	}
	if((bind(socket_,(struct sockaddr *)&mydireccion,sizeof(mydireccion)))<0)//Enlazamos el socket con nuestra direccion. Si hay error, abortamos.
	{
		perror("bind()");
		exit(EXIT_FAILURE);
	}
	/*printf("Puerto del servicio \t %i \n",servidor.sin_port);
	 * printf("IP\t%S\n",inet_ntoa(servidor.sin_addr));*/
	enlace=connect(socket_,(struct sockaddr *)&servidor,sizeof(servidor));
	if (enlace<0) //Si no se ha podido crear un enlace con el servidor, abortamos el programa.
	{
		perror("connect()");
		exit(EXIT_FAILURE);
	}
	/*printf("Se ha conectado con el servidor \n");*/
	int aux=0;
	if((aux=recv(socket_,buffer,SIZE,0))<0)
	{
		perror("recv()");
		exit(EXIT_FAILURE);
	}
	/*buffer[aux]='\0';*/
	printf("%s\n",buffer);
	shutdown(socket_, SHUT_RDWR);
	/*Verificamos el cierre de la conexion, recibiendo el paquete con 0 bytes.*/
	if((aux= recv(socket_,buffer,SIZE,0))<0)
	{
		perror("recv()");
		exit(EXIT_FAILURE);
	}

	if(aux == 0)
	{
		/*printf("Cerrando conexion.\n");*/
		if(close(socket_)<0)
		{
			perror("close()");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		printf("Error, no se ha podido cerrar la conexion.\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}


