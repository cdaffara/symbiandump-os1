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
// The class representing a PSY request to the LBS sub-system. 
// 
//

#include <test/testexecutestepbase.h>
#include <centralrepository.h>
#include <lbs/lbsnetcommon.h>
#include "te_lbspsyrequest.h"
#include "te_lbspsybasestep.h"
#include "lbsqualityprofile.h"
#include "te_lbspsystaticdata.h"
#include "te_lbspsyutils.h"

EXPORT_C CTe_PsyRequest* CTe_PsyRequest::NewLC(MTe_ActiveObserver& aObserver)
	{
	CTe_PsyRequest* self = new(ELeave) CTe_PsyRequest(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTe_PsyRequest::CTe_PsyRequest(MTe_ActiveObserver& aObserver) : 
CTe_Active(aObserver)
	{	
	}

void CTe_PsyRequest::ConstructL()
	{
	iPosUpdateReqs.OpenL(MTe_LbsPsyStaticData::Static().UpdateReqChanId());
	
	// Create the connection to LbsAdmin
	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);	

	// Get the profile Id for self locate requests
	TLbsQualityProfileId profileId;
	User::LeaveIfError(admin->Get(KLbsSettingQualityProfileSelfLocate, profileId));
	// Retrieve the quality profile that belongs to the given Id
	TQualityProfile quality;
	User::LeaveIfError(LbsQualityProfile::GetQualityProfileById(profileId, quality));
	iDefaultPosQuality.SetHorizontalAccuracy(quality.MinHorizontalAccuracy());
	iDefaultPosQuality.SetVerticalAccuracy(quality.MinVerticalAccuracy());
	iDefaultPosQuality.SetTimeToFirstFix(quality.MaxFixTime());
	iDefaultPosQuality.SetTimeToNextFix(quality.MaxFixTime());
	
	CleanupStack::PopAndDestroy(admin);	
	}

CTe_PsyRequest::~CTe_PsyRequest()
	{
	Cancel();
	iPosUpdateReqs.Close();
	}
	
EXPORT_C void CTe_PsyRequest::NotifyPsyRequest()
	{
	Cancel();
	
	iPosUpdateReqs.NotifyPositionUpdateRequestChange(iStatus);
	SetActive();
	}

// Update request, or status update or cancel received:
EXPORT_C void CTe_PsyRequest::CheckPosUpdateOrReqReceivedL()
	{
	if(!iWasUpdateOrRequestReceived)
		{
		TRACESTRING("CTe_PsyRequest::CheckPosUpdateOrReqReceivedL() nothing received");
		User::Leave(KErrNotFound);
		}
	iWasUpdateOrRequestReceived = EFalse;
	
	TLbsPositionUpdateRequestStatus& requestReceived = 
		static_cast<TLbsPositionUpdateRequestStatus&>(iRequestReceived);
		
		TLbsPositionUpdateRequestStatus& requestExpected = 
			static_cast<TLbsPositionUpdateRequestStatus&>(iRequestExpected);
	
	if(requestReceived.Type()!= requestExpected.Type())
		{
		TRACESTRING2("CTe_PsyRequest::CheckPosUpdateOrReqReceivedL() unexpected request received type = %d", iRequestReceived.Type());
		User::Leave(EFail);
		}
	
	if(iRequestReceived.Type() == TLbsPositionUpdateRequestBase::EUpdateRequest)
		{
		TLbsPositionUpdateRequest& requestReceived = static_cast<TLbsPositionUpdateRequest&>(iRequestReceived);
		// Exclude the method from comparison, just set it to whatever we received:
		UpdateRequest().RequestMethod() = requestReceived.RequestMethod();
		CheckPosUpdateReqReceivedWithMethodL();		
		}
	}

EXPORT_C void CTe_PsyRequest::CheckPosUpdateReqReceivedWithMethodL()
	{
	if(!iWasPosUpdRequestReceived)
		{
		User::Leave(KErrNotFound);
		}
	iWasUpdateOrRequestReceived = EFalse;
	iWasPosUpdRequestReceived = EFalse;
	
	if(iRequestReceived.Type()!= TLbsPositionUpdateRequestBase::EUpdateRequest)
		{
		TRACESTRING2("CTe_PsyRequest::CheckPosUpdateReqReceivedWithMethodL() unexpected request received type = %d", iRequestReceived.Type());
		User::Leave(EFail);
		}
	
		TLbsPositionUpdateRequest& requestReceived = 
			static_cast<TLbsPositionUpdateRequest&>(iRequestReceived);
		TLbsPositionUpdateRequest& requestExpected = UpdateRequest(); 
		
	TLbsNetPosMethodInt posMethodReceived, posMethodExpected;
	if(requestExpected.RequestMethod().NumPosMethods())	// in the case of the network psy there is no method set
		{
		User::LeaveIfError( requestReceived.RequestMethod().GetPosMethod(0, posMethodReceived)); // nb: LS only ever passes a single mode
		User::LeaveIfError( requestExpected.RequestMethod().GetPosMethod(0, posMethodExpected));
		}
	else
		{
		posMethodExpected.SetPosMethod(KLbsPositioningMeansGps, (TPositionModuleInfo::ETechnologyUnknown));
		posMethodReceived.SetPosMethod(KLbsPositioningMeansGps, (TPositionModuleInfo::ETechnologyUnknown));
		}
	//Exclude the target time, hybrid request from comparison
	//We need to do it as these values are updated by the positioner itself and are 
	//different from the update we publish on the bus.
		UpdateRequest().TargetTime() = requestReceived.TargetTime();
		UpdateRequest().HybridRequest() = requestReceived.HybridRequest();
		
		if(requestExpected.Tracking()!=requestReceived.Tracking() || 
		   requestExpected.RequestQuality().MinHorizontalAccuracy()!=
		   			requestReceived.RequestQuality().MinHorizontalAccuracy() ||
		   requestExpected.RequestQuality().MinVerticalAccuracy()!=
		   			requestReceived.RequestQuality().MinVerticalAccuracy() ||
		   requestExpected.RequestQuality().MaxFixTime()!=
		   			requestReceived.RequestQuality().MaxFixTime() ||
	   posMethodReceived.PosMode() != posMethodExpected.PosMode()	||  
	   requestExpected.NewClient()!= requestReceived.NewClient() ||  
	   requestExpected.PowerAdvice()!= requestReceived.PowerAdvice())
		{
		TRACESTRING("CTe_PsyRequest::CheckPosUpdateReqReceivedWithMethodL() expected != received");
		User::Leave(EFail);
		}
	}

EXPORT_C void CTe_PsyRequest::CheckPowerAdviceReceivedL()
	{
	if(!iWasUpdateOrRequestReceived)
		{
		TRACESTRING("CTe_PsyRequest::CheckPowerAdviceReceivedL() nothing received");
		User::Leave(KErrNotFound);
		}
	iWasUpdateOrRequestReceived = EFalse;
	
	if(iRequestReceived.Type() != TLbsPositionUpdateRequestBase::EStatus && iRequestReceived.Type() != TLbsPositionUpdateRequestBase::ECancel)
		{
		TRACESTRING2("CTe_PsyRequest::CheckPowerAdviceReceivedL() unexpected type %d", iRequestReceived.Type());
		User::Leave(EFail);	
		}
	
	TLbsPositionUpdateRequestStatus& requestReceived = 
		static_cast<TLbsPositionUpdateRequestStatus&>(iRequestReceived);
		
		TLbsPositionUpdateRequestStatus& requestExpected = 
			static_cast<TLbsPositionUpdateRequestStatus&>(iRequestExpected);

	if(requestReceived.PowerAdvice()!= requestExpected.PowerAdvice())	
			{
		TRACESTRING2("CTe_PsyRequest::CheckPowerAdviceReceivedL() unexpected power advice = %d", requestReceived.PowerAdvice());
			User::Leave(EFail);
			}
		}

// Has anything been received (update request/status update/update request cancel)?
EXPORT_C void CTe_PsyRequest::CheckNothingReceivedL()
	{
	if(iWasUpdateOrRequestReceived)
		{
		iWasUpdateOrRequestReceived = EFalse;
		User::Leave(EFail);
		}
	iWasPosUpdRequestReceived = EFalse;
	}

// Has a position update request been received?
EXPORT_C void CTe_PsyRequest::CheckPosUpdRequestNotReceivedL()
	{
	if(iWasPosUpdRequestReceived)
		{
		iWasPosUpdRequestReceived = EFalse;
		User::Leave(EFail);
		}
	}


void CTe_PsyRequest::RunL()
	{
	TRACESTRING("CTe_PsyRequest::RunL()");
	CTe_Active::RunL();
	iWasUpdateOrRequestReceived = ETrue;
	TInt err= iPosUpdateReqs.GetPositionUpdateRequest(iRequestReceived);
	if(err!=KErrNone)
		{
		iWasUpdateOrRequestReceived = EFalse;
		}	
	TRACESTRING2("Received type = %d", iRequestReceived.Type());
	if(iRequestReceived.Type() == TLbsPositionUpdateRequestBase::EUpdateRequest)  
		{
		iWasPosUpdRequestReceived = ETrue;
		}

	}

void CTe_PsyRequest::DoCancel()
	{
	iPosUpdateReqs.CancelNotifyPositionUpdateRequestChange();
	}

EXPORT_C void CTe_PsyRequest::ExpectDefaultUpdateRequest()
	{
	TLbsPositionUpdateRequest request;

	request.SetPowerAdvice(TLbsPositionUpdateRequestBase::EPowerAdviceOn);
	request.RequestQuality().SetMinHorizontalAccuracy(iDefaultPosQuality.HorizontalAccuracy());
	request.RequestQuality().SetMinVerticalAccuracy(iDefaultPosQuality.VerticalAccuracy());
	request.RequestQuality().SetMaxFixTime(iDefaultPosQuality.TimeToFirstFix());
	request.NewClient() = EFalse;
	iRequestExpected = request;
	}

EXPORT_C void CTe_PsyRequest::ExpectUpdateRequestWithRequestMethod(TPositionModuleInfo::TTechnologyType aMethod)
	{
	ExpectDefaultUpdateRequest();
	
	// now set the request method:
	TLbsNetPosRequestMethodInt methods;
	TLbsNetPosMethodInt posMethods[1];
	posMethods[0].SetPosMethod(KLbsPositioningMeansGps, (aMethod));
	methods.SetPosMethods(posMethods, 1);	
	static_cast<TLbsPositionUpdateRequest&>(iRequestExpected).RequestMethod() = methods;
	}

EXPORT_C void CTe_PsyRequest::ExpectDefaultStatusRequest()
	{
	TLbsPositionUpdateRequestStatus request;
	iRequestExpected = request;
	}

EXPORT_C void CTe_PsyRequest::ExpectPowerAdvice(TLbsPositionUpdateRequestBase::TPowerAdvice aPowerAdvice)
	{
	TLbsPositionUpdateRequestStatus request;
	
	request.SetPowerAdvice(aPowerAdvice);
	
	iRequestExpected = request;
	}

EXPORT_C void CTe_PsyRequest::ExpectDefaultCancelRequest()
	{
	TLbsPositionUpdateRequestCancel request;
	iRequestExpected = request;
	}
	
