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

#include "tdirectgdiresource.h"
#include <s32mem.h>
#include <graphics/directgditypes.h>
#include <graphics/directgdicontext.h>

_LIT(KClip, "Clip");
_LIT(KWriteAlpha, "WriteAlpha");

const DirectGdi::TGraphicsRotation TestRot[] =
  	{
  	DirectGdi::EGraphicsRotationNone,
  	DirectGdi::EGraphicsRotation90,
  	DirectGdi::EGraphicsRotation180,
  	DirectGdi::EGraphicsRotation270
  	};

CTDirectGdiResource::CTDirectGdiResource() :
	CActive(CActive::EPriorityStandard),
	iCurrentState(EStartup)
	{
	SetTestStepName(KTDirectGdiResourceStep);
	}

CTDirectGdiResource::~CTDirectGdiResource()
	{
	Cancel();
	DestroyCommonResource();
	}

/**
Writes output image to file. Append dest + src pixel format to the given file name.
*/
void CTDirectGdiResource::SaveOutput(const TDesC& aName)
	{
	TBuf<KFileNameLength> postfix;
	postfix.Append(KSourceString);
	postfix.Append(KSeparator);
	postfix.Append(TDisplayModeMapping::ConvertPixelFormatToShortPixelFormatString(
				iTestParams.iSourceResourcePixelFormat));

	TESTNOERROR(WriteTargetOutput(iTestParams, aName, &postfix));
	}

/**
Check to see if the current source resource pixel format contains alpha.
@return ETrue if the current source resource pixel format contais alpha, EFalse otherwise.
*/
TBool CTDirectGdiResource::SourceResourceHasAlpha() const
	{
	return iTestParams.iSourceResourcePixelFormat==EUidPixelFormatARGB_8888 ||
		iTestParams.iSourceResourcePixelFormat==EUidPixelFormatARGB_8888_PRE;
	}

/**
Check to see if tests should run that only need to be run with one pixel format,
for example tests to check that clipping regions are working correctly. Tests that are
only run once are run when the target pixel format and the source pixel format are
both EUidPixelFormatARGB_8888_PRE.
@return ETrue if both the current source and target pixel formats are EUidPixelFormatARGB_8888_PRE.
*/
TBool CTDirectGdiResource::OneTimeTestEnabled() const
	{
	return iTestParams.iTargetPixelFormat==iTestParams.iSourceResourcePixelFormat &&
		iTestParams.iTargetPixelFormat==EUidPixelFormatARGB_8888_PRE;
	}

/**
Draws an image with 4 different orientation using given position as the
origin e.g
A B
 o
D C
*/
void CTDirectGdiResource::DrawTestPattern1(const TPoint& aPos)
	{
	const TInt w = iImgSz.iWidth;
	const TInt h = iImgSz.iHeight;

	iGc->DrawResource(TPoint(aPos.iX-w, aPos.iY-h), iImgSrc, DirectGdi::EGraphicsRotationNone);
	iGc->DrawResource(TPoint(aPos.iX, aPos.iY-w), iImgSrc, DirectGdi::EGraphicsRotation90);
	iGc->DrawResource(aPos, iImgSrc, DirectGdi::EGraphicsRotation180);
	iGc->DrawResource(TPoint(aPos.iX-h, aPos.iY), iImgSrc, DirectGdi::EGraphicsRotation270);
	}

/**
Draws two drawables to fit dest rect in the following pattern:
AB
*/
void CTDirectGdiResource::DrawTestPattern1(const TRect& aDestRect,
			const TDrawableSourceAndEquivRotatedBmps& aDrawable1, const TDrawableSourceAndEquivRotatedBmps& aDrawable2)
	{
	const TInt dw = aDestRect.Width();
	const TInt dh = aDestRect.Height();
	const TInt xu = dw/2;
	const TInt yu = dh;
	
	iGc->DrawResource(TRect(aDestRect.iTl, TSize(xu, yu)), aDrawable1, KNullDesC8);
	iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+xu, aDestRect.iTl.iY), TSize(xu, yu)),
			aDrawable2, KNullDesC8);
	}

/**
Draws two drawables to fit dest rect in the following pattern:
A
B
*/
void CTDirectGdiResource::DrawTestPattern2(const TRect& aDestRect,
			const TDrawableSourceAndEquivRotatedBmps& aDrawable1, const TDrawableSourceAndEquivRotatedBmps& aDrawable2)
	{
	const TInt dw = aDestRect.Width();
	const TInt dh = aDestRect.Height();
	const TInt xu = dw;
	const TInt yu = dh/2;
	
	iGc->DrawResource(TRect(aDestRect.iTl, TSize(xu, yu)), aDrawable1, KNullDesC8);
	iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX, aDestRect.iTl.iY+yu), TSize(xu, yu)),
			aDrawable2, KNullDesC8);
	}

/**
Draws image rotated to fit dest rect in the following pattern:
AB
CC
DD
*/
void CTDirectGdiResource::DrawTestPattern1(const TRect& aDestRect, const TRect* aSrcRect)
	{
	const TInt dw = aDestRect.Width();
	const TInt dh = aDestRect.Height();
	const TInt xu = dw/2;
	const TInt yu = dh/5;
	
	if (aSrcRect)
		{
		// use DrawResource(destRect, img, srcRect, rot) API
		iGc->DrawResource(TRect(aDestRect.iTl, TSize(xu, yu)), iImgSrc, *aSrcRect, DirectGdi::EGraphicsRotationNone);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+xu, aDestRect.iTl.iY), TSize(xu, yu)), iImgSrc,
				*aSrcRect, DirectGdi::EGraphicsRotation180);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX, aDestRect.iTl.iY+yu), TSize(dw, 2*yu)), iImgSrc,
				*aSrcRect, DirectGdi::EGraphicsRotationNone);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX, aDestRect.iTl.iY+3*yu), TSize(dw, 2*yu)), iImgSrc,
				*aSrcRect, DirectGdi::EGraphicsRotation180);
		}
	else
		{
		// use DrawResource(destRect, img, rot) API
		iGc->DrawResource(TRect(aDestRect.iTl, TSize(xu, yu)), iImgSrc, DirectGdi::EGraphicsRotationNone);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+xu, aDestRect.iTl.iY), TSize(xu, yu)), iImgSrc,
				DirectGdi::EGraphicsRotation180);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX, aDestRect.iTl.iY+yu), TSize(dw, 2*yu)), iImgSrc,
				DirectGdi::EGraphicsRotationNone);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX, aDestRect.iTl.iY+3*yu), TSize(dw, 2*yu)), iImgSrc,
				DirectGdi::EGraphicsRotation180);
		}
	}

