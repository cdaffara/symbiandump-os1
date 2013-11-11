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

#ifndef __TCLIP_H__
#define __TCLIP_H__

#include "TGraphicsHarness.h"

#define testtext _L("Test text")
#define largetesttext _L("W")

class CTClipStep;

class CTClip : public CTGraphicsBase
	{
public:
	enum TGraphicsOps
		{
	EFirstGop,
		EPlot,
		EDrawLine,
		EDottedLine,
		EDashedLine,
		EWideLine,
		EDrawArc,
		EDottedArc,
		EDrawRect,
		EVerticalHatchRect,
		EForwardDiagonalHatchRect,
		EHorizontalHatchRect,
		ERearwardDiagonalHatchRect,
		ESquareCrossHatchRect,
		EDiamondCrossHatchRect,
		EVerticalHatchEllipse,
		EForwardDiagonalHatchEllipse,
		EHorizontalHatchEllipse,
		ERearwardDiagonalHatchEllipse,
		ESquareCrossHatchEllipse,
		EDiamondCrossHatchEllipse,
		EDottedRect,
		ECopyRect,
		EDrawEllipse,
		EDottedEllipse,
		EDrawRoundRect,
		EDottedRoundRect,
		EDrawPie,
		EDottedPie,
		EDrawPolygon,
		EDrawText,
		EBoxText,
		EDrawTextVertical,
		ELargeText,
		ELargeBoxText,
		ELargeTextVertical,
		EPaintRect,
		EBitBltContext,
		EBitBltBitmap,
		EBitBltMasked,
		EDrawBitmap,
		EBitmapLarge,
		EBitmapSmall,
		EBitBltMaskedBinary,
		EBitBltMaskedBinary64K,
		EBitBltMaskedBinary16MU,
		EBitBltMaskedAlpha,
		EBitBltMaskedAlpha64K,
		EBitBltMaskedAlpha16MU,
		EBitBltMaskedAlpha16MA,
		EBitBltMaskedBinary16MA,
	EGraphicsOps,
	ETerminator
		};
	enum TTestConsts
		{
		ERegions=10
		};
	enum TColors
		{
		EForeground=0,
		EBackground=0xff
		};
public:
	CTClip(CTestStep* aStep,
			TInt aTestAppNo,
			CFbsScreenDevice* aDev,
			CFbsBitGc* aCon,
			CFbsBitmap* aBmp,
			CFbsFont* aFont,
			CFbsFont* aLargeFont);
	~CTClip();
	void TestRectRgn();
	void TestHoriRgn();
	void TestVertRgn();
	void TestSimpleRgn();
	void TestComplexRgn();
	void ScanArea(TInt,TInt,TInt,TInt);
	void CheckInside(const TRect&,const TRect&);
	void Clear();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();
private:
	void DrawToScreen(TRegion& aClipReg,TRect aRect);
	void DrawToScreen(const TRect& aClipRect,TRect aRect);
	void DoDraw(TRect aRect,TBool aClipped);
	void Message(const TDesC& aMessage);
	TBool SetUpTest(TInt &aTestMode);
	inline CTClipStep* Step();
private:
	TInt iTestAppNo;
	CFbsScreenDevice* iDev;
	CFbsBitGc* iCon;
	TRect iClientRect;
	TInt iGopNum;
	TRect* iClipList;
	TInt iTestSection;
	CFbsBitmap* iBitmap;
	CFbsBitmap* iBitmap64K;
	CFbsBitmap* iBitmap16MU;
	CFbsBitmap* iBitmap16MA;
	CFbsBitmap* iBitmapMask;
	CFbsBitmap* iBitmapAlpha;
	CFbsFont* iFont;
	CFbsFont* iLargeFont;
	TSize iBmpSize;
	};

GLREF_C	void CreateTestEnvironment(CFbsScreenDevice*& aDev, CFbsBitGc*& aGc, 
								   CFbsFont*& aFont, CFbsFont*& aLargeFont);
GLREF_C	void DestroyTestEnvironment(CFbsScreenDevice* aDev, CFbsBitGc* aGc, 
								   CFbsFont* aFont, CFbsFont* aLargeFont);


class CTClipStep : public CTGraphicsStep
	{
public:
	CTClipStep();
	~CTClipStep();
	inline TBool IgnoreDiffs();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
private:
	TInt CreateScreenDevice(TDisplayMode aMode);
	void CreateTestEnvironment();
	void DestroyTestEnvironment();
protected:
	TInt iTestAppNo;
private:
	CFbsScreenDevice* iDev;
	CFbsBitGc* iGc;
	CFbsFont* iFont;
	CFbsFont* iLargeFont;
	CFbsBitmap* iBmp;
	TBool iIgnoreDiffs;
	};

class CTClip2Step : public CTClipStep
	{
public:
	CTClip2Step();
	};

_LIT(KTClipStep,"TClip");
_LIT(KTClip2Step,"TClip2");

#endif//__TCLIP_H_
