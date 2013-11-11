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
// HangUp command
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __ATHANGUP_H__
#define __ATHANGUP_H__

#include "ATCALL.H"

class CCallMobileVoice;
class CATHangUpVoice : public CATCallAlterCommands
/**
@internalComponent
*/
	{
public:
	static CATHangUpVoice* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	CATHangUpVoice(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATHangUpVoice();
	void ExecuteCommand(TTsyReqHandle aTsyReqHandle, TAny* aParams,TCallInfoTSY* aCallInfo);
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
protected:
	CCommChatString* iNoCarrierExpectString;
	TInt ValidateHangUpExpectString();
	virtual void EventSignal(TEventSource aSource);
	virtual void Complete(TInt aError,TEventSource aSource);
	enum {
		EATNotInProgress,
		EATHangupWaitForWriteComplete,
		EATHangupReadCompleted,
		EHangUpCancelling
		} iState;
	};


class CATHangUpData : public CATCallAlterCommands
/**
@internalTechnology
*/
	{
public:
	static CATHangUpData* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	CATHangUpData(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATHangUpData();
	void ExecuteCommand(TTsyReqHandle aTsyReqHandle, TAny* aParams,TCallInfoTSY* aCallInfo);
	virtual void Start(TTsyReqHandle aTsyReqHandle,TAny*);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
protected:
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void Complete(TInt aError,TEventSource aSource);
private:
	void ValidateHangUpExpectStringL();
private:
	CCommChatString* iNoCarrierExpectString;
	TInt iErrorCode;
	enum {
		EATNotInProgress,
		EDTRDropped,
		EWaitForDTRRaiseSettle,
		EATEscapeSeqWaitForWriteComplete,
		EATEscapeSeqCompleted,
		EATHangupWaitForWriteComplete,
		EATHangupReadCompleted,
		EHangUpCancelling
		} iState;
	};
#endif
