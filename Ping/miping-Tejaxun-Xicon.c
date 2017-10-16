// Practica tema 8, Tejaxun Xicon, Erick Roberto
//  Arquitectura de redes y servicios. Cliente TFTP (UDP)

#include<stdio.h>	// Uso de perror(),
#include<errno.h>	// Uso de errno(), perror(),
#include<stdlib.h>	// Uso de EXIT_FAILURE.
#include<netinet/in.h>  // Uso de inet_aton().
#include<string.h> 	// Concatenacion.
#include "estructuras.h" 
#define type 8
#define code 0
#define sequence 0

int descriptor_socket=0;
int enlace= 0;
int main(int argc, char * argv[])
{
	//Modo de ejecucucion:tftp-client ip-servidor {-r|-w} archivo [-v]
	
	if(argc<2) // Si no se ingresan los parametros minimos se aborta el programa.
	{
		printf("./miping ip-destino [-v]\n");
		exit(EXIT_FAILURE);
	}

	// Creamos el socket a usar;
	descriptor_socket= socket(AF_INET,SOCK_DGRAM,0);

	if(descriptor_socket<0) // Si hay algun error al crear el socket, se aborta el programa.
	{
		perror("socket()");
		exit(EXIT_FAILURE);
	}
	
	
	// Creamos la estructura con los datos de este host.
	struct sockaddr_in midireccion;
	midireccion.sin_family = AF_INET;		// Familia IPv4
	midireccion.sin_addr.s_addr = INADDR_ANY;	// Toma la ip de este host.
	bzero(&(midireccion.sin_zero), 8);
	// Creamos el enlace entre el socket con la direccion de este host.
	enlace= bind(descriptor_socket, (struct sockaddr *)&midireccion, sizeof(struct sockaddr));	
	if(enlace<0) // Si hubo un error al crear el enlace, se aborta el programa.
	{
		perror("bind()");
		exit(EXIT_FAILURE);
	}
	
	// Creamos la estructura con los datos del servidor.
	/*struct sockaddr_in direccion_servidor;
	direccion_servidor.sin_family  = AF_INET;	// Familia socket IP.
	*/
	
	ECHORequest echoRequest;
	ECHOResponse echoResponse;
	
	echoRequest.icmpHeader.Type = type;
	echoRequest.icmpHeader.Code = code;
    	echoRequest.ID = getpid()/(256*256);
	echoRequest.seqNumber = 0%(256*256);
	char mensaje[10];
	strcpy(mensaje,"Mensaje1");
	close(descriptor_socket);
	
	if((sendto(descriptor_socket,echoRequest.icmpHeader,sizeof(echoRequest.icmpHeader),0,))<0)
	{
	}

	return 0;
}
