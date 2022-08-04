/*
 ============================================================================
 Name        : proyectoFinal.c
 Author      : grupo4
 Version     :
 Copyright   : Your copyright notice
 Description : registro World in C, Ansi-style
 ============================================================================
 */

// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT	 8080
#define MAXLINE 1024
#define MAXPLAYERS 50

struct Player {
		char id[3];
		char mac[18];
		int vidas;
	};

// Driver code
int main() {
	int sockfd;
	char buffer[50];
	char buffer5[11] = "CHECK GAME";
	char buffer2[6] = "VIDAS";
	char buffer1[3] = "00";
	char buffer12[2] = "0";
	char buffer3[6] = "START";
	char buffer6[10] = "RESTAR2";

	int vidas = 3;
	int indice;

	char registro[] = "REGISTRADO";
	char cantVidas[] = "3";
	char inicia[] = "START";
	char ok[] = "OK";
	char finJuego[] = "GAME OVER";
	char pierde[] = "LOSER";
	int escucha = 1;
	int iniciarJuego = 0;
	int gameOver = 0;
	struct sockaddr_in servaddr, cliaddr;

	struct Player arrayPlayers[MAXPLAYERS];

	int cantidadJugadores, cantidadJugadoresControl = 0;

	int cantidadJugadores2 = 0;

	int rtaJug;

	system("clear");

	printf("Ingrese la cantidad de Jugadores: ");
	scanf("%d", &rtaJug);

	if (rtaJug>1)
		printf("\nEsperando que se registren los %d Jugadores.\n", rtaJug);
	else
		printf("\nEsperando que se registre el Jugador.\n");


	cantidadJugadoresControl=rtaJug;

	buffer1[3] = '\0';
	buffer2[6] = '\0';
	buffer3[6] = '\0';
	buffer5[11] = '\0';

	//printf("Control : %d\n", cantidadJugadoresControl);

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	//int cont = 0;
	while (escucha==1) {
		int len, n;

		len = sizeof(cliaddr); //len is value/result

		n = recvfrom(sockfd, (char *)buffer, 50,
					MSG_WAITALL, ( struct sockaddr *) &cliaddr,
					&len);

		buffer[n] = '\0';

		char buffer4[7] = "RESTAR";
		buffer4[7] = '\0';

		char id[3];
		char mensaje[50];
		char mac[18];

		memcpy(id, &buffer[0], 2);
		memcpy(mensaje, &buffer[2], 10);

		id[3] = '\0';
		mensaje[n] = '\0';

		char jugador[3];

		//DESCOMENTAR PARA DEBUG
		printf("El jugador %s ha enviado el mensaje %s.\n", id, mensaje);
		//printf("Id del Jugador : %s\n", id);


		//Recibe la MAC
		//Devuelve REGISTRADOXX
		if ((strcmp(buffer1, id)==0) && (cantidadJugadores2 < rtaJug)){
		//if (cont < 1){

			//printf("Entra \n");

			cantidadJugadores2 = cantidadJugadores2 + 1;

			memcpy(mac, &buffer[2], 17);
			mac[n] = '\0';

			//printf("MAC: %s \n", mac);


			char intAchar[2];
			//
			//printf("cantidad: %d \n", cantidadJugadores2);
			sprintf(intAchar, "%d", cantidadJugadores2);
			//printf("INTACHAR: %s \n", intAchar);

			if (cantidadJugadores2 < 10){

				//printf("cantidad: %d \n", cantidadJugadores2);

				strcpy(jugador, buffer12);


				strcat(jugador, intAchar);
				//printf("jugador: %s \n", jugador);

			}else{

				strcpy(jugador, intAchar);

			}

			memset(registro, 0, sizeof registro);

			strcat(registro, "REGISTRADO");

			strcat(registro, jugador);

			jugador[3] = '\0';
			mac[18] = '\0';

			indice = cantidadJugadores2 - 1;

			//printf("jugador: %s \n", jugador);
			strlcpy(arrayPlayers[indice].id, jugador, 3);
			arrayPlayers[indice].vidas = 0;
			//arrayPlayers[cantidadJugadores2].id = jugador;
			strlcpy(arrayPlayers[indice].mac, mac, 18);

			printf("\nJugador\t\tVidas\t\t\Identificador\n");

			for (int i=0; i<cantidadJugadores2; i++){

				printf("  %s\t\t  %d\t\t%s\n", arrayPlayers[i].id, arrayPlayers[i].vidas, arrayPlayers[i].mac);

			}

			printf("\n");

			printf("Se ha registrado el Jugador %s.\n\n", jugador);


			sendto(sockfd, (const char *)registro, strlen(registro),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);




		}

		//Recibe VIDAS
		//Devuelve int en variable vidas
		else if (strcmp(buffer2, mensaje)==0){

			cantidadJugadores++;

			sendto(sockfd, (const char *)cantVidas, strlen(cantVidas),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);
			printf("\nSe asignaron %s Vidas al Jugador %s.\n", cantVidas, id);

			//struct Player player;
			for (int i=0; i<cantidadJugadores; i++){
				if (strcmp(arrayPlayers[i].id, id)==0) {
					arrayPlayers[i].vidas = vidas;
				}
			}

			printf("\nJugador\t\tVidas\t\t\Identificador\n");

			for (int i=0; i<cantidadJugadores; i++){

				printf("  %s\t\t  %d\t\t%s\n", arrayPlayers[i].id, arrayPlayers[i].vidas, arrayPlayers[i].mac);

			}

			printf("\n");


			if (cantidadJugadores == rtaJug){
				iniciarJuego=1;
			}

		}
		else if (strcmp(buffer3, mensaje)==0){
			if (iniciarJuego == 1) {
				sendto(sockfd, (const char *)inicia, strlen(inicia),
						MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
						len);
				printf("\nSe inicia la Partida para el Jugador %s!\n\n", id);
			}

		}

		//Recibe RESTAR
		//Devuelve OK, LOSER o GAMEOVER
		else if (strcmp(buffer4, mensaje)==0){
			if (gameOver == 0) {
				//vidas--;
				for (int i=0; i<cantidadJugadores; i++){
					if (strcmp(arrayPlayers[i].id, id)==0) {
						arrayPlayers[i].vidas--;
						if (arrayPlayers[i].vidas==0)
						{
							gameOver = 1;

							sendto(sockfd, (const char *)pierde, strlen(pierde),
											MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
												len);
							printf("\nSe le ha quitado una vida al Jugador %s.\n", id);
							printf("El Jugador %s se ha quedado sin vidas (se procede a avisar al resto de los jugadores).\n\n", id);

							cantidadJugadoresControl--;
							//printf("Control : %d\n", cantidadJugadoresControl);

						}
						else {

							sendto(sockfd, (const char *)ok, strlen(ok),
												MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
													len);
							printf("\nSe le ha quitado una vida al Jugador %s.\n", id);

							printf("\nMarcador Actual:\n");
							printf("\nJugador\t\tVidas\n");
							for (int i=0; i<cantidadJugadores; i++){

								printf("  %s\t\t  %d\n", arrayPlayers[i].id, arrayPlayers[i].vidas);

							}
							printf("\n");
						}
					}

				}

				//printf("Vidas restantes: %d\n", vidas);
			}
			else {
				sendto(sockfd, (const char *)finJuego, strlen(finJuego),
								MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
									len);
				//printf("Game over.\n");
				cantidadJugadoresControl--;
				//printf("Control : %d\n", cantidadJugadoresControl);

			}


		}
		//Recibe CHECK GAME
		//Devuelve OK o GAMEOVER
		else if (strcmp(buffer5, mensaje)==0){
			if (gameOver == 0) {
				//printf("\nEntro aqui\n");
				sendto(sockfd, (const char *)ok, strlen(ok),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);
				//printf(".");
			}
			else {
				sendto(sockfd, (const char *)finJuego, strlen(finJuego),
				MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
					len);
				//printf("Game over.\n");
				cantidadJugadoresControl--;

			}
		}
		else{
			//printf("\nBuffer 4: ..%s..\n", buffer4);
			//printf("\nBuffer 6: ..%s..\n", buffer6);
			//printf("\nMensaje: ..%s..\n", mensaje);
			printf("\nNo Entro en ninguna comparacion\n");
		}
		//Chequeo que todos los jugadores se hayan desregistrado
		if (cantidadJugadoresControl==0)
		{
			//Funcion para ordenar el arreglo de struct
			int compare(const void *s1, const void *s2)
			{
			  struct Player *p1 = (struct Player *)s1;
			  struct Player *p2 = (struct Player *)s2;
			  return p2->vidas - p1->vidas;
			}

			qsort(arrayPlayers, cantidadJugadores, sizeof(struct Player), compare);

			printf("\nEl juego ha Finalizado!\n");
			printf("\nLos resultados finales son:\n");
			printf("\nPuesto\t\t\Jugador\t\tVidas\n");

			for (int i=0; i<cantidadJugadores; i++){

				printf("  %d\t\t  %s\t\t  %d\n", i+1, arrayPlayers[i].id, arrayPlayers[i].vidas);

			}
			printf("\n");
			escucha=0;
			return 0;

		}

	}

	return 0;
}
