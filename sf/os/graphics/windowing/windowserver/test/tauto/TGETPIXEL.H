
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

#ifndef __TGETPIXEL_H__
#define __TGETPIXEL_H__

#include <e32std.h>
#include <w32std.h>
#include "../tlib/testbase.h"
#include "AUTO.H"
#include "TGraphicsHarness.h"

class CTGetPixel : public CTWsGraphicsBase
	{
public:
	CTGetPixel(CTestStep* aStep);
	~CTGetPixel();
	void DrawAndCheckLine(const TPoint &aPos,TInt aLen,TRgb aColor);
	void TestCheckRect();
	void GetPixelTest();
	void ConstructL();
	void TestPanicsL();
	void PaletteTestL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void DrawColorsL();
	void TestColors(CPalette* aPalette);
	void CheckPaletteL(CPalette*& aPalette);
	void ClearOutRedraws();
private:
	TSize iWinSize;
	TRgb *iRgbBuf;
	RWindow iWindow;
	};

class CTGetPixelStep : public CTGraphicsStep
	{
public:
	CTGetPixelStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTGetPixelStep,"TGetPixel");


#endif
