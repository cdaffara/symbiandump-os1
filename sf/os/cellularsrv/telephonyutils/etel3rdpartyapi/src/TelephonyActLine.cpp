/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Code for TelephonyActPhone class, used by CTelephonyFunctions class.
*
*/


/**
 @file
*/

#include "TelephonyFunctions.h"
#include "TelephonyActLine.h"
#include "TelephonyActCall.h"

/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs this object.
@param  aIncomingCallNameSubject Reference to an active object owned by CTelephonyFunctions which implements the MIncomingCallNameSubject interface. Used by this object to reset the call name when the line is no longer ringing.
@leave	Leaves if no memory.
@return Pointer to the newly created CNotifyLineStatusAct object.
*/
CNotifyLineStatusAct* CNotifyLineStatusAct::NewL(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation, MIncomingCallNameSubject& aIncomingCallNameSubject)
	{
	CNotifyLineStatusAct* self = new(ELeave) CNotifyLineStatusAct(aTelephonyFunctions, aPendingOperation, aIncomingCallNameSubject);
	CActiveScheduler::Add(self);
	return self;
	}

/**
Default destructor.
*/
CNotifyLineStatusAct::~CNotifyLineStatusAct()
	{
	iState = ENotifyLineStatusActCleanUp;
	Cancel();
	Complete();
	}

/**
Issue Request.

The request for notification of line status can be either an internal
request or an external one.

1. Internal Request
An internal request can either come from the CTelephonyFunctions
object that owns this AO when it is initialised or from this AO
itself when the line status change event has completed and the
request is reposted. For internal requests, the aLineStatus pointer
is NULL.

2. External Request
An external request is one from a client using the CTelephony 3rd
Party API. In external requests, the aLineStatus parameter points
to a TCallStatusV1 object specified by the client to store the
resulting line status.

@param aLineStatus Package descriptor containing TCallStatus object which will hold the resulting line status.
*/
void CNotifyLineStatusAct::NotifyLineStatus(TDes8* aLineStatus)
	{
	// aLineStatus is NULL except when a client is interested
	// in the line status change.
	if(aLineStatus)
		{
		iLineStatusRequestedByClient = ETrue;
		__ASSERT_DEBUG(iISVCallStatus==NULL, User::Invariant());
		iISVCallStatus = reinterpret_cast<CTelephony::TCallStatusV1*> ( const_cast<TUint8*> (aLineStatus->Ptr()));
		}

	// At present only voice supported
	if(!IsActive())
		{
		iTelephonyFunctions->Line(CTelephony::EVoiceLine)->NotifyMobileLineStatusChange(iStatus,iMMCallStatus);
		SetActive();
		}
	}

/**
Called from CTelephonyFunctions::CancelAsync() to
notify this AO that the client is no longer interested in
the line status change.
Resets the iLineStatusRequestedByClient flag because client
is no longer interested in the line status change, however,
the AO still continues to monitor for the line status change
for notifications of an incoming call.
*/
void CNotifyLineStatusAct::CancelFromClient()
	{
	iLineStatusRequestedByClient = EFalse;
	iISVCallStatus = NULL;
	if(iTelephonyFunctions->IsRequestPending(iPendingOperation))
		{
		iTelephonyFunctions->CompleteRequest(iPendingOperation, KErrCancel);
		}
	}

/**
Service completed request.

This method copies the resulting line status change back to the
TCallStatusV1 variable supplied by the client and completes the
request back to the client if the client requested
notification of line status changes.

If the line is not ringing, then there is no incoming call and
the call name stored in the MIncomingCallNameSubject object
is reset. This prevents an old call from being answered.

The iLineStatusRequestedByClient flag is reset and the request
to listen for line status changes is then reposted except on
object destruction.
*/
void CNotifyLineStatusAct::Complete()
	{
	if(iStatus==KErrNone)
		{

		// If a client is interested in the line status change
		// (i.e. the  iLineStatusRequestedByClient flag is set),
		// the resulting line status change is copied back to the
		// parameter supplied by the client.
		if(iLineStatusRequestedByClient && iISVCallStatus)
			{
			CTelephonyFunctions::GetCallStatus(iMMCallStatus, iISVCallStatus->iStatus);
			}

		// Reset the call name if the line is no longer ringing.
		// This will ensure that the CAnswerIncomingCallAct AO will
		// only answer an incoming call when the notification of an
		// incoming call has been received
		// (CNotifyIncomingCallAct has completed) and will
		// also prevent CAnswerIncomingCallAct from attempting to
		// open an incoming call using an old call name.
		if (iMMCallStatus != RMobileCall::EStatusRinging)
			{
			iIncomingCallNameSubject.ResetCallName();
			}
		}

	if(iTelephonyFunctions->IsRequestPending(iPendingOperation))
		{
		iTelephonyFunctions->CompleteRequest(iPendingOperation, iStatus.Int());
		}

    // Reset iISVCallStatus to NULL since we don't need the pointer anymore
	iISVCallStatus = NULL;
	iLineStatusRequestedByClient = EFalse;

	// This object continuously listens for line status changes.
	// The request is only ever cancelled on object destruction,
	// do not repost the request in this case.
	if ((iStatus!=KRequestPending) && (iStatus != KErrCancel) && (iState != ENotifyLineStatusActCleanUp))
		{
		NotifyLineStatus(NULL);
		}
	}


/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
TInt CNotifyLineStatusAct::RunError(TInt aLeaveCode)
	{
	if(iTelephonyFunctions->IsRequestPending(iPendingOperation))
		{
		iTelephonyFunctions->CompleteRequest(iPendingOperation, aLeaveCode);
		}
	iLineStatusRequestedByClient = EFalse;
    iISVCallStatus = NULL;
	return KErrNone;
	}

/**
Cancel request.

Async request to be notified of line status changes is cancelled.
*/
void CNotifyLineStatusAct::DoCancel()
	{
	iTelephonyFunctions->Line(CTelephony::EVoiceLine)->CancelAsyncRequest(EMobileLineNotifyMobileLineStatusChange);
	iLineStatusRequestedByClient = EFalse;
    iISVCallStatus = NULL;
	}


/**
First-phase constructor which cannot Leave().

@param  aTelephonyFunctions object that constructs this object.
@param  aPendingOperation The pending operation.
@param  aIncomingCallNameSubject Reference to an active object owned by CTelephonyFunctions which implements the MIncomingCallNameSubject interface. Used by this object to reset the call name when the line is no longer ringing.
*/
CNotifyLineStatusAct::CNotifyLineStatusAct(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation, MIncomingCallNameSubject& aIncomingCallNameSubject)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iISVCallStatus(NULL),
  iPendingOperation(aPendingOperation),
  iIncomingCallNameSubject(aIncomingCallNameSubject),
  iLineStatusRequestedByClient(EFalse),
  iState(ENotifyLineStatusActOperating)
	{
	}
