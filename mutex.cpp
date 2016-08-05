#include "mutex.h"

SaMutex::SaMutex() {

}
SaMutex::~SaMutex() {

}

bool SaMutex::Init() {
	int rc = pthread_mutexattr_init(&attr);
	if (rc != 0)
	{
		return false;
	}
	rc = pthread_mutex_init(&mutex, &attr);
	if (rc != 0)
	{
		return false;
	}
	return true;
}

bool SaMutex::DeInit() {
	pthread_mutexattr_destroy(&attr);
	pthread_mutex_destroy(&mutex);
	return true;
}

void SaMutex::Lock() {
	pthread_mutex_lock(&mutex);
}

void SaMutex::Unlock() {
	pthread_mutex_unlock(&mutex);
}
