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
// Screen device test code
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TSCRDEV.H"

//Define this to get visible pauses (in test 6: DeviceResSwitchL)
//#define VISIBLE_PAUSES

//Define this to get extra logging which may be useful in tracking down a fail
//#define EXTRA_LOGGING

CTScreenDevice::CTScreenDevice(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{}

CTScreenDevice::~CTScreenDevice()
	{}

void CTScreenDevice::ConstructL()
	{
	//The following is just another test... it doesn't leave any resources for use by the test class AFAICT...
	RWsSession aSession;
	CWsScreenDevice *device1;
	CWsScreenDevice *device2;
	CWsScreenDevice *device3;

	aSession.Connect();
	device1=new(ELeave) CWsScreenDevice(aSession);
	device1->Construct(iTest->iScreenNumber);
	delete device1;
	device1=new(ELeave) CWsScreenDevice(aSession);
	device1->Construct(iTest->iScreenNumber);
	device2=new(ELeave) CWsScreenDevice(aSession);
	device2->Construct(iTest->iScreenNumber);
	device3=new(ELeave) CWsScreenDevice(aSession);
	device3->Construct(iTest->iScreenNumber);
	delete device3;
	CFbsFont *font;
	User::LeaveIfError(device1->GetNearestFontToDesignHeightInTwips((CFont *&)font,TFontSpec()));
	RWindowGroup group(aSession);
	group.Construct(777);
	group.SetOwningWindowGroup(TheClient->iGroup->GroupWin()->Identifier());
	RWindow win(aSession);
	win.Construct(group,77);
	CWindowGc *gc=new(ELeave) CWindowGc(device1);
	gc->Construct();
	gc->Activate(win);
	gc->UseFont(font);
	device1->ReleaseFont(font);
	aSession.Flush();
	delete gc;
	win.Close();
	group.Close();
	delete device1;
	delete device2;
	aSession.Close();
	}


void CTScreenDevice::TestTwipPixelConversions()
	{
	TInt result=TheClient->iScreen->HorizontalTwipsToPixels(TheClient->iScreen->HorizontalPixelsToTwips(1234));
	TEST(result==1234);
	if (result!=1234)
		INFO_PRINTF3(_L("TheClient->iScreen->HorizontalTwipsToPixels(TheClient->iScreen->HorizontalPixelsToTwips(1234) return value  - Expected: %d, Actual: %d"), 1234, result);

	result=TheClient->iScreen->VerticalTwipsToPixels(TheClient->iScreen->VerticalPixelsToTwips(1234));
	TEST(result==1234);
	if (result!=1234)
		INFO_PRINTF3(_L("TheClient->iScreen->VerticalTwipsToPixels(TheClient->iScreen->VerticalPixelsToTwips(1234) return value  - Expected: %d, Actual: %d"), 1234, result);

	TSize pixels=TheClient->iScreen->SizeInPixels();
	TSize twips=TheClient->iScreen->SizeInTwips();
	TInt conv=TheClient->iScreen->HorizontalPixelsToTwips(pixels.iWidth);
	TEST(conv==twips.iWidth);
	if (conv!=twips.iWidth)
		INFO_PRINTF3(_L("TheClient->iScreen->HorizontalPixelsToTwips(pixels.iWidth) return value  - Expected: %d, Actual: %d"), twips.iWidth, conv);

	conv=TheClient->iScreen->VerticalPixelsToTwips(pixels.iHeight);
	TEST(conv==twips.iHeight);
	if (conv!=twips.iHeight)
		INFO_PRINTF3(_L("TheClient->iScreen->VerticalPixelsToTwips(pixels.iHeight) return value  - Expected: %d, Actual: %d"), twips.iHeight, conv);
	}

#define TEST_VALUE 100000000
void CTScreenDevice::TestTwipPixelOverflow()
	{
	TInt result=TheClient->iScreen->HorizontalTwipsToPixels(TheClient->iScreen->HorizontalPixelsToTwips(TEST_VALUE));
	TEST(result==TEST_VALUE);
	if (result!=TEST_VALUE)
		INFO_PRINTF3(_L("TheClient->iScreen->HorizontalTwipsToPixels(TheClient->iScreen->HorizontalPixelsToTwips(TEST_VALUE)) return value  - Expected: %d, Actual: %d"), TEST_VALUE, result);

	result=TheClient->iScreen->VerticalTwipsToPixels(TheClient->iScreen->VerticalPixelsToTwips(TEST_VALUE));
	TEST(result==TEST_VALUE);
	if (result!=TEST_VALUE)
		INFO_PRINTF3(_L("TheClient->iScreen->VerticalTwipsToPixels(TheClient->iScreen->VerticalPixelsToTwips(TEST_VALUE)) return value  - Expected: %d, Actual: %d"), TEST_VALUE, result);

	result = TheClient->iScreen->HorizontalPixelsToTwips(TEST_VALUE);
	TEST(result>0);
	if (!result>0)
		INFO_PRINTF3(_L("TheClient->iScreen->HorizontalPixelsToTwips(TEST_VALUE) return value  - Expected: %d less than, Actual: %d"), 0, result);

	result = TheClient->iScreen->VerticalPixelsToTwips(TEST_VALUE);
	TEST(result>0);
	if (!result>0)
		INFO_PRINTF3(_L("TheClient->iScreen->VerticalPixelsToTwips(TEST_VALUE) return value  - Expected: %d less than, Actual: %d"), 0, result);

	result = TheClient->iScreen->HorizontalTwipsToPixels(TEST_VALUE);
	TEST(result>0);
	if (!result>0)
		INFO_PRINTF3(_L("TheClient->iScreen->HorizontalTwipsToPixels(TEST_VALUE) return value  - Expected: %d less than, Actual: %d"), 0, result);

	result = TheClient->iScreen->VerticalTwipsToPixels(TEST_VALUE);
	TEST(result>0);
	if (!result>0)
		INFO_PRINTF3(_L("TheClient->iScreen->VerticalTwipsToPixels(TEST_VALUE) return value  - Expected: %d less than, Actual: %d"), 0, result);
	}

void CTScreenDevice::TwipsCacheFlushL()
	{
	RWsSession session;
	CWsScreenDevice* device;
	CFont* font;
	CFont* font2;
	const TFontSpec fontspec(KTestFontTypefaceName,250);
	TPixelsTwipsAndRotation sizeAndRotation;

	// create a CWsScreenDevice instance
	User::LeaveIfError(session.Connect());
	device=new(ELeave) CWsScreenDevice(session);
	device->Construct(iTest->iScreenNumber);

	// Record a font height for use by latter tests
	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips(font,fontspec));
	const TInt height0=font->HeightInPixels();
	device->ReleaseFont(font);

	// 1. find font with best-matching height in twips,
	// 2. change twips size of the screen (leaving pixel-size same),
	// 3. find font with best-matching height in twips,
	// 4. make sure that the newly found font is not the same as previous and has the different height in pixels
	device->GetDefaultScreenSizeAndRotation(sizeAndRotation);
#if defined(EXTRA_LOGGING)
	_LIT(KLog1,"Font height: %d for unchanged device.   pixels=%d,%d  twips=%d,%d");
	LOG_MESSAGE6(KLog1,height0,sizeAndRotation.iPixelSize.iWidth,sizeAndRotation.iPixelSize.iHeight
						,sizeAndRotation.iTwipsSize.iWidth,sizeAndRotation.iTwipsSize.iHeight);
#endif
	sizeAndRotation.iTwipsSize.iHeight=2000;
	sizeAndRotation.iTwipsSize.iWidth =3000;
	device->SetScreenSizeAndRotation(sizeAndRotation);

	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips(font,fontspec));
	TInt height=font->HeightInPixels();
