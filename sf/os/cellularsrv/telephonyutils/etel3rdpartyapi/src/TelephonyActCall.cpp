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
* TelephonyActPhone.cpp
* Code for TelephonyActPhone class, used by CTelephonyFunctions class.
*
*/


/**
 @file
*/

#include "TelephonyFunctions.h"
#include "TelephonyActCall.h"

CDialNewCallAct* CDialNewCallAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CDialNewCallAct* self = new(ELeave) CDialNewCallAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CDialNewCallAct::~CDialNewCallAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CDialNewCallAct::DialNewCall(TDes8& aCallParams,const CTelephony::TTelNumber& aTelNumber, CTelephony::TCallId& aCallId,
								  CTelephony::TCallId& aTempCallId)
/**
Issue dial request
*/
	{
	iISVcallParams = reinterpret_cast<CTelephony::TCallParamsV1*> ( const_cast<TUint8*> (aCallParams.Ptr()) );
	iCallId=&aCallId;
	iTempCallId=aTempCallId;

	switch(iISVcallParams->iIdRestrict)
		{
		case CTelephony::ESendMyId:
			iMMcallParams.iIdRestrict = RMobileCall::ESendMyId;
			break;
		case CTelephony::EDontSendMyId:
			iMMcallParams.iIdRestrict = RMobileCall::EDontSendMyId;
			break;
		case CTelephony::EIdRestrictDefault:
		default:
			iMMcallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
			break;
		}
	iTelephonyFunctions->Call(iTempCallId)->DialISV(iStatus, iMMCallParamsPckg, aTelNumber);
	SetActive();
	}


void CDialNewCallAct::Complete()
/**
Service Completed request.

@leave	Leaves if System error.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EDialNewCall))
		{
		if(iStatus == KErrNone)
			{
			*iCallId=iTempCallId;
			}
		else if(iStatus != KRequestPending &&	iStatus != KErrNone)
	  		{
	  		iTelephonyFunctions->CloseAndReset(iTempCallId);
	  		}
	  	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EDialNewCall, iStatus.Int());
		}
	}


TInt CDialNewCallAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EDialNewCall, aLeaveCode);
	return KErrNone;
	}

void CDialNewCallAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	if(iTelephonyFunctions->Call(iTempCallId))
		{
		iTelephonyFunctions->Call(iTempCallId)->CancelAsyncRequest(EMobileCallDialISV);
		}
	}


CDialNewCallAct::CDialNewCallAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iMMCallParamsPckg(iMMcallParams)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}

CHoldAct* CHoldAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CHoldAct* self = new(ELeave) CHoldAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CHoldAct::~CHoldAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CHoldAct::Hold(const CTelephony::TCallId& aCallId)
/**
Issue Request
*/
	{
	iCallId = aCallId;
	iTelephonyFunctions->Call(aCallId)->Hold(iStatus);
	SetActive();
	}


void CHoldAct::Complete()
/**
Service Completed request.

@leave	Leaves if System error.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EHold, iStatus.Int());
	}


TInt CHoldAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EHold, aLeaveCode);
	return KErrNone;
	}

void CHoldAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	if(iTelephonyFunctions->Call(iCallId))
		{
		iTelephonyFunctions->Call(iCallId)->CancelAsyncRequest(EMobileCallHold);
		}
	}


CHoldAct::CHoldAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CResumeAct* CResumeAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CResumeAct* self = new(ELeave) CResumeAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CResumeAct::~CResumeAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CResumeAct::Resume(const CTelephony::TCallId& aCallId)
/**
Issue Request
*/
	{
	iCallId = aCallId;
	iTelephonyFunctions->Call(aCallId)->Resume(iStatus);
	SetActive();
	}


