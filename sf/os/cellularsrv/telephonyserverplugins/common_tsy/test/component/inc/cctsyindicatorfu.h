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
// The TEFUnit header file which tests the Indicator
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYINDICATORFU_H
#define CCTSYINDICATORFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyIndicatorFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetIndicator0001L();
	void TestGetIndicator0002L();
	void TestGetIndicator0004L();
	void TestGetIndicatorCaps0001L();
	void TestNotifyIndicatorChange0001L();
	void TestNotifyIndicatorChange0002L();
	void TestNotifyIndicatorChange0004L();


private:


	}; // class CCTsyIndicatorFU

#endif // CCTSYINDICATORFU_H

