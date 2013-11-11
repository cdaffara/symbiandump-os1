// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ReqstObject.h"
#include "ReqstDoer.h"
#include "rpsasciirqstdefs.h"
#include <e32debug.h>
#include "RpsSFlogger.h"
 
CReqstObjectBase::CReqstObjectBase(CEtelMngr& aEtel, TEtelLine aLine)
	: CActive(EPriorityStandard), iEtel(aEtel), iLine(aLine)
	{
	}
	
CReqstObjectBase::~CReqstObjectBase()
	{
	iObservArray.Close();
	}
	
void CReqstObjectBase::RunL()
	{
	LOGLINE2(_L(">>CReqstObjectBase::RunL [%d]"),iStatus.Int());
	if(iStatus != KErrNone)
		{
		// Log here...
		}
	NotifyL();
	LOGLINE2(_L("<<CReqstObjectBase::RunL [%d]"),iStatus.Int());
	}
	
void CReqstObjectBase::AttachL(MObserver* aObserver)
	{
	iObservArray.AppendL(aObserver);
	}
	
void CReqstObjectBase::DetachL(MObserver* aObserver)
	{
	TInt indx = iObservArray.FindL(aObserver);
	iObservArray.Remove(indx);
	}
	
void CReqstObjectBase::NotifyL()
	{
	for(TInt i=0; i < iObservArray.Count(); ++i)
		{
		iObservArray[i]->UpdateL(this);
		}
	}
	
void CReqstObjectBase::Abort()
	{
	for(TInt i=0; i < iObservArray.Count(); ++i)
		{
		iObservArray[i]->Abort();
		}
	}

//--------------------------------------------------------------------------------------------------	

//---------------
// CCallNumber
//---------------
CCallNumber* CCallNumber::NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aTelNum)
	{
	CCallNumber* self = new(ELeave)CCallNumber(aEtel, aLine);
	CleanupStack::PushL(self);
	self->ConstructL(aTelNum);
	CleanupStack::Pop(self);
	return self;
	}
	
CCallNumber::~CCallNumber()
	{
	Cancel();				// for CActive
	}

void CCallNumber::ConstructL(const TDesC8& aTelNum)
	{
	LOGLINE1(_L(">>CCallNumber::ConstructL"));
	CActiveScheduler::Add(this);
	__ASSERT_DEBUG(aTelNum.Length() < KMaxTelNumberSize,User::Panic(KPanicMsgFormat, KTelNumberTooBig));
	iTelNumber.Copy(aTelNum);
	LOGLINE1(_L("<<CCallNumber::ConstructL"));
	}

CCallNumber::CCallNumber(CEtelMngr& aEtel, TEtelLine aLine)
 : CReqstObjectBase(aEtel, aLine)
	{
	}

void CCallNumber::DoCancel()
	{
	// cancel any uncompleted asynch reqsts to Etel
	LOGLINE1(_L(">>CCallNumber::DoCancel"));
	iEtel.Call(iLine).DialCancel();
	LOGLINE1(_L("<<CCallNumber::DoCancel"));
	}
	
