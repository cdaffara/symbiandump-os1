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
// CFaxDummyBase & CFaxDGprsTsy
//
/*******************************************************************/

//	CFaxDGprsTsy

CFaxDGprsTsy* CFaxDGprsTsy::NewL(CPhoneFactoryDummyBase* aFac)
/**
 * NewL method - 2 phase construction
 */
	{
	CFaxDGprsTsy* fax=new(ELeave) CFaxDGprsTsy(aFac);
	CleanupStack::PushL(fax);
	fax->ConstructL();
	CleanupStack::Pop();
	return fax;
	}

CFaxDGprsTsy::CFaxDGprsTsy(CPhoneFactoryDummyBase* aFac)
	:CFaxDummyBase(aFac)
	{}

void CFaxDGprsTsy::ConstructL()
	{
	TInt r = iChunk.CreateGlobal(KNullDesC,sizeof (RFax::TProgress), sizeof (RFax::TProgress),EOwnerProcess); 
	if (r == KErrNone)
		{
		RFax::TProgress* progress = new(iChunk.Base()) RFax::TProgress;
		progress->iLastUpdateTime = 0;
		progress->iAnswerback.Zero ();
		progress->iPhase = ENotYetStarted;
		progress->iSpeed = 9600;
		progress->iResolution = EFaxNormal;
		progress->iCompression = EModifiedHuffman;
		progress->iECM = 0;
 		progress->iPage = 0;
		progress->iLines = 0;
		}	
	else
		User::Leave(r);
	}

RHandleBase* CFaxDGprsTsy::GlobalKernelObjectHandle()
	{
		return &iChunk;
	}

CFaxDGprsTsy::~CFaxDGprsTsy()
	{
	iChunk.Close();
	}

TInt CFaxDGprsTsy::RegisterNotification(const TInt /*aIpc*/)
/**
 * RegisterNotification - Not Supported
 */
	{
	return KErrNotSupported;
	}

TInt CFaxDGprsTsy::DeregisterNotification(const TInt /*aIpc*/)
/**
 * DeRegisterNotification - Not Supported
 */
	{
	return KErrNotSupported;
	}

//	CFaxDummyBase
CFaxDummyBase::CFaxDummyBase(CPhoneFactoryDummyBase* aFac)
	{
	iFac=aFac;
	}

CFaxDummyBase::~CFaxDummyBase()
/**
 * Destructor
 */
	{
	iFac->RemoveTimer(iRead);
	iFac->RemoveTimer(iWrite);
	iFac->RemoveTimer(iWaitForEndOfPage);
	iFac->RemoveTimer(iProgressNotification);
	}

CFaxDummyBase* CFaxDummyBase::This(TAny* aPtr)
	{
	return REINTERPRET_CAST(CFaxDummyBase*,aPtr);
	}


