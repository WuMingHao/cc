// notifypipe.h
#ifndef NOTIFY_PIPE_H
#define NOTIFY_PIPE_H

#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

class Notify
{
public:
	Notify(void)
	{
		int pipefd[2];
		int ret = pipe(pipefd);
		assert(ret == 0);
		_receivefd = pipefd[0];
		_sendfd = pipefd[1];
		fcntl(_sendfd, F_SETFL, O_NONBLOCK);
	}
	virtual ~Notify(void)
	{
		close(_sendfd);
		close(_receivefd);
	}
	
	int recevierfd(void)
	{
		return _receivefd;
	}
	
	void notify(void)
	{
		write(_sendfd, "1", 1);
	}
private:
	int _receivefd;
	int _sendfd;
};


#endif	// NOTIFY_PIPE_H
