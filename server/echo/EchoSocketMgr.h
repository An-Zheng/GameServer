#ifndef ECHOSOCKETMGR_H
#define ECHOSOCKETMGR_H
#include "EchoSocket.h"
#include "../network/SocketMgr.h"
#include <cstdint>
#include <memory>
#include <utility>
class EchoSocketMgr : public SocketMgr<EchoSocket>
{
public:
    static EchoSocketMgr& Instance()
    {
        static EchoSocketMgr instance;
        return instance;
    }


    bool StartNetwork(boost::asio::io_context& ioContext, std::string const& bindIp, uint16_t port, int threadCount = 1 ) override
    {
        
        if(!SocketMgr::StartNetwork(ioContext, bindIp, port, threadCount))
        {
            return false;
        }
        _acceptor->AsyncAcceptWithCallback<EchoSocketMgr::OnSocketAccept>();
        return true;
    }
protected:
    unique_ptr<NetworkThread<EchoSocket>[]> CreateThreads() const override
    {
        return make_unique<NetworkThread<EchoSocket>[]>(5);
    }    

    static void OnSocketAccept(tcp::socket&& socket, uint32_t threadIndex)
    {
        Instance().OpenNewSocket(std::forward<tcp::socket>(socket), threadIndex);
    }

};
#define sEchoSocketMgr EchoSocketMgr::Instance()
#endif