/**
Draws image rotated to fit dest rect in the following pattern:
ACD
BCD
*/
void CTDirectGdiResource::DrawTestPattern2(const TRect& aDestRect, const TRect* aSrcRect)
	{
	const TInt dw = aDestRect.Width();
	const TInt dh = aDestRect.Height();
	const TInt xu = dw/5;
	const TInt yu = dh/2;

	if (aSrcRect)
		{
		// use DrawResource(destRect, img, srcRect, rot) API
		iGc->DrawResource(TRect(aDestRect.iTl, TSize(xu, yu)), iImgSrc, *aSrcRect, DirectGdi::EGraphicsRotation90);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX, aDestRect.iTl.iY+yu), TSize(xu, yu)), iImgSrc,
				*aSrcRect, DirectGdi::EGraphicsRotation270);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+xu, aDestRect.iTl.iY), TSize(2*xu, dh)), iImgSrc,
				*aSrcRect, DirectGdi::EGraphicsRotation90);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+3*xu, aDestRect.iTl.iY), TSize(2*xu, dh)), iImgSrc,
				*aSrcRect, DirectGdi::EGraphicsRotation270);
		}
	else
		{
		// use DrawResource(destRect, img, rot) API
		iGc->DrawResource(TRect(aDestRect.iTl, TSize(xu, yu)), iImgSrc, DirectGdi::EGraphicsRotation90);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX, aDestRect.iTl.iY+yu), TSize(xu, yu)), iImgSrc,
				DirectGdi::EGraphicsRotation270);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+xu, aDestRect.iTl.iY), TSize(2*xu, dh)), iImgSrc,
				DirectGdi::EGraphicsRotation90);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+3*xu, aDestRect.iTl.iY), TSize(2*xu, dh)), iImgSrc,
				DirectGdi::EGraphicsRotation270);
		}
	}

/**
Draws image rotated to fit dest rect in the following pattern:
AB
CD
*/
void CTDirectGdiResource::DrawTestPattern3(const TRect& aDestRect, const TRect* aSrcRect)
	{
	TInt dw = aDestRect.Width();
	TInt dh = aDestRect.Height();
	TInt xu = dw/2;
	TInt yu = dh/2;

	if (aSrcRect)
		{
		// use DrawResource(destRect, img, srcRect, rot) API
		iGc->DrawResource(TRect(aDestRect.iTl, TSize(xu, yu)), iImgSrc, *aSrcRect, DirectGdi::EGraphicsRotationNone);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+xu, aDestRect.iTl.iY), TSize(xu, yu)), iImgSrc,
				*aSrcRect, DirectGdi::EGraphicsRotation180);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX, aDestRect.iTl.iY+yu), TSize(xu, yu)), iImgSrc,
				*aSrcRect, DirectGdi::EGraphicsRotation90);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+xu, aDestRect.iTl.iY+yu), TSize(xu, yu)), iImgSrc,
				*aSrcRect, DirectGdi::EGraphicsRotation270);
		}
	else
		{
		// use DrawResource(destRect, img, rot) API
		iGc->DrawResource(TRect(aDestRect.iTl, TSize(xu, yu)), iImgSrc, DirectGdi::EGraphicsRotationNone);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+xu, aDestRect.iTl.iY), TSize(xu, yu)), iImgSrc,
				DirectGdi::EGraphicsRotation180);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX, aDestRect.iTl.iY+yu), TSize(xu, yu)), iImgSrc,
				DirectGdi::EGraphicsRotation90);
		iGc->DrawResource(TRect(TPoint(aDestRect.iTl.iX+xu, aDestRect.iTl.iY+yu), TSize(xu, yu)), iImgSrc,
				DirectGdi::EGraphicsRotation270);
		}
	}

/**
@SYMTestCaseID			
	GRAPHICS-DIRECTGDI-DRAWRESOURCE-0001

@SYMTestPriority		
	Critical
	
@SYMPREQ				
	PREQ39

@SYMREQ
	REQ9178 
	REQ9200 
	REQ9201 
	REQ9202 
	REQ9203 
	REQ9222 
	REQ9228 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestStatus			
	Implemented
	
@SYMTestCaseDesc		
	Draws drawable resources created from RSgDrawable and RSgImage objects.
	
@SYMTestActions			
	Draw drawble resources with the various combination of parameters:
	-with and without clipping region
	-drawable from RSgDrawable and RSgImage
	-in various destination positions and scaling factor
	-using write or blend
		
@SYMTestExpectedResults	
	Resources are drawn on the given position, scaled to fit the destination
	rectangle, clipped to current clipping region (if set).
*/
void CTDirectGdiResource::TestDrawResource_ImageAsDrawableL()
	{
	_LIT(KTestName, "DrawResource_ImageAsDrawable");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	TBuf<KFileNameLength> tag;
	tag.Append(KTestName);

	ResetGc();
	if (iEnableClipRegion)
		{
		iGc->SetClippingRegion(iClip);
		tag.Append(KClip);
		}

	if (iUseWriteAlpha)
		{
		iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
		tag.Append(KWriteAlpha);
		}

	DrawTestPattern1(TRect(0,0,100,20), iDwbSrc[0], iDwbSrc[1]);
	DrawTestPattern2(TRect(0,20,100,100), iDwbSrc[0], iDwbSrc[1]);
	DrawTestPattern1(TRect(0,100,100,200), iDwbSrc[0], iDwbSrc[1]);
	DrawTestPattern2(TRect(100,0,200,200), iDwbSrc[0], iDwbSrc[1]);

	if (iEnableClipRegion)
		{
		iGc->ResetClippingRegion();
		iGc->SetDrawMode(DirectGdi::EDrawModePEN);
		}

	SaveOutput(tag);
	}

