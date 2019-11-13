#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/default.h"
#include "lib/server.h"
#include <time.h>
#define TABWIDTH 15
#define TABHEIGHT 11
#define MAXCLIENTS 5
#define MAXBOMBS 4

typedef struct{
	char dirAtual;
	char username[255];
	char identificador;
	int life;
	int kills;
	int deaths;
	int x;
	int y;
	int bombPower;
	int bombsDropped;
	int maxBombs;
	int invincible;
}player;
 
typedef struct{
	int isValid;
	char move;
}pacote_server;

typedef struct{
	char tab[TABHEIGHT][TABWIDTH];
	int bombFlag;
	int xBomb;
	int yBomb;
	int bombRange;
	int lifes;
	int kills;
	int isInvicible;
	int deaths;
	char direction[4];
}pacote_client;

typedef struct{
	int active;
	int whoDroped;
	int x;
	int y;
	clock_t deployTime;
}bomb;


void initTab(char tab[][TABWIDTH])
{
	int i, j;


	for(i = 0; i < TABHEIGHT; i++){
		for(j = 0; j < TABWIDTH; j++){

            if(i==0 || i==TABHEIGHT-1) tab[i][j] = '|';
            else if(j==0 || j==TABWIDTH-1)tab[i][j] = '|';
            else if(j%2== 0 && i%2==0)tab[i][j] = '|';
            else tab[i][j] = ' ';
		}
    }
     srand ( time(NULL) );
    //printf ("\nUm numero entre 0 e RAND_MAX (%d): %d\n", RAND_MAX, rand());
    int blocoQuebravel;
    for(blocoQuebravel = 0; blocoQuebravel<75;blocoQuebravel++){
        do{
        //sorteia de 1 a 13
        i=rand()%9 +1;
        //soteia de 1 a 5
        j=rand()%13 +1;

        }while(tab[i][j] == '|' || tab[i][j] == '}');
        tab[i][j] = '}';

    }
    //espaços do player 0
    tab[1][1] = ' ';
    tab[1][2] = ' ';
    tab[2][1] = ' ';
    //espaços do player 1
    tab[1][TABWIDTH-2] = ' ';
    tab[1][TABWIDTH-3] = ' ';
    tab[2][TABWIDTH-2] = ' ';
    //espaços do player 2
    tab[TABHEIGHT-3][1] = ' ';
    tab[TABHEIGHT-2][2] = ' ';
    tab[TABHEIGHT-2][1] = ' ';
    //espaços do player 3
    tab[TABHEIGHT-2][TABWIDTH-2] = ' ';
    tab[TABHEIGHT-2][TABWIDTH-3] = ' ';
    tab[TABHEIGHT-3][TABWIDTH-2] = ' ';
}

void printTab(char tab[][TABWIDTH])
{
	int i, j;
	for(i = 0; i < TABHEIGHT; i++){
		for(j = 0; j < TABWIDTH; j++)
			printf("%c" ,tab[i][j]);

		printf("\n");
	}
}

void copyMatrix(char tab[][TABWIDTH], pacote_client *toClient, player pList[], int id, int playersConnected)
{
	int i, j;
	for(i = 0; i < TABHEIGHT; i++){
		for(j = 0; j < TABWIDTH; j++)
			toClient->tab[i][j] = tab[i][j];
	}

	toClient->lifes = pList[id].life;
	for(i = 0; i < playersConnected; i++)
		toClient->direction[i] = pList[i].dirAtual;


		
		toClient->isInvicible = pList[i].invincible;
		toClient->kills = pList[id].kills;
		toClient->deaths = pList[id].deaths;
	
	//printf("toClient.isInvicible id: %d = %d\n" ,id ,toClient->isInvicible);
}

void setBeginPosition(int id, player pList[])
{
	switch(id){
	case 0: 
	pList[id].x = 1;
	pList[id].y = 1;
	break;
	case 1: 
	pList[id].x = 1;
	pList[id].y = 13;
	break;
	case 2: 
	pList[id].x = 9;
	pList[id].y = 13;
	break;
	case 3: 
	pList[id].x = 9;
	pList[id].y = 1;
	break;
	}
}

void initPlayer(int id, player pList[], char tab[][TABWIDTH])
{
	pList[id].life = 3;
	pList[id].identificador = 'v' + id;
	pList[id].bombPower = 1;
	pList[id].deaths = 0;
	pList[id].kills = 0;
	pList[id].maxBombs = 1;
	pList[id].bombsDropped = 0;
	pList[id].invincible = 0;
	setBeginPosition(id, pList);
	tab[pList[id].x][pList[id].y] = pList[id].identificador;
}

int validMove(int id, char move, player pList[], char tab[TABHEIGHT][TABWIDTH])
{
	int x,y;
	x = pList[id].x;
	y = pList[id].y;
	

	switch(move)
	{	
		case 'w': 
		x--;
		break;
		case 'a':
		y--;
		break;
		case 's':
		x++;
		break;
		case 'd':
		y++;
		break;
	}

	if(tab[x][y] == ' ' || tab[x][y] == '1' || tab[x][y] == '2' || tab[x][y] == '3' || tab[x][y] == '4')
		return 1;
	else
		return 0;
}

