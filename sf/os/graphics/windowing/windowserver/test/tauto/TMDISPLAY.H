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
// TBLANK.H
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TMDISPLAY_H__
#define __TMDISPLAY_H__

#include "AUTO.H"
#include "TGraphicsHarness.h"
#include "../tlib/testbase.h"

class CTMultiDisplay : public CTWsGraphicsBase
	{
public:
	CTMultiDisplay(CTestStep* aStep);
	CTMultiDisplay();
	void ConstructL();
	TestState DoTestL();
	~CTMultiDisplay();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	
private:
	void DoFocusScreenTestL();
	void DoScreenTestL(TInt aScreenNo);
	};

class CTMultiDisplayStep : public CTGraphicsStep
	{
public:
	CTMultiDisplayStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTMultiDisplayStep,"TMultiDisplay");


#endif
