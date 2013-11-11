// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__TCLIPMAIN_H__)
#define __TCLIPMAIN_H__

#include "tgdi.h"
#include <TestExecuteStepBase.h>
#include "TGraphicsHarness.h"

class TClipTest : public CTGraphicsBase
	{
public:
	TClipTest(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(const TInt aCurTestCase);
private:

private:
	};

class CTClipStep : public CTGraphicsStep
	{
public:
	CTClipStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTClipStep,"TClip");

#endif