void atualizarPos(int id, char move, player pList[], char tab[TABHEIGHT][TABWIDTH], clock_t invecebility[])
{
	if(!validMove(id, move, pList, tab))
		return;

	tab[pList[id].x][pList[id].y] = ' ';
	switch(move)
	{	
		case 'w': 
		pList[id].x--;
		break;
		case 'a':
		pList[id].y--;
		break;
		case 's':
		pList[id].x++;
		break;
		case 'd':
		pList[id].y++;
		break;
	}

	if(tab[pList[id].x][pList[id].y] == '1')
		pList[id].bombPower++;

	if(tab[pList[id].x][pList[id].y] == '2')
		pList[id].life++;

	if(tab[pList[id].x][pList[id].y] == '3')
		pList[id].maxBombs++;
	
	if(tab[pList[id].x][pList[id].y] == '4')
	{
		pList[id].invincible = 1;
		invecebility[id] = clock();
	}

	tab[pList[id].x][pList[id].y] = pList[id].identificador; 
}

void dropBomb(player pList[], int id, char tab[][TABWIDTH], bomb playerBombs[][MAXBOMBS])
{
	int x,y;
	x = pList[id].x;
	y = pList[id].y;
	
	switch(pList[id].dirAtual)
	{	
		case 'w': 
		x--;
		break;
		case 'a':
		y--;
		break;
		case 's':
		x++;
		break;
		case 'd':
		y++;
		break;
	}

	tab[x][y] = 'X';
	playerBombs[id][pList[id].bombsDropped].x = x;
	playerBombs[id][pList[id].bombsDropped].y = y;
}

int validSpot(int x, int y)
{
	if(!(x >= 0 && x < TABHEIGHT))
		return 0;
	if(!(y>= 0 && y < TABWIDTH))
		return 0;

	return 1;
}

char breakBlock(){
	int a;
	a = rand()%13;
	if(a == 6)
		return '1';
	else if(a == 7)
		return '2';
	else if(a == 8)
		return '3';
	else if(a == 9)
		return '4';

	return ' ';
}

int isPlayer(int x, int y, player pList[], int playersConnected, char tab[][TABWIDTH])
{
	int i;
	for(i = 0; i < playersConnected; i++)
		if(tab[x][y] == pList[i].identificador && pList[i].invincible == 0)
			return i;

	return -1;
}

void killPlayer(int id, player pList[], int x, int y, char tab[][TABWIDTH], int bombDroper)
{
	pList[id].life --;
	tab[x][y] = ' ';
	pList[id].deaths++;
	pList[bombDroper].kills++;
	pList[id].bombPower = 1;
	pList[id].maxBombs = 1;
	if(pList[id].life != 0){
		setBeginPosition(id, pList);
		tab[pList[id].x][pList[id].y] = pList[id].identificador;
	}
	else
		disconnectClient(id);
}

void vaiExplodir(char tab[][TABWIDTH], int x, int y, player pList[], int id, int playersConnected)
{
	int i;
	int flag[4] = {0};
	int k = pList[id].bombPower;
	srand(time(NULL));
	for(i = -k; i <= k; i++){


		if(validSpot(x-i, y) && tab[x-i][y] == '|')
			flag[0] = 1;
		
		if(validSpot(x+i, y) && tab[x+i][y] == '|')
			flag[1] = 1;

		if(validSpot(x, y+i) && tab[x][y+i] == '|')
			flag[2] = 1;

		if(validSpot(x, y-i) && tab[x][y-i] == '|')
			flag[3] = 1;


		if(validSpot(x-i, y) && flag[0] != 1 && tab[x-i][y] == '}')
			tab[x-i][y] = breakBlock();
		else if(validSpot(x-i, y) && isPlayer(x-i, y, pList, playersConnected, tab) != -1)
			killPlayer(isPlayer(x-i, y, pList, playersConnected, tab), pList, x-i, y, tab, id);
				

		if(validSpot(x+i, y) && flag[1] != 1 && tab[x+i][y] == '}')
			tab[x+i][y] = breakBlock();
		else if(validSpot(x+i, y) && isPlayer(x+i, y, pList, playersConnected, tab) != -1)
			killPlayer(isPlayer(x+i, y, pList, playersConnected, tab), pList, x+i, y, tab, id);
		

		if(validSpot(x, y+i) && flag[2] != 1 && tab[x][y+i] == '}')
			tab[x][y+i] = breakBlock();		
		else if(validSpot(x, y+i) && isPlayer(x, y+i, pList, playersConnected, tab) != -1)
			killPlayer(isPlayer(x, y+i, pList, playersConnected, tab), pList, x, y+i, tab, id);
	
		
		if(validSpot(x, y-i) && flag[3] != 1 && tab[x][y-i] == '}')
			tab[x][y-i] = breakBlock();
		else if(validSpot(x, y-i) && isPlayer(x, y-i, pList, playersConnected, tab) != -1)
			killPlayer(isPlayer(x, y-i, pList, playersConnected, tab), pList, x, y-i, tab, id);
		
	
		//printf("Flags: %d %d %d %d\n" , flag[0], flag[1], flag[2], flag[3]);
	}

	tab[x][y] = ' ';
}


