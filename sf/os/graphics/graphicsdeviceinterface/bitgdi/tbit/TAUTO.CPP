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


#include <hal.h>
#include <e32test.h>
#include <bitdraw.h>
#include <graphics/gdi/gdiconsts.h>
#include "TAUTO.H"
#include <graphics/fbsdefs.h>


TInt PanicTestThread(TAny* aOption);

enum TPanicOption
	{
	EFirst,
	EPolygonFiller,
	EBitmapDevice,
	EDrawBmp1,
	EDrawBmp2,
	EDrawBmp3,
	EGcDevice1,
	EGcDevice2,
	EGcRegion1,
	EGcRegion2,
	EUseBrush1,
	EUseBrush2,
	EUseBrush3,
	EUseFont,
	EDrawText1,
	EDrawText2,
	EDrawText3,
	ELast
	};


CTAuto::CTAuto(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iScreenDevice(NULL),
	iHalfScreen()
	{
	iTestRect[0].SetRect(21,10,22,50);
	iTestRect[1].SetRect(10,21,50,22);
	iTestRect[2].SetRect(11,20,90,30);
	iTestRect[3].SetRect(10,71,90,80);
	iTestRect[4].SetRect(20,10,31,90);
	iTestRect[5].SetRect(70,10,80,91);
	iTestRect[6].SetRect(10,10,90,90);
	iTestRect[7].SetRect(32,32,96,96);

	INFO_PRINTF1(_L(" "));
	}
	
CTAuto::~CTAuto()
	{
	
	}

void CTAuto::RunTestCaseL(const TInt aCurTestCase)
	{
	((CTAutoStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EGray2);
		break;
	case 2:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EGray4);
		break;
	case 3:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EGray16);
		break;
	case 4:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EGray256);
		break;
	case 5:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EColor16);
		break;
	case 6:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EColor256);
		break;
	case 7:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EColor4K);
		break;
	case 8:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EColor64K);
		break;
	case 9:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EColor16M);
		break;
	case 10:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EColor16MU);
		break;
	case 11:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EGray2,15);
		break;
	case 12:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EGray4,5);
		break;
	case 13:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EGray16,5);
		break;
	case 14:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EGray256,5);
		break;
	case 15:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EColor16,5);
		break;
	case 16:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EColor256,3);
		break;
	case 17:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EColor4K,5);
		break;
	case 18:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EColor64K,4);
		break;
	case 19:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EColor16M,4);
		break;
	case 20:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EColor16MU,4);
		break;
	case 21:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EGray2);
		break;
	case 22:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EGray4);
		break;
	case 23:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EGray16);
		break;
	case 24:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EGray256);
		break;
	case 25:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EColor16);
		break;
	case 26:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EColor256);
		break;
	case 27:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EColor4K);
		break;
	case 28:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EColor64K);
		break;
	case 29:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EColor16M);
		break;
	case 30:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EColor16MU);
		break;
	case 31:		
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EColor16MA);
		break;
	case 32:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EColor16MA);
		break;
	case 33:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EColor16MA,4);
		break;
	case 34:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0027"));
		TestBmpL(EColor16MAP);
		break;
	case 35:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0025"));
		TestMem(EColor16MAP);
		break;
	case 36:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0026"));
		TestMapL(EColor16MAP,4);
		break;
	case 37:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0029"));
		TestFadingL();
		INFO_PRINTF2(_L("TestCase %d - Passed Fading Test\r\n"),aCurTestCase);		
		break;
	case 38:
		((CTAutoStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0028"));
		TestPanics();
		break;
	case 39: //exit 
		((CTAutoStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTAutoStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;		
		}
	((CTAutoStep*)iStep)->RecordTestResultL();
	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0025
 
  @SYMDEF             

  @SYMTestCaseDesc Tests the memory allocation cleanup after allocations fails 
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions sets memory allocation to fail then checks it is cleaned up correctly
 
  @SYMTestExpectedResults Memory should be successfully cleaned up
*/
void CTAuto::TestMem(TDisplayMode aDispMode)
	{
	LowLevelDevice(aDispMode,EFalse);
	BitmapDevice(aDispMode);
	LowLevelDevice(aDispMode,ETrue);
	ScreenDevice(aDispMode);

	INFO_PRINTF2(_L("Mode %d passed memory test\r\n"),aDispMode);
	}

void CTAuto::ScreenDevice(TDisplayMode aDispMode)
	{
	CFbsScreenDevice* device = NULL;
	for (TInt count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;
		TRAPD(err,device = CFbsScreenDevice::NewL(_L("scdv"),aDispMode));
		if (err == KErrNotSupported)
			{
			__UHEAP_MARKEND;
			break;
			}
		else if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if (err == KErrNone)
			{
			delete device;
			__UHEAP_MARKEND;
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			User::Panic(_L("CFbsScreenDevice test failed "),err);
			}
		}

	__UHEAP_RESET;
	}

void CTAuto::BitmapDevice(TDisplayMode aDispMode)
	{
	CFbsBitmap bmp;
	TInt ret = bmp.Create(TSize(1,1),aDispMode);
	if (ret != KErrNone)
		User::Panic(_L("Failed to create bitmap"),ret);

	CFbsBitmapDevice* device = NULL;
	for (TInt count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;
		TRAPD(err,device = CFbsBitmapDevice::NewL(&bmp));
		if(err == KErrNotSupported)
			{
			__UHEAP_MARKEND;
			break;
			}
		else if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if (err == KErrNone)
			{
			delete device;
			__UHEAP_MARKEND;
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			User::Panic(_L("CFbsBitmapDevice test failed "),err);
			}
		}

	__UHEAP_RESET;
	}

void CTAuto::LowLevelDevice(TDisplayMode aDispMode,TBool aScreen)
	{
	TInt address = NULL;
	TSize size(0,0);
	TInt ret = HAL::Get(KDefaultScreenNo, HALData::EDisplayMemoryAddress,address);
	if (ret == KErrNone)
		ret = HAL::Get(KDefaultScreenNo, HALData::EDisplayXPixels,size.iWidth);
	if (ret == KErrNone)
		ret = HAL::Get(KDefaultScreenNo, HALData::EDisplayYPixels,size.iHeight);
	if (ret != KErrNone)
		User::Panic(_L("CFbsDrawDevice test failed "),ret);

	TPckgBuf<TScreenInfoV01> s;
	s().iScreenAddressValid = ETrue;
	s().iScreenAddress = REINTERPRET_CAST(TAny*,address);
	s().iScreenSize = size;

	CFbsDrawDevice* fdd = NULL;

	for(TInt count = 1; ; count++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,count);
		__UHEAP_MARK;

		if (aScreen)
			{
			TRAP(ret,fdd = CFbsDrawDevice::NewScreenDeviceL(s(),aDispMode));
			}
		else
			{
			TRAP(ret, fdd = CFbsDrawDevice::NewBitmapDeviceL(s(), aDispMode, CFbsBitmap::ScanLineLength(size.iWidth, aDispMode)));
			}

		if (ret == KErrNotSupported)
			{
			__UHEAP_MARKEND;
			break;
			}
		else if (ret == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if (ret == KErrNone)
			{
			delete fdd;
			__UHEAP_MARKEND;
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			User::Panic(_L("CFbsDrawDevice test failed "),ret);
			}
		}

	__UHEAP_RESET;
	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0026
 
  @SYMDEF             

  @SYMTestCaseDesc Colour mapping testing
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions attempts to map four shades of grey to greys in a colourmap
 
  @SYMTestExpectedResults All pixels should map to the colourmap correctly
*/
//
// 
//
void CTAuto::TestMapL(TDisplayMode aDispMode,TInt aShadowFactor)
	{
	TRAPD(err,iScreenDevice = CFbsScreenDevice::NewL(_L("scdv"),aDispMode));
	if (err == KErrNotSupported)
		return;
	User::LeaveIfError(err);

	User::LeaveIfError(iScreenDevice->CreateContext(iScreenGc));
	iScreenDevice->ChangeScreenDevice(NULL);

	TestMapColors();
	TestShadowArea(aShadowFactor);
	TestFadeArea();

	delete iScreenGc;
	delete iScreenDevice;

	INFO_PRINTF2(_L("Mode %d passed colour map test\r\n"),aDispMode);
	}

void CTAuto::TestMapColors()
	{
	TRgb colormap[4];
	TRgb screencolor;
	TInt x,y;

	for (TInt count = 0; count < KNumRects; count++)
		{
		for (TInt color = 0; color < 4 && iScreenDevice->DisplayMode() != EGray2; color++)
			{
			Clear(TRgb::Gray4(color));
			TRect r(iTestRect[count]);
			colormap[0] = TRgb::Gray4(color);
			colormap[1] = TRgb::Gray4(3-color);
			colormap[2] = colormap[1];
			colormap[3] = colormap[0];
			if (iScreenDevice->DisplayMode() == EColor64K)
				{
				colormap[0] = TRgb::Color64K(colormap[0].Color64K());
				colormap[1] = TRgb::Color64K(colormap[1].Color64K());
				colormap[2] = colormap[1];
				colormap[3] = colormap[0];
				}
			iScreenGc->MapColors(r,colormap,2);

			for(y=0;y<r.iTl.iY;y++)
				for(x=0;x<100;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray4() == color);
					}
			for(y=r.iTl.iY;y<r.iBr.iY;y++)
				for(x=0;x<r.iTl.iX;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray4() == color);
					}
			for(y=r.iTl.iY;y<r.iBr.iY;y++)
				for(x=r.iBr.iX;x<100;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray4() == color);
					}
			for(y=r.iBr.iY;y<100;y++)
				for(x=0;x<100;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray4() == color);
					}
			for(y=r.iTl.iY;y<r.iBr.iY;y++)
				for(x=r.iTl.iX;x<r.iBr.iX;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray4() == 3-color);
					}
			iScreenGc->MapColors(r,colormap,2);
			for(y=r.iTl.iY;y<r.iBr.iY;y++)
				for(x=r.iTl.iX;x<r.iBr.iX;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray4() == color);
					}
			}
		}
	}

