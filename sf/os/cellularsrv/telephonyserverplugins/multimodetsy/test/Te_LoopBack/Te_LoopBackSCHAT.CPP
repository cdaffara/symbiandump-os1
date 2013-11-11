// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements the management of strings for matching between the chat buffer (also
// called the history buffer) and the chat strings.  The chat buffer is a single buffer that
// contains information that has been written to the test harness from the tsy.  The chat
// strings are the expected strings specified in the scripting language (in the RxEvents).
// The expected or chat strings must match the strings received from the TSY.  
// This file also includes the logic to support the use of buffer markers to be associated with
// the history buffer and the Chat strings.  Essentially, these buffer markers delimit the 
// range of addresses in the history buffer where data associated with a particular command 
// could be found.  The buffer marker structure only contains one address and uses the current
// last character address as it's other delimeter.
// 
//

/**
 @file
*/

#include "Te_LoopBackATSTD.H"
#include "Te_LoopBackSCHAT.H"
#include "Te_LoopBackSLOGGER.H"

CCommChatString* CCommChatString::NewL(const TDesC8& aDes, TBool aIsFolded)
/**
 * 2 Phase Constructor
 *
 * This method creates a CCommChatString instance.  After newing the necessary, it
 * pushes the chat string pointer onto the stack.  Then it calls it's second stage
 * constructor (in this case the CreateL method), passing the parameters passed to
 * this method.
 *
 * @param	aDes references the descriptor that contains the string.
 * @param	aIsFolded a boolean that indicates the "foldedness" of this string.
 *
 * @leave	This function will leave if the system is out of memory or unable 
 *			to allocate the amount of memory requested.
 * 
 * @return	A pointer to the newly created CCommChatString
 */
	{
	CCommChatString* cs = new (ELeave) CCommChatString;
	CleanupStack::PushL(cs);
	cs->CreateL(aDes, aIsFolded);
	CleanupStack::Pop();
	return cs;
	}

CCommChatString::CCommChatString()
/**
 * This method is the standard constructor for the class CCommChatString.
 */
	{
	__DECLARE_NAME(_S("CCommChatString"));
	}

CCommChatString::~CCommChatString()
/**
 * This method is the standard destructor.  It frees the memory allocated in the CreateL
 * method during construction.
 *
 * @return	None.
 */
	{
	User::Free(iString);
	}

void CCommChatString::CreateL(const TDesC8& aDes, TBool aIsFolded)
/**
 * Second Phase of Constructor
 *
 * This method is used as the second phase of the constructor for the CComChatString
 * class.  It allocates heap memory as specified by the passed in descriptor's length.
 * Then, the string located in the descriptor is copied into the newly allocated memory.
 * The member data iLastChar is initialized to point to the last byte copied.  The member
 * data iIsFolded is initialized to the passed parameter.
 *
 * @param aDes The descriptor containing the string data specified by the caller.
 * @param aIsFolded A boolean indicating whether or not the string is folded.
 * 
 * @leave	Leaves via User::AllocL if memory can not be allocated.
 */
	{
	iIsFolded = aIsFolded;
	iString = (TText8*) User::AllocL(aDes.Length());
	iLastChar = (iString+aDes.Length())-1;
	Mem::Copy(iString, (TUint8*)aDes.Ptr(), aDes.Length());
	}

//
// CCommChatter
//
CCommChatter::CCommChatter(MCommChatNotify* aNotify, TInt aPriority)
	: CTimer(aPriority), iNotify(aNotify)
/**
 * The constructor for the CCommChatter class.  It adds the active object to the active
 * scheduler of the current thread and initializes member data elements.
 * 
 * @param aNotify A pointer to the mixin class object associated with this object.  This 
 *                information is stored in the iNotify member.
 * @param aPriority The priority which is assigned to the timer this class derives from CTimer.
 */
	{
	__DECLARE_NAME(_S("CCommChatter"));
	CActiveScheduler::Add(this);
	iList.SetOffset(_FOFF(CCommChatString,iLink));
	iMarkWrapCnt=0;
	}

CCommChatter::~CCommChatter()
/**
 * The standard destructor associated with the CCommChatter class.  This method uses the 
 * DeleteAllAndStop method to free resources saved in the CCommChatter object.  Then it frees
 * the buffer created with CCommChatter.
 *
 * @return  None
 */
	{
	DeleteAllAndStop();
	User::Free(iBuffer);
	}

