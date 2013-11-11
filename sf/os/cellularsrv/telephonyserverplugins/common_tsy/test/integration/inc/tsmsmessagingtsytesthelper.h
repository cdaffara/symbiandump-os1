// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSMSMESSAGINGTSYTESTHELPER_H
#define TSMSMESSAGINGTSYTESTHELPER_H


#include "ttsytesthelperbase.h"
	
class TSmsMessagingTsyTestHelper : public TTsyTestHelperBase
	{
public:
	TSmsMessagingTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForMobileSmsMessagingNotifyMoSmsBearerChange( 
				RMobileSmsMessaging& aMobileSmsMessaging,
				TEtelRequestBase& aRequestStatus,
				RMobileSmsMessaging::TMobileSmsBearer& aBearer, 
				RMobileSmsMessaging::TMobileSmsBearer aExpectedBearer,
				TInt aWantedStatus );

	void WaitForMobileSmsMessagingNotifyReceiveModeChange( 
				RMobileSmsMessaging& aMobileSmsMessaging,
				TEtelRequestBase& aRequestStatus,
				RMobileSmsMessaging::TMobileSmsReceiveMode& aReceiveMode, 
				RMobileSmsMessaging::TMobileSmsReceiveMode aExpectedReceiveMode,
				TInt aWantedStatus );

	void WaitForMobileSmsMessagingNotifySmspListChange( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus );

	void SendSmsMessage(TEtelRequestBase& aRequestStatus, TDesC &aMessage, 
			RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg &aAttributesPckg);
	
	}; // class TSmsMessagingTsyTestHelper 

#endif
