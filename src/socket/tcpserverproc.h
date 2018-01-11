#ifndef TCPSERVERPROC_H
#define TCPSERVERPROC_H

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "csocket.h"

using boost::asio::ip::tcp;

class TcpServerProc
    : public boost::enable_shared_from_this<TcpServerProc>
{
public:
    TcpServerProc(boost::asio::io_service &io_service, socket_param_ptr& param);
    tcp::socket& getSocket() { return m_socket; }
    void start();

private:
    tcp::socket m_socket;
    socket_param_ptr m_param;

    void handle_read_header(const boost::system::error_code& error);
};

typedef boost::shared_ptr<TcpServerProc> tcpserver_proc_ptr;

#endif // TCPSERVERPROC_H
