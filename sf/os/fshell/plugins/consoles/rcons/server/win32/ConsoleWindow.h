// ConsoleWindow.h
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

#if !defined(AFX_CONSOLEWINDOW_H__F891F8FC_FA99_41C1_93FB_6C1BB31F655D__INCLUDED_)
#define AFX_CONSOLEWINDOW_H__F891F8FC_FA99_41C1_93FB_6C1BB31F655D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Window.h"

class CTextBuffer;
class CTextView;


class MConsoleCharHandler
	{
public:
	virtual void HandleConsoleChar(TCHAR aChar, UINT aModifiers) = 0;
	virtual void HandleConsoleString(LPCTSTR aString, int aLength) = 0;
	};


class CConsoleWindow : public CWindow
	{
public:
	static CConsoleWindow* New(HINSTANCE aAppHandle, LPCTSTR aWindowClass, LPCTSTR aTitle, int aPosX, int aPosY, int aCharWidth, int aCharHeight, int aNumOverflowLines, MWindowObserver* aWindowObserver, MConsoleCharHandler* aCharHandler, bool aNoSelection);
	virtual ~CConsoleWindow();
	void Write(LPCTSTR aString);
	void Write(LPCTSTR aString, int aLength);
	void WriteFormat(LPCTSTR aFormat, ...);
	void GetCursorPos(int& aX, int& aY);
	void SetAbsCursorPos(int aX, int aY);
	void SetRelCursorPos(int aX, int aY);
	void SetCursorHeight(int aHeight);
	void GetConsoleSize(int& aWidth, int& aHeight);
	void ClearScreen();
	void ClearToEndOfLine();
	void CaptureToFile(LPCTSTR aFileName);
	void StopCaptureToFile();
	bool IsCapturingToFile() const;
	void PasteFromClipboard();
	void CopyToClipboard() const;
	void SetDimmed(bool aDimmed);
private:
	CConsoleWindow(bool aNoSelection);
	void Construct(HINSTANCE aAppHandle, LPCTSTR aWindowClass, LPCTSTR aTitle, int aPosX, int aPosY, int aWidth, int aHeight, int aNumOverflowLines, MWindowObserver* aWindowObserver, MConsoleCharHandler* aCharHandler);
private: // From CWindow.
	virtual void Draw() const;
	virtual LRESULT HandleChar(TCHAR aChar, UINT aModifiers);
	virtual LRESULT HandleFocusChange(bool aFocused);
	virtual void HandleScrollEvent(UINT aMessage, WPARAM aWParam);
	virtual void HandleSizeChanged(int aWidth, int aHeight);
	virtual void HandleMouseEvent(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
	virtual void HandleMenuPopUp(HMENU aMenu);
private:
	MConsoleCharHandler* iCharHandler;
	CTextBuffer* iBuffer;
	CTextView* iView;
	bool iNoSelection;
	};

#endif // !defined(AFX_CONSOLEWINDOW_H__F891F8FC_FA99_41C1_93FB_6C1BB31F655D__INCLUDED_)
