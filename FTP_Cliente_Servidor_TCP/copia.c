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
 

#define SIZE_MAX_ 516      //Tamanho maximo de cada paquete.
#define modo_t "octet"    //Modo de transmicion.
#define ACK 4
#define DATA 3
#define WRQ 2
#define RRQ 1

int descriptor_socket=0; //Descriptor del socket.
int enlace=0;		 //Descriptor del enlace con el socket y este host.
int modo=1;		 //Para identificar el modo de manejo de archivo. 1=Lectura, 2=Escritura.
int detalle=0;		 // Para determinar si se muestra el detalle de la operacion, o no. 0=No, 1=si.

char mensaje_in[SIZE_MAX_]; //Buffer de entrada;
char mensaje_out[SIZE_MAX_];//Bufer de salida.
char salida[SIZE_MAX_];	//Buffer de datos a escribir.
char * aux;
int numero_paquete=1;
int numblock=1;

FILE *archivo;
unsigned int paquete_ultimo=1;
unsigned int paquete_esperado=1;
unsigned int  paquete_recibido=1;
int size_=0;		// Tamanho del paquete recibido.
int tamanho=0;		// Tamanho de la estructura sockaddr;
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
	//Primero limpiamos el array del mensaje.
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

	//Imprimos mensaje si se ha activado la opcion de detalle.
	if( modo==1 && detalle == 1)
	{
		printf("Enviada solicitud de lectura de  %s a servidor tftp en %s .\n",argv[3],argv[1]);	
	}else if (modo ==2 && detalle ==1 )
	{
		printf("Enviada solicitud de escritura de  %s a servidor tftp en %s .\n",argv[3],argv[1]);	
	
	
	
	}

	if(modo == 1) // Modo de lectura;
	{	

		archivo = fopen(argv[3],"w");
		fclose(archivo);
		//Ahora lo abrimos en modo "a" , para ir escribiendo al final del archivo cada paquete entrante.
		archivo = fopen(argv[3],"w");
		struct sockaddr_in emisor;
		do
		{

			//FILE *log;
			//log = fopen("log","a");
			//fwrite("O",1,1,log);
			//fwrite(mensaje_out,1,4,log);
			//Ahora esperamos que nos venga el primer mensaje;
			//struct sockaddr_in emisor;	
			tamanho = sizeof(struct sockaddr);
			size_ = 0;
			int codigo_op;
			//Recibimos el paquete.
			size_ = recvfrom(descriptor_socket, mensaje_in, SIZE_MAX_, 0 , (struct sockaddr *)&direccion_servidor, (socklen_t *)&tamanho);
			
			//Verificamos si no hubo error en la recepcion del mensaje.
			if(size_ < 0)
			{
				perror("recvfrom()");
				exit(EXIT_FAILURE);
			}
				
			if(detalle == 1)
			{
				printf("Recibido bloque del servidor tftp. Tamano:%i\n",size_);
			}

			//fwrite(mensaje_in,1,4,log);
			// Si no hay error en el recvfrom, obtenemos el codigo de operacion del paquete para realizar alguna accion.
			//Si no hay error, significa que recibimos un paquete DATA, creamos el archivo.
			codigo_op = mensaje_in[0];
			codigo_op = mensaje_in[1];
			
			//printf("Bloque No. %i recibido.\n",numblock);
			if(codigo_op == 5) // Esto significa que hemos recibido un paquete de error.
			{
				for(contador=0; contador<(size_-4);contador++){salida[contador]=mensaje_in[contador+4];}
				printf("Paquete de error recibido:t %s\n",salida);
				exit(EXIT_FAILURE);
			}
			
			//Este buffer auxiliar sera para guardar solo la parte de datos recibidas en el paquete.
			char auxiliar[SIZE_MAX_];
			for(contador=0;contador<(size_-4);contador++){auxiliar[contador]=mensaje_in[contador+4];}
			mensaje_in[size_]='\0';
	        	
			//printf("---------------%i----%i----------\n",(unsigned char)mensaje_in[2],(unsigned char)mensaje_in[3]);
			paquete_recibido= 0;
			paquete_recibido = (unsigned int) mensaje_in[2] << 8;	 
			//printf("------------%i-------------\n",paquete_recibido);
			paquete_recibido += (unsigned int) mensaje_in[3]%256;
			//printf("------------%i-------------\n",paquete_recibido);
			/*if(paquete_recibido > 128)
			{
				paquete_recibido = paquete_recibido + 256;
				printf("Ayyy, muchas cosas. Wuuuu.\n");
			}*/
			if(detalle==1)
			{
				printf("Paquete recibido, de tamano:%i\n",size_);
			//	printf("Encabezado paquete recibido:| %d | %d |\n",ACK,paquete_ultimo);
			//	printf("Encabezado paquere enviado :| %d | %d |\n",codigo_op,paquete_recibido);
			}
			if(detalle==1)
			{
				printf("Bloque No. %i recibido. \tPaquete esperado:\t%i\n",paquete_recibido,paquete_esperado);
			}
			
			fwrite(auxiliar,1,(size_-4),archivo);
			// Ahora enviamos el paquete ACK
			
			if(paquete_recibido == paquete_esperado)
			{
				paquete_esperado++;
				paquete_ultimo = paquete_recibido;
			}

			mensaje_out[0] = 0;
			mensaje_out[1] = 4;
			mensaje_out[2] = paquete_ultimo/256;
			mensaje_out[3] = paquete_ultimo%256;
			//Enviamos el paquete ACK
			if((sendto(descriptor_socket,mensaje_out,4,0,(struct sockaddr *)&direccion_servidor, sizeof(struct sockaddr)))<0)
			{
				perror("sendto()");
				exit(EXIT_FAILURE);
			}		
			
			if(detalle == 1)
			{
				printf("Mensaje ACK enviado.\n ");
			}
			//fclose(log);
			direccion_servidor= emisor;
		}while(size_ == 516);
		fclose(archivo);
		printf("Transmision de datos finalizada.\n");
		printf("Se ha recibido el archivo %s con exito.\n",argv[3]);
	}
	else 	// Modo escritura.
	{
		



		paquete_ultimo=1;	//Ultimo paquete enviado.
		paquete_esperado=1;	//Se espera que el paquete ACK indique este numero.
		paquete_recibido=1;	//El numero de bloque recibido.
		archivo = fopen(argv[3],"r");
		if(archivo == NULL)
		{
			printf("El archivo no existe.\n");
			exit(EXIT_FAILURE);
		}

		// Ahora calculamos el tama�o del archivo.
		fseek(archivo,0L, SEEK_END);
		int tamanho_archivo = ftell(archivo);
		int numero_paquetes = tamanho_archivo / 512 ; 	//Numero de paquetes a enviar.
		numero_paquetes ++ ; 			// Para el paquete final.
		
		char bloque_data[512];
		int codigo_op;
		while(feof(archivo) == 0 && contador<=numero_paquetes)
		{
			tamanho = sizeof(struct sockaddr);
			size_ = 0;

			//Recibimos el mensaje.
			size_ = recvfrom(descriptor_socket, mensaje_in, SIZE_MAX_, 0 , (struct sockaddr *)&direccion_servidor, (socklen_t *)&tamanho);


			//Verificamos si no hubo error en la recepcion del mensaje.
			if(size_ < 0)
			{
				perror("recvfrom()");
				exit(EXIT_FAILURE);
			}

			if(detalle == 1)
			{
				printf("Recibido bloque del servidor tftp. Tamano:%i\n",size_);
			}
	
			// Si no hay error en el recvfrom, obtenemos el codigo de operacion del paquete para realizar alguna accion.
			//Si no hay error, significa que recibimos un paquete DATA, creamos el archivo.
		
			codigo_op = mensaje_in[0];
			codigo_op = mensaje_in[1];
			//printf("Bloque No. %i recibido.\n",numblock);
			if(codigo_op == 5) // Esto significa que hemos recibido un paquete de error.
			{
				for(contador=0; contador<(size_-4);contador++){salida[contador]=mensaje_in[contador+4];}
				printf("Paquete de error recibido:t %s\n",salida);
				exit(EXIT_FAILURE);
			}
			if(codigo_op != 2) 		//Si no recibimos un paquete de tipo ACK, abortamos la transmision.
			{
				printf("Error en transmision de paquetes.");
				exit(EXIT_FAILURE);
			}
			
			paquete_recibido = (unsigned int) mensaje_in[2] << 8;
			paquete_recibido += (unsigned int) mensaje_in[3]%256;
			if(paquete_recibido == paquete_esperado ) // Verificamos si estamos enviando el bloque correcto.
			{
				paquete_esperado++;	
				paquete_ultimo++;
			}	
			
			if(contador < numero_paquetes)
			{
				fgets(bloque_data,512,archivo); 	// Paquetes de 512
			}
			else
			{
				fgets(bloque_data,tamanho_archivo -(contador*512),archivo); // Ultimo paquete 
			}
			mensaje_out[0] = DATA;		// Tipo de paquete.
			mensaje_out[1] = DATA;		// Tipo de paquete.
			mensaje_out[2] = paquete_ultimo/256;	// Numero de bloque.
			mensaje_out[3] = paquete_ultimo%256;	// Numero de bloque.
			aux = mensaje_out + 4;
			strcpy(mensaje_out,bloque_data);
			aux += strlen(bloque_data) + 1;
			printf("Enviando paquete DATA No.%i\n",paquete_ultimo);
			if((sendto(descriptor_socket,mensaje_out,aux-mensaje_out,0,(struct sockaddr *)&direccion_servidor,sizeof(struct sockaddr)))<0)
			{
				perror("sendto()");
				exit(EXIT_FAILURE);
			}
		}
		fclose(archivo);
		printf("Se ha enviado el archivo %s con exito.\n",argv[3]);

	}
	//printf("Mensaje recibido: \t %s\n", mensaje_id);
	close(descriptor_socket);
	return 0;
}
