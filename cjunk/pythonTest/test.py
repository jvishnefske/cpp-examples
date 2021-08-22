# import pytest
import cppyy
cppyy.include('Poco/Net/HTTPRequestHandlerFactory.h')
cppyy.include('Poco/Net/HTTPServerRequest.h')
cppyy.include('Poco/Net/HTTPServerResponse.h')
cppyy.include('Poco/Net/HTTPServer.h')
cppyy.load_library('PocoNet')
cppyy.cppdef("""
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"

using namespace Poco::Net;
class MyRequestHandler : public Poco::Net::HTTPRequestHandler {
    virtual void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override{
        response.send() << "welcome to the server";                
    }
};
class MyRequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory {
    Poco::Net::HTTPRequestHandler* createRequestHandler(
        const Poco::Net::HTTPServerRequest& request)
    {
        return new MyRequestHandler();
        //if (request.getURI() == "/") return new RootHandler();
    }
};
""")
cppyy.cppexec("""
using namespace Poco::Net;
auto server = HTTPServer(new MyRequestHandlerFactory(), 8080);
server.start();
""")
cppyy.cppexec("auto m = MyRequestHandler();")


class Handler(cppyy.gbl.Poco.Net.HTTPRequestHandler):
    def handle_request(self, request, response):
        pass


def test_server():
    # myOstream = cppyy.gbl.Poco.Net.HTTPRequestHandler()
    # net = cppyy.gbl.Poco
    pass
