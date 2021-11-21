#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <memory>
using namespace boost::asio;
class messageClient
{
public:
    messageClient(io_service& context,const ip::tcp::endpoint& endPoint)
        :mSocket(context)
    {
        boost::system::error_code ec;
        mSocket.connect(endPoint, ec);
        if (!ec)
        {
            std::cout << "Connected to " << mSocket.remote_endpoint() << " from " << mSocket.local_endpoint() << std::endl;
            std::string message;
            bool answer_is_present = false;
            while (true)
            {
                answer_is_present = false;
                message.clear();
                std::getline(std::cin, message);
                if (message == "--exit") break;
                message += '\n';
                size_t message_size = message.size();
                boost::asio::write(mSocket, boost::asio::buffer(&message_size, sizeof(message_size)), ec);
                boost::asio::write(mSocket, boost::asio::buffer(message.data(), message.size()), ec);
                std::cout << "[Client] Message has sent. Message size: " << message_size << std::endl;
                while (answer_is_present == false)
                {
                    std::string chat;
                    boost::asio::read(mSocket, boost::asio::buffer(&message_size, sizeof(message_size)), ec);
                    chat.resize(message_size);
                    boost::asio::read(mSocket, boost::asio::buffer(chat.data(), message_size), ec);
                    std::cout << "[Server]:\n" << chat << std::endl;
                    answer_is_present = true;
                }
            }
        }
        else std::cout << "Failed to connect to server:\n" << ec.message() << std::endl;
        
        mSocket.shutdown(boost::asio::socket_base::shutdown_type::shutdown_both, ec);
        mSocket.close(ec);
        if (ec) std::cout << ec.message() << std::endl;
    }
private:
    ip::tcp::socket mSocket;
};
int main()
{
    setlocale(LC_ALL, "Russian");
    boost::system::error_code ec;
    boost::asio::io_service context;
    ip::tcp::endpoint endPoint(ip::make_address("127.0.0.1"), 9999);
    messageClient client(context, endPoint);
    system("pause");
    return 0;
}
