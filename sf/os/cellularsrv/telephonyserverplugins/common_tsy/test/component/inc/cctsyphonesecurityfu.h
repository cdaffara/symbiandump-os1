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
// The TEFUnit header file which tests the PhoneSecurity
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPHONESECURITYFU_H
#define CCTSYPHONESECURITYFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"


class CCTsyPhoneSecurityFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetLockInfo0001L();
	void TestGetLockInfo0002L();
	void TestGetLockInfo0003L();
	void TestGetLockInfo0005L();
	void TestClearBlacklist0001L();
	void TestSetLockSetting0001L();
	void TestSetLockSetting0002L();
	void TestSetLockSetting0004L();
	void TestSetLockSetting0005L();
	void TestSetLockSetting0006L();
	void TestGetSecurityCaps0001L();
	void TestAbortSecurityCode0001L();
	void TestAbortSecurityCode0005L();
	void TestAbortSecurityCode0006L();
	void TestAbortSecurityCode0007L();
	void TestChangeSecurityCode0001L();
	void TestChangeSecurityCode0002L();
	void TestChangeSecurityCode0004L();
	void TestChangeSecurityCode0005L();
	void TestVerifySecurityCode0001L();
	void TestVerifySecurityCode0002L();
	void TestVerifySecurityCode0004L();
	void TestVerifySecurityCode0005L();
	void TestVerifySecurityCode0006L();
	void TestVerifySecurityCode0007L();
	void TestVerifySecurityCode0008L();
	void TestVerifySecurityCode0009L();
	void TestVerifySecurityCode0010L();
	void TestVerifySecurityCode0011L();
	void TestVerifySecurityCode0012L();
	void TestVerifySecurityCode0013L();
	void TestVerifySecurityCode0014L();
	void TestNotifySecurityEvent0001L();
	void TestNotifySecurityEvent0002L();
	void TestNotifySecurityEvent0004L();
	void TestNotifySecurityEvent0006L();
	void TestNotifySecurityEvent0007L();
	void TestNotifySecurityEvent0008L();
	void TestNotifySecurityEvent0009L();
	void TestNotifySecurityEvent0010L();
	void TestNotifyLockInfoChange0001L();
	void TestNotifyLockInfoChange0002L();
	void TestNotifyLockInfoChange0003L();
	void TestNotifyLockInfoChange0004L();
	void TestGetSecurityCodeInfo0001L();
	void TestGetSecurityCodeInfo0002L();
	void TestGetSecurityCodeInfo0003L();
	void TestGetSecurityCodeInfo0004L();
	void TestGetSecurityCodeInfo0005L();
	void TestGetSecurityCodeInfo0006L();
	void TestGetSecurityCodeInfo0007L();
	void TestNotifySecurityCodeInfoChange0001L();
	void TestNotifySecurityCodeInfoChange0002L();
	void TestNotifySecurityCodeInfoChange0003L();
	void TestNotifySecurityCodeInfoChange0004L();
	void TestNotifySecurityCapsChange0001L();
	void TestNotifySecurityCapsChange0002L();
	void TestNotifySecurityCapsChange0004L();


private:


	}; // class CCTsyPhoneSecurityFU

#endif // CCTSYPHONESECURITYFU_H