void CTAuto::TestShadowArea(TInt aShadowFactor)
	{
	TRgb screencolor;
	TInt x,y;

	for (TInt count = 0; count < KNumRects; count++)
		{
		for (TInt color = 0; color < 4; color++)
			{
			if (color > 0 && iScreenDevice->DisplayMode() == EGray2)
				color = 3;

			Clear(TRgb::Gray4(color));
			TRect r(iTestRect[count]);
			TRegionFix<1> reg(r);

			iScreenGc->ShadowArea(&reg);

			for(y=0;y<r.iTl.iY;y++)
				for(x=0;x<100;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray4() == color);
					}

			for(y=r.iTl.iY;y<r.iBr.iY;y++)
				for(x=0;x<r.iTl.iX;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray4() == color);
					}

			for(y=r.iTl.iY;y<r.iBr.iY;y++)
				for(x=r.iBr.iX;x<100;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray4() == color);
					}

			for(y=r.iBr.iY;y<100;y++)
				for(x=0;x<100;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray4() == color);
					}

			for(y=r.iTl.iY;y<r.iBr.iY;y++)
				for(x=r.iTl.iX;x<r.iBr.iX;x++)
					{
					iScreenDevice->GetPixel(screencolor,TPoint(x,y));
					TEST(screencolor.Gray16() == Max(0,color * 5 - aShadowFactor));
					}
			}
		}
	}

