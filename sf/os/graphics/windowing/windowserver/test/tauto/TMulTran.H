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

#ifndef __TMULTRAN_H__
#define __TMULTRAN_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"


class CTMultipleTrans : public CTWsGraphicsBase
	{
public:
	CTMultipleTrans(CTestStep* aStep);
	~CTMultipleTrans();
	void ConstructL();
	void CheckDisplay();
	void SimpleTest();
	void ChangeVisibility();
	void ChangeVisibility2();
	void OrdinalPos();
	void SizeChange();
	void MoveWin();
	void GroupWinL();
	void StartShadows();
	void ShadowsOnOff();
	void StartChildL();
	void StartChild2L();
	void FadeTranWin();
protected:	//Pure virtual function from MTestCase
	void RunTestCaseL(TInt aCurTestCase);
private:
	void RecreateWindowsL(CTWinBase *aParent=0);
	void ShadowRegion(TRegion& aRegion,TInt aWin);
	void AdvancedCheckRect();
	void SizeChange(TInt aJump);
	TSize GetSize(TInt aPos);


private:
	CTBlankWindow* iBackgroundWin;
	CTBackedUpWin* iCheckWin;
	CBitmap* iCheckBitmap;
	RPointerArray<CTransWindow> iWindows;
	CTransWindow* iTopLeft;
	CTransWindow* iTopRight;
	CTransWindow* iBotLeft;
	CTransWindow* iBotRight;
	RBlankWindow iBlankWin;
	TRgb iBlankWinCol;
	TPoint iBlankWinPos;
	TSize iBlankWinSize;
	TBool iBlankWinVis;
	TBool iShadows;
	TInt iMajorTest;
	TBool iOpacitySupported;
	};

class CTMultipleTransStep : public CTGraphicsStep
	{
public:
	CTMultipleTransStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTMultipleTransStep,"TMultipleTrans");

#endif