void CCommChatter::CreateL(TInt aBufSize)
/**
 * This method is really the second stage constructor for the CCommChatter class, though it is
 * not called by the typical CCommChatter::NewL() method.  The CCommChatter class is typically
 * created by another method (in the regression test case harness it is created in the 
 * CATIO::ConstructL() method), then this method is called.  This method allocates a buffer and
 * constructs the timer derived from CTimer in this class. The member data iBuffer, iBufferEnd,
 * and iMarkWrapCnt are initialized.
 *
 * @param  aBufSize The size of the buffer to be created with this object.
 * 
 * @leave  This method can Leave via User::AllocL if memory is not available.
 */
	{
	CTimer::ConstructL();
	iBuffer = (TText8*)User::AllocL(aBufSize);
	iBufferEnd = (iBuffer+aBufSize)-1;
	ClearHistory();
	iMarkWrapCnt=0;
	}

void CCommChatter::ClearHistory()
//
// Empty history
//
/**
 * This method resets the history of the CCommChatter buffer in the object.  It simply clears
 * the iCount and initializes the iLastChar member to iBuffer (the beginning of the buffer).
 *
 * @return	None.
 */
	{
	iLastChar = iBuffer;
	iCount = 0;
	}

void CCommChatter::AddCharL(TText8 aChar)
//
// Add a character to the history buffer
// Scan all strings to find any matches that may
// be completed.
//
/**
 * This method adds a single character to the history buffer and then scans all stored
 * strings to find any matches that may have been completed because this character has
 * been added.  The function deals with folded characters during the search.  Note that 
 * the routine first checks to see if the last character in the stored string matches
 * the input character.  This reduces the number of invocations of the string match 
 * functions.
 *
 * @param		aChar The character to add to the buffer.
 * @leave	This method can leave via the ChatStringMatchL call.
 */
	{
	// Add char to buffer
	if (++iLastChar>iBufferEnd)
		{
		iLastChar = iBuffer;
		iMarkWrapCnt++;
		}
	*iLastChar = aChar;
	++iCount;

	TText8 fchar = (TText8)User::Fold(aChar);

	// Scan for matching last character
	CCommChatString* cs;
	TDblQueIter<CCommChatString> iter(iList);

	while (cs = iter++, cs!=NULL)
		{
		if (cs->IsFolded()
			? (User::Fold(cs->LastChar())==fchar && MatchF(cs))
			: (cs->LastChar()==aChar && Match(cs)))
			{
			iNotify->ChatStringMatchL(cs);
			cs = iter;	// In case user removed cs;
			}
		}
	}

TBool CCommChatter::Match(const CCommChatString* aString) const
//
// Match a chat sgring against the history buffer
// (Case sensitive)
//
/**
 * This method matchs a chat string against the history buffer in a case sensitive (unfolded)
 * manner. First the length of the two strings is compared.  Then the actual content of the 
 * strings is compared.
 *
 * @param	aString	A constant pointer to the chat string.
 *
 * @return	ETrue if strings match, EFalse if they do not.
 */
	{
	const TText8* s = aString->Ptr();
	const TText8* sp = aString->EndPtr();
	const TText8* bp = iLastChar;

	if (iCount<aString->Length())
		return EFalse;

	while (*bp==*sp && sp>=s)
		{
		--sp;
		if (--bp<iBuffer)
			bp = iBufferEnd;
		}

	return sp<s;
	}

TBool CCommChatter::MatchF(const CCommChatString* aString) const
//
// Match a folded chat string against the history buffer.
// (Case insensitive)
//

/**
 * This method matchs a chat string against the history buffer in a case insensitive or folded
 * manner.  First the length of the two strings is compared.  Then the actual content of the 
 * strings is compared.
 *
 * @param	aString	A constant pointer to the chat string.
 *
 * @return	ETrue if strings match, EFalse if they do not.
 */
	{
	const TText8* s = aString->Ptr();
	const TText8* sp = aString->EndPtr();
	const TText8* bp = iLastChar;
	if (iCount<aString->Length())
		return EFalse;

	while (User::Fold(*bp)==User::Fold(*sp) && sp>=s)
		{
		--sp;
		if (--bp<iBuffer)
			bp = iBufferEnd;
		}
	return sp<s;
	}

void CCommChatter::GetChatBufferMarker(TInputBufferMark& aBufferMarker)
/**
 * This method sets up a buffer marker structure.  It saves the current last character
 * location and notes the wrap state of the buffer.
 *
 * @param	aBufferMarker A reference to a buffer marker structure.
 *          Is modified with the current location of iLastChar and a wrap flag.
 * @return	None
 */
	{
	aBufferMarker.iMarkChar=iLastChar;
	if(++aBufferMarker.iMarkChar>iBufferEnd)
		{
		aBufferMarker.iMarkWrap=iMarkWrapCnt+1;
		aBufferMarker.iMarkChar=iBuffer;
		}
	else
		aBufferMarker.iMarkWrap=iMarkWrapCnt;
	}

