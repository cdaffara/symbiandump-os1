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
// functional unit of the Common TSY Dispatcher.
//



/**
 @file The TEFUnit header file which tests the CallControl
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYCALLCONTROLFUNEGATIVE_H
#define CCTSYCALLCONTROLFUNEGATIVE_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyCallControlFUNegative : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	//'negative' unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();
	void TestUnit0004L();
	void TestUnit0005L();
	void TestUnit0006L();
	void TestUnit0007L();
	void TestUnit0008L();
	void TestUnit0009L();

	void TestUnit0011L();
	void TestUnit0012L();
	void TestUnit0013L();
	void TestUnit0014L();
	void TestUnit0015L();
	void TestUnit0016L();
	void TestUnit0017L();

	void TestUnit0019L();

	void TestUnit0021L();
	void TestUnit0022L();
	void TestUnit0023L();
	void TestUnit0024L();


private:
	void CreateIncomingCallL(TInt aCallId, RMobilePhone::TMobileService aMode);
	void WaitForIncomingCallNotificationL(RMobileCall& aMobileCall, RMobileLine& aMobileLine, TName& aCallName);

	}; // class CCTsyCallControlFUNegative

#endif // CCTSYCALLCONTROLFUNEGATIVE_H