#if defined(EXTRA_LOGGING)
	_LIT(KLog2,"Font height: %d for device with twips set to 2000x3000");
	LOG_MESSAGE2(KLog2,height);
#endif
	TFontSpec returnedSpec=font->FontSpecInTwips();

	sizeAndRotation.iTwipsSize.iHeight=4000;
	sizeAndRotation.iTwipsSize.iWidth =6000;
	device->SetScreenSizeAndRotation(sizeAndRotation);

	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips(font2,fontspec));
	TInt height2=font2->HeightInPixels();
	TFontSpec returnedSpec2 = font2->FontSpecInTwips();
#if defined(EXTRA_LOGGING)
	_LIT(KLog3,"Font height: %d for device with twips set to 4000x6000");
	LOG_MESSAGE2(KLog3,height2);
#endif

	TEST(font!=font2);
	TEST(height!=height2);
	_LIT(KLogErrM,"Font Heights in pixels match unexpectantly, height=%d");
	if (height==height2)
		INFO_PRINTF2(KLogErrM,height);

	device->ReleaseFont(font2);
	device->ReleaseFont(font);

	// 1. find font with best-matching height in twips,
	// 2. change pixel size of the screen (leaving twip-size same),
	// 3. find font with best-matching height in twips,
	// 4. make sure that the newly found font is not the same as previous and has the different height in pixels
	device->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	sizeAndRotation.iPixelSize.iHeight=240;
	sizeAndRotation.iPixelSize.iWidth =640;
	device->SetScreenSizeAndRotation(sizeAndRotation);

	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips(font,fontspec));
	height=font->HeightInPixels();
	returnedSpec=font->FontSpecInTwips();
