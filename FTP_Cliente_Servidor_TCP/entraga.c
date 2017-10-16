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
 

#define SIZE_MAX_ 512      //Tamanho maximo de cada paquete.
#define modo_t "octet"    //Modo de transmicion.

int descriptor_socket=0; //Descriptor del socket.
int enlace=0;		 //Descriptor del enlace con el socket y este host.
int modo=1;		 //Para identificar el modo de manejo de archivo. 1=Lectura, 2=Escritura.
int detalle=0;		 // Para determinar si se muestra el detalle de la operacion, o no. 0=No, 1=si.
char mensaje_in[SIZE_MAX_]; //Buffer de entrada;
char mensaje_out[SIZE_MAX_];//Bufer de salida.
char * aux;
int numero_paquete=1;
int numblock=1;
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
		//printf("Modo lectura.\n");
		modo=1;
	}
	else if (auxiliar[1] == 'w')
	{
		//printf("Modo escritura.\n");
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
		//printf("Modo detallado\n");
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
	
	//*(short *)mensaje_out = htons(modo);
	mensaje_out[0]= modo/256;
	mensaje_out[1] = modo%256;
	aux = mensaje_out + 2;
	strcpy(aux,argv[3]);
	aux += strlen(argv[3]) + 1;
	strcpy(aux,modo_t);
	aux += strlen(modo_t) + 1;
	//Concatenamos el nombre del archivo.
	//printf("Mensaje:\t-%s-\n",aux);
	//Ahora enviamos el mensaje al servidor.
	
	if((sendto(descriptor_socket,mensaje_out,aux-mensaje_out,0,(struct sockaddr *)&direccion_servidor, sizeof(struct sockaddr)))<0)
	{
		perror("sendto()");
		exit(EXIT_FAILURE);
	}
	if( modo==1 && detalle == 1)
	{
		printf("Enviada solicitud de lectura de  %s a servidor tftp en %s .\n",argv[3],argv[1]);	
	}else if (modo ==2 && detalle ==1 )
	{
		printf("Enviada solicitud de escritura de  %s a servidor tftp en %s .\n",argv[3],argv[1]);	
	}
	FILE *archivo;
	//Ahora esperamos que nos venga el primer mensaje;
	struct sockaddr_in emisor;
	int tamanho = sizeof(struct sockaddr);
	int size_ = 0;
	//Verificamos si no es un paquete de error.
	//Enviamos el paquete RRQ
	size_ = recvfrom(descriptor_socket, mensaje_in, SIZE_MAX_, 0 , (struct sockaddr *)&emisor, (socklen_t *)&tamanho);
	int codigo_op;
	codigo_op = mensaje_in[0]/256;
	codigo_op = mensaje_in[1]%256;
	if(detalle == 1)
	{
		printf("Recibido bloque del servidor tftp.\n");
	}
	if(codigo_op == 5) // Esto significa que hemos recibido un paquete de error.
	{
		char salida[size_ - 4];
		for(contador=0;contador<(size_-4);contador++){salida[contador]=mensaje_in[contador+4];}
		printf("Paquete de error recibido. \t%s\n",salida);
		exit(EXIT_FAILURE);
	}

	if(modo == 1) // Modo de lectura;
	{	
				
		/*struct sockaddr_in emisor;
		int tamanho=sizeof(struct sockaddr);
		int size_=0;
		//Enviamos el paquete RRQ.
		size_ = recvfrom(descriptor_socket, mensaje_in , SIZE_MAX_, 0 , (struct sockaddr *)&emisor, (socklen_t *)&tamanho);
		//Verificamos si no es un paquete de error.
		int codigo_op;
		codigo_op = mensaje_in[0]/256;
		codigo_op = mensaje_in[1]%256;
		printf("OPcode:\t%i\t\n",codigo_op);
		if(codigo_op == 5) // ESto signfica que hemos recibido un paquete de erro.
		{
			printf("Error en el paquete\t%i\n",codigo_op);
			exit(EXIT_FAILURE);
		}*/
		//Si no hay error, significa que recibimos un paquete DATA, creamos el archivo.
		
		archivo = fopen(argv[3],"w");
		fclose(archivo);
		//Ahora lo abrimos en modo "a" , para ir escribiendo al final del archivo cada paquete entrante.
		archivo = fopen(argv[3],"a");
		do
		{
			codigo_op = mensaje_in[0]/256;
			codigo_op = mensaje_in[1]%256;
			//printf("Bloque No. %i recibido.\n",numblock);
			if(codigo_op == 5) // Esto significa que hemos recibido un paquete de error.
			{
				printf("Paquete de error\n");
				exit(EXIT_FAILURE);
			}
			
			
			if(size_<0)
			{
				perror("recvfrom()");
				exit(EXIT_FAILURE);
			}
			char auxiliar[SIZE_MAX_];
			for(contador=0;contador<(size_-4);contador++){auxiliar[contador]=mensaje_in[contador+4];}
			/*printf("IP emisor\t %s\n",inet_ntoa(emisor.sin_addr));
			printf("Tamano paquete:\t%d\n",size_);
			printf("Puerto emisor\t%i\n",emisor.sin_port);*/
			mensaje_in[size_]='\0';
	        	numero_paquete = mensaje_in[2]/256;	 
			numero_paquete = mensaje_in[3]%256;
			if(detalle==1)
			{
				printf("Bloque No. %o recibido. \tPaquete esperado:\t%d\n",numero_paquete,numblock);
			}
			fwrite(auxiliar,1,size_-4,archivo);
			// Ahora enviamos el paquete ACK
			//Limpiamos el buffer
			//for(contador=0;contador<SIZE_MAX_;contador++){mensaje_in[contador] ='\0'; mensaje_out[contador]='\0';}
			char mensaje[4];
			mensaje[0] = 4/256;
			mensaje[1] = 4%256;
			mensaje[2] = numblock/256;
			mensaje[3] = numblock%256;
			aux=mensaje + 4;
			//Enviamos el paquete ACK
			if((sendto(descriptor_socket,mensaje,aux-mensaje,0,(struct sockaddr *)&direccion_servidor, sizeof(struct sockaddr)))<0)
			{
				perror("sendto()");
				exit(EXIT_FAILURE);
			}
			if(numblock == numero_paquete)
			{
				numblock++;
			}
			size_ = 0 ;		
			size_ = recvfrom(descriptor_socket, mensaje_in , SIZE_MAX_, 0 , (struct sockaddr *)&emisor, (socklen_t *)&tamanho);
		}while(size_ == 512);
		fclose(archivo);
		printf("Se ha recibido el archivo %s con exito.\n",argv[3]);
	}
	else 	// Modo escritura.
	{
		numblock=0;
		archivo = fopen(argv[3],"r");
		if(archivo == NULL)
		{
			printf("El archivo no existe.\n");
			exit(EXIT_FAILURE);
		}
		
		/*numero_paquete = mensaje_in[2]/256;
		numero_paquete = mensaje_in[3]%256;
		printf("Numero de bloque solicitado %i\n",numero_paquete);*/
		
		char bloque_data[SIZE_MAX_];
		while(feof(archivo) == 0)
		{
			if(numero_paquete == numblock ) // Verificamos si estamos enviando el bloque correcto.
			{
				fgets(bloque_data,512,archivo);
				mensaje_out[0] = 3/256;
				mensaje_out[1] = 3%256;
				mensaje_out[2] = numblock/256;
				mensaje_out[3] = numblock%256;
				aux = mensaje_out + 4;
				strcpy(mensaje_out,bloque_data);
				aux += strlen(bloque_data) + 1;
				printf("\n%i\t%i\n",numblock,numero_paquete);
				if((sendto(descriptor_socket,mensaje_out,aux-mensaje_out,0,(struct sockaddr *)&direccion_servidor, sizeof(struct sockaddr)))<0)
				{
					perror("sendto()");
					exit(EXIT_FAILURE);
				}
				numblock++;
			}
				size_ = recvfrom(descriptor_socket, mensaje_in , SIZE_MAX_, 0 , (struct sockaddr *)&emisor, (socklen_t *)&tamanho);
				numero_paquete = mensaje_in[2]/256;
				numero_paquete = mensaje_in[3]%256;

		}
		fclose(archivo);
		printf("Se ha enviado el archivo %s con exito.\n",argv[3]);

	}
	//printf("Mensaje recibido: \t %s\n", mensaje_id);
	close(descriptor_socket);
	return 0;
}
