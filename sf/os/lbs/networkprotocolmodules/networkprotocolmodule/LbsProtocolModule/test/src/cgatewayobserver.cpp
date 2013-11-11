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


#include "cgatewayobserver.h"

const TInt KGatewayTimerId = 0x10101010;
const TInt KGatewayTimerDurationInSec = 1;

CGatewayObserver* CGatewayObserver::NewL()
	{
	CGatewayObserver* self = new (ELeave) CGatewayObserver();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CGatewayObserver::CGatewayObserver()
	{
	}


CGatewayObserver::~CGatewayObserver()
	{
	delete iTimer;
	delete iScheduler;
	}


void CGatewayObserver::ConstructL()
	{
	iScheduler = (CActiveSchedulerWait*) new CActiveSchedulerWait;
	iTimer = CTestTimer::NewL(KGatewayTimerId, *this);
	}


void CGatewayObserver::ResetWasObserverCalled()
	{
	iWasObserverCalled = EFalse;
	iCallBackResult = ENone;
	}


void CGatewayObserver::WaitForCallBack()
	{
	if (!iWasObserverCalled)
		{
		iScheduler->Start();
		}
	}


void CGatewayObserver::SignalObserverCallBack(const TCallBackResult aResult)
	{
	iTimer->Cancel();
	iWasObserverCalled = ETrue;
	iCallBackResult = aResult;
	if (iScheduler->IsStarted())
		{
		iScheduler->AsyncStop();
		}
	}


TBool CGatewayObserver::WasObserverCalled() const
	{
	return iWasObserverCalled;
	}


TInt CGatewayObserver::CallBackResult() const
	{
	return iCallBackResult;
	}


TInt CGatewayObserver::AssistanceDataError() const
	{
	return iAssistanceDataError;
	}


TLbsAsistanceDataGroup CGatewayObserver::AssistanceDataSetMask() const
	{
	return iAssistanceDataSetMask;
	}


TLbsAsistanceDataGroup CGatewayObserver::ProvidedDataSetMask() const
	{
	return iProvidedDataSetMask;
	}


const TLbsNetSessionId& CGatewayObserver::SessionIdValue() const
	{
	return iSessionIdValue;
	}


TInt CGatewayObserver::SessionCloseReason() const
	{
	return iSessionCloseReason;
	}


TBool CGatewayObserver::IsLocEmergency() const
	{
	return iLocEmergency;
	}


const MLbsNetworkProtocolObserver::TLbsNetProtocolService& CGatewayObserver::LocType() const
	{
	return iLocReqType;
	}


const TLbsNetPosRequestQuality& CGatewayObserver::LocQuality() const
	{
	return iLocQuality;
	}


const TPosition& CGatewayObserver::ReferencePos() const
	{
	return iReferencePos;
	}


const TLbsNetPosRequestPrivacy& 
	CGatewayObserver::PrivacyNotifType() const
	{
	return iPrivacyNotifType;
	}


void CGatewayObserver::RequesterId(TDes8& aId) const
	{
	iRequestInfo.GetRequesterId(aId);
	}

TBool CGatewayObserver::IsPrivEmergency() const
	{
	return iPrivEmergency;
	}


void CGatewayObserver::GetCurrentCapabilities(TLbsNetPosCapabilities& /*aCapabilities*/) const
	{
#pragma message ("Not using GetCurrentCapabilities")
	}


void CGatewayObserver::ProcessStatusUpdate(TLbsNetProtocolServiceMask /*aActiveServiceMask*/)
	{
#pragma message ("Not using ProcessStatusUpdate")
	}


void CGatewayObserver::ProcessLocationRequest(const TLbsNetSessionId& aSessionId, TBool aEmergency,
	TLbsNetProtocolService aType,
	const TLbsNetPosRequestQuality& aQuality,
	const TLbsNetPosRequestMethod& /*aMethod*/)
	{
#pragma message ("Not using position request method")
	iSessionIdValue = aSessionId;
	iLocEmergency = aEmergency;
	iLocReqType = aType;
	iLocQuality = aQuality;
	SignalObserverCallBack(EProcessLocationRequest);
	}


void CGatewayObserver::ProcessSessionComplete(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	iSessionIdValue = aSessionId;
	iSessionCloseReason = aReason;
	SignalObserverCallBack(EProcessSessionComplete);
	}


void CGatewayObserver::ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
			const RLbsAssistanceDataBuilderSet& aData, TInt aReason)
	{
	iAssistanceDataError = aReason;
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

	SignalObserverCallBack(EProcessAssistanceData);
	}


void CGatewayObserver::ProcessLocationUpdate(const TLbsNetSessionId& aSessionId, 
									const TPositionInfoBase& aReferenceLocation)
	{
	iSessionIdValue = aSessionId;
	reinterpret_cast <const TPositionInfo&> (aReferenceLocation).GetPosition(iReferencePos);
	SignalObserverCallBack(EProcessLocationUpdate);
	}


void CGatewayObserver::ProcessPrivacyRequest(const TLbsNetSessionId& aSessionId, TBool aEmergency, 
									const TLbsNetPosRequestPrivacy& aNotification,
									const TLbsExternalRequestInfo& aRequestInfo)
	{
	iSessionIdValue = aSessionId;
	iPrivEmergency = aEmergency;
	iPrivacyNotifType = aNotification;
	iRequestInfo = aRequestInfo;
	SignalObserverCallBack(EProcessPrivacyRequest);
	}


void CGatewayObserver::OnTimerEvent(TInt aEventId)
	{
	if (KGatewayTimerId == aEventId)
		{
		SignalObserverCallBack(ETimeout);
		}
	}


TBool CGatewayObserver::IsMoreObserverActivity()
	{
	ResetWasObserverCalled();
	iTimer->After(TTimeIntervalMicroSeconds32(KGatewayTimerDurationInSec));
	WaitForCallBack();
	return (ETimeout == CallBackResult()) ? EFalse : ETrue;
	}


