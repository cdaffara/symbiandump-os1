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
// TDrawComposite template class is used for the testing. When instantiated, it expects two
// template arguments - TDrawFunctor and TDrawParam:
// 1. TDrawFunctor class. It represents the "functor/function object" pattern and 
// must implement  "void operator()(CFbsBitGc* aGraphicsContext, TDrawParam* aPrm)" method.
// 2. TDrawParam class. This is a template argument for 
// "void TDrawFunctor::operator()(CFbsBitGc* aGraphicsContext, TDrawParam* aPrm)" method.
// If the overloaded "()" operator does not need a parameter, TEmpty class can be used as 
// a template parameter.
// When you instantiate TDrawComposite template class and call TDrawComposite::Draw() method,
// it will perform all pre- and post- drawing steps, calling 
// "void TDrawFunctor::operator()(CFbsBitGc* aGraphicsContext, TDrawParam* aPrm)" 
// at the right time.
// If you want to do the test step by step, set "aCallGetch" parameter of TDrawComposite's
// constructor to ETrue, when using it.
// 
//

#include <hal.h>
#include "TBitgdiScaling.h"

//
//Constants
//
//X and Y scaling factors.
const TInt KScalingFactorX = 3;
const TInt KScalingFactorY = 2;
//Test bitmap
_LIT(KTestBmp, "z:\\system\\data\\BmCTest.mbm");

//This clas might be used as a TDrawParam template parameter
class TEmpty
	{
	};

//This clas is used for testing CFbsBitGc drawing methods against the new scaling functionality.
template <class TDrawFunctor, class TDrawParam> class TDrawComposite
	{
public:
	TDrawComposite(const TPoint& aOrigin, TDrawFunctor& aFunctor, 
				   TDrawParam* aDrawParam, TBool aCallGetch = EFalse) :
		iOrigin(aOrigin),
		iFunctor(aFunctor),
		iPrm(aDrawParam),
		iCallGetch(aCallGetch)
		{
		}
	void Draw(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext, CTGraphicsBase* aTest)
		{
		__ASSERT_DEBUG(aScreenDevice, User::Invariant());
		__ASSERT_DEBUG(aGraphicsContext, User::Invariant());
	
		PreDrawStep(aScreenDevice, aGraphicsContext);
		iFunctor(aGraphicsContext, iPrm);
		TInt err = aScreenDevice->SetScalingFactor(iOrigin, KScalingFactorX, KScalingFactorY, 1, 1);
		aTest -> TEST2(err, KErrNone);
		aGraphicsContext->Activate(aScreenDevice);
		iFunctor(aGraphicsContext, iPrm);
		PostDrawStep(aScreenDevice, aGraphicsContext);
		err = aScreenDevice->SetScalingFactor(TPoint (0, 0), 1, 1, 1, 1);
		aTest -> TEST2(err, KErrNone);
		aGraphicsContext->Activate(aScreenDevice);
		}
private:
	void PreDrawStep(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
		{
		aGraphicsContext->Clear();
		TSize screenSize = aScreenDevice->SizeInPixels();
		TRect rc(iOrigin.iX, iOrigin.iY, screenSize.iWidth - 1, screenSize.iHeight - 1);
		aGraphicsContext->DrawRect(rc);
		aScreenDevice->Update();
		}
	void PostDrawStep(CFbsScreenDevice* aScreenDevice, CFbsBitGc*)
		{
		aScreenDevice->Update();
		}

private:
	TPoint iOrigin;
	TDrawFunctor& iFunctor;
	TDrawParam* iPrm;
	TBool iCallGetch;
	};

//
//TDrawTextFunctor class is used for CFbsBitGc text drawing/scaling tests
class TDrawTextFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, CFont* aFont)
		{
		__ASSERT_DEBUG(aFont, User::Invariant());
		_LIT(KTestText, "ABCDEFGH-0123456789");
		_LIT(KTestText2, "ijklmnopqrst");
		TPoint textPos = TPoint(0, aFont->AscentInPixels());
		TPoint textPos2 = TPoint(3, aFont->AscentInPixels() + 3);
		TInt text2Width = aFont->TextWidthInPixels(KTestText2);
		TPoint textPos3 = TPoint(aFont->HeightInPixels() * 3, aFont->AscentInPixels() + text2Width + 3);

		aGraphicsContext->DrawText(KTestText, textPos);
		aGraphicsContext->DrawTextVertical(KTestText2, textPos2, EFalse);
		aGraphicsContext->DrawTextVertical(KTestText2, textPos3, ETrue);
		}
	};


