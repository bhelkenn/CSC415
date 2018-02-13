/* This program calculates the summation of numbers from 0 to a non-negative interger. All the numbers are divided into three groups with approximately equal size. Three child threads are created and the summation of all the numbers in one group calculated in a separate child thread. The parent thread adds the summation results obtained from all child threads and prints out the overall summation of all the numbers. */

#include <pthread.h> /* header file for Pthread API */
#include <stdio.h>

#define THREAD_NUM 3 /* number of child threads */

int sum_thread[THREAD_NUM]; /* summation results obtained from child threads */
int sum = 0; /* overall summation of all numbers */
void *runner(void *param); /* child threads call this function */

struct Section {
    int index;
    int start;
    int end;
}; /* structure with index of child thread, the starting number and the ending number of the group of numbers allocated to a child thread */

int main(int argc, char *argv[])
{    
    int i;

    struct Section section[THREAD_NUM]; /* divide all numbers into three groups and allocate one group of numbers to a separate child thread */
    for (i=0; i<THREAD_NUM; i++)
    {
        if (i<THREAD_NUM-1)
        {
            section[i].index = i;
            section[i].start = atoi(argv[1])/THREAD_NUM*i;
            section[i].end = atoi(argv[1])/THREAD_NUM*(i+1)-1;
        }
        else
        {
            section[i].index = i;
            section[i].start = atoi(argv[1])/THREAD_NUM*i; 
            section[i].end = atoi(argv[1]);
        } 
    }

    pthread_t tid[THREAD_NUM]; /* thread identifiers for child threads  */
    pthread_attr_t attr; /* thread attribute  */
    
    pthread_attr_init(&attr); /* initialize thread attribute variable with default attributes */

    for (i=0; i<THREAD_NUM; i++) /* create child threads */
    {    
        pthread_create(&tid[i], &attr, runner, &section[i]);
    }

    for (i=0; i<THREAD_NUM; i++) /* wait for child threads to exit */
    {
        pthread_join(tid[i], NULL);
        sum += sum_thread[i]; /* add the summation result obtained from each child thread to overall summation */
    }

    printf("sum = %d\n", sum);

    return 0;
}

void *runner(void *param)
{
    int i;
    int index, start, end;

    struct Section *section;
    section = param;

    index = section->index;
    start = section->start;
    end = section->end;

    sum_thread[index] = 0;

    for (i=start; i<=end; i++)
        sum_thread[index] += i;

    pthread_exit(0);
}
