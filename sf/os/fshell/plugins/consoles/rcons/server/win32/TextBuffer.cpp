// TextBuffer.cpp
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
#include "Misc.h"
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include "TextBuffer.h"

const int KTabSize = 4;


CTextBuffer* CTextBuffer::New(int aWidth, int aHeight, int aMaxNumOverflowLines)
	{
	std::auto_ptr<CTextBuffer> self(new(EThrow) CTextBuffer(aWidth, aHeight, aMaxNumOverflowLines));
	self->Construct();
	return self.release();
	}

CTextBuffer::~CTextBuffer()
	{
	delete iBuffer;
	}

void CTextBuffer::SetObserver(MTextBufferObserver* aObserver)
	{
	iObserver = aObserver;
	}

int StringLength(LPCTSTR aStart, LPCTSTR aEnd)
	{
	return (aEnd - aStart) + 1;
	}

void CTextBuffer::Write(LPCTSTR aString, int aLength)
	{
	LPCTSTR startBlock = aString;
	LPCTSTR ptr = aString;
	LPCTSTR end = aString + aLength - 1;
	int numCharsToLineEnd = NumCharsToLineEnd();
	while (ptr <= end)
		{
		if (!IsOrdinaryChar(*ptr))
			{
			// Not an ordinary character, so write the block we've gathered so far.
			if (StringLength(startBlock, ptr - 1) > 0)
				{
				WriteBlock(startBlock, StringLength(startBlock, ptr - 1));
				}
			// Write the special character.
			WriteSpecialChar(*ptr);
			// Setup variables for the next block.
			startBlock = ptr + 1;
			numCharsToLineEnd = NumCharsToLineEnd();
			}
		else if (StringLength(startBlock, ptr) == numCharsToLineEnd)
			{
			// Reached line end, so write the block we've gathered so far.
			ASSERT(numCharsToLineEnd > 0);
			WriteBlock(startBlock, numCharsToLineEnd);
			// Setup variables for the next block.
			startBlock = ptr + 1;
			numCharsToLineEnd = NumCharsToLineEnd();
			}
		else if (ptr == end)
			{
			// Reached end of buffer.
			if (StringLength(startBlock, ptr) > 0)
				{
				WriteBlock(startBlock, StringLength(startBlock, ptr));
				}
			}
		++ptr;
		}
	}

void CTextBuffer::GetCursorPos(int& aX, int& aY) const
	{
	aX = iCursorPosX;
	aY = iCursorPosY;
	}

void CTextBuffer::SetAbsCursorPos(int aX, int aY)
	{
	MoveCursor(aX, aY);
	}

void CTextBuffer::SetRelCursorPos(int aX, int aY)
	{
	MoveCursor(iCursorPosX + aX, iCursorPosY + aY);
	}

void CTextBuffer::GetSize(int& aWidth, int& aHeight) const
	{
	aWidth = iWidth;
	aHeight = iHeight;
	}

int CTextBuffer::NumOverflowLines() const
	{
	return iNumOverflowLines;
	}

void CTextBuffer::Clear()
	{
	ClearBuffer();
	MoveCursor(0, 0);
	}

void CTextBuffer::ClearToEndOfLine()
	{
	const PTCHAR start = CursorPtr();
	PTCHAR ptr = start;
	const PTCHAR end = LineEndPtr();

	while (ptr <= end)
		{
		*ptr++ = TCHAR(' ');
		}

	if (iObserver)
		{
		iObserver->HandleTextBufferChange(iCursorPosX, iCursorPosY, start, StringLength(start, end));
		}
	}

CTextBuffer::CTextBuffer(int aWidth, int aHeight, int aMaxNumOverflowLines)
	: iObserver(NULL), iBuffer(NULL), iWidth(aWidth), iHeight(aHeight), iMaxNumOverflowLines(aMaxNumOverflowLines), iNumOverflowLines(0), iCursorPosX(0), iCursorPosY(0), iCaptureFile(0)
	{
	}

void CTextBuffer::Construct()
	{
	iBuffer = new(EThrow) TCHAR[iWidth * (iHeight + iMaxNumOverflowLines)];
	ClearBuffer();
	}

