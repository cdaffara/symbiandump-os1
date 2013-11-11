// win32cons.cpp
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

// os_version.h must be included before the windows headers
// to ensure that _WIN32_WINNT has the right value
#include "os_version.h"
#include <stdio.h>
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <emulator.h>
#include "win32cons.h"
#include "keymappings.h"

//#define DEBUG_CONSOLE

#ifdef DEBUG_CONSOLE
#define CONS_DEBUG(x) x
#else
#define CONS_DEBUG(x)
#endif

inline int Min(int a, int b) {return a<b ? a : b;}
inline int Max(int a, int b) {return a<b ? b : a;}

#define WIN32_START() Emulator::Lock()
#define WIN32_END() Emulator::Unlock()

#define WIN32_CALL(x) { WIN32_START(); x; WIN32_END(); }

int TWin32Console::AttachOrAllocConsole()
	{
	// NB The AttachConsole method is only available on WinXP and later
	WIN32_START();
#if _WIN32_WINNT >= 0x0501
	if (!::AttachConsole(-1)) 
		{
		// no luck
		int err = GetLastError();
		if (err == ERROR_ACCESS_DENIED)
			{
			WIN32_END();
			// we're already attached to a console (?) - continue.
			}
		else if (err == ERROR_INVALID_HANDLE)
			{
#endif
			if (!AllocConsole())
				{
				WIN32_END();
				return 0; //failed
				}
			WIN32_END();
			CONS_DEBUG(DebugMsg("Allocated console"));
#if _WIN32_WINNT >= 0x0501
			}
		else
			{
			WIN32_END();
			CONS_DEBUG(DebugMsg("Failed to attach to console, error %d", err));
			return 0;
			}
		}
	else
		{
		int err = GetLastError();
		WIN32_END();
		CONS_DEBUG(DebugMsg("Attached console err %d", err));
		}
#endif
	
	iHaveConsole = true;
	return 1;
	}
	
bool TestHandle(void* aHandle, int* aError = NULL)
	{
	WIN32_START();
	DWORD written;
	bool r = WriteFile(aHandle, "", 0, &written, NULL);
	if (aError) *aError = GetLastError();
	WIN32_END();
	return r;
	}

