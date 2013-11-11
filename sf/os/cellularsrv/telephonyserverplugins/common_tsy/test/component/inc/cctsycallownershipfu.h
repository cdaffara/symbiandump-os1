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
// The TEFUnit header file which tests the CallOwnership
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYCALLOWNERSHIPFU_H
#define CCTSYCALLOWNERSHIPFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyCallOwnershipFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestLoanDataPort0001L();
	void TestLoanDataPort0006L();
	void TestLoanDataPort0007L();
	void TestLoanDataPort0008L();
	void TestLoanDataPort00011L();
	void TestRecoverDataPort0001L();
	void TestRecoverDataPort0006L();
    void TestRecoverDataPort0007L();
	void TestRecoverDataPort00011L();
	void TestAcquireOwnership0001L();
	void TestAcquireOwnership0006L();
	void TestAcquireOwnership0007L();
	void TestAcquireOwnership0009L();
	void TestAcquireOwnership00011L();
	void TestTransferOwnership0001L();
	void TestTransferOwnership0006L();
	void TestTransferOwnership00011L();


private:

	void TestLoanDataPortNotSupportedL(const TDesC &aName, const TInt aCallId);
	void TestRecoverDataPortNotSupportedL(const TDesC &aName);
	void TestAcquireOwnershipNotSupportedL(const TDesC &aName);
	void TestTransferOwnershipNotSupportedL(const TDesC &aName);

	}; // class CCTsyCallOwnershipFU

#endif // CCTSYCALLOWNERSHIPFU_H

