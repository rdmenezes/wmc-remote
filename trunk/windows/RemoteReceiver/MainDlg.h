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

#include "Messages.h"

#include <map>
#include <memory>
#include <boost/scoped_ptr.hpp>


typedef std::vector<WORD> KeyCodes;
typedef std::map<int, KeyCodes> KeyCodesMap;


class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler,
      public CWinDataExchange<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(RWM_REMOTESIGNAL, OnRemoteSignal)
      MESSAGE_HANDLER(RWM_TRAYICON, OnTrayIcon)
      MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
      COMMAND_HANDLER(IDC_PORT, EN_CHANGE, OnPortChange)
      COMMAND_HANDLER(IDC_LISTEN, BN_CLICKED, OnListenClicked)
      COMMAND_HANDLER(IDC_IPADDRESSES, CBN_SELCHANGE, OnIPAddressChange)
      COMMAND_HANDLER(IDC_START, BN_CLICKED, OnStartClicked)
      COMMAND_HANDLER(IDC_PORT, EN_KILLFOCUS, OnPortKillFocus)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
      COMMAND_ID_HANDLER(ID__EXIT, OnCancel)
      COMMAND_ID_HANDLER(ID__OPTIONS, OnOptions)
   END_MSG_MAP()

   BEGIN_DDX_MAP(CMainDlg)
      DDX_CONTROL_HANDLE(IDC_IPADDRESSES, c_ipAddresses)
      DDX_CONTROL_HANDLE(IDC_PORT, c_port)
      DDX_CONTROL_HANDLE(IDC_LISTEN, c_listen)
      DDX_CONTROL_HANDLE(IDC_START, c_start)
   END_DDX_MAP()

private:
   boost::scoped_ptr<Config> config_;
   std::unique_ptr<Network> network_;
   boost::scoped_ptr<TrayIcon> trayIcon_;
   boost::scoped_ptr<AutoStart> autoStart_;

   KeyCodesMap map_;

   CComboBox c_ipAddresses;
   CEdit c_port;
   CButton c_listen;
   CButton c_start;

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
//       BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/,
//       BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
         BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
         BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/,
         BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/,
         BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/,
         BOOL& /*bHandled*/);
   LRESULT OnPortChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/,
         BOOL& /*bHandled*/);
   LRESULT OnListenClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/,
         BOOL& /*bHandled*/);
   LRESULT OnStartClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/,
         BOOL& /*bHandled*/);
   LRESULT OnIPAddressChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/,
         BOOL& /*bHandled*/);
   LRESULT OnRemoteSignal(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
         BOOL& /*bHandled*/);
   LRESULT OnTrayIcon(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
         BOOL& /*bHandled*/);
   LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
         BOOL& /*bHandled*/);
   LRESULT OnPortKillFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/,
         BOOL& /*bHandled*/);
   LRESULT OnOptions(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/,
         BOOL& /*bHandled*/);

	void CloseDialog(int nVal);

   void PopulateAddresses();
   void PopulatePort();

   void StartListening();
   void StopListening();

   void SetupKeyCodes();
   void SendKeyCodes(KeyCodes& codes);

   void ShowTrayIconMenu();

   void ToggleWindow();

   static CMainDlg* kludge_;
   static void CALLBACK DelayedSingleClick(HWND, UINT, UINT_PTR, DWORD);

   static void BringMediaCenterToForeground();
};
