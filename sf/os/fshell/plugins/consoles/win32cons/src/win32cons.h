// win32cons.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __win32cons_h__
#define __win32cons_h__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <e32keys.h>

class TWin32Console
	{
public:	
	/**
	Return non-zero on success.
	*/
	int AttachConsole();
	void FreeConsole();
	int Write(const char* aText, int aLen);
	int Write(const WCHAR* aText, int aLen);
	int WriteStdErr(const char* aText, int aLen);
	int WriteStdErr(const WCHAR* aText, int aLen);
	int Read(void* buf, int len, int& bytesRead);
	int ReadKey(TKeyCode& aCode, TUint& aModifiers);
	int GetCursorPos(int& aX, int& aY) const;
	int SetCursorPos(int aX, int aY);
	int SetCursorPosRel(int aX, int aY);
	int SetCursorHeight(int aPercentage);
	int SetTitle(const char* aTitle);
	int ClearScreen();
	int ClearToEndOfLine();
	int GetScreenSize(int& aWidth, int& aHeight) const;

	// Copied from consoleextensions.h
	enum TAttribute
		{
		ENone			= 0x0001,
		EBold			= 0x0002,
		EUnderscore		= 0x0004,
		EBlink			= 0x0008,
		EInverse		= 0x0010,
		EConceal		= 0x0020
		};
	enum TColor
		{
		EBlack,
		ERed,
		EGreen,
		EYellow,
		EBlue,
		EMagenta,
		ECyan,
		EWhite,
		EReset,
		EUnchanged
		};
	int SetAttributes(unsigned aAttributes, TWin32Console::TColor aForegroundColor, TWin32Console::TColor aBackgroundColor);

private:
	int AttachOrAllocConsole();
private:
	bool iHaveConsole;
	void* iStdOutHandle;
	void* iStdErrHandle;
	void* iStdinHandle;
	unsigned long iOldConsMode;
	bool iConsModeSet;
	KEY_EVENT_RECORD iCachedKey;
	WORD iOrigAttributes;
	};
	
void DebugMsg(const char* aMsg, ...);
void DebugMsg(const unsigned short int* aMsg, ...);

#endif //__win32cons_h__


