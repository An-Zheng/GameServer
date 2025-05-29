#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H


#include <algorithm>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/system/error_code.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>


using namespace boost::asio::ip;
using namespace std;
template<class SocketType>
class NetworkThread
{
public:
    NetworkThread(): 
    _thread(nullptr),
    _ioContext(1),
    _acceptSocket(_ioContext),
    _updateTimer(_ioContext),
    _connections(0),
    _stopped(false)
    {

    }



    virtual ~NetworkThread()
    {
        Stop();
        if(_thread)
            DestroyThread();
    }

    bool Start()
    {
        if(_thread)
            return false;
        _thread = make_unique<thread>(&NetworkThread::Run, this);
        return true;
    }

    void Stop()
    {
        _stopped = true;
        _ioContext.stopped();
    }

    void DestroyThread()
    {
        if (_thread && _thread->joinable()) 
        {
            _thread->join();
        }
        _thread.reset();
    }

    int32_t GetConnectionCount() const
    {
        return _connections;
    }
    virtual void AddSocket(shared_ptr<SocketType> sock)
    {
        lock_guard<mutex> lock(_newSocketsLock);

        ++_connections;
        _newSockets.push_back(sock);
    }

    shared_ptr<tcp::socket> GetSocketForAccept() { return make_shared<tcp::socket>(_ioContext);};
protected:
    void Run()
    {
        _updateTimer.expires_from_now(boost::posix_time::milliseconds(1));
        _updateTimer.async_wait([this](boost::system::error_code const& ){this->Update();});
        _ioContext.run();

        _newSockets.clear();
        _sockets.clear();
    }

    void Update()
    {
        if(_stopped)
            return;
        _updateTimer.expires_from_now(boost::posix_time::milliseconds(1));
        _updateTimer.async_wait([this](boost::system::error_code const& ){this->Update();});

        AddNewSockets();

        _sockets.erase(remove_if(_sockets.begin(),_sockets.end(),[this](shared_ptr<SocketType> socket)
        {
            if(!socket->Update())
            {
                if(socket->IsOpen())
                    socket->CloseSocket();

                --this->_connections;
                return true;
            }
            return false;
        }), _sockets.end());
    }

    void AddNewSockets()
    {
        lock_guard<mutex> lock(_newSocketsLock);
        if (_newSockets.empty()) 
            return;

        for (shared_ptr<SocketType> sock : _newSockets)
        {
            if (!sock->IsOpen()) 
            {
                --_connections;
            }
            else
                _sockets.push_back(sock);
        }
        _newSockets.clear();
    }


private:
    
    typedef vector<shared_ptr<SocketType>> SocketContainer;

    unique_ptr<thread> _thread;
    boost::asio::io_context _ioContext;
    tcp::socket _acceptSocket;

    SocketContainer _sockets;
    mutex _newSocketsLock;
    SocketContainer _newSockets;

    atomic<int32_t> _connections;
    atomic<bool> _stopped;

    boost::asio::deadline_timer _updateTimer;
};

#endif