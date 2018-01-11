#ifndef CTCPSERVER_H
#define CTCPSERVER_H

#include "csocket.h"
#include "tcpserverproc.h"

using boost::asio::ip::tcp;

class CTcpServer : public CSocket
{
public:
    CTcpServer();
    void SetParam(socket_param_ptr& param);

private:
    void start_accept();
    void handle_accept(tcpserver_proc_ptr proc_ptr,
          const boost::system::error_code& error);

    tcp::acceptor m_acceptor;
    std::vector<tcpserver_proc_ptr> m_vecProcs;
};

#endif // CTCPSERVER_H
