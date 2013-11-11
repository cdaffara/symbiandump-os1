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
// The TEFUnit header file which tests the Personalisation
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPERSONALISATIONFU_H
#define CCTSYPERSONALISATIONFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyPersonalisationFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetPersonalisationCaps0001L();
	void TestGetPersonalisationStatus0001L();
	void TestSetPersonalisationStatus0001L();


private:


	}; // class CCTsyPersonalisationFU

#endif // CCTSYPERSONALISATIONFU_H

