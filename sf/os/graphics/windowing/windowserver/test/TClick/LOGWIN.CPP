// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Extracted from DEBLOGWN.CPP
// Code to run the WIN32 windows with the output
// 
//

#include <e32std.h>
#include <e32base.h>
#include <emulator.h>
#include "LOGWIN.H"

GLDEF_D struct TSharedMemory GSharedMemory;
//GLDEF_D struct HWND__ *Hwnd;
//LOCAL_D RSemaphore Sem;

const TUint KHeapSize=0x8000;


TInt numLines(const CArrayVarSeg<TText> &aTextArray, TBool aOutOfMemory)
	{
	return (TInt)(aOutOfMemory)?
				aTextArray.Count():
				aTextArray.Count()-1;
	}

TInt numVisibleLines(TInt aHeight, TInt aTextHeight)
	{
	return aHeight/aTextHeight;
	}

TInt32 __stdcall WndProc(struct HWND__ *aHwnd, TUint aMessage, TUint wParam, TInt32 lParam)
    {
    HDC hdc;
    PAINTSTRUCT ps;
	HFONT hfont;
    RECT rect;
	TInt i,
		paintMin,
		paintMax;
	static LOGFONT logFont;
	static TEXTMETRIC tm;
	static TInt textHeight,
		width,
		height,
		scrollMin=0,
		scrollMax,
		numLinesAbove=scrollMin,
		prevNumLinesAbove;
	static CArrayVarSeg<TText> *pmsg;
	static TBool outOfMemory=EFalse;

    switch (aMessage)
        {
    case WM_CREATE:
		hdc=GetDC(aHwnd);

		pmsg=new(ELeave) CArrayVarSeg<TText>(20);
		{
		TBuf<0x20> errorMsg;
		errorMsg=_L("ERROR: out of memory");
		errorMsg.ZeroTerminate();
		TRAPD(err,pmsg->AppendL(*errorMsg.Ptr(), (errorMsg.Length()+1)*sizeof(TText)));
		if (err!=KErrNone)
			return(err);
		}
		GetTextMetrics(hdc, &tm);
		textHeight=tm.tmHeight+tm.tmExternalLeading;
        GetClientRect(aHwnd, &rect);
		width=rect.right;
		height=rect.bottom;
		scrollMax=numLines(*pmsg, outOfMemory);
		SetScrollPos(aHwnd, SB_VERT, numLinesAbove, TRUE);
		SetScrollRange(aHwnd, SB_VERT, scrollMin, scrollMax, FALSE);
		logFont.lfHeight=8;
		wsprintf(logFont.lfFaceName, (LPCTSTR)_S("courier"));

		ReleaseDC(aHwnd, hdc);
		return 0;
	case WM_USER+EAppendText:
		{
		TRAPD(err,pmsg->InsertL(pmsg->Count()-1, *(TText *)lParam, wParam));
		if (err!=KErrNone)
			{
			pmsg->Delete(0, Max(0, (TInt)pmsg->Count()-1));
			numLinesAbove=scrollMax=scrollMin;
			SetScrollPos(aHwnd, SB_VERT, numLinesAbove, TRUE);
			SetScrollRange(aHwnd, SB_VERT, scrollMin, scrollMax, FALSE);
			hdc=GetDC(aHwnd);
			PatBlt(hdc, 0, 0, width, height, WHITENESS);
			ReleaseDC(aHwnd, hdc);
			return(err);
			}
		// adjust the scroll position so that the last line added is visible
		if (numLinesAbove<numLines(*pmsg, outOfMemory)-numVisibleLines(height, textHeight))
			{
			prevNumLinesAbove=numLinesAbove;
			numLinesAbove=numLines(*pmsg, outOfMemory)-numVisibleLines(height, textHeight);
			scrollMax=numLines(*pmsg, outOfMemory);
			SendMessage(aHwnd, WM_USER+EScrollToPos, 0, 0L);
			}
		else
			{
			hdc=GetDC(aHwnd);
			hfont=(HFONT)SelectObject(hdc, CreateFontIndirect(&logFont));
			TText *text=&((*pmsg)[scrollMax]);
			TextOut(hdc, 0, (scrollMax-numLinesAbove)*textHeight, (LPCTSTR)text, User::StringLength(text));
			scrollMax=numLines(*pmsg, outOfMemory);
			ReleaseDC(aHwnd, hdc);
			}
		return KErrNone;
		}
	case WM_USER+EScrollToPos:
		SetScrollPos(aHwnd, SB_VERT, numLinesAbove, TRUE);
		SetScrollRange(aHwnd, SB_VERT, scrollMin, scrollMax, FALSE);

		hdc=GetDC(aHwnd);
		hfont=(HFONT)SelectObject(hdc, CreateFontIndirect(&logFont));

		if (numLinesAbove>prevNumLinesAbove)
		// scrolling towards end, therefore text moves up on screen
			{
			TInt numLinesToDraw=numLinesAbove-prevNumLinesAbove;
			TInt numLinesToBlt=numVisibleLines(height, textHeight)-numLinesToDraw;
			if (numLinesToBlt>0)
				ScrollWindow(aHwnd, 0, -numLinesToDraw*textHeight, NULL, NULL);
			PatBlt(hdc, 0, numLinesToBlt*textHeight, width, numLinesToDraw*textHeight, WHITENESS);
			paintMin=Max(Min(numLinesAbove+numLinesToBlt, scrollMax), scrollMin);
			paintMax=Min(paintMin+numLinesToDraw, scrollMax);
			for (i=paintMin; i<paintMax; i++)
				{
				TText *text=&((*pmsg)[i]);
				TextOut(hdc, 0, (i-numLinesAbove)*textHeight, (LPCTSTR)text, User::StringLength(text));
				}
			}
		else
		// scrolling towards beginning, therefore text moves down on screen
			{
			TInt numLinesToDraw=prevNumLinesAbove-numLinesAbove;
			TInt numLinesToBlt=numVisibleLines(height, textHeight)-numLinesToDraw;
			if (numLinesToBlt>0)
				ScrollWindow(aHwnd, 0, numLinesToDraw*textHeight, NULL, NULL);
			PatBlt(hdc, 0, 0, width, numLinesToDraw*textHeight, WHITENESS);
			paintMin=Max(Min(numLinesAbove, scrollMax), scrollMin);
			paintMax=Min(paintMin+numLinesToDraw, scrollMax);
			for (i=paintMin; i<paintMax; i++)
				{
				TText *text=&((*pmsg)[i]);
				TextOut(hdc, 0, (i-numLinesAbove)*textHeight, (LPCTSTR)text, User::StringLength(text));
				}
			}

		DeleteObject(hfont);
		ReleaseDC(aHwnd, hdc);
		return 0;
    case WM_PAINT:
		hdc=BeginPaint(aHwnd, &ps);
		hfont=(HFONT)SelectObject(hdc, CreateFontIndirect(&logFont));

		paintMin=Max(scrollMin, numLinesAbove);
		paintMax=Min(numLines(*pmsg, outOfMemory), numLinesAbove+numVisibleLines(height, textHeight));
		for (i=paintMin; i<paintMax; i++)
				{
				TText *text=&((*pmsg)[i]);
				TextOut(hdc, 0, (i-numLinesAbove)*textHeight, (LPCTSTR)text, User::StringLength(text));
				}

		DeleteObject(hfont);
		EndPaint(aHwnd, &ps);
        return 0;
    case WM_SIZE:
        width=LOWORD(lParam);
        height=HIWORD(lParam);
		return 0;
    case WM_VSCROLL:
		prevNumLinesAbove=numLinesAbove;
        switch (LOWORD(wParam))
			{
		case SB_TOP:
			numLinesAbove=scrollMin;
			break;
		case SB_BOTTOM:
			numLinesAbove=scrollMax;
			break;
		case SB_LINEUP:
			numLinesAbove--;
			break;
		case SB_LINEDOWN:
			numLinesAbove++;
			break;
		case SB_PAGEUP:
			numLinesAbove-=numVisibleLines(height, textHeight);
			break;
		case SB_PAGEDOWN:
			numLinesAbove+=numVisibleLines(height, textHeight);
			break;
		case SB_THUMBTRACK:
			numLinesAbove=HIWORD(wParam);
			break;
			}
		numLinesAbove=max(scrollMin, min(numLinesAbove, scrollMax));

		if (numLinesAbove!=prevNumLinesAbove)
			SendMessage(aHwnd, WM_USER+EScrollToPos, 0, 0L);
		return 0;
    case WM_KEYDOWN:
		switch (wParam)
			{
		case VK_HOME:
			SendMessage(aHwnd, WM_VSCROLL, SB_TOP, 0L);
			break;
		case VK_END:
			SendMessage(aHwnd, WM_VSCROLL, SB_BOTTOM, 0L);
			break;
		case VK_PRIOR:
			SendMessage(aHwnd, WM_VSCROLL, SB_PAGEUP, 0L);
			break;
		case VK_NEXT:
			SendMessage(aHwnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
			break;
		case VK_UP:
			SendMessage(aHwnd, WM_VSCROLL, SB_LINEUP, 0L);
			break;
		case VK_DOWN:
			SendMessage(aHwnd, WM_VSCROLL, SB_LINEDOWN, 0L);
			break;
			}
		return 0;
	case WM_DESTROY:
		delete pmsg;
		PostQuitMessage(0);
		return 0;
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
	//wndclass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hbrBackground=(HBRUSH)GetSysColorBrush(COLOR_BTNFACE);
    wndclass.lpszMenuName=NULL;
    wndclass.lpszClassName=(LPCTSTR)szAppName;

    RegisterClass(&wndclass);

	GSharedMemory.iHwnd=CreateWindow((LPCTSTR)szAppName,
                    (LPCTSTR)szAppName,
					WS_OVERLAPPEDWINDOW,
					//WS_OVERLAPPEDWINDOW|WS_VSCROLL|WS_MINIMIZE,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    NULL,
                    NULL,
                    NULL,
                    NULL);

	ShowWindow(GSharedMemory.iHwnd, SW_SHOWMINNOACTIVE);
	//ShowWindow(GSharedMemory.iHwnd, SW_MINIMIZE);
	UpdateWindow(GSharedMemory.iHwnd);
	GSharedMemory.iStartSemaphore.Signal(); // allows logging to start now that the window, etc. has been set up
	Emulator::Escape();
    while (GetMessage(&msg, NULL, 0, 0))
        {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }
	Emulator::Reenter();
    return msg.wParam;
    }

GLDEF_C void CreateLogWinThreadL()
	{
	_LIT(KLogWin,"LogingWindow");
	RThread wsThread;
	GSharedMemory.iStartSemaphore.CreateLocal(0);
	User::LeaveIfError(wsThread.Create(KLogWin,logWinMain,KDefaultStackSize,KHeapSize,KHeapSize,NULL));
	wsThread.Resume();
	GSharedMemory.iStartSemaphore.Wait();
	}
