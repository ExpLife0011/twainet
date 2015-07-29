#include "socket_factories.h"
#include "secure_socket.h"
#include "proxy_socket.h"
#include "udp_socket.h"
#ifdef WIN32
#	include "ppp_socket.h"
#	include "ppp/ppp_library.h"
#endif/*WIN32*/

#ifndef INVALID_SOCKET
#	define INVALID_SOCKET -1
#endif/*INVALID_SOCKET*/

/*******************************************************************************************************/
/*                                          TCPSocketFactory                                           */
/*******************************************************************************************************/
AnySocket* TCPSocketFactory::CreateSocket()
{
	return new TCPSocket();
}

AnySocket* TCPSocketFactory::CreateSocket(int socket)
{
	return new TCPSocket(socket);
}

SocketFactory* TCPSocketFactory::Clone()
{
	return new TCPSocketFactory;
}

/*******************************************************************************************************/
/*                                      TCPSecureSocketFactory                                         */
/*******************************************************************************************************/
TCPProxySocketFactory::TCPProxySocketFactory(const std::string& ip, int port, const std::string& user, const std::string& pass)
: m_ip(ip), m_port(port), m_user(user), m_pass(pass){}

AnySocket* TCPProxySocketFactory::CreateSocket()
{
	ProxyTCPSocket* socket = new ProxyTCPSocket(m_ip, m_port);
	socket->SetUserName(m_user);
	socket->SetPassword(m_pass);
	return socket;
}

AnySocket* TCPProxySocketFactory::CreateSocket(int socket)
{
	ProxyTCPSocket* new_socket = new ProxyTCPSocket(socket, m_ip, m_port);
	new_socket->SetUserName(m_user);
	new_socket->SetPassword(m_pass);
	return new_socket;
}

SocketFactory* TCPProxySocketFactory::Clone()
{
	return new TCPProxySocketFactory(m_ip, m_port, m_user, m_pass);
}

/*******************************************************************************************************/
/*                                      TCPSecureSocketFactory                                         */
/*******************************************************************************************************/
AnySocket* TCPSecureSocketFactory::CreateSocket()
{
	return new SecureTCPSocket();
}

AnySocket* TCPSecureSocketFactory::CreateSocket(int socket)
{
	return new SecureTCPSocket(socket);
}

SocketFactory* TCPSecureSocketFactory::Clone()
{
	return new TCPSecureSocketFactory;
}

/*******************************************************************************************************/
/*                                      TCPSecureSocketFactory                                         */
/*******************************************************************************************************/
TCPSecureProxySocketFactory::TCPSecureProxySocketFactory(const std::string& ip, int port, const std::string& user, const std::string& pass)
: m_ip(ip), m_port(port), m_user(user), m_pass(pass){}

AnySocket* TCPSecureProxySocketFactory::CreateSocket()
{
	SecureProxyTCPSocket* socket = new SecureProxyTCPSocket(m_ip, m_port);
	socket->SetUserName(m_user);
	socket->SetPassword(m_pass);
	return socket;
}

AnySocket* TCPSecureProxySocketFactory::CreateSocket(int socket)
{
	SecureProxyTCPSocket* new_socket = new SecureProxyTCPSocket(socket, m_ip, m_port);
	new_socket->SetUserName(m_user);
	new_socket->SetPassword(m_pass);
	return new_socket;
}

SocketFactory* TCPSecureProxySocketFactory::Clone()
{
	return new TCPSecureProxySocketFactory(m_ip, m_port, m_user, m_pass);
}

/*******************************************************************************************************/
/*                                         UDPSocketFactory                                            */
/*******************************************************************************************************/
AnySocket* UDPSocketFactory::CreateSocket()
{
	return new UDPSocket;
}

AnySocket* UDPSocketFactory::CreateSocket(int socket)
{
	return new UDPSocket(socket);
}

SocketFactory* UDPSocketFactory::Clone()
{
	return new UDPSocketFactory;
}

/*******************************************************************************************************/
/*                                         UDTSocketFactory                                            */
/*******************************************************************************************************/
UDTSocketFactory::UDTSocketFactory()
: m_udpSocket(INVALID_SOCKET) {}

AnySocket* UDTSocketFactory::CreateSocket()
{
	if(m_udpSocket != INVALID_SOCKET)
	{
		return new UDTSocket();
	}
	else
	{
		return new UDTSocket(m_udpSocket, true);
	}
}

AnySocket* UDTSocketFactory::CreateSocket(int socket)
{
	if(m_udpSocket != INVALID_SOCKET)
	{
		return new UDTSocket(socket, false);
	}
	else
	{
		return new UDTSocket(m_udpSocket, socket);
	}
}

void UDTSocketFactory::SetUdpSocket(int udpSocket)
{
	m_udpSocket = udpSocket;
}

SocketFactory* UDTSocketFactory::Clone()
{
	UDTSocketFactory* factory = new UDTSocketFactory;
	if(m_udpSocket != INVALID_SOCKET)
		factory->SetUdpSocket(m_udpSocket);
	return factory;
}

/*******************************************************************************************************/
/*                                      UDTSecureSocketFactory                                         */
/*******************************************************************************************************/
UDTSecureSocketFactory::UDTSecureSocketFactory()
: m_udpSocket(INVALID_SOCKET) {}

AnySocket* UDTSecureSocketFactory::CreateSocket()
{
	if(m_udpSocket == INVALID_SOCKET)
	{
		return new SecureUDTSocket();
	}
	else
	{
		return new SecureUDTSocket(m_udpSocket, true);
	}
}

AnySocket* UDTSecureSocketFactory::CreateSocket(int socket)
{
	if(m_udpSocket == INVALID_SOCKET)
	{
		return new SecureUDTSocket(socket, false);
	}
	else
	{
		return new SecureUDTSocket(m_udpSocket, socket);
	}
}

void UDTSecureSocketFactory::SetUdpSocket(int udpSocket)
{
	m_udpSocket = udpSocket;
}

SocketFactory* UDTSecureSocketFactory::Clone()
{
	UDTSecureSocketFactory* factory = new UDTSecureSocketFactory;
	if(m_udpSocket != INVALID_SOCKET)
		factory->SetUdpSocket(m_udpSocket);
	return factory;
}

#ifdef WIN32
/*******************************************************************************************************/
/*                                          PPPSocketFactory                                           */
/*******************************************************************************************************/
PPPSocketFactory::PPPSocketFactory(const std::string& sessionId)
	: m_sessionId(sessionId){}

AnySocket* PPPSocketFactory::CreateSocket()
{
	int socket = PPPLibrary::GetInstance().CreateSocket(m_sessionId);
	return new PPPSocket(socket);
}

AnySocket* PPPSocketFactory::CreateSocket(int socket)
{
	return new PPPSocket(socket);
}

SocketFactory* PPPSocketFactory::Clone()
{
	return new PPPSocketFactory(m_sessionId);
}

/*******************************************************************************************************/
/*                                     PPPSecureSocketFactory                                          */
/*******************************************************************************************************/
PPPSecureSocketFactory::PPPSecureSocketFactory(const std::string& sessionId)
	: m_sessionId(sessionId){}

AnySocket* PPPSecureSocketFactory::CreateSocket()
{
	int socket = PPPLibrary::GetInstance().CreateSocket(m_sessionId);
	return new SecurePPPSocket(socket);
}

AnySocket* PPPSecureSocketFactory::CreateSocket(int socket)
{
	return new SecurePPPSocket(socket);
}

SocketFactory* PPPSecureSocketFactory::Clone()
{
	return new PPPSecureSocketFactory(m_sessionId);
}
#endif/*WIN32*/