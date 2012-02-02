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
#include "AboutDlg.h"


LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/,
      LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   DoDataExchange(FALSE);
   c_website.SetToolTipText(L"wmc-remote.googlecode.com");
   c_website.SetHyperLink(L"http://wmc-remote.googlecode.com");
	CenterWindow(GetParent());
	return TRUE;
}


LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID,
      HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
