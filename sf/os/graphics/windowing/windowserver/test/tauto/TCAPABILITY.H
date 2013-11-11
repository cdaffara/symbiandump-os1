
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

#ifndef __TCAPABILITY_H__
#define __TCAPABILITY_H__

#include "AUTO.H"
#include "../tlib/testbase.h"
#include "TGraphicsHarness.h"

class CTCapability : public CTWsGraphicsBase
	{
public:
	CTCapability(CTestStep* aStep);
	~CTCapability();
	void ConstructL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void GetCapabilityTestResultsL(TInt& aNoOfTest, TInt&aNoOfTestPass);
	void LaunchNewProcess(const TDesC& aExecutableName,const TDesC& acapability);
	};

class CTCapabilityStep : public CTGraphicsStep
	{
public:
	CTCapabilityStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTCapabilityStep,"TCapability");


#endif
