#ifndef SA_TIMER_H
#define SA_TIMER_H
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <pthread.h>

typedef void (*pfOnTimeCallback)(void*);

class SaTimer
{
public:
    SaTimer(void);
    virtual ~SaTimer(void);
	
	friend void* EpollStartTimer(void* param);
public:
	void Init(int interval, bool oneshot, pfOnTimeCallback callback, void* pUserParam);
	void Start(void);
private:
	void DeInit(void);

private:
	pfOnTimeCallback   m_pfCallback;
	void*              m_pUserParam;
	struct itimerspec  m_Interval;
	int                m_nTimerFd;
	int                m_nEpollFd;
	bool               m_bOneshot;
	struct epoll_event m_EpollEvent;
	pthread_t          m_nEpollThread;
};

#endif //TIMER_H
