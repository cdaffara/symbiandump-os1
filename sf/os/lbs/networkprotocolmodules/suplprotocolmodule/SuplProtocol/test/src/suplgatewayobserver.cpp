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
// Protocol Module gateway observer class implementation
// 
//


#include "suplgatewayobserver.h"

const TInt KGatewayTimerId = 0x10101011;
const TInt KGatewayTimerDurationInMicroSec = 1000000;

CSuplGatewayObserver* CSuplGatewayObserver::NewL()
	{
	CSuplGatewayObserver* self = new (ELeave) CSuplGatewayObserver();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CSuplGatewayObserver::CSuplGatewayObserver()
	{
	}


CSuplGatewayObserver::~CSuplGatewayObserver()
	{
	delete iTimer;
	delete iScheduler;
	}


void CSuplGatewayObserver::ConstructL()
	{
	iScheduler = (CActiveSchedulerWait*) new CActiveSchedulerWait;
	iTimer = CLbsCallbackTimer::NewL(*this);
	SetDefaultCapabilities();
	}


void CSuplGatewayObserver::ResetWasObserverCalled()
	{
	iWasObserverCalled = EFalse;
	iCallBackResult = ENone;
	}


void CSuplGatewayObserver::WaitForCallBack()
	{
	if (!iWasObserverCalled)
		{
		iScheduler->Start();
		}
	}


void CSuplGatewayObserver::SignalObserverCallBack(const TCallBackResult aResult)
	{
	iTimer->Cancel();
	iWasObserverCalled = ETrue;
	iCallBackResult = aResult;
	if (iScheduler->IsStarted())
		{
		iScheduler->AsyncStop();
		}
	}


TBool CSuplGatewayObserver::WasObserverCalled() const
	{
	return iWasObserverCalled;
	}


TInt CSuplGatewayObserver::CallBackResult() const
	{
	return iCallBackResult;
	}


TInt CSuplGatewayObserver::AssistanceDataError() const
	{
	return iAssistanceDataError;
	}


TLbsAsistanceDataGroup CSuplGatewayObserver::AssistanceDataSetMask() const
	{
	return iAssistanceDataSetMask;
	}
	
TLbsAsistanceDataGroup CSuplGatewayObserver::AssistanceDataErrorMask() const
	{
	return iAssistanceDataErrorMask;
	}

TLbsAsistanceDataGroup CSuplGatewayObserver::ProvidedDataSetMask() const
	{
	return iProvidedDataSetMask;
	}


const TLbsNetSessionId& CSuplGatewayObserver::SessionIdValue() const
	{
	return iSessionIdValue;
	}


TInt CSuplGatewayObserver::SessionCloseReason() const
	{
	return iSessionCloseReason;
	}


TBool CSuplGatewayObserver::IsLocEmergency() const
	{
	return iLocEmergency;
	}


const MLbsNetworkProtocolObserver::TLbsNetProtocolService& CSuplGatewayObserver::LocType() const
	{
	return iLocReqType;
	}


const TLbsNetPosRequestQuality& CSuplGatewayObserver::LocQuality() const
	{
	return iLocQuality;
	}


const TPosition& CSuplGatewayObserver::ReferencePos() const
	{
	return iReferencePos;
	}


const TLbsNetPosRequestPrivacy& 
	CSuplGatewayObserver::PrivacyNotifType() const
	{
	return iPrivacyNotifType;
	}


void CSuplGatewayObserver::RequesterId(TDes8& aId) const
	{
	iRequestInfo.GetRequesterId(aId);
	}

TBool CSuplGatewayObserver::IsPrivReqReceived()
	{
	TBool ret = iPrivReqReceived;
	//Clear flag for future tests
	//(compiler won't allow to just set to EFalse)
	iPrivReqReceived = EFalse;
	return ret;
	}


void CSuplGatewayObserver::GetCurrentCapabilities(TLbsNetPosCapabilities& aCapabilities) const
	{
	aCapabilities = iCapabilities;
	}

void CSuplGatewayObserver::SetDefaultCapabilities() 
	{
	TInt numMethods = 3;
	TLbsNetPosMethod posMethods[KLbsMaxNetPosMethods];
	posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
	posMethods[1].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
	posMethods[2].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal);
	TInt err = iCapabilities.SetPosMethods(posMethods, numMethods);
	}

void CSuplGatewayObserver::SetCurrentCapabilities(const TLbsNetPosCapabilities& aCapabilities)
	{
	iCapabilities = aCapabilities;
	}


void CSuplGatewayObserver::ProcessStatusUpdate(TLbsNetProtocolServiceMask /*aActiveServiceMask*/)
	{
#pragma message ("Not using ProcessStatusUpdate")
	}


void CSuplGatewayObserver::ProcessLocationRequest(const TLbsNetSessionId& aSessionId, TBool aEmergency,
	TLbsNetProtocolService aType,
	const TLbsNetPosRequestQuality& aQuality,
	const TLbsNetPosRequestMethod& aMethod)
	{
#pragma message ("Not using position request method")
	iSessionIdValue = aSessionId;
	iLocEmergency = aEmergency;
	iLocReqType = aType;
	iLocQuality = aQuality;
	iMethod = aMethod;
	SignalObserverCallBack(EProcessLocationRequest);
	}


void CSuplGatewayObserver::ProcessSessionComplete(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	iSessionIdValue = aSessionId;
	iSessionCloseReason = aReason;
	SignalObserverCallBack(EProcessSessionComplete);
	}


