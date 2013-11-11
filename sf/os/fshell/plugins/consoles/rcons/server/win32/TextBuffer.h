// TextBuffer.h
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

#ifndef __TEXTBUFFER_H__
#define __TEXTBUFFER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "base.h"


class MTextBufferObserver
	{
public:
	virtual void HandleTextBufferChange(int aPosX, int aPosY, PTCHAR aPtr, int aLength) = 0;
	virtual void HandleTextBufferScroll() = 0;
	virtual void HandleTextBufferCursorChange() = 0;
	virtual void HandleTextBufferCleared() = 0;
	};


class CTextBuffer : public CBase
	{
public:
	static CTextBuffer* New(int aWidth, int aHeight, int aMaxNumOverflowLines);
	virtual ~CTextBuffer();
	void SetObserver(MTextBufferObserver* aObserver);
	void Write(LPCTSTR aString, int aLength);
	void GetCursorPos(int& aX, int& aY) const;
	void SetAbsCursorPos(int aX, int aY);
	void SetRelCursorPos(int aX, int aY);
	void GetSize(int& aWidth, int& aHeight) const;
	int NumOverflowLines() const;
	void Clear();
	void ClearToEndOfLine();
	LPCTSTR GetLine(int aPos) const;
	void CaptureToFile(LPCTSTR aFileName);
	void StopCaptureToFile();
	bool IsCapturingToFile() const;
private:
	CTextBuffer(int aWidth, int aHeight, int aMaxNumOverflowLines);
	void Construct();
	void MoveCursor(int aNewX, int aNewY);
	PTCHAR CursorPtr() const;
	PTCHAR LineEndPtr() const;
	PTCHAR BufEndPtr() const;
	void WriteSpecialChar(TCHAR aChar);
	void WriteBlock(LPCTSTR aString, int aLength);
	bool IsOrdinaryChar(TCHAR aChar) const;
	int NumCharsToLineEnd() const;
	void ClearBuffer();
	void CursorLeft();
	void CursorRight();
	void FormFeed();
	void LineFeed();
	void CarriageReturn();
	void BackSpace();
	void HorizontalTab();
	void ScrollUp();
	void WriteLineToCaptureFile(LPCTSTR aLinePtr);
	void WriteOverflowLinesToCaptureFile();
	void WriteBufferToCaptureFile();
private:
	MTextBufferObserver* iObserver;
	PTCHAR iBuffer;
	const int iWidth;
	const int iHeight;
	const int iMaxNumOverflowLines;
	int iNumOverflowLines;
	int iCursorPosX;
	int iCursorPosY;
	int iCaptureFile;
	};

#endif // __TEXTBUFFER_H__
