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

#ifndef TW32CMDBUF_H
#define TW32CMDBUF_H

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

_LIT(KTW32CmdBufSecondThread, "TW32CmdBufSecondThread");

enum TTestFunctionIndex
	{
	ECallDrawBitmap1,
	ECallDrawBitmap2,
	ECallDrawBitmap3,
	ECallDrawBitmapMasked,
	ECallUseBrushPattern,
	ECallBitBlt1,
	ECallBitBlt2,
	ECallBitBltMasked,
	ECallAlphaBlendBitmaps,
	};

class CTW32CmdBuf : public CTGraphicsBase
	{
public:
	CTW32CmdBuf(CTestStep* aStep);
	~CTW32CmdBuf();
	void ConstructL();

protected:
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void DoCmdBufTestsL();
	void CreateSecondThreadAndDoTestL(TTestFunctionIndex aFunctionIndex);
	static TInt TestCmdBufFunction(TAny* aInfo);
	static TInt DoTestCmdBufFunctionL(TTestFunctionIndex aFunctionIndex);
	};

class CTW32CmdBufStep : public CTGraphicsStep
	{
public:
	CTW32CmdBufStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTW32CmdBufStep,"TW32CmdBuf");

#endif
