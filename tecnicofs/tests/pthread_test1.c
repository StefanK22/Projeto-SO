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
 * Then copies to a external file the writen characters.
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
	char *path2 = "external_file.txt";
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

	char buffer[SIZE], str_aux1[SIZE], str_aux2[SIZE];
	assert(tfs_read(f, buffer, SIZE) == SIZE);

	/* Creates two strings that represent the expected reading. */
	memset(str_aux1, 'a', COUNT);
	memset(str_aux1 + COUNT, 'b', COUNT);
	memset(str_aux2, 'b', COUNT);
	memset(str_aux2 + COUNT, 'a', COUNT);

	assert(strcmp(buffer, str_aux1) == 0 || strcmp(buffer, str_aux2) == 0);

    assert(tfs_close(f) != -1);
	
	/* Copies to external file. */
	assert(tfs_copy_to_external_fs(path, path2) != -1);
	FILE *fp = fopen(path2, "r");
	assert(fp != NULL);
	assert(fread(buffer, 1, sizeof(buffer), fp) == sizeof(buffer));
	assert(strcmp(buffer, str_aux1) == 0 || strcmp(buffer, str_aux2) == 0);
	assert(fclose(fp) != -1);
	
    printf("Successful test.\n");
	return 0;
}