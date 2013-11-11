// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TGDI_H__
#define __TGDI_H__

#include <test/ttmsgraphicsstep.h>
#ifndef __E32STD_H__
#include <e32std.h>
#endif
#ifndef __E32TEST_H__
#include <e32test.h>
#endif
#ifndef __GDI_H__
#include <gdi.h>
#endif
#ifndef __FBS_H__
#include <fbs.h>
#endif
#ifndef __BITMAP_H__
#include <bitmap.h>
#endif
#ifndef __BITSTD_H__
#include <bitstd.h>
#endif

const TInt KFailureSectionNameBufferLength = 64;

class TestGdi : public CTTMSGraphicsStep
	{
public:
	IMPORT_C TestGdi();
	IMPORT_C void Construct(CBitmapDevice* aDevice,CFbsBitGc* aContext,TDes& aFailureSection);
	IMPORT_C void ConstructL(CBitmapDevice* aDevice,CFbsBitGc* aContext,TDes& aFailureSection);
	IMPORT_C TBool Test();
private:
	void DoTestL();
	void CheckL(TBool aValue);
	void Clear();
	void ScanAreaL(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
	void CheckClearL();
	void CheckPlotL(const TPoint& aPoint);
	void CheckRectInsideL(const TRect& aRect);
	void CheckRectOutsideL(const TRect& aRect);
	void CheckQuadInsideL(const TRect& aRect);
	void CheckQuadOutsideL(const TRect& aRect);
	void CheckMaskedResultsL(CFbsBitmap* aBgBmp,CFbsBitmap* aSrcBmp,CFbsBitmap* aMaskBmp,TRect& aTarRect,TBool aInvertMask,TBool aAlphaBlend=EFalse,TBool aSemiTransparentMask=EFalse);
	void DoModeL();
	void DoColorL();
	void DoLineL();
	void DoWideLineL();
	void DoArcL();
	void DoRectL();
	void DoEllipseL();
	void DoPieL();
	void DoRoundRectL();
	void DoPolygonArrayL();
	void DoPolygonPtrL();
	void DoPolygonPtr0();
	void DoText();
	void DoPaintL();
	void DoCopyL();
	void DoBltL();
	void DoBltMaskedL();
	void DoShadowL();
	void DoFadeL();
	void DoDrawBitmapMaskedL(TBool aColorMask);
	void DoBltCompressedL();
	void DoDrawBitmapSemiTransparentMaskedL();
	void FillBitmap(CFbsBitmap* aBitmap, TRgb aRgb);
	TVerdict doTestStepL(){return TestStepResult();};
	void DoBoxTextVertical();
private:
	CBitmapDevice* iDev;
	CFbsBitGc* iCon;
	CFbsBitmap iBitmap;
	CFbsBitmap iBitmap2;
	TInt iBitmapWidth;
	TInt iBitmapHeight;
	TSize iSize;
	TRect iRect;
	TRect iZeroRect;
	TRect iUnitRect;
	TRect iUnNormRect;
	TRect iLargeRect;
	TRect iCentredRect;
	TRect iOffScreenRects[8];
	TDes* iSection;
	};

#endif