//
//TDrawBitmapFunctor class is used for CFbsBitGc bitmap (DrawBitmap) drawing/scaling tests
class TDrawBitmapFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, CFbsBitmap* aBitmap)
		{
		__ASSERT_DEBUG(aBitmap, User::Invariant());
		TSize size = aBitmap->SizeInPixels();
		aGraphicsContext->DrawBitmap(TRect(2, 2, size.iWidth + 2, size.iHeight + 2), aBitmap);
		}
	};


//
//TBitBltMaskedFunctor class is used for CFbsBitGc::BitBltMasked drawing/scaling tests
struct TMaskedBitmapPrm
	{
	CFbsBitmap* iBitmap;
	CFbsBitmap* iMaskBitmap;
	};
class TBitBltMaskedFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, TMaskedBitmapPrm* aPrm)
		{
		__ASSERT_DEBUG(aPrm, User::Invariant());
		TSize size = aPrm->iBitmap->SizeInPixels();
		TPoint pt(2, 2);
		TPoint pt2(pt.iX + size.iWidth + 10, pt.iY);
		TRect rc(pt, size);
		aGraphicsContext->BitBlt(pt, aPrm->iBitmap);
		aGraphicsContext->BitBltMasked(pt2, aPrm->iBitmap, rc, aPrm->iMaskBitmap, EFalse);
		}
	};

//-------------
CTBitgdiScaling::CTBitgdiScaling(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	
	}

CTBitgdiScaling::~CTBitgdiScaling()
	{
	delete iScreenDevice;
	delete iGraphicsContext;
	delete iScreenDevice2;
	delete iGraphicsContext2;
	}
	
void CTBitgdiScaling::ConstructL()
	{
#if defined __WINS__ || defined __WINSCW__
 	ChangeScreenDeviceL();
#endif//defined __WINS__ || defined __WINSCW__

	TDisplayMode theDisplayMode = (static_cast<CTBitgdiScalingStep*>(iStep))->DisplayMode();
	_LIT(KLog,"Screen Display Mode %S");
	INFO_PRINTF2(KLog,&ColorModeName(theDisplayMode));
	TInt err = CreateScreenDeviceAndContextL(theDisplayMode, iScreenDevice, iGraphicsContext);
	TEST(err == KErrNone);
	
	err = CreateScreenDeviceAndContextL(theDisplayMode, iScreenDevice2, iGraphicsContext2);
	TSize size = iScreenDevice->SizeInPixels();
	_LIT(KSize,"[%d,%d] screen pixels");
	INFO_PRINTF3(KSize, size.iWidth, size.iHeight);
	TEST(err == KErrNone);
	}
	
