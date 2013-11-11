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
// Generate a list of rectangles to be used as parameters for graphics tests
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TRECTLIST_H__
#define __TRECTLIST_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTRectList : public CTWsGraphicsBase
	{
public:
	CTRectList(CTestStep* aStep);
	~CTRectList();
	void ConstructL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TPoint iOffset1;
	TPoint iOffset2;
	TSize iWinSize;
	};
	
class CTRectListStep : public CTGraphicsStep
	{
public:
	CTRectListStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTRectListStep,"TRectList");


#endif
