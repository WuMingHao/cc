#ifndef SA_TCP_H
#define SA_TCP_H
#include <sys/epoll.h>			
#include <vector>
#include <map>
#include <string>

typedef struct _tagClientInfo
{
	std::string ip;
	int port;
}tagClientInfo;

typedef std::vector<epoll_event> saEventList;
typedef std::map<int, tagClientInfo> saClientMap;

class SaTCPServer
{
public:
	SaTCPServer(void);
	~SaTCPServer(void);	
public:
	int  Init(int nPort);
	int  Start(void);
	int  Stop(void);
	int  DeInit(void);
	void SetMaxConnect(int nNum);
private:
	void AddFd(int fd);
	void RemoveFd(int fd);
	int  DoRecvFd(int fd);
	void ReceiveClientConn(int nFd, std::string strIP, int nPort);
	void ReceiveClientLeave(int nFd);
	void DisplayClient(void);
private:
	int m_bInit;
	int m_bStart;
	int m_nMaxConnect;
	int m_nListenFd;
	int m_nEpollFd;	
	int m_nClientCount;
	char m_pchRecvData[10240];
	saEventList m_vEventList;
	saClientMap m_mClientMap;
};

#endif	// SA_TCP_H
