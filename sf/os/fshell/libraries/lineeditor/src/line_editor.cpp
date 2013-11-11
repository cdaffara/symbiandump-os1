// line_editor.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "line_editor.h"
#include <f32file.h>
#include <fshell/ioutils.h>
#include <fshell/common.mmh>

using namespace IoUtils;


//
// Constants.
//

_LIT(KSpace, " ");
_LIT(KNewLine, "\r\n");
_LIT(KBackspace, "\x08 \x08");


//
// RConsole.
//

RConsole::RConsole(MAbstractConsoleWriter& aStdout)
	: iStdout(aStdout), iCursorPos(TPoint(0, 0)), iCursorMode(EInsert), iCursorVisible(EFalse)
	{
	// Calling these here cause fshell to deadlock if being launched within a "pcons start fshell".
	// Doesn't appear to cause any problems removing them, as Start()/Refresh() get called shortly after.
	//iStdout.GetScreenSize(iSize);
	//SetCursorVisible(ETrue);
	}

void RConsole::Close()
	{
	iScrollObservers.Close();
	}

void RConsole::Start()
	{
	Refresh();
	}
	
void RConsole::Refresh()
	{
	iStdout.GetScreenSize(iSize);
	iStdout.GetCursorPos(iCursorPos);
	}

void RConsole::Write(const TDesC &aDes)
	{
	const TInt desLength = aDes.Length();
	for (TInt i = 0; i < desLength; ++i)
		{
		switch(aDes[i])
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
			case 0x0b:	// Vertical tab.
			case 0x0c:  // Form feed.
				// These should already have been swallowed by CLineEditor::HandleKey.
				ASSERT(FALSE);
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
	iStdout.Write(aDes);
	}

void RConsole::SetCursorPosAbs(const TPoint& aPoint)
	{
	iCursorPos = aPoint;
	iStdout.SetCursorPosAbs(aPoint);
	}

void RConsole::SetCursorPosRel(const TPoint& aPoint)
	{
	iCursorPos += aPoint;
	iStdout.SetCursorPosRel(aPoint);
	}

void RConsole::MoveCursorLeft()
	{
	CursorLeft();
	iStdout.SetCursorPosAbs(iCursorPos);
	}

void RConsole::MoveCursorRight()
	{
	CursorRight();
	iStdout.SetCursorPosAbs(iCursorPos);
	}

void RConsole::SetCursorMode(TCursorMode aMode)
	{
	iCursorMode = aMode;
	SetCursorVisible(iCursorVisible);
	}

void RConsole::SetCursorVisible(TBool aVisible)
	{
	iCursorVisible = aVisible;
	iStdout.SetCursorHeight(iCursorVisible ? (iCursorMode == EInsert ? 20 : 100) : 0);
	}
	
void RConsole::ClearToEndOfLine()
	{
	iStdout.ClearToEndOfLine();
	// no affect on cursor position
	}

void RConsole::Clear()
	{
	iStdout.ClearScreen();
	iCursorPos.iX = iCursorPos.iY = 0;
	}

TPoint RConsole::PosFrom(const TPoint& aStartPos, TInt aLength) const
	{
	// if the width is 0 we'll enter an infinite loop below.
	if (iSize.iWidth == 0) return TPoint(aStartPos.iX + aLength, aStartPos.iY);
	
	TPoint endPos(aStartPos);
	if (aLength > 0)
		{
		while (aLength)
			{
			const TInt min = Min(iSize.iWidth - endPos.iX, aLength);
			aLength -= min;
			if ((endPos.iX + min) < iSize.iWidth)
				{
				endPos.iX += min;
				}
			else
				{
				ASSERT((endPos.iX + min) == iSize.iWidth);
				endPos.iX = 0;
				++endPos.iY;
				}
			}
		}
	else
		{
		aLength = -aLength;
		while (aLength)
			{
			const TInt min = Min(endPos.iX, aLength);
			if (min == 0)
				{
				endPos.iX = (iSize.iWidth - 1);
				--endPos.iY;
				--aLength;
				}
			else 
				{
				endPos.iX -= min;
				aLength -= min;
				}
			}
		}
	return endPos;
	}

TPoint RConsole::CursorPos() const
	{
	return iCursorPos;
	}

TInt RConsole::AddObserver(MConsoleScrollObserver& aObserver)
	{
	return iScrollObservers.Append(&aObserver);
	}

void RConsole::RemoveObserver(MConsoleScrollObserver& aObserver)
	{
	const TInt numObservers = iScrollObservers.Count();
	for (TInt i = 0; i < numObservers; ++i)
		{
		if (iScrollObservers[i] == &aObserver)
			{
			iScrollObservers.Remove(i);
			return;
			}
		}
	ASSERT(EFalse);
	}

TSize RConsole::Size() const
	{
	return iSize;
	}

void RConsole::CursorLeft()
	{
	if (iCursorPos.iX > 0)
		{
		// Not yet reached beginning of line.
		--iCursorPos.iX;
		}
	else if (iCursorPos.iY > 0)
		{
		// Reached beginning of line, so jump to end of line above.
		iCursorPos.iX = (iSize.iWidth - 1);
		--iCursorPos.iY;
		}
	}

void RConsole::CursorRight()
	{
	if (iCursorPos.iX < (iSize.iWidth - 1))
		{
		// Not yet reached the end of the line.
		++iCursorPos.iX;
		}
	else if (iCursorPos.iY < (iSize.iHeight - 1))
		{
		// Reached the end of the line and there's space below - jump to the beginning of the line below.
		iCursorPos.iX = 0;
		++iCursorPos.iY;
		}
	else
		{
		// Reached the end of the line and there's no space below - console will scroll up a line and jump to the beginning of the newly exposed line.
		iCursorPos.iX = 0;
		NotifyScrollObservers();
		}
	}

void RConsole::LineFeed()
	{
	if (iCursorPos.iY < (iSize.iHeight - 1))
		{
		iCursorPos.iX = 0;
		++iCursorPos.iY;
		}
	else
		{
		iCursorPos.iX = 0;
		NotifyScrollObservers();
		}
	}

void RConsole::CarriageReturn()
	{
	iCursorPos.iX = 0;
	}

void RConsole::NewLine()
	{
	iStdout.Write(KNewLine);
	iCursorPos.iX = 0;
	if (iCursorPos.iY < (iSize.iHeight - 1))
		{
		++iCursorPos.iY;
		}
	else
		{
		iCursorPos.iX = 0;
		NotifyScrollObservers();
		}
	}

void RConsole::NotifyScrollObservers()
	{
	const TInt numObservers = iScrollObservers.Count();
	for (TInt i = 0; i < numObservers; ++i)
		{
		iScrollObservers[i]->CsoHandleConsoleScrolled();
		}
	}


//
// TConsoleLine.
//

EXPORT_C TConsoleLine::TConsoleLine(RConsole& aConsole)
	: iConsole(aConsole), iPromptLength(0), iBufPos(0), iStartPos(aConsole.CursorPos()), iHidden(ETrue)
	{
	}

EXPORT_C void TConsoleLine::Start(const TDesC& aPrompt)
	{
	Start(aPrompt, KNullDesC);
	}

EXPORT_C void TConsoleLine::Start(const TDesC& aPrompt, const TDesC& aInitialInput)
	{
	if (iStarted)
		{
		Restart(aPrompt);
		return;
		}

	iStarted = ETrue;
	if (!iHidden)
		{
		iConsole.AddObserver(*this);
		}
	iConsole.Start();
	iBuf = aPrompt;
	iBuf.Append(aInitialInput);
	iPromptLength = aPrompt.Length();
	iBufPos = iBuf.Length();
	if (!iHidden)
		{
		Redraw();
		iConsole.SetCursorVisible(ETrue);
		}
	}

void TConsoleLine::Abort()
	{
	iStarted = EFalse;
	iConsole.RemoveObserver(*this);
	}

void TConsoleLine::Restart(const TDesC& aPrompt)
	{
	// a) Hide the cursor.
	// b) Move the cursor to the start of the line.
	// c) Create a buffer that will fully overwrite the old line.
	// d) Write the buffer to the console.
	// e) Set the cursor to the end of the new prompt.
	// f) Make the cursor visible again.
	iConsole.SetCursorVisible(EFalse);
	iConsole.SetCursorPosAbs(iStartPos);
	const TInt origBufLength = iBuf.Length();
	iBuf = aPrompt;
	if (iBuf.Length() < origBufLength)
		{
		iBuf.AppendFill(' ', origBufLength - iBuf.Length());
		}
	iConsole.Write(iBuf);
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, aPrompt.Length()));
	iBuf.SetLength(aPrompt.Length());
	iBufPos = aPrompt.Length();
	iConsole.SetCursorVisible(ETrue);
	}

