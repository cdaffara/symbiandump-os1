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
 @file The TEFUnit header file which tests the SupplementaryServices
*/

/**
 @file
 @internalAll 
*/

#ifndef CCTSYSUPPLEMENTARYSERVICESFU_H
#define CCTSYSUPPLEMENTARYSERVICESFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsySupplementaryServicesFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// use-case tests
	void TestUseCase0001L();
	void TestUseCase0002L();
	void TestUseCase0003L();
	void TestUseCase0004L();
	void TestUseCase0005L();
	
	//other unit tests
	void TestUnit0001L();
	void TestUnit0002L();
	void TestUnit0003L();
	void TestUnit0004L();
	void TestUnit0005L();
	void TestUnit0006L();
	void TestUnit0007L();
	void TestUnit0008L();
	
private:

	}; // class CCTsySupplementaryServicesFU

#endif // CCTSYSUPPLEMENTARYSERVICESFU_H

