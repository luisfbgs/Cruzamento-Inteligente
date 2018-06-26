/*
* Autor : Luís Felipe Braga Gebrim Silva
* Matrícula : 16/0071569
* Algoritmo: Semáforo inteligente simulado com programação concorrente
*/

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define OO 10000000000000
#define LIM 6
#define N 200

int width, height;
								/*Pista 11~21*/
char cruz[35][35] = {"          |     |     |          ", /* 0*/
					 "          |     |     |          ", /* 1*/
					 "          |     |     |          ", /* 2*/
					 "          |     |     |          ", /* 3*/
					 "          |     |     |          ", /* 4*/
					 "          |     |     |          ", /* 5*/
					 "          |     |     |          ", /* 6*/
					 "          |     |     |          ", /* 7*/
					 "          |     |     |          ", /* 8*/
					 "          |     |     |          ", /* 9*/
					 "-----------     |     -----------", /* 10*/
					 "                                 ", /* 11*/
					 "                                 ", /* 12*/
					 "                                 ", /* 13*/
					 "                                 ", /* 14*/
					 "                                 ", /* 15*/
					 "-----------           -----------", /* 16*/
					 "                                 ", /* 17*/
					 "                                 ", /* 18*/
					 "                                 ", /* 19*/
					 "                                 ", /* 20*/
					 "                                 ", /* 21*/
					 "-----------     |     -----------", /* 22*/
					 "          |     |     |          ", /* 23*/
					 "          |     |     |          ", /* 24*/
					 "          |     |     |          ", /* 25*/
					 "          |     |     |          ", /* 26*/
					 "          |     |     |          ", /* 27*/
					 "          |     |     |          ", /* 28*/
					 "          |     |     |          ", /* 29*/
					 "          |     |     |          ", /* 30*/
					 "          |     |     |          ", /* 31*/
					 "          |     |     |          "  /* 32*/
														};
								/* Pista 11~21*/
int PROB_UP;
int PROB_DOWN;
int PROB_LEF;
int PROB_RIG;

pthread_t lef[N], rig[N], up[N], down[N];
pthread_t semCo;
pthread_mutex_t pista[35][35];
pthread_mutex_t qtdHorL, qtdVerL, qtdCenL;
pthread_mutex_t freeVerL, freeHorL;
pthread_cond_t freeVerC, freeHorC;
int lefOc[N], rigOc[N], upOc[N], downOc[N]; 
int qtdHor, qtdVer, qtdCen;
int freeVer, freeHor; 
int turn;

void show_cruz(){
	system("clear");
	for(int i = 0; i < height; i++)
		printf("%s\n",cruz[i] );
}

void ver_close(){
	freeVer = 0;

	for(int i = 11; i < 16; i++){
		pthread_mutex_lock(&pista[10][i]);
		cruz[10][i] = '#';
	}
	for(int i = 17; i < 22; i++){
		pthread_mutex_lock(&pista[22][i]);
		cruz[22][i] = '#';
	}	
}

void hor_close(){
	freeHor = 0;

	for(int i = 17; i < 22; i++){
		pthread_mutex_lock(&pista[i][10]);
		cruz[i][10] = '#';
	}
	for(int i = 11; i < 16; i++){
		pthread_mutex_lock(&pista[i][22]);
		cruz[i][22] = '#';
	}
}

void ver_open(){
	for(int i = 11; i < 16; i++){
		cruz[10][i] = ' ';
		pthread_mutex_unlock(&pista[10][i]);
	}
	for(int i = 17; i < 22; i++){
		cruz[22][i] = ' ';
		pthread_mutex_unlock(&pista[22][i]);
	}

	pthread_mutex_lock(&freeVerL);
	freeVer = 1;
	pthread_cond_broadcast(&freeVerC);
	pthread_mutex_unlock(&freeVerL);
}

void hor_open(){
	for(int i = 17; i < 22; i++){
		cruz[i][10] = ' ';
		pthread_mutex_unlock(&pista[i][10]);
	}
	for(int i = 11; i < 16; i++){
		cruz[i][22] = ' ';
		pthread_mutex_unlock(&pista[i][22]);
	}
	
	pthread_mutex_lock(&freeHorL);
	freeHor = 1;
	pthread_cond_broadcast(&freeHorC);
	pthread_mutex_unlock(&freeHorL);
}

void in_center_p(int x, int y){
	if((y == 10 && x >= 17 && x <= 21) || 
		(x == 10 && y >= 11 && y <= 15) || 
		(y == 22 && x >= 11 && x <= 15) || 
		(x == 22 && y >= 17 && y <= 21)){
		pthread_mutex_lock(&qtdCenL);
		qtdCen++;
		pthread_mutex_unlock(&qtdCenL);
	}
}

