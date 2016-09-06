#include <pthread.h>
#include <sys/time.h>
#include "semp.h"

void SempUtils::semp_init(semp_t *semp, unsigned int count) {
	pthread_cond_init(&semp->condition, NULL);
	pthread_mutex_init(&semp->mutex, NULL);
	semp->semcount = count;
}

void SempUtils::semp_deinit(semp_t *semp) {
	pthread_cond_destroy(&semp->condition);
	pthread_mutex_destroy(&semp->mutex);
}

void SempUtils::semp_pend(semp_t *semp) {
	pthread_mutex_lock(&semp->mutex);
	while (semp->semcount == 0) {
		pthread_cond_wait(&semp->condition, &semp->mutex);
	}
	semp->semcount--;
	pthread_mutex_unlock(&semp->mutex);
}

void SempUtils::semp_post(semp_t *semp) {
	pthread_mutex_lock(&semp->mutex);
	semp->semcount++;
	pthread_cond_signal(&semp->condition);
	pthread_mutex_unlock(&semp->mutex);
}

void SempUtils::semp_reset(semp_t *semp) {
	pthread_mutex_lock(&semp->mutex);
	semp->semcount = 0;
	pthread_mutex_unlock(&semp->mutex);
}

void SempUtils::semp_wait(semp_t *semp) {
	pthread_mutex_lock(&semp->mutex);
	pthread_cond_wait(&semp->condition, &semp->mutex);
	pthread_mutex_unlock(&semp->mutex);
}

void SempUtils::semp_signal(semp_t *semp) {
	pthread_mutex_lock(&semp->mutex);
	pthread_cond_signal(&semp->condition);
	pthread_mutex_unlock(&semp->mutex);
}

