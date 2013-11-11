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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TGRAPHICSDRAWER_H__
#define __TGRAPHICSDRAWER_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTGraphicsDrawer : public CTGraphicsBase
	{
public:
	CTGraphicsDrawer(CTestStep* aStep);
	~CTGraphicsDrawer();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
public:
	void ConstructL();
	void IndefiniteLoopL();

private:
	CTestBase *iTest;
	};

class CTGraphicsDrawerStep : public CTGraphicsStep
	{
public:
	CTGraphicsDrawerStep();
protected:	
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTGraphicsDrawerStep,"TGraphicsDrawer");


#endif
