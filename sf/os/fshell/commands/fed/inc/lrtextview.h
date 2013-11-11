// lrtextview.h
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
#ifndef LRTEXTVIEW_H_
#define LRTEXTVIEW_H_

#include "textview.h"

class CColorConsoleBase;
class CLineData;

//Main class reading/writing normal (not-unicode) text arranged left-to-right on the screen.
class CLRTextView : public CTextView
{
public:
	static CLRTextView* NewL(MConsoleProvider& aConsoleProvider, CFedBufferBase& aBuffer);
	~CLRTextView();

protected:
	CLRTextView(MConsoleProvider& aConsoleProvider, CFedBufferBase& aBuffer);
	void ConstructL();

public:
	//MDeferredClose
	TBool TryCloseL();

	//MKeyConsumer
	TBool ConsumeKeyL(const TKeyCode& aCode);

	//CTextView
	void DoResizeL(const TWindow& aOldWindow);
	void DoRedrawL();
	void DoDrawL();

protected:
	void ValidateCursor();
	void UpdateCursor(const TPoint& aNewPos);
	void HandleControlChar();
	void AppendChar(TUint16 aChar);
	void WriteLine();
	void WriteChars();
	void GoToNextLine();
	void WriteLineContinue();
	void LineFinished();
	TInt DocumentPosition() const;
	void MoveCursor(TInt aX, TInt aY);
	void InsertTextL(const TDesC& aText);
	void DeleteTextL(TInt aNumChars);
	void DeleteCurrentLineL();
	void UpdateStatus();
	TInt Save();
	TInt SaveAs();
	void HandleSaveResult(TInt aError, TBool aWasOpen, const TDesC& aName);
	void GoToLine();
	void GoToLine(TInt aLine);
	void DoDrawL(TBool aUpdateCursorAndStatus); // Occasionally we want to do a draw without updating
	void CenterDocPosOnScreen(TInt aDocumentPosition);
	TPoint WindowLocationForDocumentPosition(TInt aDocPos) const; // Assuming the doc pos is on screen in an area that's been drawn
	void RedrawFromPositionToEndOfLine(TInt aDocumentPosition, TInt aNumCharsInserted);
	void Find();
	void CopyOrCutL(TBool aCut);
	void PasteL();
	void CopyToClipboardL(const TDesC& aBuf);
	HBufC* GetFromClipboardL();
	void SetMark();
	void CancelMark();
	static TInt FlashMark(TAny* aSelf);
	void DoFlashMark();
	TBool MarkActive() const;
	void UpdateDocumentPositionsStartingAtLine(TInt aLine, TInt aDelta);

private:
	CLineData* iLineData;
	TInt iCurrentDrawDocIndex; // Used while drawing to construct iLineData document positions, has no meaning outside of there
	TBool iLineIsErrorMessage;
	TBuf<256> iFindText;
	CPeriodic* iMarkFlasher;
	TInt iMarkDocPos;
	TBool iMarkShown;
	TInt iOldNextLine; // Used to limit how much redrawing we need to do
	TInt iOldNextLineDocPos; // Ditto
	TInt iPredictedOldNextLineDocPosDelta; // Ditto
	TBool* iRecursiveUpdateFlag; // For times when DoDrawL's aUpdateStatusAndCursor flag needs tracking through calls to RequestData
};

class CLineData : public CBase
	{
public:
	void EnsureCapacityL(TInt aWidth, TInt aHeight);
	TInt DocumentPosition(TInt aLine) const;
	TInt FileLineNumber(TInt aLine) const;
	TBool IsScreenPositionValid(TInt aLine, TInt aCol) const;
	TInt LastValidPosition(TInt aLine) const;

	void SetDocumentPositionForLine(TInt aLine, TInt aDocumentPosition);
	void SetFileLineNumberForLine(TInt aLine, TInt aDocumentPosition);
	void SetPositionIsValid(TInt aLine, TInt aCol, TBool aValid);
	void LineFinishedAt(TInt aLine, TInt aCol);

	~CLineData();

private:
	const TUint8& ByteForPos(TInt aLine, TInt aCol, TInt& aOffset) const;
	TUint8& ByteForPos(TInt aLine, TInt aCol, TInt& aOffset);

private:
	TInt iLineLen;
	TInt* iDocPosAndLine;
	TUint8* iScreenIndexes;
	};


#endif /*LRTEXTVIEW_H_*/
