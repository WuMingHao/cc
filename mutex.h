#ifndef SA_MUTEX_H
#define SA_MUTEX_H

#include <pthread.h>


class SaMutex{
public:
	SaMutex(void);
	~SaMutex(void);

public:
	bool Init(void);
	bool DeInit(void);
	void Lock(void);
	void Unlock(void);

private:
	pthread_mutexattr_t attr;
	pthread_mutex_t mutex;
    
};

#endif	// SA_MUTEX_H
