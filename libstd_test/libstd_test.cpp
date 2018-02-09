// libstd_test.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "../inc/socket.h"

using namespace std;

void TcpClientTest();

int main()
{
	TcpClientTest();
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