/**
@SYMTestCaseID			
	GRAPHICS-DIRECTGDI-DRAWRESOURCE-0002
	
@SYMTestPriority		
	Critical
	
@SYMPREQ				
	PREQ39

@SYMREQ
	REQ9178 
	REQ9200 
	REQ9201 
	REQ9202 
	REQ9203 
	REQ9222 
	REQ9228 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestStatus			
	Implemented
	
@SYMTestCaseDesc		
	Draw an image resource created from RSgImage object.
	
@SYMTestActions			
	Draw an image resource with the various combination of parameters:
	-with and without clipping region
	-in various destination positions on/off target
	-using write or blend
	-no scaling is performed in this test
	
@SYMTestExpectedResults	
	Resources are drawn on the given position, unscaled, rotated and
	clipped to current clipping region (if set).
*/
void CTDirectGdiResource::TestDrawResource_PosL()
	{
	_LIT(KTestName, "DrawResource_Pos");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	TBuf<KFileNameLength> tag;
	tag.Append(KTestName);
	
	ResetGc();

	if (iEnableClipRegion)
		{
		iGc->SetClippingRegion(iClip);
		tag.Append(KClip);
		}

	if (iUseWriteAlpha)
		{
		iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
		tag.Append(KWriteAlpha);
		}

	DrawTestPattern1(TPoint(0,0));
	DrawTestPattern1(TPoint(100,0));
	DrawTestPattern1(TPoint(200,0));

	DrawTestPattern1(TPoint(50,50));
	DrawTestPattern1(TPoint(150,50));

	DrawTestPattern1(TPoint(0,100));
	DrawTestPattern1(TPoint(100,100));
	DrawTestPattern1(TPoint(200,100));

	DrawTestPattern1(TPoint(50,150));
	DrawTestPattern1(TPoint(150,150));

	DrawTestPattern1(TPoint(0,200));
	DrawTestPattern1(TPoint(100,200));
	DrawTestPattern1(TPoint(200,200));

	if (iEnableClipRegion)
		{
		iGc->ResetClippingRegion();
		iGc->SetDrawMode(DirectGdi::EDrawModePEN);
		}

	SaveOutput(tag);
	}

/**
@SYMTestCaseID			
	GRAPHICS-DIRECTGDI-DRAWRESOURCE-0003
	
@SYMTestPriority		
	Critical
	
@SYMPREQ				
	PREQ39

@SYMREQ
	REQ9178 
	REQ9200 
	REQ9201 
	REQ9202 
	REQ9203 
	REQ9222 
	REQ9228 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestStatus			
	Implemented
	
@SYMTestCaseDesc		
	Draw a scaled image resource created from RSgImage object.
	
@SYMTestActions			
	Draw a scaled image resource with the various combination of parameters:
	-with and without clipping region
	-in various destination positions and scaling factor
	-using write or blend
	-the destination rectangle that the image is drawn to scaled
	
@SYMTestExpectedResults	
	Resources are drawn on the given position, scaled to fit the
	destination rectangle, rotated and clipped to current clipping region (if set).
*/
void CTDirectGdiResource::TestDrawResource_DestRectL()
	{
	_LIT(KTestName, "DrawResource_DestRect");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	TBuf<KFileNameLength> tag;
	tag.Append(KTestName);

	ResetGc();

	if (iEnableClipRegion)
		{
		iGc->SetClippingRegion(iClip);
		tag.Append(KClip);
		}

	if (iUseWriteAlpha)
		{
		iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
		tag.Append(KWriteAlpha);
		}

	DrawTestPattern1(TRect(0,0,100,100));
	DrawTestPattern2(TRect(100,0,200,100));
	DrawTestPattern3(TRect(0,100,100,200));
	DrawTestPattern1(TRect(100,100,150,150));
	DrawTestPattern2(TRect(150,100,200,150));
	DrawTestPattern3(TRect(100,150,200,200));

	if (iEnableClipRegion)
		{
		iGc->ResetClippingRegion();
		iGc->SetDrawMode(DirectGdi::EDrawModePEN);		
		}

	SaveOutput(tag);
	}

