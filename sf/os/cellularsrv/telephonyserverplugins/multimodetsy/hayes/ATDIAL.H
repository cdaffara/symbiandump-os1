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

#ifndef __ATDIAL_H__
#define __ATDIAL_H__

#include "ATCALL.H"


class CATDialVoice : public CATVoiceCallConnectCommands
/**
Dial command specific to voice call
@internalComponent
*/
	{
public:
	static CATDialVoice* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);

private:
	CATDialVoice(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);

	// CATCommands inherited stuff
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);

	// Utility methods
	TInt AddDialExpectStrings();
	TInt ValidateDialExpectString();
	void RemoveDialExpectStrings();

private:
	CCommChatString* iCallMonitoringExpectString;	// This object not owned by this class
	CCommChatString* iCallEndExpectString;			// This object not owned by this class
	CCommChatString* iBusyExpectString;			// This object not owned by this class
	CCommChatString* iNoDialToneExpectString;	// This object not owned by this class
	CCommChatString* iNoAnswerExpectString;		// This object not owned by this class
	CCommChatString* iDelayedExpectString;		// This object not owned by this class
	TDesC* iTelnum;
	enum {
		EATNotInProgress,
		EATInitialising,
		EATSendDialCommand,
		EATDialWaitForWriteComplete,
		EATDialReadComplete,
		EATTickleWaitForWriteComplete,
		EATTickleReadComplete,
		EATCancellingWaitForWriteComplete,
		EATCancellingReadCompleted,
		EDTRDropped,
		EWaitForDTRRaiseSettle,
		EATHangupWaitForWriteComplete,
		EATHangupReadCompleted
		} iState;

	//
	// The below TBuf class data are not strictly required, but they are used 
	// to help keep the memory usage of the TSY at a constant.
	TBuf8<KCommsDbSvrMaxFieldLength> iCallEndString;
	TBuf8<KCommsDbSvrMaxFieldLength> iBusyString;
	TBuf8<KCommsDbSvrMaxFieldLength> iNoDialToneString;
	TBuf8<KCommsDbSvrMaxFieldLength> iNoAnswerString;
	};


class CATDialData : public CATDataCallConnectCommands
/**
Dial command specific to data call
@internalAll
*/
	{
public:
	static CATDialData* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);

private:
	CATDialData(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);

	// CATCommands inherited stuff
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);

	// Utility functions
	void AddDialExpectStringsL();
	void ValidateDialExpectStringL();
	void RemoveDialExpectStrings();
	TBool SendBearerCapsCommand(TEventSource aSource);
	void StartDialCommand(TEventSource aSource);

private:
	CCommChatString* iBusyExpectString;			// This object not owned by this class
	CCommChatString* iSpeedExpectString;		// This object not owned by this class
	CCommChatString* iNoDialToneExpectString;	// This object not owned by this class
	CCommChatString* iNoAnswerExpectString;		// This object not owned by this class
	CCommChatString* iDelayedExpectString;		// This object not owned by this class
	TDesC* iTelnum;
	enum {
		EATNotInProgress,
		EATInitialising,
		EATBearerCapsWaitForWriteComplete,
		EATBearerCapsReadComplete,
		EATDialWaitForWriteComplete,
		EATDialReadComplete,
		EATSpeedReadComplete,
		EATCancellingWaitForWriteComplete,
		EATCancellingReadCompleted,
		EDTRDropped,
		EWaitForDTRRaiseSettle,
		EATHangupWaitForWriteComplete,
		EATHangupReadCompleted
		} iState;

	//
	// The below TBuf class data are not strictly required, but they are used 
	// to help keep the memory usage of the TSY at a constant.
	TBuf8<KCommsDbSvrMaxFieldLength> iBusyString;
	TBuf8<KCommsDbSvrMaxFieldLength> iNoDialToneString;
	TBuf8<KCommsDbSvrMaxFieldLength> iNoAnswerString;
	};


#endif
