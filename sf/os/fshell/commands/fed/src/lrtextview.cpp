// lrtextview.cpp
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

#include "lrtextview.h"
#include <e32cons.h>
#include "filebuffer.h"
#include <fshell/common.mmh>
#include <fshell/ltkutils.h>

const TInt KLastCtrlChar = 0x1f;	//Last control char in ASCII
const TInt KCtrlHTab = 0x09;		//Horizontal Tab

CLRTextView* CLRTextView::NewL(MConsoleProvider& aConsoleProvider, CFedBufferBase& aBuffer)
	{
	CLRTextView* self = new (ELeave) CLRTextView(aConsoleProvider, aBuffer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CLRTextView::~CLRTextView()
	{
	delete iLineData;
	delete iMarkFlasher;
	}

CLRTextView::CLRTextView(MConsoleProvider& aConsoleProvider, CFedBufferBase& aBuffer)
	: CTextView(aConsoleProvider, aBuffer), iMarkDocPos(-1), iOldNextLine(-1)
	{
	}

void CLRTextView::ConstructL()
	{
	iLineData = new(ELeave) CLineData;
	CTextView::ConstructL();
	iMarkFlasher = CPeriodic::NewL(CActive::EPriorityStandard);
	}

//MDeferredClose
TBool CLRTextView::TryCloseL()
	{
	if (!iBuffer.Modified())
		{
		return ETrue; // Ok to close now
		}
	else
		{
		TKeyCode ch = iConsoleProvider.Query(_L("Save changes to file? (yes/no/cancel) "), _L("ync\x1B"));
		if (ch == 'c' || ch == EKeyEscape) return EFalse;
		if (ch == 'y')
			{
			TInt err = Save();
			if (err) return EFalse; // Don't allow exit if the save failed
			}
		return ETrue;
		}
	}

TInt CLRTextView::Save()
	{
	if (!iBuffer.Editable() || iLineIsErrorMessage) return KErrNotSupported;
	
	CancelMark();
	TBool wasOpen = iBuffer.IsOpen();
	TInt err = KErrNone;
	const TDesC& name = iBuffer.Title();
	if (name.Length())
		{
		iConsoleProvider.InfoPrint(_L("Saving"));
		TRAP(err, iBuffer.SaveL(name, ETrue));
		HandleSaveResult(err, wasOpen, iBuffer.Title());
		return err;
		}
	else
		{
		return SaveAs();
		}
	}

TInt CLRTextView::SaveAs()
	{
	if (!iBuffer.Editable() || iLineIsErrorMessage) return KErrNotSupported;
	
	CancelMark();
	TBool wasOpen = iBuffer.IsOpen();
	TFileName name = iBuffer.Title();
	TInt err = KErrCancel;
	TBool go = iConsoleProvider.QueryFilename(_L("Save as: "), name);
	if (go)
		{
		iConsoleProvider.InfoPrint(_L("Saving"));
		TRAP(err, iBuffer.SaveL(name, EFalse));
		if (err == KErrAlreadyExists)
			{
			if (iConsoleProvider.Query(_L("File already exists. Replace? (yes/no) "), _L("yn")) == 'y')
				{
				iConsoleProvider.InfoPrint(_L("Saving"));
				TRAP(err, iBuffer.SaveL(name, ETrue));
				}
			}
		HandleSaveResult(err, wasOpen, iBuffer.Title());
		}
	return err;
	}

void CLRTextView::HandleSaveResult(TInt aError, TBool aWasOpen, const TDesC& aName)
	{
	if (aError == KErrNone)
		{
		TRAP_IGNORE(DoRedrawL()); // Not only can QueryFilename mess up the screen, saving the file could change the line endings and thus change the character counts, meaning we need to recalculate & redraw
		iConsoleProvider.InfoPrint(_L("Save succeeded"));
		}
	else if (aWasOpen && !iBuffer.IsOpen())
		{
		// An error occurred after the original file was closed. This is fatal so we need to put up a dialog to that effect
		iLine.Format(_L("Temp file @ %S"), &aName);
		iLineIsErrorMessage = ETrue;
		UpdateStatus();
		iConsoleProvider.InfoPrint(_L("Failed to rename temp file (%d)"), aError);
		}
	else
		{
		// Failed to save, but we didn't mess with the original so we can carry on using it
		TRAP_IGNORE(DoRedrawL()); // Not only can QueryFilename mess up the screen, saving the file could change the line endings and thus change the character counts, meaning we need to recalculate & redraw
		switch (aError)
			{
			case KErrDiskFull:
				iConsoleProvider.InfoPrint(_L("Disk full, couldn't save file"));
				break;
			case KErrNoMemory:
				iConsoleProvider.InfoPrint(_L("Out of memory!"));
				break;
			case KErrPathNotFound:
				iConsoleProvider.InfoPrint(_L("KErrPathNotFound. Directory doesn't exist?"));
				break;
			case KErrGeneral:
				iConsoleProvider.InfoPrint(_L("Couldn't save file. No idea why"));
				break;
			case KErrBadName:
				iConsoleProvider.InfoPrint(_L("Bad file name"));
				break;
			case KErrAccessDenied:
				iConsoleProvider.InfoPrint(_L("Access denied. Read-only file or disk?"));
				break;
			default:
				iConsoleProvider.InfoPrint(_L("Unable to save file: %d"), aError);
				break;
			}
		}
	}

//MKeyConsumer
TBool CLRTextView::ConsumeKeyL(const TKeyCode& aCode)
	{
	TBool handled = ETrue;
	switch(aCode)
		{
		case EKeyUpArrow:
			MoveCursor(0, -1); break;
		case EKeyDownArrow:
			MoveCursor(0, 1); break;
		case EKeyLeftArrow:
			MoveCursor(-1, 0); break;
		case EKeyRightArrow:
			MoveCursor(1, 0); break;
		case EKeyPageUp:
		case CTRL('u'):
			MoveCursor(0, -iWindow.iHeight); break;
		case EKeyPageDown:
		case CTRL('d'):
			MoveCursor(0, iWindow.iHeight); break;
		case EKeyHome:
			UpdateCursor(TPoint(0, iCursor.iY));
			break;
		case CTRL('t'):
			iCursor.SetXY(0, 0);
			RequestData(ETrue, 0); break;
		case CTRL('b'):
			GoToLine(KMaxTInt); break;
		case EKeyEnd:
			{
			TInt pos = iLineData->LastValidPosition(iCursor.iY);
			UpdateCursor(TPoint(pos, iCursor.iY));
			break;
			}
		case EKeyBackspace:
			DeleteTextL(-1);
			break;
		case EKeyDelete:
			DeleteTextL(1);
			break;
		case CTRL('s'):
			Save();
			break;
		case CTRL('a'):
			SaveAs();
			break;
		case CTRL('k'):
			DeleteCurrentLineL();
			break;
		case CTRL('g'):
			GoToLine();
			break;
		case CTRL('f'):
			Find();
			break;
		case CTRL('v'):
			PasteL();
			break;
		case CTRL('c'):
			if (MarkActive()) CopyOrCutL(EFalse);
			else SetMark();
			break;
		case CTRL('x'):
			if (MarkActive()) CopyOrCutL(ETrue);
			else SetMark();
			break;
		default:
			handled = EFalse;
			break;
		}

	if (!handled && (aCode == EKeyEnter || aCode == EKeyTab || TChar(aCode).IsPrint()))
		{
		TBuf<1> buf;
		buf.Append(aCode);
		InsertTextL(buf);
		handled = ETrue;
		}
	return handled;
}

void CLRTextView::MoveCursor(TInt aX, TInt aY)
	{
	if (aX) ASSERT(aY == 0);
	if (aY) ASSERT(aX == 0); // Can't handle moving in both axes at once

	TInt line = iCursor.iY;
	TInt dir = (aX > 0) ? 1 : -1;
	TInt newX = iCursor.iX;
	while (aX)
		{
		newX += dir;
		if (newX < 0)
			{
			// Move to end of previous line
			newX = iWindow.iWidth; // Validate cursor will fix this, in the case where this causes us to scroll up a line
			aY--;
			break; // We don't really support seeking further than one line either way
			}
		else if (newX >= iWindow.iWidth-1)
			{
			// Move to start of next line
			newX = 0;
			aY++;
			break; // We don't really support seeking further than one line either way
			}

		if (iLineData->IsScreenPositionValid(line, newX))
			{
			aX -= dir;
			}
		}

	if (line + aY < 0)
		{
		// Scroll up
		SeekData(iLineData->DocumentPosition(0), aY);
		}
	else if (line + aY >= iWindow.iHeight)
		{
		// Scroll down
		SeekData(iLineData->DocumentPosition(0), aY);
		}
	else
		{
		// Just update cursor
		if (iLineData->IsScreenPositionValid(line + aY, 0))
			{
			// If the line len is -1 it means the doc doesn't extend as far as this line, therefore we shouldn't allow the cursor to be moved to it
			UpdateCursor(TPoint(newX, iCursor.iY + aY));
			}
		}
	}

void CLRTextView::DoResizeL(const TWindow& /*aOldWindow*/)
	{
	DoRedrawL();
	}

void CLRTextView::DoRedrawL()
	{
	iLineData->EnsureCapacityL(iWindow.iWidth, iWindow.iHeight);
	ValidateCursor();
	RequestData(ETrue, iLineData->DocumentPosition(0));
	}

void CLRTextView::DoDrawL()
	{
	DoDrawL(iRecursiveUpdateFlag ? *iRecursiveUpdateFlag : ETrue);
	}

void CLRTextView::DoDrawL(TBool aUpdateCursorAndStatus)
	{
	if (iDrawPoint == TPoint(iWindow.iX, iWindow.iY))
		{
		iLineData->SetDocumentPositionForLine(0, iRange.iLocation);
		iLineData->SetFileLineNumberForLine(0, iRangeStartLineNumber);
		}

	ASSERT(iRange.iLength == iDes.Length());
	HideCursor(); // Don't show the cursor while we're moving it around drawing - will be shown again by UpdateCursor
	for (iCurrentDrawDocIndex = 0; iCurrentDrawDocIndex < iDes.Length(); iCurrentDrawDocIndex++)
		{
		TUint16 c = iDes[iCurrentDrawDocIndex];
		if(iDrawPoint.iY >= iWindow.NextY())
			{
			break;
			}
		if(c <= KLastCtrlChar)
			HandleControlChar();
		else
			AppendChar(c);
		}

	TBool moreData = !iBuffer.DocumentPositionIsEof(iRange.Max());
	TBool filledScreen = iDrawPoint.iY >= iWindow.NextY();
	if (!filledScreen)
		{
		WriteChars();
		LineFinished();
		}
	if (!moreData)
		{
		iDrawPoint.iY++; // Start from the line *after* the line we were on
		// In the case where we've finished writing but haven't filled the screen, we need to blank the rest of it (because to avoid flicker we don't do ClearScreens)
		while (iDrawPoint.iY < iWindow.NextY())
			{
			iDrawPoint.iX = iWindow.iX;
			iLine.Copy(_L("^"));
			WriteChars();
			TInt lineNum = iDrawPoint.iY - iWindow.iY;
			iLineData->SetDocumentPositionForLine(lineNum+1, iLineData->DocumentPosition(lineNum));
			iLineData->SetFileLineNumberForLine(lineNum, -1);
			iLineData->LineFinishedAt(lineNum, -1);
			iDrawPoint.iY++;
			}
		}

	if (filledScreen || !moreData)
		{
		// Finished - null iDes and restore cursor
		iDes.Set(NULL, 0);
		if (aUpdateCursorAndStatus)
			{
			UpdateCursor(iCursor);
			}
		}
	else
		{
		TBool flagSet = EFalse;
		if (!iRecursiveUpdateFlag)
			{
			iRecursiveUpdateFlag = &aUpdateCursorAndStatus;
			flagSet = ETrue;
			}
		// More data needed to fill the screen
		RequestData(EFalse, iRange.Max());
		if (flagSet) iRecursiveUpdateFlag = NULL;
		}
}

void CLRTextView::ValidateCursor()
	{	
	CTextView::ValidateCursor();
	TInt maxX = iWindow.iWidth - 1;
	TInt line = iCursor.iY;
	while (iCursor.iX > 0 && !iLineData->IsScreenPositionValid(line, iCursor.iX))
		{
		// In case the cursor ended up in the dead space in the middle of a tab character
		iCursor.iX--;
		}
	while (line > 0 && !iLineData->IsScreenPositionValid(line, 0))
		{
		// In case the cursor has ended up in dead space
		iCursor.iY--;
		line--;
		}
	maxX = iLineData->LastValidPosition(line);
	if(iCursor.iX > maxX)
		iCursor.iX = maxX;
	}

void CLRTextView::HandleControlChar()
	{
	TUint8 c = iDes[iCurrentDrawDocIndex];
	if (c == KCtrlHTab)
		{
		TInt pos = iDrawPoint.iX - iWindow.iX + iLine.Length();
		TInt numToWrite = TabWidth() - (pos % TabWidth()); // Tabs align to 4-column boundaries rather than just outputting 4 spaces
		numToWrite = Min(numToWrite, iWindow.iWidth-1-pos); // In case the tab causes a line continuation, we want to make sure the tab ends at the end of the line so the following character will start at column zero
		for(TInt i=0; i<numToWrite; i++)
			{
			TInt line = iDrawPoint.iY - iWindow.iY;
			TInt col = iDrawPoint.iX - iWindow.iX + iLine.Length();
			AppendChar(' ');
			if (i > 0) iLineData->SetPositionIsValid(line, col, EFalse); // i==0 (ie the first screen position of the tab) is a valid place to move the cursor to
			}
		}
	else if (c == '\r')
		{
		if (iCurrentDrawDocIndex+1 < iDes.Length() && iDes[iCurrentDrawDocIndex+1] == '\n') iCurrentDrawDocIndex++; // Skip over the CR to the LF
		WriteLine();
		}
	else if (c == '\n')
		{
		WriteLine();
		}
	else
		{
		// unknown control char - so escape it
		AppendChar('.');
		}
	}

void CLRTextView::AppendChar(TUint16 aChar)
	{
	iLine.Append(TChar(aChar));
	iLineData->SetPositionIsValid(iDrawPoint.iY - iWindow.iY, iDrawPoint.iX - iWindow.iX + iLine.Length(), ETrue);
	if(iDrawPoint.iX + iLine.Length() == iWindow.NextX()-1)
		{
		WriteChars();
		WriteLineContinue();
		GoToNextLine();
		}
	}

//Writes the current line on the screen and moves to the next verse and line
void CLRTextView::WriteLine()
	{
	TInt thisLine = iDrawPoint.iY-iWindow.iY;
	WriteChars();
	GoToNextLine();
	iLineData->SetFileLineNumberForLine(thisLine+1, iLineData->FileLineNumber(thisLine) + 1);
	if (iOldNextLine != -1 && thisLine+1 == iOldNextLine && iLineData->DocumentPosition(iOldNextLine) == iOldNextLineDocPos + iPredictedOldNextLineDocPosDelta)
		{
		// We have drawn everything we need to, and the drawing didn't cause the former next line to move
		iDrawPoint.iY = iWindow.NextY();
		UpdateDocumentPositionsStartingAtLine(iOldNextLine+1, iPredictedOldNextLineDocPosDelta);
		}
	iOldNextLine = -1;
	}

//Writes the current line on the screen
void CLRTextView::WriteChars()
	{
	/* As an performance optimisation, we avoid calling ClearScreen when redrawing. Therefore we must ensure we 
	 * write to the entire line to prevent artifacts. 
	 *
	 * There are 2 approaches to this - either using CConsoleBase::ClearToEndOfLine or padding the line with 
	 * spaces up to the window width. ClearToEndOfLine is really slow in WINSCW tshell which is why it's not
	 * used by default. On target it's probably more efficient to use it.
	 */
	TPoint drawPoint(iDrawPoint);
	TPoint endPoint(drawPoint.iX + iLine.Length(), drawPoint.iY);
	iConsole.SetCursorPosAbs(drawPoint);
#ifdef FSHELL_WSERV_SUPPORT
// text windowserver is really slow at ClearToEndOfLine
#define USE_CLEARTOENDOFLINE
#endif
#ifdef USE_CLEARTOENDOFLINE
	iConsole.Write(iLine);
	iConsole.ClearToEndOfLine();
#else
	iLine.AppendFill(' ', iWindow.iWidth - (iDrawPoint.iX-iWindow.iX) - iLine.Length());
	iConsole.Write(iLine);
#endif
	iLine.Zero();
	// Remember to reset the console pos and the drawpoint to the end of the line proper in case the WriteChars 
	// was to half a line and the rest of the line is in another block.
	iDrawPoint = endPoint;
	iConsole.SetCursorPosAbs(iDrawPoint);
	}

// Moves to the next screen line but doesn't move to the next file line (used when a line is longer than the width of the window)
void CLRTextView::GoToNextLine()
	{
	LineFinished();
	iDrawPoint.iX = iWindow.iX;
	iDrawPoint.iY++;
	iLineData->SetPositionIsValid(iDrawPoint.iY-iWindow.iY, 0, ETrue);
	}

void CLRTextView::LineFinished()
	{
	TInt currentLine = iDrawPoint.iY - iWindow.iY;
	if (currentLine == iWindow.iHeight) return; // This can happen (and is ok) when drawing the last line of the screen
	iLineData->LineFinishedAt(currentLine, iDrawPoint.iX - iWindow.iX);
	iLineData->SetDocumentPositionForLine(currentLine+1, iRange.iLocation+iCurrentDrawDocIndex+1); // Plus one cos we haven't drawn the first character of the next line yet
	}

void CLRTextView::WriteLineContinue()
	{
	CColorConsoleBase* cons = iConsoleProvider.ColorConsole();
	if (cons) cons->SetTextAttribute(ETextAttributeHighlight);
	_LIT(KContinuation, "\\");
	iConsole.Write(KContinuation);
	if (cons) cons->SetTextAttribute(ETextAttributeNormal);

	TInt thisLine = iDrawPoint.iY-iWindow.iY;
	iLineData->SetFileLineNumberForLine(thisLine+1, iLineData->FileLineNumber(thisLine)); // The new line has the same file number as the current line, because it's a continuation line
	}

TInt CLRTextView::DocumentPosition() const
	{
	TInt line = iCursor.iY;
	TInt pos = iLineData->DocumentPosition(line);
	for (TInt i = 0; i < iCursor.iX; i++)
		{
		if (iLineData->IsScreenPositionValid(line, i)) pos++;
		}
	return pos;
	}

void CLRTextView::InsertTextL(const TDesC& aText)
	{
	if (!iBuffer.Editable() || iLineIsErrorMessage) return;
	
	CancelMark();
	TInt pos = DocumentPosition();
	iBuffer.InsertTextL(pos, aText);
	if (iCursor.iX == iWindow.iWidth-1)
		{
		// If the cursor is over a line continuation character (which it will be if it gets to this value) move cursor to the real insertion point at the start of the next line before doing anything - makes the logic much easier!
		MoveCursor(1, 0);
		}

	// If we can be sure it won't cause a reflow, shortcut the requesting and redrawing mechanism, for performance reasons
	TPoint cursorPos = WindowLocationForDocumentPosition(pos);
	TInt lineNum = cursorPos.iY;
	TBool posWasAtEndOfLine = cursorPos.iX == iLineData->LastValidPosition(lineNum) && cursorPos.iX + aText.Length() < iWindow.iWidth-1;
	if (posWasAtEndOfLine && aText.Locate('\t') == KErrNotFound && aText.Locate('\r') == KErrNotFound)
		{
		// Can't optimise if the text contains tabs or newlines, we need to do the full algorithm in that case
		iConsole.Write(aText);
		for (TInt i = 0; i < aText.Length(); i++)
			{
			iLineData->SetPositionIsValid(lineNum, cursorPos.iX + 1 + i, ETrue); // Each added char makes the character position one after that character valid
			}
		for (TInt l = lineNum+1; l <= iWindow.iHeight; l++)
			{
			iLineData->SetDocumentPositionForLine(l, iLineData->DocumentPosition(l) + aText.Length());
			}
		MoveCursor(aText.Length(),0);
		}
	else
		{
		RedrawFromPositionToEndOfLine(pos, aText.Length());
		CenterDocPosOnScreen(pos + aText.Length());
		}
	}

void CLRTextView::RedrawFromPositionToEndOfLine(TInt aDocumentPosition, TInt aNumCharsInserted)
	{
	TPoint point = WindowLocationForDocumentPosition(aDocumentPosition);
	ASSERT(point.iY >= 0);
	iDrawPoint = point + iWindow.Origin();
	// Try and avoid redrawing the whole screen if the RequestData doesn't make the line longer (in terms of screen lines)
	TInt lastScreenLineForCurrentLine = point.iY;
	while (iLineData->FileLineNumber(lastScreenLineForCurrentLine+1) == iLineData->FileLineNumber(lastScreenLineForCurrentLine))
		{
		// We have to allow for the current line wrapping onto additional screen lines
		lastScreenLineForCurrentLine++;
		}
	iOldNextLine = lastScreenLineForCurrentLine + 1;
	iOldNextLineDocPos = iLineData->DocumentPosition(iOldNextLine);
	iPredictedOldNextLineDocPosDelta = aNumCharsInserted;
	RequestData(EFalse, aDocumentPosition);
	}

void CLRTextView::DeleteTextL(TInt aNumChars)
	{
	if (!iBuffer.Editable() || iLineIsErrorMessage) return;

	CancelMark();
	ASSERT(aNumChars >= -1); // Can't backspace more than one character
	TInt pos = DocumentPosition();
	TRange range;
	if (aNumChars < 0)
		range = TRange(pos + aNumChars, -aNumChars);
	else
		range = TRange(pos, aNumChars);
	range.Intersect(TRange(0, KMaxTInt)); // Clamp the range to zero

	if (range.iLength)
		{
		iBuffer.DeleteTextL(range);
		TInt line = iCursor.iY;
		if (aNumChars == -1) MoveCursor(-1, 0);
		// Seems to be an issue using RedrawFromPositionToEndOfLine when deleting from the start of a line - since we know it wouldn't be worth using it in that case, don't do it.
		if (iCursor.iY == line && aNumChars == -1)
			{
			RedrawFromPositionToEndOfLine(range.iLocation, -range.iLength);
			}
		else
			{
			iDrawPoint = iCursor + iWindow.Origin();
			RequestData(EFalse, DocumentPosition());
			}
		}
	}

void CLRTextView::DeleteCurrentLineL()
	{
	if (!iBuffer.Editable() || iLineIsErrorMessage) return;

	CancelMark();
	TInt currentLine = iCursor.iY;
	TInt lineStart = iLineData->DocumentPosition(currentLine);
	TInt lineLen = iLineData->DocumentPosition(currentLine+1) - lineStart;
	if (lineLen > 0)
		{
		TRange range(lineStart, lineLen);
		iBuffer.DeleteTextL(range);
		iDrawPoint = TPoint(iWindow.iX, iWindow.iY + iCursor.iY);
		RequestData(EFalse, lineStart);
		}
	}

void CLRTextView::UpdateCursor(const TPoint& aNewPos)
	{
	CTextView::UpdateCursor(aNewPos);
	UpdateStatus();
	}

void CLRTextView::UpdateStatus()
	{
	if (iLineIsErrorMessage)
		{
		iConsoleProvider.WriteStatus(iLine, KNullDesC);
		return;
		}

	_LIT(KUnknownDelim, "CRLF"); // Since that's what it will get output as
	_LIT(KUnknownEnc, "UTF-8"); // ditto
	const TDesC* lineEnding = &KUnknownDelim;
#define CASE_LIT(res, x, y) case x: { _LIT(KName, y); res = &KName; break; }
	switch(iBuffer.DelimiterType())
		{
		CASE_LIT(lineEnding, EDelimLF, "LF")
		CASE_LIT(lineEnding, EDelimCR, "CR")
		CASE_LIT(lineEnding, EDelimCRLF, "CRLF")
		CASE_LIT(lineEnding, EDelimUnicode, "UniLF")
		default:
			break;
		}
	const TDesC* encoding = &KUnknownEnc;
	switch (iBuffer.Encoding())
		{
		CASE_LIT(encoding, EEncodingNarrow, "8-bit")
		CASE_LIT(encoding, EEncodingUtf8, "UTF-8")
		CASE_LIT(encoding, EEncodingUtf16LE, "UTF-16LE")
		CASE_LIT(encoding, EEncodingUtf16BE, "UTF-16BE")
		default:
			break;
		}

	const TDesC* modified = &KNullDesC;
	if (iBuffer.Modified())
		{
		_LIT(KMod, "(*)");
		modified = &KMod;
		}

	TInt currentLine = iLineData->FileLineNumber(iCursor.iY) + 1; // Human-readable line numbers are always shown as 1-based
	TInt currentCol = iCursor.iX + 1; // Human-readable col numbers are always shown as 1-based, annoying as it is we will do the same
	TInt docPos = DocumentPosition();
	_LIT(KStatus, "%S %S %S Ln:%d Col:%d Char:%d");
	_LIT(KShortStatus, "%S %S %S Ln:%d");
	_LIT(KMarkStatus, "Mark@%d. ^X/^C to complete. Ln:%d Ch:%d");
	const TDesC* title = &iBuffer.Title();
	_LIT(KUntitled, "untitled");
	if (title->Length() == 0) title = &KUntitled;
	if (MarkActive())
		{
		iConsoleProvider.WriteStatus(KNullDesC, KMarkStatus, iMarkDocPos, currentLine, docPos);
		}
	else if (iConsole.ScreenSize().iWidth < 50)
		{
		iConsoleProvider.WriteStatus(*title, KShortStatus, modified, lineEnding, encoding, currentLine);
		}
	else
		{
		iConsoleProvider.WriteStatus(*title, KStatus, modified, lineEnding, encoding, currentLine, currentCol, docPos);
		}
	}

void CLRTextView::GoToLine()
	{
	TBuf<32> lineText;
	TBool go = iConsoleProvider.QueryText(_L("Go to line: "), lineText);
	if (go == EFalse) return;

	TLex lex(lineText);
	TBool gotoLine = ETrue;
	if (lex.Peek() == 'c')
		{
		// Go to character pos
		lex.Inc();
		gotoLine = EFalse;
		}

	TInt pos;
	TInt err = lex.Val(pos);
	if (err || pos <= 0)
		{
		iConsoleProvider.InfoPrint(_L("Couldn't parse line number '%S'"), &lineText);
		return;
		}

	if (gotoLine && pos > 1)
		{
		GoToLine(pos-1); // -1 to change 1-based display number to internal 0-based.
		}
	else
		{
		CenterDocPosOnScreen(pos-1);
		}
	}

void CLRTextView::GoToLine(TInt aLine)
	{
	TInt err = iBuffer.SeekFromOffset(*this, 0, aLine, KMaxTInt); 
	if (err)
		{
		HandleDataLoadError(err);
		}
	else
		{
		CenterDocPosOnScreen(iRange.iLocation);
		}
	}

void CLRTextView::Find()
	{
	TBool searchBackwards = EFalse;
	TBool proceed = iConsoleProvider.QueryText(_L("Find text: "), iFindText);
	while (proceed)
		{
		TInt nextLineStart = iLineData->DocumentPosition(iCursor.iY + 1);
		iConsoleProvider.InfoPrint(_L("Searching..."));
		TInt found = iBuffer.Find(nextLineStart, iFindText, searchBackwards);
		if (found == KErrNotFound)
			{
			iConsoleProvider.InfoPrint(_L("Cannot find '%S'"), &iFindText);
			proceed = EFalse;
			}
		else if (found < 0)
			{
			iConsoleProvider.InfoPrint(_L("Error during searching: %d"), found);
			proceed = EFalse;
			}
		else
			{
			CenterDocPosOnScreen(found);
			/*TKeyCode ch = iConsoleProvider.Query(_L("Search again? (next/previous/cancel) "));
			if (ch == 'n')
				{
				searchBackwards = EFalse;
				continue;
				}
			else if (ch == 'p')
				{
				searchBackwards = ETrue;
				continue;
				}
			else*/
				{
				proceed = EFalse;
				}
			}
		}
	}

void CLRTextView::CenterDocPosOnScreen(TInt aDocumentPosition)
	{
	const TInt numScreenLines = iWindow.iHeight;
	TPoint point = WindowLocationForDocumentPosition(aDocumentPosition);
	if (point.iY != -1)
		{
		// pos is on screen already - just move the cursor
		UpdateCursor(point);
		return;
		}

	TInt err = iBuffer.SeekFromOffset(*this, aDocumentPosition, -(numScreenLines/2), iWindow.iWidth-1);
	if (err)
		{
		HandleDataLoadError(err);
		}
	else
		{
		iDrawPoint.SetXY(iWindow.iX, iWindow.iY);
		DoDrawL(EFalse); // EFalse to say we will update the cursor and status ourselves
		UpdateCursor(WindowLocationForDocumentPosition(aDocumentPosition));
		}
	}

void CLineData::EnsureCapacityL(TInt aWidth, TInt aHeight)
	{
	if (!iDocPosAndLine || (TUint)User::AllocLen(iDocPosAndLine) < sizeof(TInt)*2 * (aHeight+1))
		{
		User::Free(iDocPosAndLine);
		iDocPosAndLine = NULL;
		iDocPosAndLine = (TInt*)User::AllocZL(sizeof(TInt)*2*(aHeight+1));
		}
	iLineLen = aWidth-1;
	TInt lineWidthBytes = ((iLineLen+7)&~7)/8;
	TInt screenBytes = lineWidthBytes * (aHeight+1);
	if (!iScreenIndexes || User::AllocLen(iScreenIndexes) < screenBytes)
		{
		User::Free(iScreenIndexes);
		iScreenIndexes = NULL;
		iScreenIndexes = (TUint8*)User::AllocZL(screenBytes);
		}
	SetPositionIsValid(0, 0, ETrue); // This position is always valid even if the document is empty
	}

TInt CLineData::DocumentPosition(TInt aLine) const
	{
	return iDocPosAndLine[aLine*2];
	}

TInt CLineData::FileLineNumber(TInt aLine) const
	{
	return iDocPosAndLine[aLine*2 + 1];
	}

const TUint8& CLineData::ByteForPos(TInt aLine, TInt aCol, TInt& aOffset) const
	{
	return const_cast<CLineData*>(this)->ByteForPos(aLine, aCol, aOffset);
	}

TUint8& CLineData::ByteForPos(TInt aLine, TInt aCol, TInt& aOffset)
	{
	TInt lineWidthBytes = ((iLineLen+7)&~7)/8;
	TInt byteIndex = aLine*lineWidthBytes + aCol/8;
	aOffset = aCol%8;
	TUint8& b = iScreenIndexes[byteIndex];
	return b;
	}

TBool CLineData::IsScreenPositionValid(TInt aLine, TInt aCol) const
	{
	TInt offset;
	TUint8 b = ByteForPos(aLine, aCol, offset);
	return b & (1 << offset);
	}

void CLineData::SetDocumentPositionForLine(TInt aLine, TInt aDocumentPosition)
	{
	ASSERT(iDocPosAndLine && aLine*2*sizeof(TInt) < (TUint)User::AllocLen(iDocPosAndLine));
	iDocPosAndLine[aLine*2] = aDocumentPosition;
	}

void CLineData::SetFileLineNumberForLine(TInt aLine, TInt aFileLineNumber)
	{
	ASSERT(iDocPosAndLine && (aLine*2+1)*sizeof(TInt) < (TUint)User::AllocLen(iDocPosAndLine));
	iDocPosAndLine[aLine*2 + 1] = aFileLineNumber;
	}

void CLineData::SetPositionIsValid(TInt aLine, TInt aCol, TBool aValid)
	{
	TInt offset;
	TUint8& b = ByteForPos(aLine, aCol, offset);
	if (aValid)
		{
		b |= (1<<offset);
		}
	else
		{
		b &= ~(1<<offset);
		}
	}

void CLineData::LineFinishedAt(TInt aLine, TInt aCol)
	{
	if (aCol >=0 && aCol < iLineLen) SetPositionIsValid(aLine, aCol, ETrue); //  The last position on the line is valid even though nothing is drawn there
	TInt offset;
	TUint8& b = ByteForPos(aLine, aCol+1, offset); // Plus one as we want to clear everything beyond the aCol position
	// Clear everything above the offset'th bit
	b &= (1 << offset) - 1;
	// And clear all bytes from b to the end of the line
	TUint8* endb = &ByteForPos(aLine, iLineLen-1, offset);
	// Armv5 doesn't like passing in a length of zero to memset, not sure why. Anyway, check there are actually some other bytes that need clearing
	if (endb > &b)
		{
		Mem::FillZ((&b)+1, endb - &b);
		}
	}

CLineData::~CLineData()
	{
	User::Free(iDocPosAndLine);
	User::Free(iScreenIndexes);
	}

TInt CLineData::LastValidPosition(TInt aLine) const
	{
	TInt offset;
	TInt pos = iLineLen-1; // Start comparing from the last possible valid position
	const TUint8* b = &ByteForPos(aLine, pos, offset);
	while(*b == 0)
		{
		b--; // Optimisation to skip over blanks
		pos -= offset+1;
		offset = 7;
		}

	while (offset >= 0 && ((*b) & (1<<offset)) == 0)
		{
		pos--;
		offset--;
		}
	ASSERT(IsScreenPositionValid(aLine, pos)); // Something's gone very wrong in the logic of this function if this assert is not true!
	return pos;
	}

void CLRTextView::CopyToClipboardL(const TDesC& aBuf)
	{
	LtkUtils::CopyToClipboardL(aBuf, &iConsoleProvider.Fs());
	}

HBufC* CLRTextView::GetFromClipboardL()
	{
	return LtkUtils::GetFromClipboardL(&iConsoleProvider.Fs());
	}

void CLRTextView::PasteL()
	{
	HBufC* text = GetFromClipboardL();
	CleanupStack::PushL(text);
	if (text->Length() == 0)
		{
		iConsoleProvider.InfoPrint(_L("Nothing to paste"));
		}
	else
		{
		InsertTextL(*text);
		}
	CleanupStack::PopAndDestroy(text);
	}

TPoint CLRTextView::WindowLocationForDocumentPosition(TInt aDocumentPosition) const
	{
	TPoint result(-1,-1); // We return this if doc pos is not on screen
	const TInt numScreenLines = iWindow.iHeight;
	if (aDocumentPosition >= iLineData->DocumentPosition(0) && aDocumentPosition < iLineData->DocumentPosition(numScreenLines))
		{
		// pos is on screen
		for (TInt line = 0; line < numScreenLines; line++)
			{
			if (aDocumentPosition < iLineData->DocumentPosition(line+1))
				{
				result.iY = line;
				TInt docPos = iLineData->DocumentPosition(line);
				TInt column = 0;
				// Find the column screen position whose document position is what we want
				for (; docPos <= aDocumentPosition; column++)
					{
					if (iLineData->IsScreenPositionValid(line, column))
						{
						docPos++;
						}
					}
				result.iX = column-1;
				break;
				}
			}
		}
	return result;
	}

void CLRTextView::SetMark()
	{
	iMarkFlasher->Cancel();
	iMarkDocPos = DocumentPosition();
	iMarkShown = EFalse;
	const TInt KFlashInterval = 500000;
	iMarkFlasher->Start(0, KFlashInterval, TCallBack(&FlashMark, this));
	UpdateStatus();
	iConsoleProvider.InfoPrint(_L("Mark set. Move cursor then press ^X/^C to cut/copy"));
	}

void CLRTextView::CancelMark()
	{
	if (MarkActive())
		{
		iMarkFlasher->Cancel();
		if (WindowLocationForDocumentPosition(iMarkDocPos).iY != -1)
			{
			// If the mark pos is on screen, redraw it to get rid of the '*'
			RedrawFromPositionToEndOfLine(iMarkDocPos, 0);
			}
		}
	}

TInt CLRTextView::FlashMark(TAny* aSelf)
	{
	static_cast<CLRTextView*>(aSelf)->DoFlashMark();
	return 1;
	}

void CLRTextView::DoFlashMark()
	{
	TPoint markPos = WindowLocationForDocumentPosition(iMarkDocPos);
	if (markPos.iY != -1)
		{
		TPoint cursor = iConsole.CursorPos();
		iConsole.SetCursorPosAbs(markPos);
		if (iMarkShown)
			iConsole.Write(_L(" "));
		else
			iConsole.Write(_L("*"));
		iMarkShown = !iMarkShown;
		iConsole.SetCursorPosAbs(cursor);
		}
	}

TBool CLRTextView::MarkActive() const
	{
	return iMarkFlasher->IsActive();
	}

void CLRTextView::CopyOrCutL(TBool aCut)
	{
	TInt docPos = DocumentPosition();
	const TInt start = Min(docPos, iMarkDocPos);
	const TInt len = Abs(docPos - iMarkDocPos);
	CancelMark();
	RBuf textCopy;
	CleanupClosePushL(textCopy);
	TInt dataPos = start;
	while(ETrue)
		{
		TInt err = iBuffer.GetData(*this, dataPos);
		if (err)
			{
			HandleDataLoadError(err);
			CleanupStack::PopAndDestroy(&textCopy);
			return;
			}
		else
			{
			TPtrC text = iDes.Left(len - textCopy.Length());
			if (textCopy.MaxLength() == 0 && len <= iRange.iLength)
				{
				// All fits in one buffer, no need to copy anything!
				CopyToClipboardL(text);
				break;
				}
			else
				{
				// Need to do some copying
				if (textCopy.MaxLength() == 0) textCopy.CreateL(len);
				textCopy.Append(text);
				dataPos += text.Length();
				if (textCopy.Length() == len)
					{
					// Done
					CopyToClipboardL(textCopy);
					break;
					}
				}
			}
		}
	CleanupStack::PopAndDestroy(&textCopy);
	if (aCut)
		{
		TRange range(start, len);
		iBuffer.DeleteTextL(range);
		iCursor = WindowLocationForDocumentPosition(start);
		if (iCursor.iY == -1)
			{
			// Start of the selection is offscreen - force a scroll
			CenterDocPosOnScreen(start);
			}
		else
			{
			iDrawPoint = iCursor + iWindow.Origin();
			RequestData(EFalse, start);
			}
		iConsoleProvider.InfoPrint(_L("Text cut to clipboard"));
		}
	else
		{
		iConsoleProvider.InfoPrint(_L("Text copied to clipboard"));
		}
	}

void CLRTextView::UpdateDocumentPositionsStartingAtLine(TInt aLine, TInt aDelta)
	{
	// This is important because even though we can optimise away the drawing, when a line has changed
	// length but not caused wrapping, we still need to make sure the document positions of all the
	// lines we didn't draw are correct to reflect the extra characters.
	for (TInt i = aLine; i < iWindow.iHeight+1; i++)
		{
		iLineData->SetDocumentPositionForLine(i, iLineData->DocumentPosition(i) + aDelta);
		}
	}
