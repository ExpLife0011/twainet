#include "tcp_socket.h"

TCPSocket::TCPSocket()
{
	m_socket = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TCPSocket::TCPSocket(int socket)
: m_socket(socket)
{
}

TCPSocket::~TCPSocket()
{
	Close();
}

bool TCPSocket::Bind(const std::string& host, int port)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_in si;
	if(host.empty())
	{
		si.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		hostent* h = gethostbyname(host.c_str());
		if(h)
		{
			si.sin_addr.s_addr = *(u_long*)h->h_addr;
		}
		else
		{
			si.sin_addr.s_addr = inet_addr(host.c_str());
		}
	}

	si.sin_port = htons(port);
	si.sin_family = AF_INET;
	
	return bind(m_socket, (sockaddr*)&si, sizeof(si)) == 0;
}

bool TCPSocket::Listen(int limit)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	return listen(m_socket, limit) == 0;
}

int TCPSocket::Accept(std::string& ip, int& port)
{
	if(m_socket == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}

	sockaddr_in si;
#ifdef WIN32
		int len = sizeof(si);
#else
		unsigned int len = sizeof(si);
#endif/*WIN32*/
	int sock = (int)accept(m_socket, (sockaddr*)&si, &len);
	ip = inet_ntoa(si.sin_addr);
	port = si.sin_port;
	return sock;
}

bool TCPSocket::Connect(const std::string& host, int port)
{
	if(m_socket == INVALID_SOCKET || host.empty())
	{
		return false;
	}

	sockaddr_in si;
	hostent* h = gethostbyname(host.c_str());
	if(h)
	{
		si.sin_addr.s_addr = *(u_long*)h->h_addr;
	}
	else
	{
		si.sin_addr.s_addr = inet_addr(host.c_str());
	}

	si.sin_port = htons(port);
	si.sin_family = AF_INET;

	return connect(m_socket, (sockaddr*)&si, sizeof(si)) == 0;
}

bool TCPSocket::Send(char* data, int len)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	CSLocker locker(&m_cs);
	int sendlen = len;
	while (sendlen > 0)
	{
		int res = send(m_socket, data + len - sendlen, sendlen, 0);
		if(res <= 0)
		{
			return false;
		}

		sendlen -= res;
	}

	return true;
}

bool TCPSocket::Recv(char* data, int len)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	int recvlen = len;
	while (recvlen > 0)
	{
		int res = recv(m_socket, data + len - recvlen, recvlen, 0);
		if(res <= 0)
		{
			return false;
		}

		recvlen -= res;
	}
	
	return true;
}

bool TCPSocket::Close()
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}
	
	shutdown(m_socket, SD_BOTH);
#ifdef WIN32
	return closesocket(m_socket) == 0;
#else
	return close(m_socket) == 0;
#endif/*WIN32*/
}

void TCPSocket::GetIPPort(std::string& ip, int& port)
{
	sockaddr_in addr = {0};
#ifdef WIN32
		int len = sizeof(addr);
#else
		unsigned int len = sizeof(addr);
#endif/*WIN32*/
	if (!getsockname(m_socket, (sockaddr*)&addr, &len))
	{
		ip = inet_ntoa(addr.sin_addr);
		port = ntohs(addr.sin_port);
	}
}

int TCPSocket::GetSocket()
{
	return m_socket;
}

void TCPSocket::Initialize()
{
	m_socket = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int TCPSocket::GetMaxBufferSize()
{
	if(m_socket == INVALID_SOCKET)
	{
		return 0;
	}
	
	int sndsize, rcvsize;
#ifdef WIN32
	int len = sizeof(sndsize);
#else
	socklen_t len = sizeof(sndsize);
#endif/*WIN32*/
	if (getsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char*)&sndsize, &len) != 0 ||
	    getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&rcvsize, &len) != 0)
	{
		return 0;
	}
	
	return ((sndsize < rcvsize) ? sndsize : rcvsize);
}
