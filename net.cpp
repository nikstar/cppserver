
using namespace net;

explicit Conn::Conn(int sd) : sd(sd) {};

std::string Conn::ReadString() {
	return "";
}
	
void Conn::WriteString(std::string str) {

}

void Conn::Close() {

}


Conn Dial(const std::string host, const int port) throws(ConnectionError) {
	return Conn(0);
}


std::string ConnectionError::Report() {
	return "Connection error";
}


explicit Listner(int sd) : sd(sd);

Conn Listner::Accept() {
	return Conn(0);
}

Listner Listen(const int port) {
	return Listner(0);
}



