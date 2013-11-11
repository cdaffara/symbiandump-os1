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
// Dummy TSY classes that represent the core ETel API support.
// 
//

/**
 @file
*/

#include "coretsy.h"
#include "testdef.h"

#include <et_clsvr.h>

void TsyPanic(TTsyPanic aPanic)
	{
	_LIT(KTsyPanic,"Tsy Panic");
	User::Panic(KTsyPanic,aPanic);
	}

TTsyTimer::TTsyTimer()
	{
	iPending=EFalse;
	}

/*******************************************************************/
//
// CCallDummyBase
//
/*******************************************************************/

CCallDummyBase::CCallDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

void CCallDummyBase::ConstructL()
	{
	}

CCallDummyBase::~CCallDummyBase()
	{
	iFac->RemoveTimer(iNotifyStatusChange);
	iFac->RemoveTimer(iDial);
	iFac->RemoveTimer(iAnswer);
	iFac->RemoveTimer(iHangUp);
	}

CCallDummyBase* CCallDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CCallDummyBase*,aPtr);
	}

CPhoneFactoryDummyBase* CCallDummyBase::FacPtr() const
	{
	return iFac;
	}

/*******************************************************************/
//
// Core ETel API call requests - all these must be implemented by TSY
// even if just to return KErrNotSupported
//
/*******************************************************************/