void CTBitgdiScaling::RunTestCaseL(TInt aCurTestCase)
	{
	((CTBitgdiScalingStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0032"));
		FontScalingL(iScreenDevice, iGraphicsContext);
		break;
	case 2:
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0033"));
		BitmapScalingL(iScreenDevice, iGraphicsContext);
		break;
	case 3:
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0034"));
		BitBltMaskedScalingL(iScreenDevice, iGraphicsContext);
		break;
	case 4:
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0035"));
		DrawingScalingL(iScreenDevice, iGraphicsContext);
		break;
	case 5:
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0036"));
		MapColors(iScreenDevice, iGraphicsContext);
		break;
	case 6:
/**
  @SYMTestCaseID GRAPHICS-BITGDI-0117
*/
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0117"));
		BitmapScalingL(iScreenDevice, iGraphicsContext, iScreenDevice2, iGraphicsContext2);
		break;
	case 7:
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0037"));
		RunTests2L();
		return;
	case 8:
		((CTBitgdiScalingStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTBitgdiScalingStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTBitgdiScalingStep*)iStep)->RecordTestResultL();
	}

//This function creates screen device and graphics context objects and pushesh them on the
//cleanup stack.
TInt CTBitgdiScaling::CreateScreenDeviceAndContextL(TDisplayMode aDisplayMode,
									CFbsScreenDevice*& aScreenDevice,
									CFbsBitGc*& aGraphicsContext)
	{
	__ASSERT_DEBUG(!aScreenDevice, User::Invariant());
	__ASSERT_DEBUG(!aGraphicsContext, User::Invariant());
	TRAPD(err, aScreenDevice = CFbsScreenDevice::NewL(KNullDesC, aDisplayMode));
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to create screen device for mode %S  err=%d");
		INFO_PRINTF3(KLog,&ColorModeName(aDisplayMode),err);
		}
	if(err == KErrNotSupported)
		{
		return err;
		}
	TEST(err == KErrNone);
	err = aScreenDevice->CreateContext((CGraphicsContext*&)aGraphicsContext);
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to create screen graphics context  mode %S  err=%d");
		INFO_PRINTF3(KLog,&ColorModeName(aDisplayMode),err);
		}
	TEST(err == KErrNone);
	aGraphicsContext->SetUserDisplayMode(aDisplayMode);
	aScreenDevice->ChangeScreenDevice(NULL);
	aScreenDevice->SetAutoUpdate(EFalse);
	return err;
	}

#if defined __WINS__ || defined __WINSCW__
//This test function might be used to debug ChangeScreenDevice() call.
void CTBitgdiScaling::ChangeScreenDeviceL()
	{
	CFbsScreenDevice* screenDevice1=NULL;
	TInt leaveErr=KErrNone;;
	TInt depth1= EColor256;

	// Try to get a colour screendevice
	for (;depth1<EColorLast && !screenDevice1;depth1++)
		{
		TRAP(leaveErr,screenDevice1 = CFbsScreenDevice::NewL(KNullDesC, TDisplayMode(depth1)));
		}
	if (leaveErr != KErrNone || !screenDevice1)
		{
		// Try to get a greyscale screendevice as failed to get a colour one
		for (depth1=ENone;depth1<EColor256 && !screenDevice1;depth1++)
			{
			TRAP(leaveErr,screenDevice1 = CFbsScreenDevice::NewL(KNullDesC, TDisplayMode(depth1)));
			}
		if (leaveErr != KErrNone || !screenDevice1)
			{
			INFO_PRINTF1(_L("Failed to create any screen devices. Re-leaving"));
			User::Leave(leaveErr);
			}
		}
	CleanupStack::PushL(screenDevice1);

	CFbsBitGc* graphicsContext1 = NULL;
	User::LeaveIfError(screenDevice1->CreateContext(graphicsContext1));
	CleanupStack::PushL(graphicsContext1);
	graphicsContext1->SetUserDisplayMode(EColor256);
	screenDevice1->ChangeScreenDevice(NULL);
	screenDevice1->SetAutoUpdate(EFalse);

	TInt err = screenDevice1->SetScalingFactor(TPoint(440, 40), 1, 1, 1, 1);
	TEST(err == KErrNone);
	graphicsContext1->Activate(screenDevice1);
	TRect rect1;
	screenDevice1->GetDrawRect(rect1);

	CFbsScreenDevice* screenDevice2 = NULL;
	for (TInt depth2=ENone;depth2<EColorLast && ! screenDevice1;depth2++)
		if (depth2!=depth1)
			{
			TRAP(leaveErr,screenDevice2 = CFbsScreenDevice::NewL(KNullDesC, TDisplayMode(depth2)))
			}
	if (leaveErr != KErrNone || !screenDevice2)
		{
			INFO_PRINTF1(_L("Failed to create a different screen device - test skipped"));
		
		}
	else
		{
		CleanupStack::PushL(screenDevice2);
		TRect rect2;
		screenDevice2->GetDrawRect(rect2);

		screenDevice1->ChangeScreenDevice(screenDevice2);
		graphicsContext1->Activate(screenDevice1);
		screenDevice1->GetDrawRect(rect1);
		TEST(rect1 == rect2);
		TRegionFix<1> area(rect2);
		graphicsContext1->SetClippingRegion(&area);
		::CleanupStack::PopAndDestroy(screenDevice2);
		}
	::CleanupStack::PopAndDestroy(2);//screenDevice1 and graphicsContext1
	}