TPtrC8 CCommChatter::GetChatBufferLC(TInputBufferMark& aBufferMarker)
/**
 * This method deals with extracting data from the Chat or History Buffer and returning it
 * to the caller.  The data that is extracted from the buffer marker to the current last
 * character in the buffer.  This method creates a new HBufC8 object and returns a pointer
 * to that descriptor.  This method deals properly with a wrapped history buffer.
 *
 * @param		aBufferMarker is a reference to a buffer marker structure that has been
 *				previously initialized.  This buffer marker structure limits the range of the
 *				history buffer which must be copied/examined to find the relevant data.
 * @leave		Leaves if error processing buffer between the mark and the current
 *				last character.
 * @return		TPtrC8 pointer to a newly created HBufC8 which contains the iRxBuffer
 *				contents between the marker and the current last character.  Data that
 *				is of interest to this completion has been copied from the general 
 *				receive buffer to this buffer.
 */
	{
	TText8* markChar=aBufferMarker.iMarkChar;
	if(iMarkWrapCnt==aBufferMarker.iMarkWrap)
		{
		__ASSERT_ALWAYS(iLastChar>markChar,User::Leave(KErrGeneral));			// Internal Error
		HBufC8* buffer=HBufC8::NewLC(iLastChar-markChar+1);
		TPtr8 ptr(buffer->Des());
		ptr.Copy(markChar,iLastChar-markChar+1);
		return ptr;
		}
	else
		{
		if((iLastChar>markChar)||(iMarkWrapCnt>aBufferMarker.iMarkWrap+1))
			User::Leave(KErrGeneral);																						// Data Lost
		HBufC8* buffer=HBufC8::NewLC(KChatBufferSize-(markChar-iLastChar)+1);
		TPtr8 ptr(buffer->Des());
		ptr.Copy(TPtrC8(markChar,iBufferEnd-markChar+1));
		ptr.Append(TPtrC8(iBuffer,iLastChar-iBuffer+1));
		return ptr;
		}
	}

void CCommChatter::RunL()
/**
 * This is the RunL associated with the CTimer inherited by the CCommChatter class.  This 
 * RunL simply processess a timeout.
 *
 * @return	None
 */
	{
	iNotify->ChatTimeout();
	}

CCommChatString* CCommChatter::AddString(const TDesC8& aString)
/**
 * This method adds a string to the stored chat strings.  It creates a new chat string object
 * and adds it to the list of outstanding chat strings.
 *
 * @param	aString This is a constant TDesC8 reference that is used to create the new string.
 * @return	A pointer to the newly created CCommChatString object.
 */
	{
	CCommChatString* chatString=NULL;
	TRAP_IGNORE(chatString=CCommChatString::NewL(aString,ETrue));		// trap but ignore leaves
	iList.AddLast(*chatString);
	return chatString;
	}

void CCommChatter::RemoveString(CCommChatString* aString)
/**
 * This method removes a string from the stored chat strings.  It dequeues the string from the
 * list of stored chat strings but does not delete the object.
 *
 * @param	aString A pointer to a CCommChatString
 * @return	None
 */	{
	aString->iLink.Deque();
	}

void CCommChatter::DeleteAllAndStop()
/**
 * This method stops the outstanding CTimer and removes all of the outstanding chat strings
 * from the chat string list and deletes them.
 *
 * @return	None.
 */
	{
	StopTimer();
	CCommChatString* cs;
	while (!iList.IsEmpty())
		{
		cs = iList.First();
		RemoveString(cs);
		delete cs;
		}
	}

void CCommChatter::StartTimer(const TTimeIntervalMicroSeconds32 aTimeout)
/**
 * This method starts the timer associated with the CCommChatter class.  This timer
 * triggers after a specific interval of time.  This is a one shot timer.
 *
 * @param	aTimeout	The interval in micro-seconds before the timer should trigger.
 * @return	None
 */
	{
	if (IsActive())
		Cancel();
	After(aTimeout);
	}

void CCommChatter::StopTimer()
/**
 * This method simply stops the outstanding timer for the CCommChatter class.  This timer
 * is implemented via an active object by the CTimer class.
 *
 * @return	None
 */
	{
	Cancel();
	}
