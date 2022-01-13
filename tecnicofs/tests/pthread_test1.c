#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


void* thr_func_a(void* arg){

	int f = *((int*)arg);
	char* a = "a";
	for (int i = 0; i < 10; i++){
		tfs_write(f, a, strlen(a));
	}

	return NULL;
}

void* thr_func_b(void* arg){
	int f = *((int*)arg);
	char* b = "b";
	for (int i = 0; i < 10; i++){
		tfs_write(f, b, strlen(b));
	}

	return NULL;
}


int main(){



	assert(tfs_init() != -1);
	char *path = "/f1";
	int f = tfs_open(path, TFS_O_CREAT);

	pthread_t tid[2];

	pthread_create(&tid[0], NULL, thr_func_a, (void*)&f);
	pthread_create(&tid[1], NULL, thr_func_b, (void*)&f);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	char buffer[31];

	tfs_close(f);
	f = tfs_open(path, 0);

	tfs_read(f, buffer, sizeof(buffer));

	printf("%s\n", buffer);

    assert(tfs_close(f) != -1);
	
	return 0;
}