#if defined(EXTRA_LOGGING)
	_LIT(KLog4,"Font height: %d for device with pixels set to 240x640   twips=%d,%d");
	LOG_MESSAGE4(KLog4,height,sizeAndRotation.iTwipsSize.iWidth,sizeAndRotation.iTwipsSize.iHeight);
#endif

	sizeAndRotation.iPixelSize.iHeight=480;
	sizeAndRotation.iPixelSize.iWidth =1280;
	device->SetScreenSizeAndRotation(sizeAndRotation);

	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips(font2,fontspec));
	height2=font2->HeightInPixels();
	returnedSpec2=font2->FontSpecInTwips();
#if defined(EXTRA_LOGGING)
	_LIT(KLog5,"Font height: %d for device with pixels set to 480x1280");
	LOG_MESSAGE2(KLog5,height2);
#endif

	TEST(font!=font2);
	TEST(height!=height2);
	if (height==height2)
		INFO_PRINTF2(KLogErrM,height);

	device->ReleaseFont(font2);
	device->ReleaseFont(font);

	// 1. double the width and height of screen in both pixels and twips
	// 2. find font with best-matching height in twips,
	// 3. make sure that the newly found font is the same as previous and has the same height in pixels
	device->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	device->SetScreenSizeAndRotation(sizeAndRotation);
	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips(font,fontspec));
	sizeAndRotation.iPixelSize.iHeight*=2;
	sizeAndRotation.iPixelSize.iWidth *=2;
	sizeAndRotation.iTwipsSize.iHeight*=2;
	sizeAndRotation.iTwipsSize.iWidth *=2;
	device->SetScreenSizeAndRotation(sizeAndRotation);

	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips(font2,fontspec));
	height2=font2->HeightInPixels();
#if defined(EXTRA_LOGGING)
	_LIT(KLog6,"Font height: %d for device doubled size.   pixels=%d,%d  twips=%d,%d");
	LOG_MESSAGE6(KLog6,height2,sizeAndRotation.iPixelSize.iWidth,sizeAndRotation.iPixelSize.iHeight
							,sizeAndRotation.iTwipsSize.iWidth,sizeAndRotation.iTwipsSize.iHeight);
#endif

	TEST(font==font2);
	TEST(height0==height2);
	_LIT(KLogErrD,"Font Heights in pixels don't match, expected=%d, actual=%d");
	if (height0!=height2)
		INFO_PRINTF3(KLogErrD,height0,height2);

	device->ReleaseFont(font2);
	device->ReleaseFont(font);

	// 1. swap width and height sizes of screen for both pixels and twips
	// 2. find font with best-matching height in twips,
	// 3. make sure that the newly found font is the same as previous and has the same height in pixels
	device->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	device->SetScreenSizeAndRotation(sizeAndRotation);
	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips(font,fontspec));
	TInt swap=sizeAndRotation.iPixelSize.iHeight;
	sizeAndRotation.iPixelSize.iHeight=sizeAndRotation.iPixelSize.iWidth;
	sizeAndRotation.iPixelSize.iWidth =swap;
	swap=sizeAndRotation.iTwipsSize.iHeight;
	sizeAndRotation.iTwipsSize.iHeight=sizeAndRotation.iTwipsSize.iWidth;
	sizeAndRotation.iTwipsSize.iWidth =swap;
	device->SetScreenSizeAndRotation(sizeAndRotation);
	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips(font2,fontspec));
	height2 = font2->HeightInPixels();
