// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

// CTMemLeakCheckEnable

class CTMemLeakCheckEnable : public CTWsGraphicsBase
	{
public:
	CTMemLeakCheckEnable(CTestStep* aStep);
	void ConstructL();
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TBool iIsWsTestMode;
	};

class CTMemLeakCheckEnableStep : public CTGraphicsStep
	{
public:
	CTMemLeakCheckEnableStep();
	TestState DoTestL();
protected:	
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTMemLeakCheckEnableStep,"TMemLeakCheckEnable");

// CTMemLeakCheckOneShot

class CTMemLeakCheckOneShot : public CTWsGraphicsBase
	{
public:
	CTMemLeakCheckOneShot(CTestStep* aStep);
	void ConstructL();
protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TBool iIsWsTestMode;
	};

class CTMemLeakCheckOneShotStep : public CTGraphicsStep
	{
public:
	CTMemLeakCheckOneShotStep();
	TestState DoTestL();
protected:	
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTMemLeakCheckOneShotStep,"TMemLeakCheckOneShot");
