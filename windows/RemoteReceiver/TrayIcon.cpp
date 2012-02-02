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
#include "TrayIcon.h"


SafeIcon::SafeIcon(UINT id) : id_(id),
   hIcon_(reinterpret_cast<HICON>(LoadImage(GetModuleHandle(NULL),
      MAKEINTRESOURCE(id), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON),
      GetSystemMetrics(SM_CYSMICON), LR_LOADTRANSPARENT)))
{
}


SafeIcon::~SafeIcon()
{
   DestroyIcon(hIcon_);
}


TrayIcon::TrayIcon(UINT id, const wchar_t* text) :
      text_(text), icon_(id)
{
   ZeroMemory(&nid_, sizeof(nid_));
   nid_.cbSize = sizeof(nid_);
}


TrayIcon::~TrayIcon()
{
   Remove();
}


void TrayIcon::Add(HWND hWnd)
{
   nid_.hWnd = hWnd;
   nid_.uID = icon_.GetID();
   nid_.hIcon = icon_;
   nid_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
   nid_.uCallbackMessage = RWM_TRAYICON;

   wcscpy_s(nid_.szTip, _countof(nid_.szTip), text_.c_str());

   Shell_NotifyIconW(NIM_ADD, &nid_);
}


void TrayIcon::Remove()
{
   Shell_NotifyIconW(NIM_DELETE, &nid_);
}
