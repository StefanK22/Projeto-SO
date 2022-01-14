#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE 1000

/* 
 * This test creates two threads that write 1000 
 * characters in a new file, copy to an external 
 * file and check if the writing was done correctly.
 */


void* thr_func_1(void* arg){

	char *buffer = (char*) arg;
	char *path = "/f1";
	char *path2 = "external_file1.txt";
	char buffer2[SIZE + 1];

	int f = tfs_open(path, TFS_O_CREAT);
	assert(f != -1);
	assert(tfs_write(f, buffer, strlen(buffer)) == strlen(buffer));
	assert(tfs_copy_to_external_fs(path, path2) != -1);

	FILE *fp = fopen(path2, "r");
	assert(fp != NULL);
	assert(fread(buffer2, 1, SIZE, fp) == SIZE);
	assert(strcmp(buffer, buffer2) == 0);
	assert(fclose(fp) != -1);

	return NULL;
}

void* thr_func_2(void* arg){

	char *buffer = (char*) arg;
	char *path = "/f1";
	char *path2 = "external_file2.txt";
	char buffer2[SIZE + 1];

	int f = tfs_open(path, TFS_O_CREAT);
	assert(f != -1);
	assert(tfs_write(f, buffer, strlen(buffer)) == strlen(buffer));
	assert(tfs_copy_to_external_fs(path, path2) != -1);

	FILE *fp = fopen(path2, "r");
	assert(fp != NULL);
	assert(fread(buffer2, 1, SIZE, fp) == SIZE);
	assert(strcmp(buffer, buffer2) == 0);
	assert(fclose(fp) != -1);

	return NULL;
}

int main(){
	
	pthread_t tid[2];
	assert(tfs_init() != -1);
	
	/* Creates a buffer that contains 1000 characters. */
	char buffer[SIZE + 1];
	memset(buffer, 'A', SIZE);
	memset(buffer + SIZE - 1, '\0', 1);

	assert(pthread_create(&tid[0], NULL, thr_func_1, (void*) &buffer) == 0);
	assert(pthread_create(&tid[1], NULL, thr_func_2, (void*) &buffer) == 0);

	assert(pthread_join(tid[0], NULL) == 0);
	assert(pthread_join(tid[1], NULL) == 0);

    printf("Successful test.\n");
	return 0;
}