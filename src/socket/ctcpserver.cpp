#include "stdafx.h"
#include "ctcpserver.h"
#include "iniconfig.h"
#include <sstream>
#include <iostream>
#include <boost/function.hpp>
#include <boost/thread.hpp>

CTcpServer::CTcpServer()
{
}

CTcpServer::~CTcpServer()
{
}

void CTcpServer::SetParam(socket_param_ptr &param)
{
    CSocket::SetParam(param);

    std::stringstream section;
    section << "TcpServer" << param->nId;
    std::string szIp = ReadIniStdString(section.str().c_str(), "Address", "0.0.0.0", param->szIniPath.c_str());
    int nPort = ReadIniInt(section.str().c_str(), "Port", 10000, param->szIniPath.c_str());

    tcp::endpoint endpoint(boost::asio::ip::address::from_string(szIp), nPort);
    m_acceptor = boost::make_shared<tcp::acceptor>(m_io_service, endpoint);

    start_accept();

	boost::thread thrd(boost::bind(&boost::asio::io_service::run, &m_io_service));
}

int CTcpServer::Send(const char * szSendBuf, int nlen, const char * szDstIP, int nDstPort)
{
	int nIndex = getProcIndex(szDstIP, nDstPort);
	if (nIndex == -1)
	{
		if (m_vecProcs.size() == 1)
			return m_vecProcs[0]->Send(szSendBuf, nlen);
		else
			return SOCKET_ERROR;
	}
	return m_vecProcs[nIndex]->Send(szSendBuf, nlen);
}

int CTcpServer::Recv(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort)
{
	return 0;
}

void CTcpServer::start_accept()
{
    tcpserver_proc_ptr proc_ptr = boost::make_shared<TcpServerProc>(m_io_service, m_param);
    m_acceptor->async_accept(proc_ptr->getSocket(),
                            boost::bind(&CTcpServer::handle_accept, this, proc_ptr,
                                        boost::asio::placeholders::error));
}

void CTcpServer::handle_accept(tcpserver_proc_ptr proc_ptr,
      const boost::system::error_code& error)
{
	tcp::endpoint remote_endpoint = proc_ptr->getSocket().remote_endpoint();
    if (!error)
    {
		proc_ptr->socket_closed = boost::bind(&CTcpServer::socket_closed, this, _1, _2);
		m_vecProcs.push_back(proc_ptr);
        proc_ptr->start();
#ifdef _DEBUG
        std::cout << "New connection: " << remote_endpoint.address().to_string()
                  << ":" << remote_endpoint.port() << std::endl;
#endif
        if (m_param->callback)
        {
            m_param->callback(RECV_SOCKET, remote_endpoint.address().to_string().c_str(),
                              remote_endpoint.port(), 0, 0);
        }
    }
	else
	{
		if (m_param->callback)
		{
			m_param->callback(RECV_ERROR, remote_endpoint.address().to_string().c_str(),
				remote_endpoint.port(), error.message().length() + 1, error.message().c_str());
		}
	}

    start_accept();
}

int CTcpServer::getProcIndex(const char* ip, int port)
{
	if (!ip) return -1;
	for (int i = 0; i < m_vecProcs.size(); i++)
	{
		tcp::endpoint ep = m_vecProcs[i]->getSocket().remote_endpoint();
		if ((ep.address().to_string().compare(ip) == 0) && (ep.port() == port))
		{
			return i;
		}
	}
	return -1;
}

void CTcpServer::socket_closed(const char * ip, int port)
{
	int nIndex = getProcIndex(ip, port);
	if (nIndex != -1)
	{
		m_vecProcs.erase(m_vecProcs.begin() + nIndex);
	}
}
