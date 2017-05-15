#include <iostream>
#include "net.h"
#include <unistd.h>

class MJSServer : public http::Server {
public:
    MJSServer(int port);

    virtual Response Handle(Request req);
};

MJSServer::MJSServer(int port) : http::Server(port) {}

http::Response MJSServer::Handle(http::Request req) {
    auto script = req.endpoint;

    try {
        auto str = run(script);
        return Response(200, {}, str);
    }
    catch (mjs::Error err) {
        return Response(503, {}, err.str());
    }
}


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
