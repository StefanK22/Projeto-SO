#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


void* thr_func(void* arg){

	int f = *((int*)arg);
	char buffer[736];
	tfs_read(f, buffer, 736);

	printf("%s\n\n\n", buffer);

	return NULL;
}


int main(){

	pthread_t tid[2];
	char *str = "Super Mario Bros. Logo.svg Sobre esta imagem Super Mario Bros. é um jogo eletrônico de plataforma desenvolvido pela Nintendo Research & Development 4 e publicado pela Nintendo para o Famicom em 1985 no Japão e para o Nintendo Entertainment System (NES) em 1985 e 1987 na América do Norte e Europa, respectivamente. É o sucessor do jogo de arcade Mario Bros., de 1983. Os jogadores controlam Mario, ou seu irmão Luigi no modo multijogador, enquanto viajam pelo Reino dos Cogumelos para resgatar a Princesa Peach de Bowser. Eles devem percorrer os mundos em uma visão em rolagem lateral, evitando perigos como inimigos e buracos com a ajuda de power-ups como o Super Cogumelo, Fire Flower e Starman. REPETIÇÃO Super Mario Bros.";

	assert(tfs_init() != -1);
	char *path = "/f1";
	int f = tfs_open(path, TFS_O_CREAT);
    assert(f != -1);

    int r = (int) tfs_write(f, str, strlen(str));
    assert(r == strlen(str));

	pthread_create(&tid[0], NULL, thr_func, (void*)&f);
	pthread_create(&tid[1], NULL, thr_func, (void*)&f);

	pthread_join(&tid[0], NULL);
	pthread_join(&tid[1], NULL);

}