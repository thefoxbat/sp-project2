<<<<<<< HEAD
#include <stdlib.h>
#include "mybarrier.h"


mybarrier_t * mybarrier_init(unsigned int count)
{
    mybarrier_t * barrier = malloc(sizeof(*barrier));
    
	if (NULL == barrier) {
        return NULL;
    }

	if (count == 0) {
		free(barrier);
		return NULL;
	}

	barrier->count = (int)count;
	barrier->waiting = 0;
	barrier->active = 0;
	barrier->released = 0;
	barrier->aborted = 0;

	if (pthread_mutex_init(&barrier->mutex, NULL) != 0) {
		free(barrier);
		return NULL;
	}

	if (pthread_cond_init(&barrier->cond, NULL) != 0) {
		pthread_mutex_destroy(&barrier->mutex);
		free(barrier);
		return NULL;
	}

	if (pthread_cond_init(&barrier->destroy_cond, NULL) != 0) {
		pthread_cond_destroy(&barrier->cond);
		pthread_mutex_destroy(&barrier->mutex);
		free(barrier);
		return NULL;
	}

	return barrier;
}

int mybarrier_destroy(mybarrier_t * barrier)
{

	if (barrier == NULL) {
		return -1;
	}

	pthread_mutex_lock(&barrier->mutex);

	/* If barrier hasn't been released, mark it aborted and wake waiters so
	 * they can return. This ensures destroy will not free resources while
	 * threads are still blocked in mybarrier_wait(). */
	if (!barrier->released) {
		barrier->aborted = 1;
		pthread_cond_broadcast(&barrier->cond);
	}

	/* Wait until all threads currently inside wait() have left */
	while (barrier->active > 0) {
		pthread_cond_wait(&barrier->destroy_cond, &barrier->mutex);
	}

	pthread_mutex_unlock(&barrier->mutex);

	pthread_cond_destroy(&barrier->destroy_cond);
	pthread_cond_destroy(&barrier->cond);
	pthread_mutex_destroy(&barrier->mutex);

	free(barrier);
	return 0;
}

int mybarrier_wait(mybarrier_t * barrier)
{

	if (barrier == NULL) {
		return -1;
	}

	pthread_mutex_lock(&barrier->mutex);

	/* If barrier already used (released or aborted), can't use again */
	if (barrier->released || barrier->aborted) {
		pthread_mutex_unlock(&barrier->mutex);
		return -1;
	}

	/* Mark this thread as active and waiting */
	barrier->active++;
	barrier->waiting++;

	if (barrier->waiting == barrier->count) {
		/* Last thread arrived: release all and mark single-use */
		barrier->released = 1;
		barrier->waiting = 0;
		pthread_cond_broadcast(&barrier->cond);

		/* this thread will now proceed; decrement active before leaving */
		barrier->active--;
		if (barrier->active == 0) {
			pthread_cond_signal(&barrier->destroy_cond);
		}
		pthread_mutex_unlock(&barrier->mutex);
		return 0;
	}

	/* Wait until released or aborted */
	while (!barrier->released && !barrier->aborted) {
		pthread_cond_wait(&barrier->cond, &barrier->mutex);
	}

	int ret = 0;
	if (!barrier->released) {
		/* aborted */
		ret = -1;
	} else {
		ret = 0;
	}

	/* leaving wait() */
	barrier->active--;
	if (barrier->active == 0) {
		pthread_cond_signal(&barrier->destroy_cond);
	}

	pthread_mutex_unlock(&barrier->mutex);
	return ret;
}
=======
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
>>>>>>> ddd7f05bc5f35db669a3bca20d2c59f27002315c
