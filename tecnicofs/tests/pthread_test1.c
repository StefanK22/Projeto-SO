#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define COUNT 10
#define SIZE 21

/* 
 * This test creates two threads that write 10 characters in a cicle.
 * Then checks if all the characters were written.
 */

void* thr_func_a(void* arg){

	int f = *((int*)arg);
	char* a = "a";
	for (int i = 0; i < COUNT; i++)
		assert(tfs_write(f, a, 1) == 1);

	return NULL;
}

void* thr_func_b(void* arg){
	int f = *((int*)arg);
	char* b = "b";
	for (int i = 0; i < COUNT; i++)
		assert(tfs_write(f, b, 1) == 1);

	return NULL;
}


int main(){

	char *path = "/f1";
	pthread_t tid[2];

	assert(tfs_init() != -1);
	int f = tfs_open(path, TFS_O_CREAT);
	assert(f != -1);

	assert(pthread_create(&tid[0], NULL, thr_func_a, (void*)&f) == 0);
	assert(pthread_create(&tid[1], NULL, thr_func_b, (void*)&f) == 0);
	assert(pthread_join(tid[0], NULL) == 0);
	assert(pthread_join(tid[1], NULL) == 0);

	assert(tfs_close(f) != -1);
	f = tfs_open(path, 0);
	assert(f != -1);

	char buffer[SIZE];
	assert(tfs_read(f, buffer, SIZE) == SIZE);

    assert(tfs_close(f) != -1);
	

	
    printf("Successful test.\n");
	return 0;
}