#ifndef ECHOSOCKET_H
#define ECHOSOCKET_H
#include "../network/Socket.h"
#include <cstddef>
class EchoSocket : public Socket<EchoSocket>
{
    
public:
    explicit EchoSocket(tcp::socket&& socket);

    virtual void Start() override;

    virtual bool Update() override;

    virtual void ReadHandler() override;

    void SendMessage(const uint8_t* msg, std::size_t len);
    
protected:
    virtual void OnClose() override;
};
#endif