// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// code for the Windows-dependent derived class 
// 
//

#include "../SERVER/w32cmd.h"
#include <emulator.h>

#include "_windows.h"

#include "DEBLOGWN.H"


// static container for sharing data between Symbian & Windows threads
struct TSharedMemory
	{
	RSemaphore		iStartSemaphore;
	struct HWND__*	iHwnd;
	};

static struct TSharedMemory GSharedMemory;


enum TWindowsUserMessage
	{
	EAppendText16,
	EAppendText8,
	};

// Constants
const int KMaxLogLines = 1000;
const int KAverageLogLineLength = 60;
const int KWinTimerId = 1;


EXPORT_C CDebugLogBase *CreateDebugLog(TBool aIsFirst, TDesC &aParams)
	{
	CDebugLogWin *device=new(ELeave) CDebugLogWin();
	CDebugLog *log=NULL;
	TRAPD(err,log=new(ELeave) CDebugLog(device));
	if (err!=KErrNone)
		{
		delete device;
		User::Leave(err);
		}
	TRAP(err,log->ConstructL(aIsFirst, aParams));
	if (err!=KErrNone)
		{
		delete log;
		User::Leave(err);
		}
	return(log);
	}

CDebugLogWin::CDebugLogWin() :iThreadCreated(EFalse)
	{}

CDebugLogWin::~CDebugLogWin()
	{
	if (iThreadCreated)
		{
		if (GSharedMemory.iHwnd)
			{
			PostMessage(GSharedMemory.iHwnd, WM_CLOSE, 0, 0);
			}
		iThread.Close();
		iThreadCreated = 0;
		GSharedMemory.iStartSemaphore.Close();
		}
	}

void CDebugLogWin::ConstructL(TBool , TDesC &)
	{
	_LIT(KLog,"DebugLog");
	GSharedMemory.iStartSemaphore.CreateLocal(0);
	User::LeaveIfError(iThread.Create(KLog,logWinMain,KDefaultStackSize,KHeapSize,KHeapSize,(TAny *)123));
	iThreadCreated=ETrue;
	iThread.Resume();
	GSharedMemory.iStartSemaphore.Wait();
	}

void CDebugLogWin::WriteToLogL(const TDesC &aDes, const TDesC &aDes2)
	{
	TBuf<LogTBufSize*2+1> bufPlusZero;
	bufPlusZero.Copy(iTextBuf);
	bufPlusZero.Append(aDes);
	bufPlusZero.ZeroTerminate();
	TInt32 bufferAddr = (TInt32)(bufPlusZero.Ptr());
	// synchronously transfer string to debug window
	Emulator::Escape();
	SendMessage(GSharedMemory.iHwnd, WM_USER + EAppendText16, 0, bufferAddr);
	Emulator::Reenter();
	iTextBuf.Copy(aDes2);
	}

void CDebugLogWin::WriteToLog8L(const TDesC8 &aDes, const TDesC8 &aDes2)
	{
	TBuf8<LogTBufSize*2+1> bufPlusZero;
	bufPlusZero.Copy(iTextBuf);
	bufPlusZero.Append(aDes);
	bufPlusZero.ZeroTerminate();
	TInt32 bufferAddr = (TInt32)(bufPlusZero.Ptr());
	// synchronously transfer string to debug window
	Emulator::Escape();
	SendMessage(GSharedMemory.iHwnd, WM_USER + EAppendText8, 0, bufferAddr);
	Emulator::Reenter();
	iTextBuf.Copy(aDes2);
	}