TInt CCallDummyBase::RelinquishOwnership()
	{
	// Request not implemented in this dummy TSY
	RelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::TransferOwnership(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::AcquireOwnership(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RCall::TCaps*)
	{
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DSATTSY_TIMEOUT,CCallDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyHookChange(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus* /*aHookStatus*/)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	iNotifyStatusChangeParams=aStatus;
	iFac->QueueTimer(iNotifyStatusChange,aTsyReqHandle,DSATTSY_TIMEOUT,CCallDummyBase::NotifyStatusChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyDurationChange(const TTsyReqHandle aTsyReqHandle,TTimeIntervalSeconds*)
	{
	iFac->QueueTimer(iNotifyDurationChange,aTsyReqHandle,DSATTSY_TIMEOUT,CCallDummyBase::NotifyDurationChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::GetInfo(const TTsyReqHandle aTsyReqHandle, RCall::TCallInfo* /*aCallInfo*/)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	*aStatus=DSATTSY_CORE_CALL_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetCaps(const TTsyReqHandle aTsyReqHandle,RCall::TCaps*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::LoanDataPort(const TTsyReqHandle aTsyReqHandle, RCall::TCommPort* /*aCommPort*/)
	{
	iFac->QueueTimer(iLoanDataPortTimer,aTsyReqHandle,DSATTSY_TIMEOUT,CCallDummyBase::LoanDataPortHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::RecoverDataPort(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::RecoverDataPortAndRelinquishOwnership()
	{
	// Request not implemented in this dummy TSY
	RecoverDataPortAndRelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8*, TDesC*)
	{
	iFac->QueueTimer(iDial,aTsyReqHandle,DSATTSY_TIMEOUT,CCallDummyBase::DialHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8*)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8*)
	{
	iFac->QueueTimer(iAnswer,aTsyReqHandle,DSATTSY_TIMEOUT,CCallDummyBase::AnswerHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::HangUp(const TTsyReqHandle aTsyReqHandle)
	{
	// If this is a voice call then any client can hang it up.
	// If this is a data or fax call then only the owning client can hang it up
	if (CheckOwnership(aTsyReqHandle)==CCallBase::EOwnedTrue)
		{
		iFac->QueueTimer(iHangUp,aTsyReqHandle,DSATTSY_TIMEOUT,CCallDummyBase::HangUpHandler,this);
		return KErrNone;
		}
	else
		return KErrEtelNotCallOwner;
	}

TInt CCallDummyBase::GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService* aService)
	{
	aService->iBearerSpeed=DSATTSY_CALL_BEARER_SPEED;
	aService->iBearerCaps=DSATTSY_CALL_BEARER_CAPS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetCallParams(const TTsyReqHandle aTsyReqHandle, TDes8*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetCallDuration(const TTsyReqHandle aTsyReqHandle, TTimeIntervalSeconds* aTime)
	{
	*aTime = DSATTSY_CALL_DURATION1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetFaxSettings(const TTsyReqHandle aTsyReqHandle,RCall::TFaxSessionSettings* aSettings)
	{
	aSettings->iFaxId=DSATTSY_MYFAX;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::SetFaxSettings(const TTsyReqHandle aTsyReqHandle,const RCall::TFaxSessionSettings* aSettings)
	{
	if (aSettings->iFaxId.Compare(DSATTSY_MYFAX)!=KErrNone)
		return(KErrEtelDataCorrupted);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// Cancellation functions for call requests
//
/*******************************************************************/

TInt CCallDummyBase::AcquireOwnershipCancel(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyCapsChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyCapsChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyStatusChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyStatusChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::NotifyDurationChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyDurationChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyDurationChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::LoanDataPortCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iLoanDataPortTimer.iTsyReqHandle)
		{
		iFac->RemoveTimer(iLoanDataPortTimer);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::DialCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iDial.iTsyReqHandle)
		{
		iFac->RemoveTimer(iDial);
		SetUnowned();
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::ConnectCancel(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CCallDummyBase::AnswerIncomingCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iAnswer.iTsyReqHandle)
		{
		iFac->RemoveTimer(iAnswer);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CCallDummyBase::HangUpCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iHangUp.iTsyReqHandle)
		{
		iFac->RemoveTimer(iHangUp);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}


/*******************************************************************/
//
// Asynchronous handler functions - called after a timeout
//
/*******************************************************************/

TInt CCallDummyBase::NotifyCapsChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCapsChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCapsChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyStatusChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyStatusChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyStatusChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyDurationChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyDurationChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyDurationChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::DialHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iDial);
	This(aPtr)->ReqCompleted(This(aPtr)->iDial.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::AnswerHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iAnswer);
	This(aPtr)->ReqCompleted(This(aPtr)->iAnswer.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::HangUpHandler(TAny* aPtr)
	{
	This(aPtr)->SetUnowned();
	This(aPtr)->iFac->ResetPending( This(aPtr)->iHangUp);
	This(aPtr)->ReqCompleted(This(aPtr)->iHangUp.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::LoanDataPortHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iLoanDataPortTimer);
	This(aPtr)->ReqCompleted(This(aPtr)->iLoanDataPortTimer.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

/*******************************************************************/
//
// CLineDummyBase
//
/*******************************************************************/

CLineDummyBase::CLineDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

void CLineDummyBase::ConstructL()
	{
	}

CLineDummyBase::~CLineDummyBase()
	{
	iFac->RemoveTimer(iNotifyIncomingCall);
	iFac->RemoveTimer(iNotifyStatusChange);
	}

CLineDummyBase* CLineDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CLineDummyBase*,aPtr);
	}

CPhoneFactoryDummyBase* CLineDummyBase::FacPtr() const
	{
	return iFac;
	}

/*******************************************************************/
//
// Core ETel API line requests - all these must be implemented by TSY
// even if just to return KErrNotSupported
//
/*******************************************************************/

TInt CLineDummyBase::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RLine::TCaps* aCaps)
	{
	iCapsParams=aCaps;
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DSATTSY_TIMEOUT,CLineDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	iStatusParams=aStatus;
	iFac->QueueTimer(iNotifyStatusChange,aTsyReqHandle,DSATTSY_TIMEOUT,CLineDummyBase::NotifyStatusChangeHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyIncomingCall(const TTsyReqHandle aTsyReqHandle, TName* aName)
	{
	iNameParams=aName;
	iFac->QueueTimer(iNotifyIncomingCall,aTsyReqHandle,DSATTSY_TIMEOUT,CLineDummyBase::NotifyIncomingCallHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* /*aHookStatus*/)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCallAdded(const TTsyReqHandle aTsyReqHandle, TName* /*aName*/)
	{
	iFac->QueueTimer(iNotifyCallAdded,aTsyReqHandle,DSATTSY_TIMEOUT,CLineDummyBase::NotifyCallAddedHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::GetCaps(const TTsyReqHandle aTsyReqHandle,RLine::TCaps*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetInfo(const TTsyReqHandle aTsyReqHandle, RLine::TLineInfo* /*aLineInfo*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	*aStatus=DSATTSY_CORE_LINE_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetHookStatus(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus*)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::EnumerateCall(const TTsyReqHandle aTsyReqHandle, TInt*)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex*)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// Cancellation functions for line requests
//
/*******************************************************************/

TInt CLineDummyBase::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyCapsChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyCapsChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CLineDummyBase::NotifyStatusChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyStatusChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyStatusChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CLineDummyBase::NotifyIncomingCallCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyIncomingCall.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyIncomingCall);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CLineDummyBase::NotifyHookChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCallAddedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyCallAdded.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyCallAdded);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

/*******************************************************************/
//
// Asynchronous handler functions - called after a timeout
//
/*******************************************************************/

TInt CLineDummyBase::NotifyCapsChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCapsChange);
	This(aPtr)->iCapsParams->iFlags=DSATTSY_CORE_LINE_CAPS;
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCapsChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyStatusChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyStatusChange);
	*(This(aPtr)->iStatusParams)=DSATTSY_CORE_LINE_STATUS2;
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyStatusChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyIncomingCallHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyIncomingCall);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyIncomingCall.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCallAddedHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCallAdded);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCallAdded.iTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// CPhoneDummyBase
//
/*******************************************************************/

CPhoneDummyBase::CPhoneDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

void CPhoneDummyBase::ConstructL()
	{
	}

CPhoneDummyBase::~CPhoneDummyBase()
	{
	iFac->RemoveTimer(iNotifyPhoneDetected);
	iFac->RemoveTimer(iNotifyCapsChange);
	}

CPhoneDummyBase* CPhoneDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CPhoneDummyBase*,aPtr);
	}

CPhoneFactoryDummyBase* CPhoneDummyBase::FacPtr() const
	{
	return iFac;
	}

/*******************************************************************/
//
// Core ETel API phone requests - all these must be implemented by TSY
// even if just to return KErrNotSupported
//
/*******************************************************************/

TInt CPhoneDummyBase::ControlledInitialisation(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps*)
	{
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DSATTSY_TIMEOUT,CPhoneDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyModemDetected(const TTsyReqHandle aTsyReqHandle,RPhone::TModemDetection* aDetection)
	{
	iDetectionParams=aDetection;
	iFac->QueueTimer(iNotifyPhoneDetected,aTsyReqHandle,DSATTSY_TIMEOUT,CPhoneDummyBase::NotifyPhoneDetectedHandler,this);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetInfo(const TTsyReqHandle aTsyReqHandle, RPhone::TPhoneInfo* /*aPhoneInfo*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetStatus(const TTsyReqHandle aTsyReqHandle,RPhone::TStatus* aPhoneStat)
	{
	aPhoneStat->iModemDetected=DSATTSY_CORE_PHONE_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetCaps(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::EnumerateLines(const TTsyReqHandle aTsyReqHandle, TInt*)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetLineInfo(const TTsyReqHandle aTsyReqHandle, TLineInfoIndex* /*aLineInfo*/)
	{
	// Request not implemented in this dummy TSY
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// Cancellation functions for line requests
//
/*******************************************************************/

TInt CPhoneDummyBase::ControlledInitialisationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyCapsChange.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyCapsChange);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

TInt CPhoneDummyBase::NotifyModemDetectedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if (aTsyReqHandle==iNotifyPhoneDetected.iTsyReqHandle)
		{
		iFac->RemoveTimer(iNotifyPhoneDetected);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
	}

/*******************************************************************/
//
// Asynchronous handler functions - called after a timeout
//
/*******************************************************************/

TInt CPhoneDummyBase::NotifyCapsChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyCapsChange);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCapsChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyPhoneDetectedHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending(This(aPtr)->iNotifyPhoneDetected);
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyPhoneDetected.iTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// CPhoneFactoryDummyBase
//
/*******************************************************************/

CPhoneFactoryDummyBase::CPhoneFactoryDummyBase()
	{
	}

void CPhoneFactoryDummyBase::ConstructL()
	{
	iTimer=CDeltaTimer::NewL(CActive::EPriorityHigh,KEtelTimerGranularity);
	}

CPhoneFactoryDummyBase::~CPhoneFactoryDummyBase()
	{
	delete iTimer;
	}

void CPhoneFactoryDummyBase::QueueTimer(TTsyTimer& aTsyTimer, const TTsyReqHandle aTsyReqHandle,TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,
										TInt (*aFunction)(TAny *aPtr), TAny* aPtr)
	{
	TCallBack callBackFn(aFunction,aPtr);
 	
	aTsyTimer.iEntry.Set(callBackFn);
	aTsyTimer.iPending=ETrue;
	aTsyTimer.iTsyReqHandle=aTsyReqHandle;

	aTimeInMicroSeconds=aTimeInMicroSeconds.Int()+(KEtelTimerGranularity>>2);
	if(aTimeInMicroSeconds.Int()<100000)
		aTimeInMicroSeconds=aTimeInMicroSeconds.Int()+KEtelTimerGranularity;
	
	iTimer->Queue(aTimeInMicroSeconds,aTsyTimer.iEntry);
	}

TBool CPhoneFactoryDummyBase::RemoveTimer(TTsyTimer& aTsyTimer)
	{
	if (aTsyTimer.iPending)
		{
		aTsyTimer.iPending=EFalse;
		iTimer->Remove(aTsyTimer.iEntry);
		return ETrue;
		}
	return EFalse;
	}

void CPhoneFactoryDummyBase::ResetPending(TTsyTimer& aTsyTimer)
	{
	aTsyTimer.iPending=EFalse;
	}

TInt CPhoneFactoryDummyBase::GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aInfo)
	{
	if (aIndex==0)
		{
		aInfo.iNetworkType=DSATTSY_NETWORK_TYPE;
		aInfo.iName=DSATTSY_PHONE_NAME;
		aInfo.iNumberOfLines=DSATTSY_NUMBER_OF_LINES;
		return KErrNone;
		}
	else
		return KErrNotFound;
	}

TInt CPhoneFactoryDummyBase::EnumeratePhones()
	{
	return (DSATTSY_NUMBER_OF_PHONES);
	}

/*******************************************************************/
//
// CSubSessionExtDummyBase
//
/*******************************************************************/

CSubSessionExtDummyBase::CSubSessionExtDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

CSubSessionExtDummyBase::~CSubSessionExtDummyBase()
	{}

CPhoneFactoryDummyBase* CSubSessionExtDummyBase::FacPtr() const
	{
	return iFac;
	}

