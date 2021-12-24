/* =================================================================== */
// Progrmame Serveur qui calcule le résultat d'un coup joué à partir
// des coordonnées reçues de la part d'un client "joueur".
// Version ITERATIVE : 1 seul client/joueur à la fois
/* =================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "tresor.h"
#define N 10
#define ADDRESS "127.0.0.1"
#define PORT 4444

int main(int argc, char **argv)
{

      
	int sockfd,newSocket;
	struct sockaddr_in serverAddr,newAddr;
	

	socklen_t taille;
	char buffer[1024];
	int x_tresor = 4, y_tresor = 5;
	if (argc == 2 && strcmp(argv[1], "rand") == 0)
	{
		srand(time(NULL));
		x_tresor = 1 + rand() % N;
		y_tresor = 1 + rand() % N;
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("[+]Ouverture de la socket.\n");
	memset(&serverAddr, '\0', sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(ADDRESS);

	bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	printf("[+]Serveur en écoute sur le port %d.\n",PORT);

	listen(sockfd, 5);
	int lig, col, res;
	printf("[+]Attente de connexion...\n");
	while(1)
	{

	
	newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &taille);
	
	if(newSocket< 0)
	{
		exit(2);
	}
	
	while (1)
	{

		printf("Attente des données du client \n");
		bzero(buffer, sizeof(buffer));
		read(newSocket, buffer, sizeof(buffer));
		sscanf(buffer, "%d %d", &lig, &col);
		printf("Ligne [%d] et colonne [%d] reçu du client \n",lig,col);
		bzero(buffer, sizeof(buffer));
		res = recherche_tresor(N, x_tresor, y_tresor, lig, col);
		sprintf(buffer, "%d", res);
		write(newSocket, buffer, sizeof(buffer));
	}
	}
	close(newSocket);

	return 0;
}
