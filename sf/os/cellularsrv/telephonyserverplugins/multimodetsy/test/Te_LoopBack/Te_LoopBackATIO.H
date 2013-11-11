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
//

/**
 @file
 @internalComponent 
*/

#ifndef __SIO_H
#define __SIO_H

#include "Te_LoopBackSCHAT.H"
#include "Te_LoopBackSCOMM.H"
#include "Te_LoopBackATSTD.H" // for enum TPortAccess

const TInt KKeyReaderPriority = 0;
const TInt KChatterPriority = 0;
const TInt KChatTimeout = 3000000;
const TInt KCommRxBufSize = 800;
const TInt KCommTimeout = 10000000;
const TInt KCommReadPriority = 10;
const TInt KCommWritePriority = 20;
const TInt KCommTimerPriority = 5;

const TInt KReadTimeOutSec=60;
const TInt KWriteTimeOutSec=10;
const TInt KInitialisationTimeOut=20;
const TInt KDefaultTimeOutMillisec=5000;

class CATIO;
class CATBase;

/**
 * This class inherits from CBase and is used by the test harness to manage the expect
 * strings.  Every expect (or chat) string is associated with a completion entry.  There
 * is a list of expected strings managed by the test harness code.  When a string comes
 * in from the TSY, this class is used (as the data container) for matching the expected
 * strings and the input from the TSY.
 */
class CCompletionEntry : public CBase
	{
public:
	static CCompletionEntry* NewL(CCommChatString* aCs,	CATBase* aAtCommand);
	CCompletionEntry(CCommChatString* aCs, CATBase* aAtCommand);
	~CCompletionEntry();
private:
	CCommChatString* iCs;		//< Pointer to the chat string (expected string) for this
								//< completion entry.
	CATBase* iAtCommand;		//< Pointer to the CATBase object associated with this
								//< completion entry.  Used for various tasks such as 
								//< event signalling, removing of expected strings, etc.
	TDblQueLink iLink;			//< Element of Doubly linked list
	friend class CATIO;
	};

class CATErrorHandler;
class CATBase;
/**
 * This class inherits from CBase, MComm, and MCommChatNotify.
 * It represents the basic IO mechanism for the regression test harness.
 * This class handles the receive buffer from the GSM TSY and the expect
 * strings (reading and writing).
 */
class CATIO : public CBase, public MComm, public MCommChatNotify
	{
public:
	static CATIO* NewL(const TDesC& aCsy, const TDesC& aPort);
	CATIO();
	void ConstructL(const TDesC& aCsy, const TDesC& aPort);
	~CATIO();
	TInt ConfigurePort(TCommConfig aConfiguration);
	TInt Start(CATBase* aCompletionClass);
	CCommChatString* AddExpectString(CATBase* aATBase, const TDesC8& aString);
	void RemoveExpectStrings(CATBase* aATBase);
	void RemoveExpectString(CCommChatString* aExpectString);
	void SignalCommandsWithError(TInt aStatus);
	TInt Read();
	TInt Write(CATBase* aWriteCommand, const TDesC8& aString);
	TBool RWPending();
	TBool ReadPending();
	void GetExcessData(TDes8& aBuffer);
	void MarkRxBuffer(TInputBufferMark& aBufferMarker);
	TPtrC8 GetRxBufferLC(TInputBufferMark& aBufferMarker);
	void Disconnect();
	void Cancel();
	void WriteAndTimerCancel(CATBase* aATBase);
	void WriteAndTimerCancelAll();
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
	TUint Signals();
	void SignalMark(TUint aSignal);
	void SignalSpace(TUint aSignal);

	// from MComm
	virtual void CommReadComplete(TInt aStatus);
	virtual void CommWriteComplete(TInt aStatus);
	// from MCommChatNotify
	virtual void ChatStringMatchL(CCommChatString* aCs);
	virtual void ChatTimeout();
private:
	CATBase* iControllingClass;				//< Pointer to the controlling CATBase entity for read 
											//< completions.
	CATBase* iTimeOutCommand;				//< Pointer to the controlling CATBase entity for timeout
											//< completions.
	CATBase* iWriteCommand;					//< Pointer to the controlling CATBase entity for write
											//< completions.
	CATErrorHandler* iErrorHandler;			//< Ptr to error handler, not apparently used by test harness.
	CCommChatter* iChat;					//< Pointer to the CComChatter object which is associated
											//< with this IO class and iRxBuf.
	TInt iRxBufOffset;						//< Offset in iRxBuf where received data is to be placed.
	TBuf8<KCommRxBufSize> iRxBuf;			//< The buffer which contains commands/data received from
											//< the GSM TSY.
	TBool iReadPending;						//< True if a read is pending; false otherwise
	TBool iWritePending;					//< True if a write is pending; false otherwise
	TBool iWaitTimerPending;				//< Boolean used for noting an outstanding timer on a chat string.
	TBool iInitPending;						//< Not currently used in Regression Test Harness
	TBool iStringFound;						//< True if a matching string is found, set to false when
											//< a read is pended.
	CArrayFixFlat<CCommChatString*>* iChatStringFound;	//< Array of ptrs to chat strings that have been
														//< matched.
	CCommChatString* iCurrentFoundChatString;	//< Pointer to the chat string that has currently
												//< been matched.
	TDblQue<CCompletionEntry> iExpectList;	//< Queue of expected strings (in CCompletionEntry)
	TInt iSecondChanceForCommsError;		//< Counter of number of comms errors.  One comm error
											//< can be retried during this test.
	};

#endif
