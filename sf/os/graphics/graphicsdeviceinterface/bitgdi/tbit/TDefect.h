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
//

//This file contains test bitmap file names for TDefect test app.
#ifndef __TDEFECT_H__
#define __TDEFECT_H__


#include <bitdev.h>
#include "TGraphicsHarness.h"

//The next enum is used by 16 bpp and 24 bpp bitmap compression tests.
typedef enum 
	{
	E16BppBmpType,
	E24BppBmpType,
	//
	//New enum members - insert them here
	//
	ELastBmpType
	} TBitmapType;

//The main test class. Add your test methods here.
class CTDefect : public CTGraphicsBase
	{
public:
	typedef enum {EScalingAllowed, EScalingNotAllowed} TAllowScaling;
public:
	CTDefect(CTestStep* aStep);
	~CTDefect();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();
private:
	void CreateScreenDeviceL(TAllowScaling aScaling = EScalingAllowed);
	TInt CreateScreenDeviceL(TDisplayMode aDisplayMode, TAllowScaling aScaling = EScalingAllowed);
	void DEF014364L();
	void DEF017758();
	void DEF017758L();
	void DEF023605L();
	void BitmapCompressionL(TBitmapType aBitmapType);
	void NewRscFormatL();
	void INC031920L(TBitmapType aBitmapType, TBool aRomBitmap);
	void Test8bitBmpComressionL();
	void Test12bitBmpComressionL();
	void ZeroSizeBitmap();
	void DEF034134L();
	void TestTilingHorizontalAndVerticalL();
	void DrawPieBorderlineCasesL();
	void INC037380L();
	void INC037474L();
	void INC070043L();
	CFbsBitGcBitmap* LoadBitmapLC(const TDesC& aFileName);
	void TestSettingDisplayModeL();
	void TestSettingDisplayMode2L();
	void SetAndCheckDisplayMode(TDisplayMode aMode, const TSize& aInitialSize);
	void CheckScanLine();
	void TestSwappingWidthAndHeightL();
	void INC037370L();
	void DEF038774L();
  void TestBitBltOperationsOn16muUndefinedByteL();
	void ExternalizeInternalizeCFbsBitGCExtraData();
	
	void DeleteBitmap();
	void DeleteBitmapDevice();
	void DeleteScreenDevice();
	void DeleteGraphicsContext();

	void DisplayBitmapL(const TDesC& aFileName, TInt aBitmapNo, TInt aBitmapOffset, const TDesC& aText, const TRect&);
	void DisplayBitmap(const TDesC& aText, const TRect&);
	void DisplayBitmap(const TRect&, TDisplayMode aScreenMode, TDisplayMode aBmpMode);

	void CreateFontL();
	void DestroyFont();

	void DrawPie(const TDesC& aText, const TRect& aRc, const TPoint& aPtStart, const TPoint& aPtEnd);
	void CreateBitmapL(const TSize& aSize, TDisplayMode aMode);
	void INC042156L();
	void TestOom();
	void BitmapsSwappingTest();
	void INC093055L();
	void PDEF103809L();
	void CheckResults(CFbsDevice* aDevice, TInt aWidth, TInt aHeight, TInt aPattern, TInt aBarHeight);
	void InitialiseSourceAndMaskLC(CFbsBitmap*& aSrc, TDisplayMode aSrcDispMode, CFbsBitmap*& aMask, TDisplayMode aMaskDispMode, TSize aPicSize, TInt aNumMaskBars, TInt aMaskBarHeight, TSize* aMaskSize=NULL);
	void TestDoBltMaskedL(TDisplayMode aSrcDispMode, TDisplayMode aMaskDispMode, TDisplayMode aDeviceDispMode, TSize aPicSize, TBool aUseScreenDevice, TBool aInvertMask, TSize* aMaskSize=NULL);
	TUint32 Color(const TRgb& aColor, const TDisplayMode& aDisplayMode);
	void TestDoBitBltMaskedSizeL();	
	void TestBitBltMaskedEColor16MUL();
	void CheckBitmapContents(CFbsDevice* aDevice, TInt aWidth, TInt aHeight, TUint32 aExpectedValue);
	TUint32 AlphaBlendWithDestAlphaFF(const TUint32 aSrcPixel, const TUint8 aMask, TUint32 aDestPixel);
	void TestClippedScaledBitmapL();
	void TestDrawBitmapTwipsNotSet();
	void TestDoDrawBitmapMaskedL(TDisplayMode aSrcDispMode, TDisplayMode aMaskDispMode, TDisplayMode aDeviceDispMode, TSize aPicSize, TSize* aMaskSize=NULL);
	void TestDrawBitmapMaskedL();

private:
	CFbsScreenDevice* iDevice;
	CFbsBitmapDevice* iBmpDevice;
	CFbsBitGc* iGc;
	CFbsBitmap* iBitmap;
	TSize iSize;
	TDisplayMode iCurrentMode;	
	TBool iSkipRomBitmapTests;
	};

class CTDefectStep : public CTGraphicsStep
	{
public:
	CTDefectStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTDefectStep,"TDefect");


#endif//__TDEFECT_H__

