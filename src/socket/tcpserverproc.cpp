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
    memset(m_readBuf, 0, readBufSize);
    m_socket.async_read_some(boost::asio::buffer(m_readBuf, readBufSize),
                             boost::bind(
                               &TcpServerProc::handle_read_header, shared_from_this(),
                               boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void TcpServerProc::handle_read_header(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    tcp::endpoint remote_endpoint = m_socket.remote_endpoint();
    if (!error)
    {
#ifdef _DEBUG
        std::cout << "Receviced data from " << remote_endpoint.address().to_string()
                  << ":" << remote_endpoint.port() << " : " << m_readBuf << std::endl;
#endif
        if (m_param->callback)
        {
            m_param->callback(RECV_DATA, remote_endpoint.address().to_string().c_str(),
                              remote_endpoint.port(), bytes_transferred, (const char*)m_readBuf);
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
        std::cerr << "Socket error from " << remote_endpoint.address().to_string()
                  << ":" << remote_endpoint.port() << " : " << error.message() << std::endl;
#endif
		if (m_param->callback)
		{
			if (error.value() == 2)
			{
				m_param->callback(RECV_CLOSE, remote_endpoint.address().to_string().c_str(),
					remote_endpoint.port(), error.message().length() + 1, error.message().c_str());
			}
			else
			{
				m_param->callback(RECV_ERROR, remote_endpoint.address().to_string().c_str(),
					remote_endpoint.port(), error.message().length() + 1, error.message().c_str());
			}
        }
    }
}
