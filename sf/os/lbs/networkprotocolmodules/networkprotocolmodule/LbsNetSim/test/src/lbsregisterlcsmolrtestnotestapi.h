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
#if (!defined LBSREGISTERLCSMOLRTESTNOTESTAPI_H)
#define LBSREGISTERLCSMOLRTESTNOTESTAPI_H
#include <test/testexecutestepbase.h>
#include "Te_lbsnetsimtestSuiteStepBase.h"
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>
#include <lbs/test/lbsnetsimgateway.h>

class CLbsRegisterLcsMoLrTestNoTestAPI : 
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

		CLbsRegisterLcsMoLrTestNoTestAPI();
		~CLbsRegisterLcsMoLrTestNoTestAPI();
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepL();
		virtual TVerdict doTestStepPostambleL();
	private:
		class CAOGateway : 
			public CAOGateway_Base
			{
			public:
				CAOGateway(CLbsRegisterLcsMoLrTestNoTestAPI* aBase);
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
				TBool iMeasurementControlLocation;
			};
	};


_LIT(KRegisterLcsMoLrNoTestApiTest,"RegisterLcsMoLrNoTestApiTest");

#endif // LBSREGISTERLCSMOLRTESTNOTESTAPI_H