#if defined(EXTRA_LOGGING)
	_LIT(KLog7,"Font height: %d for device rotated size.   pixels=%d,%d  twips=%d,%d");
	LOG_MESSAGE6(KLog7,height2,sizeAndRotation.iPixelSize.iWidth,sizeAndRotation.iPixelSize.iHeight
							,sizeAndRotation.iTwipsSize.iWidth,sizeAndRotation.iTwipsSize.iHeight);
#endif

	TEST(font==font2);
	TEST(height0==height2);
	if (height0!=height2)
		INFO_PRINTF3(KLogErrD,height0,height2);

	device->ReleaseFont(font2);
	device->ReleaseFont(font);

	delete device;
	session.Close();
	}

void CTScreenDevice::CheckBitmapL(CFbsBitmap *aBitmap, const TRect &aRect)
	{
	CFbsBitmapDevice *device=CFbsBitmapDevice::NewL(aBitmap);
	CleanupStack::PushL(device);
	TInt height=Min(aRect.Size().iHeight,aBitmap->SizeInPixels().iHeight);
	TInt width=Min(aRect.Size().iWidth,aBitmap->SizeInPixels().iWidth);
	TPoint pos(0,0);
	TInt bufLen=CFbsBitmap::ScanLineLength(width,EGray4);
	TAny *buf1=User::AllocLC(bufLen);
	TAny *buf2=User::AllocLC(bufLen);
	TPtr8 scanBuf1((TUint8 *)buf1,bufLen);
	TPtr8 scanBuf2((TUint8 *)buf2,bufLen);
	TInt oddPixels=width%4;
	TInt byte1,byte2;
	for(;pos.iY<height;pos.iY++)
		{
		TheClient->iScreen->GetScanLine(scanBuf1,pos+aRect.iTl,width,EGray4);
		device->GetScanLine(scanBuf2,pos,width,EGray4);
		if (oddPixels>0)
			{
			byte1=(scanBuf1[bufLen-1]<<(2*(4-oddPixels)))%256;
			byte2=(scanBuf2[bufLen-1]<<(2*(4-oddPixels)))%256;
			TEST(byte1==byte2);
			if (byte1!=byte2)
				INFO_PRINTF3(_L("byte1==byte2  - byte1: %d, byte2 Height: %d"), byte1, byte2);

			scanBuf1=scanBuf1.Left(bufLen-1);
			scanBuf2=scanBuf2.Left(bufLen-1);
			}
		TBool retVal = !scanBuf1.Compare(scanBuf2);
		TEST(retVal);
		if (!retVal)
			INFO_PRINTF3(_L("!scanBuf1.Compare(scanBuf2) return value  - Expected: %d, Actual: %d"), ETrue, retVal);
		}
	CleanupStack::PopAndDestroy(3);
	}

void CTScreenDevice::doTestScreenToBitmapL()
	{
	RBlankWindow shieldWin(TheClient->iWs);
	shieldWin.Construct(*TheClient->iGroup->GroupWin(),1);
	shieldWin.SetOrdinalPosition(0,-1);
	shieldWin.SetColor(TRgb::Gray4(1));
	shieldWin.Activate();
	CleanupStack::PushL(TCleanupItem(CleanUpWindow,&shieldWin));
	TSize scrSize(TheClient->iScreen->SizeInPixels());
	CWsBitmap *wsBitmap=new(ELeave) CWsBitmap(TheClient->iWs);
	CleanupStack::PushL(wsBitmap);
	User::LeaveIfError(wsBitmap->Create(scrSize,EGray4));
	CFbsBitmap *bitmap=new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);

	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();
	
	User::LeaveIfError(bitmap->Create(TSize(30,10),EGray4));
//
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(wsBitmap));
	CheckBitmapL(wsBitmap,TRect(scrSize));
//
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(bitmap));
	CheckBitmapL(bitmap,TRect(scrSize));
//
	TRect rect1(111,10,222,20);
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(wsBitmap,rect1));
	CheckBitmapL(wsBitmap,rect1);
//
	TRect rect2(10,20,100,200);
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(bitmap,rect2));
	CheckBitmapL(bitmap,rect2);