void CTextBuffer::MoveCursor(int aNewX, int aNewY)
	{
	ASSERT((aNewX >= 0) && (aNewY >= 0));
	if ((aNewX >= 0) && (aNewY >= 0))
		{
		if (aNewX < iWidth)
			{
			iCursorPosX = aNewX;
			}
		if (aNewY < iHeight)
			{
			iCursorPosY = aNewY;
			}
		if (iObserver)
			{
			iObserver->HandleTextBufferCursorChange();
			}
		}
	}

PTCHAR CTextBuffer::CursorPtr() const
	{
	return iBuffer + (((iMaxNumOverflowLines + iCursorPosY) * iWidth) + iCursorPosX);
	}

PTCHAR CTextBuffer::LineEndPtr() const
	{
	return iBuffer + (((iMaxNumOverflowLines + iCursorPosY) * iWidth) + (iWidth - 1));
	}

PTCHAR CTextBuffer::BufEndPtr() const
	{
	return iBuffer + (iWidth * (iHeight + iMaxNumOverflowLines));
	}

void CTextBuffer::WriteSpecialChar(TCHAR aChar)
	{
	switch(aChar)
		{
		case 0x00:	// Null.
			break;
		case 0x07:	// Bell.
			break;
		case 0x08:	// Backspace.
		case 0x7f:	// Delete.
			BackSpace();
			break;
		case 0x09:
			HorizontalTab();
			break;
		case 0x0a:
			LineFeed();
			break;
		case 0x0b:	// Vertical tab.
			break;
		case 0x0c:
			FormFeed();
			break;
		case 0x0d:
			CarriageReturn();
			break;
		default:
			ASSERT(FALSE);
		}
	}

void CTextBuffer::WriteBlock(LPCTSTR aString, int aLength)
	{
	PTCHAR cursorPtr = CursorPtr();
	PTCHAR ptr = cursorPtr;
	for (int i = 0; i < aLength; ++i)
		{
		*ptr++ = aString[i];
		}
	if (iObserver)
		{
		iObserver->HandleTextBufferChange(iCursorPosX, iCursorPosY, cursorPtr, aLength);
		}
	if ((iCursorPosX + aLength) == iWidth)
		{
		MoveCursor(iWidth - 1, iCursorPosY);
		CursorRight();
		}
	else
		{
		MoveCursor(iCursorPosX + aLength, iCursorPosY);
		}
	}

bool CTextBuffer::IsOrdinaryChar(TCHAR aChar) const
	{
	switch(aChar)
		{
		case 0x00:	// Null.
		case 0x07:	// Bell.
		case 0x08:	// Backspace.
		case 0x7f:	// Delete.
		case 0x09:	// Tab.
		case 0x0a:	// Line feed.
		case 0x0b:	// Vertical tab.
		case 0x0c:	// Form feed.
		case 0x0d:	// Carriage return.
			return FALSE;
		default:
			return TRUE;
		}
	}

int CTextBuffer::NumCharsToLineEnd() const
	{
	return iWidth - iCursorPosX;
	}

void CTextBuffer::ClearBuffer()
	{
	PTCHAR bufPtr = iBuffer;
	PTCHAR end = BufEndPtr();
	while (bufPtr < end )
		{
		*bufPtr++ = TCHAR(' ');
		}
	iNumOverflowLines = 0;
	if (iObserver)
		{
		iObserver->HandleTextBufferCleared();
		}
	}

LPCTSTR CTextBuffer::GetLine(int aPos) const
	{
	ASSERT(aPos >= -iNumOverflowLines);
	ASSERT(aPos < iHeight);
	return iBuffer + ((iMaxNumOverflowLines + aPos) * iWidth);
	}

void CTextBuffer::CaptureToFile(LPCTSTR aFileName)
	{
	ASSERT(iCaptureFile == 0);
	iCaptureFile = _wopen(aFileName, _O_CREAT | _O_WRONLY | _O_BINARY, _S_IWRITE);
	if (iCaptureFile == -1)
		{
		iCaptureFile = 0;
		throw KExceptionFailedToCreateCaptureFile;
		}
	WriteOverflowLinesToCaptureFile();
	}

void CTextBuffer::StopCaptureToFile()
	{
	if (iCaptureFile)
		{
		WriteBufferToCaptureFile();
		_close(iCaptureFile);
		iCaptureFile = 0;
		}
	}

bool CTextBuffer::IsCapturingToFile() const
	{
	return !(iCaptureFile == 0);
	}