EXPORT_C void TConsoleLine::Insert(TChar aChar)
	{
	TPtrC charDes((TUint16*)&aChar, 1);
	const TInt availableSpace = iBuf.MaxLength() - iBuf.Length();
	if (iBufPos < iBuf.Length())
		{
		if (availableSpace < 1)
			{
			iBuf.Delete(iBuf.Length() - 1, 1);
			}
		iBuf.Insert(iBufPos, charDes);
		iConsole.SetCursorVisible(EFalse);
		iConsole.Write(iBuf.Mid(iBufPos));
		iConsole.SetCursorPosAbs(iConsole.PosFrom(iConsole.CursorPos(), iBufPos - iBuf.Length() + 1));
		iConsole.SetCursorVisible(ETrue);
		iBufPos++;
		}
	else
		{
		ASSERT(iBufPos == iBuf.Length());
		if (availableSpace > 0)
			{
			++iBufPos;
			iBuf.Append(charDes);
			iConsole.Write(charDes);
			}
		}
	}

EXPORT_C void TConsoleLine::Overwrite(TChar aChar)
	{
	if (iBufPos < iBuf.MaxLength())
		{
		TPtrC charDes((TUint16*)&aChar, 1);
		if (iBufPos < iBuf.Length())
			{
			iBuf.Replace(iBufPos++, 1, charDes);
			}
		else
			{
			ASSERT(iBufPos == iBuf.Length());
			iBuf.Append(charDes);
			++iBufPos;
			}
		iConsole.Write(charDes);
		}
	}

