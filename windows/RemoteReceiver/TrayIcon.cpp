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
