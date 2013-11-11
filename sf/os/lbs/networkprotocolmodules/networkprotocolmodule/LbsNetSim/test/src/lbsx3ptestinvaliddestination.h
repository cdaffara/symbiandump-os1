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
#if (!defined LBSX3PESTINVALIDDESTINATION_H)
#define LBSX3PESTINVALIDDESTINATION_H
#include <test/testexecutestepbase.h>
#include "Te_lbsnetsimtestSuiteStepBase.h"
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>
#include <lbs/test/lbsnetsimgateway.h>

class CLbsX3PTestInvalidDestination : 
	public CTe_lbsnetsimtestSuiteStepBase
	{
	public:
		enum EStates
			{
			EError,
			EStart,
			EGatewayRegisterLcsMoLr,
			EGatewayReleaseLcsMoLr,
			EFinished
			};

		CLbsX3PTestInvalidDestination();
		~CLbsX3PTestInvalidDestination();
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepL();
		virtual TVerdict doTestStepPostambleL();
	private:
		class CAOGateway : 
			public CAOGateway_Base
			{
			public:
				CAOGateway(CLbsX3PTestInvalidDestination* aBase);
				~CAOGateway();
				
				void RunL();
				void DoCancel();
				TBool Finished();
			
				// MLbsNetSimGatewayObserver
				virtual void ProcessMeasurementControlLocationError(TInt aError);
			private:
				TBufC<20>	iDestination;
				TBool iMeasurementControlLocationError;
			};
			
		class CAOTest :
			public CAOTest_Base
			{
			public:
				CAOTest(CLbsX3PTestInvalidDestination* aBase, TInt aDelay, TBool aStepMode);
				~CAOTest();
				
				void RunL();
				void DoCancel();
				TBool Finished();
			
				// MLbsNetSimTestObserver
				virtual void NotifyReleaseLcsMoLr(TInt aReason);
			private:
				TBool	iNotificationReleaseLcsMoLr;
			};
	};


_LIT(KX3PTestInvalidDestination,"X3PTestInvalidDestination");

#endif // LBSX3PESTINVALIDDESTINATION_H