#endif//defined __WINS__ || defined __WINSCW__

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0032
 
  @SYMDEF             

  @SYMTestCaseDesc FontScalingL function is used for CFbsBitGc text drawing/scaling tests
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests the drawing of some text to screen in specified height
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
//
void CTBitgdiScaling::FontScalingL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	__ASSERT_DEBUG(aScreenDevice, User::Invariant());
	__ASSERT_DEBUG(aGraphicsContext, User::Invariant());
	INFO_PRINTF1(_L("Font Scaling"));
	const TInt KTypeFacesCnt = aScreenDevice->NumTypefaces();
	for(TInt i=0;i<KTypeFacesCnt;++i)
		{
		TTypefaceSupport typeFaceSupport;
		aScreenDevice->TypefaceSupport(typeFaceSupport, i);
		TFontSpec fontSpec;
		fontSpec.iTypeface = typeFaceSupport.iTypeface;
		fontSpec.iHeight = 14;
		CFont* font = NULL;
		User::LeaveIfError(aScreenDevice->GetNearestFontToDesignHeightInPixels(font, fontSpec));
		TDesC& fontName = fontSpec.iTypeface.iName;
		RDebug::Print(_L("%S\r\n"), &fontName);
		aGraphicsContext->UseFont(font);
		DrawTestText(aScreenDevice, aGraphicsContext, font);
		aScreenDevice->ReleaseFont(font);
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0033
 
  @SYMDEF             

  @SYMTestCaseDesc BitmapScalingL fucntion is used for CFbsBitGc bitmap (DrawBitmap) drawing/scaling tests
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Loads in a number of bitmaps then scales them to the screen
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTBitgdiScaling::BitmapScalingL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("Bitmap Scaling"));
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	//8 - it is the number of bitmaps in mbm file - see GenBitmaps.mk where this mbm file is generated
	for(TInt i=0;i<8;i++)
		{
		bitmap->Reset();
		User::LeaveIfError(bitmap->Load(KTestBmp, i));
		TSize size = bitmap->SizeInPixels();
		TPoint ptOrigin(size.iWidth + 10, 3);

		TDrawBitmapFunctor functor;
		TDrawComposite<TDrawBitmapFunctor, CFbsBitmap> composite(ptOrigin, functor, bitmap);
		composite.Draw(aScreenDevice, aGraphicsContext, this);
		}
	CleanupStack::PopAndDestroy(bitmap);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0034
 
  @SYMDEF             

  @SYMTestCaseDesc BitBltMaskedScalingL fucntion is used for CFbsBitGc::BitBltMasked drawing/scaling tests
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Loads in a number of bitmaps and their masks then scales them to the screen
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/	
void CTBitgdiScaling::BitBltMaskedScalingL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("BitBltMasked Scaling"));

	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Load(KTestBmp, 0));
	TSize size = bitmap->SizeInPixels();

	CFbsBitmap* maskBitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(maskBitmap);
	User::LeaveIfError(maskBitmap->Create(size, EGray256));

	TBitmapUtil bmpUtil(maskBitmap);
	bmpUtil.Begin(TPoint(0, 0));
	for(TInt i=0;i<size.iWidth;++i)
		{
		for(TInt j=0;j<size.iHeight;++j)
			{
			bmpUtil.SetPos(TPoint(i, j));
			bmpUtil.SetPixel(0x00555555);
			}
		}
	bmpUtil.End();

	TPoint ptOrigin(size.iWidth * 3, 7);
	TMaskedBitmapPrm prm = {bitmap, maskBitmap};
	TBitBltMaskedFunctor functor;
	TDrawComposite<TBitBltMaskedFunctor, TMaskedBitmapPrm> composite(ptOrigin, functor, &prm);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	CleanupStack::PopAndDestroy(2);//maskBitmap and bitmap
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0035
 
  @SYMDEF             

  @SYMTestCaseDesc DrawingScalingL function is used for CFbsBitGc::DrawXXX drawing/scaling tests
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests the scaling various basic graphic primitives
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTBitgdiScaling::DrawingScalingL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	DrawArc(aScreenDevice, aGraphicsContext);
	DrawPie(aScreenDevice, aGraphicsContext);
	DrawRoundRect(aScreenDevice, aGraphicsContext);
	DrawPolyLineL(aScreenDevice, aGraphicsContext);
	DrawPolyLineNoEndPointL(aScreenDevice, aGraphicsContext);
	DrawPolygonL(aScreenDevice, aGraphicsContext);
	DrawEllipse(aScreenDevice, aGraphicsContext);
	DrawLine(aScreenDevice, aGraphicsContext);
	DrawRect(aScreenDevice, aGraphicsContext);
	ShadowFade(aScreenDevice, aGraphicsContext);
	}

