// RemoteReceiver.cpp : main source file for RemoteReceiver.exe
//

#include "stdafx.h"
#include "resource.h"
#include "AutoStart.h"
#include "TrayIcon.h"
#include "Network.h"
#include "Config.h"
#include "AboutDlg.h"
#include "MainDlg.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if (dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

   // initial dialog state is hidden
	dlgMain.ShowWindow(SW_HIDE);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
      LPTSTR lpstrCmdLine, int nCmdShow)
{
   HANDLE hMutexSingleInstance = CreateMutexW(NULL, FALSE,
         L"D552D814_DA45_43A0_A084_D691B51DC4C8");

   if (GetLastError() == ERROR_ALREADY_EXISTS ||
       GetLastError() == ERROR_ACCESS_DENIED)
   {
      return -1;
   }

	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call
// instead to make the EXE free threaded. This means that calls come in on a
// random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// This resolves ATL window thunking problem when Microsoft Layer for
   // Unicode (MSLU) is used.
	::DefWindowProc(NULL, 0, 0, 0L);

   // add flags to support other controls
	AtlInitCommonControls(ICC_BAR_CLASSES);

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