EXPORT_C void TConsoleLine::Replace(const TDesC& aDes)
	{
	// a) Hide the cursor.
	// b) Move the cursor to the start of the line (after the prompt).
	// c) Create a buffer that will fully overwrite the old line.
	// d) Write the buffer to the console.
	// e) Set the cursor to the end of the new line contents.
	// f) Make the cursor visible again.
	iConsole.SetCursorVisible(EFalse);
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, iPromptLength));
	const TInt origBufLength = iBuf.Length() - iPromptLength;
	iBuf.Delete(iPromptLength, origBufLength);
	iBuf.Append(aDes);
	if (aDes.Length() < origBufLength)
		{
		iBuf.AppendFill(' ', origBufLength - aDes.Length());
		}
	iConsole.Write(iBuf.Mid(iPromptLength));
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, iPromptLength + aDes.Length()));
	iBuf.SetLength(iPromptLength + aDes.Length());
	iBufPos = iPromptLength + aDes.Length();
	iConsole.SetCursorVisible(ETrue);
	}

EXPORT_C void TConsoleLine::Replace(TInt aFrom, const TDesC& aDes)
	{
	// a) Hide the cursor.
	// b) Move the cursor to aFrom.
	// c) Overwrite chars up to iBufPos.
	// d) Insert remaining chars (if any).
	// e) Make the cursor visible again.
	aFrom += iPromptLength;
	ASSERT(iBufPos >= aFrom);
	iConsole.SetCursorVisible(EFalse);
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, aFrom));
	const TInt desLength = aDes.Length();
	const TInt numOverwriteChars = iBufPos - aFrom;
	TInt lengthAdjust = 0;
	if (numOverwriteChars > 0)
		{
		iBuf.Replace(aFrom, numOverwriteChars, aDes.Left(numOverwriteChars));
		if (desLength < numOverwriteChars)
			{
			lengthAdjust = numOverwriteChars - desLength;
			iBuf.AppendFill(' ', lengthAdjust);
			iBufPos -= lengthAdjust;
			}
		}
	const TInt numInsertChars = desLength - numOverwriteChars;
	if (numInsertChars > 0)
		{
		iBuf.Insert(iBufPos, aDes.Mid(numOverwriteChars));
		iBufPos += numInsertChars;
		}
	iConsole.Write(iBuf.Mid(aFrom));
	iBuf.SetLength(iBuf.Length() - lengthAdjust);
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, aFrom + aDes.Length()));
	iConsole.SetCursorVisible(ETrue);
	}

EXPORT_C void TConsoleLine::Redraw()
	{
	if (!iStarted) return;
	iConsole.Refresh();
	iStartPos = iConsole.CursorPos();
	if (iStartPos.iX > 0)
		{
		iConsole.NewLine();
		iStartPos = iConsole.CursorPos();
		}
	iConsole.Write(iBuf);
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, iBufPos));
	}

EXPORT_C void TConsoleLine::End()
	{
	iStarted = EFalse;
	if (!iHidden)
		{
		iConsole.RemoveObserver(*this);
		if (iBufPos < iBuf.Length())
			{
			iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, iBuf.Length()));
			}
		iConsole.NewLine();
		}
	}