/**
@SYMTestCaseID			
	GRAPHICS-DIRECTGDI-DRAWRESOURCE-0004
	
@SYMTestPriority		
	Critical
	
@SYMPREQ				
	PREQ39

@SYMREQ
	REQ9178 
	REQ9200 
	REQ9201 
	REQ9202 
	REQ9203 
	REQ9222 
	REQ9228 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestStatus			
	Implemented
	
@SYMTestCaseDesc		
	Draw a scaled portion of an image resource created from RSgImage object.
	
@SYMTestActions			
	Draw a scaled portion of an image resource with the various combination of parameters:
	-with and without clipping region
	-various destination positions and size
	-using write or blend
	-only a portion of the image resource is drawn, it is drawn scaled
	
@SYMTestExpectedResults	
	Resources are drawn on the given position, scaled to fit the
	destination rectangle, rotated and clipped to current clipping region (if set).
*/
void CTDirectGdiResource::TestDrawResource_DestRectSrcRectL()
	{
	_LIT(KTestName, "DrawResource_DestRectSrcRect");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	TBuf<KFileNameLength> tag;
	tag.Append(KTestName);

	ResetGc();
	if (iEnableClipRegion)
		{
		iGc->SetClippingRegion(iClip);
		tag.Append(KClip);
		}

	if (iUseWriteAlpha)
		{
		iGc->SetDrawMode(DirectGdi::EDrawModeWriteAlpha);
		tag.Append(KWriteAlpha);
		}

	TRect srcRect(TPoint(1,1), TSize(48,18));
	DrawTestPattern1(TRect(0,0,100,100), &srcRect);
	DrawTestPattern2(TRect(100,0,200,100), &srcRect);
	DrawTestPattern3(TRect(0,100,100,200), &srcRect);
	DrawTestPattern1(TRect(100,100,150,150), &srcRect);
	DrawTestPattern2(TRect(150,100,200,150), &srcRect);
	DrawTestPattern3(TRect(100,150,200,200), &srcRect);

	if (iEnableClipRegion)
		{
		iGc->ResetClippingRegion();
		iGc->SetDrawMode(DirectGdi::EDrawModePEN);		
		}

	SaveOutput(tag);
	}

/**
@SYMTestCaseID			
	GRAPHICS-DIRECTGDI-DRAWRESOURCE-0005
	
@SYMTestPriority		
	Critical
	
@SYMPREQ				
	PREQ39

@SYMREQ
	REQ9178 
	REQ9200 
	REQ9201 
	REQ9202 
	REQ9203 
	REQ9222 
	REQ9228 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestStatus			
	Implemented
	
@SYMTestCaseDesc
	Attempt to draw a resource with a NULL handle for each of the DrawResource methods
	that draw image sources to improve code coverage.
	
@SYMTestActions	
	Create an RSgImage,
	Create a RDirectGdiDrawableSource from it,
	Attempt to draw the RDirectGdiDrawableSource with each of the DrawResource methods.
	Each method should set an error of KErrBadHandle.
	
@SYMTestExpectedResults	
	Driver returns KErrBadHandle for each attempt to draw invalid resource.
 */
void CTDirectGdiResource::TestDrawResource_NullHandleL()
	{
	_LIT(KTestName, "DrawResource_NullHandle");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	CDirectGdiDriver* drv = CDirectGdiDriver::Static();
	TESTL(drv != NULL);

	// null handle
	//
	RDirectGdiDrawableSource dwb(*drv);
	RDirectGdiDrawableSource img(*drv);
	
	// Convert the image source, drawable source and bitmaps to TDrawableSourceAndEquivRotatedBmps so it can be
	// drawn using the BitGdi test code as well as the DirectGdi code
	TDrawableSourceAndEquivRotatedBmps imageSource;
	imageSource.iDrawableSrc = &img;
	TDrawableSourceAndEquivRotatedBmps drawableSource;
	drawableSource.iDrawableSrc = &dwb;

	TPoint pos;
	TRect dstRect(pos, TSize(100,100));
	TRect srcRect(pos, TSize(10,10));	
	
	// DrawResource(const TRect&, const RDirectGdiDrawableSource&, const TDesC8&)
	iGc->DrawResource(dstRect, drawableSource, KNullDesC8);
	TESTL(iGc->GetError() == KErrBadHandle);

	for (TInt rot=0; rot<sizeof(TestRot)/sizeof(TestRot[0]); ++rot)
		{
		// DrawResource(const TPoint&, const RDirectGdiDrawableSource&, DirectGdi::TGraphicsRotation)
		iGc->DrawResource(pos, imageSource, TestRot[rot]);
		TESTL(iGc->GetError() == KErrBadHandle);

		// DrawResource(const TRect&, const RDirectGdiDrawableSource&, DirectGdi::TGraphicsRotation)
		iGc->DrawResource(dstRect, imageSource, TestRot[rot]);
		TESTL(iGc->GetError() == KErrBadHandle);

		// DrawResource(const TRectt&, const RDirectGdiDrawableSource&, const TRect&, DirectGdi::TGraphicsRotation)
		iGc->DrawResource(dstRect, imageSource, srcRect, TestRot[rot]);
		TESTL(iGc->GetError() == KErrBadHandle);
		}
	}

