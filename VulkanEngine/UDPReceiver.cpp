#pragma comment(lib, "ws2_32.lib")


#include "UDPReceiver.h"

#include <iostream>
#include <string.h>
#include <inttypes.h>

extern "C" {
#include <stdio.h>
#include <time.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}
#define INBUF_SIZE 4096


typedef struct RTHeader {
	double		  time;
	unsigned long packetnum;
} RTHeader_t;





UDPReceiver::UDPReceiver() {
	recbuffer = new char[1000];
}


void UDPReceiver::init(int port) {
	sock = socket(AF_INET6, SOCK_DGRAM, 0);

	memset(&addr, 0, sizeof(addr));

	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(port);
	addr.sin6_addr = in6addr_any;
	auto ret = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	//printf("Binding port %d return %d errno %d\n", port, ret, WSAGetLastError());
}


Report UDPReceiver::receive() {
	struct sockaddr_in6 si_other;
	socklen_t slen = sizeof(si_other);

	//RTHeader_t* pheader = (RTHeader_t*)recbuffer;

	Report* report = (Report*)recbuffer;

	//while (true) {
		auto ret = recvfrom(sock, recbuffer, 65000, 0, (sockaddr*)&si_other, &slen);
		//std::cout << "Ret size: " << ret << std::endl;
		/*if (ret > sizeof RTHeader_t) {
			memcpy(buffer, recbuffer + sizeof RTHeader_t, ret - sizeof RTHeader_t);
			return ret - sizeof RTHeader_t;
		}
		*/
	
	return *report;
}


Inputs UDPReceiver::receiveInputs() {
	struct sockaddr_in6 si_other;
	socklen_t slen = sizeof(si_other);

	//RTHeader_t* pheader = (RTHeader_t*)recbuffer;

	Inputs* report = (Inputs*)recbuffer;

	//while (true) {
	auto ret = recvfrom(sock, recbuffer, 1000, 0, (sockaddr*)&si_other, &slen);
	std::cout << "Ret size: " << ret << std::endl;
	/*if (ret > sizeof RTHeader_t) {
		memcpy(buffer, recbuffer + sizeof RTHeader_t, ret - sizeof RTHeader_t);
		return ret - sizeof RTHeader_t;
	}
	*/

	return *report;
}



void UDPReceiver::closeSock() {
	closesocket(sock);
	sock = 0;
}
