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
 @file lbsnetsimtest0Step.h
*/
#if (!defined LBSCONNECTDISCONNECTNOTIFICATIONTEST_H)
#define LBSCONNECTDISCONNECTNOTIFICATIONTEST_H
#include <test/testexecutestepbase.h>
#include "Te_lbsnetsimtestSuiteStepBase.h"
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>
#include <lbs/test/lbsnetsimgateway.h>

class CLbsConnectDisconnectNotificationTest;

class CLbsConnectDisconnectNotificationTest : 
	public CTe_lbsnetsimtestSuiteStepBase
	{
	public:
		enum EStates
			{
			EStart,
			ETestConnected,
			EGatewayConnected,
			ETestGotConnection,
			EGatewayDisconnected,
			ETestGotDisconnection,
			EFinished,
			};


		CLbsConnectDisconnectNotificationTest();
		~CLbsConnectDisconnectNotificationTest();
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepL();
		virtual TVerdict doTestStepPostambleL();
	private:
	class CAOGateway : 
		public CAOGateway_Base
		{
		public:
			CAOGateway(CLbsConnectDisconnectNotificationTest* aBase);
			~CAOGateway();
			
			void RunL();
			void DoCancel();
			TBool Finished();
		};
		
	class CAOTest :
		public CAOTest_Base
		{
		public:
			CAOTest(CLbsConnectDisconnectNotificationTest* aBase, TInt aDelay, TBool aStepMode);
			~CAOTest();
			
			void RunL();
			void DoCancel();
			TBool Finished();
		
			// MLbsNetSimTestObserver
			virtual void Connected();
			virtual void Disconnected();
		private:
			TBool	iConnected;
			TBool	iDisconnected;
		};
	};


_LIT(KConnectDisconnectNotificationTest,"ConnectDisconnectNotificationTest");

#endif // LBSCONNECTDISCONNECTNOTIFICATIONTEST_H
