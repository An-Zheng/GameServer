#include <boost/asio/io_context.hpp>
#include <memory>
#include <iostream>
#include "EchoSocketMgr.h"
int main()
{
    std::shared_ptr<boost::asio::io_context> ioContext = std::make_shared<boost::asio::io_context>();
    std::cout<< "ioContext addr :" << &ioContext <<std::endl;
    if (!sEchoSocketMgr.StartNetwork(*ioContext,"0.0.0.0", 8989 ,5)) 
    {
        cout<< "Failed to initialize network " << endl;
    }

    ioContext->run();
    sEchoSocketMgr.StopNetwork();
    return 0;
}