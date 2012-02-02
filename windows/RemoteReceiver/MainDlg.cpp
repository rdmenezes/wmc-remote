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
#include "resource.h"
#include "AutoStart.h"
#include "TrayIcon.h"
#include "Network.h"
#include "Config.h"
#include "AboutDlg.h"
#include "MainDlg.h"

#include <functional>
#include <sstream>


const wchar_t* APP_NAME = L"RemoteReceiver";
const wchar_t* CONFIG_FILE = L"config";
const wchar_t* TRAYICON_TEXT = L"Remote Receiver";
const wchar_t* SHORTCUT_NAME = L"Remote Receiver";
const wchar_t* SHORTCUT_DESC = L"A remote control receiver for Windows Media Center";

const int DEFAULT_PORT = 29092;
const int MIN_PORT = 1024;
const int MAX_PORT = 65535;

// {F694C3E4-F901-45FF-9B7D-6DD06F278CA9}
static const GUID TRAYGUID = 
{ 0xf694c3e4, 0xf901, 0x45ff, { 0x9b, 0x7d, 0x6d, 0xd0, 0x6f, 0x27, 0x8c, 0xa9 } };

const int TIMER_ID_CLICK = 1;
const int TIMER_ID_RESTORE = 2;
const int TIMER_DELAY_RESTORE = 7000;

CMainDlg* CMainDlg::kludge_ = NULL;


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}


BOOL CMainDlg::OnIdle()
{
	return FALSE;
}


LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/,
      LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, 
         ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR,
         ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

   DoDataExchange(FALSE);
   
   kludge_ = this;

   SetupKeyCodes();

   config_.reset(new Config(APP_NAME, CONFIG_FILE));

   autoStart_.reset(new AutoStart(SHORTCUT_NAME, AutoStart::GetAppPath().c_str(),
         AutoStart::GetWorkingDir().c_str(), SHORTCUT_DESC));

   if (autoStart_->IsEnabled())
   {
      c_start.SetCheck(BST_CHECKED);
   }

   PopulateAddresses();
   PopulatePort();

   c_listen.SetCheck(BST_CHECKED);
   StartListening();

   trayIcon_.reset(new TrayIcon(IDR_MAINFRAME, TRAYICON_TEXT));
   trayIcon_->Add(m_hWnd);

	return TRUE;
}


void CMainDlg::PopulateAddresses()
{
   Strings ipAddresses = Network::GetIPAddresses();
   
   for (Strings::iterator iter = ipAddresses.begin();
         iter != ipAddresses.end(); ++iter)
   {
      CA2W szAddress((*iter).c_str());
      c_ipAddresses.AddString(szAddress);
   }

   CA2W defaultAddress(config_->GetIPAddress().c_str());

   int index = c_ipAddresses.FindStringExact(-1, defaultAddress);

   if (index != CB_ERR)
   {
      c_ipAddresses.SetCurSel(index);
   }
   else
   {
      c_ipAddresses.SetCurSel(0);
      int curSel = c_ipAddresses.GetCurSel();
      if (curSel != CB_ERR)
      {
         ATL::CString text;
         c_ipAddresses.GetLBText(curSel, text);
         CW2A szText(text);
         config_->SetIPAddress(szText);
      }
   }
}


void CMainDlg::PopulatePort()
{
   int port = config_->GetPort();

   if (port >= MIN_PORT && port <= MAX_PORT)
   {
      std::wstringstream ss;
      ss << port;
      c_port.SetWindowText(ss.str().c_str());
   }
   else
   {
      port = DEFAULT_PORT;
      std::wstringstream ss;
      ss << port;
      c_port.SetWindowText(ss.str().c_str());
      config_->SetPort(port);
   }
}


LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/,
      LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}


LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, 
      HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}


LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/,
      BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}


LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/,
      BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}


void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}


void CMainDlg::StartListening()
{
   int curSel = c_ipAddresses.GetCurSel();

   if (curSel != CB_ERR)
   {
      ATL::CString text;
      c_ipAddresses.GetLBText(curSel, text);
      CW2A szText(text);

      c_port.GetWindowText(text);
      int port = _ttoi(text);

      network_.reset(new Network(m_hWnd, szText, port));
   }
}


void CMainDlg::StopListening()
{
   if (network_)
   {
      network_.reset();
   }

   if (c_listen.GetCheck() == BST_CHECKED)
   {
      c_listen.SetCheck(BST_UNCHECKED);
   }
}


LRESULT CMainDlg::OnPortChange(WORD /*wNotifyCode*/, WORD /*wID*/,
      HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   StopListening();
   return 0;
}


