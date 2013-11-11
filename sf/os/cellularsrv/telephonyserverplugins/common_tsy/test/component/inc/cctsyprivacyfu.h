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
// The TEFUnit header file which tests the Privacy
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPRIVACYFU_H
#define CCTSYPRIVACYFU_H

#include <test/tefunit.h>

#include <etelmm.h>

#include "cctsycomponenttestbase.h"

class CCTsyPrivacyFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestSetPrivacy0001L();
	void TestSetPrivacy0006L();
	void TestSetPrivacy0011L();
	void TestNotifyPrivacyConfirmation0001L();
	void TestNotifyPrivacyConfirmation0002L();
	void TestNotifyPrivacyConfirmation0004L();
	void TestNotifyPrivacyConfirmation0006L();
	void TestNotifyPrivacyConfirmation0007L();
	void TestNotifyPrivacyConfirmation0009L();
	void TestNotifyPrivacyConfirmation00011L();
	void TestNotifyPrivacyConfirmation00012L();
	void TestNotifyPrivacyConfirmation00014L();
	void TestGetDefaultPrivacy0001L();
	void TestSetDefaultPrivacy0001L();
	void TestNotifyDefaultPrivacyChange0001L();


private:
	void DoSetPrivacyL(const TDesC& aLineName,RMobilePhone::TMobilePhonePrivacy aPrivacySetting);
	void DoNotifyPrivacyConfirmationL(const TDesC& aLineName);
	void DoCancelNotifyPrivacyConfirmationL(const TDesC& aLineName);
	void DoMultiNotifyPrivacyConfirmationL(const TDesC& aLineName);

	}; // class CCTsyPrivacyFU

#endif // CCTSYPRIVACYFU_H

