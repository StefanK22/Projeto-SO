#include "operations.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tfs_init() {
	state_init();

	/* create root inode */
	int root = inode_create(T_DIRECTORY);
	if (root != ROOT_DIR_INUM) {
		return -1;
	}

	return 0;
}

int tfs_destroy() {
	state_destroy();
	return 0;
}

static bool valid_pathname(char const *name) {
	return name != NULL && strlen(name) > 1 && name[0] == '/';
}


int tfs_lookup(char const *name) {
	if (!valid_pathname(name)) {
		return -1;
	}

	// skip the initial '/' character
	name++;

	return find_in_dir(ROOT_DIR_INUM, name);
}

int tfs_open(char const *name, int flags) {
	int inum;
	size_t offset;

	/* Checks if the path name is valid */
	if (!valid_pathname(name)) {
		return -1;
	}
	// FALTA PROTEGER
	inum = tfs_lookup(name);
	if (inum >= 0) {
		/* The file already exists */
		inode_t *inode = inode_get(inum);
		pthread_mutex_lock(&inode->i_lock);	
		if (inode == NULL) {
			return -1;
		}
		/* Trucate (if requested) */
		if (flags & TFS_O_TRUNC) {
			if (inode->i_size > 0) {
				for(size_t i = 0; i < NUM_DIRECT_REF ; i++){
					if (data_block_free(inode->data_block_list[i]) == -1) {
						pthread_mutex_unlock(&inode->i_lock);
						return -1;
					}
				}
				inode->i_size = 0;
			}
		}
		/* Determine initial offset */
		if (flags & TFS_O_APPEND) {
			offset = inode->i_size;
		} else {
			offset = 0;
		}
		pthread_mutex_unlock(&inode->i_lock);
	} else if (flags & TFS_O_CREAT) {
		/* The file doesn't exist; the flags specify that it should be created*/
		/* Create inode */
		inum = inode_create(T_FILE);
		if (inum == -1) {
			return -1;
		}
		/* Add entry in the root directory */
		if (add_dir_entry(ROOT_DIR_INUM, inum, name + 1) == -1) {
			inode_delete(inum);
			return -1;
		}
		offset = 0;
	} else {
		return -1;
	}

	/* Finally, add entry to the open file table and
	 * return the corresponding handle */
	return add_to_open_file_table(inum, offset);

	/* Note: for simplification, if file was created with TFS_O_CREAT and there
	 * is an error adding an entry to the open file table, the file is not
	 * opened but it remains created */
}


int tfs_close(int fhandle) { return remove_from_open_file_table(fhandle); }

ssize_t tfs_write(int fhandle, void const *buffer, size_t to_write) {
	open_file_entry_t *file = get_open_file_entry(fhandle);
	if (file == NULL) {
		return -1;
	}

	/* From the open file table entry, we get the inode */
	pthread_mutex_lock(&file->of_lock);
	inode_t *inode = inode_get(file->of_inumber);
	pthread_mutex_lock(&inode->i_lock);
	if (inode == NULL) {
		pthread_mutex_unlock(&file->of_lock);
		return -1;
	}

	/* Determine how many bytes to write */
	if (to_write + file->of_offset > (BLOCK_SIZE * (NUM_DIRECT_REF + MAX_SUP_BLOCKS))) {
		to_write = BLOCK_SIZE * (NUM_DIRECT_REF + MAX_SUP_BLOCKS) - file->of_offset;
	} 

	if (to_write > 0) {
		if (inode->i_size == 0) {
			/* If empty file, allocate new block */
			inode->data_block_list[0] = data_block_alloc();
		}

		/* Stores the number of bytes that are going to be written */
		size_t to_be_written = to_write; 

		/* Stores the bytes already written */
		size_t written = 0; 
		/* Writes from the first block */
		if (file->of_offset == 0){
			for (int i = 0; to_be_written != 0; i++){
				void *block = block_order_get(i, inode);
				if (block == NULL) {
					pthread_mutex_unlock(&file->of_lock);
					return -1;
				}
				/* Finishes writing */
				if (to_be_written < BLOCK_SIZE){
					memcpy(block, buffer + written, to_be_written);
					to_be_written = 0;
				}
				else {
					memcpy(block, buffer + written, BLOCK_SIZE);
					written += BLOCK_SIZE;
					to_be_written -= BLOCK_SIZE;
				}
				inode->i_size = to_write - to_be_written;
			}

		}
		else {
			/* Determine the order of the current block in the file*/
			int file_block_order = (int) file->of_offset / BLOCK_SIZE;

			/* Determine the offset of the current block*/
			size_t block_offset = file->of_offset % BLOCK_SIZE + 1;
			void *block = block_order_get(file_block_order, inode);
			if (block == NULL){
				//FALTA
				pthread_mutex_unlock(&file->of_lock);
				return -1;
			}
			
			/* Bytes to write are smaller than a BLOCK SIZE */
			if (block_offset + to_be_written <= BLOCK_SIZE){
				memcpy(block + block_offset - 1, buffer, to_be_written);
			}
			else {
				/* Writes until the end of the current block of the file */
				memcpy(block + block_offset - 1, buffer, BLOCK_SIZE - block_offset);
				written = BLOCK_SIZE - block_offset;
				to_be_written -= written;
				
				/* Writes from the block next to the current one of the file */ 
				for (int i = file_block_order + 1; to_be_written != 0; i++){
					block = block_order_get(i, inode);
					if (block == NULL){
						// FALTA
						pthread_mutex_unlock(&file->of_lock);
						return -1;
					}

					/* Finishes writing */
					if (to_be_written < BLOCK_SIZE){
						memcpy(block - 1, buffer + written, to_be_written);
						inode->i_size += to_be_written;
						to_be_written = 0;
					}
					/* Keeps writing */
					else {
						memcpy(block - 1, buffer + written, BLOCK_SIZE);
						written += BLOCK_SIZE;
						to_be_written -= BLOCK_SIZE;
						inode->i_size += BLOCK_SIZE;
					}
				}
			}
		}
		/* The offset associated with the file handle is
		 * incremented accordingly */
		file->of_offset += to_write;
		if (file->of_offset > inode->i_size) {
			inode->i_size = file->of_offset;
		}
	}
	pthread_mutex_unlock(&inode->i_lock);
	pthread_mutex_unlock(&file->of_lock);
	return (ssize_t)to_write;
}


