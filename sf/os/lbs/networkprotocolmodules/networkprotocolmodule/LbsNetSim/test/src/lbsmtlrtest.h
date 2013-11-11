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
#if (!defined LBSMTLRTEST_H)
#define LBSMTLRTEST_H
#include <test/testexecutestepbase.h>
#include "Te_lbsnetsimtestSuiteStepBase.h"
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>
#include <lbs/test/lbsnetsimgateway.h>

class CLbsMtLrTest : 
	public CTe_lbsnetsimtestSuiteStepBase
	{
	public:
		enum EStates
			{
			EError,
			EStart,
			ETestStartNetworkPrivacyRequest,
			EGatewayProcessRegisterLcsLocationNotification,
			EGatewayReleaseLcsLocationNotification,
			ETestNotifyReleaseLcsLocationNotification,
			ETestStartNetworkLocationRequest,
			EGatewayMeasurementReportLocation,
			EFinished
			};

		CLbsMtLrTest();
		~CLbsMtLrTest();
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepL();
		virtual TVerdict doTestStepPostambleL();
	private:
		class CAOGateway : 
			public CAOGateway_Base
			{
			public:
				CAOGateway(CLbsMtLrTest* aBase);
				~CAOGateway();
				
				void RunL();
				void DoCancel();
				TBool Finished();
			
				// MLbsNetSimGatewayObserver
				virtual void ProcessRegisterLcsLocationNotification(const TLbsExternalRequestInfo& aInfo, const TLbsNetPosRequestPrivacy& aType);
				virtual void ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
			private:
				TBool iProcessRegisterLcsLocationNotification;
				TBool iProcessMeasurementControlLocation;
			};
			
		class CAOTest :
			public CAOTest_Base
			{
			public:
				CAOTest(CLbsMtLrTest* aBase, TInt aDelay, TBool aStepMode);
				~CAOTest();
				
				void RunL();
				void DoCancel();
				TBool Finished();
			
				// MLbsNetSimTestObserver
				virtual void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
				virtual void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
				virtual void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);

			private:
				TBool					iNotifiyReleaseLcsLocationNotification;
				TBool					iNotifiyMeasurementControlLocation;
				TBool					iNotifiyMeasurementReportLocation;
				
			};
	};


_LIT(KMtLrTest,"MtLrTest");

#endif // LBSMTLRTEST_H
