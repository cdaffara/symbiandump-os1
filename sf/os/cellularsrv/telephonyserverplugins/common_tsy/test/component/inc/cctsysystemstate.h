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
// The TEFUnit header file which tests access to the System State 
// from the Common TSY.
// 
//

#ifndef CCTSYSYSTEMSTATE_H
#define CCTSYSYSTEMSTATE_H

#include <test/tefunit.h>
#include "cctsycomponenttestbase.h"

class CCTsySystemState : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps
	void TestSystemState0001L();
	
private:

	}; // class CCTsySystemState

#endif // CCTSYSYSTEMSTATE_H
