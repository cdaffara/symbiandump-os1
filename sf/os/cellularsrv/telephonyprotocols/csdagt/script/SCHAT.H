/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Header for classes for manipulting of received data.
* This should all be 8-bit so we don't need to deal with any conversions at this level.
* 
*
*/



/**
 @file Schat.h
*/

#if !defined(__SCHAT_H__)
#define __SCHAT_H__

#include <e32base.h>

// Forward declarations 
class CCommChatString;

class MCommChatNotify
/**
Interface between CScriptIO and CCommChatter object.

@internalComponent
*/
	{
public:
	virtual void ChatStringMatch(TInt aIndex)=0;
	virtual void ChatTimeout()=0;
	};

class CCommChatter : public CTimer
/**
Deals with data received from the comm port.  One of these for each CScriptIO object.

@internalComponent
*/
	{
public:
	static CCommChatter* NewL(MCommChatNotify* aNotify,TInt aPriority,TInt aBufferSize);
	CCommChatter(MCommChatNotify* aNotify,TInt aPriority);
	virtual ~CCommChatter();	
	void ConstructL(TInt aBufSize);
	void ClearHistory();
	void AddChar(TText8 aChar);
	TBool Match(const CCommChatString* aString) const;
	TBool MatchF(const CCommChatString* aString) const;
	void AddString(CCommChatString* aString);
	void RemoveString(CCommChatString* aString);
	void DeleteAllAndStop();
	void StartTimer(const TTimeIntervalMicroSeconds32& aTimeout);
	void StopTimer();
protected:
	virtual void RunL();
protected:
	MCommChatNotify* iNotify;
	TDblQue<CCommChatString> iList;
	TText8* iBuffer;
	TText8* iBufferEnd;
	TText8* iLastChar;	///< Last character added
	TInt iCount;		///< No of chars added to buffer
	};


class CCommChatString : public CBase
/**
@internalComponent
*/
	{
public:
	static CCommChatString* NewL(const TDesC8& aDes,TBool aIsFolded);
	CCommChatString();
	void ConstructL(const TDesC8& aDes,TBool aIsFolded);
	virtual ~CCommChatString();
	inline TText8 LastChar() const { return *iLastChar; }
	inline TInt Length() const { return (iLastChar-iString)+1; }
	inline const TText8* Ptr() const { return iString; }
	inline const TText8* EndPtr() const { return iLastChar; }
	inline TBool IsFolded() const { return iIsFolded; }
	inline TPtrC8 Des() { return TPtrC8(Ptr(), Length()); }
private:
	friend class CCommChatter;
	TDblQueLink iLink;
protected:
	TBool iIsFolded;
	TText8* iString;
	TText8* iLastChar;
	};

#endif
