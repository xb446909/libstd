#include "stdafx.h"
#include "cserialport.h"
#include "iniconfig.h"
#include <sstream>
#include <iostream>

CSerialPort::CSerialPort(int nId, const std::string & szIniPath, SerialPortRecvCallback pCallback)
	: m_nId(nId)
	, m_szIniPath(szIniPath)
	, m_pCallback(pCallback)
	, m_serialPort(m_io_service)
{
}

int CSerialPort::Open()
{
	std::stringstream ssSection;
	ssSection << "SerialPort" << m_nId;

	std::string name = ReadIniStdString(ssSection.str().c_str(), "Name", "COM1", m_szIniPath.c_str());
	int nBaudRate = ReadIniInt(ssSection.str().c_str(), "BaudRate", 115200, m_szIniPath.c_str());
	int nDataBits = ReadIniInt(ssSection.str().c_str(), "DataBits", 8, m_szIniPath.c_str());
	int nStopBits = ReadIniInt(ssSection.str().c_str(), "StopBits", 1, m_szIniPath.c_str());
	int nParity = ReadIniInt(ssSection.str().c_str(), "Parity", 1, m_szIniPath.c_str());
	int nFlowControl = ReadIniInt(ssSection.str().c_str(), "FlowControl", 0, m_szIniPath.c_str());

	boost::system::error_code ec;

	try
	{
		m_serialPort.open(name);
		m_serialPort.set_option(boost::asio::serial_port::baud_rate(nBaudRate));
		m_serialPort.set_option(boost::asio::serial_port::character_size(nDataBits));
		switch (nStopBits)
		{
		case 1:
			m_serialPort.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
			break;
		case 2:
			m_serialPort.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::two));
			break;
		default:
			std::cerr << "Set " << name << " set data bits error" << std::endl;
			m_serialPort.close(ec);
			return COM_ERROR;
			break;
		}
		m_serialPort.set_option(boost::asio::serial_port::parity((boost::asio::serial_port::parity::type)nParity));
		m_serialPort.set_option(boost::asio::serial_port::flow_control((boost::asio::serial_port::flow_control::type)nFlowControl));
	}
	catch (const boost::system::system_error& error)
	{
		std::cerr << "Open " << name << " error: " << error.what() << std::endl;
		m_serialPort.close(ec);
		return COM_ERROR;
	}

#ifdef _DEBUG
	std::cout << "Open " << name << " successfully!" << std::endl;
#endif // _DEBUG

	return COM_SUCCESS;
}

int CSerialPort::Write(const char * szBuf, int nLen)
{
	int transferredbytes = 0;
	try
	{
		transferredbytes = boost::asio::write(m_serialPort, boost::asio::buffer(szBuf, nLen));
	}
	catch (boost::system::system_error ec)
	{
		std::cerr << "Write COM: " << ec.what() << std::endl;
	}

	return transferredbytes;
}

int CSerialPort::Read(char * szBuf, int nBufLen, int nTimeoutMs)
{
	return 0;
}