//
//TMapColorsFunctor class is used for CFbsBitGc::MapColors drawing/scaling tests
class TMapColorsFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, TEmpty*)
		{
		aGraphicsContext->DrawRect(TRect(7, 13, 45, 67));
		TRgb colors[] = {TRgb(0x00, 0x00, 0x00), TRgb(0xFF, 0x20, 0x20)};
		aGraphicsContext->MapColors(TRect(5, 10, 50, 70), colors);
		}
	};

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0036
 
  @SYMDEF             

  @SYMTestCaseDesc MapColors function is used for CFbsBitGc::MapColors drawing/scaling tests
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions tests scaling of mapp3ed colours
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
//
void CTBitgdiScaling::MapColors(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("MapColors scaling"));
	TMapColorsFunctor functor;
	TDrawComposite<TMapColorsFunctor, TEmpty> composite(TPoint(83, 11), functor, NULL);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	}

//BitmapScalingL fucntion is used for CFbsBitGc bitmap (DrawBitmap) drawing/scaling tests
//2 screen devices used - scaled and non-scaled
void CTBitgdiScaling::BitmapScalingL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext, 
						   CFbsScreenDevice* aScreenDevice2, CFbsBitGc* aGraphicsContext2,
						   TBool aCallGetch)
	{
	__ASSERT_DEBUG(aScreenDevice, User::Invariant());
	__ASSERT_DEBUG(aGraphicsContext, User::Invariant());
	__ASSERT_DEBUG(aScreenDevice2, User::Invariant());
	__ASSERT_DEBUG(aGraphicsContext2, User::Invariant());

	INFO_PRINTF1(_L("Bitmap Scaling - 2 devices"));

	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);

	//8 - it is the number of bitmaps in mbm file - see GenBitmaps.mk where this mbm file is generated
	for(TInt i=0;i<8;i++)
		{
		bitmap->Reset();
		User::LeaveIfError(bitmap->Load(KTestBmp, i));
		TSize size = bitmap->SizeInPixels();
		TPoint ptOrigin(size.iWidth + 10, 3);

		TInt err = aScreenDevice2->SetScalingFactor(ptOrigin, KScalingFactorX, KScalingFactorY, 1, 1);
		TEST(err == KErrNone);
		aGraphicsContext2->Activate(aScreenDevice2);

		aGraphicsContext->Clear();
		aGraphicsContext2->Clear();

		TSize screenSize = aScreenDevice->SizeInPixels();
		TRect rc(0, 0, screenSize.iWidth - 1, screenSize.iHeight - 1);
		aGraphicsContext->DrawRect(rc);

		aGraphicsContext->DrawBitmap(TRect(2, 2, size.iWidth + 2, size.iHeight + 2), bitmap);

		TSize screenSize2 = aScreenDevice2->SizeInPixels();
		TRect rc2(0, 0, screenSize2.iWidth - 1, screenSize2.iHeight - 1);
		aGraphicsContext2->DrawRect(rc2);

		aGraphicsContext2->DrawBitmap(TRect(2, 2, size.iWidth + 2, size.iHeight + 2), bitmap);

		aScreenDevice->Update();
		aScreenDevice2->Update();

		if(aCallGetch)
			{
			//TheTest.Getch();
			}
		}
	CleanupStack::PopAndDestroy(bitmap);
	}

