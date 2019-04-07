# Implementaciones de sistema cliente servidor en C

Este repositorio contiene diferentes implementaciones de arquitectura cliente servidor utilizando tanto socket UDP como socket TPC. Como parte del curso de Arquitectura de Redes y servicios. UVa 2017. 

## Daytime_Cliente_Servidor_UDP

Aquí se implementa un sistema cliente-servidor utilizando socket UDP (user datagram protocol). El servidor espera una petición del cliente y al recibirla, este le envía un mensaje con la hora y el id del usuario de la máquina donde está ejecutandose.

  - daytime-udp-client-Tejaxun-Xicon.c  Es el cliente.
  - daytime-udp-Server-Tejaxun-Xicon.c  Es el servidor.

## Daytime_Cliente_Servidor_TCP

Aquí se implementa un sistema cliente-servidor utilizando socket TCP (Transport control protocol). El servidor espera una petición del cliente y al recibirla, este le envía un mensaje con la hora y el id del usuario de la máquina donde está ejecutandose. 

  - daytime-udp-client-Tejaxun-Xicon.c  Es el cliente.
  - daytime-udp-Server-Tejaxun-Xicon.c  Es el servidor.

## FTP Cliente_Servidor_TCP 

Aquí se implemente un sistema FTP (file transport protocol) cliente-servidor utilizando socket TCP (Transport control protocol).

El servidor está a la espera de recibir peticiones tanto de lectura como de escritura dentro del sistema de archivos de la máquina donde esté siendo ejecutado. 

  - tftp-tejaxun-xicon.c Nombre del cliente FTP


## Ping 
Este programa es una implementación de una herramienta para diagnostico de redes, utilizando el protocolo ICMP y socket de tipo RAW el cuál hace una echo Request a la máquina con la dirección ip indicada. La capa de Red es la que se encarga de realizar el echo Response y se calcula el tiempo de latencia de la conexión. 

  - miping.c  Programa para hacer el diagnostico de red.
  - estructuras.h Estructuras necesarias para la implementación del sistema. Créditos al Dr. Diego Llanos, diego@infor.uva.es, Universidad de Valladolid.


