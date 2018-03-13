// libstd_test.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "../inc/socket.h"
#include "../inc/serialport.h"

#include <process.h>

using namespace std;

void TcpClientTest();
void SerialPortTest();


int main()
{
	TcpClientTest();
	//SerialPortTest();
	system("pause");
    return 0;
}

void TcpClientTest()
{
	cout << "===== Start tcp client test =====" << endl;
	int nRet = InitSocket(0, TCP_CLIENT, "libstd_tcp.ini");
	cout << "InitSocket..." << ((nRet == SOCK_SUCCESS) ? "Pass" : "Failed") << endl;

	nRet = TCPConnect(0, 2000);
	cout << "TCPConnect..." << ((nRet == SOCK_SUCCESS) ? "Pass" : "Failed") << endl;

	nRet = TCPSend(0, "Tcp send test!", strlen("Tcp send test!") + 1);
	cout << "TCPSend..." << ((nRet == strlen("Tcp send test!") + 1) ? "Pass" : "Failed") << endl;

	char szRecv[512] = { 0 };
	nRet = TCPRecv(0, szRecv, 512, 5000);
	cout << "TCPRecv..." << ((nRet == strlen(szRecv)) ? "Pass" : "Failed") << endl;

	UninitSocket(0);
}

void THREAD_BODY(void *pParam) 
{
	int nRet;
	while (true)
	{
		char szBuf[1024] = { 0 };
		//std::cout << "Enter with thread" << std::endl;
		nRet = ReadCOM(0, szBuf, 1024, 300);
		if (nRet > 0)
		{
			std::cout << "Read data: " << szBuf << std::endl;
		}
	}
}

void SerialPortTest()
{
	cout << "===== Start serial port client test =====" << endl;
	int nRet = InitCOM(0, "libstd_serialport.ini");
	cout << "InitCOM..." << ((nRet == SOCK_SUCCESS) ? "Pass" : "Failed") << endl;

	nRet = OpenCOM(0);
	cout << "OpenCOM..." << ((nRet == SOCK_SUCCESS) ? "Pass" : "Failed") << endl;


	//while (true)
	//{
	//	char szBuf[1024] = { 0 };
	//	WriteCOM(0, "xyz123", 7);
	//	nRet = ReadCOM(0, szBuf, 1024, 300);
	//	if (nRet > 0)
	//	{
	//		std::cout << "Read data: " << szBuf << std::endl;
	//	}
	//}
}