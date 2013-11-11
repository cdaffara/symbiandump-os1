// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The class representing CPositioner and assosiated request.
// 
//

#include <test/testexecutestepbase.h>

#include "te_lbspsybasestep.h"
#include "te_lbspsypositioner.h"
#include "EPos_PositionerConstructParams.h"
#include "te_lbspsyresponse.h"
#include "cadaptationpositioner.h"
#include "te_lbspsystaticdata.h"


CTe_PsyPositioner* CTe_PsyPositioner::NewLC(const TUid& aUid, MTe_ActiveObserver& aObserver,
		CTestStep& aTestStep)
	{
	CTe_PsyPositioner* self = new(ELeave) CTe_PsyPositioner(aObserver, aTestStep);
	CleanupStack::PushL(self);
	self->ConstructL(aUid);
	return self;
	}

CTe_PsyPositioner::CTe_PsyPositioner(MTe_ActiveObserver& aObserver, CTestStep& aTestStep) 
: CTe_Active(aObserver), iTestStep(aTestStep), iLogger(iTestStep.Logger())
	{
	iPositionerId = MTe_LbsPsyStaticData::Static().CountPositioner();
	}

void CTe_PsyPositioner::ConstructL(const TUid& aUid)
	{
	TPositionerConstructParams params;
	params.iImplementationUid = aUid;
	params.iParamObserver = this;
	params.iStatusObserver = this;

	iPositioner = CPositioner::NewL(&params);
	
	TPtrC ptr;
	User::LeaveIfError(iTestStep.GetStringFromConfig(iTestStep.ConfigSection(), KPosInfo, ptr));
	
	iPosInfoType = EPosInfo;
	
	if(ptr == KCourseInfo)
		{
		iPosInfoType = ECourseInfo;
		}
	else if(ptr == KSatInfo)	
		{
		iPosInfoType = ESatInfo;
		}
	else if(ptr == KGenericInfo)
		{
		iPosInfoType = EGenericInfo;
		iGenericInfo = HPositionGenericInfo::NewL();
		}
	}


CTe_PsyPositioner::~CTe_PsyPositioner()
	{
    INFO_PRINTF1(_L("CTe_PsyPositioner::~CTe_PsyPositioner()"));
	Cancel();
	delete iGenericInfo;
    INFO_PRINTF1(_L("->CPositioner::~CPositioner()"));
	delete iPositioner; 
	}
		
EXPORT_C void CTe_PsyPositioner::NotifyPositionUpdate()
	{
	iPositioner->NotifyPositionUpdate(PosInfo(), iStatus);
	SetActive();
	}

EXPORT_C void CTe_PsyPositioner::CancelNotifyPositionUpdate()
	{
	iPositioner->CancelNotifyPositionUpdate();
	}

void CTe_PsyPositioner::RunL()
	{
    CTe_Active::RunL();
    iPosUpdateReceived = ETrue; // Note this just means the the original request got completed and not necessarily a valid pos update has been received.
    if (iStatus.Int() == KErrNone)
        {
        INFO_PRINTF2(_L("A position update has been received successfully for positioner %d"),iPositionerId);
        }
    else
        {
        INFO_PRINTF3(_L("The position update request for positioner %d has been completed with the error %d"), iPositionerId, iStatus.Int());
        }
	}

void CTe_PsyPositioner::DoCancel()
	{
	iPositioner->CancelNotifyPositionUpdate();
	}

TInt CTe_PsyPositioner::GetRequiredPositionQuality(TPositionQuality& /*aPositionQuality*/) const
	{
	__ASSERT_ALWAYS(EFalse, User::Invariant());
	return KErrNone;
	}

void CTe_PsyPositioner::GetMaxAge(TTime& aMaxAge) const
	{
	aMaxAge = iMaxAge;
	}

TBool CTe_PsyPositioner::IsPartialUpdateAllowed() const
	{
	return iAcceptPartialUpdates;
	}

void CTe_PsyPositioner::GetUpdateTimeOut(TTimeIntervalMicroSeconds& aUpdateTimeOut) const
	{
	if(iUpdateTimeout.Int64())
		{
		aUpdateTimeOut = iUpdateTimeout;
		}
	else
		{
		aUpdateTimeOut = iPsyDefaultUpdateTimeout;
		}
	}

void CTe_PsyPositioner::SetPsyDefaultUpdateTimeOut(const TTimeIntervalMicroSeconds& aUpdateTimeOut)
	{
	iPsyDefaultUpdateTimeout = aUpdateTimeOut;
	}

void CTe_PsyPositioner::ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& /*aAdditionalTime*/)
	{
	__ASSERT_ALWAYS(EFalse, User::Invariant());
	}

