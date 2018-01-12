#include "stdafx.h"
#include "ctcpclient.h"
#include <sstream>
#include "iniconfig.h"

CTcpClient::CTcpClient()
	: m_socket(m_io_service)
	, m_timer(m_io_service)
{
}

CTcpClient::~CTcpClient()
{
}


int CTcpClient::Connect(int nTimeoutMs)
{
	std::stringstream section;
	section << "TcpClient" << m_param->nId;
	std::string szIp = ReadIniStdString(section.str().c_str(), "Address", "0.0.0.0", m_param->szIniPath.c_str());
	std::string szPort = ReadIniStdString(section.str().c_str(), "Port", "10000", m_param->szIniPath.c_str());

	tcp::resolver resolver(m_io_service);
	tcp::resolver::query query(szIp, szPort);
	tcp::resolver::iterator iterator = resolver.resolve(query);

	boost::asio::async_connect(m_socket, iterator,
		boost::bind(&CTcpClient::handle_connect, this,
			boost::asio::placeholders::error));

	m_timer.expires_from_now(boost::posix_time::mili)
	return 0;
}

void CTcpClient::handle_connect(const boost::system::error_code & error)
{
}
