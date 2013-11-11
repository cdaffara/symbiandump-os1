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
 @internalAll 
*/

#ifndef __ATANSWER_H__
#define __ATANSWER_H__

#include "ATCALL.H"

class CATAnswerVoice : public CATVoiceCallConnectCommands
/**
@internalComponent
*/
	{
public:
	static CATAnswerVoice* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATAnswerVoice();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void Complete(TInt aError,TEventSource aSource);
protected:
	CATAnswerVoice(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
private:
	enum {
		EATNotInProgress,
		EATInitialising,
		EATSendAnswerCommand,
		EATAnswerWaitForWriteComplete,
		EATAnswerReadCompleted,
		EATTickleWaitForWriteComplete,
		EATTickleReadCompleted,
		EATCancellingWaitForWriteComplete,
		EATCancellingReadCompleted,
		EDTRDropped,
		EWaitForDTRRaiseSettle,
		EATHangupWaitForWriteComplete,
		EATHangupReadCompleted
		} iState;
	};


class CATWaitForNoCarrier;
class CATAnswerData : public CATDataCallConnectCommands
/**
@internalComponent
*/
	{
public:
	static CATAnswerData* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATAnswerData();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void Complete(TInt aError,TEventSource aSource);
protected:
	CATAnswerData(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
private:
	TInt ValidateAnswerExpectString();
private:
	enum {
		EATNotInProgress,
		EATInitialising,
		EATSendAnswerCommand,
		EATAnswerWaitForWriteComplete,
		EATAnswerReadCompleted,
		EATSpeedReadCompleted,
		EATCancellingWaitForWriteComplete,
		EATCancellingReadCompleted,
		EDTRDropped,
		EWaitForDTRRaiseSettle,
		EATHangupWaitForWriteComplete,
		EATHangupReadCompleted
		} iState;
	};



#endif
