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

/**
 @file
 @internalComponent
 @test
*/


#ifndef LBSNETSIMTESTHANDLER_H
#define LBSNETSIMTESTHANDLER_H
#include "lbsnetsim.h"
#include "lbsnetsimsessionhandler.h"

#include <e32property.h>

// Forwards
class CLbsNetSimGatewayHandler;
class CLbsNetSimAssistanceDataProvider;

class CLbsNetSimTestHandler : public MLbsNetSimSessionHandler
	{
	public:
		// Construction
		static CLbsNetSimTestHandler* NewL(CLbsNetSimAssistanceDataProvider& aAssitanceDataProvider);
		void SetGatewayHandler(CLbsNetSimGatewayHandler* aGatewayHandler);
		
		// MLbsNetSimSessionHandler
		void ServiceL(const RMessage2& aMessage);
		void Connected();
		void Disconnected();
		
		// Notifications from the Gateway Handler
		TInt GatewayConnected();
		TInt GatewayDisconnected();
		TInt GatewayRegisterLcsMoLr(const TDesC& aData);
		TInt GatewayProcessAssistanceDataL(RLbsAssistanceDataBuilderSet& aAssistanceData, TPositionInfo& aPosition,  TLbsNetPosRequestQuality& aQuality);
		TInt GatewayMeasurementReportLocationL(const TPositionInfo& aPostion);
		TInt GatewayFacilityLcsMoLrResultL(const TInt aReason, const TPositionInfo& aPostion);
		TInt GatewayReleaseLcsMoLrL(const TInt aReason);
		TInt GatewayMeasurementControlFailureL(const TInt aReason);
		TInt GatewayRequestMoreAssistanceDataL(const TLbsAssistanceDataGroup& aFilter);
		TInt GatewayReleaseLcsLocationNotificationL(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aReponse);
	private:
		// Constructors
		CLbsNetSimTestHandler(CLbsNetSimAssistanceDataProvider& aAssitanceDataProvider);
		void ConstructL();
		
		// Message handlers
		void HandleCallbacksL(const RMessage2& aMessage);
		void HandleSetReferenceLocationL(const RMessage2& aMessage);
		void HandleSetReferenceLocationDataL(const RMessage2& aMessage);
		void HandleSetResponseTime(const RMessage2& aMessage);
		void HandleSetResponseError(const RMessage2& aMessage);
		void HandleSetDefaultAssitanceDataFilter(const RMessage2& aMessage);
		void HandleSetMoreDefaultAssitanceDataFilter(const RMessage2& aMessage);
		void HandleClearAssitanceDataFilters(const RMessage2& aMessage);
		void HandleSetAssistanceDataProvider(const RMessage2& aMessage);
		void HandleSetEmergenyStatus(const RMessage2& aMessage);
		void HandleSetRoamingStatus(const RMessage2& aMessage);
		void HandleSetQuality(const RMessage2& aMessage);
		void HandleSetStepMode(const RMessage2& aMessage);
		void HandleReleaseLcsLocationNotificationL(const RMessage2& aMessage);
		void HandleReleaseLcsLocationNotificationCancelL();
		void HandleStartNetworkLocationRequestL(const RMessage2& aMessage);
		void HandleStartNetworkLocationRequestCancelL();
		void HandleSendResetAssistanceData(const RMessage2& aMessage);
		
		// Others
		TInt StepModeWait();
	private:
		CLbsNetSimAssistanceDataProvider& 	iAssistanceDataProvider; // Assistance data provider
		CLbsNetSimGatewayHandler*			iGatewayHandler;
		
		TBool								iGatewayConnected;
		TBool								iFireConnectedNotification;
		TBool								iStepMode;
		RProperty							iStepModeProperty;
		
		// Callback handlers
		RMessage2							iMessageConnect;
		RMessage2							iMessageDisconnect;
		RMessage2							iMessageRegisterLcsMoLr;
		RMessage2							iMessageMeasurementControlLocation;
		RMessage2							iMessageReleaseLcsMoLr;
		RMessage2							iMessageReleaseLcsLocationNotification;
		RMessage2							iMessageMeasurementReportLocation;
		RMessage2							iMessageMeasurementReportRequestMoreAssitanceData;
		RMessage2							iMessageMeasurementReportControlFailure;		
		RMessage2							iMessageFacilityLcsMoLrResult;
		RMessage2							iMessageStartNetworkLocationRequest;
	};

#endif // LBSNETSIMTESTHANDLER_H
