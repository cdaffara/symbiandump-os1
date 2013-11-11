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
// The TEFUnit header file which tests the USIMApplication
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYUSIMAPPLICATIONFU_H
#define CCTSYUSIMAPPLICATIONFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyUSIMApplicationFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetUSimApplicationInfo0001L();
	void TestGetUSimAppsSelectionMode0001L();
	void TestSetUSimApplicationStatus0001L();
	void TestSetUSimAppsSelectionMode0001L();
	void TestEnumerateUSimApplications0001L();
	void TestNotifyUSimApplicationsInfoChange0001L();
	void TestNotifyUSimAppsSelectionModeChange0001L();
	void TestGetCurrentActiveUSimApplication0001L();
	void TestGetCurrentActiveUSimApplication0002L();
	void TestGetCurrentActiveUSimApplication0004L();
	void TestGetCurrentActiveUSimApplication0005L();	

private:


	}; // class CCTsyUSIMApplicationFU

#endif // CCTSYUSIMAPPLICATIONFU_H

