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
// Serial IO Functions
// 
//

/**
 @file
 @internalAll 
*/


#ifndef __ATIO_H__
#define __ATIO_H__

#include "SCOMM.H"
#include "ATBASE.H"
/**
@internalComponent
*/
const TInt KKeyReaderPriority = 0;
const TInt KChatterPriority = 0;
const TInt KChatTimeout = 3000000;
const TInt KCommRxBufSize = 200;
const TInt KCommTimeout = 10000000;
const TInt KCommReadPriority = 10;
const TInt KCommWritePriority = 20;
const TInt KCommTimerPriority = 5;
/**
@internalComponent
*/
const TInt KReadTimeOutSec=60;
const TInt KWriteTimeOutSec=10;
const TInt KInitialisationTimeOut=20;
const TInt KDefaultTimeOutMillisec=5000;

class CATIO;
class CATBase;
class CCompletionEntry : public CBase
/**
@internalComponent
*/
	{
public:
	static CCompletionEntry* NewL(CCommChatString* aCs,	CATBase* aAtCommand);
	CCompletionEntry(CCommChatString* aCs, CATBase* aAtCommand);
	~CCompletionEntry();
private:
	CCommChatString* iCs;
	CATBase* iAtCommand;
	TDblQueLink iLink;
	friend class CATIO;
	};

class CATErrorHandler;
class CATBase;
class CATIO : public CBase, public MComm, public MCommChatNotify
/**
@internalComponent
*/
	{
public:
	static CATIO* NewL(TFileName& csy, TName& port,TPortAccess& aPortAccess);
	CATIO(TPortAccess& aPortAccess);
	void ConstructL(TFileName& csy, TName& port);
	~CATIO();
	TInt ConfigurePort(TCommConfig aConfiguration);
	void Start(CATBase* aCompletionClass);
	void StartWrite(CATBase* aCompletionClass);
	CCommChatString* AddExpectString(CATBase* aATBase, const TDesC8& aString, TBool aPartLine=EFalse);
	CCommChatString* DoAddExpectStringL(CATBase* aATBase, const TDesC8& aString, TBool aPartLine=EFalse);
	void RemoveExpectStrings(CATBase* aATBase);
	void RemoveExpectString(CCommChatString* aExpectString);
	void SignalCommandsWithError(TInt aStatus);
	void Read();
	void Write(CATBase* aWriteCommand, const TDesC8& aString);
	TBool ReadPending();
	TInt BufferFindF(const TDesC8& aDes) const;
	TPtrC8 Buffer() const;
	TPtrC8 CurrentLine() const;
	void ClearBuffer();
	void ClearCurrentLine();
	void Disconnect();
	void Cancel();
	void ReadCancel();
	void WriteAndTimerCancel(CATBase* aATBase);
	CCommChatString* FoundChatString();

	void SetTimeOut(CATBase* aCompletionClass, TUint aTimePeriodSec=KDefaultTimeOutMillisec);
	void DropDtr();
	void RaiseDTR();
	void DropRTS();
	void RaiseRTS();
	void ResetReadAndWriteBuffers();
	TInt GetSizeOfRxBuffer();
	void ProcessReadCharsL();
	void SetErrorHandler(CATErrorHandler* aErrorHandler) {iErrorHandler = aErrorHandler;}
	void SetPreserveBufferFlag(TBool aFlg);
	TUint Signals();
	// from MComm
	virtual void CommReadComplete(TInt aStatus);
	virtual void CommWriteComplete(TInt aStatus);
	// from MCommChatNotify
	virtual void ChatStringMatchL(CCommChatString* aCs);
	virtual void ChatTimeout();
	TFileName iCsy;
private:
	TInt GetPortShutdownTimeout();
	TInt SetPortShutdownTimeout(TInt aTimeout);
	CATBase* iTimeOutCommand;
	CATBase* iWriteCommand;
	CATErrorHandler* iErrorHandler;
	CCommChatter* iChat;
	TInt iRxBufOffset;
	TBuf8<KCommRxBufSize> iRxBuf;
	TBool iReadPending;
	TBool iWritePending;
	TBool iWaitTimerPending;
	TBool iInitPending;
	TBool iStringFound;
	CArrayFixFlat<CCommChatString*>* iChatStringFound;
	CCommChatString* iCurrentFoundChatString;
	TDblQue<CCompletionEntry> iExpectList;
	TPortAccess& iPortAccess;
	TInt iSecondChanceForCommsError;
	TBool iPreserveBuffer;
	};

#endif