//
// Now some mad values
//
// Right edge left of left edge
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(bitmap,TRect(100,20,90,200)));
// Off the edge of the screen
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(bitmap,TRect(-10,20,-5,200)));
// Off the edge of the screen again
	User::LeaveIfError(TheClient->iScreen->CopyScreenToBitmap(bitmap,TRect(scrSize.AsPoint()+TPoint(5,5),TSize(10,10))));
//
	CleanupStack::PopAndDestroy(3);
	}



void CTScreenDevice::TestScreenToBitmap()
	{
	TRAPD(ret,doTestScreenToBitmapL());
	TEST(ret==KErrNone);
	if (ret!=KErrNone)
		INFO_PRINTF3(_L("doTestScreenToBitmapL() return value  - Expected: %d, Actual: %d"), KErrNone, ret);
	}
	

CCachePanic::~CCachePanic()
	{
	delete iWsScrDev;
	iWs.Close();
	}

void CCachePanic::ConstructL(TInt aScreenNumber, TInt aInt)
	{
	iScreenNumber = aScreenNumber;
	iInt = aInt;

	User::LeaveIfError(iWs.Connect());
	iWsScrDev=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iWsScrDev->Construct(iScreenNumber));
	}

TInt CCachePanic::DoTest(CCachePanic* aTestClass)
	{
	CActiveScheduler* activeScheduler=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);
	CleanupStack::PushL(activeScheduler);
	TRAP_IGNORE(aTestClass->TestL());
	CleanupStack::PopAndDestroy(activeScheduler);
	return EWsExitReasonFinished;
	}

void CCachePanic::TestL()
	{
	AutoPanic(EAutoPanicTestFailed);
	}

TInt CTwipsPanic::RunTest(TInt aInt, TAny* aPtr)
	{
	TInt error = KErrNone;

	CTwipsPanic* self=new(ELeave) CTwipsPanic();
	CleanupStack::PushL(self);

	TRAP(error, self->ConstructL((TInt)aPtr, aInt));
	if (!error)
		{
		error = CCachePanic::DoTest(self);
		}
	CleanupStack::PopAndDestroy(self);
	return error;
	}

void CTwipsPanic::TestL()
	{
	TPixelsTwipsAndRotation sizeAndRotation;
	TFontSpec fontspec(KTestFontTypefaceName,250);

	CFbsFont *font;
	CFbsFont *font2;

	iWsScrDev->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	sizeAndRotation.iTwipsSize.iHeight = 2000;
	sizeAndRotation.iTwipsSize.iWidth  = 3000;
	iWsScrDev->SetScreenSizeAndRotation(sizeAndRotation);
	iWsScrDev->GetNearestFontToDesignHeightInTwips((CFont *&)font,fontspec);
	iWsScrDev->GetNearestFontToDesignHeightInTwips((CFont *&)font2,fontspec);
	sizeAndRotation.iTwipsSize.iHeight = 4000;
	sizeAndRotation.iTwipsSize.iWidth  = 6000;
	iWsScrDev->SetScreenSizeAndRotation(sizeAndRotation);
	iWsScrDev->ReleaseFont(font);
	}

TInt CScanLinePanic::RunTest(TInt aInt, TAny* aPtr)
	{
	TInt error = KErrNone;

	CScanLinePanic* self=new(ELeave) CScanLinePanic();
	CleanupStack::PushL(self);

	TRAP(error, self->ConstructL((TInt)aPtr, aInt));
	if (!error)
		{
		error = CCachePanic::DoTest(self);
		}
	CleanupStack::PopAndDestroy(self);
	return error;    
	}

void CScanLinePanic::TestL()
	{
	switch (iInt)
		{
		case TESTCASE_GRAPHICS_WSERV_0473:
			TestLengthInvariant();
			break;
		case TESTCASE_GRAPHICS_WSERV_0474:
			TestBufferInvariant();
			break;
		}
	}

void CScanLinePanic::TestLengthInvariant()
	{
	TInt buffLength=50;
	TInt length=-1;
	TUint8* buffer=new(ELeave) TUint8[buffLength];
	TPtr8 scanline(buffer, buffLength, buffLength);
	iWsScrDev->GetScanLine(scanline, TPoint(), length, EColor64K);
	delete buffer;    
	}