int TWin32Console::AttachConsole()
	{
	iHaveConsole = false;
	// first, test the existing STDOUT and STDERR handles; if out output is being redirected to 
	// a file, these will be valid as they stand.
	WIN32_START();
	iStdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	iStdErrHandle = GetStdHandle(STD_ERROR_HANDLE);
	
	// try writing to stdout to see if it's valid
	int err;
	bool ok = TestHandle(iStdOutHandle, &err);
	WIN32_END();
	
	if (!ok && err == ERROR_INVALID_HANDLE)
		{
		// no redirection, so existing handle doesn't work. Attach to an existing console (if it exists)
		// or create a new one (this will happen if we're invoked from the IDE, for example)
		CONS_DEBUG(DebugMsg("Failed to write to existing STDOUT"));
		err = AttachOrAllocConsole();
		if (err==0) return err;
		WIN32_CALL(iStdOutHandle =  GetStdHandle(STD_OUTPUT_HANDLE));
		}
	else if (!ok)
		{
		// some other error occurred, fail.
		CONS_DEBUG(DebugMsg("Failed to write to existing STDOUT; error %d", err));
		return 0;
		}
	else
		{
		CONS_DEBUG(DebugMsg("Using existing STDOUT"));
		}
		
	// try writing to stderr to see if it's valid
	ok = TestHandle(iStdErrHandle, &err);
	if (!ok && err == ERROR_INVALID_HANDLE)
		{
		CONS_DEBUG(DebugMsg("Failed to write to existing STDERR"));
		if (!iHaveConsole)
			{
			err = AttachOrAllocConsole();
			if (err==0) return err;
			}
		else
			{
			CONS_DEBUG(DebugMsg("Already attached/alloc'd console"));
			}
		WIN32_CALL(iStdErrHandle = GetStdHandle(STD_ERROR_HANDLE));
		CONS_DEBUG(DebugMsg("GetStdHandle(Stderr)=%d err=%d", iStdErrHandle, GetLastError()));
		}
	else if (!ok)
		{
		CONS_DEBUG(DebugMsg("Failed to write to existing STDERR; error %d", err));
		return 0;
		}
	else
		{
		CONS_DEBUG(DebugMsg("Using existing STDERR"));
		}
		
	// in some cases (such as when we're invoked by GNU make) the handle returned by GetStdHandle
	// after attaching to a console doesn't work. In this case, open a new handle to the console
	// using CreateFile
	TInt lastError;
	if (!TestHandle(iStdOutHandle, &lastError))
		{
		CONS_DEBUG(DebugMsg("Still can't write STDOUT; err %d; opening new console handle", lastError));
		
		WIN32_START();
		iHaveConsole = false;
		iStdOutHandle = CreateFile(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		int err = GetLastError();
		WIN32_END();
		CONS_DEBUG(DebugMsg("CreateFile handle = %d, err %d", iStdOutHandle, err));
		if (!TestHandle(iStdOutHandle, &err))
			{
			CONS_DEBUG(DebugMsg("Still can't write STDOUT; err %d; giving up", err));
			return 0;
			}
		}
	
	if (!TestHandle(iStdErrHandle, &lastError))
		{
		CONS_DEBUG(DebugMsg("Still can't write STDOUT; err %d; opening new console handle", GetLastError()));
		
		WIN32_START();
		iHaveConsole = false;
		iStdErrHandle = CreateFile(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		int err = GetLastError();
		WIN32_END();
		
		CONS_DEBUG(DebugMsg("CreateFile handle = %d, err %d", iStdErrHandle, err));
		if (!TestHandle(iStdErrHandle, &err))
			{
			CONS_DEBUG(DebugMsg("Still can't write STDERR; err %d; giving up", err));
			return 0;
			}
		}
		
	//iStdinHandle = CreateFile(L"CONIN$", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	WIN32_START();
	iConsModeSet=false;
	iStdinHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (GetConsoleMode(iStdinHandle, &iOldConsMode))
		{
		DWORD consmode = iOldConsMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
		if (SetConsoleMode(iStdinHandle, consmode))
			{
			iConsModeSet=true;
			}
		}
	iOrigAttributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (GetConsoleScreenBufferInfo(iStdOutHandle, &info))
		{
		iOrigAttributes = info.wAttributes;
		}
	WIN32_END();
		
	return 1;
	}
	
void TWin32Console::FreeConsole()
	{
	WIN32_START();
	if (iConsModeSet)
		{
		SetConsoleMode(iStdinHandle, iOldConsMode);
		}
	::FreeConsole();
	WIN32_END();
	}
	
int TWin32Console::Write(const char* aText, int aLen)
	{
	DWORD written;
	WIN32_START();
	bool b = WriteFile(iStdOutHandle, (const void*)aText, aLen, &written, NULL);
	int err = GetLastError();
	WIN32_END();
	//DebugMsg("WriteFile(STDOUT)=%d, err=%d", b, err);
	return b ? 0 : (err>0 ? -err  : err);
	}

int TWin32Console::Write(const WCHAR* aText, int aLen)
	{
	if (iHaveConsole)
		{
		DWORD written;
		WIN32_START();
		bool b = WriteConsole(iStdOutHandle, (void*)aText, aLen, &written, NULL);
		int err = GetLastError();
		WIN32_END();
		//DebugMsg("WriteFile(STDOUT)=%d, err=%d", b, err);
		return b ? 0 : (err>0 ? -err  : err);
		}
	else
		{
		const int KBufLen = 256;
		char outbuf[KBufLen];
		int pos = 0;
		while (pos < aLen)
			{
			int len = Min(KBufLen, aLen - pos);
			int narrowLen = WideCharToMultiByte(CP_ACP, 0, aText, len, outbuf, KBufLen, NULL, NULL);
			if (narrowLen == 0 && aLen != 0) return 0;
			int ok = Write(outbuf, narrowLen);
			if (!ok) return ok;
			}
		return 1;
		}
	}
	
int TWin32Console::WriteStdErr(const char* aText, int aLen)
	{
	DWORD written;
	WIN32_START();
	bool b = WriteFile(iStdErrHandle, (const void*)aText, aLen, &written, NULL);
	int err = GetLastError();
	WIN32_END();
	//DebugMsg("WriteFile(STDERR)=%d, err=%d", b, err);
	return b ? 0 : (err>0 ? -err  : err);
	}

int TWin32Console::WriteStdErr(const WCHAR* aText, int aLen)
	{
	if (iHaveConsole)
		{
		DWORD written;
		WIN32_START();
		bool b = WriteConsole(iStdErrHandle, (void*)aText, aLen, &written, NULL);
		int err = GetLastError();
		WIN32_END();
		//DebugMsg("WriteFile(STDOUT)=%d, err=%d", b, err);
		return b ? 0 : (err>0 ? -err  : err);
		}
	else
		{
		const int KBufLen = 256;
		char outbuf[KBufLen];
		int pos = 0;
		while (pos < aLen)
			{
			int len = Min(KBufLen, aLen - pos);
			int narrowLen = WideCharToMultiByte(CP_ACP, 0, aText, len, outbuf, KBufLen, NULL, NULL);
			if (narrowLen == 0 && aLen != 0) return 0;
			int ok = WriteStdErr(outbuf, narrowLen);
			if (!ok) return ok;
			}
		return 1;
		}
	}
	
int TWin32Console::Read(void* buf, int len, int& bytesRead)
	{
	DWORD read;

	/*
	Read is a blocking call. We don't want to use WIN32_START() (Emulator::Lock()) as this 
	will cause all other emulator threads to block until the Read call returns. Instead,
	we escape the emulator which removes this thread from the usual emulator thraed 
	scheduling until we re-enter the emulator when ReadFile returns. This allows other
	emulator threads to run concurrently, even though (from the emulators thread shedulers
	point of view) this thread is still running.
	*/
	Emulator::Escape(); 
	bool b = ReadFile(iStdinHandle, buf, len, &read, NULL);
	Emulator::Reenter();
	
	bytesRead = read;
	return b ? 0 : -2; // TODO better error code?
	}
	
int TWin32Console::ReadKey(TKeyCode& aCode, TUint& aModifiers)
	{
	bool ok = true;
	if (iCachedKey.wRepeatCount == 0)
		{
		Emulator::Escape();
		// read a recognised key event:
		do
			{
			// read a key down event:
			do
				{
				DWORD read;
				INPUT_RECORD inp;
				// read a key event:
				do 
					{
					// read a console event:
					do 
						{
						ok = ReadConsoleInput(iStdinHandle, &inp, 1, &read);
						} while (ok && (read==0));
					} while (ok && (inp.EventType != KEY_EVENT));
				
				if (ok)
					{
					iCachedKey = inp.Event.KeyEvent;
					}
				} while (ok && !iCachedKey.bKeyDown);
			aCode = (TKeyCode)iCachedKey.uChar.AsciiChar;
			if (aCode==0) aCode = GetSymbianKeyCode(iCachedKey.wVirtualKeyCode);
			aModifiers = GetSymbianModifiers(iCachedKey.dwControlKeyState);
			
			} while (ok && (aCode==0));

		Emulator::Reenter();
			
		}
	if (!ok) return -2;
	
	CONS_DEBUG(char ch = iCachedKey.uChar.AsciiChar;
	if (!ch) ch=' ';
	DebugMsg("KeyPress %02x ('%c') down:%d keycode:%02x scancode:%02x controlkey:%02x", 
		iCachedKey.uChar.AsciiChar, ch, iCachedKey.bKeyDown, iCachedKey.wVirtualKeyCode, iCachedKey.wVirtualScanCode, iCachedKey.dwControlKeyState));
		
	if (iCachedKey.wRepeatCount>0) --iCachedKey.wRepeatCount;
	
	return 0;
	}

	
int TWin32Console::GetCursorPos(int& aX, int& aY) const
	{
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	int r;
	WIN32_CALL(r = GetConsoleScreenBufferInfo(iStdOutHandle, &sbInfo));
	if (r)
		{
		aX = sbInfo.dwCursorPosition.X;
		aY = sbInfo.dwCursorPosition.Y;
		// make sure it's not off the screen (bottom right)
		if (aX > sbInfo.srWindow.Right) aX = sbInfo.srWindow.Left;
		if (aY > sbInfo.srWindow.Bottom) aY = sbInfo.srWindow.Bottom;
		// make cursor pos relative to display window
		aX -= sbInfo.srWindow.Left;
		aY -= sbInfo.srWindow.Top;
		// make sure it's not off the screen (top left)
		if (aX < 0) aX = 0;
		if (aY < 0) aY = 0;
		}
	else
		{
		aX = aY = 0;
		}
	return r;
	}
	
int TWin32Console::SetCursorPos(int aX, int aY)
	{
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	int r;
	WIN32_CALL(r = GetConsoleScreenBufferInfo(iStdOutHandle, &sbInfo));
	if (r)
		{
		// make position relative to display window
		aX += sbInfo.srWindow.Left;
		aY += sbInfo.srWindow.Top;
		// make sure it's within the screen buffer
		if (aX > sbInfo.dwSize.X) aX = sbInfo.dwSize.X;
		if (aY > sbInfo.dwSize.Y) aY = sbInfo.dwSize.Y;
		if (aX < 0) aX = 0;
		if (aY < 0) aY = 0;
		COORD pos;
		pos.X = aX;
		pos.Y = aY;
		WIN32_CALL(r = SetConsoleCursorPosition(iStdOutHandle, pos));
		}
	return r;
	}
	
int TWin32Console::SetCursorPosRel(int aX, int aY)
	{
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	int r;
	WIN32_CALL(r = GetConsoleScreenBufferInfo(iStdOutHandle, &sbInfo));
	if (r)
		{
		sbInfo.dwCursorPosition.X += aX;
		sbInfo.dwCursorPosition.Y += aY;
		// make sure it's still within screen buffer
		sbInfo.dwCursorPosition.X = Max(0, Min(sbInfo.dwCursorPosition.X, sbInfo.dwSize.X));
		sbInfo.dwCursorPosition.Y = Max(0, Min(sbInfo.dwCursorPosition.Y, sbInfo.dwSize.Y));
		
		WIN32_CALL(r = SetConsoleCursorPosition(iStdOutHandle, sbInfo.dwCursorPosition));
		}
	return r;
	}
		
int TWin32Console::SetCursorHeight(int aPercentage)
	{
	CONSOLE_CURSOR_INFO info;
	info.dwSize = Min(100, Max(1, aPercentage));
	info.bVisible = aPercentage>0;
	int r;
	WIN32_CALL(r = SetConsoleCursorInfo(iStdOutHandle, &info));
	return r;
	}
	
int TWin32Console::SetTitle(const char* aTitle)
	{
	int r;
	WIN32_CALL(r = SetConsoleTitleA(aTitle));
	return r;
	}
	
int TWin32Console::ClearScreen()
	{
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	WIN32_START();
	int r = GetConsoleScreenBufferInfo(iStdOutHandle, &sbInfo);
	COORD topLeft = {0, 0};
	if (r)
		{
		DWORD bufferSize = sbInfo.dwSize.X * sbInfo.dwSize.Y;
		DWORD charsWritten;
		r = FillConsoleOutputCharacter(iStdOutHandle, (TCHAR)' ', bufferSize, topLeft, &charsWritten);
		// Also reset the attributes
		if (r) FillConsoleOutputAttribute(iStdOutHandle, iOrigAttributes, bufferSize, topLeft, &charsWritten);
		}
	if (r)
		{
		// move cursor to top left
		r = SetConsoleCursorPosition(iStdOutHandle, topLeft);
		}
	if (r)
		{
		// move scroll region to top left
		sbInfo.srWindow.Left -= sbInfo.srWindow.Right;
		sbInfo.srWindow.Right -= sbInfo.srWindow.Right;
		sbInfo.srWindow.Bottom -= sbInfo.srWindow.Top;
		sbInfo.srWindow.Top -= sbInfo.srWindow.Top;
		r = SetConsoleWindowInfo(iStdOutHandle, true, &sbInfo.srWindow);
		}
	WIN32_END();
	return r;
	}
	
int TWin32Console::ClearToEndOfLine()
	{
	WIN32_START();
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	int r = GetConsoleScreenBufferInfo(iStdOutHandle, &sbInfo);
	if (r)
		{
		SHORT len = sbInfo.dwSize.X - sbInfo.dwCursorPosition.X;
		DWORD charsWritten;
		r = FillConsoleOutputCharacter(iStdOutHandle, (TCHAR)' ', len, sbInfo.dwCursorPosition, &charsWritten);
		// Also reset the attributes
		if (r) FillConsoleOutputAttribute(iStdOutHandle, iOrigAttributes, len, sbInfo.dwCursorPosition, &charsWritten);
		}
	WIN32_END();
	return r;	
	}
	
int TWin32Console::GetScreenSize(int& aWidth, int& aHeight) const
	{
	CONSOLE_SCREEN_BUFFER_INFO sbInfo;
	int r;
	WIN32_CALL(r = GetConsoleScreenBufferInfo(iStdOutHandle, &sbInfo));
	if (r)
		{
		aWidth  = sbInfo.srWindow.Right - sbInfo.srWindow.Left + 1;
		aHeight = sbInfo.srWindow.Bottom - sbInfo.srWindow.Top + 1;
		}
	else
		{
		// not much else we can do?
		aWidth = 80;
		aHeight = 25; // [Shouldn't this be 24? -TomS]
		}
	return r;
	}
	
int TWin32Console::SetAttributes(unsigned aAttributes, TWin32Console::TColor aForegroundColor, TWin32Console::TColor aBackgroundColor)
	{
	WORD attrib;
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (aAttributes & ENone)
		{
		// None means reset
		attrib = iOrigAttributes;
		}
	else
		{
		BOOL ok;
		WIN32_CALL(ok = GetConsoleScreenBufferInfo(iStdOutHandle, &info));
		if (!ok) return ok;
		attrib = info.wAttributes;
		}
	
	if (aForegroundColor != EUnchanged) attrib &= ~0x0F; // Clear the fg bits in preparation for the switch below
	if (aBackgroundColor != EUnchanged) attrib &= ~0xF0; // Clear the bg bits in preparation for the switch below

	switch (aForegroundColor)
		{
		case EBlack:
			break;
		case ERed:
			attrib |= FOREGROUND_RED; break;
		case EGreen:
			attrib |= FOREGROUND_GREEN; break;
		case EYellow:
			attrib |= FOREGROUND_RED | FOREGROUND_GREEN; break;
		case EBlue:
			attrib |= FOREGROUND_BLUE; break;
		case EMagenta:
			attrib |= FOREGROUND_RED | FOREGROUND_BLUE; break;
		case ECyan:
			attrib |= FOREGROUND_BLUE | FOREGROUND_GREEN; break;
		case EWhite:
			attrib |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN; break;
		default:
			break;
		}
	switch (aBackgroundColor)
		{
		case EBlack:
			break;
		case ERed:
			attrib |= BACKGROUND_RED; break;
		case EGreen:
			attrib |= BACKGROUND_GREEN; break;
		case EYellow:
			attrib |= BACKGROUND_RED | BACKGROUND_GREEN; break;
		case EBlue:
			attrib |= BACKGROUND_BLUE; break;
		case EMagenta:
			attrib |= BACKGROUND_RED | BACKGROUND_BLUE; break;
		case ECyan:
			attrib |= BACKGROUND_BLUE | BACKGROUND_GREEN; break;
		case EWhite:
			attrib |= BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN; break;
		default:
			break;
		}
	if (aAttributes & EBold) attrib |= FOREGROUND_INTENSITY;
	if (aAttributes & EInverse)
		{
		// COMMON_LVB_REVERSE_VIDEO doesn't seem to be supported, so swap foreground and background manually
		attrib = ((attrib & 0xF) << 4) | ((attrib & 0xF0) >> 4);
		}
	//if (aAttributes & EUnderscore) attrib |= COMMON_LVB_UNDERSCORE;

	BOOL ok;
	WIN32_CALL(ok = SetConsoleTextAttribute(iStdOutHandle, attrib));
	return ok;
	}
