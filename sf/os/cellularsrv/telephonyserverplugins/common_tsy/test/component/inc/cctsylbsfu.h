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
// The TEFUnit header file which tests the LBS
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYLBSFU_H
#define CCTSYLBSFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyLBSFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyMtlr0001L();
	void TestSetLcsDomain0001L();
	void TestSendMtlrResponse0001L();
	void TestSendMtlrResponse0002L();
	void TestSendMtlrResponse0003L();
	void TestSendMeasurementReport0001L();
	void TestSendMeasurementReport0002L();
	void TestNotifyMeasurementControl0001L();
	void TestSendMolr0001L();


private:


	}; // class CCTsyLBSFU

#endif // CCTSYLBSFU_H