void CTextBuffer::CursorLeft()
	{
	if (iCursorPosX > 0)
		{
		// Not yet reached beginning of line.
		MoveCursor(iCursorPosX - 1, iCursorPosY);
		}
	else if (iCursorPosY > 0)
		{
		// Reached beginning of line, so jump to end of line above.
		MoveCursor(iWidth - 1, iCursorPosY - 1);
		}
	else
		{
		// Reached the top left corner of the console - do nothing.
		}
	}

void CTextBuffer::CursorRight()
	{
	if (iCursorPosX < (iWidth - 1))
		{
		// Not yet reached the end of the line.
		MoveCursor(iCursorPosX + 1, iCursorPosY);
		}
	else if (iCursorPosY < (iHeight - 1))
		{
		// Reached the end of the line and there's space below - jump to the beginning of the line below.
		MoveCursor(0, iCursorPosY + 1);
		}
	else
		{
		// Reached the end of the line and there's no space below - scroll up a line and jump to the beginning of the newly exposed line.
		ScrollUp();
		MoveCursor(0, iCursorPosY);
		}
	}

void CTextBuffer::FormFeed()
	{
	Clear();
	}

void CTextBuffer::LineFeed()
	{
	if (iCursorPosY < (iHeight - 1))
		{
		MoveCursor(0, iCursorPosY + 1);
		}
	else
		{
		ScrollUp();
		MoveCursor(0, iCursorPosY);
		}
	}

void CTextBuffer::CarriageReturn()
	{
	MoveCursor(0, iCursorPosY);
	}

void CTextBuffer::BackSpace()
	{
	if (!((iCursorPosX == 0) && (iCursorPosY == 0)))
		{
		CursorLeft();
		WriteBlock(TEXT(" "), 1);
		CursorLeft();
		}
	}

void CTextBuffer::HorizontalTab()
	{
	MoveCursor(iCursorPosX - (iCursorPosX % KTabSize) + KTabSize, iCursorPosY);
	if (iCursorPosX > (iWidth - 1))
		{
		CarriageReturn();
		LineFeed();
		}
	}

void CTextBuffer::ScrollUp()
	{
	WriteLineToCaptureFile(GetLine(0));
	PTCHAR dst = iBuffer;
	PTCHAR end = BufEndPtr();
	PTCHAR src = dst + iWidth;
	while (src < end)
		{
		*dst++ = *src++;
		}
	while (dst < end)
		{
		*dst++ = TCHAR(' ');
		}
	if (iNumOverflowLines < iMaxNumOverflowLines)
		{
		++iNumOverflowLines;
		}
	if (iObserver)
		{
		iObserver->HandleTextBufferScroll();
		}
	}

void CTextBuffer::WriteLineToCaptureFile(LPCTSTR aLinePtr)
	{
	if (iCaptureFile)
		{
		LPCTSTR lineEnd = aLinePtr + iWidth - 1;
		while ((*lineEnd == TCHAR(' ')) && (lineEnd > aLinePtr))
			{
			--lineEnd;
			}
		if (lineEnd == aLinePtr)
			{
			if (_write(iCaptureFile, "\r\n", 2) != 2)
				{
				_close(iCaptureFile);
				iCaptureFile = 0;
				throw KExceptionFailedToWriteToCaptureFile;
				}
			}
		else
			{
			const int lineLength = (lineEnd - aLinePtr) + 3;
			std::auto_ptr<char> narrowBuf(new(EThrow) char[lineLength]);
			char* narrowBufPtr = narrowBuf.get();
			while (aLinePtr <= lineEnd)
				{
				*narrowBufPtr++ = (char)*aLinePtr++;
				}
			*narrowBufPtr++ = '\r';
			*narrowBufPtr = '\n';
			if (_write(iCaptureFile, narrowBuf.get(), lineLength) != lineLength)
				{
				_close(iCaptureFile);
				iCaptureFile = 0;
				throw KExceptionFailedToWriteToCaptureFile;
				}
			
			}
		}
	}

void CTextBuffer::WriteOverflowLinesToCaptureFile()
	{
	for (int i = -iNumOverflowLines; i < 0; ++i)
		{
		WriteLineToCaptureFile(GetLine(i));
		}
	}

void CTextBuffer::WriteBufferToCaptureFile()
	{
	for (int i = 0; i < iHeight; ++i)
		{
		WriteLineToCaptureFile(GetLine(i));
		}
	}