//DrawTestText function is used for CFbsBitGc text drawing/scaling tests
void CTBitgdiScaling::DrawTestText(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext, CFont* aFont)
	{
	__ASSERT_DEBUG(aFont, User::Invariant());
	TPoint ptOrigin(aFont->HeightInPixels() * 4, aFont->AscentInPixels() * 2);
	TDrawTextFunctor functor;
	TDrawComposite<TDrawTextFunctor, CFont> composite(ptOrigin, functor, aFont);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	}

//
//TDrawArcFunctor class is used for CFbsBitGc::DrawArc drawing/scaling tests
class TDrawArcFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, TEmpty*)
		{
		aGraphicsContext->DrawArc(TRect(0, 0, 40, 40), TPoint(0, 20), TPoint(40, 20));
		}
	};

//DrawArc function is used for CFbsBitGc::DrawArc drawing/scaling tests
void CTBitgdiScaling::DrawArc(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawArc scaling"));
	TDrawArcFunctor functor;
	TDrawComposite<TDrawArcFunctor, TEmpty> composite(TPoint(40, 0), functor, NULL);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	}

//
//TDrawPieFunctor class is used for CFbsBitGc::DrawPie drawing/scaling tests
class TDrawPieFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, TEmpty*)
		{
		aGraphicsContext->DrawPie(TRect(0, 0, 40, 40), TPoint(0, 20), TPoint(40, 20));
		}
	};

//DrawPie function is used for CFbsBitGc::DrawPie drawing/scaling tests
void CTBitgdiScaling::DrawPie(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawPie scaling"));
	TDrawPieFunctor functor;
	TDrawComposite<TDrawPieFunctor, TEmpty> composite(TPoint(40, 0), functor, NULL);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	}

//
//TDrawRoundRectFunctor class is used for CFbsBitGc::DrawRoundRect drawing/scaling tests
class TDrawRoundRectFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, TEmpty*)
		{
		aGraphicsContext->DrawRoundRect(TRect(0, 0, 40, 40), TSize(5, 5));
		}
	};

//DrawRoundRect function is used for CFbsBitGc::DrawRoundRect drawing/scaling tests
void CTBitgdiScaling::DrawRoundRect(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawRoundRect scaling"));
	TDrawRoundRectFunctor functor;
	TDrawComposite<TDrawRoundRectFunctor, TEmpty> composite(TPoint(40, 0), functor, NULL);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	}

//

typedef CArrayFixFlat<TPoint> CPointArray;

//TDrawPolyLineFunctor class is used for CFbsBitGc::DrawPolyLine drawing/scaling tests
class TDrawPolyLineFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, CPointArray* aPoints)
		{
		aGraphicsContext->DrawPolyLine(aPoints);
		}
	};

//DrawPolyLineL function is used for CFbsBitGc::DrawPolyLine drawing/scaling tests
void CTBitgdiScaling::DrawPolyLineL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawPolyLine scaling"));
	CPointArray* points = new (ELeave) CPointArray (4);
	CleanupStack::PushL(points);
	TPoint pt(1, 1);
	points->AppendL(pt);
	pt.SetXY(77, 23);
	points->AppendL(pt);
	pt.SetXY(38, 63);
	points->AppendL(pt);
	pt.SetXY(70, 51);
	points->AppendL(pt);
	TDrawPolyLineFunctor functor;
	TDrawComposite<TDrawPolyLineFunctor, CPointArray> composite(TPoint(80, 0), functor,
																points);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	CleanupStack::PopAndDestroy(points);
	}