int main()
{
	int i, j;
	char tab[TABHEIGHT][TABWIDTH];
	initTab(tab);
	serverInit(MAXCLIENTS);
	int sair = 1;
	player pList[MAXCLIENTS];
	int playersConnected = 0;
	struct msg_ret_t retorno;
	pacote_server fromClient;
	pacote_client toClient;
	clock_t tAtual;
	clock_t invecebility[MAXCLIENTS];
	bomb playerBombs[MAXCLIENTS][MAXBOMBS];
	for(i = 0; i < MAXCLIENTS; i++)
		for(j = 0; j < MAXBOMBS; j++){
			playerBombs[i][j].active = 0;
			playerBombs[i][j].deployTime = clock();
		}
	
	/*for(i = 0; i < 4; i++){
		toClient.isInvicible[i] = 0;
	}*/

	double TEMPO_BOMBA = 0.06;
	double TEMPO_INVENCIVEL = 0.30;

	while(sair)
	{	
		int id = acceptConnection();
		player p;
		if(id != NO_CONNECTION)
		{
			printf("Novo jogador conectado, id: %d\n" ,id);
			pList[id] = p;
			playersConnected++;
			initPlayer(id, pList, tab);
			copyMatrix(tab, &toClient, pList, id, playersConnected);
		}


		//printTab(toClient.tab);
		if(id != NO_CONNECTION){
			sendMsgToClient(&toClient, sizeof(toClient), id);
		}
		
		retorno = recvMsg(&fromClient);

		if(retorno.status == MESSAGE_OK)
		{
			id = retorno.client_id;
			if(fromClient.isValid == 1)
			{
				char move = fromClient.move;
				if(move != pList[id].dirAtual && move != ' ')
					pList[id].dirAtual = move;
				else if(move != ' '){
					atualizarPos(id, move, pList, tab, invecebility);
				}
				else if(pList[id].bombsDropped < MAXBOMBS && move == ' ' && validMove(id, pList[id].dirAtual, pList, tab) && pList[id].bombsDropped < pList[id].maxBombs){
					printf("Player %d has dropped a bomb\n" ,id);
					dropBomb(pList, id, tab, playerBombs);
					playerBombs[id][pList[id].bombsDropped].whoDroped = id;
					playerBombs[id][pList[id].bombsDropped].deployTime = clock();
					playerBombs[id][pList[id].bombsDropped].active = 1;
					pList[id].bombsDropped++;
				}
				else if(pList[id].bombsDropped >= pList[id].maxBombs)
				{
					printf("trying to dropo bomb, id : %dbombs dropped: %d\n" ,id ,pList[id].bombsDropped);
					printf("Unable to drop bomb because of space\n");
				}
			}
		}

		tAtual = clock();
		for(i = 0; i < playersConnected; i++)
			for(j = 0; j < MAXBOMBS; j++){
				if(playerBombs[i][j].active == 0)
					continue;

				//printf("%d %d %.2lf\n" ,pList[i].bombsDropped, j, (double)(tAtual-playerBombs[i][j].deployTime)/CLOCKS_PER_SEC);
				if((double)(tAtual-playerBombs[i][j].deployTime)/CLOCKS_PER_SEC >= TEMPO_BOMBA){
					printf("VAI EXPLODIIRRR, id: %d\n" ,playerBombs[i][j].whoDroped);
					toClient.bombFlag = 1;
					toClient.xBomb = playerBombs[i][j].x;
					toClient.yBomb = playerBombs[i][j].y;
					toClient.bombRange = pList[id].bombPower;
					vaiExplodir(tab, playerBombs[i][j].x, playerBombs[i][j].y, pList, playerBombs[i][j].whoDroped, playersConnected);
					playerBombs[i][j].active = 0;
					pList[i].bombsDropped--;
				}
			}

		tAtual = clock();
		if(id != NO_CONNECTION && pList[id].invincible == 1){
			//printf("tempo: %.2lf\n" ,(double)(tAtual-invecebility[id])/CLOCKS_PER_SEC);
			if((double)(tAtual-invecebility[id])/CLOCKS_PER_SEC >= TEMPO_INVENCIVEL){
				printf("Player %d is no longer invincible!\n" ,id);
				pList[id].invincible = 0;
			}
		}


		if(toClient.bombFlag == 1)
		{
			for(i = 0; i < playersConnected; i++){
				copyMatrix(tab, &toClient, pList, i, playersConnected);
				sendMsgToClient(&toClient, sizeof(toClient), i);
			}
		}
		else if(id != NO_CONNECTION){
			copyMatrix(tab, &toClient, pList, id, playersConnected);
			sendMsgToClient(&toClient, sizeof(toClient), id);
		}

		toClient.bombFlag = 0;
	}

	return 0;
}

