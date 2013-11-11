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
// The TEFUnit header file which tests the UssdMessaging
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYUSSDMESSAGINGFU_H
#define CCTSYUSSDMESSAGINGFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyUssdMessagingFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestSendMessage0001L();
	void TestSendMessage0002L();
	void TestSendMessage0003L();
	void TestSendMessage0004L();
	void TestSendMessage0005L();
	void TestSendRelease0001L();
	void TestSendRelease0001bL();
	void TestSendRelease0002L();
	void TestSendRelease0003L();
	void TestSendRelease0004L();
	void TestSendRelease0005L();
	void TestReceiveMessage0001L();
	void TestReceiveMessage0002L();
	void TestReceiveMessage0003L();
	void TestReceiveMessage0004L();
	void TestNotifyNetworkRelease0001L();
	void TestNotifyNetworkRelease0002L();
	void TestNotifyNetworkRelease0003L();
	void TestNotifyNetworkRelease0004L();
	void TestSendMessageNoFdnCheck0001L();
	void TestSendMessageNoFdnCheck0002L();
	void TestSendMessageNoFdnCheck0003L();
	void TestSendMessageNoFdnCheck0004L();
	void TestSendMessageNoFdnCheck0005L();
	void TestGetCaps0001L();
	void TestGetCaps0003L();


private:


	}; // class CCTsyUssdMessagingFU

#endif // CCTSYUSSDMESSAGINGFU_H

