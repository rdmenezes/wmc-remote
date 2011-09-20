#pragma once

#include <ShellAPI.h>
#include <string>
#include <boost/utility.hpp>


const UINT RWM_TRAYICON = 
      RegisterWindowMessageW(L"C771A50A_AD69_4D8C_90FA_A698E1A44C48");


class SafeIcon : boost::noncopyable
{
public:
   explicit SafeIcon(UINT id);
   ~SafeIcon();

   UINT GetID() const { return id_; }
   inline operator HICON() { return hIcon_; }

private:
   UINT id_;
   HICON hIcon_;
};


class TrayIcon : boost::noncopyable
{
public:
   TrayIcon(UINT id, const wchar_t* text);
   ~TrayIcon();

   void Add(HWND hWnd);
   void Remove();

private:
   std::wstring text_;
   SafeIcon icon_;
   NOTIFYICONDATAW nid_;
};
