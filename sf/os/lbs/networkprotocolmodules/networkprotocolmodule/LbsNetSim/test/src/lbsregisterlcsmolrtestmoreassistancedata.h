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
#if (!defined LBSREGISTERLCSMOLRTESTMOREASSISTANCEDATA_H)
#define LBSREGISTERLCSMOLRTESTMOREASSISTANCEDATA_H
#include <test/testexecutestepbase.h>
#include "Te_lbsnetsimtestSuiteStepBase.h"
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>
#include <lbs/test/lbsnetsimgateway.h>

class CLbsRegisterLcsMoLrTestMoreAssistanceData : 
	public CTe_lbsnetsimtestSuiteStepBase
	{
	public:
		enum EStates
			{
			EError,
			EStart,
			EGatewayRegisterLcsMoLr,
			EGatewayRequestMoreAssistanceData,
			EGatewayMeasurementReportLocation,
			EGatewayReleaseLcsMoLr,
			EFinished
			};

		CLbsRegisterLcsMoLrTestMoreAssistanceData();
		~CLbsRegisterLcsMoLrTestMoreAssistanceData();
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepL();
		virtual TVerdict doTestStepPostambleL();
	private:
		class CAOGateway : 
			public CAOGateway_Base
			{
			public:
				CAOGateway(CLbsRegisterLcsMoLrTestMoreAssistanceData* aBase);
				~CAOGateway();
				
				void RunL();
				void DoCancel();
				TBool Finished();
			
				// MLbsNetSimGatewayObserver
				virtual void ProcessFacilityLcsMoLrResult(TInt aReason, const TPositionInfoBase* aPosition);
				virtual void ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
			private:
				TBufC<10>	iDestination;
				TBool iFacilityLcsMoLrResult;
				TInt  iMeasurementControlLocation;
			};
			
		class CAOTest :
			public CAOTest_Base
			{
			public:
				CAOTest(CLbsRegisterLcsMoLrTestMoreAssistanceData* aBase, TInt aDelay, TBool aStepMode);
				~CAOTest();
				
				void RunL();
				void DoCancel();
				TBool Finished();
			
				// MLbsNetSimTestObserver
				virtual void NotifyRegisterLcsMoLr(const TDesC& aData);
				virtual void NotifyReleaseLcsMoLr(TInt aReason);
				virtual void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
				virtual void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
				virtual void NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter);
				virtual void NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition);			
			private:
				TBool	iNotificationRegisterLcsMoLr;
				TBool	iNotificationMeasurementReportLocation;
				TBool	iNotificationReleaseLcsMoLr;
				TBool	iNotificationFacilityLcsMoLrResult;
				TInt   	iNotificationMeasurementControlLocation;
				TBool	iNotificationMeasurementReportRequestMoreAssistanceData;
			};
	};


_LIT(KRegisterLcsMoLrTestMoreAssistanceData,"RegisterLcsMoLrTestMoreAssistanceData");

#endif // LBSREGISTERLCSMOLRTESTMOREASSISTANCEDATA_H
