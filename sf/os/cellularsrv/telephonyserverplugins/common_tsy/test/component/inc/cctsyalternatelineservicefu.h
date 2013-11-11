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
// The TEFUnit header file which tests the AlternateLineService
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYALTERNATELINESERVICEFU_H
#define CCTSYALTERNATELINESERVICEFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyAlternateLineServiceFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetALSLine0001L();
	void TestGetALSLine0005L();
	void TestSetALSLine0001L();
	void TestSetALSLine0004L();
	void TestSetALSLine0005L();
	void TestNotifyALSLineChange0001L();
	void TestNotifyALSLineChange0002L();
	void TestNotifyALSLineChange0004L();


private:


	}; // class CCTsyAlternateLineServiceFU

#endif // CCTSYALTERNATELINESERVICEFU_H

