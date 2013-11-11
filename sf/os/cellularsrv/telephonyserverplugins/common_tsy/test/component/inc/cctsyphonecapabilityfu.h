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
// The TEFUnit header file which tests the PhoneCapability
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPHONECAPABILITYFU_H
#define CCTSYPHONECAPABILITYFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyPhoneCapabilityFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyCapsChange0001L();
	void TestNotifyCapsChange0001aL();
	void TestNotifyCapsChange0001bL();
	void TestNotifyCapsChange0001cL();
	void TestNotifyCapsChange0002L();
	void TestNotifyCapsChange0004L();
	void TestGetMultimodeCaps0001L();


private:


	}; // class CCTsyPhoneCapabilityFU

#endif // CCTSYPHONECAPABILITYFU_H

