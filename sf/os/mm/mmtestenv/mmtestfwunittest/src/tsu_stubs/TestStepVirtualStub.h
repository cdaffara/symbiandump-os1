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
// This file contains definitions of the test step classes for RTestStepVirtualStub
// 
//

#ifndef __TESTSTEPVIRTUALSTUB_H__
#define __TESTSTEPVIRTUALSTUB_H__


_LIT(KTestStepVirtualStubName, "TestStepVirtualStub");
_LIT(KTestStepVirtualStub2Name, "TestStepVirtualStub2");

// RTestStepVirtualStub : a test step stub class to run testing on

class RTestStepVirtualStub : public RTestStep
	{
public:
	static RTestStepVirtualStub* NewL();
	RTestStepVirtualStub();
	virtual TVerdict OpenL();
	virtual void Close();
	virtual TVerdict DoTestStepL();

	// extra accessors for testing
	CTestSuite* TestGetSuite() const;
	TVerdict TestGetResult() const;
	void TestSetStepName(const TDesC& aName);

	TBool PreambleRun();
	TBool PostambleRun();
	TBool StepRun();
	TBool TestGetBoolFromConfig(const TDesC &aSectName, const TDesC &aKeyName, TBool &aResult);

protected:
	TBool iPreambleRun;
	TBool iPostambleRun;
	TBool iStepRun;
	};

// RTestStepVirtualStub2 : a test step stub class which is identical to 
// RTestStepVirtualStub in all but name.


class RTestStepVirtualStub2 : public RTestStepVirtualStub
	{
public:
	static RTestStepVirtualStub2* NewL();
	RTestStepVirtualStub2();
	};

#endif // __TESTSTEPVIRTUALSTUB_H__
