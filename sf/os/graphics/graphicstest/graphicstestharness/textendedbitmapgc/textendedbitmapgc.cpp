// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "textendedbitmapgc.h"
#include <s32mem.h>

const TInt KNumBitmapDrawingTests = 10;
const TInt KNumBrushPatternTests = 8;
const TUid KUidExampleExtendedBitmap = {0x10285A78};

/** Constructor.
@param aCallBack Pointer to a valid implementation of MTestHarnessCallBack, to allow logging and 
test status requests to be passed back to the creator of a CTExtendedBitmapGc object.
@param aGc A graphics context to use for all drawing operations.
@param aDrawMode The mode to draw extended bitmaps in, either EDrawFlag or EDrawWhite.
@param aDisplayMode The display mode to use when creating extended bitmaps, this
must be one of EGray256, EColor64K, EColor16MU or EColor16MAP.
@return a pointer to a newly constructed CTExtendedBitmapGc object. 
@leave KErrNotSupported if an unsupported display mode is passed in aDisplayMode.
 */
EXPORT_C CTExtendedBitmapGc* CTExtendedBitmapGc::NewL(MTestHarnessCallBack* aCallBack, 
												CBitmapContext& aGc, 
												TDrawMode aDrawMode,
												TDisplayMode aDisplayMode)
	{	
	CTExtendedBitmapGc* ebgc = new(ELeave) CTExtendedBitmapGc(aCallBack, aGc, aDrawMode);
	CleanupStack::PushL(ebgc);
	ebgc->ConstructL(aDisplayMode);
	CleanupStack::Pop(ebgc);
	return ebgc;
	}

CTExtendedBitmapGc::CTExtendedBitmapGc(MTestHarnessCallBack* aCallBack, 
										CBitmapContext& aGc, 
										TDrawMode aDrawMode) :
iCallBack(aCallBack), iDrawMode(aDrawMode), iGc(aGc), iIsFbs(aGc.IsFbsBitGc())
	{
	}

void CTExtendedBitmapGc::ConstructL(TDisplayMode aDisplayMode)

	{	
	// Check that the display mode chosen is supported by the example rasterizer
	if ((aDisplayMode != EGray256) && (aDisplayMode != EColor64K) && (aDisplayMode != EColor16MU) && (aDisplayMode != EColor16MAP))
		{
		INFO_PRINTF2(_L("Failed to construct CTExtendedBitmapGc with unsupported display mode %d"), aDisplayMode);
		User::Leave(KErrNotSupported);
		}
	iDisplayMode = aDisplayMode; 
	
	// Get a font for use with the DrawText() and DrawTextVertical() tests
	_LIT(KFontName, "DejaVu Sans Condensed");
	TFontSpec fontSpec(KFontName, 75);
	fontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	iCallBack->TestTrue(KErrNone == static_cast<CBitmapDevice*>(iGc.Device())->GetNearestFontToDesignHeightInPixels(iFont, fontSpec));
	iGc.UseFont(iFont);
	}

EXPORT_C CTExtendedBitmapGc::~CTExtendedBitmapGc()
	{
	if(iFont)
		{
		iGc.DiscardFont();
		static_cast<CBitmapDevice*>(iGc.Device())->ReleaseFont(iFont);
		}
	}

/** Test case calling function used to test drawing of Extended Bitmaps on graphics contexts.
Utilised by GRAPHICS-BITGDI-0103 to test with CFbsBitGc, and by GRAPHICS-WSERV-0493 with CWindowGc.
@param aCaseNumber Case number to run
 */
EXPORT_C void CTExtendedBitmapGc::RunTestCaseL(TInt aCurTestCase)
	{
	--aCurTestCase;
	switch(aCurTestCase / KNumBitmapDrawingTests)
		{
	case 0:
		TestBitmapDrawingL(EFalse, EVerticalStripe, aCurTestCase % KNumBitmapDrawingTests);	
		break;
	case 1:
		TestBitmapDrawingL(EFalse, EHorizontalStripe, aCurTestCase % KNumBitmapDrawingTests);	
		break;
	case 2:
		TestBitmapDrawingL(ETrue, EVerticalStripe, aCurTestCase % KNumBitmapDrawingTests);	
		break;
	case 3:
		TestBitmapDrawingL(ETrue, EHorizontalStripe, aCurTestCase % KNumBitmapDrawingTests);	
		break;
	default:
		{
		TInt brushCase = aCurTestCase - (4 * KNumBitmapDrawingTests);
		if(brushCase < KNumBrushPatternTests)
			{
			TestBrushPatternL(brushCase);
			}
		else
			{
			// Finished tests
			iCallBack->TestComplete();
			}
		}
		}
	}