void CTAuto::TestFadeArea()
	{
	DoTestFadeArea(0,255);
	DoTestFadeArea(0,127);
	DoTestFadeArea(128,255);
	DoTestFadeArea(64,192);
	}

void CTAuto::DoTestFadeArea(TUint8 aBlackMap,TUint8 aWhiteMap)
	{
	iScreenGc->SetFadingParameters(aBlackMap,aWhiteMap);

	DoTestFadeArea(KRgbWhite,FadeColor(KRgbWhite,aBlackMap,aWhiteMap));
	DoTestFadeArea(KRgbBlack,FadeColor(KRgbBlack,aBlackMap,aWhiteMap));

	if (iScreenDevice->DisplayMode() == EGray2)
		return; // EGray2 mode dithers so only check black & white

	DoTestFadeArea(KRgbGray,FadeColor(KRgbGray,aBlackMap,aWhiteMap));
	DoTestFadeArea(KRgbDarkGray,FadeColor(KRgbDarkGray,aBlackMap,aWhiteMap));

	if (iScreenDevice->DisplayMode() == EGray4)
		return; // EGray4 mode dithers so only check four gray scales

	DoTestFadeArea(KRgbRed,FadeColor(KRgbRed,aBlackMap,aWhiteMap));
	DoTestFadeArea(KRgbGreen,FadeColor(KRgbGreen,aBlackMap,aWhiteMap));
	DoTestFadeArea(KRgbBlue,FadeColor(KRgbBlue,aBlackMap,aWhiteMap));
	}