void CResumeAct::Complete()
/**
Service Completed request.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EResume, iStatus.Int());
	}


TInt CResumeAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EResume, aLeaveCode);
	return KErrNone;
	}

void CResumeAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	if(iTelephonyFunctions->Call(iCallId))
		{
		iTelephonyFunctions->Call(iCallId)->CancelAsyncRequest(EMobileCallResume);
		}
	}


CResumeAct::CResumeAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CSwapAct* CSwapAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CSwapAct* self = new(ELeave) CSwapAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CSwapAct::~CSwapAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CSwapAct::Swap(const CTelephony::TCallId& aCallId)
/**
Issue Request
*/
	{
	iCallId = aCallId;
	iTelephonyFunctions->Call(aCallId)->Swap(iStatus);
	SetActive();
	}


void CSwapAct::Complete()
/**
Service Completed request.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ESwap, iStatus.Int());
	}


TInt CSwapAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::ESwap, aLeaveCode);
	return KErrNone;
	}

void CSwapAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	if(iTelephonyFunctions->Call(iCallId))
		{
		iTelephonyFunctions->Call(iCallId)->CancelAsyncRequest(EMobileCallSwap);
		}
	}


CSwapAct::CSwapAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CHangupAct* CHangupAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CHangupAct* self = new(ELeave) CHangupAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CHangupAct::~CHangupAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CHangupAct::Hangup(const CTelephony::TCallId& aCallId)
/**
Issue Request
*/
	{
	iCallId = aCallId;
	iTelephonyFunctions->Call(aCallId)->HangUp(iStatus);
	SetActive();
	}


void CHangupAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EHangup))
		{
		if(iStatus == KErrNone)	//close the call handle if hangup was successful
	  		{
	  		//close and reset call handle
	  		iTelephonyFunctions->CloseAndReset(iCallId);
	  		}
	  	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EHangup, iStatus.Int());
		}
	}


TInt CHangupAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EHangup, aLeaveCode);
	return KErrNone;
	}

void CHangupAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	if(iTelephonyFunctions->Call(iCallId))
		{
		iTelephonyFunctions->Call(iCallId)->CancelAsyncRequest(EEtelCallHangUp);
		}
	}


CHangupAct::CHangupAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}

/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs this object.
@param  aIncomingCallNameSubject Reference to an active object owned by iTelephonyFunctions. Used to retrieve the call name of the new incoming call.
@param  aCallPool Reference to the call pool array owned by aTelephonyFunctions
@param  aCallPoolStatus Reference to the call pool status array owned by aTelephonyFunction

