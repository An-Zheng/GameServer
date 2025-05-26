#ifndef SOCKETMGR_H
#define SOCKETMGR_H


#include "AsyncAcceptor.h"
#include "NetworkThread.h"
#include <algorithm>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/system_error.hpp>
#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <utility>
using boost::asio::ip::tcp;
using namespace std;
template<class SocketType>
class SocketMgr
{
public:
    virtual ~SocketMgr()
    {

    }

    virtual bool StartNetwork(boost::asio::io_context& ioContext, std::string const& bindIp, uint16_t port, int threadCount)
    {
        unique_ptr<AsyncAcceptor> acceptor = nullptr;
        try
        {
            acceptor = make_unique<AsyncAcceptor>(ioContext, bindIp, port);
        }
        catch (boost::system::system_error const& err)
        {
            cout << "SocketMgr.StartNetwork " <<err.what() << endl;
            return false;
        }
        if (!acceptor->Bind())
        {
            cout << "StartNetwork failed to bind socket acceptor " << endl;
            return false;
        }

        _acceptor = std::move(acceptor);
        _threadCount = threadCount;
        _threads.reset(CreateThreads().release());

        for (int32_t i = 0; i < _threadCount; ++i) 
            _threads[i].Start();
        _acceptor->SetSocketFactory([this](){return GetSocketForAccept();});
        return true;
    }

    virtual void StopNetwork()
    {
        _acceptor->Close();
        if ( _threads != 0 )
            for (int32_t i = 0; i < _threadCount; ++i)
                _threads[i].Stop();
        Wait();
        _acceptor.reset();
        _threads.reset();
        _threadCount = 0;
    }

    void Wait()
    {
        if (_threadCount != 0)
            for(int32_t i = 0; i < _threadCount; ++i)
                _threads[i].DestroyThread();
    }

    virtual void OpenNewSocket(tcp::socket&& socket, u_int32_t threadIndex)
    {
        try
        {
            shared_ptr<SocketType> newSocket = make_shared<SocketType>(std::move(socket));
            newSocket->Start();

            _threads[threadIndex].AddSocket(newSocket);
        }
        catch (boost::system::system_error const & err)
        {
            cout << "SocketMgr.OpenNewSocket " <<err.what() << endl;
        }
    }

    int32_t GetNetworkThreadCount() const {return _threadCount;}
    uint32_t SelectThreadWithMinConnections() const 
    {
        uint32_t min = 0;

        return min;
    }
    std::pair<shared_ptr<tcp::socket>, uint32_t> GetSocketForAccept()
    {
        uint32_t threadIndex = SelectThreadWithMinConnections();
        return std::make_pair(_threads[threadIndex].GetSocketForAccept(), threadIndex);
    }

    SocketMgr() : _threadCount(0)
    {   

    }
    virtual unique_ptr<NetworkThread<SocketType>[]> CreateThreads() const = 0;
    unique_ptr<AsyncAcceptor> _acceptor;
    unique_ptr<NetworkThread<SocketType>[]> _threads;
    int32_t _threadCount;
};
#endif