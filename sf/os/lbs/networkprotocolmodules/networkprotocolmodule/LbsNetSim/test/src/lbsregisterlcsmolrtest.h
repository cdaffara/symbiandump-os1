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
#if (!defined LBSREGISTERLCSMOLRTEST_H)
#define LBSREGISTERLCSMOLRTEST_H
#include <test/testexecutestepbase.h>
#include "Te_lbsnetsimtestSuiteStepBase.h"
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>
#include <lbs/test/lbsnetsimgateway.h>

class CLbsRegisterLcsMoLrTest;

class CLbsRegisterLcsMoLrTest : 
	public CTe_lbsnetsimtestSuiteStepBase
	{
	public:
		enum EStates
			{
			EError,
			EStart,
			EGatewayRegisterLcsMoLr,
			EGatewayMeasurementReportLocation,
			EGatewayReleaseLcsMoLr,
			EFinished
			};

		CLbsRegisterLcsMoLrTest();
		~CLbsRegisterLcsMoLrTest();
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepL();
		virtual TVerdict doTestStepPostambleL();
	private:
		class CAOGateway : 
			public CAOGateway_Base
			{
			public:
				CAOGateway(CLbsRegisterLcsMoLrTest* aBase);
				~CAOGateway();
				
				void RunL();
				void DoCancel();
				TBool Finished();
				
				void ProcessFacilityLcsMoLrResult(TInt, const TPositionInfoBase*);
				void ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
			private:
				TBufC<10>	iDestination;
				TBool iFacilityLcsMoLrResult;
				TBool iMeasurementControlLocation;
			};
			
		class CAOTest :
			public CAOTest_Base
			{
			public:
				CAOTest(CLbsRegisterLcsMoLrTest* aBase, TInt aDelay, TBool aStepMode);
				~CAOTest();
				
				void RunL();
				void DoCancel();
				TBool Finished();
				
				void NotifyRegisterLcsMoLr(const TDesC& aData);
				void NotifyReleaseLcsMoLr(TInt);
				void NotifyMeasurementReportLocation(const TPositionInfo&);
				void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
				void NotifyFacilityLcsMoLrResult(TInt, const TPositionInfo&);
			private:
				TBool	iNotificationRegisterLcsMoLr;
				TBool	iNotificationMeasurementReportLocation;
				TBool	iNotificationReleaseLcsMoLr;
				TBool	iNotificationFacilityLcsMoLrResult;
				TBool   iNotificationMeasurementControlLocation;
			};
	};


_LIT(KRegisterLcsMoLrTest,"RegisterLcsMoLrTest");

#endif // LBSREGISTERLCSMOLRTEST_H
