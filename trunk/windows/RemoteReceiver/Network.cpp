// wmc-remote -- An Android-based remote control for Windows Media Center
// Copyright (C) 2011  http://wmc-remote.googlecode.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "Network.h"
#include "Messages.h"

#include <boost/bind.hpp>

#include <exception>

#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")


namespace ip = boost::asio::ip;


class WSAException : public std::exception
{
public:
   WSAException(const char* message, int err) : message_(message), err_(err) {}
   virtual const char* what() const throw() { return message_.c_str(); }

private:
   std::string message_;
   int err_;
};


struct WSAInit
{
   WSAInit()
   {
      WORD wVersionRequested = MAKEWORD(2, 2);
      WSADATA wsaData;
      int err = WSAStartup(wVersionRequested, &wsaData);
      if (err != 0 || LOBYTE(wsaData.wVersion) != 2 || 
            HIBYTE(wsaData.wVersion) != 2)
      {
         throw WSAException("WSAStartup failure", err);
      }
   }

   ~WSAInit()
   {
      WSACleanup();
   }
};


Network::Network(HWND parent, const char* ipAddress, int port) :
      parent_(parent), ioService_(),
      socket_(ioService_, 
            udp::endpoint(ip::address_v4::from_string(ipAddress), port))
{
   StartReceive();
   worker_ = boost::thread(boost::bind(&boost::asio::io_service::run,
            &ioService_));
}


Network::~Network()
{
   ioService_.stop();
   worker_.join();
}


Strings Network::GetIPAddresses()
{
   Strings ipAddresses;

   WSAInit wsaInit;

   char hostname[128];
   gethostname(hostname, sizeof(hostname));
 
   struct hostent* phe = gethostbyname(hostname);
   if (phe == NULL)
   {
      throw WSAException("gethostbyname() failure", 0);
   }

   for (int i = 0; phe->h_addr_list[i] != 0; ++i)
   {
      struct in_addr addr;
      memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
      ipAddresses.push_back(inet_ntoa(addr));
   }

   return ipAddresses;
}


void Network::StartReceive()
{
   socket_.async_receive_from(boost::asio::buffer(recvBuffer_),
      remoteEndpoint_, boost::bind(&Network::HandleReceive, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
}


void Network::HandleReceive(const boost::system::error_code& error,
      std::size_t /*bytes_transferred*/)
{
   PostMessage(parent_, RWM_REMOTESIGNAL, NULL, recvBuffer_[0]);
   StartReceive();
}
