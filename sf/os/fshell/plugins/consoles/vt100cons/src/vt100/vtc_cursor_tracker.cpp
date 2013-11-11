// vtc_cursor_tracker.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "vtc_cursor_tracker.h"


//
// Constants.
//

const TInt KTabSize = 4;


//
// TCursorTracker.
//

TCursorTracker::TCursorTracker(TSize aConsoleSize, MConsoleScrollHandler* aScrollHander)
	: iConsoleSize(aConsoleSize), iCursorPos(TPoint(0, 0)), iScrollHandler(aScrollHander)
	{
	}

void TCursorTracker::Write(const TDesC &aDes)
	{
	const TInt desLength = aDes.Length();
	for (TInt i = 0; i < desLength; ++i)
		{
		WriteChar(aDes[i]);
		}
	}
	
void TCursorTracker::Write(const TDesC8& aDes)
	{
	const TInt desLength = aDes.Length();
	for (TInt i = 0; i < desLength; ++i)
		{
		WriteChar(aDes[i]);
		}	
	}
	
void TCursorTracker::WriteChar(TChar aChar)
	{
	switch(aChar)
		{
		case 0x00:	// Null.
			break;
		case 0x07:	// Bell.
			break;
		case 0x08:	// Backspace.
		case 0x7f:	// Delete.
			CursorLeft();
			break;
		case 0x09:	// Tab.
			SetCursorPosRel(TPoint(KTabSize - (iCursorPos.iX % KTabSize), 0));
			break;
		case 0x0b:	// Vertical tab.
			// Can't easily handle this - fall through.
		case 0x0c:  // Form feed.
			Reset();
			break;
		case 0x0a:
			LineFeed();
			break;
		case 0x0d:
			CarriageReturn();
			break;
		default:
			CursorRight();
		}
	}

void TCursorTracker::SetCursorPosAbs(const TPoint& aPoint)
	{
	iCursorPos = aPoint;
	}

void TCursorTracker::SetCursorPosRel(const TPoint& aPoint)
	{
	iCursorPos += aPoint;
	}

void TCursorTracker::Reset()
	{
	iCursorPos.iX = iCursorPos.iY = 0;
	}

TPoint TCursorTracker::CursorPos() const
	{
	return iCursorPos;
	}

TSize TCursorTracker::ConsoleSize() const
	{
	return iConsoleSize;
	}

void TCursorTracker::CursorLeft()
	{
	if (iCursorPos.iX > 0)
		{
		// Not yet reached beginning of line.
		--iCursorPos.iX;
		}
	else if (iCursorPos.iY > 0)
		{
		// Reached beginning of line, so jump to end of line above.
		iCursorPos.iX = (iConsoleSize.iWidth - 1);
		--iCursorPos.iY;
		}
	}

void TCursorTracker::CursorRight()
	{
	if (iCursorPos.iX < (iConsoleSize.iWidth - 1))
		{
		// Not yet reached the end of the line.
		++iCursorPos.iX;
		}
	else if (iCursorPos.iY < (iConsoleSize.iHeight - 1))
		{
		// Reached the end of the line and there's space below - jump to the beginning of the line below.
		iCursorPos.iX = 0;
		++iCursorPos.iY;
		}
	else
		{
		// Reached the end of the line and there's no space below - console will scroll up a line and jump to the beginning of the newly exposed line.
		iCursorPos.iX = 0;
		if (iScrollHandler) iScrollHandler->ConsoleScrolled(1);
		}
	}

void TCursorTracker::LineFeed()
	{
	if (iCursorPos.iY < (iConsoleSize.iHeight - 1))
		{
		iCursorPos.iX = 0;
		++iCursorPos.iY;
		}
	else
		{
		iCursorPos.iX = 0;
		if (iScrollHandler) iScrollHandler->ConsoleScrolled(1);
		}
	}

void TCursorTracker::CarriageReturn()
	{
	iCursorPos.iX = 0;
	}
