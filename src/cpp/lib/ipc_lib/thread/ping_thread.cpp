#include "ping_thread.h"
#include "connector/ipc_connector.h"

PingThread::PingThread(IPCConnector* connector)
: m_connector(connector)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

PingThread::~PingThread()
{
}

void PingThread::ManagerFunc()
{
	PingMessage msg(m_connector);
	m_connector->toMessage(msg);
}