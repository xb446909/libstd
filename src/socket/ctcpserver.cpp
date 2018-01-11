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

	start();
}

void CTcpServer::start_accept()
{
    tcpserver_proc_ptr proc_ptr = boost::make_shared<TcpServerProc>(m_io_service, m_param);
    m_acceptor->async_accept(proc_ptr->getSocket(),
                            boost::bind(&CTcpServer::handle_accept, this, proc_ptr,
                                        boost::asio::placeholders::error));
    m_vecProcs.push_back(proc_ptr);
}

void CTcpServer::handle_accept(tcpserver_proc_ptr proc_ptr,
      const boost::system::error_code& error)
{
	tcp::endpoint remote_endpoint = proc_ptr->getSocket().remote_endpoint();
    if (!error)
    {
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

void CTcpServer::run()
{
	m_io_service.run();
}

void CTcpServer::start()
{
	boost::function0<void> f = boost::bind(&CTcpServer::run, this);
	boost::thread thrd(f);
}
