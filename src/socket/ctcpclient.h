#ifndef __CTCPCLIENT_H
#define __CTCPCLIENT_H

#include "csocket.h"
#include <boost/thread/mutex.hpp>  
#include <boost/thread/condition.hpp>

using boost::asio::ip::tcp;

class CTcpClient :
	public CSocket
{
public:
	enum { readBufSize = 10240 };
	CTcpClient();
	virtual int Connect(int nTimeoutMs);
	virtual int Send(const char * szSendBuf, int nlen, const char * szDstIP, int nDstPort);
	virtual int Recv(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort);
	virtual void Close();
private:
	void handle_timer(const boost::system::error_code& error);
	void handle_connect(const boost::system::error_code& error);
	void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred);

	tcp::socket m_socket;
	boost::asio::deadline_timer m_timer;
	boost::mutex m_io_mutex;
	boost::condition_variable_any m_condition;
	uint8_t m_readBuf[readBufSize];
	int m_nReadBytes;
};
#endif // __CTCPCLIENT_H