int in_center_m(int x, int y){
	if((y == 22 && x >= 17 && x <= 21) || 
		(x == 10 && y >= 17 && y <= 21) || 
		(y == 10 && x >= 11 && x <= 16) || 
		(x == 22 && y >= 11 && y <= 16)){
		pthread_mutex_lock(&qtdCenL);
		qtdCen--;
		pthread_mutex_unlock(&qtdCenL);
		return 1;
	}
	return 0;
}

void* sem_controller(void* arg){
	long long opTime = OO;
	int lastTime = -1;

	while(1){
		if(qtdHor > 0 && (qtdVer == 0 || turn == 0)){
			if(freeVer)
				ver_close();
			turn = 0;
		}
		if(qtdVer > 0 && (qtdHor == 0 || turn == 1)){
			if(freeHor)
				hor_close();
			turn = 1;
		}

		if(turn == 0 && qtdCen == 0){
			if(!freeHor)
				hor_open();
			if(lastTime != 0){
				opTime = time(0);
				lastTime = 0;
			}
		}
		if(turn == 1 && qtdCen == 0){
			if(!freeVer)
				ver_open();
			if(lastTime != 1){
				opTime = time(0);
				lastTime = 1;
			}
		}

		if(qtdVer > 0 && turn == 0 && time(0) - opTime >= LIM){
			turn = 1;
			opTime = OO;
		}

		if(qtdHor > 0 && turn == 1 && time(0) - opTime >= LIM){
			turn = 0;
			opTime = OO;
		}

		usleep(35000);
	}
}

void* up_car(void* arg){
	int id = *((int *) arg);
	int x = 0, y = 11 + rand() % 5;
	
	pthread_mutex_lock(&qtdVerL);
	qtdVer++;
	pthread_mutex_unlock(&qtdVerL);
	
	while(x < height){
		if(x == 10){
			pthread_mutex_lock(&freeVerL);
			if(!freeVer)
				pthread_cond_wait(&freeVerC, &freeVerL);
			pthread_mutex_unlock(&freeVerL);
		}

		pthread_mutex_lock(&pista[x][y]);
		if(x){
			cruz[x - 1][y] = ' ';
			pthread_mutex_unlock(&pista[x - 1][y]);
		}
		cruz[x][y] = 'V';
		in_center_p(x, y);
		if(in_center_m(x, y)){
			pthread_mutex_lock(&qtdVerL);
			qtdVer--;
			pthread_mutex_unlock(&qtdVerL);
		}
		usleep(rand() % 300000 + 35000);
		x++;
	}
	cruz[x - 1][y] = ' ';
	pthread_mutex_unlock(&pista[x - 1][y]);
	upOc[id] = 0;
	
	pthread_exit(0);
}

void* down_car(void* arg){
	int id = *((int *) arg);
	int x = height - 1, y = 17 + rand() % 5;
	
	pthread_mutex_lock(&qtdVerL);
	qtdVer++;
	pthread_mutex_unlock(&qtdVerL);
	
	while(x >= 0){
		if(x == 22){
			pthread_mutex_lock(&freeVerL);
			if(!freeVer)
				pthread_cond_wait(&freeVerC, &freeVerL);
			pthread_mutex_unlock(&freeVerL);
		}

		pthread_mutex_lock(&pista[x][y]);
		if(x < height - 1){
			cruz[x + 1][y] = ' ';
			pthread_mutex_unlock(&pista[x + 1][y]);
		}
		cruz[x][y] = '^';
		in_center_p(x, y);
		if(in_center_m(x, y)){
			pthread_mutex_lock(&qtdVerL);
			qtdVer--;
			pthread_mutex_unlock(&qtdVerL);
		}
		usleep(rand() % 300000 + 35000);
		x--;
	}
	cruz[x + 1][y] = ' ';
	pthread_mutex_unlock(&pista[x + 1][y]);
	downOc[id] = 0;
	
	pthread_exit(0);
}

void* lef_car(void* arg){
	int id = *((int *) arg);
	int x = 17 + rand() % 5, y = 0;

	pthread_mutex_lock(&qtdHorL);
	qtdHor++;
	pthread_mutex_unlock(&qtdHorL);
	
	while(y < width){
		if(y == 10){
			pthread_mutex_lock(&freeHorL);
			if(!freeHor)
				pthread_cond_wait(&freeHorC, &freeHorL);
			pthread_mutex_unlock(&freeHorL);
		}

		pthread_mutex_lock(&pista[x][y]);
		if(y){
			cruz[x][y - 1] = ' ';
			pthread_mutex_unlock(&pista[x][y - 1]);
		}
		cruz[x][y] = '>';
		in_center_p(x, y);
		if(in_center_m(x, y)){
			pthread_mutex_lock(&qtdHorL);
			qtdHor--;
			pthread_mutex_unlock(&qtdHorL);
		}
		usleep(rand() % 300000 + 35000);
		y++;
	}
	cruz[x][y - 1] = ' ';
	pthread_mutex_unlock(&pista[x][y - 1]);
	lefOc[id] = 0;

	pthread_exit(0);
}

