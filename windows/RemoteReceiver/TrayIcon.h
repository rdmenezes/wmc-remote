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
