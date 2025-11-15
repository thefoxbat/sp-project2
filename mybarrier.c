#include <stdlib.h>
#include "mybarrier.h"


mybarrier_t * mybarrier_init(unsigned int count)
{
    mybarrier_t * barrier = malloc(sizeof(*barrier));
    
	if (NULL == barrier) {
        return NULL;
    }
	
	/* Add your implementation here */
	if(count == 0)
	{
		free(barrier);
		return NULL;
	}
	barrier -> count = count;
	barrier -> waiting = 0;
	barrier -> barrier_satisfied = 0;
	barrier -> destroy_pending = 0;

	if(pthread_mutex_init(&barrier -> mutex, NULL) != 0)
	{
		free(barrier);
		return NULL;
	}
	if(pthread_cond_init(&barrier -> cond, NULL) != 0)
	{
		pthread_mutex_destroy(&barrier -> mutex);
		free(barrier);
		return NULL;
	}
    return barrier;	
}

int mybarrier_destroy(mybarrier_t * barrier)
{
	int ret = 0;
	
	/* Add your implementation here */
	if (barrier == NULL)
	{
		return -1;
	}
	pthread_mutex_lock(&barrier -> mutex);
	barrier -> destroy_pending = 1;
	if(barrier -> waiting > 0 && !barrier -> barrier_satisfied)
	{
		barrier -> barrier_satisfied = 1;
		pthread_cond_broadcast(&barrier -> cond);
	}
	pthread_mutex_unlock(&barrier -> mutex);

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 10000000;
	nanosleep(&ts, NULL);

	pthread_cond_destroy(&barrier -> cond);
	pthread_mutex_destroy(&barrier -> mutex);
	free(barrier);
	
	return ret;
}

int mybarrier_wait(mybarrier_t * barrier)
{
	int ret = 0;
	
	/* Add your implementation here */
	if(barrier == NULL)
	{
		return -1;
	}
	pthread_mutex_lock(&barrier -> mutex);
	if(barrier -> barrier_satisfied)
	{
		pthread_mutex_unlock(&barrier -> mutex);
		return -1;
	}
	barrier -> waiting++;
	if(barrier -> waiting == barrier -> count)
	{
		barrier -> barrier_satisfied = 1;
		pthread_cond_broadcast(&barrier -> cond);
		pthread_mutex_unlock(&barrier -> mutex);
		return ret;
	}
	else
	{
		while (!barrier -> barrier_satisfied)
		{
			pthread_cond_wait(&barrier -> cond, &barrier -> mutex);
		}
		pthread_mutex_unlock(&barrier -> mutex);
		return ret;
	}
}