@leave	Leaves if no memory.
@return A pointer to the created CAnswerIncomingCallAct object.
*/
CAnswerIncomingCallAct* CAnswerIncomingCallAct::NewL(CTelephonyFunctions* aTelephonyFunctions, MIncomingCallNameSubject& aIncomingCallNameSubject, RArray<RMobileCall>& aCallPool, RArray<CTelephonyFunctions::TCallPoolOperation>& aCallPoolStatus)
	{
	CAnswerIncomingCallAct* self = new(ELeave) CAnswerIncomingCallAct(aTelephonyFunctions, aIncomingCallNameSubject, aCallPool, aCallPoolStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Second phase constructor.
*/
void CAnswerIncomingCallAct::ConstructL()
	{
	// Create timer for use later if needed.
	User::LeaveIfError(iTimer.CreateLocal());
	// Register interest in receiving notification when a
	// new incoming call is received.
	iIncomingCallNameSubject.RegisterObserver(this);
	CActiveScheduler::Add(this);
	}

/**
Default destructor.
*/
CAnswerIncomingCallAct::~CAnswerIncomingCallAct()
	{
	iIncomingCallNameSubject.DeregisterObserver(this);
	Cancel();
	Complete();
	iTimer.Close();
	}

/**
Issue Request.

This AO starts off in the EAnswerIncomingCallStateIdle state.
In order to answer the incoming call, the line must be ringing and
the CNotifyIncomingCallAct AO must have completed. When this
function is called, it checks if CNotifyIncomingCallAct
has a valid call name available. This means that the notify incoming
call event has completed and the call object has been created.
When this happens, the call can be answered. This AO then moves into
the EAnswerIncomingCallStateAnswering state and proceeds to answer
the call. If CNotifyIncomingCallAct does not have a valid
call name available, then there is no guarantee that the call object
exists and hence the call cannot be answered yet. If this happens,
this AO will move into the EAnswerIncomingCallStateWaiting
state and will wait for up to 1 second for CNotifyIncomingCallAct
AO to complete. If it does complete during or when this one second
interval is up, an attempt to answer the call will be made, otherwise
KErrTimedOut will be returned back to the client.

@param aCallId Stores the call ID which will be returned to the client.
@param aTempCallId Stores free call ID.
*/
void CAnswerIncomingCallAct::AnswerIncomingCall(CTelephony::TCallId& aCallId, CTelephony::TCallId& aTempCallId)
	{
	iCallId=&aCallId;
	iTempCallId=aTempCallId;

	switch (iState)
		{
	case EAnswerIncomingCallStateIdle:
	case EAnswerIncomingCallStateAnswering:
		{
		TName callName;
		if (iIncomingCallNameSubject.CallName(callName) == KErrNone)
			{
			// A valid call name is available, therefore there
			// is a new incoming call. The notify incoming call
			// event has completed, therefore it is possible to
			// answer the call.
			iState = EAnswerIncomingCallStateAnswering;

			// Create subsession with ringing call object.
			// Open a call using the voice line and the call name
			// of the incoming call.
			TInt ret = iCallPool[iTempCallId].OpenExistingCall(*(iTelephonyFunctions->Line(CTelephony::EVoiceLine)), callName);
			if (ret != KErrNone)
				{
				// Call could not be opened, close and complete
				// request back to the client.
				iTelephonyFunctions->CloseAndReset(iTempCallId);
				iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EAnswerIncomingCall, KErrAccessDenied);
				iState = EAnswerIncomingCallStateIdle;
				return;
				}
			// Post the request to answer the incoming call.
			iCallPoolStatus[iTempCallId]=CTelephonyFunctions::EAnswer;
			iTelephonyFunctions->Call(iTempCallId)->AnswerIncomingCallISV(iStatus, iMMCallParamsPckg);
			}
		else
			{
			// Call name is not available, NotifyIncomingCallAct AO has
			// not yet completed. Start the timer for 1 second.
			iState = EAnswerIncomingCallStateWaiting;
			iTimer.After(iStatus,KOneSecond);
			}
		SetActive();
		break;
		}
	case EAnswerIncomingCallStateWaiting:
	default:
		// This AO shouldn't be in any other state when this function
		// is called, therefore, complete the CTelephonyFunctions
		// request with an error.
		iStatus = KErrTimedOut;
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EAnswerIncomingCall, iStatus.Int());
		//iState = EAnswerIncomingCallStateIdle; // Should iState be set to idle?
		break;
		} // End switch(iState)
	}

/**
Service completed request.

Two different requests could have been issued by AnswerIncomingCall()
depending on the state of this AO.
If it is in the EAnswerIncomingCallStateAnswering state, then it has
just finished answering the incoming call.
If it is in the EAnswerIncomingCallStateWaiting state, then it was
waiting for the CNotifyIncomingCallAct AO to complete and the 1
second timeout has finished.  When this happens, it checks to see if
CNotifyIncomingCallAct contains a valid call name. If so, it has
completed, and this AO will post a request to answer the incoming call.
If not, then there is a problem and the KErrAccessDenied
error is returned via the iStatus status variable.
*/
void CAnswerIncomingCallAct::Complete()
	{
	if (iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EAnswerIncomingCall))
		{
		switch (iState)
			{
		case EAnswerIncomingCallStateAnswering:
			// Normal situation.
			if(iStatus == KErrNone)
				{
				*iCallId=iTempCallId;
				}
			else if((iStatus != KRequestPending) &&
	  				(iStatus != KErrNone))
	  			{
	  			iTelephonyFunctions->CloseAndReset(iTempCallId);
	  			}
	  		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EAnswerIncomingCall, iStatus.Int());
	  		iState = EAnswerIncomingCallStateIdle;
			break;
		case EAnswerIncomingCallStateWaiting:
			{
			TName callName;
			if (iIncomingCallNameSubject.CallName(callName) == KErrNone)
				{
				iState = EAnswerIncomingCallStateAnswering;
				AnswerIncomingCall(*iCallId, iTempCallId);
				}
			else
				{
				// Timer has finished and there is still no call name,
				// complete with an error.
				iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EAnswerIncomingCall, KErrTimedOut);
				iState = EAnswerIncomingCallStateIdle;
				}
			break;
			}
		// This AO can complete in the idle state on object destruction.
		// It should not reach this code in any other situation.
		case EAnswerIncomingCallStateIdle:
		default:
			iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EAnswerIncomingCall, KErrAccessDenied);
			iState = EAnswerIncomingCallStateIdle;
			break;
			} // switch (iState)
		}

	}