/** Test case function used to test drawing of Extended Bitmaps on graphics contexts.
Utilised by GRAPHICS-BITGDI-0103 to test with CFbsBitGc, and by GRAPHICS-WSERV-0493 with CWindowGc.
@param aTestRegionOfInterest ETrue if to set a region of interest before drawing, EFalse not to
@param aStripeStyle Direction to draw the flag stripes, when rasterizer is present
@param aCaseNumber Case number to run - between 0 and (KNumBitmapDrawingTests - 1)
 */
void CTExtendedBitmapGc::TestBitmapDrawingL(TBool aTestRegionOfInterest, TStripeStyle aStripeStyle, TInt aCaseNumber)
	{
	_LIT(KVertical, "vertical");
	_LIT(KHorizontal, "horizontal");	
	_LIT(KRegionNotSet, "not set");
	_LIT(KRegionSet, "set");
	const TPtrC KStripeStyle[2] = {KVertical(), KHorizontal()};
	const TPtrC KRegionStyle[2] = {KRegionNotSet(), KRegionSet()};
	const TInt KNumColors = 3;
	const TRgb KColors[KNumColors] = {TRgb(0,255,255), TRgb(255,0,255), TRgb(255,255,0)};

	const TRgb KMaskColors[KNumColors] = {KRgbBlack, KRgbWhite, KRgbBlack};
	const TRgb KAlphaColors[KNumColors] = {KRgbDarkGray, KRgbGray, KRgbDarkGray};
	const TStripeStyle KMaskBitmapStripeStyle = EVerticalStripe;
	const TStripeStyle KAlphaBitmapStripeStyle = EHorizontalStripe;

	TInt dataSize = sizeof(KColors)+sizeof(TUint8); // estimate the data size
	TUint8* data = new(ELeave) TUint8[dataSize];
	CleanupStack::PushL(data);
	
	const TSize scrSize = iGc.Device()->SizeInPixels();	

	// Decide what size the bitmaps should be
	TRect extendedRect;
	extendedRect.SetRect(10,10,(scrSize.iWidth>>1)-10,scrSize.iHeight-10);
	TRect standardRect(extendedRect);
	standardRect.Move(scrSize.iWidth>>1,0);
	const TSize sizeInPixels(extendedRect.Width(),extendedRect.Height());

	// Rects used for scaling tests
	TRect scaledExtendedRect = extendedRect;
	scaledExtendedRect.SetWidth(sizeInPixels.iWidth*3/4);
	scaledExtendedRect.SetHeight(sizeInPixels.iHeight*3/5);
	TRect scaledStandardRect = standardRect;
	scaledStandardRect.SetWidth(sizeInPixels.iWidth*3/4);
	scaledStandardRect.SetHeight(sizeInPixels.iHeight*3/5);

	// Positions for drawing extended and normal bitmaps
	TPoint bmpExPos(10,10);
	TPoint bmpNmlPos(10+(scrSize.iWidth>>1),10);

	// Use black and white vertical stripes as the data for the mask
	WriteExtendedBitmapInfoL(data, dataSize, KMaskColors, KMaskBitmapStripeStyle);

	// Create an extended bitmap to use as a mask
	CFbsBitmap* bmpMaskEx = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmpMaskEx);
	TInt err = bmpMaskEx->CreateExtendedBitmap(sizeInPixels, iDisplayMode, KUidExampleExtendedBitmap, data, dataSize);
	iCallBack->TestTrue(err == KErrNone);

	// Create a normal mask bitmap to use when testing the extended mask bitmap above
	CFbsBitmap* bmpMaskNml;
	CreateTestBitmapLC(bmpMaskNml, sizeInPixels, iDisplayMode, KRgbBlack, KRgbWhite, KRgbBlack, KMaskBitmapStripeStyle);

	// Use dark grey and grey horizontal stripes as the data for the alpha bitmap
	WriteExtendedBitmapInfoL(data, dataSize, KAlphaColors, KAlphaBitmapStripeStyle);

	// Create an EGray256 extended bitmap to use when alpha blending
	CFbsBitmap* bmpAlphaEx = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmpAlphaEx);
	err = bmpAlphaEx->CreateExtendedBitmap(sizeInPixels, EGray256, KUidExampleExtendedBitmap, data, dataSize);
	iCallBack->TestTrue(err == KErrNone);

	// Create a normal EGray256 alpha bitmap to use when testing the extended alpha bitmap above
	CFbsBitmap* bmpAlphaNml;
	CreateTestBitmapLC(bmpAlphaNml, sizeInPixels, EGray256, KRgbDarkGray, KRgbGray, KRgbDarkGray, KAlphaBitmapStripeStyle);

	// Create a clipping region and a clipping rect, making sure the shape of the region
	// that is set is the same on both sides of the screen
	const TInt KStripeSize = ((scrSize.iWidth>=300)&&(scrSize.iHeight>=100))?50:30;
	TRegionFix<8> clippingRegion;
	clippingRegion.AddRect(TRect(TPoint(0,0), TSize(scrSize.iWidth,KStripeSize)));
	clippingRegion.AddRect(TRect(TPoint(0,scrSize.iHeight>>1), TSize(scrSize.iWidth,KStripeSize)));
	clippingRegion.AddRect(TRect(TPoint(KStripeSize,0), TSize(KStripeSize<<1,scrSize.iHeight)));
	clippingRegion.AddRect(TRect(TPoint((scrSize.iWidth>>1)+KStripeSize,0), TSize(KStripeSize<<1,scrSize.iHeight)));
	iCallBack->TestTrue(clippingRegion.CheckError() == EFalse);
	TRect clippingRect(TPoint(0,KStripeSize>>1), TSize(scrSize.iWidth,scrSize.iHeight-KStripeSize));

	// Write the colours to be used in the extended bitmap to the extended bitmap data,
	// we run the tests one with horizontal stripes and once with vertical stripes
	WriteExtendedBitmapInfoL(data, dataSize, KColors, aStripeStyle);

	// Create an extended bitmap
	CFbsBitmap* bmpEx = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmpEx);
	err = bmpEx->CreateExtendedBitmap(sizeInPixels, iDisplayMode, KUidExampleExtendedBitmap, data, dataSize);
	iCallBack->TestTrue(err == KErrNone);

	// Create a normal bitmap to use when testing the extended bitmap above
	CFbsBitmap* bmpNml;
	CreateTestBitmapLC(bmpNml, sizeInPixels, iDisplayMode, KColors[0], KColors[1], KColors[2], aStripeStyle);

	// Clear to red
	iGc.SetBrushColor(KRgbRed);
	iGc.Clear();

	if (aTestRegionOfInterest)
		{
		iGc.SetClippingRegion(clippingRegion);
		iGc.SetClippingRect(clippingRect);
		}

	// Draw the extended bitmap(s) on the left side of the screen and the normal bitmap(s) on the
	// right side of the screen in each case
	switch (aCaseNumber)
		{
		case 0:
			{
			INFO_PRINTF3(_L("Stripe style %S. Region of interest %S"), &KStripeStyle[aStripeStyle], &KRegionStyle[aTestRegionOfInterest]);
			INFO_PRINTF1(_L("... BitBlt()"));
			iGc.BitBlt(bmpExPos, bmpEx);
			iGc.BitBlt(bmpNmlPos, bmpNml);
			break;
			}
		case 1:
			{
			INFO_PRINTF1(_L("... BitBltMasked()"));
			iGc.BitBltMasked(bmpExPos, bmpEx, sizeInPixels, bmpMaskEx, EFalse);
			iGc.BitBltMasked(bmpNmlPos, bmpNml, sizeInPixels, bmpMaskNml, EFalse);
			break;
			}
		case 2:
			{
			INFO_PRINTF1(_L("... DrawBitmap()"));
			iGc.DrawBitmap(extendedRect, bmpEx);
			iGc.DrawBitmap(standardRect, bmpNml);
			break;
			}
		case 3:
			{
			INFO_PRINTF1(_L("... DrawBitmap() with scaling"));
			iGc.DrawBitmap(scaledExtendedRect, bmpEx);
			iGc.DrawBitmap(scaledStandardRect, bmpNml);
			break;
			}
		case 4:
			{
			INFO_PRINTF1(_L("... DrawBitmapMasked()"));
			iGc.DrawBitmapMasked(extendedRect, bmpEx, sizeInPixels, bmpMaskEx, EFalse);
			iGc.DrawBitmapMasked(standardRect, bmpNml, sizeInPixels, bmpMaskNml, EFalse);
			break;
			}
		case 5:
			{
			INFO_PRINTF1(_L("... DrawBitmapMasked() with scaling"));
			iGc.DrawBitmapMasked(scaledExtendedRect, bmpEx, sizeInPixels, bmpMaskEx, EFalse);
			iGc.DrawBitmapMasked(scaledStandardRect, bmpNml, sizeInPixels, bmpMaskNml, EFalse);
			break;
			}
		case 6:
			{
			if(iIsFbs)
				{
				CFbsBitGc& fbsGc = static_cast<CFbsBitGc&>(iGc);
				INFO_PRINTF1(_L("... AlphaBlendBitmaps() - 1"));
				iCallBack->TestTrue(fbsGc.AlphaBlendBitmaps(bmpExPos, bmpEx, bmpMaskEx, sizeInPixels, TPoint(0,0), bmpAlphaEx, TPoint(0,0)) == KErrNone);
				iCallBack->TestTrue(fbsGc.AlphaBlendBitmaps(bmpNmlPos, bmpNml, bmpMaskNml, sizeInPixels, TPoint(0,0), bmpAlphaNml, TPoint(0,0)) == KErrNone);
				}
			break;
			}
		case 7:
			{
			if(iIsFbs)
				{
				CFbsBitGc& fbsGc = static_cast<CFbsBitGc&>(iGc);
				INFO_PRINTF1(_L("... AlphaBlendBitmaps() - 1 with offset"));
				iCallBack->TestTrue(fbsGc.AlphaBlendBitmaps(bmpExPos, bmpEx, bmpMaskEx, TSize(sizeInPixels.iWidth-5, sizeInPixels.iHeight-10), TPoint(5,10), bmpAlphaEx, TPoint(3,2)) == KErrNone);
				iCallBack->TestTrue(fbsGc.AlphaBlendBitmaps(bmpNmlPos, bmpNml, bmpMaskNml, TSize(sizeInPixels.iWidth-5, sizeInPixels.iHeight-10), TPoint(5,10), bmpAlphaNml, TPoint(3,2)) == KErrNone);
				}
			break;
			}
		case 8:
			{
			INFO_PRINTF1(_L("... AlphaBlendBitmaps() - 2"));
			iCallBack->TestTrue(iGc.AlphaBlendBitmaps(bmpExPos, bmpEx, sizeInPixels, bmpAlphaEx, TPoint(0,0)) == KErrNone);
			iCallBack->TestTrue(iGc.AlphaBlendBitmaps(bmpNmlPos, bmpNml, sizeInPixels, bmpAlphaNml, TPoint(0,0)) == KErrNone);
			break;
			}
		case 9:
			{
			INFO_PRINTF1(_L("... AlphaBlendBitmaps() - 2 with offset"));
			iCallBack->TestTrue(iGc.AlphaBlendBitmaps(bmpExPos, bmpEx, TSize(sizeInPixels.iWidth-16, sizeInPixels.iHeight-7), bmpAlphaEx, TPoint(16,7)) == KErrNone);
			iCallBack->TestTrue(iGc.AlphaBlendBitmaps(bmpNmlPos, bmpNml, TSize(sizeInPixels.iWidth-16, sizeInPixels.iHeight-7), bmpAlphaNml, TPoint(16,7)) == KErrNone);
			break;
			}
		default:
			{
			// Should not be reached
			iCallBack->TestTrue(EFalse);
			}
			break;
		}

	if (aTestRegionOfInterest)
		{
		// Cancel clipping rectangle and region if necessary
		iGc.CancelClippingRegion();
		iGc.CancelClippingRect();
		}
		
	CleanupStack::PopAndDestroy(7, data);
	}

