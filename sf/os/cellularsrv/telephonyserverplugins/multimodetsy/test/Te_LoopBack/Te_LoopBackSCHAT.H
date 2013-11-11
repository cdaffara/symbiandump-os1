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
// This file defines the classes necessary for buffer and expect string management in the 
// regression test harness.
// 
//

/**
 @file
 @internalComponent
*/
#if !defined(__CHAT_H__)
#define __CHAT_H__

#include <e32std.h>
#include <e32base.h>

const TInt KChatBufferSize = 400;

struct TInputBufferMark {
	TText8* iMarkChar;
	TUint iMarkWrap;
	};

class CCommChatString;

/**
 * A mixin class used to notify the derived class that either a string match has been
 * found or a time out has occurred.
 */
class MCommChatNotify
	{
public:
	// Used to notify the derived class that a string has been matched.
	virtual void ChatStringMatchL(CCommChatString* aCs)=0;
	// Used to notify the derived class that the timer has expired.
	virtual void ChatTimeout()=0;
	};

/**
 * CCommChatter class is used to buffer and manage the information coming from the GSMTSY and
 * to correlate it with the expected strings (see class CCommChatString).  This is derived from
 * the CTimer class.  Every time a character is added to the iBuffer, the outstanding chat strings
 * are checked to see if they have completed.
 */
class CCommChatter : public CTimer
	{
public:
	CCommChatter(MCommChatNotify* aNotify, TInt aPriority);
	virtual ~CCommChatter();
	void CreateL(TInt aBufferSize);
	TBool Match(const CCommChatString* aString) const;
	TBool MatchF(const CCommChatString* aString) const;
	void AddCharL(TText8 aChar);
	CCommChatString* AddString(const TDesC8& aString);
	void RemoveString(CCommChatString* aString);
	void GetChatBufferMarker(TInputBufferMark& aBufferMarker);
	TPtrC8 GetChatBufferLC(TInputBufferMark& aBufferMarker);
	void StartTimer(const TTimeIntervalMicroSeconds32 aTimeout);
	void StopTimer();
	void DeleteAllAndStop();
protected:
	virtual void RunL();
	void ClearHistory();
protected:
	MCommChatNotify* iNotify;		//< Pointer to Mixin class for notifications.
	TDblQue<CCommChatString> iList;	//< Doubly linked list of all outstanding Chat strings
	TText8* iBuffer;				//< Pointer to allocated buffer to contain cmds/data from GSMTSY
	TText8* iBufferEnd;				//< Pointer to last character in iBuffer
	TText8* iLastChar;				//< Last character added
	TUint iMarkWrapCnt;				//< Count of number of times iBuffer has wrapped (used in buffer marking)
	TInt iCount;					//< No of chars added to buffer
	};

/**
 * This class is the object that manages expect strings for the Regression Test Harness.
 * It is derived from CBase.
 * A Chat string in the regression test harness is created by ERx type commands in the
 * command script.  These chat strings are the expect strings and are used to match up
 * the progress of the modem to the expected progress of the script.
 */
class CCommChatString : public CBase
	{
public:
	static CCommChatString* NewL(const TDesC8& aDes, TBool aIsFolded);
	CCommChatString();
	virtual ~CCommChatString();
	void CreateL(const TDesC8& aDes, TBool aIsFolded);
	inline TText8 LastChar() const { return *iLastChar; }
	inline TInt Length() const { return (iLastChar-iString)+1; }
	inline const TText8* Ptr() const { return iString; }
	inline const TText8* EndPtr() const { return iLastChar; }
	inline TBool IsFolded() const { return iIsFolded; }
	inline TPtrC8 Des() { return TPtrC8(Ptr(), Length()); }
protected:
	TBool iIsFolded;		//< Is this string folded or not
	TText8* iString;		//< Pointer to memory which contains expected string (memory from heap).
	TText8* iLastChar;		//< Pointer to last character in iString
private:
	friend class CCommChatter;
	TDblQueLink iLink;		//< Links to previous and next chat strings in linked list.
	};

#endif
