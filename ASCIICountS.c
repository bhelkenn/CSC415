#define BUFFER_SIZE 65536
#define THREADS 4
#define ASCII_COUNT 128

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *runner(void *param);

char buffer[BUFFER_SIZE];
int counts[ASCII_COUNT] = {0}; // total ascii counts
size_t actual_size; // actual size of file, determined upon reading
int i; //used in loops */
pthread_mutex_t mutex; //mutex lock int

struct Segment {
	int start;
	int end;
};

int main (int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./a.out test.txt\n");
		return -1;
	}
	
	FILE *fp = fopen("test.txt", "r");
	actual_size = fread(buffer, 1, BUFFER_SIZE, fp);
	fclose(fp);
	
	struct Segment segment[THREADS];
	for (i = 0; i < THREADS; i++) {
		segment[i].start = i * (int)actual_size / THREADS;
		if (i + 1 < THREADS)
			segment[i].end = (i + 1) * (int)actual_size / THREADS - 1;
		else
			segment[i].end = (int)actual_size - 1;
	}
	pthread_mutex_init(&mutex, NULL);
	
	pthread_t threads[THREADS];
	pthread_attr_t attr;
	
	/* default attributes */
	pthread_attr_init(&attr);
	
	printf("\n* WITH MUTEX *\n");
	
	/* create the threads */
	for (i = 0; i < THREADS; i++) {
		pthread_create(&threads[i], &attr, runner, &segment[i]);
	}
	/* wait for the threads to exit */
	for (i = 0; i < THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	/* print results */
	for (i = 0; i < ASCII_COUNT; i++) {
		if (counts[i] != 0) {
			if (i < 32)
				printf("%d occurrences of 0x%d\n", counts[i], i);
			else if (i == 32)
				printf("%d occurrences of [space]\n", counts[i]);
			else if (i < ASCII_COUNT - 1)
				printf("%d occurrences of %c\n", counts[i], i);
			else
				printf("%d occurrences of [delete]\n", counts[i]);
		}
	}
	return 0;
}

void *runner(void *param) {
	struct Segment *segment = param;
	
	int start = segment->start;
	int end = segment->end;
	
	pthread_mutex_lock(&mutex);
	for (i = start; i <= end; i++) {
		int ch = buffer[i]; //get ascii value
		counts[ch] += 1; //increment count for that ascii value
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}