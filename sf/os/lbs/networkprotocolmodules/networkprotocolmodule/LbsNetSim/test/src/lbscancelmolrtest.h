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
#if (!defined LBSCANCELMOLRTEST_H)
#define LBSCANCELMOLRTEST_H
#include <test/testexecutestepbase.h>
#include "Te_lbsnetsimtestSuiteStepBase.h"
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>
#include <lbs/test/lbsnetsimgateway.h>

class CLbsCancelMoLrTest : 
	public CTe_lbsnetsimtestSuiteStepBase
	{
	public:
		enum EStates
			{
			EError,
			EStart,
			EGatewayRegisterLcsMoLr,
			EGatewayMeasurementControlFailure,
			EGatewayReleaseLcsMoLr,
			EFinished
			};

		CLbsCancelMoLrTest();
		~CLbsCancelMoLrTest();
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepL();
		virtual TVerdict doTestStepPostambleL();
	private:
		class CAOGateway : 
			public CAOGateway_Base
			{
			public:
				CAOGateway(CLbsCancelMoLrTest* aBase);
				~CAOGateway();
				
				void RunL();
				void DoCancel();
				TBool Finished();
			
				// MLbsNetSimGatewayObserver
				virtual void ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
			private:
				TBufC<10>	iDestination;
				TBool iMeasurementControlLocation;
			};
			
		class CAOTest :
			public CAOTest_Base
			{
			public:
				CAOTest(CLbsCancelMoLrTest* aBase, TInt aDelay, TBool aSteMode);
				~CAOTest();
				
				void RunL();
				void DoCancel();
				TBool Finished();
			
				// MLbsNetSimTestObserver
				virtual void NotifyRegisterLcsMoLr(const TDesC& aData);
				virtual void NotifyReleaseLcsMoLr(TInt aReason);
				virtual void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
				virtual void NotifyMeasurementReportControlFailure(TInt);
			private:
				TBool	iNotificationRegisterLcsMoLr;
				TBool	iNotificationReleaseLcsMoLr;
				TBool   iNotificationMeasurementControlLocation;
				TBool	iNotificationMeasurementControlFailure;
			};
	};


_LIT(KCancelMoLrTest,"CancelMoLrTest");

#endif // LBSCANCELMOLRTEST_H
