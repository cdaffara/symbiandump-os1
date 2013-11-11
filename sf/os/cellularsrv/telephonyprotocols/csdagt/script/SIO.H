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
* Script Serial Comms I/O Functions Header.
* This should all be 8-bit so we don't need to deal with any conversions at this level.
* 
*
*/



/**
 @file Sio.h
 @internalComponent
*/

#ifndef __SIO_H__
#define __SIO_H__

#include <nifutl.h>
#include <etel.h>
#include "ND_STD.H"
#include "SCHAT.H"
#include "SCOMMAND.H"

// Forward declarations

class CScriptExecutor;
class CPreSendPause;

class CScriptIO : public CBase, public MCommV2, public MCommChatNotify

/**
One of these per CScriptExecutor object (also uses CScriptExecutor object).
Has CPreSendPause and CCommChatter objects.

@internalComponent
*/
	{
public:
	static CScriptIO* NewL(CScriptExecutor* aScriptExecutor, const TDesC& aCommsChannel);
	~CScriptIO();
//	
	void ConfigurePort(TRequestStatus& aStatus, const TCommConfig& aConfiguration);
	void CancelConfigurePort();
	void CreateChannel(TRequestStatus& aStatus);
	void CancelCreateChannel();
	void ShutdownChannel(TRequestStatus& aStatus);
	void Start();
	void Read(CLabelSearchArray* aSearchArray, const TReal& aTimeOut);
	void ReadEcho();
	void Write(const TDesC8& aString);
	void PreSendPauseCompleted();
	TBool RWPending();
	inline TBool WritePending() const;
	TInt GetExcessData(TDes8& aBuffer);
	void Disconnect();
	void ReConfigureAndCancelPort(TRequestStatus& aStatus);
	void DropSignals(TRequestStatus& aStatus);
	void Cancel();
	void ReadEchoCancel();
	void DropDTR(TRequestStatus* aStatusPtr);
	void RaiseDTR(TRequestStatus* aStatusPtr);
	void SetControlLines(TRequestStatus* aStatusPtr, TUint aSetMask, TUint aClearMask);
	
	// MComm
	virtual void CommReadComplete(TInt aStatus);
	virtual void CommWriteComplete(TInt aStatus);
	virtual const TDesC& BcaStack();
	virtual void Stop(TInt aError);
	virtual TInt IapId();
	virtual void InitializeComplete();
	virtual void ShutdownComplete(TInt aError);
	//
	// MCommChatNotify
	virtual void ChatStringMatch(TInt aIndex);
	virtual void ChatTimeout();
	//
private:
	CScriptIO(CScriptExecutor* aScriptExecutor);
	void ConstructL(const TDesC& aCommsChannel);
private:
	CScriptExecutor* iScriptExecutor;
	CCommChatter* iChat;
	CPreSendPause* iPreSendPause;
	TInt iRxBufOffset;
	TBuf8<KRxBufferSize> iRxBuffer;
	TBuf8<KTxBufferSize> iTxBuffer;
	CLabelSearchArray* iSearchArray;
	TPtrC8 iExcessData;
	TBool iReadPending;
	TBool iWritePending;
	TInt iStringFound;
	TRequestStatus* iCreateAndShutdownStatus;
	TInt iCreateError;
	RBuf iCommsChannel;
	TPckgBuf<TCommConfig> iConfig;
	TUint iCommClosed : 1;
	};

#include "SIO.INL"


class CPreSendPause : public CTimer
/**
One of these per CScriptIO object.  Implements pause before sending.

@internalComponent
*/
	{
public:
	static CPreSendPause* NewL(CScriptIO* aNotifier);
	CPreSendPause(CScriptIO* aNotifier);
	void Start();
	void RunL();
private:
	CScriptIO* iNotifier;
	};

#endif
