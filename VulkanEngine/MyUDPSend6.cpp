/*
 *  UDPSend6.cpp
 *
 *  Created by Helmut Hlavacs (2022).
 *
 */

#pragma comment(lib, "ws2_32.lib")


#include "MyUDPSend6.h"
#include <iostream>

extern "C" {
#include <stdio.h>
#include <time.h>
}


typedef struct RTHeader {
	double		  time;
	unsigned long packetnum;
} RTHeader_t;


int startWinsock(void)
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2, 0), &wsa);
}


void MyUDPSend6::init(const char* address, int port) {
	sock = socket(AF_INET6, SOCK_DGRAM, 0);

	struct addrinfo hints;

	memset(&addr, 0, sizeof(addr));
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;

	struct addrinfo* result = NULL;
	auto dwRetval = getaddrinfo(address, nullptr, &hints, &result);
	if (dwRetval != 0) {
		printf("getaddrinfo failed with error: %d\n", dwRetval);
		return;
	}
	for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		if (ptr->ai_family == AF_INET6) {
			memcpy(&addr, ptr->ai_addr, ptr->ai_addrlen);
			addr.sin6_port = htons(port);
			addr.sin6_family = AF_INET6;
		}
	}
	freeaddrinfo(result);
}



int MyUDPSend6::send(uint8_t* buffer, int size) {
	char sendbuffer[65000];

	packetnum++;
	
	if (size > 65000) {
		return 0;
	}

	RTHeader_t header;
	header.time = clock() / (double)CLOCKS_PER_SEC;
	header.packetnum = packetnum;

	int ret;
	memcpy(sendbuffer, &header, sizeof(header));
	memcpy(sendbuffer + sizeof(header), buffer, size);

	ret = sendto(sock, sendbuffer, sizeof(header) + size, 0, (const struct sockaddr*)&addr, sizeof(addr));
	return ret;
}


void MyUDPSend6::closeSock() {
	closesocket(sock);
	sock = 0;
}



void MyUDPSend6::start() {

	startWinsock();

	init("::1", 50000);

	

	/*for (int i = 0; i < size; i++) {
		std::cout << data[i];
	}*/


	
}




void MyUDPSend6::close() {
	std::cout << "close" << std::endl;
	WSACleanup();
	closeSock();
}