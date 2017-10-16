// Practica tema 7, Tejaxun Xicon, Erick Roberto
//  Arquitectura de redes y servicios. Cliente TFTP (UDP)

#include<sys/types.h> 	// Primitiva socket(),
#include<sys/socket.h>  // Primitiva socket(),
#include<stdio.h>	// Uso de perror(),
#include<errno.h>	// Uso de errno(), perror(),
#include<stdlib.h>	// Uso de EXIT_FAILURE.
#include<netinet/ip.h>  // Para definir estructura sockaddr_in 
#include<netinet/in.h>  // Uso de inet_aton().
#include<arpa/inet.h>	// Uso de inet_aton().
#include<netdb.h>	// Uso de getservbyname().
#include<string.h> 	// Concatenacion.
#include<sys/unistd.h>  // Close
 

#define SIZE_MAX_ 512
#define modo_t "octet"
int descriptor_socket=0; //Descriptor del socket.
int enlace=0;		 //Descriptor del enlace con el socket y este host.
int modo=1;		 //Para identificar el modo de manejo de archivo. 1=Lectura, 2=Escritura.
int detalle=0;		 // Para determinar si se muestra el detalle de la operacion, o no. 0=No, 1=si.
char mensaje_in[SIZE_MAX_]; //Buffer de entrada;
char mensaje_out[SIZE_MAX_];//Bufer de salida.
char * aux;
int main(int argc, char * argv[])
{
	//Modo de ejecucucion:tftp-client ip-servidor {-r|-w} archivo [-v]
	
	if(argc<4) // Si no se ingresan los parametros minimos se aborta el programa.
	{
		printf("tftp-client ip-servidor {-r|-w} archivo [-v]\n");
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
	struct sockaddr_in direccion_servidor;
	direccion_servidor.sin_family  = AF_INET;	// Familia socket IP.
	bzero(&(direccion_servidor), 8);
	// Obteniendo la direccion ip del servidor.
	struct hostent *host;
	if((host=gethostbyname(argv[1])) == NULL)
	{
		perror("gethostbyname()");
		exit(EXIT_FAILURE);
	}
	direccion_servidor.sin_addr.s_addr = ((struct in_addr *)host->h_addr)->s_addr;
	// Para	obtener el puerto asociado al servicio tftp usamos funcion getservbyname.
	struct servent* servicio = getservbyname("tftp", "udp");
 	if(servicio == NULL)
	{
		perror("getservbyname()");
		exit(EXIT_FAILURE);
	}	
	direccion_servidor.sin_port = servicio->s_port;
	//direccion_servidor.sin_port = htons(69);
	
	//Determinamos en que modo se va a manejar el archivo.
	char auxiliar[2];
	strcpy(auxiliar,argv[2]);
	if(auxiliar[1] == 'r')
	{
		printf("Modo lectura.\n");
		modo=1;
	}
	else if (auxiliar[1] == 'w')
	{
		printf("Modo escritura.\n");
		modo=2;
	}
	else
	{
		printf("Modo no admitido.\n");
		exit(EXIT_FAILURE);
	}

	//Determinamos si se van mostrar los detalles de la operacion, o no.
	
	if(argc==5)
	{
		printf("Modo detallado\n");
		detalle=1;
	}
	
	//Preparamos el primer mensaje a enviar.
	//Primer limpiamos el array del mensaje.
	int contador=0;
	for(contador=0;contador<SIZE_MAX_;contador++)
	{
		mensaje_out[contador]='\0';
	}
	// Ingresamos el codigo de operacion.
	
	*(short *)mensaje_out = htons(modo);
	aux = mensaje_out + 2;
	strcpy(aux,argv[3]);
	aux += strlen(argv[3]) + 1;
	strcpy(aux,modo_t);
	aux += strlen(modo_t) + 1;
	//Concatenamos el nombre del archivo.
	printf("Mensaje:\t-%s-\n",aux);
	//Ahora enviamos el mensaje al servidor.
	
	if((sendto(descriptor_socket,mensaje_out,aux-mensaje_out,0,(struct sockaddr *)&direccion_servidor, sizeof(struct sockaddr)))<0)
	{
		perror("sendto()");
		exit(EXIT_FAILURE);
	}
	printf("Mensaje enviado\t %s\n",mensaje_out);	
	
	FILE *archivo;
	archivo= fopen(argv[3],"w");
	//Ahora esperamos que nos venga el primer mensaje;
		struct sockaddr_in emisor;
		int tamanho=sizeof(struct sockaddr);
		int size_=0;
		if((size_ = recvfrom(descriptor_socket, mensaje_in, SIZE_MAX_, 0, (struct sockaddr *)&emisor, (socklen_t *)&tamanho))<0)
		{
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}
		printf("IP emisor\t %s\n",inet_ntoa(emisor.sin_addr));
		printf("Tamano paquete:\t%d\n",size_);
		printf("Puerto emisor\t%i\n",emisor.sin_port);
		mensaje_in[size_]='\0';
		fwrite(mensaje_in,2,size_,archivo);
		printf("Mensaje recibido: \t %s\n", mensaje_in);
		fclose(archivo);
		close(descriptor_socket);
	return 0;
}