TInt CFaxDummyBase::WaitForEndOfPageHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iWaitForEndOfPage);
	This(aPtr)->ReqCompleted(This(aPtr)->iWaitForEndOfPage.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::WaitForEndOfPage(const TTsyReqHandle aTsyReqHandle)
	{
	iFac->QueueTimer(iWaitForEndOfPage,aTsyReqHandle,DPCKTTSY_TIMEOUT,CFaxDummyBase::WaitForEndOfPageHandler,this);
	return KErrNone;
	}

TInt CFaxDummyBase::Read(const TTsyReqHandle aTsyReqHandle,TDes8* aDes)
	{
	iReadParams=aDes;
	iFac->QueueTimer(iRead,aTsyReqHandle,DPCKTTSY_TIMEOUT,CFaxDummyBase::ReadHandler,this);
	return KErrNone;
	}

TInt CFaxDummyBase::ReadHandler(TAny* aPtr)
	{
	This(aPtr)->iReadParams->Copy(DPCKTTSY_FAX_DATA);
	This(aPtr)->iFac->ResetPending( This(aPtr)->iRead);
	This(aPtr)->ReqCompleted(This(aPtr)->iRead.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::TerminateFaxSession(const TTsyReqHandle aTsyReqHandle)
	{
	if (iFac->RemoveTimer(iRead))
		ReqCompleted(iRead.iTsyReqHandle,KErrCancel);

	if (iFac->RemoveTimer(iWrite))
		ReqCompleted(iWrite.iTsyReqHandle,KErrCancel);

	if (iFac->RemoveTimer(iWaitForEndOfPage))
		ReqCompleted(iWaitForEndOfPage.iTsyReqHandle,KErrCancel);

	if (iFac->RemoveTimer(iProgressNotification))
		ReqCompleted(iProgressNotification.iTsyReqHandle,KErrCancel);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::Write(const TTsyReqHandle aTsyReqHandle,TDesC8* aDes)
	{
	if (aDes->Compare(DPCKTTSY_FAX_DATA)!=KErrNone)
		return KErrEtelDataCorrupted;
	iFac->QueueTimer(iWrite,aTsyReqHandle,DPCKTTSY_TIMEOUT,CFaxDummyBase::WriteHandler,this);
	return KErrNone;
	}

TInt CFaxDummyBase::WriteHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iWrite);
	This(aPtr)->ReqCompleted(This(aPtr)->iWrite.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::GetProgress(const TTsyReqHandle aTsyReqHandle,RFax::TProgress* aProgress)
	{
	aProgress->iSpeed=DPCKTTSY_GET_PROGRESS_SPEED;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::ProgressNotificationHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iProgressNotification);
	This(aPtr)->ReqCompleted(This(aPtr)->iProgressNotification.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CFaxDummyBase::ProgressNotification(const TTsyReqHandle aTsyReqHandle, RFax::TProgress* aProgress)
	{
	iProgressNotificationParams=aProgress;
	iFac->QueueTimer(iProgressNotification,aTsyReqHandle,DPCKTTSY_TIMEOUT,CFaxDummyBase::ProgressNotificationHandler,this);
	return KErrNone;
	}

TInt CFaxDummyBase::ProgressNotificationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	if(aTsyReqHandle==iProgressNotification.iTsyReqHandle)
		{
		iFac->RemoveTimer(iProgressNotification);
		ReqCompleted(aTsyReqHandle,KErrCancel);
		return KErrNone;
		}
	return KErrEtelNotRecognisedCancelHandle;
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
/** 
 * Destructor
 */
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
/**
 * RelinquishOwnership - Request not implemented in this dummy TSY
 */
	{
	RelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::TransferOwnership(const TTsyReqHandle aTsyReqHandle)
/**
 * TransferOwnership - Request not implemented in this dummy TSY
 */
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::AcquireOwnership(const TTsyReqHandle aTsyReqHandle)
/**
 * AcquireOwnership - Request not implemented in this dummy TSY
 */
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyCapsChange(const TTsyReqHandle aTsyReqHandle,RCall::TCaps*)
	{
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DPCKTTSY_TIMEOUT,CCallDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyHookChange(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus* /*aHookStatus*/)
/**
 * NotifyHookChange - Request not implemented in this dummy TSY
 */
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	iNotifyStatusChangeParams=aStatus;
	iFac->QueueTimer(iNotifyStatusChange,aTsyReqHandle,DPCKTTSY_TIMEOUT,CCallDummyBase::NotifyStatusChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::NotifyDurationChange(const TTsyReqHandle aTsyReqHandle,TTimeIntervalSeconds*)
	{
	iFac->QueueTimer(iNotifyDurationChange,aTsyReqHandle,DPCKTTSY_TIMEOUT,CCallDummyBase::NotifyDurationChangeHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::GetInfo(const TTsyReqHandle aTsyReqHandle, RCall::TCallInfo* /*aCallInfo*/)
/**
 * GetInfo - Request not implemented in this dummy TSY
 */
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetStatus(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	*aStatus=DPCKTTSY_CORE_CALL_STATUS;
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
	iFac->QueueTimer(iLoanDataPortTimer,aTsyReqHandle,DPCKTTSY_TIMEOUT,CCallDummyBase::LoanDataPortHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::RecoverDataPort(const TTsyReqHandle aTsyReqHandle)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::RecoverDataPortAndRelinquishOwnership()
/**
 * RecoverDataPortAndRelinquishOwnership - Request not implemented in this dummy TSY
 */
	{
	RecoverDataPortAndRelinquishOwnershipCompleted(KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::Dial(const TTsyReqHandle aTsyReqHandle,const TDesC8*, TDesC*)
	{
	iFac->QueueTimer(iDial,aTsyReqHandle,DPCKTTSY_TIMEOUT,CCallDummyBase::DialHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::Connect(const TTsyReqHandle aTsyReqHandle,const TDesC8*)
/**
 * Connect - Request not implemented in this dummy TSY
 */
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::AnswerIncomingCall(const TTsyReqHandle aTsyReqHandle,const TDesC8*)
	{
	iFac->QueueTimer(iAnswer,aTsyReqHandle,DPCKTTSY_TIMEOUT,CCallDummyBase::AnswerHandler,this);
	return KErrNone;
	}

TInt CCallDummyBase::HangUp(const TTsyReqHandle aTsyReqHandle)
/**
 * HangUp method
 * If this is a voice call then any client can hang it up.
 * If this is a data or fax call then only the owning client can hang it up.
 */

	{
	if (CheckOwnership(aTsyReqHandle)==CCallBase::EOwnedTrue)
		{
		iFac->QueueTimer(iHangUp,aTsyReqHandle,DPCKTTSY_TIMEOUT,CCallDummyBase::HangUpHandler,this);
		return KErrNone;
		}
	else
		return KErrEtelNotCallOwner;
	}

TInt CCallDummyBase::GetBearerServiceInfo(const TTsyReqHandle aTsyReqHandle,RCall::TBearerService* aService)
	{
	aService->iBearerSpeed=DPCKTTSY_CALL_BEARER_SPEED;
	aService->iBearerCaps=DPCKTTSY_CALL_BEARER_CAPS;
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
	*aTime = DPCKTTSY_CALL_DURATION1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::GetFaxSettings(const TTsyReqHandle aTsyReqHandle,RCall::TFaxSessionSettings* aSettings)
	{
	aSettings->iFaxId=DPCKTTSY_MYFAX;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CCallDummyBase::SetFaxSettings(const TTsyReqHandle aTsyReqHandle,const RCall::TFaxSessionSettings* aSettings)
	{
	if (aSettings->iFaxId.Compare(DPCKTTSY_MYFAX)!=KErrNone)
		return (KErrEtelDataCorrupted);

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}


/*******************************************************************/
//
// Cancellation functions for call requests
//
/*******************************************************************/

TInt CCallDummyBase::AcquireOwnershipCancel(const TTsyReqHandle aTsyReqHandle)
/**
 * AcquireOwnershipCancel - Request not implemented in this dummy TSY
 */
	{
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
/**
 * NotifyHookChangeCancel - Request not implemented in this dummy TSY
 */
	{
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
/**
 * ConnectCancel - Request not implemented in this dummy TSY
 */
	{
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
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DPCKTTSY_TIMEOUT,CLineDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyStatusChange(const TTsyReqHandle aTsyReqHandle,RCall::TStatus* aStatus)
	{
	iStatusParams=aStatus;
	iFac->QueueTimer(iNotifyStatusChange,aTsyReqHandle,DPCKTTSY_TIMEOUT,CLineDummyBase::NotifyStatusChangeHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyIncomingCall(const TTsyReqHandle aTsyReqHandle, TName* aName)
	{
	iNameParams=aName;
	iFac->QueueTimer(iNotifyIncomingCall,aTsyReqHandle,DPCKTTSY_TIMEOUT,CLineDummyBase::NotifyIncomingCallHandler,this);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyHookChange(const TTsyReqHandle aTsyReqHandle, RCall::THookStatus* /*aHookStatus*/)
/**
 * NotifyHookChange - Request not implemented in this dummy TSY
 */
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyCallAdded(const TTsyReqHandle aTsyReqHandle, TName* /*aName*/)
	{
	iFac->QueueTimer(iNotifyCallAdded,aTsyReqHandle,DPCKTTSY_TIMEOUT,CLineDummyBase::NotifyCallAddedHandler,this);
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
	*aStatus=DPCKTTSY_CORE_LINE_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetHookStatus(const TTsyReqHandle aTsyReqHandle,RCall::THookStatus*)
/**
 * GetHookStatus - Request not implemented in this dummy TSY
 */
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::EnumerateCall(const TTsyReqHandle aTsyReqHandle, TInt*)
/**
 * EnumerateCall - Request not implemented in this dummy TSY
 */
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex*)
/**
 * GetCallInfo - Request not implemented in this dummy TSY
 */
	{
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
/**
 * NotifyHookChangeCancel - Request not implemented in this dummy TSY
 */
	{
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
	This(aPtr)->iCapsParams->iFlags=DPCKTTSY_CORE_LINE_CAPS;
	This(aPtr)->ReqCompleted(This(aPtr)->iNotifyCapsChange.iTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CLineDummyBase::NotifyStatusChangeHandler(TAny* aPtr)
	{
	This(aPtr)->iFac->ResetPending( This(aPtr)->iNotifyStatusChange);
	*(This(aPtr)->iStatusParams)=DPCKTTSY_CORE_LINE_STATUS2;
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
	iFac->QueueTimer(iNotifyCapsChange,aTsyReqHandle,DPCKTTSY_TIMEOUT,CPhoneDummyBase::NotifyCapsChangeHandler,this);
	return KErrNone;
	}

TInt CPhoneDummyBase::NotifyModemDetected(const TTsyReqHandle aTsyReqHandle,RPhone::TModemDetection* aDetection)
	{
	iDetectionParams=aDetection;
	iFac->QueueTimer(iNotifyPhoneDetected,aTsyReqHandle,DPCKTTSY_TIMEOUT,CPhoneDummyBase::NotifyPhoneDetectedHandler,this);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetInfo(const TTsyReqHandle aTsyReqHandle, RPhone::TPhoneInfo* /*aPhoneInfo*/)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetStatus(const TTsyReqHandle aTsyReqHandle,RPhone::TStatus* aPhoneStat)
	{
	aPhoneStat->iModemDetected=DPCKTTSY_CORE_PHONE_STATUS;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetCaps(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps*)
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::EnumerateLines(const TTsyReqHandle aTsyReqHandle, TInt*)
/**
 * EnumerateLines - Request not implemented in this dummy TSY
 */
	{
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CPhoneDummyBase::GetLineInfo(const TTsyReqHandle aTsyReqHandle, TLineInfoIndex* /*aLineInfo*/)
/**
 * GetLineInfo - Request not implemented in this dummy TSY
 */
	{
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
	TRAP_IGNORE(iTimer=CDeltaTimer::NewL(CActive::EPriorityHigh,KEtelTimerGranularity));
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
		aInfo.iNetworkType=DPCKTTSY_NETWORK_TYPE;
		aInfo.iName=DPCKTTSY_PHONE_NAME;
		aInfo.iNumberOfLines=DPCKTTSY_NUMBER_OF_LINES;
		return KErrNone;
		}
	else
		return KErrNotFound;
	}

TInt CPhoneFactoryDummyBase::EnumeratePhones()
	{
	return (DPCKTTSY_NUMBER_OF_PHONES);
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

