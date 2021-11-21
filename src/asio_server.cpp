#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <memory>
#include <deque>
#include <array>
using namespace boost::asio;
class Connection
{
public:
    Connection(io_service& Context, std::mutex& server_mutex, std::string& chat, int ID)
        :mContext(Context),
        mSocket(Context),
        mServerMutex(server_mutex),
        mChat(chat),
        mConnectionId(ID){}

    void start()
    {
        std::cout << "New Connection from: " << mSocket.remote_endpoint() << " to "<< mSocket.local_endpoint()<<std::endl;
        boost::asio::ip::tcp::socket::keep_alive keep_alive_option(true); 
        mSocket.set_option(keep_alive_option);
        pThread = std::make_unique<std::thread>(
            [this]() {
                reciever(mSocket);
            });
       
        pThread->detach();
    }
    int getId() const { return mConnectionId; }
    bool isOpen() const { return mSocket.is_open(); }
    ip::tcp::socket& socket() { return mSocket; }
    Connection(const Connection&) = delete;
    Connection& operator= (const Connection&) = delete;
    Connection(Connection&&) = default;
    Connection& operator= (Connection&&) = delete;

    ~Connection() {};
private:
    void reciever(ip::tcp::socket& sock)
    {

        while (sock.is_open())
        {
            size_t message_size;
            std::string message;
            boost::system::error_code ec;
            if (sock.available(ec) != 0)
            {
                boost::asio::read(sock, boost::asio::buffer(&message_size, sizeof(message_size)), ec);
                message.resize(message_size);
                boost::asio::read(sock, boost::asio::buffer(message.data(), message_size), ec);
                std::cout << "Thread id: " << std::this_thread::get_id()<< " ClientID: "<< mConnectionId << "\nMessage size : " << message_size << "\nMessage:\n " << message << std::endl;
                mServerMutex.lock();
                message = std::to_string(mConnectionId) + " | " + message;
                mChat += message;
                mServerMutex.unlock();
                size_t chat_size = mChat.size();
                boost::asio::write(sock, boost::asio::buffer(&chat_size, sizeof(chat_size)), ec);
                if (ec) std::cout << ec.message() << std::endl;
                
                boost::asio::write(sock, boost::asio::buffer(mChat.data(), mChat.size()), ec);
                if (ec) std::cout << ec.message() << std::endl;

            }
            
            std::array<char, 128> tempBuffer;
            boost::system::error_code error;
            sock.receive(boost::asio::buffer(tempBuffer), ip::tcp::socket::message_peek, error);
            if (error == boost::asio::error::eof)
            {
                mServerMutex.lock();
                std::cout << "EOF in connection: "<< mConnectionId << ", Thread ("<< std::this_thread::get_id() << ") will be closed\n";

                mSocket.shutdown(boost::asio::socket_base::shutdown_type::shutdown_both, ec);
                mSocket.close(ec);
                if (ec) std::cout << ec.message() << std::endl;
                
                mServerMutex.unlock();
                break;
            }
        }
     
    }
    io_service& mContext;
    ip::tcp::socket mSocket;
    std::string& mChat;
    std::unique_ptr<std::thread> pThread;
    std::mutex& mServerMutex;
    int mConnectionId;
};
class MessageServer
{
public:
    MessageServer(io_service& init_context, const ip::tcp::endpoint& init_end_point)
        :mAcceptor(init_context, init_end_point),
        mContext(init_context)
    {
        pAcceptorThread = std::make_unique<std::thread>([this]() { do_accept(); });
        pAcceptorThread->detach();
    }
    void printConnectionsStatus()
    {
        std::lock_guard<std::mutex> lg(mServerMutex);
        for (auto& connection : connections)
            std::cout << "Connection_id: " << connection.getId() << " Is_open: " << std::to_string(connection.isOpen()) << std::endl;
    }
    void printChat()
    {
        std::lock_guard<std::mutex> lg(mServerMutex);
        std::cout << "====================================" << std::endl;
        std::cout << mGlobalChat << std::endl;
        std::cout << "====================================" << std::endl;
    }
private:
    void do_accept()
    {
        std::cout << "Accept thread: " << std::this_thread::get_id() << std::endl;
        for (int i = 0; i < 100; i++) {
            boost::system::error_code ec;
            mServerMutex.lock();
            connections.emplace_back(mContext, mServerMutex, mGlobalChat, i);
            mServerMutex.unlock();
           
            mAcceptor.accept(connections.back().socket(), ec);
            if (!ec) {
                std::cout << "====================================" << std::endl;
                std::cout << mGlobalChat << std::endl;
                std::cout << "====================================" << std::endl;
                std::cout << "Client with id = "<< i << " has connected!\n" << std::endl;
                connections.back().start();
            }
            else
            {
                std::cout << "Failed to connect client to server:\n" << ec.message() << std::endl;
                connections.back().socket().close();
                mAcceptor.close();
            }
           
        }
    }
    std::deque<Connection> connections;
    ip::tcp::acceptor mAcceptor;
    boost::asio::io_service& mContext;
    std::mutex mServerMutex;
    std::string mGlobalChat;
    std::unique_ptr<std::thread> pAcceptorThread;
};
int main()
{
    std::setlocale(LC_ALL, "Russian");
    boost::asio::io_service context;
    ip::tcp::endpoint endPoint(ip::tcp::v4(), 9999);
    std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;
    MessageServer server(context, endPoint);
    std::string command;
    while (true)
    {
        command.clear();
        std::cin >> command;
        if (command == "--all")
        {
            server.printConnectionsStatus();
            continue;
        }
        if (command == "--chat")
        {
            server.printChat();
            continue;
        }
        if (command == "--exit")
            break;
        std::cout << "Ñommand was not recognized\n";
    }
    return 0;
}
