// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Send/Expect Algorithms
// 
//

#include "ATSTD.H"
#include "SCHAT.H"
#include "mSLOGGER.H"

CCommChatString* CCommChatString::NewL(const TDesC8& aDes, TBool aIsPartialLine)
	{
	CCommChatString* cs = new (ELeave) CCommChatString;
	CleanupStack::PushL(cs);
	cs->CreateL(aDes, aIsPartialLine);
	CleanupStack::Pop();
	return cs;
	}

CCommChatString::CCommChatString()
	{
	}

CCommChatString::~CCommChatString()
	{
	}

void CCommChatString::CreateL(const TDesC8& aDes, TBool aIsPartialLine)
	{
	iIsPartialLine = aIsPartialLine;
	iMatch.Set(aDes);
	}

//
// CCommChatter
//
CCommChatter::CCommChatter(MCommChatNotify* aNotify, TInt aPriority)
	: CTimer(aPriority), iNotify(aNotify)
	{
	CActiveScheduler::Add(this);
	iList.SetOffset(_FOFF(CCommChatString,iLink));
	}

CCommChatter::~CCommChatter()
	{
	DeleteAllAndStop();
	User::Free(iBuffer);
	}

void CCommChatter::CreateL(TInt aBufSize)
	{
	CTimer::ConstructL();
	iBuffer = (TText8*)User::AllocL(aBufSize);
	iBufferEnd = (iBuffer+aBufSize)-1;
	ClearBuffer();
	}

void CCommChatter::ClearBuffer()
//
// Empty history
//
	{
	iLastChar = iBuffer;
	iLineStart = iBuffer;
	iInDelimiter = ETrue;	// as though the last character was the end of a line
	}

void CCommChatter::ClearCurrentLine()
//
// Remove current line, which is always at the end of the buffer
//
	{
	iLastChar = iLineStart;
	iInDelimiter = ETrue;	// as though the last character was the end of a line
	}

TPtrC8 CCommChatter::Buffer() const
	{
	return TPtrC8(iBuffer, iLastChar-iBuffer);
	}

TPtrC8 CCommChatter::CurrentLine() const
	{
	TInt len=iLastChar-iLineStart;
	if (len>0 && iInDelimiter)
		len-=1;
	return TPtrC8(iLineStart, len);
	}

void CCommChatter::AddCharL(TText8 aChar)
//
// Add a character to the history buffer
// Scan all strings to find any matches that may
// be completed.
//
	{
	// Prevent reading of multiple delimiters into buffer
	if (iInDelimiter && (aChar == '\r' || aChar == '\n'))
		return;

	*iLastChar++=aChar;
	if (iLastChar>=iBufferEnd)
		{
		iLastChar = iBufferEnd; // Discarding characters at this point
		}

	if (aChar=='\r' || aChar=='\n')
		{
		if (iInDelimiter)
			return;		// still in delimiting sequence
		iInDelimiter=ETrue;
		}
	else
		{
		if (iInDelimiter)
			{
			iInDelimiter=EFalse;
			iLineStart=iLastChar-1;
			}
		if (iPartLineMatchers==0)
			return;		// wait for end of line before trying to match
		}

	// Scan for matching expect string

	TPtrC8 line(CurrentLine());
	CCommChatString* cs;
	TDblQueIter<CCommChatString> iter(iList);

	if (iInDelimiter)
		{
		// Simple match at end of line - test all strings
		while (cs = iter++, cs!=NULL)
			{
			if (line.Match(cs->iMatch)==0)
				{
				LOGTEXT2(_L8("SChat:\tFound match against =>%S<"), &cs->iMatch);
				iNotify->ChatStringMatchL(cs);
				}
			}
		}
	else
		{
		// partial match - use only selected matchers
		while (cs = iter++, cs!=NULL)
			{
			if (cs->iIsPartialLine && line.Match(cs->iMatch)==0)
				{
				LOGTEXT2(_L8("SChat:\tFound match against partial line =>%S<"), &cs->iMatch);
				iNotify->ChatStringMatchL(cs);
				}
			}
		}
	}

void CCommChatter::RunL()
	{
	iNotify->ChatTimeout();
	}

CCommChatString* CCommChatter::AddStringL(const TDesC8& aString, TBool aPartLine)
	{
	CCommChatString* chatString=CCommChatString::NewL(aString,aPartLine);
	iList.AddLast(*chatString);
	if (aPartLine)
		iPartLineMatchers+=1;
	return chatString;
	}

void CCommChatter::RemoveString(CCommChatString* aString)
	{
	_LIT(KCCommChatterPanic, "GSMTSY-CHAT");
	aString->iLink.Deque();
	if (aString->iIsPartialLine)
		{
		iPartLineMatchers-=1;
		__ASSERT_ALWAYS(iPartLineMatchers>=0, User::Panic(KCCommChatterPanic,1));
		}
	}

void CCommChatter::DeleteAllAndStop()
	{
	StopTimer();
	CCommChatString* cs;
	while (!iList.IsEmpty())
		{
		cs = iList.First();
		RemoveString(cs);
		delete cs;
		}
	iPartLineMatchers=0;
	}

void CCommChatter::StartTimer(const TTimeIntervalMicroSeconds32 aTimeout)
	{
	if (IsActive())
		Cancel();
	After(aTimeout);
	}

void CCommChatter::StopTimer()
	{
	Cancel();
	}