void CTe_PsyPositioner::ReportStatus(const TPositionModuleId& /*aImplementationUid*/, 
			const TPositionModuleStatus& aStatus)
	{
	iModStatusUpdated = ETrue;
	iModStatus = aStatus;
	}

EXPORT_C void CTe_PsyPositioner::CheckPosUpdateReceivedL(const CTe_PsyResponse& aLbsResponse)
	{
	if(!iPosUpdateReceived)
		{
		ERR_PRINTF2(_L("The expected position update is not received for positioner %d"), iPositionerId);
		User::Leave(EFail);
		}
	iPosUpdateReceived = EFalse;
	
    if ( (iStatus.Int() == KErrNone) || (iStatus.Int() == KPositionPartialUpdate) )
        {
        if (!aLbsResponse.Compare(PosInfo()))
            {
            ERR_PRINTF2(_L("The position update received is not equal to the sent one for positioner"),
                    iPositionerId);
            User::Leave(EFail);
            }
        INFO_PRINTF2(_L("The expected position update is received for positioner %d"), iPositionerId);
        }
    else
        {
        ERR_PRINTF3(_L("The position update request for positioner %d has been completed with error %d"), iPositionerId, iStatus.Int());
        User::Leave(EFail);
        }
	}

EXPORT_C void CTe_PsyPositioner::CheckPosUpdateNotReceivedL()
	{
	if(iPosUpdateReceived)
		{
		ERR_PRINTF2(_L("An unexpected position update is received for positioner %d"), iPositionerId);
		User::Leave(EFail);
		}
	iPosUpdateReceived = EFalse;
	INFO_PRINTF2(_L("A position update is not received as expected for positioner %d"), iPositionerId);
	}

EXPORT_C void CTe_PsyPositioner::CheckPosUpdateFailedL(TInt aExpectedError)
	{
	if(!iPosUpdateReceived)
		{
		ERR_PRINTF2(_L("The expected position update is not received for positioner %d"), iPositionerId);
		User::Leave(EFail);
		}
	iPosUpdateReceived = EFalse;
	if(iStatus!=aExpectedError)
		{
		ERR_PRINTF4(_L("The position update received has wrong status=%d while expected error=%d for positioner %d"),
				iStatus.Int(), aExpectedError, iPositionerId);
		User::Leave(EFail);
		}
	
	INFO_PRINTF3(_L("The position update is received with the status=%d as expected for positioner %d"), 
			iStatus.Int(), iPositionerId);
	}

void CTe_PsyPositioner::CheckModuleStatusReceivedL(const CTe_PsyResponse& aLbsResponse)
	{
	if(!iModStatusUpdated)
		{
		ERR_PRINTF2(_L("The expected module status is not received for positioner %d"), iPositionerId);
		User::Leave(EFail);
		}
	
	iModStatusUpdated = EFalse;
	
	if(aLbsResponse.PositionModuleStatus().DataQualityStatus() !=iModStatus.DataQualityStatus() ||
			aLbsResponse.PositionModuleStatus().DeviceStatus() !=iModStatus.DeviceStatus())
		{
		ERR_PRINTF2(_L("The module status received is not equal to the sent one for positioner"), 
						iPositionerId);
		User::Leave(EFail);
		}
	INFO_PRINTF2(_L("The expected module status is received for positioner %d"), iPositionerId);
	}


void CTe_PsyPositioner::CheckModuleStatusNotReceivedL()
	{
	if(iModStatusUpdated)
		{
		ERR_PRINTF2(_L("The unexpected module status is received for positioner %d"), iPositionerId);
		User::Leave(EFail);
		}
	}

EXPORT_C void CTe_PsyPositioner::SimulateTimerErrorsL()
	{
	CAdaptationPositioner* adaptPos = static_cast<CAdaptationPositioner*>(iPositioner);
	User::LeaveIfError(adaptPos->OnTimerError(KErrGeneral, CAdaptationPositioner::EMaxFixTimerEvent));
	User::LeaveIfError(adaptPos->OnTimerError(KErrGeneral, CAdaptationPositioner::EWarmDownTimerEvent));
	}

TBool CTe_PsyPositioner::TrackingOverridden() const
	{
	return iPositioner->TrackingOverridden();
	}

void CTe_PsyPositioner::StartTrackingL(const TTimeIntervalMicroSeconds& aInterval)
	{
	iPositioner->StartTrackingL(aInterval);
	}

void CTe_PsyPositioner::StopTracking()
	{
	iPositioner->StopTracking();
	}

TPositionInfo& CTe_PsyPositioner::PosInfo()
	{
	switch(iPosInfoType)
		{
		case ECourseInfo:
			return iCourseInfo;
		case ESatInfo:
			return iSatInfo;
		case EPosInfo:	
			return iPosInfo;
		case EGenericInfo:
			return *iGenericInfo;
		}
	return iPosInfo;
	}

