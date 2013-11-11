
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
// TBITMAP.H
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TALPHACHANNEL_H__
#define __TALPHACHANNEL_H__

#include "AUTO.H"
#include "../tlib/testbase.h"
#include "TGraphicsHarness.h"

class CTAlphaChannel;

/**
	Introduces a base class for transparent windows (both background and foreground)
*/
class CTWinAlpha : public CTWin
	{
public:
	void Draw(); //we have to overridden it to avoid panic in a based class
public:
	TInt iState;
	};

enum {ETransparencyFactor, ETransparencyAlpha, ENonTransparentAlpha};

/**
	Introduces a foreground transparent windows.
	In order to set any instance of class as transparent it has to be created with 
	ETransparencyAlpha parameter
*/
class CTWinAlphaForeground : public CTWin
	{
public:
	static CTWinAlphaForeground* NewL(CTAlphaChannel& aTest, TPoint aPos, TSize aSize, 
		CTWinBase* aParent, CWindowGc& aGc, TDisplayMode* aMode, TRgb aCol, TInt aTransparencyType = ENonTransparentAlpha);
	~CTWinAlphaForeground();
		
private:
	CTWinAlphaForeground(CTAlphaChannel& aTest);
	
public:
	void Draw();
	void SetBackgroundColor(TRgb aColor);
	void SetDisplayMode(TDisplayMode aDisplayMode);
	void StartAnimationL();
	void CreateBackedWindowL();
	inline void SetDrawOpaque(TBool aDrawOpaque = ETrue)
		{
		iDrawOpaque = aDrawOpaque;
		}
		
protected:
	void DrawTable(CBitmapContext& aGc) const;
	void DrawPrimitivesInCell(CBitmapContext& aGc, const TRect& aRect, 
								CFbsBitmap* aBitmap64K, CFbsBitmap* aBitmap16MA);
	void DrawTextInCell(CBitmapContext& aGc, const TRect& aRect);
	void PrepareForDrawingL();
	void CalculateTableMargin();
	void SetPoligonLocation(const TPoint &ptOffset);
	void CreateBackgroundBitmapL(const TDisplayMode& aDispMode);
	void CleanBackgroundBitmap();
	
protected:
	TInt iRows[4];
	TInt iCol; // width of the first column
	CFbsBitmap* iBitmapBackground;
	CFbsBitmapDevice* iBitmapDeviceBackground;
	CBitmapContext*  iBitmapContextBackground;
	TRgb iBackgroundColor;
	CArrayFix<TPoint>* iPolygon;
	TBuf<16> iTitle1;
	TBuf<16> iTitle2;
	TBool iDrawOpaque; // this value will set to CWindowGc before drawing, should have any effect on drawing with alpha channel
	
private:
	CTAlphaChannel& iTest;
	};

/** Test base class, which exersices drawing graphics operations with alpha channel */
class CTAlphaChannel : public CTWsGraphicsBase
	{
friend class CTWinAlphaForeground;	
public:
	CTAlphaChannel(CTestStep* aStep);
	~CTAlphaChannel();
	void ConstructL();
	void DrawTestWindowsNow(TBool aDrawBackgroundWin = EFalse);
	
	void SetWindow64K();
	void SetOpaqueWindow16MA();
	void SetTransparentWindow16MA();
	TInt SetWindow16MAWithMask();
	TInt SetWindow64KWithMask();
	CGraphicsContext::TDrawMode GetDrawMode(TInt aIndex) const;
	TRgb GetBrush(TInt aIndex) const;
	TRgb GetPen(TInt aIndex) const;

	// test functions
	void SetDisplayModeConfiguration(TInt aConfig);
	void SetPositionConfiguration(TInt aConfig);
	void SetColourConfiguration(TInt aConfig);
	void DoMoving();
	void DoMoveBehind();
	void DoInvisibility();
	void TestMoving();
	void TestMovingOverlapping();
	void TestFading();
	void TestChangingTransparencyFactor();
	void TestInvisibility();
	void TestMoveUnderneath();
	void TestMoveBehindInvisible();
	void TestRedrawBehind();
	void TestAnimationL();
	void TestBackedWindowL();
	void TestEffectSetOpaque();
	void TestChildWindowL();
	void TestMultipleChildrenL();
	void TestTransparentMovingUnderOpaqueL();
	void TestSetOrdinalPosition();
	
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void CreateForegroundWindowsL(const TSize& aSize, TDisplayMode aMode);
	void DestroyForegroundWindows();
	void DrawOffscreenBitmapsL(const TRgb& aPen, const TRgb& aBrush, 
										CGraphicsContext* aBitmapContext64K, CGraphicsContext* aBitmapContext16MA, 
										CFbsBitmap* aBitmap64K, CFbsBitmap* aBitmap16MA);
	void CalculateSizePrimitives(const TSize& aSize);
protected:
	TRgb iPenTable;
	CGraphicsContext::TBrushStyle	iBrushStyle;
	CGraphicsContext::TPenStyle iPenStyle;
	TSize iSizeForegroundWindow;
private:
	CTWinAlpha*	iBackgroundWindow;
	CTWinAlphaForeground*	iForegroundWindowBottom;
	CTWinAlphaForeground*	iForegroundWindowOpaque;
	CTWinAlphaForeground*	iForegroundWindowSemiTrans;
	CTWinAlphaForeground*	iForegroundWindowTrans;
	CTWinAlphaForeground*	iForegroundWindowTop;
	
	CArrayPtrFlat<CTWinAlphaForeground> iArrWindow;	
	
	
	CFbsBitmap* iBitmap64K_1;
	CFbsBitmap* iBitmap16MA_1;
	CFbsBitmap* iBitmap64K_2;
	CFbsBitmap* iBitmap16MA_2;
	CFbsBitmap* iBitmap64K_3;
	CFbsBitmap* iBitmap16MA_3;
	CFbsBitmap* iBitmapMask;
	CFbsBitmap* iBitmapGray256Mask;
	
	CFbsBitmapDevice* iBitmapDevice64K_1;
	CFbsBitmapDevice* iBitmapDevice16MA_1;
	CFbsBitmapDevice* iBitmapDevice64K_2;
	CFbsBitmapDevice* iBitmapDevice16MA_2;
	CFbsBitmapDevice* iBitmapDevice64K_3;
	CFbsBitmapDevice* iBitmapDevice16MA_3;

	CGraphicsContext* iBitmapContext64K_1;
	CGraphicsContext* iBitmapContext16MA_1;
	CGraphicsContext* iBitmapContext64K_2;
	CGraphicsContext* iBitmapContext16MA_2;
	CGraphicsContext* iBitmapContext64K_3;
	CGraphicsContext* iBitmapContext16MA_3;

	TBuf<256> iText;
	CFont* iFont; // for true type font, needs to exercise anti-aliasing 
	CFont* iFont1; // title font

	TSize iBitmapSize;
	TSize iRectangleSize;
	TSize iEllipseSize ;
	TSize iTriangleSize ;
	TInt iFirstCellWidth ;
	TInt iFirstCellHeight;
	TBool iDrawText; // if true output text, graphics primitives otherwise
	TBool iIsFading;
	TUint8 iBlackFading;
	TUint8 iWhiteFading;
	};
	
class CTAlphaChannelStep : public CTGraphicsStep
	{
public:
	CTAlphaChannelStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTAlphaChannelStep,"TAlphaChannel");


#endif
