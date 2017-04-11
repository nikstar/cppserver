#include "net.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace net {

explicit Conn::Conn(int sd) : sd(sd) {};

std::string Conn::ReadString() {
	// http://stackoverflow.com/questions/18670807/sending-and-receiving-stdstring-over-socket
	constexpr unsigned int MAX_BUF_LENGTH = 4096;
	std::string rcv;
	do {
		std::vector<char> buffer(MAX_BUF_LENGTH);
		int bytesReceived = 0;

		bytesReceived = read(sd, buffer.data(), buffer.length(), 0));
		// append string from buffer.
		if (bytesReceived == -1) {
			throw ConnectionError();
		}
		else {
			rcv.append(buffer.cbegin(), buffer.cend());
		}
	} while (bytesReceived == MAX_BUF_LENGTH);
	return rcv;
}


void Conn::WriteString(std::string str) {
	auto err = write(sd, str.c_str, str.size());
	if (err != 0) {
		throw ConnectionError();
	}
}

void Conn::Close() {
	close(sd);
}


Conn Dial(const std::string host, const int port) throws(ConnectionError) {
	sd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr addr;
	auto err = connect(sd, &addr, 100);
	if (err = -1) {
		throw ConnectionError();
	}
	return Conn(0);
}


std::string ConnectionError::Report() {
	return "Connection error";
}


explicit Listner(int sd) : sd(sd) {};

Conn Listner::Accept() {
	auto sd = accept(sd, 0, 0);
	return Conn(sd);
}

Listner Listen(const int port) {
	sd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr addr;
	bind(sd, &addr, 100);
	listen(sd, 5);
	return Listner(sd);
}


}
