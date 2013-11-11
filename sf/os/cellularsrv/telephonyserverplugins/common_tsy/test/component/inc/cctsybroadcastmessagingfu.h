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
// The TEFUnit header file which tests the BroadcastMessaging
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYBROADCASTMESSAGINGFU_H
#define CCTSYBROADCASTMESSAGINGFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyBroadcastMessagingFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestStoreBroadcastIdListL0001L();
	void TestStoreBroadcastIdListL0002L();
	void TestStoreBroadcastIdListL0003L();
	void TestStoreBroadcastIdListL0004L();
	void TestStoreBroadcastIdListL0005L();
	void TestStart0001L();
	void TestStart0002L();
	void TestStart0004L();
	void TestStart0005L();
	void TestReceiveMessage0001L();
	void TestReceiveMessage0002L();
	void TestReceiveMessage0003L();
	void TestReceiveMessage0004L();
	void TestReceiveMessage0005L();
	void TestGetFilterSetting0001L();
	void TestSetFilterSetting0001L();
	void TestSetFilterSetting0002L();
	void TestSetFilterSetting0003L();
	void TestSetFilterSetting0004L();
	void TestSetFilterSetting0005L();
	void TestGetLanguageFilter0001L();
	void TestSetLanguageFilter0001L();
	void TestNotifyFilterSettingChange0001L();
	void TestNotifyFilterSettingChange0002L();
	void TestNotifyFilterSettingChange0003L();
	void TestNotifyFilterSettingChange0004L();
	void TestNotifyLanguageFilterChange0001L();
	void TestNotifyBroadcastIdListChange0001L();
	void TestGetCaps0001L();
	void TestGetCaps0003L();


private:

	void FillInBroadcastIdListL(CMobilePhoneBroadcastIdList& aList);

	}; // class CCTsyBroadcastMessagingFU

#endif // CCTSYBROADCASTMESSAGINGFU_H

