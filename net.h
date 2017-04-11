#ifndef __SOCK_WRAP_H__
#define __SOCK_WRAP_H__

#include <string>

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
    
    class ConnctionError : public Error {
    public:
        std::string Report() {
            return "Connection error";
        }
    };
        
    class Listner {
    public:
        explicit Listner(int sd);
        
        Conn Accept();
    };
    
	Listner Listen(const int port);
}

#endif