//
//TDrawPolyLineNoEndPointFunctor class is used for CFbsBitGc::DrawPolyLineNoEndPoint drawing/scaling tests
class TDrawPolyLineNoEndPointFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, CPointArray* aPoints)
		{
		aGraphicsContext->DrawPolyLineNoEndPoint(aPoints);
		}
	};

//DrawPolyLineNoEndPointL function is used for CFbsBitGc::DrawPolyLineNoEndPoint drawing/scaling tests
void CTBitgdiScaling::DrawPolyLineNoEndPointL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawPolyLineNoEndPoint scaling"));
	CPointArray* points = new (ELeave) CPointArray (4);
	CleanupStack::PushL(points);
	TPoint pt(1, 1);
	points->AppendL(pt);
	pt.SetXY(77, 23);
	points->AppendL(pt);
	pt.SetXY(38, 63);
	points->AppendL(pt);
	pt.SetXY(70, 51);
	points->AppendL(pt);
	TDrawPolyLineNoEndPointFunctor functor;
	TDrawComposite<TDrawPolyLineNoEndPointFunctor, CPointArray> composite(
																	TPoint(80, 0), functor,
																	points);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	CleanupStack::PopAndDestroy(points);
	}

//
//TDrawPolygonFunctor class is used for CFbsBitGc::DrawPolygon drawing/scaling tests
class TDrawPolygonFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, CPointArray* aPoints)
		{
		aGraphicsContext->DrawPolygon(aPoints);
		}
	};

//DrawPolygon function is used for CFbsBitGc::DrawPolygon drawing/scaling tests
void CTBitgdiScaling::DrawPolygonL(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawPolygon scaling"));
	CPointArray* points = new (ELeave) CPointArray (6);
	CleanupStack::PushL(points);
	TPoint pt(1, 20);
	points->AppendL(pt);
	pt.SetXY(23, 3);
	points->AppendL(pt);
	pt.SetXY(61, 29);
	points->AppendL(pt);
	pt.SetXY(51, 47);
	points->AppendL(pt);
	pt.SetXY(31, 39);
	points->AppendL(pt);
	pt.SetXY(44, 17);
	points->AppendL(pt);
	TDrawPolygonFunctor functor;
	TDrawComposite<TDrawPolygonFunctor, CPointArray> composite(TPoint(80, 0), functor,
															   points);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	CleanupStack::PopAndDestroy(points);
	}

//
//TDrawEllipseFunctor class is used for CFbsBitGc::DrawEllipse drawing/scaling tests
class TDrawEllipseFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, TEmpty*)
		{
		aGraphicsContext->DrawEllipse(TRect(11, 13, 40, 70));
		}
	};

//DrawEllipse function is used for CFbsBitGc::DrawEllipse drawing/scaling tests
void CTBitgdiScaling::DrawEllipse(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawEllipse scaling"));
	TDrawEllipseFunctor functor;
	TDrawComposite<TDrawEllipseFunctor, TEmpty> composite(TPoint(50, 27), functor, NULL);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	}

//
//TDrawLineFunctor class is used for CFbsBitGc::DrawLine drawing/scaling tests
class TDrawLineFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, TEmpty*)
		{
		aGraphicsContext->DrawLine(TPoint(7, 13), TPoint(45, 67));
		aGraphicsContext->DrawLineTo(TPoint(33, 53));
		aGraphicsContext->DrawLineBy(TPoint(11, 53));
		}
	};

//DrawLine function is used for CFbsBitGc::DrawLineXX drawing/scaling tests
void CTBitgdiScaling::DrawLine(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawLineXX scaling"));
	TDrawLineFunctor functor;
	TDrawComposite<TDrawLineFunctor, TEmpty> composite(TPoint(83, 11), functor, NULL);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	}

