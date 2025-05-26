#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "MessageBuffer.h"
#include <algorithm>
#include <atomic>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <queue>
#include <memory>
#include <iostream>
#include <functional>
#include <type_traits>
#include <boost/asio/ip/tcp.hpp>

using boost::asio::ip::tcp;

#define READ_BLOCK_SIZE 4096

template<class T>
class Socket : public std::enable_shared_from_this<T>
{
public:
    explicit Socket(tcp::socket&& socket) : 
    _socket(std::move(socket)),
    _remoteAddress(_socket.remote_endpoint().address()),
    _remotePort(_socket.remote_endpoint().port()),
    _readBuffer(),
    _closed(false),
    _isWritingAsync(false)
    {
        _readBuffer.Resize(READ_BLOCK_SIZE);
    }

    virtual ~Socket()
    {
        _closed = true;
        boost::system::error_code error;
        _socket.close(error);
    }

    void CloseSocket()
    {
        if (_closed.exchange(true))
            return;
        boost::system::error_code shutdownError;
        if (shutdownError)
            std::cout << "Socket::CloseSocket error when shutting down" << shutdownError.message().c_str() << std::endl;
        OnClose(); 
    }
    boost::asio::ip::address GetRemoteIpAddress() const
    {
        return _remoteAddress;
    }

    uint16_t GetRemotePort() const
    {
        return _remotePort;
    }

    bool IsOpen() const { return !_closed; }

    void AsyncRead()
    {
        if (!IsOpen())
            return;
        _readBuffer.FormatBuffer();
        auto self = this->shared_from_this();
        _socket.async_read_some(boost::asio::buffer(_readBuffer.GetWritePointer(), _readBuffer.GetRemainingSpace()),
            [self](boost::system::error_code error, size_t transferredBytes)
        {
            self->ReadHandlerInternal(error, transferredBytes);
        }); 
    }

    bool AsynWriteFromQueue()
    {
        if(_isWritingAsync)
            return false;
        _isWritingAsync = true;
        auto self = this->shared_from_this();
        _socket.async_write_some(boost::asio::null_buffers(), [self](boost::system::error_code error, size_t transferredBytes)
        {
            self->_isWritingAsync = false;
            self->HandleWriteQueue();
        });
        return false;
    }

    bool HandleWriteQueue()
    {
        if (_writeQueue.empty())
            return false;
        MessageBuffer& currentMessage  = _writeQueue.front();
        std::size_t bytesToSend = currentMessage.GetActiveSize();

        boost::system::error_code error;
        std::size_t bytesSent = _socket.write_some(boost::asio::buffer(currentMessage.GetReadPointer(), currentMessage.GetActiveSize()),error);
        if(error)
        {
            if(error == boost::asio::error::would_block || error == boost::asio::error::try_again)
                return AsynWriteFromQueue();
            _writeQueue.pop();
            if(_writeQueue.empty())
                CloseSocket();
            return false;
        }
        //The client may in half-close status
        else if (bytesSent == 0)
        {
            _writeQueue.pop();
            return false;
        }
        //send not finished.
        else if (bytesSent < bytesToSend)
        {
            currentMessage.ReadCompleted(bytesSent);
            return AsynWriteFromQueue();
        }
        _writeQueue.pop();
        return !_writeQueue.empty();
    }
    MessageBuffer& GetReadBuffer() { return _readBuffer; }
    virtual void Start() = 0;
    virtual bool Update()
    {
        if (_closed)
            return false;
        if(_isWritingAsync || _writeQueue.empty())
            return true;

        for (; HandleWriteQueue();)
            ;
        return true;
    }

protected:
    virtual void ReadHandler() = 0;
    void SendMessage(MessageBuffer&& buffer)
    {
        _writeQueue.push(std::move(buffer));
    }
    virtual void OnClose() { }
private:
    void ReadHandlerInternal(boost::system::error_code error, size_t transferedBytes)
    {
        if(error)
        {
            CloseSocket();
            return;
        }
        _readBuffer.WriteCompleted(transferedBytes);
        ReadHandler();
    }

    tcp::socket _socket;

    boost::asio::ip::address _remoteAddress;
    uint16_t _remotePort;

    MessageBuffer _readBuffer;
    std::queue<MessageBuffer> _writeQueue;

    std::atomic<bool> _closed;

    bool _isWritingAsync;
};

#endif // __SOCKET_H__
