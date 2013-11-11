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
// ctestagpsrequestsender.h
// Base class for sending requests to the AGPS Manager using
// the same channels that the Location Server or the
// Network Request Handler would
// 
//

#include "ctestagpsrequestsender.h"

CTestAgpsRequestSender* CTestAgpsRequestSender::NewL(const TUint aChannelUidValue)
	{
	CTestAgpsRequestSender* self = new(ELeave) CTestAgpsRequestSender();
	CleanupStack::PushL(self);
	self->ConstructL(aChannelUidValue);
	CleanupStack::Pop(self);
	return self;
	}

CTestAgpsRequestSender::CTestAgpsRequestSender()
	{	
	}

CTestAgpsRequestSender::~CTestAgpsRequestSender()
	{
	iPosUpdateReq.Close();		
	}

// aChannelUidValue is the value of the UID of the process wanting to
// send requests to the AGPS Manager (either the LS or the NRH)
void CTestAgpsRequestSender::ConstructL(TUint aChannelUidValue)
	{
		const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifier = 
			{
				{KLbsGpsLocManagerUidValue},{aChannelUidValue}
			};

		iPosUpdateReq.OpenL(KChannelIdentifier);
	}

void CTestAgpsRequestSender::SendRequest(const TLbsPositionUpdateRequestBase& aRequest)
	{
	__ASSERT_ALWAYS(KErrNone == iPosUpdateReq.SetPositionUpdateRequest(aRequest), User::Invariant());
	}
	
	
// Use this call when only the positioning methods in
// the request are important for the test
void CTestAgpsRequestSender::RequestPositionUpdate(const TTestValidPosMethod& aPosMethod)
	{
	// Make up quality and target time of the request
	TLbsLocRequestQualityInt quality;
	quality.SetMaxFixTime(5000000);
	quality.SetMinHorizontalAccuracy(100);
	quality.SetMinVerticalAccuracy(100);
	TTime targetTime;
	targetTime.UniversalTime(); // target time is now and try for 5 seconds

	TLbsNetPosRequestMethodInt methods;
	
	// Build the method request object     
   	switch(aPosMethod)
   		{
   		case ETPosMethodAut:
   			{
	   		TLbsNetPosMethodInt posMethods1[1];
	   		posMethods1[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal);
	   		methods.SetPosMethods(posMethods1, 1);	
   			}
   		break;
   		
   		case ETPosMethodTb:
   			{
			TLbsNetPosMethodInt posMethods2[1];
			posMethods2[0].SetPosMethod(KLbsPositioningMeansGps,
									   TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted);
			methods.SetPosMethods(posMethods2, 1);
   			}
		break;
		
		case ETPosMethodTa:
			{
			TLbsNetPosMethodInt posMethods3[1];
			posMethods3[0].SetPosMethod(KLbsPositioningMeansGps,
									   TPositionModuleInfo::ETechnologyNetwork|TPositionModuleInfo::ETechnologyAssisted);
			methods.SetPosMethods(posMethods3, 1);
			}
		break;
		
		case ETPosMethodTaAut:
			{
			TLbsNetPosMethodInt posMethods4[2];
			posMethods4[0].SetPosMethod(KLbsPositioningMeansGps,
									   TPositionModuleInfo::ETechnologyNetwork|TPositionModuleInfo::ETechnologyAssisted);
			posMethods4[1].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal);
			methods.SetPosMethods(posMethods4, 2);
			}
		break;

		case ETPosMethodTbTa:
			{
			TLbsNetPosMethodInt posMethods5[2];
			posMethods5[0].SetPosMethod(KLbsPositioningMeansGps,
									   TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted);
			posMethods5[1].SetPosMethod(KLbsPositioningMeansGps,
									   TPositionModuleInfo::ETechnologyNetwork|TPositionModuleInfo::ETechnologyAssisted);			   
			methods.SetPosMethods(posMethods5, 2);
			}
		break;
		
		default:
		ASSERT(EFalse);
		break;
   		}
   		
	RequestPositionUpdate(quality, methods, targetTime);
	}


void CTestAgpsRequestSender::RequestPositionUpdate(const TLbsNetPosRequestMethodInt& aPosMethods)
	{
	// Make up quality and target time of the request
	TLbsLocRequestQualityInt quality;
	quality.SetMaxFixTime(5000000);
	quality.SetMinHorizontalAccuracy(100);
	quality.SetMinVerticalAccuracy(100);
	TTime targetTime;
	targetTime.UniversalTime(); // target time is now and try for 5 seconds
 
    RequestPositionUpdate(quality, aPosMethods, targetTime);	
	}

// The test can use this call to specify all of the parameters in a location update request
void CTestAgpsRequestSender::RequestPositionUpdate(const TLbsLocRequestQualityInt& aQuality,
											   const TLbsNetPosRequestMethodInt& aPosMethods,
											   const TTime& aTargetTime)
	{
	TLbsPositionUpdateRequest locRequest;
	
   	locRequest.RequestQuality() = aQuality;
	locRequest.TargetTime() = aTargetTime;
	locRequest.RequestMethod() = aPosMethods;
	
	SendRequest(locRequest);
	}

void CTestAgpsRequestSender::CancelPositionUpdate()
	{
	TLbsPositionUpdateRequestCancel cancelRequest;
	SendRequest(cancelRequest);	
	}
	
void CTestAgpsRequestSender::AdviseStatus(const TLbsPositionUpdateRequestBase::TPowerAdvice& aPowerAdvice)
	{
	TLbsPositionUpdateRequestStatus status;
	status.SetPowerAdvice(aPowerAdvice);
	SendRequest(status);
	}