void CCallNumber::StartL()
	{
	LOGLINE1(_L(">>CCallNumber::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	
	iEtel.Call(iLine).Dial(iStatus , iTelNumber);
	SetActive();
	LOGLINE1(_L("<<CCallNumber::StartL"));
	}
	

//---------------
// CCallNumberNotifyStatusChange
//---------------
CCallNumberNotifyStatusChange* CCallNumberNotifyStatusChange::NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aTelNum, RCall::TStatus aDesiredStatus)
	{
	CCallNumberNotifyStatusChange* self = new(ELeave)CCallNumberNotifyStatusChange(aEtel, aLine, aDesiredStatus);
	CleanupStack::PushL(self);
	self->ConstructL(aTelNum);
	CleanupStack::Pop(self);
	return self;
	}
	
CCallNumberNotifyStatusChange::~CCallNumberNotifyStatusChange()
	{
	LOGLINE1(_L("CCallNumberNotifyStatusChange::~CCallNumberNotifyStatusChange"));
	delete iNotifyStatusChangeHlprObj;
	
	Cancel();
	}

void CCallNumberNotifyStatusChange::ConstructL(const TDesC8& aTelNum)
	{
	LOGLINE1(_L(">>CCallNumberNotifyStatusChange::ConstructL"));
	CActiveScheduler::Add(this);
	
	__ASSERT_DEBUG(aTelNum.Length() < KMaxTelNumberSize,User::Panic(KPanicMsgFormat, KTelNumberTooBig));
	iTelNumber.Copy(aTelNum);
	
	// Create helper rqst object CCallNotifyStatusChange
	iNotifyStatusChangeHlprObj = CCallNotifyStatusChange::NewL(iEtel.Call(iLine), iDesiredStatus, *this);
	LOGLINE1(_L("<<CCallNumberNotifyStatusChange::ConstructL"));
	}

CCallNumberNotifyStatusChange::CCallNumberNotifyStatusChange(CEtelMngr& aEtel, TEtelLine aLine, RCall::TStatus aDesiredStatus)
 : CReqstObjectBase(aEtel, aLine), iDesiredStatus(aDesiredStatus)
	{
	}

void CCallNumberNotifyStatusChange::DoCancel()
	{
	LOGLINE1(_L(">>CCallNumberNotifyStatusChange::DoCancel"));
	// Cancel any uncompleted asynch reqsts to Etel
	iEtel.Call(iLine).DialCancel();
	LOGLINE1(_L("<<CCallNumberNotifyStatusChange::DoCancel"));
	}
	
void CCallNumberNotifyStatusChange::StartL()
	{
	LOGLINE1(_L(">>CCallNumberNotifyStatusChange::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	
	// Start the CCallNotifyStatusChange helper object
	iNotifyStatusChangeHlprObj->StartL();
	
	iEtel.Call(iLine).Dial(iStatus , iTelNumber);
	
	SetActive();
	LOGLINE1(_L("<<CCallNumberNotifyStatusChange::StartL"));
	}

void CCallNumberNotifyStatusChange::RunL()
	{
	// We get here when the Dial completes (when the call gets Connected).
	// This may happen some time after the call's status change that we are interested in has
	// occurred. Or it may happen before the status change we are interested in occurs.
	// Either way, it is the helper object that controls the completion of this operation by calling
	// HlprObjAborted() or HlprObjCompletedL().
	// This method does NOTHING on purpose and intentionally overrides the base class's default.
	}
	
void CCallNumberNotifyStatusChange::HlprObjAborted()
	{
	LOGLINE1(_L(">>CCallNumberNotifyStatusChange::HlprObjAborted"));
	Abort();
	LOGLINE1(_L("<<CCallNumberNotifyStatusChange::HlprObjAborted"));
	}
	
void CCallNumberNotifyStatusChange::HlprObjCompletedL()
	{
	LOGLINE1(_L(">>CCallNumberNotifyStatusChange::HlprObjCompletedL"));
	NotifyL();
	LOGLINE1(_L("<<CCallNumberNotifyStatusChange::HlprObjCompletedL"));
	}


	
//---------------
// CCallNotifyStatusChange
// This is a helper object and is meant to be used in collaboration with another rqst object,
// for example CCallNumberNotifyStatusChange, in order to sense the call's status change of a Dial operation.
//---------------

CCallNotifyStatusChange* CCallNotifyStatusChange::NewL(RMobileCall& aCall, RCall::TStatus aDesiredStatus, MCallNotifyObserver& aRqstObj)
	{
	CCallNotifyStatusChange* self = new(ELeave)CCallNotifyStatusChange(aCall, aDesiredStatus, aRqstObj);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CCallNotifyStatusChange::~CCallNotifyStatusChange()
	{
	Cancel();
	}
	
void CCallNotifyStatusChange::ConstructL()
	{
	LOGLINE1(_L(">>CCallNotifyStatusChange::ConstructL"));
	CActiveScheduler::Add(this);
	LOGLINE1(_L("<<CCallNotifyStatusChange::ConstructL"));
	}
	
CCallNotifyStatusChange::CCallNotifyStatusChange(RMobileCall& aCall, RCall::TStatus aDesiredStatus, MCallNotifyObserver& aRqstObj)
 : CActive(EPriorityStandard), iDesiredStatus(aDesiredStatus), iSession(aCall), iRqstObj(aRqstObj)
	{
	}
		
void CCallNotifyStatusChange::DoCancel()
	{
	LOGLINE1(_L(">>CCallNotifyStatusChange::DoCancel"));
	// cancel any uncompleted asynch reqsts to Etel
	iSession.NotifyStatusChangeCancel();  
	LOGLINE1(_L("<<CCallNotifyStatusChange::DoCancel"));
	}	

void CCallNotifyStatusChange::StartL()
	{
	LOGLINE1(_L(">>CCallNotifyStatusChange::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	
	iSession.NotifyStatusChange(iStatus, iCallStatus);
	SetActive();
	LOGLINE1(_L("<<CCallNotifyStatusChange::StartL"));
	}

void CCallNotifyStatusChange::RunL()
	{	
	LOGLINE1(_L(">>CCallNotifyStatusChange::RunL"));
	
	// The line below should not normally be required, but is a fix.
	// This RunL gets called every time a change is status is sensed on the call object (iSession).
	// However, it was found for some reason that when it is called, the iCallStatus variable did not always
	// reflect the true status of the call IN SOME CIRCUMSTANCES. Hence we are explicitly updating iCallStatus
	// with the real-time status of the call object. (Situations where the problem occured are when a test
	// invoked request-incoming-hangup-before-connect then followed by request-incoming-hangup-after-connect).
	iSession.GetStatus(iCallStatus);
	
	if(iCallStatus == iDesiredStatus)
		{
		// Reached the desired call state so we're done. 
		iRqstObj.HlprObjCompletedL();
		}
	else if(iCallStatus > iDesiredStatus)
		{
		// If call's status has gone beyond the desired status we have missed our window.
		// Need to abort the entire mission.
		Cancel();
		iRqstObj.HlprObjAborted();
		}
	else
		{
		// Look for next status change
		if(IsActive())
			User::Leave(KErrGeneral);
		
		// Keep notifying for next staus change
		iSession.NotifyStatusChange(iStatus, iCallStatus);
		SetActive();
		}
	LOGLINE1(_L("<<CCallNotifyStatusChange::RunL"));
	}
	
//---------------
// CLineNotifyStatusChange
// This is a helper object and is meant to be used in collaboration with another rqst object,
// for example CHangupNextCallBeforeConnect, in order to sense the line's status change during an operation.
//
//NOTE: This class is basically the same as CCallNotifyStatusChange and therefore a template class
//should be used to combine both together
//---------------


CLineNotifyStatusChange* CLineNotifyStatusChange::NewL(RMobileLine& aCall, RCall::TStatus aDesiredStatus, MCallNotifyObserver& aRqstObj)
	{
	CLineNotifyStatusChange* self = new(ELeave)CLineNotifyStatusChange(aCall, aDesiredStatus, aRqstObj);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CLineNotifyStatusChange::~CLineNotifyStatusChange()
	{
	Cancel();
	}
	
void CLineNotifyStatusChange::ConstructL()
	{
	LOGLINE1(_L(">>CLineNotifyStatusChange::ConstructL"));
	CActiveScheduler::Add(this);
	LOGLINE1(_L("<<CLineNotifyStatusChange::ConstructL"));
	}
	
CLineNotifyStatusChange::CLineNotifyStatusChange(RMobileLine& aCall, RCall::TStatus aDesiredStatus, MCallNotifyObserver& aRqstObj)
 : CActive(EPriorityStandard), iDesiredStatus(aDesiredStatus), iSession(aCall), iRqstObj(aRqstObj)
	{
	}
		
void CLineNotifyStatusChange::DoCancel()
	{
	LOGLINE1(_L(">>CLineNotifyStatusChange::DoCancel"));
	// cancel any uncompleted asynch reqsts to Etel
	iSession.NotifyStatusChangeCancel();  
	LOGLINE1(_L("<<CLineNotifyStatusChange::DoCancel"));
	}	

void CLineNotifyStatusChange::StartL()
	{
	LOGLINE1(_L(">>CLineNotifyStatusChange::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	
	iSession.NotifyStatusChange(iStatus, iCallStatus);
	SetActive();
	LOGLINE1(_L("<<CLineNotifyStatusChange::StartL"));
	}

void CLineNotifyStatusChange::RunL()
	{	
	LOGLINE1(_L(">>CLineNotifyStatusChange::RunL"));
	if(iCallStatus == iDesiredStatus)
		{
		// Reached the desired call state so we're done. 
		iRqstObj.HlprObjCompletedL();
		}
	else if(iCallStatus > iDesiredStatus)
		{
		// If call's status has gone beyond the desired status we have missed our window.
		// Need to abort the entire mission.
		Cancel();
		iRqstObj.HlprObjAborted();
		}
	else
		{
		// Look for next status change
		if(IsActive())
			User::Leave(KErrGeneral);
		
		// Keep notifying for next staus change
		iSession.NotifyStatusChange(iStatus, iCallStatus);
		SetActive();
		}
	LOGLINE1(_L("<<CLineNotifyStatusChange::RunL"));
	}
		

//---------------
// CHangUp
//---------------
CHangUp* CHangUp::NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aDelay)
	{
	CHangUp* self = new(ELeave)CHangUp(aEtel, aLine);
	CleanupStack::PushL(self);
	self->ConstructL(aDelay );
	CleanupStack::Pop(self);
	return self;
	}
	
CHangUp::~CHangUp()
	{
	Cancel();
	}

void CHangUp::ConstructL(const TDesC8& aDelay)
	{
	LOGLINE1(_L(">>CHangUp::ConstructL"));
	CActiveScheduler::Add(this);
	
	TInt delay(0);
	TLex8 lex(aDelay);
	lex.Val(delay);
	iDelay = TTimeIntervalSeconds(delay);
	LOGLINE1(_L("<<CHangUp::ConstructL"));
	}

CHangUp::CHangUp(CEtelMngr& aEtel, TEtelLine aLine)
 : CReqstObjectBase(aEtel, aLine)
	{
	}

void CHangUp::DoCancel()
	{
	LOGLINE1(_L(">>CHangUp::DoCancel"));
	// cancel any uncompleted asynch reqsts to Etel
	iEtel.Call(iLine).HangUpCancel();
	NotifyL();
	LOGLINE1(_L("<<CHangUp::DoCancel"));
	}
	
void CHangUp::StartL()
	{
	LOGLINE1(_L(">>CHangUp::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	
	// Implement the delay - this blocks!!
	RTimer timer;
	TRequestStatus timerRequest;
	timer.CreateLocal();
	timer.After(timerRequest, TTimeIntervalMicroSeconds32(iDelay.Int()*1000000)); // iDelay is in seconds
	User::WaitForRequest(timerRequest);
	timer.Close();
	
	// Issue request to Etel
	// Assuming we are always using Call1...
	RCall::TStatus status;
	iEtel.Call(iLine).GetStatus(status);
	if(status != RCall::EStatusIdle && status != RCall::EStatusHangingUp && status != RCall::EStatusUnknown)
		{
		iEtel.Call(iLine).HangUp(iStatus);
		SetActive();
		}
	else
		{
		// Signal to observer that this rqst object is done.
		NotifyL();
		}
	LOGLINE1(_L("<<CHangUp::StartL"));
	}
	

//---------------
// CHold
//---------------
CHold* CHold::NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aDelay)
	{
	CHold* self = new(ELeave)CHold(aEtel, aLine);
	CleanupStack::PushL(self);
	self->ConstructL(aDelay );
	CleanupStack::Pop(self);
	return self;
	}
	
CHold::~CHold()
	{
	Cancel();
	}

void CHold::ConstructL(const TDesC8& aDelay)
	{
	LOGLINE1(_L(">>CHold::ConstructL"));
	CActiveScheduler::Add(this);
	
	TInt delay(0);
	TLex8 lex(aDelay);
	lex.Val(delay);
	iDelay = TTimeIntervalSeconds(delay);
	LOGLINE1(_L("<<CHold::ConstructL"));
	}

CHold::CHold(CEtelMngr& aEtel, TEtelLine aLine)
 : CReqstObjectBase(aEtel, aLine)
	{
	}

void CHold::DoCancel()
	{
	LOGLINE1(_L(">>CHold::DoCancel"));
	// cancel any uncompleted asynch reqsts to Etel
	iEtel.Call(iLine).CancelAsyncRequest(EMobileCallHold);
	NotifyL();
	LOGLINE1(_L("<<CHold::DoCancel"));
	}
	
void CHold::StartL()
	{
	LOGLINE1(_L(">>CHold::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	
	// Implement the delay - this blocks!!
	RTimer timer;
	TRequestStatus timerRequest;
	timer.CreateLocal();
	timer.After(timerRequest, TTimeIntervalMicroSeconds32(iDelay.Int()*1000000)); // iDelay is in seconds
	User::WaitForRequest(timerRequest);
	timer.Close();
	
	// Issue request to Etel
	// Assuming we are always using Call1...
	RMobileCall::TMobileCallControlCaps rqrdCaps(RMobileCall::KCapsHold);
	if(iEtel.CheckCallCapabilities(iEtel.Call(iLine), rqrdCaps) == KErrNone)
		{
		iEtel.Call(iLine).Hold(iStatus);
		SetActive();
		}
	else
		{
		// Signal to observer that this rqst object is done.
		NotifyL();
		}
	LOGLINE1(_L("<<CHold::StartL"));
	}
	
#ifdef TEST_ONLY
// Check call is actually on hold
void CHold::RunL()
	{
	// check status
	RMobileCall::TMobileCallStatus callStatus;
	iEtel.VoiceCall1().GetMobileCallStatus(callStatus);
	TInt check_callStatus(0);
	
	NotifyL();
	}
#endif


//---------------
// CResume
//---------------
CResume* CResume::NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aDelay)
	{
	CResume* self = new(ELeave)CResume(aEtel, aLine);
	CleanupStack::PushL(self);
	self->ConstructL(aDelay );
	CleanupStack::Pop(self);
	return self;
	}
	
CResume::~CResume()
	{
	Cancel();
	}

void CResume::ConstructL(const TDesC8& aDelay)
	{
	LOGLINE1(_L(">>CResume::ConstructL"));
	CActiveScheduler::Add(this);
	
	TInt delay(0);
	TLex8 lex(aDelay);
	lex.Val(delay);
	iDelay = TTimeIntervalSeconds(delay);
	LOGLINE1(_L("<<CResume::ConstructL"));
	}

CResume::CResume(CEtelMngr& aEtel, TEtelLine aLine)
 : CReqstObjectBase(aEtel, aLine)
	{
	}

void CResume::DoCancel()
	{
	LOGLINE1(_L(">>CResume::DoCancel"));
	// cancel any uncompleted asynch reqsts to Etel
	iEtel.Call(iLine).CancelAsyncRequest(EMobileCallResume);
	NotifyL();
	LOGLINE1(_L("<<CResume::DoCancel"));
	}
	
void CResume::StartL()
	{
	LOGLINE1(_L(">>CResume::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	
	// Implement the delay - this blocks!!
	RTimer timer;
	TRequestStatus timerRequest;
	timer.CreateLocal();
	timer.After(timerRequest, TTimeIntervalMicroSeconds32(iDelay.Int()*1000000)); // iDelay is in seconds
	User::WaitForRequest(timerRequest);
	timer.Close();
	
	// Issue request to Etel
	// Assuming we are always using Call1...
	RMobileCall::TMobileCallControlCaps rqrdCaps(RMobileCall::KCapsResume);
	if(iEtel.CheckCallCapabilities(iEtel.Call(iLine), rqrdCaps) == KErrNone)
		{
		iEtel.Call(iLine).Resume(iStatus);
		SetActive();
		}
	else
		{
		// Signal to observer that this rqst object is done.
		NotifyL();
		}
	LOGLINE1(_L("<<CResume::StartL"));
	}
	
#ifdef TEST_ONLY
// Check call is actually resumed
void CResume::RunL()
	{
	// check status
	RMobileCall::TMobileCallStatus callStatus;
	iEtel.VoiceCall1().GetMobileCallStatus(callStatus);
	TInt check_callStatus(0);
	
	NotifyL();
	}
#endif
	
	
//---------------
// CDialCancel
//---------------
CDialCancel* CDialCancel::NewL(CEtelMngr& aEtel, TEtelLine aLine, const TDesC8& aDelay)
	{
	CDialCancel* self = new(ELeave)CDialCancel(aEtel, aLine);
	CleanupStack::PushL(self);
	self->ConstructL(aDelay );
	CleanupStack::Pop(self);
	return self;
	}
	
CDialCancel::~CDialCancel()
	{
	Cancel();
	iTimer.Close();
	}

void CDialCancel::ConstructL(const TDesC8& aDelay)
	{
	LOGLINE1(_L(">>CDialCancel::ConstructL"));
	CActiveScheduler::Add(this);
	
	TInt delay(0);
	TLex8 lex(aDelay);
	lex.Val(delay);
	iDelay = TTimeIntervalSeconds(delay);
	LOGLINE1(_L("<<CDialCancel::ConstructL"));
	}

CDialCancel::CDialCancel(CEtelMngr& aEtel, TEtelLine aLine)
 : CReqstObjectBase(aEtel, aLine)
	{
	}

void CDialCancel::DoCancel()
	{
	LOGLINE1(_L(">>CDialCancel::DoCancel"));
	// cancel any uncompleted asynch reqsts to Etel
	iTimer.Cancel();
	NotifyL();
	LOGLINE1(_L("<<CDialCancel::DoCancel"));
	}
	
void CDialCancel::StartL()
	{
	LOGLINE1(_L(">>CDialCancel::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	
	// Call must be in Dialing or Connecting state
	// Assuming we are always using Call1...
	RCall::TStatus status;
	iEtel.Call(iLine).GetStatus(status);
	if(status == RCall::EStatusDialling || status == RCall::EStatusConnecting)
		{
		// Perform required delay
		iTimer.CreateLocal();
		iTimer.After(iStatus, TTimeIntervalMicroSeconds32(iDelay.Int()*1000000)); // iDelay is in seconds
		SetActive();
		}
	else
		{
		// Signal to observer that this rqst object is done.
		NotifyL();
		}
	LOGLINE1(_L("<<CDialCancel::StartL"));
	}
	
void CDialCancel::RunL()
	{
	LOGLINE1(_L(">>CDialCancel::RunL"));
	iEtel.Call(iLine).DialCancel();
	NotifyL();
	LOGLINE1(_L("<<CDialCancel::RunL"));
	}

//---------------
// CAnswerNextCall
//---------------	
	
CAnswerNextCall* CAnswerNextCall::NewL(CEtelMngr& aEtel, TEtelLine aLine)
	{
	CAnswerNextCall* self = new(ELeave)CAnswerNextCall(aEtel, aLine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CAnswerNextCall::~CAnswerNextCall()
	{	
	Cancel();
	}

void CAnswerNextCall::ConstructL()
	{
	LOGLINE1(_L("CAnswerNextCall::ConstructL"));
	CActiveScheduler::Add(this);
	}

CAnswerNextCall::CAnswerNextCall(CEtelMngr& aEtel, TEtelLine aLine)
 : CReqstObjectBase(aEtel, aLine)
	{
	}

void CAnswerNextCall::DoCancel()
	{
	LOGLINE1(_L(">>CAnswerNextCall::DoCancel"));
	// cancel any uncompleted asynch reqsts to Etel
	iEtel.Call(iLine).AnswerIncomingCallCancel();
	NotifyL();
	LOGLINE1(_L("<<CAnswerNextCall::DoCancel"));
	}
	
void CAnswerNextCall::StartL()
	{
	LOGLINE1(_L(">>CAnswerNextCall::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	iEtel.Call(iLine).AnswerIncomingCall(iStatus);
	SetActive();
	LOGLINE1(_L("<<CAnswerNextCall::StartL"));
	}

//---------------
// CLineNotifyStatusChangeWrapper - wraps up the CCallNotifyStatusChange as a CReqstObjectBase
//---------------
CLineNotifyStatusChangeWrapper* CLineNotifyStatusChangeWrapper::NewL(CEtelMngr& aEtel, TEtelLine aLine, RCall::TStatus aDesiredStatus)
	{
	CLineNotifyStatusChangeWrapper* self = new(ELeave)CLineNotifyStatusChangeWrapper(aEtel, aLine, aDesiredStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CLineNotifyStatusChangeWrapper::~CLineNotifyStatusChangeWrapper()
	{
	delete iNotifyStatusChangeHlprObj;
	
	Cancel();
	}

void CLineNotifyStatusChangeWrapper::ConstructL()
	{
	LOGLINE1(_L("CLineNotifyStatusChangeWrapper::ConstructL"));
	//this is deliberatley not set
	//	CActiveScheduler::Add(this);
	

	
	// Create helper rqst object CCallNotifyStatusChange
	iNotifyStatusChangeHlprObj = CLineNotifyStatusChange::NewL(iEtel.Line(iLine), iDesiredStatus, *this);
	
	}

CLineNotifyStatusChangeWrapper::CLineNotifyStatusChangeWrapper(CEtelMngr& aEtel, TEtelLine aLine, RCall::TStatus aDesiredStatus)
 : CReqstObjectBase(aEtel, aLine), iDesiredStatus(aDesiredStatus)
	{
	}

void CLineNotifyStatusChangeWrapper::DoCancel()
	{
	// Cancel any uncompleted asynch reqsts to Etel
	//nothing to do here as no async requests to cancel
	}
	
void CLineNotifyStatusChangeWrapper::StartL()
	{
	LOGLINE1(_L(">>CLineNotifyStatusChangeWrapper::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	
	// Start the CCallNotifyStatusChange helper object
	iNotifyStatusChangeHlprObj->StartL();
	
	//NOTE: This is deliberately not set active as we do not actually invoke any async request 
	//on this object
	//SetActive();
	LOGLINE1(_L("<<CLineNotifyStatusChangeWrapper::StartL"));
	}

void CLineNotifyStatusChangeWrapper::RunL()
	{
	//should never get here
	}
	
void CLineNotifyStatusChangeWrapper::HlprObjAborted()
	{
	LOGLINE1(_L(">>CLineNotifyStatusChangeWrapper::HlprObjAborted"));
	Abort();
	LOGLINE1(_L("<<CLineNotifyStatusChangeWrapper::HlprObjAborted"));
	}
	
void CLineNotifyStatusChangeWrapper::HlprObjCompletedL()
	{
	LOGLINE1(_L(">>CLineNotifyStatusChangeWrapper::HlprObjCompletedL"));
	NotifyL();
	LOGLINE1(_L("<<CLineNotifyStatusChangeWrapper::HlprObjCompletedL"));
	}
	

//---------------
// CSendDataBackOnDataPort
//---------------
CSendDataBackOnDataPort* CSendDataBackOnDataPort::NewL(CEtelMngr& aEtel, const TDesC8& aDataToSend)
	{
	CSendDataBackOnDataPort* self = new(ELeave)CSendDataBackOnDataPort(aEtel);
	CleanupStack::PushL(self);
	self->ConstructL(aDataToSend);
	CleanupStack::Pop(self);
	return self;
	}
	
CSendDataBackOnDataPort::~CSendDataBackOnDataPort()
	{
	iDataToSend.Close();
	Cancel();				// for CActive
	}

void CSendDataBackOnDataPort::ConstructL(const TDesC8& aDataToSend)
	{
	LOGLINE1(_L("CSendDataBackOnDataPort::ConstructL"));
	CActiveScheduler::Add(this);
	iDataToSend.CreateL(aDataToSend);
	}

CSendDataBackOnDataPort::CSendDataBackOnDataPort(CEtelMngr& aEtel)
 : CReqstObjectBase(aEtel, EDataLine)
	{
	}

void CSendDataBackOnDataPort::DoCancel()
	{
	LOGLINE1(_L(">>CSendDataBackOnDataPort::DoCancel"));
	// cancel any uncompleted asynch reqsts to Etel
	LOGLINE1(_L("<<CSendDataBackOnDataPort::DoCancel"));
	}
	
void CSendDataBackOnDataPort::StartL()
	{
	LOGLINE1(_L(">>CSendDataBackOnDataPort::StartL"));
	if(IsActive())
		User::Leave(KErrGeneral);
	
	TInt ret = iEtel.SendDataL(iDataToSend);

	NotifyL();
	LOGLINE1(_L("<<CSendDataBackOnDataPort::StartL"));
	}
