#ifndef TCPSERVERPROC_H
#define TCPSERVERPROC_H

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "csocket.h"
#include <boost/function.hpp>

using boost::asio::ip::tcp;

class TcpServerProc
    : public boost::enable_shared_from_this<TcpServerProc>
{
public:
    enum { readBufSize = 10240 };
	boost::function<void(const char*, int)> socket_closed;

    TcpServerProc(boost::asio::io_service &io_service, socket_param_ptr& param);
    tcp::socket& getSocket() { return m_socket; }
    void start();
	int Send(const char * szSendBuf, int nlen);
	int Recv(char * szRecvBuf, int nBufLen, int nTimeoutMs);

private:
	void handle_read_header(const boost::system::error_code &error, std::size_t bytes_transferred);

    tcp::socket m_socket;
    socket_param_ptr m_param;
    uint8_t m_readBuf[readBufSize];
};

typedef boost::shared_ptr<TcpServerProc> tcpserver_proc_ptr;

#endif // TCPSERVERPROC_H
