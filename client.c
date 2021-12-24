/* =================================================================== */
// Progrmame Client à destination d'un joueur qui doit deviner la case
// du trésor. Après chaque coup le résultat retourné par le serveur est
// affiché. Le coup consite en une abcsisse et une ordonnée (x, y).
/* =================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include "tresor.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define N 10
#define YEL 1
#define RED 2
#define MAG 3
#define GRN 4
#define CYA 5
#define RED2 6
#define PORT 4444
#define ADDRESS "127.0.0.1"
/* ====================================================================== */
/*         Affichage du jeu en mode texte (bibliothèque ncurses)          */
/* ====================================================================== */
void afficher_jeu(int jeu[N][N], int res, int points, int coups)
{

    clear();
    move(0, 0);
    attron(A_BOLD);
    printw("************ TROUVEZ LE TRESOR ! ************\n");
    attroff(A_BOLD);
    printw("    ");
    for (int i = 0; i < 10; i++)
        printw("  %d ", i + 1);
    printw("\n    -----------------------------------------\n");
    for (int i = 0; i < 10; i++)
    {
        printw("%2d  ", i + 1);
        for (int j = 0; j < 10; j++)
        {
            printw("|");
            switch (jeu[i][j])
            {
            case -1:
                printw(" 0 ");
                break;
            case 0:
                attron(COLOR_PAIR(GRN));
                printw(" T ");
                attroff(COLOR_PAIR(GRN));
                break;
            case 1:
            case 2:
            case 3:
                attron(COLOR_PAIR(jeu[i][j]));
                printw(" X ");
                attroff(COLOR_PAIR(jeu[i][j]));
                break;
            }
        }
        printw("|\n");
    }
    printw("    -----------------------------------------\n");
    attron(A_BOLD);
    printw("Pts dernier coup %d | Pts total %d | Nb coups %d\n", res, points, coups);
    attroff(A_BOLD);
    refresh();
}
int send_data(int socketd, char *data)
{

    int res=0;
    char buffer[1024];
    write(socketd, data, sizeof(data));
    bzero(buffer,sizeof(buffer ));
    read(socketd, buffer, sizeof(buffer));
    sscanf(buffer,"%d", &res);
    printw("Received from server is %d",res);

    return res;
}
/* ====================================================================== */
/*                    Fonction principale                                 */
/* ====================================================================== */
int main(int argc, char **argv)
{

    int jeu[N][N];
    //int x_tresor = 4, y_tresor = 5;
    int lig, col;
    int res = -1, points = 0, coups = 0;

    /* Init args */
    
  

    /* Init jeu */
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            jeu[i][j] = -1;

    /* Init ncurses */
    WINDOW *mainwin;
    if ((mainwin = initscr()) == NULL)
    {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }
    if (has_colors() == FALSE)
    {
        endwin();
        fprintf(stderr, "Your terminal does not support color.\n");
        exit(EXIT_FAILURE);
    }
    start_color();
    init_pair(GRN, COLOR_BLACK, COLOR_GREEN);
    init_pair(YEL, COLOR_BLACK, COLOR_YELLOW);
    init_pair(RED, COLOR_BLACK, COLOR_RED);
    init_pair(RED2, COLOR_RED, COLOR_BLACK);
    init_pair(MAG, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(CYA, COLOR_BLACK, COLOR_CYAN);

    //initialisation de la socket
    int socketConnect;
    struct sockaddr_in servaddr;
    // socket create and vérification
    socketConnect = socket(AF_INET, SOCK_STREAM, 0);
    if (socketConnect == -1)
    {
        printw("Erreur de connexion");
        exit(1);
    }

    char data[50];
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ADDRESS);
    servaddr.sin_port = htons(PORT);
    if (connect(socketConnect, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printw("Echec connexion with server...\n");
        refresh();
        attrset(A_NORMAL);
        delwin(mainwin);
        endwin();
        exit(0);
    }
    /* Tentatives du joueur : stoppe quand trésor trouvé */
    do
    {
        afficher_jeu(jeu, res, points, coups);
        printw("\nEntrer le numéro de ligne : ");
        scanw("%d", &lig);
        printw("Entrer le numéro de colonne : ");
        scanw("%d", &col);
        refresh();
        sprintf(data, "%d %d", lig, col);
        res = send_data(socketConnect, data);
     
        /* Calcul résultat de la tentative du joueur */
        //res = recherche_tresor(N, x_tresor, y_tresor, lig, col);

        /* Mise à jour */
        if (lig >= 1 && lig <= N && col >= 1 && col <= N)
            jeu[lig - 1][col - 1] = res;
        points += res;
        coups++;

    } while (res);

    /* Terminaison du jeu : le joueur a trouvé le tresor */
    afficher_jeu(jeu, res, points, coups);
    attron(A_BOLD);
    attron(COLOR_PAIR(RED2));
    printw("\nBRAVO : trésor trouvé en %d essai(s) avec %d point(s)"
           " au total !\n",
           coups, points);
    getch();
    attrset(A_NORMAL);
    delwin(mainwin);
    endwin();
    close(socketConnect);
    return 0;
}