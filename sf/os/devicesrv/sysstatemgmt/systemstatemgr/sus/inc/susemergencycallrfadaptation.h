// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SUSEMERGENCYCALLRFADAPTATION_H__
#define __SUSEMERGENCYCALLRFADAPTATION_H__

#include <e32base.h>
#include "ssmadaptation.h"
#include "ssmdebug.h"
#include "ssmadaptationcmn.h"
#include "susadaptionsession.h"


/**
CEmergencyCallRfAdaptation

This class implements the EmergencyCallRf Adaptation related functionality as part of 
Adaptation server.This class implements an active object to route the requests
to Adaptation plugins loaded by CSsmAdaptationServer.When requests are received
from multiple clients this class queues the requests and processes them in order.
Adaptation server  handles only one request from each session and uses Cancel()
methods to cancel any of the session's requests made before.
The queueing mechanism is to handle multiple clients at a time not to handle multiple
requests from one particular clients session.

@internalComponent
*/
class CEmergencyAdaptationMessage;
class CEmergencyCallRfAdaptation : public CActive
{
public:
	static CEmergencyCallRfAdaptation* NewL(MEmergencyCallRfAdaptation* aAdaptation);
	~CEmergencyCallRfAdaptation();

	void Release();
	void DoActivateRfForEmergencyCallL(const RMessage2& aMessage);
	void DoDeactivateRfForEmergencyCallL(const RMessage2& aMessage);
	void DoEmergencyCallRfAdaptationCancelL(const RMessage2& aMessage);
	void SetPriorityClientSession(CSsmAdaptationSession* aPriorityClientSession);
	void RemovePriorityClientSession();
	void ReserveMemoryL();
	void DeleteAdaptationMessage();
	void NotifyAndRemoveAll();
	void Dequeue(CEmergencyAdaptationMessage *&aCurrentMessage);
protected:
	void RunL();
	TInt RunError( TInt aError );
	void DoCancel();
	void ConstructL(MEmergencyCallRfAdaptation* aAdaptation);

private:

	CEmergencyCallRfAdaptation();
	void SubmitOrQueueL(const RMessage2 &aMessage);
	void Submit(CEmergencyAdaptationMessage*& aMessage);
	
private:
	CEmergencyAdaptationMessage *iCurrentMessage;	
	RPointerArray<CEmergencyAdaptationMessage>iPendingEmergencyRequestsQueue;
	
	MEmergencyCallRfAdaptation* iEmergencyCallRfAdaptation;
	//For storing PriorityClientsession
    CSsmAdaptationSession* iPriorityClientSession;
    
    //Count of unused reserved pointers.  
    TInt iReserveMsgCount;
    //Pointer Array to store reserved adaptation message pointers
    RPointerArray<CEmergencyAdaptationMessage> iAdaptationReservedMessageArray; 
    };

enum TMessageStatus
    {
    EMsgStatusNULL = 0, //Default status of Message is 0
    EMsgReserved   = 1, //Flag to indicate the message is using Reserved memory.
    EMsgInUse      = 2  //Flag to indicate the Reserved message is Used.
    };
/**
 * Class to hold CAdaptationMessage pointer and the value to know whether the current message is from reserved pool
 * or normal message.
 */
class CEmergencyAdaptationMessage : public CBase
    {
public:    
    static CEmergencyAdaptationMessage* NewL(const RMessage2& aMessage);
    static CEmergencyAdaptationMessage* NewL(const RMessage2& aMessage, const TInt8 aMessageStatus);    
    
    ~CEmergencyAdaptationMessage();
    
    inline void UnsetMessageStatus(const TInt8 aMessageStatus);
    inline void SetMessageStatus(const TInt8 aMessageStatus);
    inline TBool IsMessageReserved() const;
    inline TBool IsMessageInuse() const;    
    void SetMessage(const RMessage2& aMessage);
    TInt Function() const;
    CSession2 *Session() const;
    void Complete(TInt aReason);
    
protected:
    void ConstructL(const RMessage2& aMessage);
    
private:
    CEmergencyAdaptationMessage(const TInt8 aMessageStatus);
    
private:
    CAdaptationMessage* iAdaptationMessage;
    // To know the Adaptation Message status.
    TInt8 iMessageStatus;
    };
#endif // __SUSEMERGENCYCALLRFADAPTATION_H__
