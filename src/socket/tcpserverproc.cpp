#include "stdafx.h"
#include "tcpserverproc.h"
#include <iostream>

TcpServerProc::TcpServerProc(boost::asio::io_service& io_service, socket_param_ptr& param)
    : m_socket(io_service)
    , m_param(param)
{

}

void TcpServerProc::start()
{
    m_remote_endpoint = m_socket.remote_endpoint();
    memset(m_readBuf, 0, readBufSize);
    m_socket.async_read_some(boost::asio::buffer(m_readBuf, readBufSize),
                             boost::bind(
                               &TcpServerProc::handle_read_header, shared_from_this(),
                               boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

int TcpServerProc::Send(const char * szSendBuf, int nlen)
{
	int transferredbytes = 0;
	try
	{
		transferredbytes = boost::asio::write(m_socket, boost::asio::buffer(szSendBuf, nlen));
	}
	catch (boost::system::system_error ec)
	{
		tcp::endpoint remote_endpoint = m_socket.remote_endpoint();
		std::cerr << "Write error: " << ec.what() << std::endl;
        socket_closed(remote_endpoint.address().to_string().c_str(), remote_endpoint.port());
		if (ec.code() == boost::asio::error::eof)
		{
			return SOCK_CLOSED;
		}
	}
	
	return transferredbytes;
}

int TcpServerProc::Recv(char * szRecvBuf, int nBufLen, int nTimeoutMs)
{
	return 0;
}

void TcpServerProc::Close()
{
	m_socket.shutdown(tcp::socket::shutdown_both);
	m_socket.close();
}

void TcpServerProc::handle_read_header(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error)
    {
#ifdef _DEBUG
        std::cout << "Receviced data from " << m_remote_endpoint.address().to_string()
                  << ":" << m_remote_endpoint.port() << " : " << m_readBuf << std::endl;
#endif
        if (m_param->callback)
        {
            m_param->callback(RECV_DATA, m_remote_endpoint.address().to_string().c_str(),
                              m_remote_endpoint.port(), bytes_transferred, (const char*)m_readBuf);
        }
        memset(m_readBuf, 0, readBufSize);
        m_socket.async_read_some(boost::asio::buffer(m_readBuf, readBufSize),
                                 boost::bind(
                                   &TcpServerProc::handle_read_header, shared_from_this(),
                                   boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else
    {
#ifdef _DEBUG
        std::cerr << "Socket error from " << m_remote_endpoint.address().to_string()
                  << ":" << m_remote_endpoint.port() << " : " << error.message() << std::endl;
#endif
		if (m_param->callback)
		{
			if (error.value() == boost::asio::error::eof)
			{
                m_param->callback(RECV_CLOSE, m_remote_endpoint.address().to_string().c_str(),
                    m_remote_endpoint.port(), error.message().length() + 1, error.message().c_str());
			}
			else
			{
                m_param->callback(RECV_ERROR, m_remote_endpoint.address().to_string().c_str(),
                    m_remote_endpoint.port(), error.message().length() + 1, error.message().c_str());
			}
            socket_closed(m_remote_endpoint.address().to_string().c_str(), m_remote_endpoint.port());
        }
    }
}
