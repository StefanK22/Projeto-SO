#include "../fs/operations.h"
#include <assert.h>
#include <string.h>

#define COUNT 80
#define SIZE 1031

/**
   This test fills in a new file up to 20 blocks via multiple writes
   (therefore causing the file to hold 10 direct references + 10 indirect
   references from a reference block),
   each write always targeting only 1 block of the file, 
   then checks if the file contents are as expected
 */

int main() {

    char *path = "/f1";
 
    char *input = "Tendes em mi um novo engenho ardente, Se sempre em verso humilde celebrado Foi de mi vosso rio alegremente, Dai-me agora um som alto e sublimado, Um estilo grandíloco e corrente,Por que de vossas águas Febo ordene Que não tenham enveja às de Hipocrene.Dai-me üa fúria grande e sonorosa,E não de agreste avena ou frauta ruda, Mas de tuba canora e belicosa,Que o peito acende e a cor ao gesto muda; Dai-me igual canto aos feitos da famosa Gente vossa, que a Marte tanto ajuda; Que se espalhe e se cante no universo, Se tão sublime preço cabe em verso.E, vós, ó bem nascida segurançaDa Lusitana antiga liberdade,E não menos certíssima esperançaDe aumento da pequena Cristandade; Vós, ó novo temor da Maura lança, Maravilha fatal da nossa idade,Dada ao mundo por Deus, que todo o mande, Pera do mundo a Deus dar parte grande;Vós, tenro e novo ramo florecente De üa árvore, de Cristo mais amada Que nenhüa nascida no Ocidente, Cesárea ou Cristianíssima chamada (Vede-o no vosso escudo, qu";

    char output[SIZE];

    assert(tfs_init() != -1);

    /* Write input COUNT times into a new file */
    int fd = tfs_open(path, TFS_O_CREAT);
    assert(fd != -1);
    for (int i = 0; i < COUNT; i++) {
        assert(tfs_write(fd, input, SIZE) == SIZE);
    }
    assert(tfs_close(fd) != -1);

    /* Open again to check if contents are as expected */
    fd = tfs_open(path, 0);
    assert(fd != -1);

    for (int i = 0; i < COUNT; i++) {
        assert(tfs_read(fd, output, SIZE) == SIZE);
		printf("%s\n\n\n%d", output, i);
        assert (memcmp(input, output, SIZE) == 0);
    }

    assert(tfs_close(fd) != -1);


    printf("Sucessful test\n");

    return 0;
}