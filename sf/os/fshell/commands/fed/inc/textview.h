// textview.h
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
#ifndef TEXTVIEW_H_
#define TEXTVIEW_H_

#include <e32base.h>

#include "viewbase.h"

class CColorConsoleBase;
class CFileBuffer;

/*
 Main parent class for all views able to show any texts on the screen using data read/written from/to the universal
 memory buffer CFileBuffer (unicode, left-to-right, right-to-left, up-to-down), etc.
*/
class CTextView : public CFedViewBase
{
protected:
	CTextView(MConsoleProvider& aConsoleProvider, CFedBufferBase& aBuffer, TInt aPriority = CActive::EPriorityStandard);
	void ConstructL();
	~CTextView();

public:
	//CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	void ResizeL(const TWindow& aWindow);
	void RedrawL(const TWindow& aWindow);
	void DeactivateL();

	//MSharedCacheClient
	void InvalidateBuffer(TRequestStatus& aStatus);

	CFedBufferBase& Buffer();

protected:
	void RequestData(TBool aFromTheTop, TInt aOffset);
	void SeekData(TInt aDocumentPosition, TInt aNumLines);
	virtual void UpdateCursor(const TPoint& aNewPos);
	virtual void ValidateCursor();
	void ShowCursor();
	void HideCursor();
	void HandleDataLoadError(TInt aError);

protected:
	virtual void DoResizeL(const TWindow& aOldWindow) = 0;
	virtual void DoRedrawL() = 0;
	virtual void DoDrawL() = 0;

private:
	enum TState
		{
		EStateNone,
		EStateGetData,
		} iState;

	//member variables to use by inheriting classes
protected:
	CFedBufferBase& iBuffer;
	TPtrC16 iDes;		// This is set to whatever cache block is currently active	
	TBool iActive;		// Whether the view is active and can draw on the screen or not
	TPoint iCursor;		// Current position of cursor on the screen (in window coords, NOT console coords)
	TPoint iDrawPoint;	// Current position of drawing new characters on the screen (in console coordinates)
	RBuf iLine;			// Temp buffer for chars to be drawn on the screen
	TRange iRange;		// Document range of last fetched data (iDes)
	TInt iRangeStartLineNumber; // Line number for the first character of iRange
};

#endif /*TEXTVIEW_H_*/
