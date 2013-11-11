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
// The TEFUnit header file which tests the PhoneControl
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPHONECONTROLFU_H
#define CCTSYPHONECONTROLFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyPhoneControlFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyModemDetected0001L();
	void TestGetPhoneInfo0001L();
	void TestGetPhoneInfo0003L();
	void TestIsSupportedByModule0001L();
	void TestIsSupportedByModule0003L();
	void TestGetCaps0001L();
	void TestGetInfo0001L();
	void TestGetStatus0001L();


private:


	}; // class CCTsyPhoneControlFU

#endif // CCTSYPHONECONTROLFU_H

