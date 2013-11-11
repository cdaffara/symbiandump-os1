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

#ifndef CCTSYPHONEFUNEGATIVE_H
#define CCTSYPHONEFUNEGATIVE_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyPhoneFUNegative : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	//'negative' unit tests
	void TestUnit0001L();

	void TestUnit0007L();
	void TestUnit0008L();
	void TestUnit0009L();
	void TestUnit0010L();
	void TestUnit0011L();
	void TestUnit0012L();
	void TestUnit0013L();
	void TestUnit0014L();
	void TestUnit0015L();
	void TestUnit0016L();
	void TestUnit0017L();
	void TestUnit0018L();
	void TestUnit0019L();
	void TestUnit0020L();
	void TestUnit0021L();
	void TestUnit0022L();
	void TestUnit0023L();
	void TestUnit0024L();
	void TestUnit0025L();
	void TestUnit0026L();
	void TestUnit0027L();
	void TestUnit0028L();
	void TestUnit0029L();
	void TestUnit0030L();
	void TestUnit0031L();
	
private:
	}; // class CCTsyPhoneFUNegative

#endif // CCTSYPHONEFUNEGATIVE_H

