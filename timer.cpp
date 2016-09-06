#include "timer.h"
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include "globalfunction.h"

void* EpollStartTimer(void* param)
{
	SaTimer* t = (SaTimer*)param;
	struct epoll_event ev;
	uint64_t data;
	while (true) {
		int n = epoll_wait(t->m_nEpollFd, &ev, 1, -1);
		if (n == -1) {
			break;
		}
		
		// read data, get ready for next time
		read(t->m_nTimerFd, &data, sizeof(uint64_t));
		
		if (t->m_pfCallback)
			t->m_pfCallback(t->m_pUserParam);
		
		if (t->m_bOneshot) {
			epoll_ctl(t->m_nEpollFd, EPOLL_CTL_DEL, t->m_nTimerFd, &t->m_EpollEvent);
			break;
		}    
	}
	return NULL;
}

SaTimer::SaTimer()
	: m_pfCallback(NULL)
	, m_pUserParam(NULL)
	, m_nTimerFd(0)
	, m_nEpollFd(0)
	, m_bOneshot(false)
	, m_nEpollThread(-1)
{
	memset(&m_Interval, 0, sizeof(itimerspec));
}

SaTimer::~SaTimer()
{
	DeInit();
}

void SaTimer::Init(int interval, bool oneshot, pfOnTimeCallback callback, void* pUserParam)
{
	m_pfCallback = callback;
	m_pUserParam = pUserParam;
	m_bOneshot  = oneshot;
	
	m_Interval.it_value.tv_sec  = interval / 1000;
	m_Interval.it_value.tv_nsec = (interval % 1000) * 1000 * 1000;		

	if (!oneshot) {
		m_Interval.it_interval.tv_sec  = interval / 1000;
		m_Interval.it_interval.tv_nsec = (interval % 1000) * 1000 * 1000;
	}

	m_nTimerFd = timerfd_create(CLOCK_MONOTONIC, 0);
	
	if (-1 == m_nTimerFd)
		sa::log_print("%s[%d]: timerfd_create error", __FILE__, __LINE__);

	m_nEpollFd = epoll_create(10);		
}

void SaTimer::Start()
{
	m_EpollEvent.data.fd = m_nTimerFd;
	m_EpollEvent.events  = EPOLLIN | EPOLLET;

	epoll_ctl(m_nEpollFd, EPOLL_CTL_ADD, m_nTimerFd, &m_EpollEvent);
	
	timerfd_settime(m_nTimerFd, 0, &m_Interval, 0);
	if (0 != pthread_create(&m_nEpollThread, NULL, EpollStartTimer, this))
		sa::log_print("%s[%d]: timer::start pthread_create error", __FILE__, __LINE__);
	
}

void SaTimer::DeInit()
{
	m_pfCallback = NULL;
	memset(&m_Interval, 0, sizeof(itimerspec));
	close(m_nTimerFd);
	close(m_nEpollFd);
}
