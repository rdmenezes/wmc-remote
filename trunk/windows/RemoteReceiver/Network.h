#pragma once

#include <string>
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>


typedef std::vector<std::string> Strings;


using boost::asio::ip::udp;


class Network : boost::noncopyable
{
public:
   Network(HWND parent, const char* ipAddress, int port);
   ~Network();

   static Strings GetIPAddresses();

private:
   HWND parent_;
   boost::asio::io_service ioService_;
   boost::thread worker_;
   udp::socket socket_;
   udp::endpoint remoteEndpoint_;
   boost::array<char, 1> recvBuffer_;

   void StartReceive();
   void HandleReceive(const boost::system::error_code& error,
         std::size_t /*bytes_transferred*/);
};
