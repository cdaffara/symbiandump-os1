// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "epos_cpospsyrequester.h"
//#include "EPos_CPosPsyVerifier.h"
#include "epos_cpospsyresult.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYRequester::CPosPSYRequester() : 
	CActive(EPriorityStandard),
	iQueryIdStrategy(ETrue),
	iQueryIdRequested(EFalse),
	iPartialUpdate(EFalse),
    iCheckStatus(EFalse),
	iStatusReported(EFalse),
    iPosInfo(NULL),
	iMaxAgeTime(0),
	iMaxAgeRequested(EFalse),
    iParent(NULL)
    {
	CActiveScheduler::Add(this);
	}

// EPOC default constructor can leave.
void CPosPSYRequester::ConstructL(MPosRequestObserver* aParent)
    {
    iParent = aParent;
    }

// Two-phased constructor.
CPosPSYRequester* CPosPSYRequester::NewL(MPosRequestObserver* aParent)
    {
    CPosPSYRequester* self = new (ELeave) CPosPSYRequester;
    CleanupStack::PushL(self);
    self->ConstructL(aParent);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYRequester::~CPosPSYRequester()
    {
	ClosePositioner();
    }

// ---------------------------------------------------------
// CPosPSYRequester::OpenPositionerL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::OpenPositionerL(
    const TUid aImplementationUid,
	TBool aCheckStatus)
    {
	iCheckStatus = aCheckStatus;
	if (!iPositioner) 
		{
		TPositionerConstructParams params;
    
		params.iImplementationUid = aImplementationUid;
		params.iParamObserver = this;
		params.iStatusObserver = this;
		TRAPD(err, iPositioner = CPositioner::NewL((TAny*)&params));
		
		// Log error
		if (err != KErrNone)
			{
			_LIT(KOpenError, "PSY leaved with %d at construction.");
			TBuf<KRequesterMessageBufSize> buf;
			buf.Format(KOpenError, err);
			iParent->AddTestResultL(buf, EErrorMessage);
			}
		// Report error back to test
		User::LeaveIfError(err);
		}
	}

// ---------------------------------------------------------
// CPosPSYRequester::MakeRequest
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::MakeRequest(
    TPositionInfo& aPosInfo)
	{
	iPosInfo = &aPosInfo;
    iStartTime.UniversalTime();
	if (iPositioner)
		{
		TInt psyTesterStatus = KRequestPending - 1;
		iStatus = psyTesterStatus;
		TRAPD(err, iPositioner->NotifyPositionUpdate(*iPosInfo, iStatus));
		// Log error
		if (err != KErrNone)
			{
			_LIT(KNotifyError, "NotifyPositionUpdate leaved with %d.");
			TBuf<KRequesterMessageBufSize> buf;
			buf.Format(KNotifyError, err);
			TInt err2 = KErrNone;
			TRAP(err2, iParent->AddTestResultL(buf, EErrorMessage));
			}
		// Leave without setting iStatus to pending or request pending
		if (err != KErrNone && 
			(iStatus == psyTesterStatus || iStatus == KRequestPending))
			{
			iParent->HandleRequestError(KErrCompletion);
			}
		else 
			{
			SetActive();		
			}
		}
	else 
		{
		iParent->HandleRequestError(KErrNotFound);
		}
	}

// ---------------------------------------------------------
// CPosPSYRequester::CancelRequest
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::CancelRequest()
	{
    if (iPositioner)
        {
        TRAPD(err, iPositioner->CancelNotifyPositionUpdate());
		// Log error
		if (err != KErrNone)
			{
			_LIT(KCancelError, "PSY leaved with %d when trying to cancel an ongoing location request.");
			TBuf<KRequesterMessageBufSize> buf;
			buf.Format(KCancelError, err);
			TRAP(err, iParent->AddTestResultL(buf, EErrorMessage));
			}
		}
	}

