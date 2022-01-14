#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE 1000
#define COUNT 500

/* 
 * This test writes a 1000 characters in a new file, 
 * then creates 500 threads that open, read and 
 * close the file.
 */
void* thr_func(void* arg){

	char* a = (char*) arg;
	char *path = "/f1";
	char buffer[SIZE + 1];

	int f = tfs_open(path, 0);
	assert(f != -1);
	assert(tfs_read(f, buffer, SIZE) == SIZE);
	assert(strcmp(buffer, a) == 0);
	assert(tfs_close(f) != -1);

	return NULL;
}

int main(){
	
	pthread_t tid[COUNT];
	assert(tfs_init() != -1);

	char *path = "/f1";
	int f = tfs_open(path, TFS_O_CREAT);
	assert(f != -1);

	char a[SIZE];
	memset(a, 'A', SIZE);

	assert(tfs_write(f, a, SIZE) == SIZE);
	assert(tfs_close(f) != -1);

	for (int i = 0; i < COUNT; i++)
		assert(pthread_create(&tid[i], NULL, thr_func, (void*) &a) == 0);

	for (int i = 0; i < COUNT; i++)
		assert(pthread_join(tid[i], NULL) == 0);

    printf("Successful test.\n");
	return 0;
}