#include "net.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


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


Conn Dial(const std::string host, const int port) { // note: not fully implemented
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


std::map<int, std::string> codeDesc = {{200, "OK"}, {404, "Not found"}};

std::map<std::string, std::string> contentTypes = {
	{"html", "text/html"},
	{"svg", "image/svg+xml"}
};


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
	o << status << ' ' << codeDesc[status] << "\r\n"; // todo: implement reason phrases
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
	
	auto ext = fname.substr(fname.find_last_of(".") + 1);
	auto inferredContentType = contentTypes[ext];
    auto body = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	return Response(200, {{"Content-Type", inferredContentType}}, body);
}


Router::Router(int port) : Server(port), routes() {}

void Router::Route(std::string path, std::shared_ptr<Server> srv) {
	routes.emplace(path, srv);
}

Response Router::Handle(Request req) {
	std::string route;
	for (const auto & pair : routes) {
		if (std::mismatch(pair.first.begin(), pair.first.end(), req.endpoint.begin()).first == pair.first.end()) {
			if (pair.first.size() > route.size()) {
				route = pair.first;
			}
		}
	}
	std::cerr << "Routing " << req.endpoint << " -> " << route << std::endl;
	if (route == "") {
		return Response(404, {}, "");
	}
	auto & srv = routes.at(route); // note: find issue with straightforward way
	req.endpoint.erase(1, route.size() - 1);
	auto resp = srv->Handle(req);
	return resp;
}

} // namespace http