void CTAuto::DoTestFadeArea(TRgb aColor,TRgb aFadedColor)
	{
	TRgb screencolor;
	TInt x,y;

	for (TInt count = 0; count < KNumRects; count++)
		{
		Clear(aColor);
		aColor = MapColorToDisplayMode(aColor);

		TRect r(iTestRect[count]);
		TRegionFix<1> reg(r);

		iScreenGc->FadeArea(&reg);

		for(y=0;y<r.iTl.iY;y++)
			for(x=0;x<100;x++)
				{
				iScreenDevice->GetPixel(screencolor,TPoint(x,y));
				TEST(screencolor == aColor);
				}

		for(y=r.iTl.iY;y<r.iBr.iY;y++)
			for(x=0;x<r.iTl.iX;x++)
				{
				iScreenDevice->GetPixel(screencolor,TPoint(x,y));
				TEST(screencolor == aColor);
				}

		for(y=r.iTl.iY;y<r.iBr.iY;y++)
			for(x=r.iBr.iX;x<100;x++)
				{
				iScreenDevice->GetPixel(screencolor,TPoint(x,y));
				TEST(screencolor == aColor);
				}

		for(y=r.iBr.iY;y<100;y++)
			for(x=0;x<100;x++)
				{
				iScreenDevice->GetPixel(screencolor,TPoint(x,y));
				TEST(screencolor == aColor);
				}

		for(y=r.iTl.iY;y<r.iBr.iY;y++)
			for(x=r.iTl.iX;x<r.iBr.iX;x++)
				{
				iScreenDevice->GetPixel(screencolor,TPoint(x,y));
				TEST(screencolor == aFadedColor);
				}
		}
	}

TRgb CTAuto::FadeColor(TRgb aColor,TInt aBlackMap,TInt aWhiteMap)
	{
	aColor = MapColorToDisplayMode(aColor);

	TInt red = ((aColor.Red() * (aWhiteMap - aBlackMap + 1)) >> 8) + aBlackMap;
	TInt green = ((aColor.Green() * (aWhiteMap - aBlackMap + 1)) >> 8) + aBlackMap;
	TInt blue = ((aColor.Blue() * (aWhiteMap - aBlackMap + 1)) >> 8) + aBlackMap;

	aColor = TRgb(red,green,blue);

	aColor = MapColorToDisplayMode(aColor);

	return aColor;
	}

TRgb CTAuto::MapColorToDisplayMode(TRgb aColor)
	{
	switch (iScreenDevice->DisplayMode())
		{
	case EGray2:
		aColor = TRgb::Gray2(aColor.Gray2());
		break;
	case EGray4:
		aColor = TRgb::Gray4(aColor.Gray4());
		break;
	case EGray16:
		aColor = TRgb::Gray16(aColor.Gray16());
		break;
	case EGray256:
		aColor = TRgb::Gray256(aColor.Gray256());
		break;
	case EColor16:
		aColor = TRgb::Color16(aColor.Color16());
		break;
	case EColor256:
		aColor = TRgb::Color256(aColor.Color256());
		break;
	case EColor4K:
		aColor = TRgb::Color4K(aColor.Color4K());
		break;
	case EColor64K:
		aColor = TRgb::Color64K(aColor.Color64K());
		break;
	case EColor16M:
		aColor = TRgb::Color16M(aColor.Color16M());
		break;
	case EColor16MU:
		aColor = TRgb::Color16MU(aColor.Color16MU());
		break;
	case EColor16MA:
		aColor = TRgb::Color16MA(aColor.Color16MA());
		break;
	case EColor16MAP:
		aColor = TRgb::Color16MAP(aColor.Color16MAP());
		break;
	default:
		User::Invariant();
		break;
		}

	return aColor;
	}