void CSuplGatewayObserver::ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
			const RLbsAssistanceDataBuilderSet& aData, TInt aReason, const TLbsNetSessionIdArray& /*aSessionIdArray*/)
	{
	
	// iAssistanceDataError only stores error reasons
	// (do not store KErrNone as sometimes valid assistance data is reported
	// straigt after invalid assistance data and that would reset the error 
	// reason to KErrNone, which is not what the tests expect).
	if (aReason != KErrNone)
	{
	iAssistanceDataError = aReason;
	iAssistanceDataErrorMask = aDataMask;
	}


	if (aReason == KErrNone)
	{
	iAssistanceDataSetMask = aDataMask;

	RLbsAssistanceDataBuilderSet& localData = const_cast<RLbsAssistanceDataBuilderSet&>(aData);

	iProvidedDataSetMask = 0;
	RUEPositioningGpsAlmanacBuilder* dataItem;
	if ((KErrNone == localData.GetDataBuilder(dataItem)) &&
		(dataItem->IsDataAvailable()))
		{
		iProvidedDataSetMask |= EAssistanceDataAlmanac;
		}

	RBadSatListBuilder* dataItem1;
	if ((KErrNone == localData.GetDataBuilder(dataItem1)) &&
		(dataItem1->IsDataAvailable()))
		{
		iProvidedDataSetMask |= EAssistanceDataBadSatList;
		}

	RUEPositioningGpsIonosphericModelBuilder* dataItem2;
	if ((KErrNone == localData.GetDataBuilder(dataItem2)) &&
		(dataItem2->IsDataAvailable()))
		{
		iProvidedDataSetMask |= EAssistanceDataIonosphericModel;
		}

	RUEPositioningGpsNavigationModelBuilder* dataItem3;
	if ((KErrNone == localData.GetDataBuilder(dataItem3)) &&
		(dataItem3->IsDataAvailable()))
		{
		iProvidedDataSetMask |= EAssistanceDataNavigationModel;
		}

	RReferenceLocationBuilder* dataItem4;
	if ((KErrNone == localData.GetDataBuilder(dataItem4)) &&
		(dataItem4->IsDataAvailable()))
		{
		iProvidedDataSetMask |= EAssistanceDataReferenceLocation;
		}

	RUEPositioningGpsReferenceTimeBuilder* dataItem5;
	if ((KErrNone == localData.GetDataBuilder(dataItem5)) &&
		(dataItem5->IsDataAvailable()))
		{
		iProvidedDataSetMask |= EAssistanceDataReferenceTime;
		}

	RUEPositioningGpsUtcModelBuilder* dataItem6;
	if ((KErrNone == localData.GetDataBuilder(dataItem6)) &&
		(dataItem6->IsDataAvailable()))
		{
		iProvidedDataSetMask |= EAssistanceDataPositioningGpsUtcModel;
		}

	RUEPositioningGpsAcquisitionAssistanceBuilder* dataItem7;
	if ((KErrNone == localData.GetDataBuilder(dataItem7)) &&
		(dataItem7->IsDataAvailable()))
		{
		iProvidedDataSetMask |= EAssistanceDataAquisitionAssistance;
		}
	}

	SignalObserverCallBack(EProcessAssistanceData);
	}


void CSuplGatewayObserver::ProcessLocationUpdate(const TLbsNetSessionId& aSessionId, 
									const TPositionInfoBase& aLocation)
	{
	iSessionIdValue = aSessionId;
	reinterpret_cast <const TPositionInfo&> (aLocation).GetPosition(iReferencePos);
	SignalObserverCallBack(EProcessLocationUpdate);
	
	
	// When Reference Location is reported by SUPL, assistance data are reported straight
	// after, overwritting this callback's result. This flag is an additional way to remember
	// that ProcessLocationUpdate has been called and should only be checked after receiving
	// assistance data when reference location was expected.
	iRefLocNotified = ETrue;
	}


void CSuplGatewayObserver::ProcessPrivacyRequest(const TLbsNetSessionId& aSessionId, TBool /*aEmergency*/, 
									const TLbsNetPosRequestPrivacy& aNotification,
									const TLbsExternalRequestInfo& aRequestInfo)
	{
	iPrivReqReceived = ETrue;
	
	iSessionIdValue = aSessionId;
	iPrivacyNotifType = aNotification;
	iRequestInfo = aRequestInfo;
	SignalObserverCallBack(EProcessPrivacyRequest);
	}

/** Handle a timeout
*/
void CSuplGatewayObserver::OnTimerEventL(TInt aTimerId)
	{
	if (KGatewayTimerId == aTimerId)
		{
		SignalObserverCallBack(ETimeout);
		}	
	}

/** Handle a timer error
*/
TInt CSuplGatewayObserver::OnTimerError(TInt aTimerId, TInt aError)
	{
	(void)aError;
	
	if (KGatewayTimerId == aTimerId)
		{
		SignalObserverCallBack(ETimerError);
		}
	return aError;
	}

TBool CSuplGatewayObserver::IsMoreObserverActivity()
	{
	TBool ret;
	ResetWasObserverCalled();
	iTimer->EventAfter(TTimeIntervalMicroSeconds32(KGatewayTimerDurationInMicroSec),KGatewayTimerId);
	WaitForCallBack();
	ret = ETimeout == CallBackResult()? EFalse : ETrue;
	ResetWasObserverCalled();
	return ret;
	}


TBool CSuplGatewayObserver::IsRefLocNotified()
{
	return iRefLocNotified;
}
