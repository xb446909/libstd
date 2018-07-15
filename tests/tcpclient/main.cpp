#include <iostream>
#include "socket.h"

int main(int argc, char* argv[])
{
	InitSocket(0, TCP_CLIENT);
	TCPConnect(0, 1000);
	TCPSend(0, "Hello", strlen("Hello") + 1);
	UninitSocket(0);
	return 0;
}
