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
// functional unit of the Common TSY.
// 
//

/**
 @file The TEFUnit header file which tests the MBMS
*/

#ifndef CCTSYUCASEMBMS002_H
#define CCTSYUCASEMBMS002_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyUCaseMbms002 : public CCtsyComponentTestBase
	{
	    
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps
	void CCtsyIntegrationTestMbms0001L();
	void CCtsyIntegrationTestMbms0002L();
	void CCtsyIntegrationTestMbms0003L();


private:

	}; // class cctsyucasembms002

#endif // CCTSYUCASEMBMS002_H