LRESULT CMainDlg::OnListenClicked(WORD /*wNotifyCode*/, WORD /*wID*/,
      HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (c_listen.GetCheck() == BST_CHECKED)
   {
      StartListening();
   }
   else
   {
      StopListening();
   }

   return 0;
}


LRESULT CMainDlg::OnIPAddressChange(WORD /*wNotifyCode*/, WORD /*wID*/,
      HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   ATL::CString text;
   c_ipAddresses.GetLBText(c_ipAddresses.GetCurSel(), text);
   CW2A szText(text);

   if (config_->GetIPAddress().compare(szText) == 0)
   {
      StopListening();
      config_->SetIPAddress(szText);
   }

   return 0;
}


LRESULT CMainDlg::OnRemoteSignal(UINT /*uMsg*/, WPARAM /*wParam*/,
      LPARAM lParam, BOOL& /*bHandled*/)
{
   bool running = IsMediaCenterRunning();

   if (lParam == 1)
   {
      if (running)
      {
         ShutdownMediaCenter();
         return 0;
      }
   }
   else if (lParam > 1)
   {
      if (!running) { return 0; }
      BringMediaCenterToForeground();
   }

   KeyCodesMap::iterator iter = map_.find(static_cast<int>(lParam));

   if (iter != map_.end())
   {
      SendKeyCodes(iter->second);
   }

   return 0;
}


LRESULT CMainDlg::OnPortKillFocus(WORD /*wNotifyCode*/, WORD /*wID*/,
      HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   ATL::CString text;
   c_port.GetWindowText(text);

   int port = _ttoi(text);

   if (port < MIN_PORT) port = MIN_PORT;
   else if (port > MAX_PORT) port = MAX_PORT;

   std::wstringstream ss;
   ss << port;

   c_port.SetWindowText(ss.str().c_str());

   if (config_->GetPort() != port)
   {
      config_->SetPort(port);
   }

   return 0;
}


void CMainDlg::SetupKeyCodes()
{
   map_.clear();

   KeyCodes codes;
   codes.push_back(VK_LWIN);
   codes.push_back(VK_MENU);
   codes.push_back(VK_RETURN);

   map_[1] = codes;

   codes.clear();
   codes.push_back(VK_CONTROL);
   codes.push_back('O');

   map_[2] = codes;

   codes.clear();
   codes.push_back(VK_RETURN);

   map_[3] = codes;

   codes.clear();
   codes.push_back(VK_BACK);

   map_[4] = codes;

   codes.clear();
   codes.push_back(VK_LEFT);

   map_[5] = codes;

   codes.clear();
   codes.push_back(VK_UP);

   map_[6] = codes;

   codes.clear();
   codes.push_back(VK_DOWN);

   map_[7] = codes;

   codes.clear();
   codes.push_back(VK_RIGHT);

   map_[8] = codes;

   codes.clear();
   codes.push_back(VK_F8);

   map_[9] = codes;

   codes.clear();
   codes.push_back(VK_F9);

   map_[10] = codes;

   codes.clear();
   codes.push_back(VK_F10);

   map_[11] = codes;

   codes.clear();
   codes.push_back(VK_CONTROL);
   codes.push_back('P');

   map_[12] = codes;

   codes.clear();
   codes.push_back(VK_CONTROL);
   codes.push_back(VK_SHIFT);
   codes.push_back('S');

   map_[13] = codes;

   codes.clear();
   codes.push_back(VK_CONTROL);
   codes.push_back('B');

   map_[14] = codes;

   codes.clear();
   codes.push_back(VK_CONTROL);
   codes.push_back('F');

   map_[15] = codes;

   codes.clear();
   codes.push_back(VK_CONTROL);
   codes.push_back(VK_SHIFT);
   codes.push_back('B');

   map_[16] = codes;

   codes.clear();
   codes.push_back(VK_CONTROL);
   codes.push_back(VK_SHIFT);
   codes.push_back('F');

   map_[17] = codes;
}


void CMainDlg::SendKeyCodes(KeyCodes& codes)
{
   // need to send both keys down and up
   std::vector<INPUT> inputs(2 * codes.size());

   // key down in order
   for (size_t i = 0; i < codes.size(); ++i)
   {
      inputs[i].type = INPUT_KEYBOARD;
      inputs[i].ki.wVk = codes[i];
      inputs[i].ki.wScan = 0;
      inputs[i].ki.dwFlags = 0;
      inputs[i].ki.time = 0;
      inputs[i].ki.dwExtraInfo = NULL;
   }

   // key up in reverse order
   for (size_t i = codes.size(); i > 0; --i)
   {
      size_t index = inputs.size() - i;
      inputs[index].type = INPUT_KEYBOARD;
      inputs[index].ki.wVk = codes[i - 1];
      inputs[index].ki.wScan = 0;
      inputs[index].ki.dwFlags = KEYEVENTF_KEYUP;
      inputs[index].ki.time = 0;
      inputs[index].ki.dwExtraInfo = NULL;
   }

   DWORD error = SendInput(static_cast<UINT>(inputs.size()), &inputs[0], sizeof(inputs[0]));
}


