// cmdwindow.cpp
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
#include "cmdwindow.h"
#include <e32cons.h>
#include <BADESCA.H>

const TInt KInfoPrintTime = 2500000; // 2.5 seconds

CCommandWindow* CCommandWindow::NewL(RFs& aFs, CColorConsoleBase& aConsole)
	{
	CCommandWindow* self = new(ELeave) CCommandWindow(aFs, aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCommandWindow* CCommandWindow::NewL(RFs& aFs, CConsoleBase& aConsole)
	{
	CCommandWindow* self = new(ELeave) CCommandWindow(aFs, aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCommandWindow::CCommandWindow(RFs& aFs, CConsoleBase& aConsole)
	: iFs(aFs), iConsole(aConsole), iColorConsole(NULL), iConsoleAdapter(aConsole)
	{}

CCommandWindow::CCommandWindow(RFs& aFs, CColorConsoleBase& aConsole)
	: iFs(aFs), iConsole(aConsole), iColorConsole(&aConsole), iConsoleAdapter(aConsole)
	{}

void CCommandWindow::ConstructL()
	{
	iInfoPrintDismisser = CPeriodic::NewL(CActive::EPriorityStandard);
	TBuf<64> buf;

	User::LeaveIfError(iFs.PrivatePath(buf));
	buf.Insert(0, _L("c:"));
	buf.Append(_L("fed_filehistory"));
	iFileNameLineEditor = CLineEditor::NewL(iFs, iConsoleAdapter, *this, *this, buf);

	User::LeaveIfError(iFs.PrivatePath(buf));
	buf.Insert(0, _L("c:"));
	buf.Append(_L("fed_generalhistory"));
	iGeneralLineEditor = CLineEditor::NewL(iFs, iConsoleAdapter, *this, *this, buf);
	}

CCommandWindow::~CCommandWindow()
	{
	if (iInfoPrintDismisser) iInfoPrintDismisser->Cancel();
	delete iInfoPrintDismisser;
	delete iGeneralLineEditor;
	delete iFileNameLineEditor;
	}

CConsoleBase& CCommandWindow::Console()
	{
	return iConsole;
	}

CColorConsoleBase* CCommandWindow::ColorConsole()
	{
	return iColorConsole;
	}

class TOverflow16 : public TDes16Overflow
	{
public:
	void Overflow(TDes16 &) {}
	};

void CCommandWindow::WriteStatus(const TDesC& aNameToTruncate, TRefByValue<const TDesC> aFmt, ...)
	{
	TDes& buf = iLastStatus;
	buf.Zero();
	VA_LIST args;
	VA_START(args, aFmt);
	TOverflow16 overflow;
	buf.AppendFormatList(aFmt, args, &overflow);
	VA_END(args);

	const TInt availWidth = iWindow.iWidth - 1;
	buf.SetLength(Min(buf.Length(), availWidth));
	TInt widthForName = availWidth - buf.Length();
	buf.Insert(0, aNameToTruncate.Right(widthForName));
	if (buf.Length() < iWindow.iWidth) buf.Append(' ');
	buf.AppendFill('-', iWindow.iWidth - buf.Length() - 1); // Don't fill to edge of screen, that causes a wrap

	DoWriteLine(buf);
	}

void CCommandWindow::DoWriteLine(const TDesC& aLine, TInt aHighlightStart, TInt aHighlightLength)
	{
	TPoint cursor = iConsole.CursorPos();
	iConsole.SetCursorHeight(0);
	iConsole.SetPos(iWindow.iX, iWindow.iY);
	if (aHighlightLength && ColorConsole())
		{
		TPtrC left = aLine.Left(aHighlightStart);
		TPtrC mid = aLine.Mid(aHighlightStart, aHighlightLength);
		TPtrC right = aLine.Mid(aHighlightStart + aHighlightLength);
		ColorConsole()->Write(left);
		ColorConsole()->SetTextAttribute(ETextAttributeInverse);
		ColorConsole()->Write(mid);
		ColorConsole()->SetTextAttribute(ETextAttributeNormal);
		ColorConsole()->Write(right);
		}
	else
		{
		iConsole.Write(aLine);
		}
	iConsole.SetCursorPosAbs(cursor);
	iConsole.SetCursorHeight(20);
	}

void CCommandWindow::InfoPrint(TRefByValue<const TDesC> aFmt, ...)
	{
	iInfoPrintDismisser->Cancel(); // If there's already a print onscreen, we bin it straight away (same as how User::InfoPrint works)
	TBuf<256> buf;
	VA_LIST args;
	VA_START(args, aFmt);
	TOverflow16 overflow;
	buf.AppendFormatList(aFmt, args, &overflow);
	VA_END(args);

	buf.SetLength(Min(buf.Length(), iWindow.iWidth-5)); // 4 for -[ and ]-, and one more to avoid hitting the final column
	TInt highlightLen = buf.Length() + 2; // Highlight the [] as well
	buf.Insert(0, _L("-["));
	buf.Append(_L("]-"));
	TInt fillWidth = iWindow.iWidth - buf.Length() - 1;
	TInt left = fillWidth / 2;
	TInt highlightStart = left+1;
	TInt right = fillWidth - left;
	while(left--)
		{
		buf.Insert(0, _L("-"));
		}
	buf.AppendFill('-', right);
	DoWriteLine(buf, highlightStart, highlightLen);
	iInfoPrintDismisser->Start(KInfoPrintTime, KInfoPrintTime, TCallBack(&DismissInfoPrint, this));
	}

TInt CCommandWindow::DismissInfoPrint(TAny* aSelf)
	{
	CCommandWindow* self = static_cast<CCommandWindow*>(aSelf);
	// Restore previous status
	self->DoWriteLine(self->iLastStatus);
	self->iInfoPrintDismisser->Cancel();
	return 0;
	}

TKeyCode CCommandWindow::Query(const TDesC& aPrompt, const TDesC& aValidKeys)
	{
	TPoint cursor = iConsole.CursorPos();
	iQuery = aPrompt;
	iQuery.Append(_L("[ ]"));
	iQuery.AppendFill('-', iQuery.MaxLength() - iQuery.Length());
	iQuery.SetLength(iWindow.iWidth - 1);
	DoWriteLine(iQuery);
	iConsole.SetPos(iWindow.iX + aPrompt.Length()+1, iWindow.iY);
	iConsole.SetCursorHeight(20);
	TKeyCode res = EKeyNull;
	while (aValidKeys.Locate(res) == KErrNotFound)
		{
		res = iConsole.Getch();
		}
	// Print the key that the user entered, between the square brackets
	if (TChar(res).IsPrint())
		{
		TBuf<1> chbuf;
		chbuf.Append(res);
		iConsole.Write(chbuf);
		}
	iConsole.SetCursorPosAbs(cursor);
	// Async clear the query (in case we are called repeatedly)
	iInfoPrintDismisser->Cancel();
	iInfoPrintDismisser->Start(0, KInfoPrintTime, TCallBack(&DismissInfoPrint, this));
	return res;
	}

TBool CCommandWindow::QueryText(const TDesC& aPrompt, TDes& aText)
	{
	iLineEditor = iGeneralLineEditor;
	return DoQueryText(aPrompt, aText);
	}

TBool CCommandWindow::QueryFilename(const TDesC& aPrompt, TFileName& aFileName)
	{
	iLineEditor = iFileNameLineEditor;
	TBool res = DoQueryText(aPrompt, aFileName);
	if (res)
		{
		IoUtils::TFileName2 fn2(aFileName);
		TRAPD(err, fn2.MakeAbsoluteL(iFs));
		if (err) res = EFalse;
		aFileName.Copy(fn2);
		}
	return res;
	}

TBool CCommandWindow::DoQueryText(const TDesC& aPrompt, TDes& aText)
	{
	// Get cursor in right position
	TPoint cursor = iConsole.CursorPos();
	// Clear the way
	iConsole.SetPos(iWindow.iX, iWindow.iY);
	iQuery.Fill(' ', iWindow.iWidth-1);
	iConsole.Write(iQuery);
	iConsole.SetPos(iWindow.iX, iWindow.iY);
	// Start the line editor going
	iLineEditor->Start(aPrompt, aText);
	iLineEditor->ReinstatePromptAndUserInput();
	iFinished = EFalse;
	TBool result = ETrue;
	iResultForQuery = &aText;
	while (!iFinished)
		{
		TKeyCode ch = iConsole.Getch();
		if (ch == EKeyEscape) 
			{
			result = EFalse;
			break;
			}
		if (ch == EKeyEnter)
			{
			// Fool line editor by moving cursor up a line, so when it does its newline it doesn't screw us up
			iConsole.SetCursorPosRel(TPoint(0, -1));
			}
		iLineEditor->HandleKey(ch, iConsole.KeyModifiers());
		}
	iConsole.SetCursorPosAbs(cursor);
	// Async clear the query (in case of... something. Velociraptors maybe?)
	iInfoPrintDismisser->Cancel();
	iInfoPrintDismisser->Start(0, KInfoPrintTime, TCallBack(&DismissInfoPrint, this));
	return result;
	}

RFs& CCommandWindow::Fs()
	{
	return iFs;
	}

void CCommandWindow::LeoHandleLine(const TDesC& aLine)
	{
	iFinished = ETrue;
	*iResultForQuery = aLine.Left(iResultForQuery->MaxLength());
	}

_LIT(KFileNameSlash, "\\");
_LIT(KDriveColonSlash, ":\\");
_LIT(KTab, "\t");
_LIT(KSpace, " ");

// This code ripped from ptf
void CCommandWindow::LcCompleteLineL(TConsoleLine& aLine, const TChar& /*aEscapeChar*/)
	{
	if (iLineEditor != iFileNameLineEditor) return; // Don't autocomplete for anything that isn't a filename

	IoUtils::TFileName2 fileNameMatch;
	fileNameMatch.Copy(aLine.ContentsToCursor());
	
	CDesCArray* suggestions = new(ELeave) CDesC16ArrayFlat(16);
	CleanupStack::PushL(suggestions);

	if (fileNameMatch.Length()==0)
		{
		SuggestDrivesL(suggestions);
		}
	else if (fileNameMatch.Length() == 2 && TChar(fileNameMatch[0]).IsAlpha() && fileNameMatch[1] == ':')
		{
		fileNameMatch.Append(KFileNameSlash);
		}

	if (!fileNameMatch.IsAbsolute())
		{
		fileNameMatch.MakeAbsoluteL(iFs);
		}
	
	TPtrC relativeDir = aLine.ContentsToCursor();
	relativeDir.Set(relativeDir.Left(relativeDir.LocateReverse('\\')+1)); // Remove any name to the right of the last backslash
		
	fileNameMatch.Append('*');
	
	CDir* files = NULL;
	User::LeaveIfError(iFs.GetDir(fileNameMatch, KEntryAttNormal | KEntryAttDir, ESortByName, files));
	CleanupStack::PushL(files);
		
	TInt numFiles = files->Count();
	if (numFiles == 1)
		{
		fileNameMatch.SetLength(fileNameMatch.DriveAndPath().Length());
		fileNameMatch.AppendComponentL((*files)[0].iName);
		TEntry entry;
		User::LeaveIfError(iFs.Entry(fileNameMatch, entry));
		suggestions->AppendL(fileNameMatch.NameAndExt());
		CompleteL(aLine, *suggestions, &relativeDir, entry.IsDir() ? &KFileNameSlash : NULL);
		}
	else if (numFiles > 0)
		{
		for (TInt i=0; i<numFiles; ++i)
			{
			suggestions->AppendL((*files)[i].iName);
			}
		CompleteL(aLine, *suggestions, &relativeDir, NULL);
		}
	
	CleanupStack::PopAndDestroy(2, suggestions); // files, suggestions
	}

void CCommandWindow::SuggestDrivesL(CDesCArray* aSuggestions)
	{
	TDriveList drives;
	User::LeaveIfError(iFs.DriveList(drives));
	
	for (TInt i=0; i<drives.Length(); ++i)
		{
		if (drives[i])
			{
			TBuf<3> buf;
			
			TChar driveChar;
			User::LeaveIfError(iFs.DriveToChar(i, driveChar));
			
			buf.Append(driveChar);
			buf.Append(KDriveColonSlash);
			aSuggestions->AppendL(buf);						
			}
		}
	}
	
void CCommandWindow::CompleteL(TConsoleLine& aLine, const CDesCArray& aPossibilities, const TDesC* aPrefix, const TDesC* aSuffix)
	{
	const TInt numPossibilities = aPossibilities.Count();

	if (numPossibilities > 1)
		{
		// Fill out possibilities buffer.
		IoUtils::CTextBuffer* possibilities = IoUtils::CTextBuffer::NewLC(0x100);
		for (TInt i = 0; i < numPossibilities; ++i)
			{
			possibilities->AppendL(aPossibilities[i]);
			if (i != (numPossibilities - 1))
				{
				possibilities->AppendL(KTab);
				}
			}

		aLine.PrintCompletionPossibilitiesL(possibilities->Descriptor());
		CleanupStack::PopAndDestroy(possibilities);
		}

	if (numPossibilities > 0)
		{
		IoUtils::CTextBuffer* completion = IoUtils::CTextBuffer::NewLC(0x100);
		TPtrC commonChars(NULL, 0);
		if (numPossibilities > 1)
			{
			// Find common leading characters of the possibilities.
			TInt commonCharPos = -1;
			TBool finished(EFalse);
			do
				{
				++commonCharPos;
				TChar character(0);
				for (TInt i = 0; i < numPossibilities; ++i)
					{
					if (commonCharPos >= aPossibilities[i].Length())
						{
						finished = ETrue;
						break;
						}
					else if (i == 0)
						{
						character = aPossibilities[0][commonCharPos];
						character.Fold();
						}
					else
						{
						TChar c(aPossibilities[i][commonCharPos]);
						c.Fold();
						if (c != character)
							{
							finished = ETrue;
							break;
							}
						}
					}
				}
				while (!finished);

			commonChars.Set(aPossibilities[0].Mid(0, commonCharPos));
			}
		else
			{
			commonChars.Set(aPossibilities[0]);
			}

		if (aPrefix)
			{
			completion->AppendL(*aPrefix);
			}
		completion->AppendL(commonChars);

		if ((numPossibilities == 1) && (aSuffix))
			{
			completion->AppendL(*aSuffix);
			}
			
		if ((numPossibilities == 1) && (!aSuffix))
			{
			completion->AppendL(KSpace);
			}
			
		if (completion->Descriptor().Length() > 0)
			{
			aLine.Replace(0, completion->Descriptor());
			}
		CleanupStack::PopAndDestroy(completion);
		}
	}
