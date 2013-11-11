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
// The TEFUnit header file which tests the SMSPStore
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYSMSPSTOREFU_H
#define CCTSYSMSPSTOREFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsySMSPStoreFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestStart0001L();
	void TestStart0002L();
	void TestStart0003L();
	void TestStart0004L();
	void TestStart0005L();


private:

	static void Cleanup(TAny* self);
	void DoCleanup();

	}; // class CCTsySMSPStoreFU

#endif // CCTSYSMSPSTOREFU_H

