// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This test application is used to test how the scaling works with CFbsBitGc drawing methods.
// 
//

#ifndef __TBITGDISCALING_H__
#define __TBITGDISCALING_H__

#include <bitdev.h>
#include "TGraphicsHarness.h"

class CTBitgdiScaling : public CTGraphicsBase
	{
public:
	CTBitgdiScaling(CTestStep* aStep);
	~CTBitgdiScaling();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();

	TInt CreateScreenDeviceAndContextL(TDisplayMode aDisplayMode,
									CFbsScreenDevice*& aScreenDevice,
									CFbsBitGc*& aGraphicsContext);
	void ChangeScreenDeviceL();
	void FontScalingL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void BitmapScalingL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void BitBltMaskedScalingL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void DrawingScalingL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void MapColors(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void BitmapScalingL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext, 
						   CFbsScreenDevice* aScreenDevice2, CFbsBitGc* aGraphicsContext2,
						   TBool aCallGetch = EFalse);
	void RunTests2L();
	void DrawTestText(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext, CFont* aFont);
	void DrawArc(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void DrawPie(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void DrawRoundRect(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void DrawPolyLineL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void DrawPolyLineNoEndPointL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void DrawPolygonL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void DrawEllipse(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void DrawLine(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void DrawRect(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void ShadowFade(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	
	void TestDrawBitmapL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestClear(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestBitbltL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestArcPie(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestPolyL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestEllipseLineRect(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestText(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext, CFont*);
	void TestTextVertical(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext, CFont*);
	void TestMapColors(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestShadowFade(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestMove(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestEllipseRect(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestDrawBitmapNegL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
	void TestMapColorsZero(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext);
        					   
private:
	CFbsScreenDevice* iScreenDevice;
	CFbsBitGc* iGraphicsContext;
	CFbsScreenDevice* iScreenDevice2;
	CFbsBitGc* iGraphicsContext2;
	};

class CTBitgdiScalingStep : public CTGraphicsStep
	{
public:
	CTBitgdiScalingStep();
	inline TDisplayMode DisplayMode() const;
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
private:
	TDisplayMode GetDisplayModeL();
private:
	TDisplayMode iDisplayMode;	
	};


inline TDisplayMode CTBitgdiScalingStep::DisplayMode() const
	{
	return iDisplayMode;
	}


_LIT(KTBitgdiScalingStep,"TBitgdiScaling");


#endif
