// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "TestSuiteNetCon.h"
#include "TestStepNetCon.h"


EXPORT_C CTestSuiteNetCon* NewTestSuiteNetConL() 
//
// NewTestSuiteNetConL is exported at ordinal 1
// this provides the interface to allow schedule test
// to create instances of this test suite
//
	{

	return new(ELeave) CTestSuiteNetCon();
    }


CTestSuiteNetCon::~CTestSuiteNetCon()
//
// D'tor
//
	{
	}


TPtrC CTestSuiteNetCon::GetVersion()
//
// Make a version string available for test system
//	
	{
	
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}


void CTestSuiteNetCon::AddTestStepL(CTestStepNetCon* aTestStep)
//
// Add a test step into the suite
//
	{

	// test steps contain a pointer back to the suite which owns them
	aTestStep->iTestSuite = this; 

	// add the step using tyhe base class method
	CTestSuite::AddTestStepL(aTestStep);
	}


void CTestSuiteNetCon::InitialiseL()
//
// Constructor for NetCon test suite
// this creates the NetCon test step and
// stores it inside CTestSuiteNetCon
//
	{

	// add test steps
	for(TInt i=1; i<43; ++i)
		{
		AddTestStepL(new(ELeave) CTestStepNetCon(i));
		}
	}

