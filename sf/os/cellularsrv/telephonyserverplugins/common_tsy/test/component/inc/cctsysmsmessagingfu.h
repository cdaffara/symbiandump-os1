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
// The TEFUnit header file which tests the SmsMessaging
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYSMSMESSAGINGFU_H
#define CCTSYSMSMESSAGINGFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsySmsMessagingFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyMoSmsBearerChange0001L();
	void TestNotifyReceiveModeChange0001L();
	void TestNotifyReceiveModeChange0002L();
	void TestNotifyReceiveModeChange0004L();
	void TestSendMessage0001L();
	void TestSendMessage0002L();
	void TestSendMessage0003L();
	void TestSendMessage0004L();
	void TestSendMessage0005L();
	void TestAckSmsStored0001L();
	void TestAckSmsStored0002L();
	void TestAckSmsStored0004L();
	void TestAckSmsStored0005L();
	void TestNackSmsStored0001L();
	void TestNackSmsStored0002L();
	void TestNackSmsStored0004L();
	void TestNackSmsStored0005L();
	void TestGetMoSmsBearer0001L();
	void TestGetReceiveMode0001L();
	void TestReceiveMessage0001L();
	void TestReceiveMessage0001AL();
	void TestReceiveMessage0002L();
	void TestReceiveMessage0003L();
	void TestReceiveMessage0004L();
	void TestSetMoSmsBearer0001L();
	void TestSetMoSmsBearer0002L();
	void TestSetMoSmsBearer0004L();
	void TestSetReceiveMode0001L();
	void TestSetReceiveMode0002L();
	void TestSetReceiveMode0004L();
	void TestStoreSmspListL0001L();
	void TestStoreSmspListL0002L();
	void TestStoreSmspListL0004L();
	void TestStoreSmspListL0005L();
	void TestResumeSmsReception0001L();
	void TestResumeSmsReception0002L();
	void TestResumeSmsReception0004L();
	void TestResumeSmsReception0005L();
	void TestGetMessageStoreInfo0001L();
	void TestGetMessageStoreInfo0002L();
	void TestGetMessageStoreInfo0003L();
	void TestGetMessageStoreInfo0004L();
	void TestGetMessageStoreInfo0005L();
	void TestNotifySmspListChange0001L();
	void TestNotifySmspListChange0002L();
	void TestNotifySmspListChange0004L();
	void TestSendMessageNoFdnCheck0001L();
	void TestSendMessageNoFdnCheck0002L();
	void TestSendMessageNoFdnCheck0003L();
	void TestSendMessageNoFdnCheck0004L();
	void TestSendMessageNoFdnCheck0005L();
	void TestEnumerateMessageStores0001L();
	void TestGetCaps0001L();
	void TestGetCaps0003L();

private:


	}; // class CCTsySmsMessagingFU

#endif // CCTSYSMSMESSAGINGFU_H

