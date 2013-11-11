// textview.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32cmn.h>
#include <e32cons.h>

#include "textview.h"
#include "filebuffer.h"

CTextView::CTextView(MConsoleProvider& aConsoleProvider, CFedBufferBase& aBuffer, TInt aPriority)
	: CFedViewBase(aConsoleProvider, aPriority), iBuffer(aBuffer)
	{
	iBuffer.IncRef();
	CActiveScheduler::Add(this);
	}

void CTextView::ConstructL()
	{
	User::LeaveIfError(iBuffer.RegisterClient(*this, iDes, iRange, iRangeStartLineNumber));
	// Make iLine as big as it will ever need to be
	TSize size = iConsole.ScreenSize();
	iLine.CreateL(size.iWidth);
	}

CTextView::~CTextView()
	{
	Cancel();
	iLine.Close();
	iBuffer.DecRef();
	}

void CTextView::DoCancel()
	{
	ASSERT(EFalse);
	}

void CTextView::RunL()
{
	ASSERT(EFalse);
/*
	if(iStatus.Int() != KErrNone)
		return;

	//redraw by calling DoDrawL if iDrawPoint is within iWindow and iActive is ETrue
	switch(iState)
		{
		case EStateGetData:
			iState = EStateNone;
			if(iActive)
				DoDrawL();
			break;
		default:;
		}
*/
}

TInt CTextView::RunError(TInt aError)
{
	return aError;
}

//MViewController
void CTextView::ShowCursor()
	{
	if (iBuffer.Editable())
		{
		// Don't show a cursor if the file isn't editable
		iConsole.SetCursorHeight(20);
		}
	}

void CTextView::HideCursor()
	{
	iConsole.SetCursorHeight(0);
	}

void CTextView::ResizeL(const TWindow& aWindow)
{
	iActive = ETrue;
	TWindow oldWindow = iWindow;
	StoreWindow(aWindow);
	DoResizeL(oldWindow);
}

void CTextView::RedrawL(const TWindow& aWindow)
{
	iActive = ETrue;
	StoreWindow(aWindow);
	DoRedrawL();
}

void CTextView::DeactivateL()
{
	iActive = EFalse;
}

//MSharedCacheClient
void CTextView::InvalidateBuffer(TRequestStatus& aStatus)
{
	aStatus = KRequestPending;
	iDes.Set(NULL, 0);
	TRequestStatus* stat = &aStatus;
	User::RequestComplete(stat, KErrNone);
}

void CTextView::RequestData(TBool aFromTheTop, TInt aOffset)
	{
	if (aFromTheTop) iDrawPoint.SetXY(iWindow.iX, iWindow.iY);
	TInt err = iBuffer.GetData(*this, aOffset);
	if (err)
		{
		HandleDataLoadError(err);
		}
	else
		{
		DoDrawL();
		}
	}

void CTextView::SeekData(TInt aDocumentPosition, TInt aNumLines)
	{
	if (aDocumentPosition == 0 && aNumLines <= 0)
		{
		// Nothing to do
		return;
		}

	iDrawPoint.SetXY(iWindow.iX, iWindow.iY); // Seeks always cause a redraw from the top
	TInt err = iBuffer.SeekFromOffset(*this, aDocumentPosition, aNumLines, iWindow.iWidth-1); // minus one because we can't use the last char
	if (err)
		{
		HandleDataLoadError(err);
		}
	else
		{
		DoDrawL();
		}
	}

void CTextView::UpdateCursor(const TPoint& aNewPos)
	{
	ASSERT(aNewPos.iX >= 0);
	ASSERT(aNewPos.iY >= 0);
	iCursor = aNewPos;
	ValidateCursor();
	if (iActive)
		{
		ShowCursor();
		iConsole.SetCursorPosAbs(iCursor + iWindow.Origin());
		}
	}

void CTextView::ValidateCursor()
	{
	// Subclasses can override for additional validation
	if(iCursor.iX < 0)
		iCursor.iX = 0;
	if(iCursor.iX >= iWindow.iWidth)
		iCursor.iX = iWindow.iWidth - 1;
	if(iCursor.iY < 0)
		iCursor.iY = 0;
	if(iCursor.iY >= iWindow.iHeight)
		iCursor.iY = iWindow.iHeight -1;
	}

CFedBufferBase& CTextView::Buffer()
	{
	return iBuffer;
	}

void CTextView::HandleDataLoadError(TInt aError)
	{
	iConsoleProvider.InfoPrint(_L("Error loading data: %d"), aError);
	}