LRESULT CMainDlg::OnTrayIcon(UINT /*uMsg*/, WPARAM /*wParam*/,
      LPARAM lParam, BOOL& /*bHandled*/)
{
   switch (LOWORD(lParam))
   {
   case WM_RBUTTONUP:
      ShowTrayIconMenu();
      break;
      
   case WM_LBUTTONDOWN:
      
      SetTimer(TIMER_ID_CLICK, GetDoubleClickTime(), DelayedSingleClick);
      break;

   case WM_LBUTTONDBLCLK:
      KillTimer(TIMER_ID_CLICK);
      CloseDialog(0);
      break;
   }
   return 0;
}


void CMainDlg::ShowTrayIconMenu()
{
   CMenu menu;
   menu.LoadMenu(IDR_TRAYMENU);

   if (menu.IsNull())
   {
      return;
   }

   SetForegroundWindow(m_hWnd);

   CMenuHandle popup = menu.GetSubMenu(0);

   if (!popup.IsNull())
   {
      CPoint ptCursor;
      GetCursorPos(&ptCursor);

      popup.SetMenuDefaultItem(ID__EXIT, FALSE);

      CMenuItemInfo mii;
      popup.GetMenuItemInfo(ID__OPTIONS, FALSE, &mii);

      mii.fMask = MIIM_STATE;
      mii.fState = (IsWindowVisible() ? MFS_DISABLED : MFS_ENABLED);

      popup.SetMenuItemInfo(ID__OPTIONS, FALSE, &mii);

      popup.TrackPopupMenu(TPM_LEFTALIGN, ptCursor.x, ptCursor.y, m_hWnd, NULL);

      SendMessage(WM_NULL, 0, 0);
   }
}


void CALLBACK CMainDlg::DelayedSingleClick(HWND, UINT, UINT_PTR id, DWORD)
{
   if (kludge_)
   {
      kludge_->KillTimer(id);
      kludge_->ToggleWindow();
   }
}


void CMainDlg::ToggleWindow()
{
   int showCmd = (IsWindowVisible() ? SW_HIDE : SW_SHOW);
   ShowWindow(showCmd);
}


LRESULT CMainDlg::OnSysCommand(UINT /*uMsg*/, WPARAM wParam,
      LPARAM /*lParam*/, BOOL& bHandled)
{
   bHandled = FALSE;

   if (SC_MINIMIZE == wParam)
   {
      ShowWindow(SW_HIDE);
      bHandled = TRUE;
   }

   return 0;
}


LRESULT CMainDlg::OnStartClicked(WORD /*wNotifyCode*/, WORD /*wID*/,
      HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (c_start.GetCheck() == BST_CHECKED)
   {
      autoStart_->Enable();
   }
   else
   {
      autoStart_->Disable();
   }

   return 0;
}


LRESULT CMainDlg::OnOptions(WORD /*wNotifyCode*/, WORD wID,
      HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   ToggleWindow();
   return 0;
}


void CMainDlg::BringMediaCenterToForeground()
{
   HWND hwnd = FindWindowW(L"eHome Render Window", NULL);
   if (hwnd)
   {
      SetForegroundWindow(hwnd);
   }
}


bool CMainDlg::IsMediaCenterRunning()
{
   HWND hwnd = FindWindowW(L"eHome Render Window", NULL);
   return hwnd != NULL;
}


void CMainDlg::ShutdownMediaCenter()
{
   HWND hwnd = FindWindowW(L"eHome Render Window", NULL);
   if (hwnd)
   {
      ::PostMessageW(hwnd, WM_CLOSE, 0, 0);
   }
}


void CALLBACK CMainDlg::DelayedTrayIconRestore(HWND, UINT, UINT_PTR, DWORD)
{
   if (kludge_)
   {
      kludge_->trayIcon_->Add(kludge_->m_hWnd);
   }
}


LRESULT CMainDlg::OnTaskbarCreated(UINT /*uMsg*/, WPARAM /*wParam*/,
      LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   SetTimer(TIMER_ID_RESTORE, TIMER_DELAY_RESTORE, DelayedTrayIconRestore);
   return 0;
}