void CScanLinePanic::TestBufferInvariant()
	{
	TInt buffLength=19;
	TInt length=10;
	TUint8* buffer=new(ELeave) TUint8[buffLength];
	TPtr8 scanline(buffer, buffLength, buffLength);
	iWsScrDev->GetScanLine(scanline, TPoint(), length, EColor64K);
	delete buffer;    
	}

void CTScreenDevice::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	((CTScreenDeviceStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0080

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test twips to pixels conversions

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Converts pixels to twips and then back the result back to pixels. Checks screen size request methods both in pixels and twips

@SYMTestExpectedResults Expects that back and forth conversions preserve an original value.
*/
		case 1:
			((CTScreenDeviceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0080"));
			iTest->LogSubTest(_L("Screen Device 1"));
			TestTwipPixelConversions();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0081

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Tests CWsScreenDevice::CopyScreenToBitmap method

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates a blank window on the screen and uses CWsScreenDevice::CopyScreenToBitmap to copy parts of the screen image to either CWsBitmap or CFbsBitmap

@SYMTestExpectedResults Expects that the method will not fail and that the image is copied correctly making pixel-to-pixel comparisons
*/
		case 2:
			((CTScreenDeviceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0081"));
			iTest->LogSubTest(_L("Copy Screen To Bitmap"));
			TestScreenToBitmap();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0082

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test for integrity of twips to pixels conversions providing big values

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Calls twips to pixels (and backward) conversions for 100000000 value

@SYMTestExpectedResults Expects that back and forth conversions preserve an original value. Make sure that the result is always greater that 0.
*/
		case 3:
			((CTScreenDeviceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0082"));
			iTest->LogSubTest(_L("Twip To Pixels Overflow"));
			TestTwipPixelOverflow();
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0083

@SYMDEF  			DEF081259

@SYMTestCaseDesc    Test that twips-in-pixel ratio change for screen size affects twips-in-pixel ratio of font

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Changes number of twips on screen (leaving number of pixels same), changes number of pixels on screen (leaving number of twips same), makes proportional changes of screen size for both twips and pixels

@SYMTestExpectedResults Expects that twips-in-pixel ratio of font leaves the same only when screen size was changed through multipying of width by a coefficient (same for both twips and pixels) and multipying of height by a coefficient (same for both twips and pixels)
*/
		case 4:
			((CTScreenDeviceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0083"));
			iTest->LogSubTest(_L("Twips cache flush after device size change"));
			TwipsCacheFlushL();
			break;
		case 5:
			((CTScreenDeviceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0083"));
			iTest->LogSubTest(_L("Twips cache reset"));
			// not expecting any panic
			TEST(iTest->TestPanicL(CTwipsPanic::RunTest, 0, 0, (TAny*)iTest->iScreenNumber, _L("")));
			break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0473

@SYMDEF             DEF116127

@SYMTestCaseDesc    Invariant testing length parameter of less then 0 values in CWsScreenDevice::GetScanLine()
					method

@SYMTestActions     Calls CWsScreenDevice::GetScanLine with aLength of -1.

**/            
		case 6:
			((CTScreenDeviceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0473"));
			iTest->LogSubTest(_L("Testing ScanLine length invariant"));
			TEST(iTest->TestPanicL(CScanLinePanic::RunTest, EWservPanicInvalidParameter, 
				 CScanLinePanic::TESTCASE_GRAPHICS_WSERV_0473,
				 (TAny*)iTest->iScreenNumber, _L("WSERV")));
			break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0474

@SYMDEF             DEF116127

@SYMTestCaseDesc    Invariant testing buffer parameter for required size.

@SYMTestActions     Calls CWsScreenDevice::GetScanLine with buffer too small to contain requested data.
**/            
		case 7:
			((CTScreenDeviceStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0474"));
			iTest->LogSubTest(_L("Testing ScanLine buffer invariant"));
			TEST(iTest->TestPanicL(CScanLinePanic::RunTest, EWservPanicInvalidParameter,
				 CScanLinePanic::TESTCASE_GRAPHICS_WSERV_0474,
				 (TAny*)iTest->iScreenNumber, _L("WSERV")));
			break;
		default:
			((CTScreenDeviceStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTScreenDeviceStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
		}
	((CTScreenDeviceStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(ScreenDevice)