/**
@SYMTestCaseID			
	GRAPHICS-DIRECTGDI-DRAWRESOURCE-0006
	
@SYMTestPriority		
	Critical
	
@SYMPREQ				
	PREQ39

@SYMREQ
	REQ9178 
	REQ9200 
	REQ9201 
	REQ9202 
	REQ9203 
	REQ9222 
	REQ9228 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestStatus			
	Implemented
	
@SYMTestCaseDesc		
	Draw onto an RSgImage as a target, then convert to it a source and draw that 
	as a resource.
	
@SYMTestActions			
	Test the use case where we:
	Draw red, green and blue rectangles to a target.
	Create a source from the targets' bitmap.
	Clear the target.
	Draw the source to the target.
	
@SYMTestExpectedResults	
	The three rectangles should have been appear when the source is copied to the target. 
*/
void CTDirectGdiResource::TestDrawTargetAsSourceL()
	{
	_LIT(KTestName, "DrawResource_TestDrawTargetAsSource");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();		
	
	// draw to the current target
	TRect rect(20,20,100,100);	
	iGc->SetPenColor(TRgb(255,0,0));
	iGc->DrawRect(rect);
	iGc->SetPenColor(TRgb(0,255,0));
	rect.Move(20,20);
	iGc->DrawRect(rect);
	iGc->SetPenColor(TRgb(0,0,255));
	rect.Move(20,20);
	iGc->DrawRect(rect);
	TESTNOERROR(iGc->GetError());
	
	// Only do the second part of the test if using DirectGdi. The resultant test bitmap should looks the
	// same when using DirectGdi as the one drawn using BitGdi that only uses the above calls.
	if (iUseDirectGdi)
		{
		iGdiTarget->Finish();
		TESTNOERROR(iGc->GetError());
		
		// get the target bitmap
		CFbsBitmap* bitmap = iGdiTarget->GetTargetFbsBitmapL();
		TESTL(bitmap != NULL);
		
		// Create an RSgImage using the target bitmap	
		TSgImageInfo imageInfo;
		imageInfo.iSizeInPixels = bitmap->SizeInPixels();
		imageInfo.iPixelFormat = iTestParams.iTargetPixelFormat; // only call this test when the target and the source resource pixel format are the same
		imageInfo.iUsage = ESgUsageDirectGdiSource;
		RSgImage sgImage;	
		TInt err = sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride());
		CleanupClosePushL(sgImage);	
		TESTNOERRORL(err);
		
		CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();	
		TESTL(dgdiDriver != NULL);	
		
		// Create a RDirectGdiDrawableSource from the RSgImage
		RDirectGdiDrawableSource dgdiImageSource(*dgdiDriver);
		err = dgdiImageSource.Create(sgImage);
		TESTNOERRORL(err);
		
		iGc->Clear();
		
		// Convert the image source and bitmap to TDrawableSourceAndEquivRotatedBmps so it can be
		// drawn using the BitGdi test code as well as the DirectGdi code
		TDrawableSourceAndEquivRotatedBmps imageSource;
		imageSource.iDrawableSrc = &dgdiImageSource;
		imageSource.iBmpRotNone = bitmap;
		
		// Draw the RDirectGdiDrawableSource using CDirectGdiContext::DrawResource	
		iGc->DrawResource(TPoint(0,0), imageSource);		
		
		dgdiImageSource.Close();
		CleanupStack::PopAndDestroy(1, &sgImage);
		}
	
	// Output the target image to make sure it contains what was drawn onto the source
	TESTNOERROR(WriteTargetOutput(iTestParams, KTestName()));
	}

/**
@SYMTestCaseID				
	GRAPHICS-DIRECTGDI-DRAWRESOURCE-0007
	
@SYMTestPriority			
	Critical
	
@SYMPREQ					
	PREQ39

@SYMREQ
	REQ9178 
	REQ9200 
	REQ9201 
	REQ9202 
	REQ9203 
	REQ9222 
	REQ9228 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestStatus				
	Implemented
	
@SYMTestCaseDesc			
	Create two CDirectGdiImageSource object from the same RSgImage
	
@SYMTestActions				
	Test the use case where we:
	Create an RSgImage
	Create two CDirectGdiImageSource objects.
	The CDirectGdiImageSource objects should share the EGL image created from
	the RSgImage
	as only one EGL image can be created per RSgImage per process.
	If the EGL image sharing is not working an error will occur when creating the
	second CDirectGdiImageSource object.
	
@SYMTestExpectedResults		
	This test does not output an image, it is just for testing that no errors or panics occur.
*/
void CTDirectGdiResource::TestShareEGLImageBetweenSourcesL()
	{
	_LIT(KTestName, "ShareEGLImageBetweenSources");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();	
	
	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	TESTL(dgdiDriver != NULL);	
	
	// Create a CFbsBitmap
	TSize patternSize(90,50);
	TRect rect(0,0,90,50);
	CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iSourceResourcePixelFormat, patternSize);
	TESTL(NULL != bitmap);
	CleanupStack::PushL(bitmap);	
	
	// Create an RSgImage from the CFbsBitmap
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = patternSize;
	imageInfo.iPixelFormat = iTestParams.iSourceResourcePixelFormat;
	imageInfo.iUsage = ESgUsageDirectGdiSource;
	RSgImage sgImage;	
	TInt res = sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride());	
	TESTNOERRORL(res);	
	CleanupClosePushL(sgImage);
	
	// Create a RDirectGdiDrawableSource from the RSgImage
	RDirectGdiDrawableSource dgdiImageSource1(*dgdiDriver);	
	res = dgdiImageSource1.Create(sgImage);		
	TESTNOERRORL(res);
	CleanupClosePushL(dgdiImageSource1);
	
	// Create a second RDirectGdiDrawableSource from the same RSgImage,
	// no error should occur here, the underlying EGL images created should be
	// shared between sources
	RDirectGdiDrawableSource dgdiImageSource2(*dgdiDriver);	
	res = dgdiImageSource2.Create(sgImage);		
	TESTNOERRORL(res);	
	
	dgdiImageSource1.Close();
	dgdiImageSource2.Close();
	CleanupStack::PopAndDestroy(3, bitmap);
	}

/**
@SYMTestCaseID			
	GRAPHICS-DIRECTGDI-DRAWRESOURCE-0008
	
@SYMTestPriority		
	Critical
	
@SYMPREQ				
	PREQ39

@SYMREQ
	REQ9178 
	REQ9200 
	REQ9201 
	REQ9202 
	REQ9203 
	REQ9222 
	REQ9228 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestStatus			
	Implemented
	
@SYMTestCaseDesc		
	Draw resource with valid resource but other invalid parameters
	
@SYMTestActions			
	Draw valid RDirectGdiDrawableSource objects with invalid
	parameters. Test the following for each valid rotation:
	- drawing a resource where the source rectangle does not intersect the actual image extent
	- drawing a resource where the source rectangle is not fully contained by the image extent
	
	
@SYMTestExpectedResults	
	Driver returns KErrArgument for each attempt to draw resource with invalid parameters.
*/
void CTDirectGdiResource::TestDrawResource_InvalidParameterL()
	{
	_LIT(KTestName, "DrawResource_InvalidParameter");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	ResetGc();

	// destRect can be anything
	TPoint pos;
	TRect dstRect(pos, iImgSz);

	// srcRect doesn't intersect image extent at all
	TRect srcRect1(TPoint(1000,1000), TSize(1,1));
	// srcRect is not fully contained within image extent
	TRect srcRect2(TPoint(10,10), TSize(1000,1000));
	TRect srcRect3(TPoint(-10,0), iImgSz);
	
	for (TInt rot=0; rot<sizeof(TestRot)/sizeof(TestRot[0]); ++rot)
		{
		iGc->DrawResource(dstRect, iImgSrc, srcRect1, TestRot[rot]);
		TESTL(iGc->GetError() == KErrArgument);

		iGc->DrawResource(dstRect, iImgSrc, srcRect2, TestRot[rot]);
		TESTL(iGc->GetError() == KErrArgument);

		iGc->DrawResource(dstRect, iImgSrc, srcRect3, TestRot[rot]);
		TESTL(iGc->GetError() == KErrArgument);
		}
	}

