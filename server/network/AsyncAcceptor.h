#ifndef ASYNCACCEPT_H
#define ASYNCACCEPT_H

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cstdint>
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <functional>
#include <ostream>
#include <utility>
#include <functional>
#define MAX_LISTEN_CONNECTIONS boost::asio::socket_base::max_connections

using namespace boost::asio::ip;
using namespace std;
class AsyncAcceptor
{
public:
    typedef void(*AcceptCallback)(tcp::socket&& newSocket, uint32_t threadIndex);
    AsyncAcceptor(boost::asio::io_context& io_context, string const& bindIp, uint16_t port):
        _acceptor( io_context), _endpoint(boost::asio::ip::address::from_string(bindIp), port),
        _socket(io_context), _closed(false), _socketFactory(bind(&AsyncAcceptor::DefaultSocketFactory, this))
    {

    }
    bool Bind()
    {
        boost::system::error_code errorCode;
        _acceptor.open(_endpoint.protocol(), errorCode);
        if (errorCode) 
        {
            cout<< "Failed to open acceptor" <<endl;
            return false;
        }

        _acceptor.set_option(tcp::acceptor::reuse_address(true), errorCode);
        if (errorCode) 
        {   
            cout<< "Failed to set reuse_address in acceptor" <<endl;
            return false;
        }

        _acceptor.bind(_endpoint, errorCode);
        if (errorCode) 
        {
            cout << "Failed to bind" << endl;
            return false;
        }

        _acceptor.listen(MAX_LISTEN_CONNECTIONS, errorCode);
        if (errorCode) 
        {
            cout << "Failed to listen" << endl;
            return false;
        }
        return true;
    }

    void Close()
    {
        if(_closed.exchange(true))
            return;
        boost::system::error_code errorCode;
        _acceptor.close(errorCode);
        if (errorCode) 
        {
            cout << "Failed to close" << endl;
        }
    }

    void SetSocketFactory(function<pair<tcp::socket*, uint32_t>()> func) { _socketFactory = func;}

    template<AcceptCallback acceptCallback>
    void AsyncAcceptWithCallback()
    {
        auto result = _socketFactory();
        tcp::socket* socket = result.first;
        uint32_t threadIndex = result.second;

        _acceptor.async_accept(*socket, [this, socket, threadIndex](boost::system::error_code error)
        {
            if(!error)
            {
                try
                {
                    socket->non_blocking(true);
                    acceptCallback(std::move(*socket), threadIndex);
                }
                catch(boost::system::system_error const& err)
                {
                    cout<< "Failed to initialize client's coket" << endl;
                }
            }

            if(!_closed)
                this->AsyncAcceptWithCallback<acceptCallback>();

        });
    }


private:
    pair<tcp::socket*, uint32_t> DefaultSocketFactory() { return make_pair( &_socket,0 );}

    tcp::acceptor _acceptor;
    tcp::endpoint _endpoint;
    tcp::socket _socket;
    atomic<bool> _closed;
    function<pair<tcp::socket*, uint32_t>()> _socketFactory;
};


#endif