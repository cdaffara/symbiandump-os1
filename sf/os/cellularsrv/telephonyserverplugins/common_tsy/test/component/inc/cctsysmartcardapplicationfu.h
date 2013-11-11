// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The TEFUnit header file which tests the SmartCardApplication
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYSMARTCARDAPPLICATIONFU_H
#define CCTSYSMARTCARDAPPLICATIONFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsySmartCardApplicationFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestReadScFile0001L();
	void TestUpdateScFile0001L();
	void TestGetScFileInfo0001L();
	void TestGetSmartCardApplicationInfo0001L();
	void TestSetSmartCardApplicationStatus0001L();
	void TestEnumerateSmartCardApplications0001L();
	void TestNotifySmartCardAppInfoChange0001L();


private:


	}; // class CCTsySmartCardApplicationFU

#endif // CCTSYSMARTCARDAPPLICATIONFU_H

