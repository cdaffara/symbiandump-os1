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


#ifndef LBSNETSIMGATEWAYHANDLER_H
#define LBSNETSIMGATEWAYHANDLER_H
#include "lbsnetsim.h"
#include "lbsnetsimsessionhandler.h"
#include "lbsnetsimassistancedataproviderobserver.h"
#include <lbs/lbsassistancedatabuilderset.h>

class CLbsNetSimTestHandler;
class CLbsNetSimAssistanceDataProvider;

class CLbsNetSimGatewayHandler : 
	public CTimer,
	public MLbsNetSimSessionHandler,
	public MLbsNetSimAssistanceDataProviderObserver
	{
	public:
		// Construction
		static CLbsNetSimGatewayHandler* NewL(CLbsNetSimAssistanceDataProvider& aAssitanceDataProvider);
		void SetTestHandler(CLbsNetSimTestHandler* aTestHandler);
		~CLbsNetSimGatewayHandler();
		
		// MLbsNetSimSessionHandler
		void ServiceL(const RMessage2& aMessage);
		void Connected();
		void Disconnected();
		
		// MLbsNetSimAssistanceDataProviderObserver
		void ProcessAssistanceDataL(TInt aError,
									RLbsAssistanceDataBuilderSet& aAssistanceData,
								    TPositionInfo& aPosition, 
									TLbsNetPosRequestQuality& aQuality);
									
		// CTimer calls
		void RunL();
		
		// Calls from the test handler
		void SetResponseTime(TInt32 aTime, TBool aSticky);
		void SetResponseError(TInt32 aError, TBool aSticky);
		void SetAssistanceDataMask(TLbsAssistanceDataGroup& aMask);
		void SetMoreAssistanceDataMask(TLbsAssistanceDataGroup& aMask);
		void ClearAssistanceDataMasks();
		void SetStepMode(TBool aStepMode);
		void StartNetworkPrivacyRequestL(TLbsNetPosRequestPrivacy& aType, TLbsExternalRequestInfo& aRequestInfo);
		void CancelNetworkPrivacyRequestL();
		void StartNetworkLocationRequestL();
		void CancelNetworkLocationRequestL();
		void SendResetAssistanceData(TLbsAssistanceDataGroup aMask);
		
	private:
		CLbsNetSimGatewayHandler(CLbsNetSimAssistanceDataProvider& aAssistanceDataProvider);
		void ConstructL();
		
		// ServiceL handlers
		void HandleRegisterLcsMoLrL(const RMessage2& aMessage);
		void HandleMeasurementReportLocationL(const RMessage2& aMessage);
		void HandleReleaseLcsMoLrL(const RMessage2& aMessage);
		void HandleMeasurementControlFailureL(const RMessage2& aMessage);
		void HandleRequestMoreAssistanceDataL(const RMessage2& aMessage);
		void HandleReleaseLcsLocationNotifcationL(const RMessage2& aMessage);
		
		// Others 
		TBool CheckDestinationValidity(TPtrC aDestination);
	private:
		enum NextMessage
			{
			ENone, // If we are in this state then there has been an issue!
			EContinueRegisterLcsMoLr1,
			EContinueRegisterLcsMoLr2,
			EContinueMeasurementReportLocation,
			EContinueReleaseLcsMoLr,
			EContinueRequestMoreAssistanceData,
			EContinueMtLr,
			EContinueMeasurementControlFailure,
			};
		CArrayFixFlat<NextMessage>				iMessages;
		void AddMessageL(NextMessage aNextMessage);
		
		enum MessageState
			{
			ENothing,
			EMoLr,
			EMtLr,
			EMtLr_E,
			EPrivacy,
			EPrivacy_E,
			EX3P,
			};
		MessageState						iState;
		
		enum MessageSubState // Needs to be known to cancel for MtLr
			{
			EPreMrl, // MoLr, X3P: MeasurementReportLocation
			EPostMrl, 
			};
		MessageSubState						iSubState;
	private:
		// Others
		void Cancel(MessageState aToStart);
		void FinishL();
		void ResetInternals();
	private:
		static TLbsAssistanceDataGroup		iDefaultAssistanceDataMask;
	
		CLbsNetSimAssistanceDataProvider&	iAssistanceDataProvider;
		CLbsNetSimTestHandler*				iTestHandler;
		
		//
		TLbsAssistanceDataGroup				iAssistanceDataMask;
		TLbsAssistanceDataGroup				iMoreAssistanceDataMask;
		
		// 
		TInt32								iWaitTime;
		TBool								iWaitTimeSticky;
		TInt32								iError;
		TBool								iErrorSticky;
		
		// Cancelling
		TBool								iMtLrOverrides;
		TBool								iRequestCancelled;
		MessageState						iStartOnComplete;
		TLbsNetPosRequestPrivacy			iPrivacyType;
		TLbsExternalRequestInfo				iPrivacyRequest;
		
		// Message handling: AssistanceData
		TInt								iAssistanceDataError;
		TBool								iGotAssistanceData; 
		RLbsAssistanceDataBuilderSet		iAssistanceData;
		RMessage2							iAssistanceDataToFill;
		
		// Message handling: RegisterLcsMoLr
		HBufC*								iRegisterLcsMoLrData;
		

		// Message handling: ReleaseLcsMoLr
		TInt								iReleaseLcsMoLrReason;
		RMessage2							iMessageReleaseLcsMoLr;

		// Message handling: MeasurementControlFailure
		TInt								iMeasurementControlFailureReason;

		// Message handling: MeasurementReportLocation
		TInt								iMeasurementReportLocationStatus;
		TPositionInfo						iMeasurementReportLocationPosition;
		RMessage2							iMessageMeasurementReportLocation;

		// Message handling: MeasurementControlLocation
		RLbsAssistanceDataBuilderSet		iMeasurementControlLocationAssistanceData;
		TPositionInfo						iMeasurementControlLocationPosition;
		TLbsNetPosRequestQuality			iMeasurementControlLocationQuality;
		
		// Message Handlers
		RMessage2							iMessageMeasurementControlFailure;
		
		// Notifications
		RMessage2							iNotificationMeasurementControlLocation;
		RMessage2							iNotificationRegisterLcsLocation;
		RMessage2							iNotificationCancelPrivacyRequest;
		RMessage2							iNotificationNetworkGone;
		RMessage2							iNotificationResetAssistanceData;
	};


#endif // LBSNETSIMGATEWAYHANDLER_H