void* rig_car(void* arg){
	int id = *((int *) arg);
	int x = 11 + rand() % 5, y = width - 1;

	pthread_mutex_lock(&qtdHorL);
	qtdHor++;
	pthread_mutex_unlock(&qtdHorL);

	while(y >= 0){
		if(y == 22){
			pthread_mutex_lock(&freeHorL);
			if(!freeHor)
				pthread_cond_wait(&freeHorC, &freeHorL);
			pthread_mutex_unlock(&freeHorL);
		}

		pthread_mutex_lock(&pista[x][y]);
		if(y < width - 1){
			cruz[x][y + 1] = ' ';
			pthread_mutex_unlock(&pista[x][y + 1]);
		}
		cruz[x][y] = '<';
		in_center_p(x, y);
		if(in_center_m(x, y)){
			pthread_mutex_lock(&qtdHorL);
			qtdHor--;
			pthread_mutex_unlock(&qtdHorL);
		}
		usleep(rand() % 300000 + 35000);
		y--;
	}
	cruz[x][y + 1] = ' ';
	pthread_mutex_unlock(&pista[x][y + 1]);
	rigOc[id] = 0;

	pthread_exit(0);
}

void gen_cars(){
	if(rand() % 10000 <= PROB_UP){
		for(int i = 0; i < N; i++){
			if(!upOc[i]){
				upOc[i] = 1;
				int *id = (int *) malloc(sizeof(int));
				*id = i;
				pthread_create(&up[i], NULL, up_car, (void *) id);
				break;
			}
		}
	}
	if(rand() % 10000 <= PROB_DOWN){
		for(int i = 0; i < N; i++){
			if(!downOc[i]){
				downOc[i] = 1;
				int *id = (int *) malloc(sizeof(int));
				*id = i;
				pthread_create(&down[i], NULL, down_car, (void *) id);
				break;
			}
		}
	}
	if(rand() % 10000 <= PROB_LEF){
		for(int i = 0; i < N; i++){
			if(!lefOc[i]){
				lefOc[i] = 1;
				int *id = (int *) malloc(sizeof(int));
				*id = i;
				pthread_create(&lef[i], NULL, lef_car, (void *) id);
				break;
			}
		}
	}
	if(rand() % 10000 <= PROB_RIG){
		for(int i = 0; i < N; i++){
			if(!rigOc[i]){
				rigOc[i] = 1;
				int *id = (int *) malloc(sizeof(int));
				*id = i;
				pthread_create(&rig[i], NULL, rig_car, (void *) id);
				break;
			}
		}
	}
}

int main(){
	srand(time(0));

	int mode;

	while(1){
		system("clear");
		printf("Modo 1: muito trafego horizontal e muito pouco trafego vertical\n");
		printf("Modo 2: pouco trafego horizontal e muito trafego vertical\n");
		printf("Modo 3: muito trafego horizontal e muito trafego vertical\n");
		printf("Modo 4: pouco trafego horizontal e pouco trafego vertical\n");
		printf("Modo 5: maximo trafego horizontal e maximo trafego vertical\n");
		printf("Modo 6: maximo trafego horizontal e maximo trafego vertical sem semaforo\n");
		printf("Digite o numero do modo desejado: ");
		scanf("%d", &mode);

		if(mode == 1){
			PROB_UP = 100;
			PROB_DOWN = 200;
			PROB_LEF = 5000;
			PROB_RIG = 4000;
			break;
		}
		else if(mode == 2){
			PROB_UP = 5000;
			PROB_DOWN = 4000;
			PROB_LEF = 500;
			PROB_RIG = 400;
			break;	
		}
		else if(mode == 3){
			PROB_UP = 5000;
			PROB_DOWN = 4000;
			PROB_LEF = 3000;
			PROB_RIG = 6000;
			break;	
		}
		else if(mode == 4){
			PROB_UP = 500;
			PROB_DOWN = 400;
			PROB_LEF = 600;
			PROB_RIG = 300;
			break;	
		}
		else if(mode == 5 || mode == 6){
			PROB_UP = 10000;
			PROB_DOWN = 10000;
			PROB_LEF = 10000;
			PROB_RIG = 10000;
			break;	
		}
	}
	height = 33;
	width = 33;
	for(int i = 0; i <= height; i++)
		for(int j = 0; j <= width; j++)
			pthread_mutex_init(&pista[i][j], NULL);

	pthread_mutex_init(&qtdVerL, NULL);
	pthread_mutex_init(&qtdHorL, NULL);
	pthread_mutex_init(&qtdCenL, NULL);

	if(mode != 6){
		hor_close();
		ver_close();

		pthread_create(&semCo, NULL, sem_controller, NULL);
	}
	else
		freeVer = freeHor = 1;

	int cont = 0;
	while(1){
		if(!cont){
			gen_cars();
		}
		show_cruz();
		usleep(30000);
		if(mode < 5)
			cont = (cont + 1) % 6;
	}
}