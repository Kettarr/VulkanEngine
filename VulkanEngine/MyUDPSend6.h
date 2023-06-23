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
#include <inttypes.h>



class MyUDPSend6 {

public:
	int sock = 0;
	struct sockaddr_in6 addr;
	unsigned int packetnum = 0;

	MyUDPSend6() {};
	~MyUDPSend6() {};
	void init(const char* address, int port);
	int send(uint8_t* buffer, int size);
	void closeSock();
	void start();
	void close();
};



