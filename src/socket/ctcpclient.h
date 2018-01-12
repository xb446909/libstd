#pragma once
#include "csocket.h"

using boost::asio::ip::tcp;

class CTcpClient :
	public CSocket
{
public:
	CTcpClient();
	virtual ~CTcpClient();
	virtual int Connect(int nTimeoutMs);

private:
	void handle_connect(const boost::system::error_code& error);

	tcp::socket m_socket;
	boost::asio::deadline_timer m_timer;
};

