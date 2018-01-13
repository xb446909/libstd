#include "stdafx.h"
#include "ctcpclient.h"
#include <sstream>
#include "iniconfig.h"
#include <iostream>
#include <boost/thread.hpp>

CTcpClient::CTcpClient()
	: m_socket(m_io_service)
	, m_timer(m_io_service)
{
}

int CTcpClient::Connect(int nTimeoutMs)
{
	std::stringstream section;
	section << "TcpClient" << m_param->nId;
	std::string szIp = ReadIniStdString(section.str().c_str(), "Address", "127.0.0.1", m_param->szIniPath.c_str());
	std::string szPort = ReadIniStdString(section.str().c_str(), "Port", "10000", m_param->szIniPath.c_str());

	boost::mutex::scoped_lock lock(m_io_mutex);

	tcp::resolver resolver(m_io_service);
	tcp::resolver::query query(szIp, szPort);
	tcp::resolver::iterator iterator = resolver.resolve(query);

	boost::asio::async_connect(m_socket, iterator,
		boost::bind(&CTcpClient::handle_connect, this,
			boost::asio::placeholders::error));

	m_timer.expires_from_now(boost::posix_time::milliseconds(nTimeoutMs));
	m_timer.async_wait(boost::bind(&CTcpClient::handle_timer, this, boost::asio::placeholders::error));
	m_io_service.reset();
	boost::thread thrd(boost::bind(&boost::asio::io_service::run, &m_io_service));

	m_condition.wait(m_io_mutex);
	return m_socket.is_open() ? SOCK_SUCCESS : SOCK_ERROR;
}

int CTcpClient::Send(const char * szSendBuf, int nlen, const char * szDstIP, int nDstPort)
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
		if (ec.code() == boost::asio::error::eof)
		{
			Close();
			return SOCK_CLOSED;
		}
	}

	return transferredbytes;
}

int CTcpClient::Recv(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort)
{
	boost::mutex::scoped_lock lock(m_io_mutex);

	m_nReadBytes = 0;
	m_socket.async_read_some(boost::asio::buffer(szRecvBuf, nBufLen),
		boost::bind(&CTcpClient::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

	m_timer.expires_from_now(boost::posix_time::milliseconds(nTimeoutMs));
	m_timer.async_wait(boost::bind(&CTcpClient::handle_timer, this, boost::asio::placeholders::error));

	m_io_service.reset();
	boost::thread thrd(boost::bind(&boost::asio::io_service::run, &m_io_service));

	m_condition.wait(m_io_mutex);
	m_socket.cancel();
	return m_nReadBytes;
}

void CTcpClient::Close()
{
	m_socket.shutdown(tcp::socket::shutdown_both);
	m_socket.close();
}

void CTcpClient::handle_timer(const boost::system::error_code & error)
{
	if (!error)
	{
		std::cout << "Timeout" << std::endl;
		m_condition.notify_one();
	}
}

void CTcpClient::handle_connect(const boost::system::error_code & error)
{
	if (error)
	{
		std::cerr << "Error: " << error.message() << std::endl;
		m_socket.close();
	}
	m_timer.cancel();
	m_condition.notify_one();
}

void CTcpClient::handle_read(const boost::system::error_code & error, std::size_t bytes_transferred)
{
	if (error)
	{
		std::cerr << "Read error: " << error.message() << std::endl;
	}
	m_nReadBytes = bytes_transferred;
	m_timer.cancel();
	m_condition.notify_one();
}