/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
TInt CAnswerIncomingCallAct::RunError(TInt aLeaveCode)
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EAnswerIncomingCall, aLeaveCode);
	iState = EAnswerIncomingCallStateIdle;
	return KErrNone;
	}

/**
Cancel request.

Async request to answer the call and timer are cancelled.
*/
void CAnswerIncomingCallAct::DoCancel()
	{
	if(iTelephonyFunctions->Call(iTempCallId))
		{
		iTelephonyFunctions->Call(iTempCallId)->CancelAsyncRequest(EMobileCallAnswerISV);
		}
	iTimer.Cancel();
	}


/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Pointer to the CTelephonyFunctions object that constructs this object.
@param  aIncomingCallNameSubject Reference to an AO owned by iTelephonyFunctions which implements the MIncomingCallNameSubject interface. Used by this object to retrieve the call name of the incoming call.
@param  aCallPool Reference to the call pool array owned by iTelephonyFunctions.
@param  aCallPoolStatus Reference to the call pool status array owned by iTelephonyFunctions.
*/
CAnswerIncomingCallAct::CAnswerIncomingCallAct(CTelephonyFunctions* aTelephonyFunctions, MIncomingCallNameSubject& aIncomingCallNameSubject, RArray<RMobileCall>& aCallPool, RArray<CTelephonyFunctions::TCallPoolOperation>& aCallPoolStatus)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iMMCallParamsPckg(iMMcallParams),
  iState(EAnswerIncomingCallStateIdle),
  iIncomingCallNameSubject(aIncomingCallNameSubject),
  iCallPool(aCallPool),
  iCallPoolStatus(aCallPoolStatus)
	{
	}

/**
Implementation of the MEventObserver interface.
If this object is in the EAnswerIncomingCallStateWaiting state, it
is waiting for up to 1 second for CNotifyIncomingCallAct to complete
so that it can answer the incoming call.  When
CNotifyIncomingCallAct completes during this 1 second period,
this function is called to stop the timer and proceed to answer
the call. After the timer is cancelled, the Active Scheduler will
automatically complete this AO.
*/
void CAnswerIncomingCallAct::EventCompleted()
	{
	if (iState == EAnswerIncomingCallStateWaiting)
		{
		iTimer.Cancel();
		}
	}

CNotifyCallStatusAct* CNotifyCallStatusAct::NewL(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CNotifyCallStatusAct* self = new(ELeave) CNotifyCallStatusAct(aTelephonyFunctions, aPendingOperation);
	CActiveScheduler::Add(self);
	return self;
	}

CNotifyCallStatusAct::~CNotifyCallStatusAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CNotifyCallStatusAct::NotifyCallStatus(TDes8& aId, const CTelephony::TCallId& aCallId)
/**
Issue Request
*/
	{
	iISVCallStatus = reinterpret_cast<CTelephony::TCallStatusV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	iCallId = aCallId;
	iTelephonyFunctions->Call(aCallId)->NotifyMobileCallStatusChange(iStatus,iMMCallStatus);
	SetActive();
	}


void CNotifyCallStatusAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(iPendingOperation))
		{
		if(iStatus==KErrNone)
			{
			CTelephonyFunctions::GetCallStatus(iMMCallStatus, iISVCallStatus->iStatus);
			}
		iTelephonyFunctions->CompleteRequest(iPendingOperation, iStatus.Int());
		}
	}


