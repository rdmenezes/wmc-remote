// wmc-remote -- An Android-based remote control for Windows Media Center
// Copyright (C) 2011-2012  http://wmc-remote.googlecode.com
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
#include "Config.h"
#include <Shlobj.h>

#include <exception>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;


const char* KEY_IP_ADDRESS = "config.ip_address";
const char* KEY_PORT = "config.port";


class SafePWSTR
{
public:
   SafePWSTR() : _p(NULL) {}
   ~SafePWSTR() { if (_p) CoTaskMemFree(_p); }

   operator PWSTR*() { return &_p; }
   operator PWSTR() { return _p; }

private:
   PWSTR _p;
};


Config::Config(const wchar_t* appName, const wchar_t* fileName) : 
      path_(GetConfigPath(appName, fileName)), dirty_(false), port_(0)
{
   Load();
}


Config::~Config()
{
   Save();
}


std::wstring Config::GetConfigPath(const wchar_t* appName,
         const wchar_t* fileName)
{
   SafePWSTR rootPath;
   HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, rootPath);

   if (FAILED(hr)) 
   {
      return L"";
   }

   fs::path configPath(rootPath);
   configPath /= appName;

   if (!fs::exists(configPath))
   {
      try { fs::create_directory(configPath); }
      catch (fs::filesystem_error& e)
      {
         std::cerr << e.what() << std::endl;
         return L"";
      }
   }

   configPath /= fileName;
   
   return configPath.c_str();
}


void Config::SetIPAddress(const char* ipAddress)
{
   if (ipAddress_ != ipAddress)
   {
      ipAddress_ = ipAddress;
      dirty_ = true;
   }
}


void Config::SetPort(int port)
{
   if (port_ != port)
   {
      port_ = port;
      dirty_ = true;
   }
}


void Config::Load()
{
   if (fs::exists(path_))
   {
      try
      {
         fs::ifstream file(path_);
         pt::ptree pt;
         pt::read_xml(file, pt);
         ipAddress_ = pt.get<std::string>(KEY_IP_ADDRESS);
         port_ = pt.get<int>(KEY_PORT);
      }
      catch (std::exception& e)
      {
         std::cerr << e.what() << std::endl;
      }
   }
}


void Config::Save()
{
   if (dirty_)
   {
      try
      {
         fs::ofstream file(path_);
         pt::ptree pt;
         pt.put(KEY_IP_ADDRESS, ipAddress_);
         pt.put(KEY_PORT, port_);
         pt::write_xml(file, pt);
         dirty_ = false;
      }
      catch (std::exception& e)
      {
         std::cerr << e.what() << std::endl;
      }
   }
}
