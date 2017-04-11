#include <iostream>
#include "net.h"


void handle(Conn conn) {
    if (!fork()) {
        auto str = conn.ReadString();
        std::cout << "Recieved from client: " << str << std::endl;
        conn.WriteString("Hello from server");
    }
}

int main(int argc, char* argv[])
{
    try {
        auto port = 8081;
        auto l = net::Listen(port);
        
        for (;;) {
            auto conn = l.Accept();
            handle(conn);
        }
    }	
    return 0;
}