void CTAuto::Clear(TRgb aColor)
	{
	iScreenGc->SetBrushColor(aColor);
	iScreenGc->Clear();
	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0027
 
  @SYMDEF             

  @SYMTestCaseDesc tests drawing a bitmap to the screen in various colour modes
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions draws to a bitmaps gc and the screens gc then compares the result on a per pixel basis
 
  @SYMTestExpectedResults bitmap and screen should be identical
*/
//
// Bitmap drawing testing
//
void CTAuto::TestBmpL(TDisplayMode aDispMode)
	{
	TRAPD(err,iScreenDevice = CFbsScreenDevice::NewL(_L("scdv"),aDispMode));
	if (err == KErrNotSupported)
		return;
	User::LeaveIfError(err);
	iScreenDevice->ChangeScreenDevice(NULL);
	User::LeaveIfError(iScreenDevice->CreateContext((CGraphicsContext*&)iScreenGc));

	iHalfScreen = iScreenDevice->SizeInPixels();
	iHalfScreen.iWidth /= 2;

	CFbsBitmap bitmap;
	User::LeaveIfError(bitmap.Create(iHalfScreen,aDispMode));

	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(&bitmap);
	CleanupStack::PushL(bitmapDevice);
	CFbsBitGc* bitmapGc = NULL;
	User::LeaveIfError(bitmapDevice->CreateContext(bitmapGc));
	CleanupStack::PushL(bitmapGc);

	TestGc(iScreenGc,iHalfScreen);
	TestGc(bitmapGc,iHalfScreen);

	iScreenGc->BitBlt(TPoint(iHalfScreen.iWidth,0),&bitmap);

	TRgb* left = new(ELeave) TRgb[iHalfScreen.iWidth];
	TRgb* right = new(ELeave) TRgb[iHalfScreen.iWidth];

	TPtr8 leftBuf(REINTERPRET_CAST(TUint8*,left),iHalfScreen.iWidth * sizeof(TRgb),iHalfScreen.iWidth * sizeof(TRgb));
	TPtr8 rightBuf(REINTERPRET_CAST(TUint8*,right),iHalfScreen.iWidth * sizeof(TRgb),iHalfScreen.iWidth * sizeof(TRgb));

	for (TInt row = 0; row < iHalfScreen.iHeight; row++)
		{
		iScreenDevice->GetScanLine(leftBuf,TPoint(0,row),iHalfScreen.iWidth,ERgb);
		bitmap.GetScanLine(rightBuf,TPoint(iHalfScreen.iWidth,row),iHalfScreen.iWidth,ERgb);

		TRgb* leftPtr = left;
		TRgb* rightPtr = right;

		for (TInt col = 0; col < iHalfScreen.iWidth; col++)
			{
			if (leftPtr[0] != rightPtr[0])
				User::Panic(_L("Screen/bitmap mismatch!"),KErrGeneral);

			leftPtr++;
			rightPtr++;
			}
		}

	delete[] left;
	delete[] right;
	CleanupStack::PopAndDestroy(2); // bitmapGc,bitmapDevice
	delete iScreenGc;
	delete iScreenDevice;

	iScreenDevice = NULL;
	iScreenGc = NULL;

	bitmap.Reset();

	INFO_PRINTF2(_L("Mode %d passed bitmap drawing test\r\n"),aDispMode);
	}

void CTAuto::TestGc(CFbsBitGc* aGc,const TSize& aSize)
	{
	aGc->SetPenStyle(CGraphicsContext::ENullPen);
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetBrushColor(KRgbBlack);
	aGc->DrawRect(TRect(TPoint(),iScreenDevice->SizeInPixels()));
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	Colors(aGc,aSize);
	Shapes(aGc,aSize);
	}

void CTAuto::Colors(CFbsBitGc* aGc,const TSize& aSize)
	{
	TInt i=0;
	TInt w=aSize.iWidth;
	TInt h=aSize.iHeight;
	TInt xinc = w / 16;
	TInt lx = 0;
	TInt rx = xinc - 1;

	for(i=0;i<256;i++)
		{
		aGc->SetPenColor(TRgb::Gray256(i));
		aGc->SetBrushColor(TRgb::Gray256(i));
		aGc->DrawRect(TRect(lx,i*h/256,rx,(i+1)*h/256));
		}
	lx += xinc;
	rx += xinc;
	for(i=0;i<16;i++)
		{
		aGc->SetPenColor(TRgb::Gray16(i));
		aGc->SetBrushColor(TRgb::Gray16(i));
		aGc->DrawRect(TRect(lx,i*h/16,rx,(i+1)*h/16));
		}
	lx += xinc;
	rx += xinc;
	for(i=0;i<4;i++)
		{
		aGc->SetPenColor(TRgb::Gray4(i));
		aGc->SetBrushColor(TRgb::Gray4(i));
		aGc->DrawRect(TRect(lx,i*h/4,rx,(i+1)*h/4));
		}
	lx += xinc;
	rx += xinc;
	for(i=0;i<256;i++)
		{
		aGc->SetPenColor(TRgb(i,0,0));
		aGc->SetBrushColor(TRgb(i,0,0));
		aGc->DrawRect(TRect(lx,i*h/256,rx,(i+1)*h/256));
		}
	lx += xinc;
	rx += xinc;
	for(i=0;i<256;i++)
		{
		aGc->SetPenColor(TRgb(0,i,0));
		aGc->SetBrushColor(TRgb(0,i,0));
		aGc->DrawRect(TRect(lx,i*h/256,rx,(i+1)*h/256));
		}
	lx += xinc;
	rx += xinc;
	for(i=0;i<256;i++)
		{
		aGc->SetPenColor(TRgb(0,0,i));
		aGc->SetBrushColor(TRgb(0,0,i));
		aGc->DrawRect(TRect(lx,i*h/256,rx,(i+1)*h/256));
		}
	lx += xinc;
	rx += xinc;
	for(i=0;i<256;i++)
		{
		aGc->SetPenColor(TRgb::Color256(i));
		aGc->SetBrushColor(TRgb::Color256(i));
		aGc->DrawRect(TRect(lx,i*h/256,rx,(i+1)*h/256));
		}
	lx += xinc;
	rx += xinc;
	for(i=0;i<16;i++)
		{
		aGc->SetPenColor(TRgb::Color16(i));
		aGc->SetBrushColor(TRgb::Color16(i));
		aGc->DrawRect(TRect(lx,i*h/16,rx,(i+1)*h/16));
		}

	}

void CTAuto::Shapes(CFbsBitGc* aGc,const TSize& aSize)
	{
	aGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc->SetPenColor(KRgbWhite);

	TInt w=aSize.iWidth;
	TInt h=aSize.iHeight;

	CArrayFixFlat<TPoint>* poly=new(ELeave) CArrayFixFlat<TPoint>(4);
	TInt a=w>>1;
	TInt b=a+(a>>1);
	TPoint temppt(w*5/8,h/48);
	poly->AppendL(temppt);
	temppt.SetXY(b-1,h*11/48);
	poly->AppendL(temppt);
	temppt.SetXY(a+1,h*11/48);
	poly->AppendL(temppt);
	temppt.SetXY(w*5/8,h/48);
	poly->AppendL(temppt);
	aGc->DrawPolyLine(poly);
	delete poly;

	aGc->DrawRect(TRect(a+1,h*7/24,b,h*12/24));
	aGc->DrawRoundRect(TRect(a+1,h*13/24,b,h*18/24),TSize(w/16,h/12));
	aGc->DrawEllipse(TRect(a+1,h*19/24,b,h*24/24));
	aGc->DrawArc(TRect(b+1,h*1/24,w-1,h*6/24),TPoint(w*15/16,h*6/24),TPoint(w*13/16,h*6/24));
	aGc->DrawPie(TRect(b+1,h*7/24,w-1,h*12/24),TPoint(w*13/16,h*7/24),TPoint(w*15/16,h*7/24));
	aGc->DrawEllipse(TRect(b+1,h*13/24,w-1,h*18/24));
	aGc->DrawEllipse(TRect(b+1,h*20/24,w-1,h*23/24));
	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0028
 
  @SYMDEF             

  @SYMTestCaseDesc Tests various graphic panics 
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Starts a thread that causes various graphic panics depending on the chosen option
 
  @SYMTestExpectedResults The thread should panic each time for each chosen option
*/
void CTAuto::TestPanics()
	{
	for (TInt opt = EFirst + 1; opt < ELast; opt++)
		{
		StartThread(opt);
		INFO_PRINTF2(_L("Panic test %d passed\r\n"),opt);
		}
	}

void CTAuto::StartThread(TInt aOption)
	{
	RThread thrd;
	TRequestStatus stat;
	TBuf<256> threadNameBuf;
	_LIT(KThreadNameFormat, "ptt%d");
	threadNameBuf.Format(KThreadNameFormat, aOption);
	TInt threadCreationVal = thrd.Create(threadNameBuf,PanicTestThread,KDefaultStackSize,0x2000,0x20000,(TAny*)aOption);
	TEST(threadCreationVal==KErrNone);	
	thrd.SetPriority(EPriorityMuchMore);
	thrd.Logon(stat);
	User::SetJustInTime(EFalse);
	thrd.Resume();
	User::WaitForRequest(stat);
	thrd.Close();
	User::SetJustInTime(ETrue);
	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0029
 
  @SYMDEF             

  @SYMTestCaseDesc tests fading functionality
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 	Tests fading functionality by drawing two rectangles with different alpha values and compares the results
  				 					
  @SYMTestExpectedResults the two bitmaps should be identical after the operations
*/
void CTAuto::TestFadingL()
	{
	TBool res = EFalse;
	TRect rectbitmap1(0,0,100,50);
	TRect rectbitmap2(25,0,75,100);
	
	CFbsBitmap* bitmap=new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(100,100), EColor64K));
	
	CFbsBitmapDevice* device=CFbsBitmapDevice::NewL(bitmap);		
	CleanupStack::PushL(device);
		
	CFbsBitGc* gc;
	User::LeaveIfError(device->CreateContext(gc));
	CleanupStack::PushL(gc);
	
	CFbsBitmap* bitmap2=new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap2);
	User::LeaveIfError(bitmap2->Create(TSize(100,100), EColor64K));
	
	CFbsBitmapDevice* device2=CFbsBitmapDevice::NewL(bitmap2);		
	CleanupStack::PushL(device2);
		
	CFbsBitGc* gc2;
	User::LeaveIfError(device2->CreateContext(gc2));
	CleanupStack::PushL(gc2);

	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->SetPenColor(TRgb(0,0,0,0));

	//Draws rectangle with Alpha Value 0 when Fading is turned OFF
	gc->SetBrushColor(TRgb(0,0,255,255));
	gc->DrawRect(rectbitmap2);
	
 	//Draws rectangle with Alpha Value 255 when Fading is turned OFF
	gc->SetBrushColor(TRgb(255,0,0,0));
	gc->DrawRect(rectbitmap1);

	gc2->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc2->SetPenColor(TRgb(0,0,0,0));	
	

 	//Draws rectangle with Alpha Value 255 when Fading is turned OFF
	gc2->SetBrushColor(TRgb(0,0,255,255));
	gc2->DrawRect(rectbitmap2);

 	//Draws rectangle with Alpha Value 0 when Fading is turned ON
 	//Fade Option Turned ON
	gc2->SetFaded(ETrue);
	gc2->SetBrushColor(TRgb(255,0,0,0));
	gc2->DrawRect(rectbitmap1);

	TRect rect(0,0,100,100);
	res = device->RectCompare(rect,*device2,rect);
	
	CleanupStack::PopAndDestroy(6, bitmap);
	TEST(res);
	}

