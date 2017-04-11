#include <iostream>
#include "net.h" 

int main(int argc, char* argv[])
{
    try {
        char host[64] = "localhost"; // gethostname(host, sizeof host)
        const int port = 8080;
        
        auto conn = net::Dial(host, port);
        
        conn.WriteString("Hello from client!");
        
        auto respStr = conn.ReadString();
        
        std::cout << "Read from server: " << respStr << std::endl;
        
        conn.Close();
    } catch (net::ConnectionError& err) {
        std::cerr << err.Report() << std::endl;
        exit(1);
    }
}

