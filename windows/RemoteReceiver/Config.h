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
