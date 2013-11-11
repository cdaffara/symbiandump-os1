
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

#ifndef __TBACKUPW_H__
#define __TBACKUPW_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include <bitstd.h>
#include "AUTO.H"
#include "TGraphicsHarness.h"

const TInt KLastSubtest = 13;

class CTBackedUpWindow : public CTWsGraphicsBase
	{
public:
	CTBackedUpWindow(CTestStep* aStep);
	~CTBackedUpWindow();
	void DoTestL();
	void DoSubTestL(TInt iState);
	void CheckWindow();
	void CheckWindow(CFbsBitmap *aBitmap);
	void Draw(TInt aDrawFunc, TAny *aParam);
	void Draw(TInt aDrawFunc, TAny *aParam, CBitmapContext *aGc1, CBitmapContext *aGc2);
	void DrawWithTwoGcsL();
	void ConstructL();
	void InvisVis();
	void WindowOnTop();
	void Resize();
	void ChildWindows();
	void DupBitmapTestL();
	void UpdateBitmapTestL();
	void OOML();
	void TestPanicsL();
	void doGraphicFunctionsL(CBitmapContext *gc,TBool aExtraDrawBitMap);
	void AllGraphicFunctionsL(RBlankWindow &aBlank,TBool aExtraDrawBitMap=ETrue);
	void AllGraphicFunctionsTestsL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TInt iSupState;
	TSize iSize;
	TPoint iCheckPos;
	TPoint iWinPos;
	RWindow iCheckWindow;
	RBackedUpWindow iBackedUpWindow;
	RBlankWindow iOomFrontWindow;
	CFbsBitmap iBitmap;
	CFbsBitGc *iBitGc;
	CFbsDevice *iBitmapDevice;
	CFbsBitmap* iTestBitmap;
	CFbsBitmap* iMaskBitmap;
	};

LOCAL_D const TPoint moveList[]={TPoint(1,0),TPoint(10,20),TPoint(-50,0),TPoint(30,-1),TPoint(-1000,20)};



class CTBackedUpWindowStep : public CTGraphicsStep
	{
public:
	CTBackedUpWindowStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTBackedUpWindowStep,"TBackedUpWindow");


#endif