TInt32 __stdcall WndProc(struct HWND__ *aHwnd, TUint aMessage,
						TUint wParam, TInt32 lParam)
	{
	static HWND hWndListBox;
	static HFONT hfont;
	static TBool timerSet = EFalse;

	switch (aMessage)
		{
	case WM_CREATE:
			{ // Disable Close menu option
			HMENU menu = GetSystemMenu(aHwnd, FALSE);
			EnableMenuItem(menu, SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

			// create fixed pitch font for debug log
			LOGFONT lf;
			if (GetObject(hfont, sizeof(lf), &lf))
				{
				lf.lfPitchAndFamily = FIXED_PITCH;
				lstrcpy(lf.lfFaceName, L"courier");
				hfont = CreateFontIndirect(&lf);
				}
			else
				{
				hfont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
						OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, L"courier");
				}

			HINSTANCE hinstance = ((LPCREATESTRUCT) lParam) -> hInstance;
			SendMessage(aHwnd, WM_SETFONT, (WPARAM) (hfont), 0);
			RECT clientRect;
			GetClientRect(aHwnd, &clientRect);

			hWndListBox = CreateWindowEx(WS_EX_CLIENTEDGE,
										L"listbox",
										NULL,
										WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT,
										0, 0,
										clientRect.right, clientRect.bottom,
										aHwnd, (HMENU) 1, hinstance, NULL);
			SendMessage(hWndListBox, WM_SETFONT, (WPARAM) (hfont), 0);

			// preallocate string memory
			SendMessage(hWndListBox, LB_INITSTORAGE, KMaxLogLines + 20, KAverageLogLineLength);
			}
		return 0;

	case WM_USER+EAppendText16:
			{ // send wide char string to ListBox
			int numRows = SendMessageW(hWndListBox, LB_ADDSTRING, 0, lParam);
			// if too many lines set a timer to delete some lines
			if (!timerSet && (numRows > KMaxLogLines) )
				{ // set a timer for 2s
				SetTimer(aHwnd, KWinTimerId, 2000, NULL);
				timerSet = ETrue;
				}

			// scroll ListBox so that newest line is visible
			SendMessage(hWndListBox, LB_SETTOPINDEX, numRows, 0);
			}
		return KErrNone;

	case WM_USER+EAppendText8:
			{ // send narrow character string to ListBox
			int numRows = SendMessageA(hWndListBox, LB_ADDSTRING, 0, lParam);
			// if too many lines set a timer to delete some lines
			if (!timerSet && (numRows > KMaxLogLines) )
				{ // set a timer for 2s
				SetTimer(aHwnd, KWinTimerId, 2000, NULL);
				timerSet = ETrue;
				}
		
			// scroll ListBox so that newest line is visible
			SendMessage(hWndListBox, LB_SETTOPINDEX, numRows, 0);
			}
		return KErrNone;

	case WM_TIMER:
			{ // too many rows in listbox, release some memory
			TInt numRows;	
			do
				{
				numRows = SendMessage(hWndListBox, LB_DELETESTRING, 0, 0);
				}
				while (numRows > KMaxLogLines);
			KillTimer(aHwnd, KWinTimerId);
			timerSet = EFalse;

			// ensure newest line is visible (delete moves focus to line 0)
			SendMessage(hWndListBox, LB_SETTOPINDEX, numRows-1, 0);
			}
		break;

	case WM_SIZE:
		// forward to the ListBox, and make it repaint
		if ( (wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED) )
			{
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			MoveWindow(hWndListBox, 0, 0, width, height, TRUE);
			}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		DeleteObject(hfont);
		return 0;

	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
			{ // do not allow window to be closed with ALT-F4 (this would close the emulator)
			return 1;
			}
		break;
		}

	return DefWindowProc(aHwnd, aMessage, wParam, lParam);
	}

TInt logWinMain(TAny *)
	{
	MSG msg;
	WNDCLASS wndclass;
	const TText *szAppName=_S("Window Server Log");

	wndclass.style=CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc=WndProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance=NULL;
	wndclass.hIcon=LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor=LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground=(HBRUSH)GetSysColorBrush(COLOR_BTNFACE);
	wndclass.lpszMenuName=NULL;
	wndclass.lpszClassName=(LPCTSTR)szAppName;

	RegisterClass(&wndclass);

	GSharedMemory.iHwnd = CreateWindow((LPCTSTR)szAppName,
					(LPCTSTR)szAppName,
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					NULL,
					NULL,
					NULL,
					NULL);

	ShowWindow(GSharedMemory.iHwnd, SW_SHOWMINNOACTIVE);

	GSharedMemory.iStartSemaphore.Signal(); // allows logging to start now that the window, etc. has been set up

	// Must remove thread from Symbian scheduler before calling blocking Windows APIs (e.g. GetMessage)
	Emulator::Escape();
	while (GetMessage(&msg, NULL, 0, 0))
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
	// return to Symbian Scheduler
	Emulator::Reenter();
	GSharedMemory.iHwnd = NULL;
	return msg.wParam;
	}