//
//TDrawRectFunctor class is used for CFbsBitGc::DrawRect drawing/scaling tests
class TDrawRectFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, TEmpty*)
		{
		aGraphicsContext->DrawRect(TRect(7, 13, 45, 67));
		}
	};

//DrawRect function is used for CFbsBitGc::DrawRect drawing/scaling tests
void CTBitgdiScaling::DrawRect(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("DrawRect scaling"));
	TDrawRectFunctor functor;
	TDrawComposite<TDrawRectFunctor, TEmpty> composite(TPoint(83, 11), functor, NULL);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	}

//
//TShadowFadeFunctor class is used for CFbsBitGc::ShadowArea/FadeArea drawing/scaling tests
class TShadowFadeFunctor
	{
public:
	void operator()(CFbsBitGc* aGraphicsContext, TEmpty*)
		{
		aGraphicsContext->SetBrushStyle(CGraphicsContext::ESolidBrush);

		aGraphicsContext->SetBrushColor(TRgb(0xBB, 0x34, 0x55));
		aGraphicsContext->DrawRect(TRect(1, 1, 40, 20));
		aGraphicsContext->DrawRect(TRect(20, 20, 40, 40));
		RRegion shadowRgn;
		shadowRgn.AddRect(TRect(1, 1, 40, 20));
		shadowRgn.AddRect(TRect(20, 20, 40, 40));
		aGraphicsContext->ShadowArea(&shadowRgn);
		shadowRgn.Close();

		aGraphicsContext->SetBrushColor(TRgb(0xFF, 0x00, 0xFF));
		aGraphicsContext->DrawRect(TRect(3, 25, 17, 33));
		aGraphicsContext->DrawRect(TRect(1, 43, 67, 55));
		RRegion fadeRgn;
		fadeRgn.AddRect(TRect(3, 25, 17, 33));
		fadeRgn.AddRect(TRect(1, 43, 67, 55));
		aGraphicsContext->FadeArea(&fadeRgn);
		fadeRgn.Close();

		aGraphicsContext->SetBrushColor(TRgb(0xFF, 0xFF, 0xFF));
		aGraphicsContext->SetBrushStyle(CGraphicsContext::ENullBrush);
		}
	};

//ShadowFade function is used for CFbsBitGc::ShadowArea/FadeArea drawing/scaling tests
void CTBitgdiScaling::ShadowFade(CFbsScreenDevice* aScreenDevice, CFbsBitGc* aGraphicsContext)
	{
	INFO_PRINTF1(_L("Shadow/Fade scaling"));
	TShadowFadeFunctor functor;
	TDrawComposite<TShadowFadeFunctor, TEmpty> composite(TPoint(83, 11), functor, NULL);
	composite.Draw(aScreenDevice, aGraphicsContext, this);
	}

//--------------
__CONSTRUCT_STEP__(BitgdiScaling)

void CTBitgdiScalingStep::TestSetupL()
	{
	iDisplayMode = GetDisplayModeL();
	}

TDisplayMode CTBitgdiScalingStep::GetDisplayModeL()
	{
	CFbsScreenDevice* device = NULL;
	TDisplayMode mode = EColor64K;
	TRAPD(err, device = CFbsScreenDevice::NewL(KNullDesC, mode));
	if (err == KErrNotSupported)
		{
		mode = EColor256;
		TRAP(err, device = CFbsScreenDevice::NewL(KNullDesC, mode));
		}
	if (err == KErrNotSupported)
		{
		mode = EColor16MA;
		TRAP(err, device = CFbsScreenDevice::NewL(KNullDesC, mode));
		}
	if (err == KErrNotSupported)
		{
		mode = EColor16MAP;
		TRAP(err, device = CFbsScreenDevice::NewL(KNullDesC, mode));
		}
	TESTL(err == KErrNone);
	delete device;
	return mode;
	}
