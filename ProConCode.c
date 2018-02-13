#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 16
int buffer[BUFFER_SIZE]; //shared buffer
sem_t empty, full; //empty and full semaphores
pthread_mutex_t mutex; // mutex for buffer access control
int filled = 0; //number of non-negative entries in buffer
int consumed = 0; //number of items consumed by consumer threads

void *produce(void *param);
void *consume(void *param);

struct passArgs {
	int thread_index;
	int item_count;
	int producer_count;
};

int main(int argc, char* argv[]) {
	/* argument validation */
	if (argc != 4) {
		char* s1 = "1) # of producers";
		char* s2 = "2) # of consumers";
		char* s3 = "3) # of items produced by each producer";
		fprintf(stderr, "Error: Missing arguments for %s, %s, and %s.", s1, s2, s3);
		return -1;
	}
	
	int producer_count = atoi(argv[1]);
	int consumer_count = atoi(argv[2]);
	int item_count = atoi(argv[3]);
	int num_of_threads = producer_count + consumer_count;
	
	// initialize all entries in buffer to -1 as an "empty" test later
	int i;
	for (i = 0; i < BUFFER_SIZE; i++) {
		buffer[i] = -1;
	}
	
	/* semaphore initialization */
	sem_init(&empty, 0, 1);
	sem_init(&full, 0, 0);
	
	/* mutex initialization */
	pthread_mutex_init(&mutex, NULL);
	
	pthread_t threads[num_of_threads];
	pthread_attr_t attr;
	
	/* default attributes */
	pthread_attr_init(&attr);
	
	/* initialize structs */
	struct passArgs args[num_of_threads];
	for (i = 0; i < num_of_threads; i++) {
		args[i].thread_index = i;
		args[i].item_count = item_count;
		args[i].producer_count = producer_count;
	}
	
	/* create threads */
	for (i = 0; i < num_of_threads; i++) {
		if (i < producer_count) {
			pthread_create(&threads[i], &attr, produce, &args[i]);
		}
		else {
			pthread_create(&threads[i], &attr, consume, &args[i]);
		}
	}
	
	/* terminate threads */
	for (i = 0; i < num_of_threads; i++) {
		pthread_join(threads[i], NULL);
	}
	
	return 0;
}

void *produce(void *param) {
	struct passArgs *args = param;
	int thread_index = args->thread_index;
	int item_count = args->item_count;
	
	int counter; // next counter integer from producer
	int index = 0; // keeps track of buffer location
	
	for (counter = 0; counter < item_count; counter++) {
		index = index % BUFFER_SIZE; // keep index within array bounds
		
		if (filled % BUFFER_SIZE == 0) {
			sem_wait(&empty); // wait until the buffer is empty before continuing
		}

		/* critical section */
		pthread_mutex_lock(&mutex);
		if (buffer[index] < 0) {
			buffer[index] = thread_index * item_count + counter;
			filled++;
		}
		index++; // moves to next buffer slot
		pthread_mutex_unlock(&mutex);
		
		if (filled == BUFFER_SIZE) sem_post(&full);
	} /* reaching the for loop means all items have been produced by this particular thread
		 * and so it can terminate safely
		 */
	pthread_exit(NULL);
}

void *consume(void *param) {
	struct passArgs *args = param;
	int producer_count = args->producer_count;
	int item_count = args->item_count;
	int index = 0;
	while (consumed < producer_count * item_count) {
		sem_wait(&full); // wait until the buffer is full before continuing
		
		index = index % BUFFER_SIZE; // keep index within bounds
		
		/* critical section */
		while (filled > 0) { // run until buffer is empty
			pthread_mutex_lock(&mutex);
			if (buffer[index] >= 0) {
				printf("%d\n", buffer[index]);
				buffer[index] = -1;
				filled--;
				consumed++;
			}
			index++;
			
			pthread_mutex_unlock(&mutex);
		} /* when the inside while loop terminates, filled == 0 and the buffer is empty */
		sem_post(&empty);
	} /*
		 * when the outside while loop terminates, consumed == producer_count * item_count 
		 * and the consumer threads can terminate, since the producer threads have also 
		 * terminated
		 */
	sem_post(&full);
	pthread_exit(NULL);
}