/**
@SYMTestCaseID			
	GRAPHICS-DIRECTGDI-DRAWRESOURCE-0009
	
@SYMTestPriority		
	Critical
	
@SYMPREQ				
	PREQ39

@SYMREQ
	REQ9178 
	REQ9200 
	REQ9201 
	REQ9202 
	REQ9203 
	REQ9222 
	REQ9228 
	REQ9223 
	REQ9236 
	REQ9237
	
@SYMTestStatus			
	Implemented
	
@SYMTestCaseDesc		
	Draw resources with a non-zero origin set.
	
@SYMTestActions			
	Set drawing context origin to various non-zero values and draw RDirectGdiDrawableSource
	objects using generic underlying DrawResource implementation that takes destination
	and source rectangle.
	
@SYMTestExpectedResults	
	Resource drawn at the specified location relative to drawing context origin.
*/
void CTDirectGdiResource::TestDrawResource_NonZeroOriginL()
	{
	_LIT(KTestName, "DrawResource_NonZeroOrigin");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	const TPoint origin[] = 
		{
		TPoint(-100,-100),
		TPoint(100,-100),
		TPoint(100,0),
		TPoint(-100,100),
		TPoint(0,100),
		TPoint(100,100)		
		};

	for (TInt idx=0; idx<sizeof(origin)/sizeof(origin[0]); ++idx)
		{
		TBuf<KFileNameLength> tag;
		tag.Append(KTestName);
		tag.AppendNum(idx);

		ResetGc();	
		iGc->SetOrigin(origin[idx]);

		TRect srcRect(TPoint(0,0), iImgSz);

		DrawTestPattern1(TRect(0,0,100,100), &srcRect);
		DrawTestPattern2(TRect(100,0,200,100), &srcRect);
		DrawTestPattern3(TRect(0,100,100,200), &srcRect);
		DrawTestPattern1(TRect(100,100,150,150), &srcRect);
		DrawTestPattern2(TRect(150,100,200,150), &srcRect);
		DrawTestPattern3(TRect(100,150,200,200), &srcRect);

		SaveOutput(tag);
		}
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-DRAWRESOURCE-0010

@SYMTestPriority
	Critical

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9178 
	REQ9200 
	REQ9201 
	REQ9202 
	REQ9203 
	REQ9222 
	REQ9228 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Tests if DirectGDI methods returns an error when called on a not constructed RDirectGdiDrawableSource object.

@SYMTestActions		
	Create a CFbsBitmap. 
	Create an RSgImage from the CFbsBitmap.
	Allocate a RDirectGdiDrawableSource but do not call Create() method.
	Draw the RDirectGdiDrawableSource using CDirectGdiContext::DrawResource.

@SYMTestExpectedResults
	CDirectGdiContext::DrawResource should return KErrBadHandle.
*/
void CTDirectGdiResource::TestDrawResourceSourceNotCreatedL()
	{
	_LIT(KTestName, "DrawResource_SourceNotCreated");
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	if (iUseDirectGdi)
		{
		ResetGc();
		
		TInt res = CDirectGdiDriver::Open();
		TESTNOERRORL(res);		
		
		CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
		TESTL(dgdiDriver != NULL);	
		CleanupClosePushL(*dgdiDriver);	
		
		// Create a CFbsBitmap
		TSize patternSize(90,50);
		CFbsBitmap* bitmap = CreateCheckedBoardBitmapL(iTestParams.iSourceResourcePixelFormat, patternSize);
		TESTL(NULL != bitmap);
		CleanupStack::PushL(bitmap);	
		
		// Create an RSgImage from the CFbsBitmap
		TSgImageInfo imageInfo;
		imageInfo.iSizeInPixels = patternSize;
		imageInfo.iPixelFormat = iTestParams.iSourceResourcePixelFormat;
		imageInfo.iUsage = ESgUsageDirectGdiSource;
		RSgImage sgImage;	
		res = sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride());
		CleanupClosePushL(sgImage);
		TESTNOERRORL(res);		
		
		//allocate RDirectGdiDrawableSource, but do not call Create() method
		RDirectGdiDrawableSource dgdiImageSource(*dgdiDriver);	
		
		// Draw the RDirectGdiDrawableSource using CDirectGdiContext::DrawResource
		TDrawableSourceAndEquivRotatedBmps imageSource;
		imageSource.iDrawableSrc = &dgdiImageSource;
		iGc->DrawResource(TPoint(10,10), imageSource);
		TESTL(iGc->GetError() == KErrBadHandle);		
		dgdiImageSource.Close();
		CleanupStack::PopAndDestroy(3, dgdiDriver); 
		}
	}

