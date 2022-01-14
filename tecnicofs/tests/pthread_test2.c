#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/*
 * This tests creates 3 threads that open a new file, then
 * checks if each file handle is different from the other.
 */

/* Global variables that store each file handle. */
int a, b, c;

void* thr_func_a(){
	char *path = "/f1";
	int f = tfs_open(path, TFS_O_CREAT);
	a = f;
	return NULL;
}

void* thr_func_b(){
	char *path = "/f1";
	int f = tfs_open(path, TFS_O_CREAT);
	b = f;
	return NULL;
}

void* thr_func_c(){
	char *path = "/f1";
	int f = tfs_open(path, TFS_O_CREAT);
	c = f;
	return NULL;
}

int main(){

	pthread_t tid[3];

	assert(tfs_init() != -1);

	assert(pthread_create(&tid[0], NULL, thr_func_a, NULL) == 0);
	assert(pthread_create(&tid[1], NULL, thr_func_b, NULL) == 0);
	assert(pthread_create(&tid[2], NULL, thr_func_c, NULL) == 0);

	assert(pthread_join(tid[0], NULL) == 0);
	assert(pthread_join(tid[1], NULL) == 0);
	assert(pthread_join(tid[2], NULL) == 0);

	/* Compares the 3 file handles */
	assert(a != b && a != c && b != c);
    printf("Successful test.\n");
	return 0;
}
