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
// Protocol Module network observer class definition
// 
//

#ifndef __CNETWORKOBSERVER_H__
#define __CNETWORKOBSERVER_H__

#include <e32base.h>
#include <e32cmn.h>
#include <lbspositioninfo.h>
#include "lbsnetsimgateway.h"
#include "lbsnetsimgatewayobserver.h"
#include "ctesttimer.h"


/** Network observer class definition.

This class is used to capture calls made from the Protocol Module
to the Network.
*/
class CNetworkObserver : public CBase, public MTestTimerObserver
	{
public:

	/** Callback method result enumeration.
	This is used to show which observer method was called.
	*/
	enum TCallBackResult
		{
		ENone,
		EConnect,
		ERegisterLcsMoLr,
		EReleaseLcsMoLr,
		EReleaseLcsLocationNotification,
		EMeasurementReportRequestMoreAssistanceData,
		EMeasurementReportLocation,
		EMeasurementControlFailure,
		EClose,
		ETimeout
		};

public:

	static CNetworkObserver* NewL();
	~CNetworkObserver();
	
	void ResetWasObserverCalled();
	void WaitForCallBack();
	TBool WasObserverCalled() const;
	TInt CallBackResult() const;
	TBool IsMoreObserverActivity();

	const TDesC& X3pDestination() const;
	TInt MeasurementFailure() const;
	TInt SessionCloseReason() const;
	const CLbsNetworkProtocolBase::TLbsPrivacyResponse& PrivacyResponse() const;
	const TPosition& MobilePos() const;
	TBool IsNetworkAlive() const;
	void SetNetworkAlive(TBool aAlive);
	const TLbsAsistanceDataGroup& MoreDataMask() const;


	// RLbsNetSim methods
	TInt ConnectL(RLbsNetSim* aNetworkInterface);
	void RegisterLcsMoLr(const TDesC& aDestination);
	void ReleaseLcsMoLr(const TInt& aReason);
	void ReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse);
	void MeasurementReportLocationRequestMoreAssistanceData(const TLbsAsistanceDataGroup& aAssistData);
	void MeasurementReportLocation(const TPositionInfoBase& aPosition);
	void MeasurementControlFailure(TInt aReason);
	void Close();
	
	// MLbsNetSimObserver wrapper methods
	void ProcessRegisterLcsLocationNotificationL(const TLbsExternalRequestInfo& aInfo, 
				const TLbsNetPosRequestPrivacy& aType);
	void ProcessReleaseLcsLocationNotificationL(TInt aReason);
	void ProcessFacilityLcsMoLrResultL(TInt aReason, const TPositionInfoBase* aPosition);
	void ProcessMeasurementControlLocationL(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
	void ProcessMeasurementControlLocationErrorL(TInt aError);


	// MTestTimerObserver methods

	/** Timer expiry event
	This is used to identify a timer expiry and is called by the timer.
	@param aEventId The identity of the timer that expired
	*/
	void OnTimerEvent(TInt aEventId);

private:

	CNetworkObserver();
	void ConstructL();

	void SignalObserverCallBack(const TCallBackResult aResult);

private:

	/** Flag to show if observer has received a callback
	*/
	TBool iWasObserverCalled;

	/** Value relating to the observer method that was called.
	*/
	TCallBackResult iCallBackResult;
	
	/** Local (nested) active scheduler used to wait for callbacks
	*/
	CActiveSchedulerWait* iScheduler;

	/** Pointer to the network interface.
	*/
	RLbsNetSim* iNetworkInterface;

	/** X3P Destination.
	*/
	TBuf<64> iX3pDestination;

	/** Measurement failure reason.
	*/
	TInt iMeasurementFailure;
	
	/** Session close reason
	*/
	TInt iSessionCloseReason;

	/** Privacy response
	*/
	CLbsNetworkProtocolBase::TLbsPrivacyResponse iPrivacyResponse;

	/** Mobile Location position
	*/
	TPosition iMobilePos;
	
	/** Test timer
	*/
	CTestTimer* iTimer;

	/** Flag to indicate if network is alive.
	Test harnesses use CNetworkObserver::SetNetworkAlive() to change this
	and thereby render network alive/dead as required.
	*/
	TBool iIsNetworkAlive;

	/** Data mask representing a request for more data
	*/
	TLbsAsistanceDataGroup iMoreDataMask;
	};


/** Global variable namespace NetworkObserverGlobals
This permits the RLbsNetSim test stub to access a test observer.
This is not ideal but no other solution could be devised which
allows the protocol module to be built for unit tests and also 
for the integrated LBS system environment.
*/
namespace NetworkObserverGlobals
	{
	extern CNetworkObserver* G_TestObserver;
	}
	
#endif // __CNETWORKOBSERVER_H__