/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTDirectGdiResource::doTestStepL()
	{			
	INFO_PRINTF1(_L("DirectGdi Resource Tests"));	
		
	// Test for each target pixel format
	for(TInt targetPixelFormatIndex = iTargetPixelFormatArray.Count() - 1; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{ 
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		TBuf<KPixelFormatNameLength> targetPixelFormatName(TDisplayModeMapping::ConvertPixelFormatToPixelFormatString(iTestParams.iTargetPixelFormat));
		// Test for each source pixel format
		for (TInt sourceResourcePixelFormatIndex = iSourceResourcePixelFormatArray.Count() - 1; sourceResourcePixelFormatIndex >= 0; sourceResourcePixelFormatIndex--)
			{
			TBool doOOM = EFalse;
			iTestParams.iSourceResourcePixelFormat = iSourceResourcePixelFormatArray[sourceResourcePixelFormatIndex];
			if(EUidPixelFormatXRGB_8888 == iTestParams.iSourceResourcePixelFormat)
				doOOM = ETrue;
			TBuf<KPixelFormatNameLength> sourcePixelFormatName(TDisplayModeMapping::ConvertPixelFormatToPixelFormatString(iTestParams.iSourceResourcePixelFormat));
			INFO_PRINTF3(_L("Target Pixel Format: %S;  Source Resource Pixel Format: %S"), &targetPixelFormatName, &sourcePixelFormatName);							
					
			SetTargetL(iTestParams.iTargetPixelFormat);
			CreateCommonResourceL();					
			
			RunTestsL();
			
			// only run OOM tests for one target pixel format to prevent duplication of tests
			if (targetPixelFormatIndex == 0 && doOOM)
				{		
				RunOomTestsL(); // from base class
				}
			
			DestroyCommonResource();			
			}
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Helper function used to create the resources that are shared between all the tests in this file.
 */
void CTDirectGdiResource::CreateCommonResourceL()
	{
	// Create a CFbsBitmap
	CFbsBitmap* bitmap;
	RSgDrawable sgDwb;

	// Generate tiled color pattern, test expects original image size to be 50x20 and has alpha values
	TSize patternSize(50,20);
	bitmap = CreateCheckedBoardBitmapL(iTestParams.iSourceResourcePixelFormat, patternSize, TSize(10, 10), ETrue);	
	iImgSrc.iBmpRotNone = bitmap;		
	iImgSz = bitmap->SizeInPixels();
	
	// Create rotated versions of the bitmap so they can be used when drawing BitGdi equivalent
	// test images for the new DrawResource methods
	iImgSrc.iBmpRot90 = new(ELeave) CFbsBitmap;		
	iImgSrc.iBmpRot180 = new(ELeave) CFbsBitmap;		
	iImgSrc.iBmpRot270 = new(ELeave) CFbsBitmap;	

	// Create an RSgImage from the CFbsBitmap
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = iImgSz;
	imageInfo.iPixelFormat = iTestParams.iSourceResourcePixelFormat;
	imageInfo.iUsage = ESgUsageDirectGdiSource;

	RSgImage sgImage;	
	TInt res = sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride());
	TESTNOERRORL(res);
	CleanupClosePushL(sgImage);	

	if (iUseDirectGdi)
		{
		// Create a RDirectGdiDrawableSource from the RSgImage

		// Driver must've been initialised by framework, no need to re-open
		//
		// Driver behaviour i.e. Sg::Open/Close, CDirectGdiDriver::Open/Close should be tested
		// somewhere else not in details drawing ops test like this
		CDirectGdiDriver* drv = CDirectGdiDriver::Static();
		TEST(drv != NULL);
		
		iImgSrc.iDrawableSrc = new RDirectGdiDrawableSource(*drv);
		TEST(iImgSrc.iDrawableSrc != NULL);
		res = iImgSrc.iDrawableSrc->Create(sgImage);		
		TESTNOERRORL(res);	

		// Create drawable source from drawable
		res = sgDwb.Open(sgImage.Id());
		TESTNOERRORL(res);
		CleanupClosePushL(sgDwb);

		iDwbSrc[0].iDrawableSrc = new RDirectGdiDrawableSource(*drv);
		TEST(iDwbSrc[0].iDrawableSrc != NULL);
		res = iDwbSrc[0].iDrawableSrc->Create(sgDwb);
		TESTNOERRORL(res);
		
		// Create drawable source from image
		iDwbSrc[1].iDrawableSrc = new RDirectGdiDrawableSource(*drv);
		TEST(iDwbSrc[1].iDrawableSrc != NULL);
		res = iDwbSrc[1].iDrawableSrc->Create(sgImage);
		TESTNOERRORL(res);
		}
	else
		{
		// Share the bitmaps from iImgSrc with this drawable for testing
		iDwbSrc[0].iBmpRotNone = iImgSrc.iBmpRotNone; 
		iDwbSrc[0].iBmpRot90 = iImgSrc.iBmpRot90; 
		iDwbSrc[0].iBmpRot180 = iImgSrc.iBmpRot180;
		iDwbSrc[0].iBmpRot270 = iImgSrc.iBmpRot270;
		
		// Share the bitmaps from iImgSrc with this drawable for testing
		iDwbSrc[1].iBmpRotNone = iImgSrc.iBmpRotNone; 
		iDwbSrc[1].iBmpRot90 = iImgSrc.iBmpRot90; 
		iDwbSrc[1].iBmpRot180 = iImgSrc.iBmpRot180;
		iDwbSrc[1].iBmpRot270 = iImgSrc.iBmpRot270;
		}

	// Create horizontal and vertical stripe clipping region
	TInt w = 1;
	TBool on = EFalse;
	for (TInt x=0; x<50; x+=w)
		{
		on = !on;
		if (on && x+w<50)
			{
			TRect r1(TPoint(x,0), TSize(w, 200));
			TRect r2(TPoint(100+x,0), TSize(w, 200));
			iClip.AddRect(r1);
			iClip.AddRect(r2);
			}
		++w;
		}
	TInt h = 1;
	on = EFalse;
	for (TInt y=0; y<200; y+=h)
		{
		on = !on;
		if (on && y+h<200)
			{
			TRect r1(TPoint(0,y), TSize(100, h));
			TRect r2(TPoint(100,y), TSize(100, h));
			iClip.AddRect(r1);
			iClip.AddRect(r2);
			}
		++h;
		}
	
	if (!iUseDirectGdi)
		{
		// We need to create 3 rotated copies of the bitmap we are drawing when testing using BitGdi
		// using the CBitmapRotator active object, start rotating the first one here
		iCurrentState = ERotatingBitmap90;
		iBitmapRotator->Rotate(&iStatus, *bitmap, *(iImgSrc.iBmpRot90), CBitmapRotator::ERotation90DegreesClockwise);
		SetActive();
		CActiveScheduler::Start();
		}
	
	// Destroy bitmap, image and drawable.
	CleanupStack::PopAndDestroy(iUseDirectGdi ? 2 : 1, &sgImage);
	}

/**
Helper function used to destroy the resources that are shared between all the tests in this file.
 */
void CTDirectGdiResource::DestroyCommonResource()
	{
	if (iImgSrc.iDrawableSrc)
		{
		iImgSrc.iDrawableSrc->Close();
		delete iImgSrc.iDrawableSrc;
		iImgSrc.iDrawableSrc = NULL;
		}
	
	delete iImgSrc.iBmpRotNone;
	iImgSrc.iBmpRotNone = NULL;
	delete iImgSrc.iBmpRot90;
	iImgSrc.iBmpRot90 = NULL;
	delete iImgSrc.iBmpRot180;
	iImgSrc.iBmpRot180 = NULL;
	delete iImgSrc.iBmpRot270;
	iImgSrc.iBmpRot270 = NULL;	

	for (TInt ii=0; ii<2; ++ii)
		{
		if (iDwbSrc[ii].iDrawableSrc)
			{
			iDwbSrc[ii].iDrawableSrc->Close();
			delete iDwbSrc[ii].iDrawableSrc;
			iDwbSrc[ii].iDrawableSrc = NULL;
			}
		
		iDwbSrc[ii].iBmpRotNone = NULL;
		iDwbSrc[ii].iBmpRot90 = NULL;
		iDwbSrc[ii].iBmpRot180 = NULL;
		iDwbSrc[ii].iBmpRot270 = NULL;		
		}

	iClip.Close();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTDirectGdiResource::doTestStepPreambleL()
	{				
	CTDirectGdiStepBase::doTestStepPreambleL();
	CActiveScheduler::Add(this);
	iBitmapRotator = CBitmapRotator::NewL();		
	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTDirectGdiResource::doTestStepPostambleL()
	{		 
	delete iBitmapRotator;	
	iBitmapRotator = NULL;
	CTDirectGdiStepBase::doTestStepPostambleL();
	return TestStepResult();
}

void CTDirectGdiResource::DoDrawTestL()
	{
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWRESOURCE-0001"));
	TestDrawResource_ImageAsDrawableL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWRESOURCE-0002"));
	TestDrawResource_PosL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWRESOURCE-0003"));
	TestDrawResource_DestRectL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWRESOURCE-0004"));
	TestDrawResource_DestRectSrcRectL();
	RecordTestResultL();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTDirectGdiResource::RunTestsL()
	{		
	// For all combination of destination and source pixel format do the following tests
	//
	DoDrawTestL();
	
	// Extra test using EDrawModeWriteAlpha. Geometry tests are irrelevant here, only contents tests are.
	//
	if (SourceResourceHasAlpha())
		{
		iUseWriteAlpha = ETrue;
		DoDrawTestL();
		iUseWriteAlpha = EFalse;
		}

	// Geometry and negative tests doesn't need repeating, do it once
	//
	if (OneTimeTestEnabled())
		{
		iEnableClipRegion = ETrue;
		DoDrawTestL();
		iEnableClipRegion = EFalse;

		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWRESOURCE-0009"));
		TestDrawResource_NonZeroOriginL();
		RecordTestResultL();
		if (iUseDirectGdi)
			{
			// These test for errors being set that are specific to DirectGdi and
			// have no output so there is no need to run for BitGdi
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWRESOURCE-0005"));
			TestDrawResource_NullHandleL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWRESOURCE-0008"));
			TestDrawResource_InvalidParameterL();
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWRESOURCE-0010"));
			TestDrawResourceSourceNotCreatedL();
			RecordTestResultL();
			}		
		}

	// This test uses the target as a source, so only call it when the target
	// pixel format is the same as the source resource pixel format
	//
	if (iTestParams.iTargetPixelFormat == iTestParams.iSourceResourcePixelFormat)
		{
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWRESOURCE-0006"));
		TestDrawTargetAsSourceL();
		RecordTestResultL();
		}

	if (iUseDirectGdi)
		{
		// This test is only appropriate when using DirectGdi as it tests sharing of EGL images
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRAWRESOURCE-0007"));
		TestShareEGLImageBetweenSourcesL();
		RecordTestResultL();
		}
	}

/**
From CActive
 */
void CTDirectGdiResource::RunL()
	{
	switch (iCurrentState)
		{
		case ERotatingBitmap90:
			// Rotate the next bitmap (180)
			iCurrentState = ERotatingBitmap180;
			iBitmapRotator->Rotate(&iStatus, *(iImgSrc.iBmpRotNone), *(iImgSrc.iBmpRot180), CBitmapRotator::ERotation180DegreesClockwise);
			SetActive();
			break;
			
		case ERotatingBitmap180:
			// Rotate the next bitmap (270)
			iCurrentState = ERotatingBitmap270;
			iBitmapRotator->Rotate(&iStatus, *(iImgSrc.iBmpRotNone), *(iImgSrc.iBmpRot270), CBitmapRotator::ERotation270DegreesClockwise);
			SetActive();
			break;
			
		case ERotatingBitmap270:
			// Finished rotating the bitmaps, run the tests now
			iCurrentState = EDone;			
			CActiveScheduler::Stop();
			break;
		}
	}

/**
From CActive
 */
void CTDirectGdiResource::DoCancel()
	{
	}
