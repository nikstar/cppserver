#ifndef _net_h
#define _net_h

#include <string>
#include <map>

namespace net {
    
    class Conn {
    public:
        explicit Conn(int sd);
        
        std::string ReadString();        
        void WriteString(std::string str);
        
        void Close();
    };
    
    Conn Dial(const std::string host, const int port);
    
    class Error {
    public:
        std::string Report() = 0;
    };
    
    class ConnectionError : public Error {
        std::string descr;
    public:
        ConnectionError(std::string descr);

        std::string Report() {
            return "Connection error: " + descr;
        }
    };
        
    class Listner {
    public:
        explicit Listner(int sd);
        
        Conn Accept();
    };
    
	Listner Listen(const int port);
}

namespace http {

    class Request {
    public:
        Request(std::string method = "GET", std::string endpoint, std::map<std::string, std::string> headers, std::string body);

        std::string method;
        std::string endpoint;
        std::map<std::string, std::string> headers;
        std::string body;
    }

    class Response {
    public:
        Response(int status = 200, std::map<std::string, std::string> headers = std::map(), std::string body);        

        int status;
        std::map<std::string, std::string> headers;
        std::string body;
    }

    class Server {
        Listner l;
    public:
        final void ListenAndServe();
        virtual Response Handle(Request req);
    }

}

#endif
