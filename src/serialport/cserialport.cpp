#include "stdafx.h"
#include "cserialport.h"
#include "iniconfig.h"
#include <sstream>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

CSerialPort::CSerialPort(int nId, const std::string & szIniPath, SerialPortRecvCallback pCallback)
	: m_nId(nId)
	, m_szIniPath(szIniPath)
	, m_pCallback(pCallback)
	, m_serialPort(m_io_service)
	, m_timer(m_io_service)
	, m_nReadRet(0)
{
}

CSerialPort::~CSerialPort()
{
	m_io_service.stop();
	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
}

int CSerialPort::Open()
{
	std::stringstream ssSection;
	ssSection << "SerialPort" << m_nId;

	m_szPort = ReadIniStdString(ssSection.str().c_str(), "PortName", "COM1", m_szIniPath.c_str());
	int nBaudRate = ReadIniInt(ssSection.str().c_str(), "BaudRate", 115200, m_szIniPath.c_str());
	int nDataBits = ReadIniInt(ssSection.str().c_str(), "DataBits", 8, m_szIniPath.c_str());
	int nStopBits = ReadIniInt(ssSection.str().c_str(), "StopBits", 1, m_szIniPath.c_str());
	int nParity = ReadIniInt(ssSection.str().c_str(), "Parity", 1, m_szIniPath.c_str());
	int nFlowControl = ReadIniInt(ssSection.str().c_str(), "FlowControl", 0, m_szIniPath.c_str());

	boost::system::error_code ec;

	try
	{
		m_serialPort.open(m_szPort);
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
			std::cerr << "Set " << m_szPort << " set data bits error" << std::endl;
			m_serialPort.close(ec);
			return COM_ERROR;
			break;
		}
		m_serialPort.set_option(boost::asio::serial_port::parity((boost::asio::serial_port::parity::type)nParity));
		m_serialPort.set_option(boost::asio::serial_port::flow_control((boost::asio::serial_port::flow_control::type)nFlowControl));
	}
	catch (const boost::system::system_error& error)
	{
		std::cerr << "Open " << m_szPort << " error: " << error.what() << std::endl;
		m_serialPort.close(ec);
		return COM_ERROR;
	}

#ifdef _DEBUG
	std::cout << "Open " << m_szPort << " successfully!" << std::endl;
#endif // _DEBUG

	if (m_pCallback)
	{
		memset(m_readBuf, 0, readBufSize);
		m_serialPort.async_read_some(boost::asio::buffer(m_readBuf, readBufSize),
			boost::bind(&CSerialPort::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		m_io_service.reset();
		boost::thread thrd(boost::bind(&boost::asio::io_service::run, &m_io_service));
	}

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
		std::cerr << "Write " << m_szPort << " error: " << ec.what() << std::endl;
	}

	return transferredbytes;
}

int CSerialPort::Read(char * szBuf, int nBufLen, int nTimeoutMs)
{
	if (m_pCallback)
	{
		return COM_ERROR;
	}
	boost::mutex::scoped_lock lock(m_io_mutex);

	m_nReadRet = 0;
	m_serialPort.async_read_some(boost::asio::buffer(m_readBuf, readBufSize),
		boost::bind(&CSerialPort::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

	m_timer.expires_from_now(boost::posix_time::milliseconds(nTimeoutMs));
	m_timer.async_wait(boost::bind(&CSerialPort::handle_timer, this, boost::asio::placeholders::error));

	m_io_service.reset();
	boost::thread thrd(boost::bind(&boost::asio::io_service::run, &m_io_service));
	m_condition.wait(m_io_mutex);
	if (m_nReadRet > 0)
	{
		memcpy(szBuf, m_readBuf, min(nBufLen, m_nReadRet));
		m_serialPort.cancel();
	}
	return m_nReadRet;
}

void CSerialPort::handle_timer(const boost::system::error_code & error)
{
	if (!error)
	{
		std::cout << "Timeout" << std::endl;
	}
	else if (error.value() != boost::asio::error::operation_aborted)
	{
		std::cerr << "Timer error: " << error.message() << std::endl;
	}
	m_condition.notify_one();
}

void CSerialPort::read_handler(
	const boost::system::error_code& error, // Result of operation.
	std::size_t bytes_transferred           // Number of bytes read.
)
{
	if (error)
	{
		std::cerr << "Read error: " << error.message() << std::endl;
		if (m_pCallback)
		{
			m_pCallback(m_szPort.c_str(), COM_ERROR, 0);
		}
		else
		{
			m_nReadRet = COM_ERROR;
			m_timer.cancel();
			m_condition.notify_one();
		}
		return;
	}
	if (!m_pCallback)
	{
		m_nReadRet = bytes_transferred;
		m_timer.cancel();
		m_condition.notify_one();
	}
	else
	{
		m_pCallback(m_szPort.c_str(), bytes_transferred, (const char*)m_readBuf);
		memset(m_readBuf, 0, readBufSize);
		m_serialPort.async_read_some(boost::asio::buffer(m_readBuf, readBufSize),
			boost::bind(&CSerialPort::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}