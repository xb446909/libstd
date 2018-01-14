#ifndef __CSERIALPORT_H
#define __CSERIALPORT_H

#include "serialport.h"
#include <string>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>

class CSerialPort
{
public:
	CSerialPort(int nId, const std::string& szIniPath, SerialPortRecvCallback pCallback);
	int Open();
	int Write(const char* szBuf, int nLen);
	int Read(char* szBuf, int nBufLen, int nTimeoutMs);
private:
	int m_nId;
	std::string m_szIniPath;
	SerialPortRecvCallback m_pCallback;

	boost::asio::io_service m_io_service;
	boost::asio::serial_port m_serialPort;
};

typedef boost::shared_ptr<CSerialPort> serialport_ptr;

#endif // !__CSERIALPORT_H
