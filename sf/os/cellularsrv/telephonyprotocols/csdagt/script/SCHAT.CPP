// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// For manipulting of received data
// 
//

/**
 @file schat.cpp 
*/

#include "SCHAT.H"
#include "ND_STD.H"

//
// CCommChatter definitions
//

CCommChatter* CCommChatter::NewL(MCommChatNotify* aNotify,TInt aPriority,TInt aBufferSize)
/**
2 phased constructor for CCommChatter, first phase.

@param aNotify a pointer to chat notifier.
@param aPriority is priority for this timer.
@param aBufferSize is buffer size for this object.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CCommChatter object.
*/
	{
	CCommChatter* cc=new(ELeave) CCommChatter(aNotify,aPriority);
	CleanupStack::PushL(cc);
	cc->ConstructL(aBufferSize);
	CleanupStack::Pop();
	return cc;
	}

CCommChatter::CCommChatter(MCommChatNotify* aNotify, TInt aPriority)
	: CTimer(aPriority), iNotify(aNotify)
/**
Private constructor for CCommChatter, used in the first phase of construction.

@param aNotify a pointer to chat notifier.
@param aPriority is priority for this timer.
*/
	{
	CActiveScheduler::Add(this);
	iList.SetOffset(_FOFF(CCommChatString,iLink));
	}

void CCommChatter::ConstructL(TInt aBufSize)
/**
Instantiates member variables.

@param aBufSize is buffer size for this object.
@exception Leaves if CTimer::ConstructL() or User::AllocL() leaves.
*/
	{
	CTimer::ConstructL();
	iBuffer=(TText8*)User::AllocL(aBufSize);
	iBufferEnd=(iBuffer+aBufSize)-1;
	ClearHistory();
	}

CCommChatter::~CCommChatter()
/**
Destructor.
Frees iBuffer's allocated memory.
*/
	{
	User::Free(iBuffer);
	}

void CCommChatter::ClearHistory()
/**
Empties history.
*/
	{
	iLastChar=iBuffer;
	iCount=0;
	}

void CCommChatter::AddChar(TText8 aChar)
/**
Adds a character to the history buffer.
Scan all strings to find any matches that may be completed.
*/
	{
	if (++iLastChar>iBufferEnd)
		iLastChar=iBuffer;
	*iLastChar=aChar;
	++iCount;

	TText8 fchar=(TText8)User::Fold(aChar);

	// Scan for matching last character	
	CCommChatString* cs;
	TDblQueIter<CCommChatString> iter(iList);
	TInt index=0;

	while (cs=iter++, cs!=NULL)
		{
		if (cs->IsFolded()
			? (cs->LastChar()==fchar && MatchF(cs))
			: (cs->LastChar()==aChar && Match(cs)) )
			{
			iNotify->ChatStringMatch(index);
			}
		index++;
		}
	}

TBool CCommChatter::Match(const CCommChatString* aString) const
/**
Matches a chat string against the history buffer (Case sensitive).
*/
	{
	__ASSERT_ALWAYS(aString!=NULL, NetDialPanic(ENullCommChatString));

	const TText8* s=aString->Ptr();
	const TText8* sp=aString->EndPtr();
	const TText8* bp=iLastChar;

	if (iCount<aString->Length())
		return EFalse;

	while (*bp==*sp && sp>=s)
		{
		--sp;
		if (--bp<iBuffer)
			bp=iBufferEnd;
		}

	return sp<s;
	}

TBool CCommChatter::MatchF(const CCommChatString* aString) const
/**
Matches a folded chat string against the history buffer (Case insensitive).
*/
	{
	__ASSERT_ALWAYS(aString!=NULL, NetDialPanic(ENullCommChatString));

	const TText8* s=aString->Ptr();
	const TText8* sp=aString->EndPtr();
	const TText8* bp=iLastChar;

	if (iCount<aString->Length())
		return EFalse;

	while (User::Fold(*bp)==*sp && sp>=s)
		{
		--sp;
		if (--bp<iBuffer)
			bp=iBufferEnd;
		}

	return sp<s;
	}

void CCommChatter::RunL()
/**
Timer completed.
*/
	{
	iNotify->ChatTimeout();
	}

void CCommChatter::AddString(CCommChatString* aString)
/**
Adds a string to the list.
*/
	{
	__ASSERT_ALWAYS(aString!=NULL, NetDialPanic(ENullCommChatString));
	iList.AddLast(*aString);
	}

void CCommChatter::RemoveString(CCommChatString* aString)
/**
Removes a string from the list.
*/
	{
	__ASSERT_ALWAYS(aString!=NULL, NetDialPanic(ENullCommChatString));
	aString->iLink.Deque();
	}

void CCommChatter::DeleteAllAndStop()
/**
Removes all strings from the list.
*/
	{
	StopTimer();
	CCommChatString* cs;
	while (!iList.IsEmpty())
		{
		cs=iList.First();
		RemoveString(cs);
		delete cs;
		}
	}

void CCommChatter::StartTimer(const TTimeIntervalMicroSeconds32& aTimeout)
/**
Starts timer.
*/
	{
	
	if (IsActive())
		Cancel();
	After(aTimeout);
	}

void CCommChatter::StopTimer()
/**
Stops timer.
*/
	{
	Cancel();
	}

//
// CCommChatString definitions
//

CCommChatString* CCommChatString::NewL(const TDesC8& aDes,TBool aIsFolded)
/**
2 phased constructor for CCommChatString, first phase.

@param aDes a pointer to chat notifier.
@param aIsFolded is buffer size for this object.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CCommChatString object.
*/
	{
	CCommChatString* cs=new (ELeave) CCommChatString;
	CleanupStack::PushL(cs);
	cs->ConstructL(aDes,aIsFolded);
	CleanupStack::Pop();
	return cs;
	}

CCommChatString::CCommChatString()
/**
Private constructor for CCommChatString, used in the first phase of construction.
*/
	{}

void CCommChatString::ConstructL(const TDesC8& aDes,TBool aIsFolded)
/**
Instantiates member variables.
*/
	{
	__ASSERT_ALWAYS(aDes.Length()>0, NetDialPanic(ENullCommChatString));
	iIsFolded=aIsFolded;
	iString=(TText8*)User::AllocL(aDes.Length());
	iLastChar=(iString+aDes.Length())-1;
	Mem::Copy(iString, (TText8*)aDes.Ptr(), aDes.Length());
	}

CCommChatString::~CCommChatString()
/**
Destructor.
Deletes allocations/objects.
*/
	{
	User::Free(iString);
	}
