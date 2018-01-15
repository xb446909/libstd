#ifndef __CSERIALPORT_H
#define __CSERIALPORT_H

#include "serialport.h"
#include <string>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>  
#include <boost/thread/condition.hpp>

class CSerialPort
{
public:
	enum { readBufSize = 10240 };

	CSerialPort(int nId, const std::string& szIniPath, SerialPortRecvCallback pCallback);
	int Open();
	int Write(const char* szBuf, int nLen);
	int Read(char* szBuf, int nBufLen, int nTimeoutMs);
private:
	void read_handler(
		const boost::system::error_code& error, // Result of operation.
		std::size_t bytes_transferred           // Number of bytes read.
	);
	void handle_timer(const boost::system::error_code & error);

	int m_nId;
	std::string m_szIniPath;
	SerialPortRecvCallback m_pCallback;
	std::string m_szPort;

	uint8_t m_readBuf[readBufSize];

	boost::asio::io_service m_io_service;
	boost::asio::serial_port m_serialPort;
	boost::mutex m_io_mutex;
	boost::asio::deadline_timer m_timer;
	boost::condition_variable_any m_condition;
	int m_nReadRet;
};

typedef boost::shared_ptr<CSerialPort> serialport_ptr;

#endif // !__CSERIALPORT_H
