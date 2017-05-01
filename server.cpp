#include <iostream>
#include "net.h"
#include <unistd.h>

void run() {
    auto server1 = std::shared_ptr<http::Server>(new http::DirServer(0, "./test"));
    auto server2 = std::shared_ptr<http::Server>(new http::DirServer(0, "./second_test"));
    http::Router router(8081);
    router.Route("/", server1);
    router.Route("/a", server2);

    try {
        router.ListenAndServe();
    }
    catch (net::Error & err) {
        std::cerr << err.Report() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    run();
    
    return 0;
}
