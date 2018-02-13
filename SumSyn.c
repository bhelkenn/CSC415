#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define THREAD_NUM 8

int sum = 0;
void *runner(void *param);
pthread_mutex_t mutex;


struct Section {
    int start;
    int end;
};

int main(int argc, char *argv[])
{    
    int i;

    struct Section section[THREAD_NUM];
    for (i=0; i<THREAD_NUM; i++)
    {
        if (i<THREAD_NUM-1)
        {
            section[i].start = atoi(argv[1])/THREAD_NUM*i;
            section[i].end = atoi(argv[1])/THREAD_NUM*(i+1)-1;
        }
        else
        {
            section[i].start = atoi(argv[1])/THREAD_NUM*i; 
            section[i].end = atoi(argv[1]);
        }
    }
    
    
    pthread_mutex_init(&mutex, NULL);
    
    pthread_t tid[THREAD_NUM];
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);

    for (i=0; i<THREAD_NUM; i++)
    {    
        pthread_create(&tid[i], &attr, runner, &section[i]);
    }

    for (i=0; i<THREAD_NUM; i++)
    {
        pthread_join(tid[i], NULL);
    }

    printf("sum = %d\n", sum);

    return 0;
}

void *runner(void *param)
{
    int i;
    int start, end;

    struct Section *section;
    section = param;

    start = section->start;
    end = section->end;

    for (i=start; i<=end; i++)
    {
        pthread_mutex_lock(&mutex);
        sum += i;
        pthread_mutex_unlock(&mutex);
        
    }

    pthread_exit(0);
}
