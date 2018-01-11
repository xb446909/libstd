#include "tcpserverproc.h"

TcpServerProc::TcpServerProc(boost::asio::io_service& io_service, socket_param_ptr& param)
    : m_socket(io_service)
    , m_param(param)
{

}

void TcpServerProc::start()
{
    boost::asio::async_read(m_socket,
            boost::asio::buffer(read_msg_.data(), chat_message::header_length),
            boost::bind(
              &TcpServerProc::handle_read_header, shared_from_this(),
              boost::asio::placeholders::error));
}

void TcpServerProc::handle_read_header(const boost::system::error_code &error)
{

}