// ---------------------------------------------------------
// CPosPSYRequester::GetResult
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::GetResult(
	TInt& aStatus, 
	TTimeIntervalMicroSeconds& aRequestTime)
    {
    aStatus = iStatus.Int();
    aRequestTime = iRequestTime;
    }

// ---------------------------------------------------------
// CPosPSYRequester::ClosePositioner
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::ClosePositioner()
    {
	Cancel();
	TRAPD(err, delete iPositioner);
    iPositioner = NULL;
	
	// Log error
	if (err != KErrNone)
		{
		_LIT(KDeleteError, "PSY leaved with %d in destructor");
		TBuf<KRequesterMessageBufSize> buf;
		buf.Format(KDeleteError, err);
		TRAP(err, iParent->AddTestResultL(buf, EErrorMessage));
		}
	iCheckStatus = EFalse;
    }

// ---------------------------------------------------------
// CPosPSYRequester::SetMaxAge
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::SetMaxAge(
    const TTime aTime)
	{
	iMaxAgeTime = aTime;
	}

// ---------------------------------------------------------
// CPosPSYRequester::MaxAgeRequested
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYRequester::MaxAgeRequested() const 
	{
	return iMaxAgeRequested;
	}

// ---------------------------------------------------------
// CPosPSYRequester::SetMaxAgeRequested
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::SetMaxAgeRequested(TBool aBool)
	{
	iMaxAgeRequested = aBool;
	}

// ---------------------------------------------------------
// CPosPSYRequester::SetQueryId
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::SetQueryId(TBool aQueryId)
	{
	iQueryIdStrategy = aQueryId;
	}

// ---------------------------------------------------------
// CPosPSYRequester::QueryIdRequested
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYRequester::QueryIdRequested() const 
	{
	return iQueryIdRequested;
	}

// ---------------------------------------------------------
// CPosPSYRequester::SetQueryIdRequested
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::SetQueryIdRequested(TBool aBool)
	{
	iQueryIdRequested = aBool;
	}

// ---------------------------------------------------------
// CPosPSYRequester::SetPartialUpdateAllowed
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::SetPartialUpdateAllowed(
    const TBool aPartialUpdate)
	{
	iPartialUpdate = aPartialUpdate;
	}

// ---------------------------------------------------------
// CPosPSYRequester::CheckStatusL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::CheckStatus()
    {
    if (iModuleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceDisabled) //2
        {
        iStatusChangesDone |= EDisabledDone;
        }
    if (iModuleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceActive) //7
        {
        iStatusChangesDone |=  EActiveDone;
        }
    if (iModuleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceInitialising) //4
        {
        iStatusChangesDone |=  EInitialisingDone;
        }
    if (iModuleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceReady) //6
        {
        iStatusChangesDone |= EReadyDone;
        }
    if (iModuleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceInactive) //3
        {
        iStatusChangesDone |= EInactiveDone;
        }
    if (iModuleStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityUnknown) //0
        {
        iStatusChangesDone |= EQualityUnknown;
        }
    iStatusReported = ETrue;
    }

// ---------------------------------------------------------
// CPosPSYRequester::ClearStatusHistory
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::ClearStatusHistory()
	{
	iStatusReported = EFalse;
	iStatusChangesDone = 0;
	iModuleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceUnknown);
	iModuleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityUnknown);
	}

// ---------------------------------------------------------
// CPosPSYRequester::TrackingOverridden
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYRequester::TrackingOverridden()
	{
	TBool trackingOverridden = EFalse;
    TRAPD(err, trackingOverridden = iPositioner->TrackingOverridden());
	// Log error
	if (err != KErrNone)
		{
		_LIT(KTrackError, "PSY leaved with %d when checking if tracking is overridden.");
		TBuf<KRequesterMessageBufSize> buf;
		buf.Format(KTrackError, err);
		TRAP(err, iParent->AddTestResultL(buf, EErrorMessage));
		}
	return trackingOverridden;
	}

