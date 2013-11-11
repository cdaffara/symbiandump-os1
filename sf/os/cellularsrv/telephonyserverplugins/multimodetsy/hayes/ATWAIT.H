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
// Wait for asynchronous notifications
// +CRING:		incoming call
// +CREG:		network registration change
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __ATWAIT_H__
#define __ATWAIT_H__

#include "ATBASE.H"

class CATWaitForCall;
class CSetLineToIdle : public CTimer
/**
@internalComponent
*/
	{
public:
	static CSetLineToIdle* NewL(CATWaitForCall* aWait);
	~CSetLineToIdle();
	void Start();
	void Stop();
protected:
	CSetLineToIdle(CATWaitForCall* aWait);
	virtual void RunL();
private:
	CATWaitForCall* iWait;
	};

class CCallHayes;
class CATWaitForCall : public CATBase
/**
@internalComponent
*/
	{
public:
	static CATWaitForCall* NewL(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	~CATWaitForCall();
	void StartWait();
	void EndRing();
	void ResetRingCounter();
	RMobilePhone::TMobilePhoneRegistrationStatus MappingRegistrationStatusFromETSIToMM(TInt *aNumber);
protected:
	CATWaitForCall(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	TInt ParseIncomingCallIndicationL();
	void ParseNetworkRegistrationL();
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void EventSignal(TEventSource aSource);
protected:
	CCommChatString* iRingExpectString;
	CCommChatString* iCRingExpectString;
	CCommChatString* iRegExpectString;
	CCommChatString* iTailExpectString;
	CSetLineToIdle* iSetLineToIdle;
	TInt iRingCounter;
	enum {
		EATWaitForUnsolicited,
		EATWaitForCRingRead,
		EATWaitForCRegRead
		} iState;
	};

#endif
