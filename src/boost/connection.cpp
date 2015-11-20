#include "connection.hpp"
#include "connection_manager.hpp"

using namespace std;
using namespace gsl;


Connection::Connection(boost::asio::ip::tcp::socket socket,
                       ConnectionManager& manager,
                       int numStreamBytes):
    _socket(std::move(socket)),
    _connectionManager(manager),
    _stream{numStreamBytes}
{
}

void Connection::start() {
    doRead();
}

void Connection::stop() {
    _socket.close();
}

void Connection::doRead() {
    auto self(shared_from_this());
    _socket.async_read_some(boost::asio::buffer(_stream.readableData(), _stream.readableDataSize()),
        [this, self](boost::system::error_code error, std::size_t ) {
            if(!error) {
                cerr << "Error reading from TCP socket" << endl;
                doRead();
            } else if(error != boost::asio::error::operation_aborted) {
                _connectionManager.stop(shared_from_this());
            }
        });
}

void Connection::newMessage(span<const ubyte> bytes) {
    if(!connected) return;

    auto self(shared_from_this());
    boost::asio::async_write(_socket, boost::asio::buffer(bytes.data(), bytes.size()),
                             [this, self](boost::system::error_code, std::size_t) {
                             });
}
