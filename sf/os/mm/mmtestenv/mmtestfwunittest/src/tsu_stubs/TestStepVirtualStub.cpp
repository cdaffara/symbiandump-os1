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
// This file is a helper for TestFramework Unit Test classes.
// It subclasses the abstract RTestStep to produce a working class we can test
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

#include "TestStepVirtualStub.h"

// --------------------------------------------
// RTestStepVirtualStub

RTestStepVirtualStub* RTestStepVirtualStub::NewL()
	{
	RTestStepVirtualStub* self = new(ELeave) RTestStepVirtualStub;
	return self;
	}

// Each test step initialises its own name
RTestStepVirtualStub::RTestStepVirtualStub()
	{
	iTestStepName = KTestStepVirtualStubName;

	iPreambleRun = EFalse;
	iPostambleRun = EFalse;
	iStepRun = EFalse;
	}

// preamble
TVerdict RTestStepVirtualStub::OpenL()
	{
	INFO_PRINTF1(_L("RTestStepVirtualStub::OpenL() has been called"));
	iPreambleRun = ETrue;
	return iTestStepResult = EPass;
	}

// postamble
void RTestStepVirtualStub::Close()
	{
	INFO_PRINTF1(_L("RTestStepVirtualStub::Close() has been called"));
	iPostambleRun = ETrue;
	}

// do the test step
TVerdict RTestStepVirtualStub::DoTestStepL()
	{
	INFO_PRINTF1(_L("RTestStepVirtualStub::DoTestStepL() has been called"));
	iStepRun = ETrue;
	return iTestStepResult = EPass;
	}

// Extra methods by which RTestStep accessors can be tested.
// Not for use by anything except Test Harnesses
CTestSuite* RTestStepVirtualStub::TestGetSuite() const
	{
	return iSuite;
	}

TVerdict RTestStepVirtualStub::TestGetResult() const
	{
	return iTestStepResult;
	}

void RTestStepVirtualStub::TestSetStepName(const TDesC& aName)
	{
	iTestStepName = aName;
	}

TBool RTestStepVirtualStub::PreambleRun()
	{ 
	return iPreambleRun;
	}

TBool RTestStepVirtualStub::PostambleRun()
	{
	return iPostambleRun;
	}

TBool RTestStepVirtualStub::StepRun()
	{
	return iStepRun;
	}

TBool RTestStepVirtualStub::TestGetBoolFromConfig(const TDesC &aSectName, const TDesC &aKeyName, TBool &aResult)
	{
	return GetBoolFromConfig(aSectName, aKeyName, aResult);
	}; 


// --------------------------------------------
// RTestStepVirtualStub2

RTestStepVirtualStub2* RTestStepVirtualStub2::NewL()
	{
	RTestStepVirtualStub2* self = new(ELeave) RTestStepVirtualStub2;
	return self;
	}

// Each test step initialises its own name
RTestStepVirtualStub2::RTestStepVirtualStub2()
	{
	iTestStepName = KTestStepVirtualStub2Name;

	iPreambleRun = EFalse;
	iPostambleRun = EFalse;
	iStepRun = EFalse;
	}