TInt PanicTestThread(TAny* aOption)
	{
	CTrapCleanup::New();
	RFbsSession::Connect();

	CFbsScreenDevice* screendevice=NULL;
	CFbsBitGc* bitgc=NULL;
	CPolygonFiller* polyfill=NULL;
	RRegion region;
	const TInt option = TInt(aOption);
	TInt ret;

	switch (option)
		{
	case EPolygonFiller:
		polyfill=(CPolygonFiller*)User::Alloc(sizeof(CPolygonFiller));
		new(polyfill) CPolygonFiller;
		break;
	case EDrawBmp1:
	case EDrawBmp2:
	case EDrawBmp3:
	case EDrawText1:
	case EDrawText2:
	case EDrawText3:
		TRAP(ret,screendevice=CFbsScreenDevice::NewL(_L("scdv"),EColor256));
		if (ret == KErrNotSupported)
			TRAP(ret,screendevice=CFbsScreenDevice::NewL(_L("scdv"),EGray4));
		if (ret == KErrNotSupported)
			TRAP(ret,screendevice=CFbsScreenDevice::NewL(_L("scdv"),EColor4K));
		if (ret != KErrNone)
			User::Panic(_L("screendevice failed"),ret);
		screendevice->CreateContext((CGraphicsContext*&)bitgc);
		break;
	case EGcDevice1:
	case EGcDevice2:
	case EGcRegion1:
	case EGcRegion2:
	case EUseBrush1:
	case EUseBrush2:
	case EUseBrush3:
	case EUseFont:
		TRAP(ret,bitgc=CFbsBitGc::NewL());
		break;
		};

	switch (option)
		{
	case EPolygonFiller:
		polyfill->Construct(NULL,CGraphicsContext::EAlternate);
		break;
	case EBitmapDevice:
		(void)CFbsBitmapDevice::NewL(NULL);
		break;
	case EDrawBmp1:
		bitgc->DrawBitmap(TPoint(0,0),NULL);
		break;
	case EDrawBmp2:
		bitgc->DrawBitmap(TRect(0,0,0,0),NULL);
		break;
	case EDrawBmp3:
		bitgc->DrawBitmap(TRect(0,0,0,0),NULL,TRect(0,0,0,0));
		break;
	case EGcDevice1:
		bitgc->Resized();
		break;
	case EGcDevice2:
		bitgc->MapColors(TRect(0,0,0,0),NULL,0,EFalse);
		break;
	case EGcRegion1:
		region.ForceError();
		bitgc->SetClippingRegion(&region);
		break;
	case EGcRegion2:
		region.AddRect(TRect(-1,-1,1,1));
		bitgc->SetClippingRegion(&region);
		break;
	case EUseBrush1:
		bitgc->UseBrushPattern(NULL);
		break;
	case EUseBrush2:
		bitgc->UseBrushPattern((CFbsBitmap*)bitgc);
		break;
	case EUseBrush3:
		bitgc->UseBrushPattern(0);
		break;
	case EUseFont:
		bitgc->UseFont(NULL);
		break;
	case EDrawText1:
		bitgc->DrawText(_L("abc"),TPoint(0,0));
		break;
	case EDrawText2:
		bitgc->DrawText(_L("abc"),TRect(0,0,1,1),0,0);
		break;
	case EDrawText3:
		bitgc->DrawTextVertical(_L("abc"),EFalse);
		break;
	default:
		User::Panic(_L("Default panic"),KErrGeneral);
		};

	return KErrNone;
	}
//-------
__CONSTRUCT_STEP__(Auto)

void CTAutoStep::TestSetupL()
	{
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	}
	
void CTAutoStep::TestClose()
	{
	RFbsSession::Disconnect();
	}