ssize_t tfs_read(int fhandle, void *buffer, size_t len) {
	open_file_entry_t *file = get_open_file_entry(fhandle);
	if (file == NULL) {
		return -1;
	}

	pthread_mutex_lock(&file->of_lock);
	/* From the open file table entry, we get the inode */
	inode_t *inode = inode_get(file->of_inumber);
	if (inode == NULL) {
		pthread_mutex_unlock(&file->of_lock);
		return -1;
	}

	pthread_mutex_lock(&inode->i_lock);
	/* Determine how many bytes to read */
	size_t to_read = inode->i_size - file->of_offset + 1;
	if (to_read > len) {
		to_read = len;
	}
	
	/* How many bytes to read */
	size_t to_be_read = to_read;

	/* Determine the order of the current block in the file */
	int file_block_order = (int) file->of_offset / BLOCK_SIZE;

	/* Determine the offset of the current block */
	size_t block_offset = file->of_offset % BLOCK_SIZE;
	
	void *block = block_order_get(file_block_order, inode);
	if (block == NULL){
		pthread_mutex_unlock(&file->of_lock);
		return -1;
	}

	/* Bytes to read are smaller than the BLOCK_SIZE */
	if (block_offset + to_be_read <= BLOCK_SIZE){
		memcpy(buffer, block + block_offset, to_be_read);
	}
	
	else {
		memcpy(buffer, block + block_offset, BLOCK_SIZE - block_offset);
		/* Amount of bytes that have been read */
		size_t read = BLOCK_SIZE - block_offset;
		to_be_read -= read;
		
		/* Reads from the block next to the current one of the file */
		for (int i = file_block_order + 1; to_be_read != 0; i++){
			block = block_order_get(i, inode);
			if (block == NULL)
				return -1;
				
			/* Finishes reading */
			if (to_be_read < BLOCK_SIZE){
				memcpy(buffer + read, block, to_be_read);
				to_be_read = 0;
			}
			
			/* Keeps reading */
			else {
				memcpy(buffer + read, block, BLOCK_SIZE);
				read += BLOCK_SIZE;
				to_be_read -= BLOCK_SIZE;
			}
		}
	}
	/* Update the file offset */
	file->of_offset += to_read;
	pthread_mutex_unlock(&file->of_lock);
	pthread_mutex_unlock(&inode->i_lock);
	//teste();
	return (ssize_t)to_read;
}

int tfs_copy_to_external_fs(char const *source_path, char const *dest_path){

	char buffer[128];
	
	/* Bytes that have been read */
	ssize_t bytes_read;

	/* Opens the source file */
	int fs = tfs_open(source_path, TFS_O_CREAT);
	if (fs == -1) 
		return -1;

	/* Opens the destination file */
	FILE *fd = fopen(dest_path, "w");
	if (fd == NULL)
		return -1;
	
	/* Reads and copies the file */
	do {
		bytes_read = tfs_read(fs, buffer, sizeof(buffer));
		if (bytes_read < 0)
			return -1;

		if (fwrite(buffer, 1, (size_t) bytes_read, fd) < 0)
			return -1; 
		
	} while (bytes_read == 128);

	/* Closes the two files */ 
	if (tfs_close(fs) == -1 || fclose(fd) == -1) return -1;

	return 0;
}
