#define BUFFER_SIZE 65536
#define THREADS 8
#define ASCII_COUNT 128

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *runner(void *param);

char buffer[BUFFER_SIZE];
int counts_thread[THREADS][ASCII_COUNT]; // ascii counts per thread
int counts[ASCII_COUNT]; // total ascii counts
size_t actual_size; // actual size of file, determined upon reading

struct Segment {
	int index;
	int start;
	int end;
};

int main (int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./a.out test.txt\n");
		return -1;
	}
	
	FILE *fp;
	fp = fopen("test.txt", "r");
	actual_size = fread(buffer, 1, BUFFER_SIZE, fp);
	fclose(fp);
	
	/* used in for loops */
	int i, j;
	
	struct Segment segment[THREADS];
	for (i = 0; i < THREADS; i++) {
		segment[i].index = i;
		segment[i].start = i * actual_size / THREADS;
		if (i < THREADS - 1)
			segment[i].end = (i + 1) * (actual_size / THREADS) - 1;
		else
			segment[i].end = actual_size;
	}
	
	pthread_t threads[THREADS];
	pthread_attr_t attr;
	
	/* default attributes */
	pthread_attr_init(&attr);
	
	/* create the threads */
	for (i = 0; i < THREADS; i++) {
		pthread_create(&threads[i], &attr, runner, &segment[i]);
	}
	
	/* wait for the threads to exit */
	for (i = 0; i < THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	/* combine results */
	for (i = 0; i < THREADS; i++) {
		for (j = 0; j < ASCII_COUNT; j++) {
			counts[j] += counts_thread[i][j];
		}
	}
	
	/* print results */
	for (i = 0; i < ASCII_COUNT; i++) {
		if (i < 32)
			printf("\n%d occurrences of 0x%d", counts[i], i);
		else if (i == 32)
			printf("\n%d occurrences of [space]", counts[i]);
		else if (i < ASCII_COUNT - 1)
			printf("\n%d occurrences of %c", counts[i], i);
		else
			printf("\n%d occurrences of [delete]\n", counts[i]);
	}
	
	return 0;
}

void *runner(void *param) {
	struct Segment *segment = param;
	
	int index = segment->index;
	int start = segment->start;
	int end = segment->end;
	
	int i;
	for (i = start; i <= end; i++) {
		int ch = buffer[i]; //get ascii value
		counts_thread[index][ch] += 1; //increment count for that ascii value
	}
	pthread_exit(NULL);
}