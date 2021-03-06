#ifndef _net_h
#define _net_h

#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace net {
    
    class Conn {
        int sd;
    public:
        explicit Conn(int sd);
        
        std::string ReadString();        
        void WriteString(std::string str);
        void Close();
    };
    
    Conn Dial(const std::string host, const int port); // note: not currently in use
    

    class Listner {
        int sd;
    public:
        explicit Listner(int sd = 0);
        
        Conn Accept();
    };
    
	Listner Listen(const int port);


    class Error {
        std::string msg;
    public:
        Error(std::string descr);

        std::string Report();
    };
    
}


namespace http {

    extern std::map<int, std::string> codeDesc;
    extern std::map<std::string, std::string> contentType;
    
    class Request {
    public:
        Request(std::string method, std::string endpoint, std::map<std::string, std::string> headers, std::string body);
        Request(std::string str);

        std::string method;
        std::string endpoint;
        std::map<std::string, std::string> headers;
        std::string body;
    };

    std::ostream &operator<<(std::ostream &o, Request const &req);


    class Response {
    public:
        Response(int status, std::map<std::string, std::string> headers, std::string body);        

        int status;
        std::map<std::string, std::string> headers;
        std::string body;

        std::string str();
    };

    class Handler {
    public:
        virtual Response Handle(Request req) = 0;
    };

    class Server : public Handler {
    public:
        Server(int port);

        void ListenAndServe();
        virtual Response Handle(Request req) = 0;
    protected:
        int port;
        net::Listner l;
    };


    class DirServer : public Server {
    public:
        DirServer(int port, std::string dir);

        virtual Response Handle(Request req);
    protected:
        std::string dir;
    };


    class Router : public Server {
    public:
        Router(int port);

        virtual void Route(std::string path, std::shared_ptr<Server> srv);
        virtual Response Handle(Request req);
    protected:
        std::map<std::string, std::shared_ptr<Server> > routes;
    };

}

#endif
