#include "fs/operations.h"
#include <assert.h>
#include <string.h>

int main() {

    //char *str = "AAA!";
    char *str = "Super Mario Bros. Logo.svg Sobre esta imagem Super Mario Bros. é um jogo eletrônico de plataforma desenvolvido pela Nintendo Research & Development 4 e publicado pela Nintendo para o Famicom em 1985 no Japão e para o Nintendo Entertainment System (NES) em 1985 e 1987 na América do Norte e Europa, respectivamente. É o sucessor do jogo de arcade Mario Bros., de 1983. Os jogadores controlam Mario, ou seu irmão Luigi no modo multijogador, enquanto viajam pelo Reino dos Cogumelos para resgatar a Princesa Peach de Bowser. Eles devem percorrer os mundos em uma visão em rolagem lateral, evitando perigos como inimigos e buracos com a ajuda de power-ups como o Super Cogumelo, Fire Flower e Starman. REPETIÇÃO Super Mario Bros. Logo.svg Sobre esta imagem Super Mario Bros. é um jogo eletrônico de plataforma desenvolvido pela Nintendo Research & Development 4 e publicado pela Nintendo para o Famicom em 1985 no Japão e para o Nintendo Entertainment System (NES) em 1985 e 1987 na América do Norte e Europa, respectivamente. É o sucessor do jogo de arcade Mario Bros., de 1983. Os jogadores controlam Mario, ou seu irmão Luigi no modo multijogador, enquanto viajam pelo Reino dos Cogumelos para resgatar a Princesa Peach de Bowser. Eles devem percorrer os mundos em uma visão em rolagem lateral, evitando perigos como inimigos e buracos com a ajuda de power-ups como o Super Cogumelo, Fire Flower e Starman. FIM";
    char *path = "/f1";
   // char buffer[40];

    assert(tfs_init() != -1);

    int f;
    ssize_t r;

    f = tfs_open(path, TFS_O_CREAT);
    assert(f != -1);

    r = tfs_write(f, str, strlen(str));
    assert(r == strlen(str));

    assert(tfs_close(f) != -1);

    //f = tfs_open(path, 0);
    //assert(f != -1);

    //r = tfs_read(f, buffer, sizeof(buffer) - 1);
    //assert(r == strlen(str));

    //int i = tfs_copy_to_external_fs(path, "teste.txt");
    //assert(i == 0);

    /*buffer[r] = '\0';
    assert(strcmp(buffer, str) == 0);

    assert(tfs_close(f) != -1);*/

    printf("Successful test.\n");

    return 0;
}