EXPORT_C void TConsoleLine::DeleteLeft()
	{
	if (iBufPos > iPromptLength)
		{
		iBuf.Delete(--iBufPos, 1);
		iConsole.SetCursorVisible(EFalse);
		if (iBufPos == iBuf.Length())
			{
			iConsole.Write(KBackspace);
			}
		else
			{
			iBuf.Append(KSpace);
			iConsole.MoveCursorLeft();
			TPoint cursorPos(iConsole.CursorPos());
			iConsole.Write(iBuf.Mid(iBufPos));
			iBuf.SetLength(iBuf.Length() - 1);
			iConsole.SetCursorPosAbs(cursorPos);
			}
		iConsole.SetCursorVisible(ETrue);
		}
	}

EXPORT_C void TConsoleLine::DeleteRight()
	{
	if (iBufPos < iBuf.Length())
		{
		iBuf.Delete(iBufPos, 1);
		iConsole.SetCursorVisible(EFalse);
		TPoint cursorPos(iConsole.CursorPos());
		iBuf.Append(KSpace);
		iConsole.Write(iBuf.Mid(iBufPos));
		iBuf.SetLength(iBuf.Length() - 1);
		iConsole.SetCursorPosAbs(cursorPos);
		iConsole.SetCursorVisible(ETrue);
		}
	}

EXPORT_C void TConsoleLine::CursorLeft()
	{
	if (iBufPos > iPromptLength)
		{
		--iBufPos;
		iConsole.MoveCursorLeft();
		}
	}

EXPORT_C void TConsoleLine::CursorRight()
	{
	if (iBufPos < iBuf.Length())
		{
		++iBufPos;
		iConsole.MoveCursorRight();
		}
	}

EXPORT_C void TConsoleLine::CursorPreviousWord()
	{
	TLex lex(iBuf);
	lex.Inc(iBufPos - 1);
	while ((lex.Offset() > iPromptLength) && lex.Peek().IsSpace())
		{
		lex.UnGet();
		}
	while ((lex.Offset() > iPromptLength) && !lex.Peek().IsSpace())
		{
		lex.UnGet();
		}
	lex.SkipSpace();
	iBufPos = Max(iPromptLength, lex.Offset());
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, iBufPos));
	}

EXPORT_C void TConsoleLine::CursorNextWord()
	{
	TLex lex(iBuf);
	lex.Inc(iBufPos);
	lex.SkipSpace();
	lex.SkipCharacters();
	iBufPos = lex.Offset();
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, iBufPos));
	}

EXPORT_C void TConsoleLine::CursorBeginning()
	{
	iBufPos = iPromptLength;
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, iPromptLength));
	}

EXPORT_C void TConsoleLine::CursorEnd()
	{
	iBufPos = iBuf.Length();
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, iBuf.Length()));
	}

EXPORT_C void TConsoleLine::PrintCompletionPossibilitiesL(const TDesC& aPossibilities)
	{
	if (iBufPos < iBuf.Length())
		{
		iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, iBuf.Length()));
		}
	iConsole.NewLine();
	CTextFormatter* formatter = CTextFormatter::NewLC(iConsole.Size().iWidth);
	formatter->ColumnizeL(0, 2, aPossibilities);
	iConsole.Write(formatter->Descriptor());
	CleanupStack::PopAndDestroy(formatter);
	iStartPos = iConsole.CursorPos();
	iConsole.Write(iBuf);
	iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, iBufPos));
	}

EXPORT_C TPtrC TConsoleLine::Contents() const
	{
	return iBuf.Mid(iPromptLength);
	}

EXPORT_C TPtrC TConsoleLine::ContentsToCursor() const
	{
	return iBuf.Mid(iPromptLength, iBufPos - iPromptLength);
	}

void TConsoleLine::CsoHandleConsoleScrolled()
	{
	--iStartPos.iY;
	}
	
void TConsoleLine::Hide()
	{
	if (iHidden) return;
	iHidden = ETrue;
	if (!iStarted) return;
	iConsole.RemoveObserver(*this);
	iConsole.SetCursorPosAbs(TPoint(0, iConsole.CursorPos().iY));
	iConsole.ClearToEndOfLine();
	iConsole.SetCursorVisible(EFalse);
	}
	
void TConsoleLine::Show()
	{
	if (!iHidden) return;
	iHidden = EFalse;
	if (!iStarted) return;

	iConsole.AddObserver(*this);
	iConsole.Start();
	Redraw();
	iConsole.SetCursorVisible(ETrue);
	}

void TConsoleLine::SetCursorPosition(TInt aPosition)
	{
	TInt newPos = iPromptLength + aPosition;
	if (newPos < iBuf.Length())
		{
		iBufPos = newPos;
		iConsole.SetCursorPosAbs(iConsole.PosFrom(iStartPos, newPos));
		}
	}

