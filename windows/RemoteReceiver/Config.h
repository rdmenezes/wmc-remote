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

#include <boost/utility.hpp>
#include <string>


class Config : boost::noncopyable
{
public:
   Config(const wchar_t* appName, const wchar_t* fileName);
   ~Config();

   static std::wstring GetConfigPath(const wchar_t* appName,
         const wchar_t* fileName);

   const std::string& GetIPAddress() const { return ipAddress_; }
   void SetIPAddress(const char* ipAddress);

   int GetPort() const { return port_; }
   void SetPort(int port);

   void Load();
   void Save();

private:
   std::wstring path_;
   bool dirty_;
   
   std::string ipAddress_;
   int port_;
};
