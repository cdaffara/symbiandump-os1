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
// The TEFUnit header file which tests the PhonePower
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPHONEPOWERFU_H
#define CCTSYPHONEPOWERFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyPhonePowerFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetBatteryCaps0001L();
	void TestGetBatteryCaps0002L();
	void TestGetBatteryCaps0003L();
	void TestGetBatteryCaps0005L();
	void TestGetBatteryInfo0001L();
	void TestGetBatteryInfo0002L();
	void TestGetBatteryInfo0003L();
	void TestGetBatteryInfo0004L();
	void TestGetBatteryInfo0005L();
	void TestNotifyBatteryInfoChange0001L();
	void TestNotifyBatteryInfoChange0002L();
	void TestNotifyBatteryInfoChange0003L();
	void TestNotifyBatteryInfoChange0004L();
	void TestNotifyBatteryInfoChange0005L();

	void TestHWRMGetBatteryInfo0001L();
	void TestHWRMGetBatteryInfo0004L();
	void TestHWRMNotifyBatteryInfoChange0001L();
	void TestHWRMNotifyBatteryInfoChange0002L();
	void TestHWRMNotifyBatteryInfoChange0004L();

private:


	}; // class CCTsyPhonePowerFU

#endif // CCTSYPHONEPOWERFU_H

