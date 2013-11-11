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
// Send/Expect Algorithms
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __SCHAT_H__
#define __SCHAT_H__

#include <e32base.h>
/**
@internalComponent
*/
const TInt KChatBufferSize = 400; //increased from 200 to stop WrapCnt overrunning due to concatenated messages!!

class CCommChatString;
class MCommChatNotify
/**
@internalComponent
*/
	{
public:
	virtual void ChatStringMatchL(CCommChatString* aCs)=0;
	virtual void ChatTimeout()=0;
	};

class CCommChatter : public CTimer
/**
@internalComponent
*/
	{
public:
	CCommChatter(MCommChatNotify* aNotify, TInt aPriority);
	virtual ~CCommChatter();
	void CreateL(TInt aBufferSize);
	void AddCharL(TText8 aChar);
	CCommChatString* AddStringL(const TDesC8& aString, TBool aPartLine);
	void RemoveString(CCommChatString* aString);
	TPtrC8 Buffer() const;
	TPtrC8 CurrentLine() const;
	void ClearBuffer();
	void ClearCurrentLine();
	void StartTimer(const TTimeIntervalMicroSeconds32 aTimeout);
	void StopTimer();
	void DeleteAllAndStop();
protected:
	virtual void RunL();
protected:
	MCommChatNotify* iNotify;
	TDblQue<CCommChatString> iList;
	TText8* iBuffer;
	TText8* iBufferEnd;
	TText8* iLastChar;			// Last character added
	TText8* iLineStart;			// Start of \r\n delimited line
	TBool iInDelimiter;
	TInt iPartLineMatchers;		// count of CCommChatStrings which need partial line matches
	};

class CCommChatString : public CBase
/**
@internalComponent
*/
	{
public:
	static CCommChatString* NewL(const TDesC8& aDes, TBool aIsPartialLine);
	CCommChatString();
	virtual ~CCommChatString();
	void CreateL(const TDesC8& aDes, TBool aIsPartialLine);
	inline const TPtrC8& Des() const {return iMatch;};
protected:
	TBool iIsPartialLine;
	TPtrC8 iMatch;
private:
	friend class CCommChatter;
	TDblQueLink iLink;
	};

#endif
