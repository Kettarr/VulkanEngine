#pragma once


#include <inttypes.h>
#include <queue>

extern "C" {
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <in6addr.h>
#include <ws2ipdef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <WS2tcpip.h>
}

typedef struct Report {
	double recBytes;
	double recPackets;
	double framerate;

} Report;


typedef struct Inputs {
	int keyA = 0;
	int keyB = 0;
	int x = 0;
	int y = 0;
	bool mouse = false;

};
class UDPReceiver
{


public:
	int sock;
	struct sockaddr_in6 addr;
	char* recbuffer;

	UDPReceiver();
	~UDPReceiver() { delete recbuffer; };

	int startWinsock(void)
	{
		WSADATA wsa;
		auto err = WSAStartup(MAKEWORD(2, 2), &wsa);
		return err;
	}
	void init(int port);
	Report receive();
	Inputs receiveInputs();
	void closeSock();
};

