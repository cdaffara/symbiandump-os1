/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file lbsregisterlcsmolrtest.h
*/
#if (!defined LBSCANCELMOLRFORMTLR_H)
#define LBSCANCELMOLRFORMTLR_H
#include <test/testexecutestepbase.h>
#include "Te_lbsnetsimtestSuiteStepBase.h"
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>
#include <lbs/test/lbsnetsimgateway.h>

class CLbsMolrCancelForMtLr : 
	public CTe_lbsnetsimtestSuiteStepBase
	{
	public:
		enum EStates
			{
			EError,
			EStart,
			EGWRegisterLcsMoLr,
			ETestStartNetworkPrivacyRequest,
			EGWReleaseLcsMoLr,
			EGWReleaseLcsLocationNotification,
			ETestStartNetworkLocationRequest,
			EGWMeasurementReportLocation,
			EFinished
			};

		CLbsMolrCancelForMtLr();
		~CLbsMolrCancelForMtLr();
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepL();
		virtual TVerdict doTestStepPostambleL();
	private:
		class CAOGateway : 
			public CAOGateway_Base
			{
			public:
				CAOGateway(CLbsMolrCancelForMtLr* aBase);
				~CAOGateway();
				
				void RunL();
				void DoCancel();
				TBool Finished();
			
				// MLbsNetSimGatewayObserver
				virtual void ProcessRegisterLcsLocationNotification(const TLbsExternalRequestInfo& aInfo, const TLbsNetPosRequestPrivacy& aType);
				virtual void ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
				virtual void ProcessMeasurementControlLocationError(TInt aError);
			private:
				TBufC<10>	iDestination;
				
				TBool iRegisterLcsLocationNotification;
				TBool iMeasurementControlLocation;
				TBool iMeasurementControlLocationError;
			};
			
		class CAOTest :
			public CAOTest_Base
			{
			public:
				CAOTest(CLbsMolrCancelForMtLr* aBase, TInt aDelay, TBool aStepMode);
				~CAOTest();
				
				void RunL();
				void DoCancel();
				TBool Finished();
			
				// MLbsNetSimTestObserver
				virtual void NotifyRegisterLcsMoLr(const TDesC& aData);
				virtual void NotifyReleaseLcsMoLr(TInt aReason);
				virtual void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
				virtual void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
				virtual void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
			private:
				TBool iNotificationRegisterLcsMoLr;
				TBool iReleaseLcsLocationNotification;
				TBool iMeasurementReportLocation;
				TBool iNotificationReleaseLcsMoLr;
				TBool iNotifiyMeasurementControlLocation;
			};
	};


_LIT(KMoLrCancelForMtLr,"MoLrCancelForMtLr");

#endif // LBSCANCELMOLRFORMTLR_H