//
// CLineHistory.
//

NONSHARABLE_CLASS(CLineHistory) : public CBase
	{
public:
	enum TRecallType
		{
		ENext,
		EPrevious,
		EFirst,
		ELast
		};
public:
	static CLineHistory* NewL(RFs& aFs, TInt aMaximumSize, const TDesC& aFileName);
	~CLineHistory();
	void AddL(const TDesC& aLine, TInt aPos=0);
	const TDesC& Recall(TRecallType aType);
	const TDesC& RecallMatch(const TDesC& aMatch); // Always searches backwards
	void Reset();
private:
	CLineHistory(RFs& aFs, TInt aMaximumSize, const TDesC& aFileName);
	void ConstructL();
	void RestoreL();
	void StoreL();
	TInt Find(const TDesC& aLine);
	void ResetBackgroundWriter();
	static TInt BackgroundWriterCallback(TAny* aSelf);
	void DoBackgroundWriterCallback();
private:
	RFs& iFs;
	const TInt iMaximumSize;
	TFileName iFileName;
	TInt iIndex;
	RPointerArray<HBufC> iHistory;
	CPeriodic* iBackgroundWriter;
	TTime iLastChangedTime;
	TTime iLastSyncedTime;
	};

CLineHistory* CLineHistory::NewL(RFs& aFs, TInt aMaximumSize, const TDesC& aFileName)
	{
	CLineHistory* self = new(ELeave) CLineHistory(aFs, aMaximumSize, aFileName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CLineHistory::~CLineHistory()
	{
#ifdef EKA2
	TRAP_IGNORE(StoreL());
#else
	TRAPD(err, StoreL());
#endif
	iHistory.ResetAndDestroy();
	delete iBackgroundWriter;
	}

void CLineHistory::AddL(const TDesC& aLine, TInt aPos)
	{
	if (aLine.Length() > 0)
		{
		TInt index = Find(aLine);
		if (index == 0)
			{
			// Already present and at begining - nothing to do.
			}
		else if (index > 0)
			{
			// Already present - move to beginning.
			HBufC* line = iHistory[index];
			iHistory.Remove(index);
			iHistory.Insert(line, aPos); // Should not fail due to the above deletion.
			iLastChangedTime.UniversalTime();
			}
		else 
			{
			if (iHistory.Count() == iMaximumSize)
				{
				// History at maximum size - throw away the oldest entry.
				delete iHistory[iMaximumSize - 1];
				iHistory.Remove(iMaximumSize - 1);
				}
			HBufC* line = aLine.AllocLC();
			User::LeaveIfError(iHistory.Insert(line, aPos));
			CleanupStack::Pop(line);
			iLastChangedTime.UniversalTime();
			}
		}
	iIndex = -1;
	ResetBackgroundWriter();
	}

const TDesC& CLineHistory::Recall(TRecallType aType)
	{
	switch (aType)
		{
		case ENext:
			{
			if (iIndex >= 0)
				{
				--iIndex;
				}
			break;
			}
		case EPrevious:
			{
			if (iIndex < (iHistory.Count() - 1))
				{
				++iIndex;
				}
			break;
			}
		case EFirst:
			{
			iIndex = 0;
			break;
			}
		case ELast:
			{
			iIndex = (iHistory.Count() - 1);
			break;
			}
		default:
			{
			ASSERT(EFalse);
			break;
			}
		}

	if ((iIndex >= 0) && (iIndex < iHistory.Count()))
		{
		return *iHistory[iIndex];
		}
	else
		{
		return KNullDesC();
		}
	}

CLineHistory::CLineHistory(RFs& aFs, TInt aMaximumSize, const TDesC& aFileName)
	: iFs(aFs), iMaximumSize(aMaximumSize), iFileName(aFileName), iIndex(-1)
	{
	}

void CLineHistory::ConstructL()
	{
	if (CActiveScheduler::Current())
		{
		// Line editor may be run without a scheduler, in which case we can't try using an AO.
		iBackgroundWriter = CPeriodic::NewL(CActive::EPriorityStandard);
		}

	TRAPD(err, RestoreL());
	if (err)
		{
		iHistory.ResetAndDestroy();
		iFs.Delete(iFileName);
		}
	}

void CLineHistory::RestoreL()
	{
	if (iFileName.Length() == 0) return;

	RFile file;
	TInt err = file.Open(iFs, iFileName, EFileRead);
	if (err == KErrNone)
		{
		CleanupClosePushL(file);
		TInt32 int32;
		TPckg<TInt32> intPckg(int32);
		User::LeaveIfError(file.Read(intPckg));
		const TInt historyCount = Min(int32, iMaximumSize);
		for (TInt i = 0; i < historyCount; ++i)
			{
			User::LeaveIfError(file.Read(intPckg));
			iFs.Delete(KNullDesC);
			HBufC* line = HBufC::NewLC(int32);
			TPtr linePtr(line->Des());
			TInt size = int32 * 2;
			TPtr8 linePtr8((TUint8*)linePtr.Ptr(), 0, size);
			User::LeaveIfError(file.Read(linePtr8, size));
			linePtr.SetLength(int32);
			User::LeaveIfError(iHistory.Append(line));
			CleanupStack::Pop(line);
			}
		iLastSyncedTime.UniversalTime();
		CleanupStack::PopAndDestroy(&file);
		}
	else if ((err == KErrNotFound) || (err == KErrPathNotFound))
		{
		// Ignore error - leave history empty.
		}
	else
		{
		User::Leave(err);
		}
	}

void CLineHistory::StoreL()
	{
	if (iFileName.Length() == 0) return; // User clearly doesn't want to be persisted

	TTime modified;
	TInt err = iFs.Modified(iFileName, modified);
	if (err == KErrNone && modified > iLastSyncedTime)
		{
		// Someone else has updated our history under our feet - have a stab at merging them
		// For the moment we'll follow the algorithm of:
		// * In newest-first order, insert anything in the history file that isn't in ours (maintaining the newest-first order)
		// * Stop if we get to half the max history size, otherwise we'll risk reinserting stuff that we've just evicted, or something. Not quite sure that's totally correct but sounds fairly sensible.
		CLineHistory* newerHistory = CLineHistory::NewL(iFs, iMaximumSize, iFileName);
		newerHistory->iFileName.Zero(); // So newerHistory doesn't try and overwrite our history when we delete it
		CleanupStack::PushL(newerHistory);
		iLastSyncedTime.UniversalTime(); // Otherwise the AddLs below will go recursive...
		const TInt historyCount = newerHistory->iHistory.Count();
		TInt insertionPoint = 0;
		for (TInt i = 0; i < historyCount; i++)
			{
			HBufC* line = newerHistory->iHistory[i];
			TInt index = Find(*line);
			if (index == KErrNotFound)
				{
				AddL(*line, insertionPoint);
				insertionPoint++;
				}
			if (i >= iMaximumSize/2) break;
			}
		CleanupStack::PopAndDestroy(newerHistory);
		}

	iFs.MkDirAll(iFileName);
	RFile file;
	User::LeaveIfError(file.Replace(iFs, iFileName, EFileWrite));
	CleanupClosePushL(file);
	const TInt32 historyCount = iHistory.Count();
	TPckgC<TInt32> countPckg(historyCount);
	User::LeaveIfError(file.Write(countPckg));
	for (TInt i = 0; i < historyCount; ++i)
		{
		const HBufC* line = iHistory[i];
		const TInt32 length = line->Length();
		TPckgC<TInt32> lengthPckg(length);
		User::LeaveIfError(file.Write(lengthPckg));
		TPtrC8 linePtr8((const TUint8*)line->Ptr(), length * 2);
		User::LeaveIfError(file.Write(linePtr8));
		}
	CleanupStack::PopAndDestroy(&file);
	iLastSyncedTime.UniversalTime();
	}

TInt CLineHistory::Find(const TDesC& aLine)
	{
	const TInt historyCount = iHistory.Count();
	for (TInt i = 0; i < historyCount; ++i)
		{
		if (aLine == *iHistory[i])
			{
			return i;
			}
		}
	return KErrNotFound;
	}

void CLineHistory::Reset()
	{
	iIndex = -1;
	}

const TDesC& CLineHistory::RecallMatch(const TDesC& aMatch)
	{
	const TInt historyCount = iHistory.Count();
	TInt oldIndex = iIndex;
	for (iIndex = iIndex+1; iIndex < historyCount; iIndex++)
		{
		const TDesC& line = *iHistory[iIndex];
		if (line.Left(aMatch.Length()).CompareF(aMatch) == 0)
			{
			return line;
			}
		}
	// If nothing found, restore iIndex to what it was
	iIndex = oldIndex;
	return KNullDesC;
	}

const static TInt KWriterPeriod = 10*1000*1000; // 10 seconds

void CLineHistory::ResetBackgroundWriter()
	{
	if (iLastChangedTime.Int64() > iLastSyncedTime.Int64() + KWriterPeriod)
		{
		// For whatever reason (probably that the scheduler doesn't exist or isn't running) it's been ages since we last persisted the history
		// but the background writer hasn't kicked in - so do it now
		DoBackgroundWriterCallback();
		}
	else if (iBackgroundWriter)
		{
		iBackgroundWriter->Cancel();
		iBackgroundWriter->Start(KWriterPeriod, KWriterPeriod, TCallBack(&BackgroundWriterCallback, this));
		}
	}

TInt CLineHistory::BackgroundWriterCallback(TAny* aSelf)
	{
	static_cast<CLineHistory*>(aSelf)->DoBackgroundWriterCallback();
	return KErrNone;
	}

void CLineHistory::DoBackgroundWriterCallback()
	{
	if (iBackgroundWriter) iBackgroundWriter->Cancel(); // No point the periodic going off repeatedly if nothing has actually changed. We'll restart it in ResetBackgroundWriter if the history actually changes

	if (iLastChangedTime.Int64() > iLastSyncedTime.Int64())
		{
		TRAP_IGNORE(StoreL());
		}
	}

//
// CLineEditor.
//
	
EXPORT_C CLineEditor* CLineEditor::NewL(RFs& aFs, 
										MAbstractConsoleWriter& aStdout, 
										MLineEditorObserver& aObserver, 
										MLineCompleter& aCompleter,
										const TDesC& aConsoleHistoryFile)
	{
	CLineEditor* self = new(ELeave) CLineEditor(aStdout, aObserver, aCompleter);
	CleanupStack::PushL(self);
	self->ConstructL(aFs, aConsoleHistoryFile);
	CleanupStack::Pop(self);
	return self;	
	}


EXPORT_C CLineEditor::~CLineEditor()
	{
	delete iHistory;
	iConsole.Close();
	}

CLineEditor::CLineEditor(MAbstractConsoleWriter& aStdout, MLineEditorObserver& aObserver, MLineCompleter& aCompleter)
	: iObserver(aObserver), iCompleter(aCompleter), iConsole(aStdout), iLine(iConsole)
	{
	}

void CLineEditor::ConstructL(RFs& aFs, const TDesC& aConsoleHistoryFile)
	{
	iHistory = CLineHistory::NewL(aFs, 50, aConsoleHistoryFile);
	}

EXPORT_C void CLineEditor::HandleKey(TUint aKeyCode, TUint aModifiers)
	{
	iLine.Show();
	TBool handled(EFalse);
	if (aModifiers & EModifierFunc)
		{
		handled = ETrue;
		switch (aKeyCode)
			{
			case 'b':
			case EKeyLeftArrow:
				{
				HandlePreviousWord();
				break;
				}
			case 'f':
			case EKeyRightArrow:
				{
				HandleNextWord();
				break;
				}
			default:
				{
				handled = EFalse;
				}
			}
		}

	if (!handled)
		{
		switch (aKeyCode)
			{
			case EKeyEnter:
#ifdef FSHELL_PLATFORM_S60
			case EKeyDevice3: // confirm key
#endif
				{
				HandleEnter();
				break;
				}
			case EKeyBackspace:
				{
				HandleBackspace();
				break;
				}
			case EKeyTab:
				{
				HandleTab();
				break;
				};
			case CTRL('d'):
			case EKeyDelete:
				{
				HandleDelete();
				break;
				}
			case CTRL('b'):
			case EKeyLeftArrow:
				{
				HandleLeftArrow();
				break;
				}
			case CTRL('f'):
			case EKeyRightArrow:
				{
				HandleRightArrow();
				break;
				}
			case CTRL('p'):
			case EKeyUpArrow:
				{
				HandleUpArrow();
				break;
				}
			case CTRL('n'):
			case EKeyDownArrow:
				{
				HandleDownArrow();
				break;
				}
			case CTRL('a'):
			case EKeyHome:
				{
				HandleHome();
				break;
				}
			case CTRL('e'):
			case EKeyEnd:
				{
				HandleEnd();
				break;
				}
			case EKeyPageUp:
				{
				HandlePageUp();
				break;
				}
			case EKeyPageDown:
				{
				HandlePageDown();
				break;
				}
			case EKeyInsert:
				{
				HandleInsert();
				break;
				}
			case EKeyEscape:
				{
				HandleEscape();
				break;
				}
			case EKeyF4:
			case EKeyF8:
				{
				HandleF8Completion();
				break;
				}
			default:
				{
				if ((aKeyCode >= EKeySpace) && (aKeyCode < ENonCharacterKeyBase)) 
					{
					InsertChar(aKeyCode);
					}
				break;
				}
			}
		}
	}

EXPORT_C void CLineEditor::Start(const TDesC& aPrompt)
	{
	iLine.Start(aPrompt, KNullDesC);
	}

EXPORT_C void CLineEditor::Start(const TDesC& aPrompt, const TDesC& aInitialInput)
	{
	iLine.Start(aPrompt, aInitialInput);
	}

EXPORT_C void CLineEditor::Abort()
	{
	iLine.Abort();
	}
	
EXPORT_C void CLineEditor::Redraw()
	{
	iLine.Redraw();
	}
	
EXPORT_C void CLineEditor::RemovePromptAndUserInput()
	{
	iLine.Hide();
	}
	
EXPORT_C void CLineEditor::ReinstatePromptAndUserInput()
	{
	iLine.Show();
	}

void CLineEditor::InsertChar(TChar aChar)
	{
	(iMode == EInsert) ? iLine.Insert(aChar) : iLine.Overwrite(aChar);
	SetState(EEditing);
	}

void CLineEditor::HandleEnter()
	{
	if (iLine.Contents().Length() > 0)
		{
		iHistory->AddL(iLine.Contents());
		}
	iLine.End();
	iObserver.LeoHandleLine(iLine.Contents());
	SetState(EIdle);
	}

void CLineEditor::HandleBackspace()
	{
	SetState(EEditing);
	iLine.DeleteLeft();
	}

void CLineEditor::HandleTab()
	{
	SetState(EEditing);
#ifdef EKA2
	TRAP_IGNORE(HandleTabL());
#else
	TRAPD(err, HandleTabL());
#endif
	}

void CLineEditor::HandleTabL()
	{
	iCompleter.LcCompleteLineL(iLine, TChar('/'));
	}

void CLineEditor::HandleDelete()
	{
	SetState(EEditing);
	iLine.DeleteRight();
	}

void CLineEditor::HandleLeftArrow()
	{
	SetState(EEditing);
	iLine.CursorLeft();
	}

void CLineEditor::HandleRightArrow()
	{
	SetState(EEditing);
	iLine.CursorRight();
	}

void CLineEditor::HandleUpArrow()
	{
	ReplaceLine(iHistory->Recall(CLineHistory::EPrevious));
	}

void CLineEditor::HandleDownArrow()
	{
	ReplaceLine(iHistory->Recall(CLineHistory::ENext));
	}

void CLineEditor::HandlePreviousWord()
	{
	SetState(EEditing);
	iLine.CursorPreviousWord();
	}

void CLineEditor::HandleNextWord()
	{
	SetState(EEditing);
	iLine.CursorNextWord();
	}

void CLineEditor::HandleHome()
	{
	SetState(EEditing);
	iLine.CursorBeginning();
	}

void CLineEditor::HandleEnd()
	{
	SetState(EEditing);
	iLine.CursorEnd();
	}

void CLineEditor::HandlePageUp()
	{
	ReplaceLine(iHistory->Recall(CLineHistory::ELast));
	}

void CLineEditor::HandlePageDown()
	{
	ReplaceLine(iHistory->Recall(CLineHistory::EFirst));
	}

void CLineEditor::HandleInsert()
	{
	SetState(EEditing);
	if (iMode == EInsert)
		{
		iMode = EOverwrite;
		iConsole.SetCursorMode(RConsole::EOverwrite);
		}
	else
		{
		iMode = EInsert;
		iConsole.SetCursorMode(RConsole::EInsert);
		}
	}
	
void CLineEditor::HandleEscape()
	{
	SetState(EEditing);
	iLine.Replace(KNullDesC);	
	}


void CLineEditor::ReplaceLine(const TDesC& aNewLine)
	{
	TPtrC newLine(aNewLine);

	if (iState == EEditing)
		{
		if (aNewLine.Length() == 0)
			{
			return;
			}
		iLineBackup = iLine.Contents();
		SetState(ERecallingHistory);
		}
	else if ((iState == ERecallingHistory) && (aNewLine.Length() == 0))
		{
		SetState(EEditing);
		newLine.Set(iLineBackup);
		}
	else
		{
		SetState(ERecallingHistory);
		}

	iLine.Replace(newLine);
	}

void CLineEditor::SetState(TState aState)
	{
	if ((aState == EEditing) && (iState != EEditing))
		{
		iHistory->Reset();
		}
	iState = aState;
	}

void CLineEditor::HandleF8Completion()
	{
	// Search the history backwards for a line matching everything currently entered
	TPtrC toMatch = iLine.ContentsToCursor();
	TPtrC history = iHistory->RecallMatch(toMatch);
	if (history.Length())
		{
		ReplaceLine(history);
		iLine.SetCursorPosition(toMatch.Length());
		}
	}