TInt CNotifyCallStatusAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(iPendingOperation, aLeaveCode);
	return KErrNone;
	}

void CNotifyCallStatusAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	if(iTelephonyFunctions->Call(iCallId)!=NULL)
		{
		iTelephonyFunctions->Call(iCallId)->CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
		}
	}


CNotifyCallStatusAct::CNotifyCallStatusAct(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iPendingOperation(aPendingOperation)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CNotifyRemotePartyInfoAct* CNotifyRemotePartyInfoAct::NewL(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CNotifyRemotePartyInfoAct* self = new(ELeave) CNotifyRemotePartyInfoAct(aTelephonyFunctions, aPendingOperation);
	CActiveScheduler::Add(self);
	return self;
	}

CNotifyRemotePartyInfoAct::~CNotifyRemotePartyInfoAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CNotifyRemotePartyInfoAct::NotifyRemotePartyInfo(TDes8& aId, const CTelephony::TCallId& aCallId)
/**
Issue Request
*/
	{
	iISVRemotePartyInfo = reinterpret_cast<CTelephony::TRemotePartyInfoV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	iCallId = aCallId;
	iTelephonyFunctions->Call(aCallId)->NotifyRemotePartyInfoChange(iStatus,iMMRemotePartyInfoPckg);
	SetActive();
	}


void CNotifyRemotePartyInfoAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(iPendingOperation))
		{
		if(iStatus==KErrNone)
			{
			switch(iMMRemotePartyInfo.iRemoteIdStatus)
				{
			case RMobileCall::ERemoteIdentityAvailable:
				iISVRemotePartyInfo->iRemoteIdStatus = CTelephony::ERemoteIdentityAvailable;
				break;
			case RMobileCall::ERemoteIdentitySuppressed:
				iISVRemotePartyInfo->iRemoteIdStatus = CTelephony::ERemoteIdentitySuppressed;
				break;
			case RMobileCall::ERemoteIdentityUnknown:
			default:
				iISVRemotePartyInfo->iRemoteIdStatus = CTelephony::ERemoteIdentityUnknown;
				break;
				}

			iISVRemotePartyInfo->iCallingName.Copy(iMMRemotePartyInfo.iCallingName);
			CTelephonyFunctions::CopyTelAddress(iMMRemotePartyInfo.iRemoteNumber, iISVRemotePartyInfo->iRemoteNumber);

			switch(iMMRemotePartyInfo.iDirection)
				{
			case RMobileCall::EMobileOriginated:
				iISVRemotePartyInfo->iDirection = CTelephony::EMobileOriginated;
				break;
			case RMobileCall::EMobileTerminated:
				iISVRemotePartyInfo->iDirection = CTelephony::EMobileTerminated;
				break;
			case RMobileCall::EDirectionUnknown:
			default:
				iISVRemotePartyInfo->iDirection = CTelephony::EDirectionUnknown;
				break;
				}
			}
		iTelephonyFunctions->CompleteRequest(iPendingOperation, iStatus.Int());
		}
	}


TInt CNotifyRemotePartyInfoAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(iPendingOperation, aLeaveCode);
	return KErrNone;
	}

void CNotifyRemotePartyInfoAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	if(iTelephonyFunctions->Call(iCallId)!=NULL)
		{
		iTelephonyFunctions->Call(iCallId)->CancelAsyncRequest(EMobileCallNotifyRemotePartyInfoChange);
		}
	}

