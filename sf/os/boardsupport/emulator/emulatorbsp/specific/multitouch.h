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
// wins\specific\multitouch.h
// 
//
#ifndef MULTITOUCH_H
#define MULTITOUCH_H

#include <e32err.h>
#include <emulator.h>
#include <commctrl.h>
#include "multitouchwindowsheader.h"

#ifndef WM_INPUT
#define WM_INPUT 0x00ff
#endif

/**
 * The main class for Multitouch
*/
class DMultiTouch
	{
public:
	DMultiTouch(TInt aProximityStep, TInt aPressureStep);
	void OnWmInput(HWND aHWnd,TUint aMessage,TUint aWParam,TUint aLParam,HWND aParentHwnd);
	void ShowPrimaryCursor(HWND aHWnd);
	static bool Init();
	static void ShowCursors();
	static void HideCursors();
	BOOL Register();
	BOOL UnRegister();

public:
	static int iNumberOfMice;
	static bool iMultiTouchSupported;
	static bool iMultiTouchCreated;
	static bool iMultiTouchTempEnabled;
	int iZMaxRange;
	int iPressureStep;
	int iProximityStep;
	int iMaxPressure;	
	};

extern DMultiTouch* TheMultiTouch;
extern void MultiTouchWndPointer(TUint message,TInt aXpos,TInt aYpos, TInt aPointerId, TInt aZ);

/**
 * The class of the Cursor Window
 */
class CursorWindow
{
public:
	CursorWindow();
	HWND Create(HMODULE hm, HWND hwndParent, int number);
	BOOL SetCursor(HCURSOR hc);
	void GetPosition(POINT& pt);
	void SetPosition(POINT& pt);
	void Show();
	void Hide();

	HWND iHwnd;
	int iNumber;
	HCURSOR iCursor;
	POINT iHotspot;
};

/**
 * The class encapsulating each mouse device
 */
class DMultiMouse
	{
public:
	DMultiMouse();
	static TInt Add(RAWINPUTDEVICELIST& aDev);
	static DMultiMouse* Find(HANDLE aHandle);
	void HandleRawMouseEvent(RAWMOUSE& aEvent, HWND aWnd);
	void CreateCursor(LPVOID aArg, HWND aWnd);
	enum
		{KMaxMice = 8};

private:
	void CorrectSystemMouse();
	void ShowMousePos(HWND aHWnd);

private:
	int iX;
	int iY;
	int iZ;
	HANDLE iDevice;
	int iId;

public:
	bool iIsPrimary;
	CursorWindow iCursorWnd;

	static DMultiMouse iMice[KMaxMice];
	static int iNumMice;
	static DMultiMouse* iPrimary;
	static int iMouseId;
	};

#endif
