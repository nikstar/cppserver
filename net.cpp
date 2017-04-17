#include "net.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <vector>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cstring>

#include <string>
#include <sstream>
#include <fstream>

namespace net {


Conn::Conn(int sd) : sd(sd) {};

std::string Conn::ReadString() {
	// http://stackoverflow.com/questions/18670807/sending-and-receiving-stdstring-over-socket
	constexpr unsigned int MAX_BUF_LENGTH = 4096;
	std::string rcv;
	int bytesReceived = 0;
	do {
		std::vector<char> buffer(MAX_BUF_LENGTH);
		
		bytesReceived = read(sd, buffer.data(), buffer.size());
		// append string from buffer.
		if (bytesReceived == -1) {
			throw net::Error("failed to read from socket");
		}
		rcv.append(buffer.cbegin(), buffer.cend());
	} while (bytesReceived == MAX_BUF_LENGTH);
	return rcv;
}


void Conn::WriteString(std::string str) {
	const auto err = write(sd, str.c_str(), str.size());
	if (err == -1) {
		throw Error("failed to write to socket");
	}
}

void Conn::Close() {
	close(sd);
}


Conn Dial(const std::string host, const int port) {
	const auto sd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr addr;
	const auto err = connect(sd, &addr, 100);
	if (err == -1) {
		throw Error("failed to connect");
	}
	return Conn(sd);
}


Error::Error(std::string descr) {
	msg = descr + ": " + std::strerror(errno);
}

std::string Error::Report() {
	return msg;
}


Listner::Listner(int sd) : sd(sd) {}

Conn Listner::Accept() {
	const auto sd = accept(this->sd, 0, 0);
	if (sd == -1) {
		throw Error("failed to accept");
	}
	return Conn(sd);
}

Listner Listen(const int port) {
	const auto sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		throw Error("failed to create socket");
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port=htons(port);
	auto err = bind(sd, (const struct sockaddr *)&addr, sizeof(addr));
	if (err == -1) {
		throw Error("failed to bind port");
	}
	err = listen(sd, 5);
	if (err == -1) {
		throw Error("failed to listen to port");
	}
	return Listner(sd);
}


} // namespace net


namespace http {


Request::Request(std::string method, std::string endpoint, std::map<std::string, std::string> headers, std::string body)
	: method(method), endpoint(endpoint), headers(headers), body(body) {}



Request::Request(std::string str) {
	std::istringstream ss(str);
    
	std::getline(ss, method, ' ');
	std::getline(ss, endpoint, ' ');
	
	std::string tmp;
	char c;
	std::getline(ss, tmp, '\n');

	for (;;) {
		std::string entry;
		std::getline(ss, entry, '\r');
		ss.ignore(1);

		if (entry.size() == 0) {
			break;
		}

		std::istringstream ls(entry);
		std::string key, value;

		std::getline(ls, key, ':');
		ls.ignore(1);
		std::getline(ls, value, '\r');
		ls.ignore(1);
		
		headers[key] = value; 
	}

	std::getline(ss, tmp, '\n');

	std::getline(ss, body);
}

std::ostream &operator<<(std::ostream &o, Request const &req) {
	o << "Request: " << req.method << " -> " << req.endpoint << std::endl;
	o << "[Headers]\n";
	for (const auto & pair : req.headers) {
		o << pair.first << ": " << pair.second << std::endl;
	}
	o << "[Body]\n" << req.body << std::endl;
	return o;
}


Response::Response(int status, std::map<std::string, std::string> headers, std::string body)
	: status(status), headers(headers), body(body) {}

std::string Response::str() {
	std::ostringstream o;
    o << "HTTP/1.1 ";
	o << status << ' ' << "OK" << "\r\n"; // todo: implement reason phrases
	for (const auto & pair : headers) {
		o << pair.first << ": " << pair.second << std::endl;
	}
	o << "\r\n";
	o << body;
    return  o.str();
}


Server::Server(int port) : port(port) {}

void Server::ListenAndServe() {
	l = net::Listen(port);
	
	for (;;) {
		auto conn = l.Accept();
		if (!fork()) {
			auto str = conn.ReadString();
			std::cout << "Recieved from client:\n" << str << std::endl;
			auto req = Request(str);
			auto resp = Handle(req);
			conn.WriteString(resp.str());
			conn.Close();
		}
		conn.Close();
	}
}

Response Server::Handle(Request req) {
	std::cout << req;
	return Response(200, {}, "<!doctype html><title>Hello</title><h1>Hello from server!</h1>\n");			
}


DirServer::DirServer(int port, std::string dir) : Server(port), dir(dir) {}

Response DirServer::Handle(Request req) {
	std::cout << req;
	
	if (req.endpoint == "/") {
		req.endpoint = "/index.html";
	}
	auto fname = dir + req.endpoint;


	// http://stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c
	// http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
	std::ifstream ifs(fname);

	if (!ifs.good()) {
		return Response(404, {}, "");
	}

    auto body = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	return Response(200, {}, body);
}

} // namespace http