/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
CNotifyRemotePartyInfoAct::CNotifyRemotePartyInfoAct(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iMMRemotePartyInfoPckg(iMMRemotePartyInfo),
  iPendingOperation(aPendingOperation)
	{
	}

/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs the returned object.
@param	aPendingOperation The pending operation.
@leave	Leaves if no memory.
@return Pointer to the created CNotifyIncomingCallAct object.
*/
CNotifyIncomingCallAct* CNotifyIncomingCallAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
	{
	CNotifyIncomingCallAct* self = new (ELeave) CNotifyIncomingCallAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

/**
Default destructor.
*/
CNotifyIncomingCallAct::~CNotifyIncomingCallAct()
	{
	iState = ENotifyIncomingCallActStateCleanUp;
	Cancel();
	Complete();
	iIncomingCallCompletionObservers.Close();
	}

/**
Issue request to be notified of an incoming call so that the call name can be retrieved.
*/
void CNotifyIncomingCallAct::NotifyIncomingCall()
	{
	// Only voice lines are supported
	iTelephonyFunctions->Line(CTelephony::EVoiceLine)->NotifyIncomingCall(iStatus, iCallName);
	SetActive();
	}

/**
Registers an observer with this object. Observers of this AO are AOs
owned by CTelephonyFunctions which want to be notified when this
AO completes (i.e. of the arrival of an incomingcall).

@param aOperation The operation that the aObserver active object performs.
@param aObserver Pointer to the observer object.
*/
void CNotifyIncomingCallAct::RegisterObserver(MEventObserver* aObserver)
	{
	iIncomingCallCompletionObservers.Append(aObserver);
	}

/**
Deregisters an observer with this object. The deregistered observer
will no longer be notified when this active object completes. If the
observer was registered more than once with this AO, this method will
only deregister one of those registrations.

@param aOperation The operation that the aObserver active object performs.
*/
void CNotifyIncomingCallAct::DeregisterObserver(MEventObserver* aObserver)
	{
	TInt pos = iIncomingCallCompletionObservers.Find(aObserver);
	if (pos != KErrNotFound)
		{
		iIncomingCallCompletionObservers.Remove(pos);
		}
	}

/**
Deletes the contents of iCallName.
*/
void CNotifyIncomingCallAct::ResetCallName()
	{
	iCallName.FillZ();
	iCallName.Zero();
	}

/**
Checks if there is a valid call name available and stores it in
aCallName if there is.

@param aCallName Stores the call name of the new incoming call if there is one available.

@return KErrNone if there is a valid call name available, KErrNotFound if there is not.
*/
TInt CNotifyIncomingCallAct::CallName(TName& aCallName) const
	{
	if (iCallName.Length() == 0)
		{
		return KErrNotFound;
		}
	else
		{
		aCallName = iCallName;
		return KErrNone;
		}
	}

/**
Service completed request.
*/
void CNotifyIncomingCallAct::Complete()
	{
	if (iStatus == KErrNone)
		{
		// Let all the observers interested in being notified of an
		// incoming call that an incoming call has been received.
		TInt numObservers = iIncomingCallCompletionObservers.Count();
		for (TInt i = 0; i < numObservers; i++)
			{
			if (iIncomingCallCompletionObservers[i] != NULL)
				{
				iIncomingCallCompletionObservers[i]->EventCompleted();
				}
			}
		}

	// Listen again for an incoming call.
	// This AO continuously listens for incoming calls, the request
	// will only be cancelled when the object is being destroyed,
	// in this case do not repost.
	if ((iStatus != KRequestPending) && (iStatus != KErrCancel) && (iState != ENotifyIncomingCallActStateCleanUp))
		{
		NotifyIncomingCall();
		}
	}

/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
TInt CNotifyIncomingCallAct::RunError(/*TInt aLeaveCode*/)
	{
	// ENotifyIncomingCall is not a supported client request, therefore,
	// don't need to complete back to the client.
	return KErrNone;
	}

/**
Cancel NotifyIncomingCall request.
*/
void CNotifyIncomingCallAct::DoCancel()
	{
	iTelephonyFunctions->Line(CTelephony::EVoiceLine)->NotifyIncomingCallCancel();
	}

/**
First-phase constructor which cannot Leave().

@param aTelephonyFunctions Pointer to the CTelephonyFunctions object that created this object.
@param aPendingOperation The pending operation.
*/
CNotifyIncomingCallAct::CNotifyIncomingCallAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
iTelephonyFunctions(aTelephonyFunctions),
iState(ENotifyIncomingCallActStateOperating)
	{
	}
