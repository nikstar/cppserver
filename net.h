#ifndef __SOCK_WRAP_H__
#define __SOCK_WRAP_H__

#include <string>
#include <iostream>
#include <io.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace net {
    
    class Conn {
    public:
        explicit Conn(int sd);
        
        std::string ReadString();        
        void WriteString(std::string str);
        
        Close();
    }
    
    Conn Dial(const std::string host, const int port) throws (ConnectionError);
    
    class Error {
    public:
        std::string Report() = 0;
    }
    
    class ConnctionError : public Error {
    public:
        std::string Report() {
            return "Connection error";
        }
    }
        
    class Listner {
    public:
        explicit Listner(int sd);
        
        Conn Accept();
    }
    
	Listner Listen(const int port);
}

#endif
