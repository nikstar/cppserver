#include <iostream>
#include "net.h"
#include <unistd.h>

void handle(net::Conn conn) {
    if (!fork()) {
        auto str = conn.ReadString();
        std::cout << "Recieved from client:\n" << str << std::endl;
        conn.WriteString("Hello from server\n");
        conn.Close();
    }
    conn.Close();
}

void run_tcp_server() {
    try {
        auto port = 8081;
        auto l = net::Listen(port);
        
        for (;;) {
            auto conn = l.Accept();
            handle(conn);
        }
    }	
    catch (net::Error & err) {
        std::cerr << err.Report() << std::endl;
    }
}

void run_http_server() {
    http::Server server(8081);
    try {
        server.ListenAndServe();
    }
    catch (net::Error & err) {
        std::cerr << err.Report() << std::endl;
    }
}

void run_dir_http_server() {
    http::DirServer server(8081, "./test");
    try {
        server.ListenAndServe();
    }
    catch (net::Error & err) {
        std::cerr << err.Report() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    // run_tcp_server();
    // run_http_server();
    run_dir_http_server();
    
    return 0;
}