/** Test case function used to test the use of Extended Bitmaps as brush patterns within graphics contexts.
Utilised by GRAPHICS-BITGDI-0103 to test with CFbsBitGc, and by GRAPHICS-WSERV-0493 with CWindowGc.
@param aCaseNumber Case number to run - between 0 and (KNumBrushPatternTests - 1)
 */
void CTExtendedBitmapGc::TestBrushPatternL(TInt aCaseNumber)
	{	
	const TInt KNumColors = 3;
	const TRgb KColors[KNumColors] = {TRgb(0,255,255), TRgb(255,0,255), TRgb(255,255,0)};
	const TSize KBrushSize(63,63);		
	TInt dataSize = sizeof(KColors)+sizeof(TUint8); // estimate the data size
	TUint8* data = new(ELeave) TUint8[dataSize];
	CleanupStack::PushL(data);	
	
	// Write the colours to be used in the extended bitmap to the data	
	WriteExtendedBitmapInfoL(data, dataSize, KColors, EHorizontalStripe);	
	
	// Create the extended bitmap to be used a a brush
	CFbsBitmap* extendedBmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(extendedBmp);
	TInt err = extendedBmp->CreateExtendedBitmap(KBrushSize, iDisplayMode, KUidExampleExtendedBitmap, data, dataSize);
	iCallBack->TestTrue(err == KErrNone);
	
	// Create the standard bitmap to also be used as a brush
	CFbsBitmap* standardBmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(standardBmp);
	err = standardBmp->Create(KBrushSize, iDisplayMode);
	iCallBack->TestTrue(err == KErrNone);
	
	// Create a bitmap device and a context so that the flag pattern can be drawn on
	// the standard bitmap
	CFbsBitmapDevice* bitDev = CFbsBitmapDevice::NewL(standardBmp);
	CleanupStack::PushL(bitDev);
	CFbsBitGc* bitGc;
	err = bitDev->CreateContext(bitGc);
	iCallBack->TestTrue(err == KErrNone);
	CleanupStack::PushL(bitGc);
	
	// Draw the flag pattern to the standard bitmap if the draw mode is EDrawFlag, 
	// leave as white otehrwise
	if (iDrawMode == EDrawFlag)
		{
		bitGc->SetPenStyle(CGraphicsContext::ENullPen);
		bitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		for (TInt i = 0; i < KNumColors; i++)
			{
			bitGc->SetBrushColor(KColors[i]);
			bitGc->DrawRect(TRect(0,(KBrushSize.iHeight/3)*i,KBrushSize.iWidth,(KBrushSize.iHeight/3)*(i+1)));	
			}
		}	
	CleanupStack::PopAndDestroy(2, bitDev);
	
	// Create a screen device and a context to draw to
	TSize scrSize = iGc.Device()->SizeInPixels();
		
	TRect extendedShapeSize;
	extendedShapeSize.SetRect(10,10,(scrSize.iWidth>>1)-10,scrSize.iHeight-10);
	TRect standardShapeSize(extendedShapeSize);
	standardShapeSize.Move(scrSize.iWidth>>1,0);

	TInt baseline = extendedShapeSize.Height() / 2 + iFont->AscentInPixels() / 2;
	
	iGc.SetBrushColor(KRgbBlue);
	iGc.Clear();
	iGc.SetBrushColor(KRgbWhite);		
	iGc.UseBrushPattern(extendedBmp);
	iGc.SetBrushStyle(CGraphicsContext::EPatternedBrush);		
	
	switch (aCaseNumber)
		{
		case 0: //DrawPie()
			{
			INFO_PRINTF1(_L("Testing brush patterns with an extended bitmap"));
			INFO_PRINTF1(_L("... DrawPie()"));
			iGc.DrawPie(extendedShapeSize, TPoint(extendedShapeSize.iTl.iX,0), TPoint(extendedShapeSize.iBr.iX,0));				
			iGc.UseBrushPattern(standardBmp);					
			iGc.DrawPie(standardShapeSize, TPoint(standardShapeSize.iTl.iX,0), TPoint(standardShapeSize.iBr.iX,0));				
			break;
			}
		case 1: // DrawRoundRect()
			{
			INFO_PRINTF1(_L("... DrawRoundRect()"));
			iGc.DrawRoundRect(extendedShapeSize, TSize(10,10));				
			iGc.UseBrushPattern(standardBmp);				
			iGc.DrawRoundRect(standardShapeSize, TSize(10,10));
			break;
			}
		case 2: // DrawPolygon() 
			{
			INFO_PRINTF1(_L("... DrawPolygon() 1"));
			const TInt KNumPoints = 3;
			CArrayFix<TPoint>* points = new(ELeave) CArrayFixFlat<TPoint>(KNumPoints);
			CleanupStack::PushL(points);
			points->AppendL(TPoint(extendedShapeSize.iTl.iX,extendedShapeSize.iTl.iY)); 
			points->AppendL(TPoint(extendedShapeSize.iBr.iX,extendedShapeSize.iBr.iY));
			points->AppendL(TPoint(extendedShapeSize.iTl.iX,extendedShapeSize.iBr.iY));
			iGc.DrawPolygon(points, CGraphicsContext::EWinding);
			iGc.UseBrushPattern(standardBmp);				
			iGc.SetOrigin(TPoint(standardShapeSize.iTl.iX-10,0));
			iGc.DrawPolygon(points, CGraphicsContext::EWinding);
			iGc.SetOrigin(TPoint(0,0));
			CleanupStack::PopAndDestroy(points);				
			break;
			}
		case 3: // DrawPolygon()
			{
			INFO_PRINTF1(_L("... DrawPolygon() 2"));
			const TInt KNumPoints = 3;
			TPoint* points = static_cast<TPoint*>(User::AllocL(sizeof(TPoint)*KNumPoints));
			CleanupStack::PushL(points);
			points[0] = TPoint(extendedShapeSize.iBr.iX,extendedShapeSize.iTl.iY); 
			points[1] = TPoint(extendedShapeSize.iBr.iX,extendedShapeSize.iBr.iY);
			points[2] = TPoint(extendedShapeSize.iTl.iX,extendedShapeSize.iBr.iY/2);
			iGc.DrawPolygon(points, KNumPoints, CGraphicsContext::EWinding);
			iGc.UseBrushPattern(standardBmp);				
			iGc.SetOrigin(TPoint(standardShapeSize.iTl.iX-10,0));
			iGc.DrawPolygon(points, KNumPoints, CGraphicsContext::EWinding);
			iGc.SetOrigin(TPoint(0,0));		
			CleanupStack::PopAndDestroy(points);
			break;
			}
		case 4: // DrawEllipse()
			{
			INFO_PRINTF1(_L("... DrawEllipse()"));
			iGc.DrawEllipse(extendedShapeSize);
			iGc.UseBrushPattern(standardBmp);				
			iGc.DrawEllipse(standardShapeSize);
			break;
			}
		case 5: // DrawRect()
			{
			INFO_PRINTF1(_L("... DrawRect()"));
			iGc.DrawRect(extendedShapeSize);				
			iGc.UseBrushPattern(standardBmp);				
			iGc.DrawRect(standardShapeSize);
			break;
			}
		case 6: // DrawText()
			{
			INFO_PRINTF1(_L("... DrawText()"));			
			iGc.DrawText(_L("HELLO"), extendedShapeSize, baseline);
			iGc.UseBrushPattern(standardBmp);
			iGc.DrawText(_L("HELLO"), standardShapeSize, baseline);
			break;
			}
		case 7: // DrawTextVertical()
			{				
			INFO_PRINTF1(_L("... DrawTextVertical()"));																		
			iGc.DrawTextVertical(_L("HELLO"), extendedShapeSize, baseline, EFalse);
			iGc.UseBrushPattern(standardBmp);
			iGc.DrawTextVertical(_L("HELLO"), standardShapeSize, baseline, EFalse);
			break;
			}
		default:
			{
			// Should not be reached
			iCallBack->TestTrue(EFalse);
			}
		}			
	CleanupStack::PopAndDestroy(3, data);
	}

