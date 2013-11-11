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
// Protocol Module network observer class implementation
// 
//


#include <e32base.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include "cnetworkobserver.h"

const TInt KGatewayTimerId = 0x20202020;
const TInt KGatewayTimerDurationInSec = 1;

// Use global variables in namespace NetworkObserverGlobals
// This is not ideal but we have no other way of allowing
// the RLbsNetSim test stub to access this test observer.
namespace NetworkObserverGlobals
	{
	CNetworkObserver* G_TestObserver;
	}
using namespace NetworkObserverGlobals;


/** Static constructor
*/
CNetworkObserver* CNetworkObserver::NewL()
	{
	CNetworkObserver* self = new (ELeave) CNetworkObserver();
	CleanupStack::PushL(self);
	self->ConstructL();
	G_TestObserver = self;
	CleanupStack::Pop(self);
	return self;
	}


/** Constructor
*/
CNetworkObserver::CNetworkObserver() : iIsNetworkAlive(ETrue)
	{
	G_TestObserver = NULL;
	}


/** Destructor
*/
CNetworkObserver::~CNetworkObserver()
	{
	delete iTimer;
	delete iScheduler;
	}


/** Second phase Constructor
*/
void CNetworkObserver::ConstructL()
	{
	iScheduler = (CActiveSchedulerWait*) new CActiveSchedulerWait;
	iTimer = CTestTimer::NewL(KGatewayTimerId, *this);
	}


/** Reset the observer's called status
*/
void CNetworkObserver::ResetWasObserverCalled()
	{
	iWasObserverCalled = EFalse;
	iCallBackResult = ENone;
	}


/** Wait for an observer callback
*/
void CNetworkObserver::WaitForCallBack()
	{
	if (!iWasObserverCalled)
		{
		iScheduler->Start();
		}
	}


/** Signal that observer has received a callback
@param aResult Enumeration showing which observer method was called.
*/
void CNetworkObserver::SignalObserverCallBack(const TCallBackResult aResult)
	{
	iTimer->Cancel();
	iWasObserverCalled = ETrue;
	iCallBackResult = aResult;
	if (iScheduler->IsStarted())
		{
		iScheduler->AsyncStop();
		}
	}


/** Check if the observer was called
*/
TBool CNetworkObserver::WasObserverCalled() const
	{
	return iWasObserverCalled;
	}


/** Retrieve callback result
*/
TInt CNetworkObserver::CallBackResult() const
	{
	return iCallBackResult;
	}


/** Retrieve X3P destination
*/
const TDesC& CNetworkObserver::X3pDestination() const
	{
	return iX3pDestination;
	}


/** Retrieve measurement failure reason
*/
TInt CNetworkObserver::MeasurementFailure() const
	{
	return iMeasurementFailure;
	}


/** Session close reason
*/
TInt CNetworkObserver::SessionCloseReason() const
	{
	return iSessionCloseReason;
	}


/** Privacy response
*/
const CLbsNetworkProtocolBase::TLbsPrivacyResponse& CNetworkObserver::PrivacyResponse() const
	{
	return iPrivacyResponse;
	}


/** Mobile location position
*/
const TPosition& CNetworkObserver::MobilePos() const
	{
	return iMobilePos;
	}


TInt CNetworkObserver::ConnectL(RLbsNetSim* aNetworkInterface)
	{
	TInt result = KErrCouldNotConnect;

	if (iIsNetworkAlive)
		{
		User::LeaveIfNull(aNetworkInterface);
		iNetworkInterface = aNetworkInterface;
		result = KErrNone;
		}

	return result;
	}


void CNetworkObserver::RegisterLcsMoLr(const TDesC& aDestination)
	{
	if (iIsNetworkAlive)
		{
		iX3pDestination = aDestination;
		SignalObserverCallBack(ERegisterLcsMoLr);
		}
	else
		{
		iNetworkInterface->Observer()->ProcessError(KErrDisconnected);
		}
	}


void CNetworkObserver::ReleaseLcsMoLr(const TInt& aReason)
	{
	if (iIsNetworkAlive)
		{
		iSessionCloseReason = aReason;
		SignalObserverCallBack(EReleaseLcsMoLr);
		}
	else
		{
		iNetworkInterface->Observer()->ProcessError(KErrDisconnected);
		}
	}


