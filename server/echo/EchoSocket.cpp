#include "EchoSocket.h"
#include <cstddef>
#include <iterator>
#include <string_view>

using namespace std;
EchoSocket::EchoSocket(tcp::socket&& socket):Socket(std::move(socket))
{
    
}

void EchoSocket::Start()
{
    string ip_address = GetRemoteIpAddress().to_string();
    cout<< "Accepted connection from  " << ip_address << endl;

    AsyncRead();
}

bool EchoSocket::Update()
{
    return Socket::Update();
}

void EchoSocket::ReadHandler()
{
    MessageBuffer& msg = GetReadBuffer();
    //get string
    const char* rptr = reinterpret_cast<const char*>(msg.GetReadPointer());
    size_t len = msg.GetBufferSize();

    string_view buf(rptr, len);

    size_t pos = buf.find_first_of("\r\n");
    if(pos == string_view::npos)
        return;

    cout<< "Recevied: " << string(rptr, pos + 2);
    SendMessage(msg.GetReadPointer(), pos);

    msg.ReadCompleted(pos + 2);
    AsyncRead();
}

void EchoSocket::SendMessage(const uint8_t* msg, std::size_t len)
{
    MessageBuffer buf(len + 4);
    buf.Write("Sent:", 5);
    buf.Write(msg, len);
    buf.Write("\r\n", 2);
    Socket::SendMessage(std::move(buf));
}

void EchoSocket::OnClose()
{
    cout<< " EchoSocket::OnClose()" <<endl;
    Socket::OnClose();
}