/** Helper method for creating  and filling striped bitmaps equivalent to the extended bitmaps used in the test 
CTExtendedBitmap::TestBitmapDrawingL().
@param aBmpRet The newly created bitmap.
@param aSize The size of the bitmap to create.
@param aDisplayMode The display mode of the bitmap to create.
@param aColor1 The colour of the first stripe.
@param aColor2 The colour of the second stripe.
@param aColor3 The colour of the third stripe.
@param aStripeStyle Horizontal or vertical stripes, 0 for vertical stripe, 1 for horizontal stripes.
@return KErrNone if the bitmap was created successfully and returned in aBmpRet, one of the system
wide error codes otherwise. 
@post Leaves aBmpRet on the clean up stack if it is created successfully 
 */
void CTExtendedBitmapGc::CreateTestBitmapLC(CFbsBitmap*& aBmpRet,
										const TSize& aSize,
										TDisplayMode aDisplayMode, 
										const TRgb& aColor1, 
										const TRgb& aColor2, 
										const TRgb& aColor3,
										TStripeStyle aStripeStyle)
	{
	const TInt KNumColors = 3;
	TRgb colors[3] = {aColor1, aColor2, aColor3};
	
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp); // This gets left on the cleanup stack when the function returns
	TInt err = bmp->Create(aSize, aDisplayMode);	
	User::LeaveIfError(err);
		
	CFbsBitmapDevice* bmpDev = CFbsBitmapDevice::NewL(bmp);	
	CleanupStack::PushL(bmpDev);	
	CFbsBitGc* gc;
	err = bmpDev->CreateContext(gc);
	User::LeaveIfError(err);
	CleanupStack::PushL(gc);
	
	// Only draw stripes onto the test bitmap if the draw mode is EDrawFlag, otherwise
	// it should be left as cleared to white
	if (iDrawMode == EDrawFlag)
		{
		TInt outerStripeSize;												
		TInt middleStripeSize;							
		TRect stripeRect[3];
		
		if (aStripeStyle == EHorizontalStripe)
			{
			outerStripeSize = aSize.iHeight/3;												
			middleStripeSize = aSize.iHeight-(outerStripeSize<<1);							
			stripeRect[0] = TRect(TPoint(0,0), TSize(aSize.iWidth,outerStripeSize));
			stripeRect[1] = TRect(TPoint(0,outerStripeSize), TSize(aSize.iWidth,middleStripeSize));
			stripeRect[2] = TRect(TPoint(0,outerStripeSize+middleStripeSize), TSize(aSize.iWidth,outerStripeSize));
			}
		else if (aStripeStyle == EVerticalStripe)
			{
			outerStripeSize = aSize.iWidth/3;												
			middleStripeSize = aSize.iWidth-(outerStripeSize<<1);							
			stripeRect[0] = TRect(TPoint(0,0), TSize(outerStripeSize,aSize.iHeight));
			stripeRect[1] = TRect(TPoint(outerStripeSize,0), TSize(middleStripeSize,aSize.iHeight));
			stripeRect[2] = TRect(TPoint(outerStripeSize+middleStripeSize,0), TSize(outerStripeSize,aSize.iHeight));
			}
		else
			{
			INFO_PRINTF2(_L("Unsupported stripe style passed to CTExtendedBitmap::CreateTestBitmapLC: %d"), aStripeStyle);
			User::Leave(KErrArgument);
			}
		
		for (TInt i = 0; i < KNumColors; i++)
			{
			gc->SetBrushColor(colors[i]);
			gc->SetPenStyle(CGraphicsContext::ENullPen);
			gc->SetBrushStyle(CGraphicsContext::ESolidBrush);			
			gc->DrawRect(stripeRect[i]);
			}		
		}
	
	// Only pop the bitmap device and the gc, the created bitmap is left on the cleanup stack
	CleanupStack::PopAndDestroy(2, bmpDev); 	
	
	aBmpRet = bmp; 	
	}

