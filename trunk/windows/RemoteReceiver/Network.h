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