void CNetworkObserver::ReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse)
	{
	if (iIsNetworkAlive)
		{
		iPrivacyResponse = aResponse;
		SignalObserverCallBack(EReleaseLcsLocationNotification);
		}
	else
		{
		iNetworkInterface->Observer()->ProcessError(KErrDisconnected);
		}
	}


void CNetworkObserver::MeasurementReportLocationRequestMoreAssistanceData(const TLbsAsistanceDataGroup& aAssistData)
	{
	if (iIsNetworkAlive)
		{
		iMoreDataMask = aAssistData;
		SignalObserverCallBack(EMeasurementReportRequestMoreAssistanceData);
		}
	else
		{
		iNetworkInterface->Observer()->ProcessError(KErrDisconnected);
		}
	}


void CNetworkObserver::MeasurementReportLocation(const TPositionInfoBase& aPosition)
	{
	if (iIsNetworkAlive)
		{
		reinterpret_cast <const TPositionInfo&> (aPosition).GetPosition(iMobilePos);
		SignalObserverCallBack(EMeasurementReportLocation);
		}
	else
		{
		iNetworkInterface->Observer()->ProcessError(KErrDisconnected);
		}
	}


void CNetworkObserver::MeasurementControlFailure(TInt aReason)
	{
	if (iIsNetworkAlive)
		{
		iMeasurementFailure = aReason;
		SignalObserverCallBack(EMeasurementControlFailure);
		}
	else
		{
		iNetworkInterface->Observer()->ProcessError(KErrDisconnected);
		}
	}


void CNetworkObserver::Close()
	{
	if (iIsNetworkAlive)
		{
		iNetworkInterface = NULL;
		SignalObserverCallBack(EClose);
		}
	}


const TLbsAsistanceDataGroup& CNetworkObserver::MoreDataMask() const
	{
	return iMoreDataMask;
	}


void CNetworkObserver::ProcessMeasurementControlLocationL(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	User::LeaveIfNull(iNetworkInterface);
	User::LeaveIfNull(iNetworkInterface->Observer());
	iNetworkInterface->Observer()->ProcessMeasurementControlLocation(aPosition, aData, aQuality);
	}


void CNetworkObserver::ProcessMeasurementControlLocationErrorL(TInt aError)
	{
	User::LeaveIfNull(iNetworkInterface);
	User::LeaveIfNull(iNetworkInterface->Observer());
	iNetworkInterface->Observer()->ProcessMeasurementControlLocationError(aError);
	}


void CNetworkObserver::ProcessFacilityLcsMoLrResultL(TInt aReason, const TPositionInfoBase* aPosition)
	{
	User::LeaveIfNull(iNetworkInterface);
	User::LeaveIfNull(iNetworkInterface->Observer());
	iNetworkInterface->Observer()->ProcessFacilityLcsMoLrResult(aReason, aPosition);
	}


void CNetworkObserver::ProcessRegisterLcsLocationNotificationL(const TLbsExternalRequestInfo& aInfo, 
			const TLbsNetPosRequestPrivacy& aType)
	{
	User::LeaveIfNull(iNetworkInterface);
	User::LeaveIfNull(iNetworkInterface->Observer());
	iNetworkInterface->Observer()->ProcessRegisterLcsLocationNotification(aInfo, aType);
	}


void CNetworkObserver::ProcessReleaseLcsLocationNotificationL(TInt aReason)
	{
	User::LeaveIfNull(iNetworkInterface);
	User::LeaveIfNull(iNetworkInterface->Observer());
	iNetworkInterface->Observer()->ProcessReleaseLcsLocationNotification(aReason);
	}


void CNetworkObserver::OnTimerEvent(TInt aEventId)
	{
	if (KGatewayTimerId == aEventId)
		{
		SignalObserverCallBack(ETimeout);
		}
	}


/** Check if more observer activity takes place
*/
TBool CNetworkObserver::IsMoreObserverActivity()
	{
	ResetWasObserverCalled();
	iTimer->After(TTimeIntervalMicroSeconds32(KGatewayTimerDurationInSec));
	WaitForCallBack();
	return (ETimeout == CallBackResult()) ? EFalse : ETrue;
	}


/** Indicate if network is alive
@return TBool ETrue means network is alive
*/
TBool CNetworkObserver::IsNetworkAlive() const
	{
	return iIsNetworkAlive;
	}


/** Set network alive status
@param aAlive is the alive status for the network
*/
void CNetworkObserver::SetNetworkAlive(TBool aAlive)
	{
	iIsNetworkAlive = aAlive;
	}

