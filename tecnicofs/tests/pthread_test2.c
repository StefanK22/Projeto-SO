#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* thr_func(){

	char *path = "/f1";
	int a = tfs_open(path, TFS_O_CREAT);
	int *f;
	f = &a;

	return (void*)f;
}

int main(){

	assert(tfs_init() != -1);

	pthread_t tid[2];

	pthread_create(&tid[0], NULL, thr_func, NULL);
	pthread_create(&tid[1], NULL, thr_func, NULL);

	int a, b;
	pthread_join(tid[0],(void*) &a);
	pthread_join(tid[1],(void*) &b);

	assert(a != b);
    printf("Successful test.\n");
	return 0;
}
