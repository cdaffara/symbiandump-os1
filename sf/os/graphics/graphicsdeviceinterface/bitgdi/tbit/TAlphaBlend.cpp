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

#include <hal.h>
#include <e32math.h>
#include "TAlphaBlend.h"


const TInt KWidth = 10;//Used in alpha blending tests
const TInt KHeight = 3;//Used in alpha blending tests
const TInt KMaximumAttempts = 2;	// Allow retries on some tests, due to spurious InfoPrints 
TBool iExtraLogging1=EFalse;		//Used to trigger logging at times the test fails
TBool iExtraLogging2=EFalse;		//Used to trigger logging at times the test fails

CTAlphaBlending::CTAlphaBlending(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iDevice(NULL),
	iGc(NULL)
	{
	}

CTAlphaBlending::~CTAlphaBlending()
	{
	DeleteGraphicsContext();
	DeleteScreenDevice();
	}

void CTAlphaBlending::RunTestCaseL(TInt aCurTestCase)
	{
	((CTAlphaBlendingStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTAlphaBlendingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0024"));
		INFO_PRINTF1(_L("Alpha blending"));
		TestAlphaBlendingL();
		break;		
	case 2:
/**
@SYMTestCaseID		GRAPHICS-BITGDI-0114
*/
		((CTAlphaBlendingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0114"));
		INFO_PRINTF1(_L("Alpha blending 2"));
		TestAlphaBlending2L();
		break;
	case 3:
		((CTAlphaBlendingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0085"));
 		INFO_PRINTF1(_L("Alpha blending Correctness test 16MU 16MA"));
 		TestAlphaBlendCorrect(EColor16MU, EColor16MU);
 		break;
 	case 4:
 		((CTAlphaBlendingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0097"));
 		INFO_PRINTF1(_L("Alpha plot test"));
		TestAlphaBlendingPlotL();
		break;
	case 5:
/**
@SYMTestCaseID		GRAPHICS-BITGDI-0115
*/
		((CTAlphaBlendingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0115"));
		INFO_PRINTF1(_L("Draw bitmap blending"));
		DoDrawBitmapTestsL();
		break;
	case 6:
		//DEF118268
		((CTAlphaBlendingStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0085"));
		INFO_PRINTF1(_L("Alpha blending Correctness test 16M 16MA"));
		TestAlphaBlendCorrect(EColor16M, EColor16MA);
		break;
	case 7:
		((CTAlphaBlendingStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTAlphaBlendingStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTAlphaBlendingStep*)iStep)->RecordTestResultL();
	}

TInt CTAlphaBlending::CreateScreenDevice(TDisplayMode aDisplayMode, CFbsBitGc::TGraphicsOrientation aOrientation)
	{
	DeleteGraphicsContext();
	DeleteScreenDevice();
	TRAPD(err, iDevice = CFbsScreenDevice::NewL(_L("scdv"), aDisplayMode));
	if(err == KErrNotSupported)
		{
		return err;
		}
	TEST2(err, KErrNone);
	err = iDevice->CreateContext((CGraphicsContext*&)iGc);
	TEST2(err, KErrNone);
	if (!iGc->SetOrientation(aOrientation))
		{
		return(KErrNotSupported);
		}
	iGc->SetUserDisplayMode(aDisplayMode);
	iDevice->ChangeScreenDevice(NULL);
	iDevice->SetAutoUpdate(EFalse);
	return err;
	}

void CTAlphaBlending::DeleteScreenDevice()
	{
	delete iDevice;
	iDevice = NULL;
	}

void CTAlphaBlending::DeleteGraphicsContext()
	{
	delete iGc;
	iGc = NULL;
	}

// returns the pixel colour from the provided bitmap in aarrggbb format
// if pixel is outside the bitmaps limits return top left pixel
TUint32 CTAlphaBlending::GetRawPixel(CFbsBitmap* aBitmap, TPoint aPos)
	{
	TBitmapUtil bmpUtil(aBitmap);	
	TUint32 value = 0;
	ASSERT(aPos.iX>=0 && aPos.iY>=0);
	ASSERT(aPos.iX<aBitmap->SizeInPixels().iWidth && aPos.iY<aBitmap->SizeInPixels().iHeight);
	bmpUtil.Begin(aPos);
	value = bmpUtil.GetPixel();
	bmpUtil.End();
	return value;	
	}

/**
@SYMTestCaseID		GRAPHICS-BITGDI-0097

@SYMDEF             DEF113229

@SYMTestCaseDesc    CDrawThirtyTwoBppBitmapCommon::WriteRgb did not change the dest alpha value to 255 when 
					the dest alpha was >0 and <255 and the a soure pen had an alpha of 255	

@SYMTestPriority    Normal

@SYMTestStatus      Implemented

@SYMTestActions     Creates a bitmap, clears it to black (destination) 50% opaque then plots a series of points 
					with a 100% opaque pen on it. Tests the resultant alpha value.

@SYMTestExpectedResults Final alpha value should be 255
**/
void CTAlphaBlending::TestAlphaBlendingPlotL()
	{
	const TSize KRectSize(100,100);
	const TRect KTargetRect(TPoint(0,0), KRectSize);
		
	// create the target bitmap
	CFbsBitmap* destBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(destBmp);
	User::LeaveIfError(destBmp->Create(KRectSize, EColor16MA));
	destBmp->SetSizeInTwips(KRectSize);
	
	// create bitmap device and graphics context
	CFbsBitmapDevice* destBmpDevice = CFbsBitmapDevice::NewL(destBmp);
	CleanupStack::PushL(destBmpDevice);
	CFbsBitGc* destGc = NULL;
	User::LeaveIfError(destBmpDevice->CreateContext(destGc));
	CleanupStack::PushL(destGc);
	destGc->SetPenStyle(CGraphicsContext::ENullPen);
	destGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		
	TDisplayMode screenMode = EColor16MA;
	TInt err = CreateScreenDevice(screenMode);
	if (err != KErrNone)
		{
		screenMode = EColor64K;
		err = CreateScreenDevice(screenMode);
		}
	
	if(err==KErrNone)
		{
		const TInt KSqrMin=45;
		const TInt KSqrMax=55;
						
		iGc->SetUserDisplayMode(screenMode);
		destGc->SetBrushColor(TRgb(0,0,0,127));
		destGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
		destGc->Clear(KTargetRect);

		// copy over to screen dc for anyone watching
		iGc->BitBlt(TPoint(0,0), destBmp, KTargetRect);
		iDevice->Update();
		
		// set the pen colour to white and plot some points
		destGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
		destGc->SetPenStyle(CGraphicsContext::ESolidPen);
		destGc->SetPenColor(TRgb(0,70,130,255));
		for( TInt y=KSqrMin;y<=KSqrMax;y++)
			{
			for( TInt x=KSqrMin;x<=KSqrMax;x++)
				{
				destGc->Plot(TPoint(x,y));
				}
			}
			
		// copy over to screen dc for anyone watching
		iGc->BitBlt(TPoint(0,0), destBmp, KTargetRect);
		iDevice->Update();
		
		TUint32 actualValue=0;
		// check the resulting values alpha values are 0xFF in the square we drew
		for( TInt y=KSqrMin;y<=KSqrMax;y++)
			{
			for( TInt x=KSqrMin;x<=KSqrMax;x++)
				{
				actualValue = GetRawPixel(destBmp, TPoint(x,y));
				if( (actualValue&0xFF000000) != 0xFF000000 )
					{
					TEST(EFalse);
					INFO_PRINTF2(_L("TestAlphaBlendingPlotL() ***FAILED*** - expected alpha value 0xFF got %d "),((actualValue&0xFF000000)>>24));
					}
				}
			}
		}
		CleanupStack::PopAndDestroy(3); //destGc,destBmpDevice,destBmp
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0024
 
  @SYMDEF             

  @SYMTestCaseDesc 
   
	System, GT0173 System Libraries, BITGDI support required for semi-transparent windows
	DEF039083 - Using BitBltMasked to do alpha-blending only works if your brush style is "null"
	DEF039409 - Wrong part of Alpha Bitmap is used when there is a clipping region 
	DEF039669 - The UserDisplayMode is not honoured during the AlphaBlendBitmap function. 
	REQ3413 Second overload of CFbsBitGc's AlphaBlendBitmaps - "Supply a second overload of 
	CFbsBitGc's AlphaBlendBitmaps function, but with one of the source bitmaps being passed 
	as a CFbsBitGc object, so that the screen can be used as a source."

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 
  	
  	Tests alpha blending - for all display modes, some brush styles, user display modes, 
	different positions on the screen.
  
	Shadow/fade mode is no more tested, because the existing BitBltMasked() and the new 
	AlphaBlendBitmaps() methods treat it a different way - see MAlphaBlend interface in
	ScreenDriver component.

  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/

void CTAlphaBlending::TestAlphaBlendingL()
	{
	TPoint originPt(0, 0);
	TPoint destPt(0, 0);
	TRect scrRc1(0, 0, KWidth, KHeight);
	TPoint srcPt2(0, 0);
	TPoint alphaPt(0, 0);
	//
	//If we compare CFbsBitGc::BitBltMasked() aguments with CFbsBitGc::AlphaBlending() arguments,
	//we will see that AlphaBlending() has more arguments than BitBltMasked() - 
	//srcPt2, alphaPt. To make it possible - the comparison between these two methods,
	//we have to change aAlphaPt and aSrcPt2 values accordingly with the changes of scrRc1 value.
	//
	//test 1 - the origin is moved
	originPt = TPoint(97, 33);
	DoAlphaBlendingTestsL(originPt, destPt, scrRc1, srcPt2, alphaPt);
#if !defined(__X86GCC__)	//These test take too long to run in X86GCC
	//test 2 - the origin is (0, 0)
	originPt = TPoint(0, 0);
	DoAlphaBlendingTestsL(originPt, destPt, scrRc1, srcPt2, alphaPt);
	//test 3 - scrRect1 is not (0, 0, KWidth, KHeight)
	scrRc1 = TRect(3, 1, KWidth, KHeight);
	alphaPt = TPoint(3, 1);
	DoAlphaBlendingTestsL(originPt, destPt, scrRc1, srcPt2, alphaPt);
	//test 4 - restore scrRc1 and alphaPt, move the destination point
	scrRc1 = TRect(0, 0, KWidth, KHeight);
	alphaPt = TPoint(0, 0);
	destPt = TPoint(13, 17);
	iExtraLogging1=ETrue;
	DoAlphaBlendingTestsL(originPt, destPt, scrRc1, srcPt2, alphaPt);
	iExtraLogging1=EFalse;
#endif	//__X86GCC__
	}

void CTAlphaBlending::DoDrawBitmapTestsL()
	{
	TDisplayMode modes[] = {EColor16MA, EColor16MAP, EColor16MU, EColor16M, EColor256, EColor4K, EColor64K, 
						   EGray256, EGray16, EGray4, EGray2, EColor16};
	const TInt KNumTestDisplayModes=sizeof(modes)/sizeof(modes[0]);
	for(TInt modeIndex=0;modeIndex<KNumTestDisplayModes;modeIndex++)
		{
		TDisplayMode screenMode=modes[modeIndex];
		if (CreateScreenDevice(screenMode)!=KErrNone)
			continue;
		DoDrawBitmapTestL(screenMode);
		}
	}

void CTAlphaBlending::DoDrawBitmapTestL(TDisplayMode aTestDisplayMode)
	{
	iGc->Reset();
	TBool alphaSupported=(aTestDisplayMode==EColor16MA || aTestDisplayMode==EColor16MAP);
	TSize screenSize=iDevice->SizeInPixels();
//
	const TInt KNumTestSrcSizes=4;
	const TSize testSrcSizes[KNumTestSrcSizes]={TSize(2,2),TSize(20,10),TSize(200,5),TSize(55,555)};
	for(TInt srcSizeIndex=0;srcSizeIndex<KNumTestSrcSizes;srcSizeIndex++)
		{
		TSize srcSize(testSrcSizes[srcSizeIndex]);
//
		CFbsBitmap* srcBmp=new(ELeave) CFbsBitmap;
		CleanupStack::PushL(srcBmp);
		User::LeaveIfError(srcBmp->Create(srcSize,aTestDisplayMode));
		CFbsBitmapDevice* srcDevice = CFbsBitmapDevice::NewL(srcBmp);
		CleanupStack::PushL(srcDevice);
		CFbsBitGc* srcGc=NULL;
		User::LeaveIfError(srcDevice->CreateContext(srcGc));
		CleanupStack::PushL(srcGc);
//
		CFbsBitmap* srcAlpha=new(ELeave) CFbsBitmap;
		CleanupStack::PushL(srcAlpha);
		User::LeaveIfError(srcAlpha->Create(srcSize,aTestDisplayMode));
		CFbsBitmapDevice* srcAlphaDevice = CFbsBitmapDevice::NewL(srcAlpha);
		CleanupStack::PushL(srcAlphaDevice);
		CFbsBitGc* srcAlphaGc=NULL;
		User::LeaveIfError(srcAlphaDevice->CreateContext(srcAlphaGc));
		CleanupStack::PushL(srcAlphaGc);
		srcAlphaGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
//
		CFbsBitmap* srcMask=new(ELeave) CFbsBitmap;
		CleanupStack::PushL(srcMask);
		User::LeaveIfError(srcMask->Create(srcSize,EGray256));
		CFbsBitmapDevice* srcMaskDevice = CFbsBitmapDevice::NewL(srcMask);
		CleanupStack::PushL(srcMaskDevice);
		CFbsBitGc* srcMaskGc=NULL;
		User::LeaveIfError(srcMaskDevice->CreateContext(srcMaskGc));
		CleanupStack::PushL(srcMaskGc);
//
		TInt maxX=srcSize.iWidth-1;
		TInt maxY=srcSize.iHeight-1;
		for(TInt yLoop=0;yLoop<srcSize.iHeight;yLoop++)
			{
			for(TInt xLoop=0;xLoop<srcSize.iWidth;xLoop++)
				{
				TPoint plotPos(xLoop,yLoop);
				TRgb pen(xLoop*255/maxX,yLoop*255/maxY,(xLoop+yLoop)*128/(maxX+maxY));
				srcGc->SetPenColor(pen);
				srcGc->Plot(plotPos);
				TInt alpha=(xLoop+yLoop)*255/(maxX+maxY);
				pen.SetAlpha(alpha);
				srcAlphaGc->SetBrushColor(pen);
				srcAlphaGc->Clear(TRect(plotPos,TSize(1,1)));
				srcMaskGc->SetPenColor(TRgb::Gray256(alpha));
				srcMaskGc->Plot(plotPos);
				}
			}
		const TInt KNumTestTargSizes=5;
		const TSize testTargSizes[KNumTestTargSizes]={TSize(0,0), TSize(20,1),TSize(30,20),TSize(200,31),TSize(55,5)};
		for(TInt targSizeIndex=0;targSizeIndex<KNumTestSrcSizes;targSizeIndex++)
			{
			TSize targSize(testTargSizes[targSizeIndex]);
			if (targSizeIndex==0)
				{
				targSize=srcSize;	// Special case with no scaling
				if (targSize.iWidth>screenSize.iWidth)
					targSize.iWidth=screenSize.iWidth;
				TInt maxHeight=screenSize.iHeight/3;
				if (targSize.iHeight>maxHeight)
					targSize.iHeight=maxHeight;
				}
//
			CFbsBitmap* targBmp=new(ELeave) CFbsBitmap;
			CleanupStack::PushL(targBmp);
			User::LeaveIfError(targBmp->Create(targSize,aTestDisplayMode));
			CFbsBitmapDevice* targBmpDevice = CFbsBitmapDevice::NewL(targBmp);
			CleanupStack::PushL(targBmpDevice);
			CFbsBitGc* targGc=NULL;
			User::LeaveIfError(targBmpDevice->CreateContext(targGc));
			CleanupStack::PushL(targGc);
//
			CFbsBitmap* targMask=new(ELeave) CFbsBitmap;
			CleanupStack::PushL(targMask);
			User::LeaveIfError(targMask->Create(targSize,EGray256));
			CFbsBitmapDevice* targMaskDevice = CFbsBitmapDevice::NewL(targMask);
			CleanupStack::PushL(targMaskDevice);
			CFbsBitGc* targMaskGc=NULL;
			User::LeaveIfError(targMaskDevice->CreateContext(targMaskGc));
			CleanupStack::PushL(targMaskGc);
//
			TPoint drawPos;
			TRect testRect1(targSize);
			iGc->Clear();
// First we pre-stretch the source and mask bitmaps into temp bitmaps
			targGc->DrawBitmap(TRect(targSize),srcBmp);
			targMaskGc->DrawBitmap(TRect(targSize),srcMask);
// Then blend them onto the screen with a call to BitBltMasked
			iGc->BitBltMasked(drawPos,targBmp,TRect(targSize),targMask,EFalse);
			drawPos.iY+=targSize.iHeight;
			TRect testRect2(drawPos,targSize);
// Next we combine the stretching and masking with one call to DrawBitmapMasked,
// this should give the same end result.
			iGc->DrawBitmapMasked(testRect2,srcBmp,TRect(srcSize),srcMask,EFalse);
			TRect testRect3;
			if (alphaSupported)
				{
// Finally if alpha blending supported we stretch and blend, again to achieve the exact same end result
// as the two previous calls. This was specificially done to catch DEF116427.
				drawPos.iY+=targSize.iHeight;
				testRect3=TRect(drawPos,targSize);
				iGc->DrawBitmap(testRect3,srcAlpha);
				}
//Use this just to check what we've put in the test bitmaps
/*
			drawPos.iY+=targSize.iHeight+1;
			iGc->BitBlt(drawPos,srcBmp);
			drawPos.iY+=srcSize.iHeight+1;
			iGc->BitBlt(drawPos,srcMask);
			drawPos.iY+=srcSize.iHeight+1;
			if (alphaSupported)
				iGc->BitBlt(drawPos,srcAlpha);
*/
		   	iDevice->Update();
			TBool ret1=iDevice->RectCompare(testRect1,*iDevice,testRect2);
		   	TBool ret2=alphaSupported?iDevice->RectCompare(testRect1,*iDevice,testRect3):ETrue;
			if (!ret1 || !ret2)
				{
				INFO_PRINTF4(_L("DrawBitmapTest, ret1=%d, ret2=%d, Screen mode=%d"),ret1,ret2,aTestDisplayMode);
				TEST(EFalse);
				}
			CleanupStack::PopAndDestroy(3,targMask);
			CleanupStack::PopAndDestroy(3,targBmp);
			}
		CleanupStack::PopAndDestroy(3,srcMask);
		CleanupStack::PopAndDestroy(3,srcAlpha);
		CleanupStack::PopAndDestroy(3,srcBmp);
		}
	}

//Tests alpha blending - for all display modes, some brush styles, user display modes, 
//different positions on the screen.
void CTAlphaBlending::DoAlphaBlendingTestsL(const TPoint& aOrigin,
										const TPoint& aDestPt, 
										const TRect& aSrcRc1, 
										const TPoint& aScrPt2,
										const TPoint& aAlphaPt)
	{
	TBuf<128> buf;
	_LIT(KLog,"Origin=(%d,%d) DestPt=(%d,%d) SrcRect=(%d,%d,%d,%d) ScrPt=(%d,%d) AlphaPt=(%d,%d)");
	buf.Format(KLog,aOrigin.iX,aOrigin.iY,aDestPt.iX,aDestPt.iY,aSrcRc1.iTl.iX,aSrcRc1.iTl.iY
						,aSrcRc1.iBr.iX,aSrcRc1.iBr.iY,aScrPt2.iX,aScrPt2.iY,aAlphaPt.iX,aAlphaPt.iY);
	INFO_PRINTF1(buf);
	TDisplayMode mode[] = {EColor16MA, EColor16MAP, EColor16MU, EColor16M, EColor256, EColor4K, EColor64K,
						   EGray256, EGray16, EGray4, EGray2, EColor16};
	CFbsBitmap* screenBmp = NULL;
	CFbsBitmap* srcBmp = NULL;
	CFbsBitmap* alphaBmp = NULL;
	CGraphicsContext::TBrushStyle brushStyle[] = {CGraphicsContext::ENullBrush, CGraphicsContext::ESolidBrush};
	for(TInt i=0;i<TInt(sizeof(mode)/sizeof(mode[0]));i++)
		{
		for(TInt orientation=0;orientation<=CFbsBitGc::EGraphicsOrientationRotated270;orientation++)
			{
			if (CreateScreenDevice(mode[i],(CFbsBitGc::TGraphicsOrientation)orientation) != KErrNone)
				{
				continue;
				}
			iExtraLogging2=(iExtraLogging1 && mode[i]==11);
	 		INFO_PRINTF3(_L("Mode=%d, Orientation=%d"), mode[i], orientation);
			CreateAlphaBlendingBitmapsLC(screenBmp, srcBmp, alphaBmp, mode[i]);
			for(TInt j=0;j<TInt(sizeof(brushStyle)/sizeof(brushStyle[0]));j++)
				{
				if(mode[i] == EGray4 && brushStyle[j] == CGraphicsContext::ESolidBrush)
					{
					//When the display mode is EGray4 - Flicker-free blitting is not enabled.
					//The screen will be filled with 0xFF color, which is not the same color used by
					//the test - 0x00. And BitBltMasked and AlphaBlendBitmaps will produce different results.
					continue;
					}
				iGc->SetOrigin(aOrigin);
				iGc->SetBrushStyle(brushStyle[j]);
				TDisplayMode userDisplayMode[] = {EColor16MA, EColor16MAP, EColor16MU, EColor16M, EColor256, 
												  EColor4K, EColor64K, EGray256, 
												  EGray16, EGray4, EGray2, EColor16};
				for(TInt l=0;l<TInt(sizeof(userDisplayMode)/sizeof(userDisplayMode[0]));l++)
					{
					iGc->SetUserDisplayMode(userDisplayMode[l]);
					TSize scrDevSize = iDevice->SizeInPixels();
					TRect clipRc[] = {TRect(0, 0, scrDevSize.iWidth, scrDevSize.iHeight),
										TRect(0, 1, scrDevSize.iWidth, scrDevSize.iHeight), // Tests Y clipping only
										TRect(5, 0, scrDevSize.iWidth, scrDevSize.iHeight), // Tests X clipping only
										TRect(5, 1, scrDevSize.iWidth, scrDevSize.iHeight),
										TRect(3, 0, 14, 23)};//14 and 23 values are not accidental! 
															 //Sometimes the method is called with aDestPt(13, 17).
					for(TInt k=0;k<TInt(sizeof(clipRc)/sizeof(clipRc[0]));k++)
						{
						if (iExtraLogging2)
							{
							_LIT(KLog,"  BrushStyle=%d UserDisplayMode=%d ClipRect=(%d,%d,%d,%d)");
			 				INFO_PRINTF7(KLog,j,l,clipRc[k].iTl.iX,clipRc[k].iTl.iY,clipRc[k].iBr.iX,clipRc[k].iBr.iY);
							}
						iGc->Clear();
						iGc->SetClippingRect(clipRc[k]);
						DoAlphaBlendingTestL(screenBmp, srcBmp, alphaBmp, aDestPt, aSrcRc1, aScrPt2, aAlphaPt);
						iGc->CancelClippingRect();
						}
					}//end of - for(TInt l=0;l<TInt(sizeof(userDisplayMode)/sizeof(userDisplayMode[0]));l++)
				iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
				}
			DestroyAlphaBlendingBitmaps(screenBmp, srcBmp, alphaBmp);
			}//end of - for(TInt j=0;j<TInt(sizeof(brushStyle)/sizeof(brushStyle[0]));j++)
		}//end of - for(TInt i=0;i<TInt(sizeof(mode)/sizeof(mode[0]));i++)
	}

//The method compares results of two alpha blending methods:
//iGc->BitBltMasked() and iGc->AlphaBlendBitmaps().
//To make that possible, aScreenBmp is copied to the screen before the call of
//iGc->BitBltMasked().
void CTAlphaBlending::DoAlphaBlendingTestL(CFbsBitmap* aScreenBmp, 
									   const CFbsBitmap* aSrcBmp, 
									   const CFbsBitmap* aAlphaBmp,
									   const TPoint& aDestPt, 
									   const TRect& aSrcRc1, 
									   const TPoint& aSrcPt2,
									   const TPoint& aAlphaPt)
	{
	_LIT(KScreenBmpFile, "C:\\BMP_DATA.DAT");
	iGc->SetShadowMode(EFalse);
   	TInt i;
   	TInt res = -1;
	TSize scrDevSize = iDevice->SizeInPixels();
	TInt allocatedSize = scrDevSize.iWidth * scrDevSize.iHeight * 2;
	TRect screenBmpRc;
   	//The screen alpha blended data after calling of BitBltMasked() - will be filled after the call
	TUint8* screenBmpDestData1 = new (ELeave) TUint8[allocatedSize];
	CleanupStack::PushL(screenBmpDestData1);
   	//The screen alpha blended data after calling of AlphaBlendingBitmaps() - will be filled after the call
	TUint8* screenBmpDestData2 = new (ELeave) TUint8[allocatedSize];
	CleanupStack::PushL(screenBmpDestData2);
	// Allow an effective restart of the test, since sometimes there are spurious
	// InfoPrints that affect the comparisons.
	for (TInt attempt = 0; res && (attempt < KMaximumAttempts); attempt++)
		{
		//Fill the blocks with some default value
		Mem::Fill(screenBmpDestData1, allocatedSize, 0xCA);
		Mem::Fill(screenBmpDestData2, allocatedSize, 0xCA);
	   	//Check screen bitmap size
	   	TSize screenBmpSize = aScreenBmp->SizeInPixels();
	   	if ((screenBmpSize.iWidth != KWidth) || (screenBmpSize.iHeight != KHeight))
	   		{
	   		_LIT(KScreenErr,"DoAlphaBlendingTestL test: w:%d!=%d || h:%d!=%d");
	   		INFO_PRINTF5(KScreenErr, screenBmpSize.iWidth, KWidth, screenBmpSize.iHeight, KHeight);
	   		TEST(EFalse);
	   		}
		//  Alpha blending using CFbsBitGc::BitBltMasked  //
		if (iExtraLogging2)
			{
			_LIT(KLog1,"    CFbsBitGc::BitBltMasked test");
			INFO_PRINTF1(KLog1);
			}
		//Screen bitmap rectangle
		screenBmpRc.SetRect(aDestPt, TSize(aSrcRc1.Width(), aSrcRc1.Height()));
		
		//Save screen bitmap 
		TInt saveAttempts = 5;
		TInt err = aScreenBmp->Save(KScreenBmpFile);
		while ((err != KErrNone) && saveAttempts--)
			{
			// Retry the save
			_LIT(KSaveRetry,"DoAlphaBlendingTestL: Bitmap save failed, retrying.");
			INFO_PRINTF1(KSaveRetry);
			User::After(10000); 
			err = aScreenBmp->Save(KScreenBmpFile);
			}
		User::LeaveIfError(err);
		
		User::LeaveIfError(aScreenBmp->Resize(TSize(aSrcRc1.Width(), aSrcRc1.Height())));
		//Draw screen bitmap
	   	iGc->Clear();
		iGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
		iGc->DrawBitmap(screenBmpRc, aScreenBmp);
	   	iDevice->Update();
	   	//Do BitBltMasked()
	   	iGc->BitBltMasked(aDestPt, aSrcBmp, aSrcRc1, aAlphaBmp, EFalse);
	   	iDevice->Update();
		//Get screen data and write the data to screenBmpDestData1.
		for(i=0;i<scrDevSize.iHeight;i++)
	   		{
			TPtr8 p(screenBmpDestData1 + i * scrDevSize.iWidth * 2, scrDevSize.iWidth * 2, scrDevSize.iWidth * 2);
			iDevice->GetScanLine(p, TPoint(0, i), scrDevSize.iWidth, EColor64K);
	   		}
		//  Alpha blending using explicit form of CFbsBitGc::AlphaBlendBitmaps  //
		if (iExtraLogging2)
			{
			_LIT(KLog2,"    CFbsBitGc::AlphaBlendBitmaps explicit test");
			INFO_PRINTF1(KLog2); 
			}
		//Clear screen
	   	iGc->Clear();
	   	iDevice->Update();
		//Load screen bitmap 
		User::LeaveIfError(aScreenBmp->Load(KScreenBmpFile));
	   	//Do AlphaBlendBitmaps()
	   	User::LeaveIfError(iGc->AlphaBlendBitmaps(aDestPt, aSrcBmp, aScreenBmp, aSrcRc1, 
													aSrcPt2, aAlphaBmp, aAlphaPt));
	   	iDevice->Update();
		//Get screen data and write the data to screenBmpDestData2.
		for(i=0;i<scrDevSize.iHeight;i++)
	   		{
			TPtr8 p(screenBmpDestData2 + i * scrDevSize.iWidth * 2, scrDevSize.iWidth * 2, scrDevSize.iWidth * 2);
			iDevice->GetScanLine(p, TPoint(0, i), scrDevSize.iWidth, EColor64K);
	   		}
		//Compare screen bitmaps //
		res = Mem::Compare(screenBmpDestData1, allocatedSize, screenBmpDestData2, allocatedSize);

		// colour comparison tolerance between RGB565 components.
		const TInt KColourComparisonTolerance = 2;
		if (res)
			{
			TBool testPassed = ETrue;
			// strict byte-for-byte comparison of the pixel maps could have failed because blending algorithms may  
			// differ slightly, but actual resulting colours may be close enough
			for (int byteNumber = 0; byteNumber < allocatedSize; byteNumber+=2)
				{
				// find any RGB565 value that doesn't match and examine each component
				if ( *(TUint16*)(screenBmpDestData1 + byteNumber) != *(TUint16*)(screenBmpDestData2 + byteNumber))
					{
					TUint16 Rgb1 = *(TUint16*)(screenBmpDestData1 + byteNumber);
					TUint16 Rgb2 = *(TUint16*)(screenBmpDestData2 + byteNumber);
					
					TInt16 Red1 = (Rgb1 & 0xF800) >> 11;
					TInt16 Red2 = (Rgb2 & 0xF800) >> 11;
					TInt16 DiffRed = Abs(Red1 - Red2);
					
					TInt16 Green1 = (Rgb1 & 0x07E0) >> 5;
					TInt16 Green2 = (Rgb2 & 0x07E0) >> 5;
					TInt16 DiffGreen = Abs(Green1 - Green2);
					
					TInt16 Blue1 = (Rgb1 & 0x001F);
					TInt16 Blue2 = (Rgb2 & 0x001F);
					TInt16 DiffBlue = Abs(Blue1 - Blue2);
					
					// is any difference is outside the tolerance break out signaling test failure
					if (DiffRed > KColourComparisonTolerance || 
						DiffGreen > KColourComparisonTolerance || 
						DiffBlue > KColourComparisonTolerance)
						{
						testPassed = EFalse;
						break;
						}
					}
				}
			if (testPassed)
				{
				res = 0;
				}
			}
		
		if (res && (attempt < KMaximumAttempts - 1))
			{
			INFO_PRINTF1(_L("Memory comparison 1 failed, retrying"));
			// Skip to next attempt
			continue;
			}
		TEST(res == 0);

		//  Alpha blending using implicit form of CFbsBitGc::AlphaBlendBitmaps  //
		if (iExtraLogging2)
			{
			_LIT(KLog3,"    CFbsBitGc::AlphaBlendBitmaps implicit test");
			INFO_PRINTF1(KLog3);
			}
		//Clear screen
	   	iGc->Clear();
	   	iDevice->Update();
		//Draw screen bitmap (it's already loaded)
		User::LeaveIfError(aScreenBmp->Resize(TSize(aSrcRc1.Width(), aSrcRc1.Height())));
		iGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
		iGc->DrawBitmap(screenBmpRc, aScreenBmp);
	   	iDevice->Update();
	   	//Do AlphaBlendBitmaps()
	   	User::LeaveIfError(iGc->AlphaBlendBitmaps(aDestPt, aSrcBmp, aSrcRc1, aAlphaBmp, aSrcRc1.iTl));
	   	iDevice->Update();
		//Get screen data and write the data to screenBmpDestData2.
		for(i=0;i<scrDevSize.iHeight;i++)
	   		{
			TPtr8 p(screenBmpDestData2 + i * scrDevSize.iWidth * 2, scrDevSize.iWidth * 2, scrDevSize.iWidth * 2);
			iDevice->GetScanLine(p, TPoint(0, i), scrDevSize.iWidth, EColor64K);
	   		}
		//Compare screen bitmaps //
		if (iExtraLogging2)
			{
			_LIT(KLog4,"    Compare Screen Bitmaps");
			INFO_PRINTF1(KLog4);
			}
		res = Mem::Compare(screenBmpDestData1, allocatedSize, screenBmpDestData2, allocatedSize);

		if (res && (attempt < KMaximumAttempts - 1))
			{
			INFO_PRINTF1(_L("Memory comparison 2 failed, retrying"));
			}
		
		// Reload the screen bitmap as it was before:
		User::LeaveIfError(aScreenBmp->Load(KScreenBmpFile));
		}
	TEST(res == 0);
	
	//Destroy the allocated memory blocks
	CleanupStack::PopAndDestroy(2);//screenBmpDestData1 & screenBmpDestData2
	}

void CTAlphaBlending::CreateAlphaBlendingBitmapsLC(CFbsBitmap*& aScreenBmp, 
											   CFbsBitmap*& aSrcBmp, 
											   CFbsBitmap*& aAlphaBmp,
											   TDisplayMode aMode)
	{
	TInt i;
	TSize size(KWidth, KHeight);
	//The screen data
	TUint8 screenBmpSrcData[KHeight][KWidth * 3 + 2] = // "+ 2" - every row is aligned to a 32 bit boundary
		{//0               1                 2                 3                 4                 5                 6                 7                 8                 9     
		{0x00, 0x00, 0x00, 0x20, 0x21, 0x22, 0x30, 0x31, 0x32, 0x40, 0x41, 0x42, 0x50, 0x51, 0x51, 0x60, 0x61, 0x62, 0x70, 0x71, 0x72, 0x80, 0x81, 0x82, 0x90, 0x91, 0x92, 0xA0, 0xA1, 0xA2, 0x00, 0x00},
		{0x19, 0x18, 0x17, 0x29, 0x28, 0x27, 0x39, 0x38, 0x37, 0x49, 0x48, 0x47, 0x59, 0x58, 0x57, 0x69, 0x68, 0x67, 0x79, 0x78, 0x77, 0x89, 0x88, 0x87, 0x99, 0x98, 0x97, 0xA9, 0xA8, 0xA7, 0x00, 0x00},
		{0x1F, 0x1E, 0x1D, 0x2F, 0x2E, 0x2D, 0x3F, 0x3E, 0x3D, 0x4F, 0x4E, 0x4D, 0x5F, 0x5E, 0x5D, 0x6F, 0x6E, 0x6D, 0x7F, 0x7E, 0x7D, 0x8F, 0x8E, 0x8D, 0x9F, 0x9E, 0x9D, 0xAF, 0xAE, 0xAD, 0x00, 0x00}
		};
	//The source bitmap data
	TUint8 srcBmpData[KHeight][KWidth * 3 + 2] = // "+ 2" - every row is aligned to a 32 bit boundary
		{//0               1                 2                 3                 4                 5                 6                 7                 8                 9     
		{0x32, 0x67, 0xA2, 0x11, 0x34, 0x67, 0xC5, 0xA3, 0x91, 0x01, 0xB3, 0xA8, 0xF3, 0x3F, 0x1E, 0x88, 0x11, 0x12, 0xAE, 0xEE, 0x9A, 0x56, 0x12, 0x81, 0xB4, 0xCA, 0x91, 0xFF, 0x1A, 0x2A, 0x00, 0x00},
		{0x02, 0xB1, 0xE2, 0xAA, 0xBB, 0x13, 0x22, 0xA8, 0xC3, 0x75, 0x8D, 0xFF, 0xA4, 0xAB, 0x00, 0xC5, 0xA6, 0x22, 0xBB, 0x09, 0xC1, 0x97, 0x25, 0xC6, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0x00, 0x00},
		{0x32, 0x50, 0x76, 0x27, 0xCC, 0x45, 0x81, 0xE5, 0xE9, 0xB7, 0xCD, 0x11, 0x32, 0xB1, 0x23, 0xFF, 0x71, 0x11, 0xCC, 0xAA, 0xF2, 0x98, 0x13, 0x8C, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x00, 0x00}
		};
	//The alpha bitmap data
	TUint8 alphaBmpData[KHeight][KWidth + 2] = // "+ 2" - every row is aligned to a 32 bit boundary
		{//0   1     2     3     4     5     6     7     8     9     
		{0x68, 0x68, 0x68, 0xAC, 0xD9, 0xB2, 0x8F, 0x11, 0xA0, 0xC1, 0x00, 0x00},
		{0x71, 0x5A, 0xF6, 0xEE, 0xF9, 0xE5, 0x06, 0x4C, 0xBB, 0x7B, 0x00, 0x00},
		{0x9F, 0x99, 0x45, 0x17, 0xA8, 0xF5, 0xFF, 0xD2, 0x22, 0x1D, 0x00, 0x00}
		};

	if(aMode == EColor16MU)
		{
		
		TInt buffer_size = KWidth * 4 ;
		TUint8* buffer = new(ELeave) TUint8[buffer_size];
		TPtr8 source_ptr(buffer, buffer_size, buffer_size);

		//Screen bitmap
		aScreenBmp = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(aScreenBmp);
		User::LeaveIfError(aScreenBmp->Create(size, aMode));
		for(i=0; i<KHeight; i++)
			{
			TUint8* bufCur = buffer;
			TUint8* bufSrcCur = screenBmpSrcData[i];
			TUint8* bufSrcEnd = bufSrcCur + KWidth * 3;
			while(bufSrcCur < bufSrcEnd)
				{
				*bufCur++ = *bufSrcCur++;
				*bufCur++ = *bufSrcCur++;
				*bufCur++ = *bufSrcCur++;
				*bufCur++ = 0xff;
				}
			aScreenBmp->SetScanLine(source_ptr, i);
			}
			
		//Source bitmap
		aSrcBmp = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(aSrcBmp);
		User::LeaveIfError(aSrcBmp->Create(size, aMode));
		for(i=0; i<KHeight; i++)
			{
			TUint8* bufCur = buffer;
			TUint8* bufSrcCur = srcBmpData[i];
			TUint8* bufSrcEnd = bufSrcCur + KWidth * 3;
			while(bufSrcCur < bufSrcEnd)
				{
				*bufCur++ = *bufSrcCur++;
				*bufCur++ = *bufSrcCur++;
				*bufCur++ = *bufSrcCur++;
				*bufCur++ = 0xff;
				}
			aSrcBmp->SetScanLine(source_ptr, i);
			}
			
			
		//Alpha bitmap
		aAlphaBmp = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(aAlphaBmp);
		User::LeaveIfError(aAlphaBmp->Create(size, EGray256));
		for(i=0;i<KHeight;i++)
			{
			TPtr8 p(alphaBmpData[i], KWidth * 3, KWidth * 3);
			aAlphaBmp->SetScanLine(p, i);
			}
			
		delete [] buffer;	
		}
	else
		{
		//Screen bitmap
		aScreenBmp = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(aScreenBmp);
		User::LeaveIfError(aScreenBmp->Create(size, aMode));
		for(i=0;i<KHeight;i++)
			{
			TPtr8 p(screenBmpSrcData[i], KWidth * 3, KWidth * 3);
			aScreenBmp->SetScanLine(p, i);
			}
		//Source bitmap
		aSrcBmp = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(aSrcBmp);
		User::LeaveIfError(aSrcBmp->Create(size, aMode));
		for(i=0;i<KHeight;i++)
			{
			TPtr8 p(srcBmpData[i], KWidth * 3, KWidth * 3);
			aSrcBmp->SetScanLine(p, i);
			}
		//Alpha bitmap
		aAlphaBmp = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(aAlphaBmp);
		User::LeaveIfError(aAlphaBmp->Create(size, EGray256));
		for(i=0;i<KHeight;i++)
			{
			TPtr8 p(alphaBmpData[i], KWidth * 3, KWidth * 3);
			aAlphaBmp->SetScanLine(p, i);
			}
		}	
	}

void CTAlphaBlending::DestroyAlphaBlendingBitmaps(CFbsBitmap*& aScreenBmp, 
											  CFbsBitmap*& aSrcBmp, 
											  CFbsBitmap*& aAlphaBmp)
	{
	CleanupStack::PopAndDestroy(aAlphaBmp);
	aAlphaBmp = NULL;
	CleanupStack::PopAndDestroy(aSrcBmp);
	aSrcBmp = NULL;
	CleanupStack::PopAndDestroy(aScreenBmp);
	aScreenBmp = NULL;
	}

//The test doesn't check anything. It is for debugging only.
void CTAlphaBlending::TestAlphaBlending2L()
	{
	static const TDisplayMode modeList[] = {
			EColor64K, EColor256, EColor16MAP, EColor16MA, EColor16MU, EColor4K
	};
	const TInt KNumTestDisplayModes=sizeof(modeList)/sizeof(modeList[0]);
	for(TInt orientation=0;orientation<=CFbsBitGc::EGraphicsOrientationRotated270;orientation++)
		{
		CFbsBitmap* srcBmp = NULL;
		CFbsBitmap* alphaBmp = NULL;
		TInt err = KErrNotSupported;
		// Try several modes
		for (TInt modeIndex = 0; (err != KErrNone) && modeIndex < KNumTestDisplayModes; modeIndex++)
			{
			err = CreateScreenDevice(modeList[modeIndex],(CFbsBitGc::TGraphicsOrientation)orientation);
			}
		if (err == KErrNotSupported)
			{
			// Orientation not supported, try next one
			continue;
			}
		TInt j;
		//The source bitmap data
		TUint8 srcBmpData[100];
		for(j=0;j<100;j++)
			{
			srcBmpData[j] = 0xAA;
			}
		//The alpha bitmap data
		TUint8 alphaBmpData[20];
		for(j=0;j<20;j++)
			{
			alphaBmpData[j] = TUint8(j);
			}
		//Source bitmap
		srcBmp = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(srcBmp);
		User::LeaveIfError(srcBmp->Create(TSize(100, 1), EColor256));
		TPtr8 p1(srcBmpData, 100, 100);
		srcBmp->SetScanLine(p1, 0);
		//Alpha bitmap
		alphaBmp = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(alphaBmp);
		User::LeaveIfError(alphaBmp->Create(TSize(20, 1), EGray256));
		TPtr8 p2(alphaBmpData, 20, 20);
		alphaBmp->SetScanLine(p2, 0);
		//Do BitBltMasked()
		iGc->BitBltMasked(TPoint(20, 20), srcBmp, TRect(10, 0, 100, 1), alphaBmp, EFalse);
		iDevice->Update();
		iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
		//
		CleanupStack::PopAndDestroy(alphaBmp);
		CleanupStack::PopAndDestroy(srcBmp);
		}
	}

TUint32 AlphaBlend(TUint32 aDestPixel, TUint32 aSrcPixel, TUint8 aMask)
 	{
 	TUint32 dr = (aDestPixel & 0x00FF0000) >> 16;
 	TUint32 dg = (aDestPixel & 0x0000FF00) >> 8;
 	TUint32 db = aDestPixel & 0x000000FF;
 	
 	TUint32 sr = (aSrcPixel & 0x00FF0000) >> 16;
 	TUint32 sg = (aSrcPixel & 0x0000FF00) >> 8;
 	TUint32 sb = aSrcPixel & 0x000000FF;
 	
 	TUint32 rr = (aMask * sr)/255 + ((0xFF - aMask) * dr)/255;
 	TUint32 rg = (aMask * sg)/255 + ((0xFF - aMask) * dg)/255;
 	TUint32 rb = (aMask * sb)/255 + ((0xFF - aMask) * db)/255;
 	
 	return(rr << 16 | rg << 8 | rb | 0xff000000);
 	}
 
inline TUint32 OptimizedBlend32(TInt aPrimaryRed,TInt aPrimaryGreen,TInt aPrimaryBlue,TUint32 aSecondary,TUint8 aAlphaValue)
 	{
  
  	if(aAlphaValue == 0xff)
 		{
 		return (aPrimaryBlue + (aPrimaryGreen<<8) + (aPrimaryRed<<16)) | 0xff000000;
  		}
  	else
  		{
  		const TUint32 alphaValue = (aAlphaValue << 8) + aAlphaValue;
  
  		const TInt r2 = (aSecondary & 0x00ff0000) >> 16;
  		const TInt g2 = (aSecondary & 0x0000ff00) >> 8;
  		const TInt b2 = aSecondary & 0x000000ff;
  
  		const TInt r3 = ((alphaValue * (aPrimaryRed   - r2)) >> 16) + r2;
 		const TInt g3 = ((alphaValue * (aPrimaryGreen - g2)) >> 16) + g2;
 		const TInt b3 = ((alphaValue * (aPrimaryBlue  - b2)) >> 16) + b2;
 
 		return (b3 & 0xFF) | ((g3<<8) & 0xFF00) | ((r3<<16) & 0xFF0000) | 0xFF000000;
  		}
  	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0085
 
  @SYMDEF             

  @SYMTestCaseDesc 
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 
 
  @SYMTestExpectedResults 
*/  	
// This tests the correctness of the results of alpha-blending with EColor16MA
void CTAlphaBlending::TestAlphaBlendCorrect(TDisplayMode /* aScreenMode */, TDisplayMode /* aBitmapMode */)
 	{
 	// test data
 	TUint32 top = 0xFFCCEE55;
 	TUint32 beneath = 0xFFEEAA66;
 	TUint8 alpha = 0xF0;
 	
 	TInt maxDiff = 0;
 	
 	for(TInt i = 0; i < 100000; i++)
 		{
 		top = Math::Random();
 		beneath = Math::Random();
 		alpha = Math::Random();
 		TUint32 res1 = AlphaBlend(beneath, top, alpha);
 		TUint32 res2 = OptimizedBlend32((top >> 16) & 0xFF,(top >>8) & 0xFF,(top & 0xFF),beneath, alpha);
 		
 		if(res1 != res2)
 			{
 			TInt diff = 0;
 			TInt diff1 = res1 & 0xFF;
 			TInt diff2 = res2 & 0xFF;
 			
 			diff = diff1 - diff2;
 			
 			if(diff < 0)
 				diff*=-1;
 				
 			if(diff > maxDiff)
 				maxDiff = diff;
 				
 			diff1 = (res1 >> 8) & 0xFF;
 			diff2 = (res2 >> 8) & 0xFF;
 			
 			diff = diff1 - diff2;
 			
 			if(diff < 0)
 				diff*=-1;
 				
 			if(diff > maxDiff)
 				maxDiff = diff;
 			
 			
 			diff1 = (res1 >> 16) & 0xFF;
 			diff2 = (res2 >> 16) & 0xFF;
 			
 			diff = diff1 - diff2;
 			
 			if(diff < 0)
 				diff*=-1;
 				
 			if(diff > maxDiff)
 				maxDiff = diff;
 			
 			}
 		}
	
 	INFO_PRINTF1(_L("Results:"));
 		
 	if(maxDiff)
 		INFO_PRINTF2(_L("Max Diff = %i"), maxDiff);
 	else
 		INFO_PRINTF1(_L("Results are identical"));
  	}

//--------------
__CONSTRUCT_STEP__(AlphaBlending)

void CTAlphaBlendingStep::TestSetupL()
	{
	}
	
