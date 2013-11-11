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

#ifndef CCTSYSECURITYFUNEGATIVE_H
#define CCTSYSECURITYFUNEGATIVE_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsySecurityFUNegative : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// 'Negative' unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();
	void TestUnit0004L();

	void TestUnit0006L();

	void TestUnit0008L();
	void TestUnit0009L();
	void TestUnit0010L();
	void TestUnit0011L();
	void TestUnit0012L();
	
private:
	}; // class CCTsySecurityFUNegative

#endif // CCTSYSECURITYFUNEGATIVE_H

