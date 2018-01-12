#ifndef CTCPSERVER_H
#define CTCPSERVER_H

#include "csocket.h"
#include "tcpserverproc.h"

using boost::asio::ip::tcp;

class CTcpServer : public CSocket
{
public:
    CTcpServer();
	virtual ~CTcpServer();
	virtual void SetParam(socket_param_ptr& param);
	virtual int Send(const char * szSendBuf, int nlen, const char * szDstIP, int nDstPort);
	virtual int Recv(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort);
private:
    void start_accept();
    void handle_accept(tcpserver_proc_ptr proc_ptr,
          const boost::system::error_code& error);
	int getProcIndex(const char* ip, int port);
	void socket_closed(const char* ip, int port);

    boost::shared_ptr<tcp::acceptor> m_acceptor;
    std::vector<tcpserver_proc_ptr> m_vecProcs;
};

#endif // CTCPSERVER_H