/** Helper function for writing colour and stripe information used when creating an extended bitmap to
a write stream.
@param aData A pointer to a buffer that is large enough to write three TRgb colours and one TUint8 to.
@param aDataSize The size of buffer pointed to by aData, the actual size written is returned here
@param aColourArray A pointer to an array of the three colours to be written to the buffer
@param aHorizontalStripe ETrue to use horizontal stripes, EFalse for vertcial stripes
 */
EXPORT_C void CTExtendedBitmapGc::WriteExtendedBitmapInfoL(TUint8* aData,
												TInt& aDataSize,															
												const TRgb* aColorArray, 
												TStripeStyle aStripeStyle)
	{
	const TInt KNumColors = 3;

	// We expect an array of three colours
	if (sizeof(aColorArray) == (sizeof(TRgb)*KNumColors))
		{		
		User::Leave(KErrArgument);
		}
	
	RMemWriteStream ws;
	ws.Open(aData, aDataSize);
	CleanupClosePushL(ws);
	ws << aColorArray[0] << aColorArray[1] << aColorArray[2] << static_cast<TUint8>(aStripeStyle); // horizontal stripe
	aDataSize = ws.Sink()->TellL(MStreamBuf::EWrite).Offset(); // get the actual size written
	CleanupStack::PopAndDestroy(1, &ws);
	}
