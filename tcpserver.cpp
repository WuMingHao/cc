#include "tcpserver.h"
#include <sys/types.h>
#include <sys/socket.h>			// socket()
#include <arpa/inet.h>			// ntohs()
#include <string.h>				// memset()
#include <errno.h>				// errno
#include "sysutil.h"			// make_socket_non_blocking()

namespace
{
	const int kMaxConnect = 10;
}

SaTCPServer::SaTCPServer()
	: m_bInit(false),
	  m_bStart(false),
	  m_nMaxConnect(kMaxConnect),
	  m_nListenFd(-1),
	  m_nEpollFd(-1),
	  m_nClientCount(0)
{
}

SaTCPServer::~SaTCPServer()
{
	DeInit();
}

int SaTCPServer::Init(int nPort)
{
	if (m_bInit)
		return -1;

	m_nListenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_nListenFd < 0) {
		printf("socket error.\n");
		return -1;
	}

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port   = htons(nPort);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(m_nListenFd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
		perror("bind()");
		return -1;
	}

	make_socket_non_blocking(m_nListenFd);
	listen(m_nListenFd, m_nMaxConnect);

	m_nEpollFd = epoll_create1(0);
	AddFd(m_nListenFd);	
	m_vEventList.resize(m_nMaxConnect);		

	m_bInit = true;
	return 0;
}

int SaTCPServer::Start()
{
	if (m_bStart)
		return -1;
	
	m_bStart = true;
	sockaddr_in peeraddr;
	socklen_t len = sizeof(peeraddr);
	while (m_bStart) {
		int ready = epoll_wait(m_nEpollFd, &*m_vEventList.begin(), m_vEventList.size(), -1);
		if (ready == m_vEventList.size())
			m_vEventList.resize(ready * 2);
		if (-1 == ready) {
			printf("epoll_wait return error\n");
			break;
		}

		for (int i = 0; i < ready; ++ i) {
			if (m_vEventList[i].data.fd == m_nListenFd) {
				// new connect in
				int connfd = accept(m_nListenFd, (struct sockaddr *)&peeraddr, &len);
				if (-1 == connfd)
					continue;
				make_socket_non_blocking(connfd);
				AddFd(connfd);
				ReceiveClientConn(connfd, inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
			} else if (m_vEventList[i].events & EPOLLIN) {
				// receive data
				if (-1 == DoRecvFd(m_vEventList[i].data.fd)) {
					RemoveFd(m_vEventList[i].data.fd);
					ReceiveClientLeave(m_vEventList[i].data.fd);
				}
			}
		}
	}
	return 0;
}

int SaTCPServer::Stop()
{
	m_bStart = false;
	return 0;
}

int SaTCPServer::DeInit()
{
	if (!m_bInit)
		return -1;

	close(m_nListenFd);
	close(m_nEpollFd);
	m_vEventList.clear();
	m_nClientCount = 0;
	m_bInit = false;
	m_bStart = false;
	return 0;
}

void SaTCPServer::SetMaxConnect(int nNum)
{
	m_nMaxConnect = nNum;
}

void SaTCPServer::AddFd(int fd)
{
	epoll_event event;
	event.data.fd = fd;
	event.events  = EPOLLIN | EPOLLET;
	epoll_ctl(m_nEpollFd, EPOLL_CTL_ADD, fd, &event);
}

void SaTCPServer::RemoveFd(int fd)
{
	epoll_event event;
	epoll_ctl(m_nEpollFd, EPOLL_CTL_DEL, fd, &event);
}

int SaTCPServer::DoRecvFd(int fd)
{
	int recvlen = 0;
	int count = 0;
	int ret = 0;
 
	while (true) {
		count = read(fd, m_pchRecvData + recvlen, sizeof(m_pchRecvData) - recvlen);
		if (count == 0) {
			// End of file, remote has closed the connection 
			close(fd);
			ret = -1;
			break;
		} else if (count == -1) {
			if (errno != EAGAIN) {
				perror("read");
				ret = -1;
			}
			break;
		}
		recvlen += count;
	}

	// display data
	write(1, m_pchRecvData, recvlen);
	write(1, "\n", 1);
	// write back
	write(fd, m_pchRecvData, recvlen);
	
	return ret;
}

void SaTCPServer::ReceiveClientConn(int nFd, std::string strIP, int nPort)
{
	++ m_nClientCount;
	tagClientInfo info;
	info.ip   = strIP;
	info.port = nPort;
	m_mClientMap[nFd] = info;
	printf("accept[%d]: %s:%d\n", m_nClientCount, strIP.c_str(), nPort);
	DisplayClient();
}

 void SaTCPServer::ReceiveClientLeave(int nFd)
 {
	 -- m_nClientCount;
	 printf("client %s:%d leave.\n", m_mClientMap[nFd].ip.c_str(), m_mClientMap[nFd].port);
	 m_mClientMap.erase(nFd);
	 DisplayClient();
 }

 void SaTCPServer::DisplayClient()
 {
	 saClientMap::iterator it = m_mClientMap.begin();
	 int i = 0;
	 printf("-------------------------\n");
	 for (; it != m_mClientMap.end(); it ++) {
		 printf("[%d]: %s:%d\n", ++ i, it->second.ip.c_str(), it->second.port);
	 }
	 printf("-------------------------\n");
 }
