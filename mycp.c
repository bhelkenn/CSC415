#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

int readToBuffer(int, char*);
int writeToFile(char*);

char buffer[BUFFER_SIZE];
ssize_t file_size = -1; // if -1 then an error occurred during read

int main (int argc, char* argv[]) {
	if (argc == 3) {
		char file1[16], file2[16];
		strcpy(file1, "./");
		strcat(file1, argv[1]);
		
		strcpy(file2, "./");
		strcat(file2, argv[2]);
		int descriptor = -1; // if -1 then the file doesn't exist
		if (readToBuffer(descriptor, file1)) {
			writeToFile(file2);
		}
	}
	else fprintf(stderr, "Error: %s requires 2 filename arguments.\n", argv[0]);
	return 0;
}

int readToBuffer(int descriptor, char* filename) {
	descriptor = open(filename, O_RDONLY);
	if (descriptor < 0) {
		fprintf(stderr, "Error: %s doesn't exist!\n", filename);
		return 0;
	}
	
	file_size = read(descriptor, buffer, BUFFER_SIZE);
	if (file_size < 0) {
		fprintf(stderr, "Error: Unable to read %s!\n", filename);
		return 0;
	}
	if (close(descriptor) < 0) {
		fprintf(stderr, "Error: Unable to close %s!\n", filename);
		return 0;
	}
	/* all fail conditions accounted for */
	return 1;
}

int writeToFile(char* filename) {
	int descriptor = -1; // new file descriptor
	
	descriptor = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (descriptor < 0) {
		fprintf(stderr, "Error: Unable to create %s!\n", filename);
		return 0;
	}
	
	ssize_t written = write(descriptor, buffer, file_size);
	if (written != file_size || written < 0) {
		fprintf(stderr, "Error: unexpected written amounts: %d written vs %d size\n", written, file_size);
		return 0;
	}
	
	if (close(descriptor) < 0) {
		fprintf(stderr, "Error: Unable to close %s!\n", filename);
		return 0;
	}
	/* all fail conditions accounted for */
	printf("copied %d bytes\n", written);
	return 1;
}