// ---------------------------------------------------------
// CPosPSYRequester::StartTrackingL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::StartTrackingL(const TTimeIntervalMicroSeconds& aInterval)
	{
    iPositioner->StartTrackingL(aInterval);
	}

// ---------------------------------------------------------
// CPosPSYRequester::StopTracking
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::StopTracking()
	{
    TRAPD(err, iPositioner->StopTracking());
	// Log error
	if (err != KErrNone)
		{
		_LIT(KStopTrackError, "PSY leaved with %d when stopping tracking session.");
		TBuf<KRequesterMessageBufSize> buf;
		buf.Format(KStopTrackError, err);
		TRAP(err, iParent->AddTestResultL(buf, EErrorMessage));
		}
	}

// ---------------------------------------------------------
// CPosPSYRequester::StatusChanges
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::StatusChanges(
    TInt& aChanges, 
    TBool& aStatusReported,
    TPositionModuleStatus& aModuleStatus)
    {
    aChanges = iStatusChangesDone;
    aStatusReported = iStatusReported;
    aModuleStatus = iModuleStatus;
    }

// ---------------------------------------------------------
// CPosPSYRequester::QueryId
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
//TInt CPosPSYRequester::QueryId() const
//	{
//	CPosPSYRequester* self = const_cast<CPosPSYRequester*> (this);
//	self->SetQueryIdRequested(ETrue); 
//
//	TInt queryId = CPosPSYVerifier::GetQueryId();
//	if (!iQueryIdStrategy) 
//		{
//		// Set query id to a false number
//		queryId += 20;
//		}
//	return queryId;
//	}

// ---------------------------------------------------------
// CPosPSYRequester::GetMaxAge
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::GetMaxAge(
    TTime& aMaxAge) const
	{
	CPosPSYRequester* self = const_cast<CPosPSYRequester*> (this);
	self->SetMaxAgeRequested(ETrue);
	aMaxAge = iMaxAgeTime;
	}

// ---------------------------------------------------------
// CPosPSYRequester::IsPartialUpdateAllowed
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosPSYRequester::IsPartialUpdateAllowed() const
	{
	return iPartialUpdate;
	}

// ---------------------------------------------------------
// CPosPSYRequester::GetRequiredPositionQuality
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYRequester::GetRequiredPositionQuality(
    TPositionQuality& /*aRequestedPosQuality*/) const
	{
	return 1;
	}

void CPosPSYRequester::GetUpdateTimeOut(TTimeIntervalMicroSeconds& /*aUpdateTimeOut*/) const
	{
	//Intentionally left blank
	}

void CPosPSYRequester::SetPsyDefaultUpdateTimeOut(const TTimeIntervalMicroSeconds& /*aUpdateTimeOut*/)
	{
	//Intentionally left blank
	}

void CPosPSYRequester::ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& /*aAdditionalTime*/)
	{
	//Intentionally left blank
	}

// ---------------------------------------------------------
// CPosPSYRequester::ReportStatus
// From MPosParameterObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::ReportStatus(
    const TPositionModuleId& /*aImplementationUid*/,
    const TPositionModuleStatus& aStatus)
	{
	if (iCheckStatus)
        {
        iModuleStatus = aStatus;
        CheckStatus();
        }
	}

// ---------------------------------------------------------
// CPosPSYRequester::RunL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::RunL()
    {
    TTime now;
    now.UniversalTime();
    iRequestTime = now.MicroSecondsFrom(iStartTime);    
	iParent->HandlePositionCompleteL(iStatus.Int(), iRequestTime);
	}

// ---------------------------------------------------------
// CPosPSYRequester::DoCancel
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYRequester::DoCancel()
    {
	CancelRequest();
	}

// ---------------------------------------------------------
// CPosPSYRequester::RunError
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPSYRequester::RunError(TInt aError)
    {
	iParent->HandleRequestError(aError);	
	return KErrNone;
	}

//  End of File
