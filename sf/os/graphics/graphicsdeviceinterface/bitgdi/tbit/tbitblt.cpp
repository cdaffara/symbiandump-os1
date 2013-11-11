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

#include <hal.h>
#include "tbitblt.h"

//_LIT(KBit16BitmapOnZ, "z:\\system\\data\\16bit.mbm");
_LIT(KBit32BitmapOnZ, "z:\\system\\data\\32bit_2.mbm");

/**
Create a new virtual bitmap device
*/
CVirtualBitmapDevice*  CVirtualBitmapDevice::NewL(TDisplayMode aDisplayMode, TSize aSize)
	{
	CVirtualBitmapDevice* self = new(ELeave) CVirtualBitmapDevice();
	CleanupStack::PushL(self);
	self->ConstructL(aDisplayMode, aSize);
	CleanupStack::Pop(self);
	return self;
	}

void CVirtualBitmapDevice::ConstructL(TDisplayMode aDisplayMode, TSize aSize)
	{	
	// Attempt to create a screen device
	CFbsScreenDevice* screenDevice = NULL;
	TRAPD(ret, screenDevice = CFbsScreenDevice::NewL(_L("scdv"), aDisplayMode));
	if (ret != KErrNone)
		{
		// Screen device cannot be created so create a off screen bitmap device
		iBitmap = new(ELeave) CFbsBitmap;
		iBitmap->Create(aSize, aDisplayMode);
		iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
		iIsScreenDevice = EFalse;
		}
	else
		{
		screenDevice->SetAutoUpdate(EFalse);
		iBitmapDevice = screenDevice;
		iIsScreenDevice = ETrue;
		}
	}

CVirtualBitmapDevice::~CVirtualBitmapDevice()
	{
	delete iBitmapDevice;
	delete iBitmap;
	}

/**
Update implements the same method on CFbsScreenDevice to Update the screen. This only works on a screen device.
Off screen bitmaps do not have to be updated and no action will be taken in this case.
*/
void CVirtualBitmapDevice::Update()
	{
	if (iIsScreenDevice)
		{
		CFbsScreenDevice* screenDevice = static_cast<CFbsScreenDevice*>(iBitmapDevice);
		screenDevice->Update();
		}
	}

/**
Returns the actual bitmap device
*/
CBitmapDevice& CVirtualBitmapDevice::BitmapDevice()
	{
	return *iBitmapDevice;
	}

//  CTBitBlt Methods
CTBitBlt::CTBitBlt(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	}

CTBitBlt::~CTBitBlt()
	{
	delete iBitmap;
	}

void CTBitBlt::ConstructL()
	{
	INFO_PRINTF1(_L("Pre-test setup"));
	iBitmap=new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Load(KBit32BitmapOnZ,0,EFalse));  //EMbmTbmpTcolor
	}

void CTBitBlt::RunTestCaseL(TInt aCurTestCase)
	{
	((CTBitBltStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTBitBltStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0017"));
		TestBitBltPositionsL(EFalse);	// Uncompressed bitmaps
		TestBitBltPositionsL(ETrue);	// Compressed bitmaps
		break;
	case 2:
		((CTBitBltStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTBitBltStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTBitBltStep*)iStep)->RecordTestResultL();
	}

/**
Captures the screen data from aDevice and returns it as a TUint8 buffer in EColor256.

@param aDevice the bitmap device to capture
@param aScreenByteSize the size of the returned data
@return the screen data in EColor256
*/
TUint8* CTBitBlt::CaptureDeviceDataLC(CBitmapDevice& aDevice, TInt& aScreenByteSize)
	{
	TSize scrDevSize = aDevice.SizeInPixels();
	TDisplayMode displayMode = aDevice.DisplayMode();

	// Create memory to hold device data assuming EColor256
	aScreenByteSize = scrDevSize.iWidth * scrDevSize.iHeight;
	TUint8* screenData = new (ELeave) TUint8[aScreenByteSize];
	CleanupArrayDeletePushL(screenData);

	// Fill the blocks with some default value
	Mem::Fill(screenData, aScreenByteSize, 0xCA);

	// Get screen data and write the data to screenBmp.
	for(TInt y=0; y<scrDevSize.iHeight;y++)
   		{
		TPtr8 p(screenData + y * scrDevSize.iWidth, scrDevSize.iWidth, scrDevSize.iWidth);
		aDevice.GetScanLine(p, TPoint(0, y), scrDevSize.iWidth, EColor256); // Get Scanline and convert to EColor256 if not already in that display mode
   		}
   	return screenData;
	}

/**
Creates a bitmap image of what is meant to be displayed on the screen without using any bitblt methods.
This bitmap is compared with the one on screen and returns 0 if they match.

@param aDevice the screen device to capture
@param aGc graphics context for aDevice
@param aScreenSize the size of the display of aDevice
@param aBitmapSize the size of the bitmap
@param aScreenMode the screen mode to use
@param aOffset the offset of the bitmap from the top left corner of the display
@param aRect the clipping rectangle for the bitmap
@return if bitmap matches what is on the screen 0 is returned
*/
TInt CTBitBlt::CreateBitmapImageAndCompareL(CVirtualBitmapDevice& aDevice, CFbsBitGc& aGc, TSize aScreenSize, TSize aBitmapSize, TDisplayMode aScreenMode, TPoint aOffset, TRect& aRect)
	{
	// Create Bitmap
	CFbsBitmap* tBitmap = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(tBitmap);
	tBitmap->Create(aScreenSize, aScreenMode);

	// Create Bitmap Device
	CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL(tBitmap);
	CleanupStack::PushL(bmpDevice);

	// Create GC for bitmap device and draw a some graphics
	CFbsBitGc* bmpGc;
	User::LeaveIfError(bmpDevice->CreateContext(bmpGc));
	CleanupStack::PushL(bmpGc);
	bmpGc->Clear();
	bmpGc->SetClippingRect(TRect(aRect.iTl.iX+aOffset.iX, aRect.iTl.iY+aOffset.iY, aRect.iBr.iX+aOffset.iY, aRect.iBr.iY+aOffset.iY));
	for (TInt i = aBitmapSize.iWidth/2; i>0; --i)
		{
		bmpGc->SetPenColor(TRgb::Color256(i));
		bmpGc->DrawRect(TRect(i+aOffset.iX,i+aOffset.iY,aBitmapSize.iWidth - i + aOffset.iX, aBitmapSize.iHeight - i + aOffset.iY));
		}
	bmpGc->CancelClippingRect();

	// Now compare tBitmap with what is on the screen
	TInt allocatedSize;
	TUint8* screenData = CaptureDeviceDataLC(aDevice.BitmapDevice(), allocatedSize);
	TUint8* testData = CaptureDeviceDataLC(*bmpDevice, allocatedSize);
	TInt res = Mem::Compare(screenData, allocatedSize, testData, allocatedSize);
	// Display on screen
	aGc.Clear();
	aGc.BitBlt(TPoint(0,0), tBitmap, TRect(TPoint(0,0), aScreenSize));
	aDevice.Update();
	CleanupStack::PopAndDestroy(5, tBitmap);
	return res;
	}

/**
Bitblts a bitmap and compares it with how it should look

@param aDevice the screen device to capture
@param aGc graphics context for aDevice
@param aBitmap the bitmap to bitblt
@param aScreenMode the screen mode
@param aPointStart the top left point to start displaying the bitmap
@param aRect the clipping rectangle for the bitmap
*/
void CTBitBlt::SimpleBitBltAndTestL(CVirtualBitmapDevice& aDevice, CFbsBitGc& aGc, CFbsBitmap& aBitmap, TDisplayMode aScreenMode, TPoint aStartPoint, TRect& aRect)
	{
	aGc.Clear();
	aGc.BitBlt(aStartPoint, &aBitmap, aRect);
	aDevice.Update();
	TPoint offset(10,10);
	TInt res = CreateBitmapImageAndCompareL(aDevice, aGc, aDevice.BitmapDevice().SizeInPixels(), aBitmap.SizeInPixels(), aScreenMode, offset, aRect);
	TEST(res == 0);
	}

/**
	@SYMTestCaseID GRAPHICS-BITGDI-0017
 
	@SYMDEF DEF105390
   
	@SYMTestCaseDesc  Tests simple BitBlt using clipping rect is various places.
 
	@SYMTestPriority Low
  
	@SYMTestStatus Implemented
   
	@SYMTestActions Test the BitBlt function for positioning by blitting only some part of the image.
   
	@SYMTestExpectedResults Test should perform graphics operations succesfully.

 */

void CTBitBlt::TestBitBltPositionsL(TBool aDoCompressed)
	{
	if(aDoCompressed)
		{
		INFO_PRINTF1(_L("BitBlt Positions testing : Compressed Bitmaps"));
		}
	else
		{
		INFO_PRINTF1(_L("BitBlt Positions testing : Uncompressed Bitmaps"));
		}

	TDisplayMode dstDispMode[] = {EColor16M, EColor16MU, EColor256, EColor4K, EColor64K};	
	TDisplayMode dispMode[] = {EColor16MA, EColor16MAP, EColor16M, EColor16MU, EColor256, EColor4K, EColor64K, EGray256, EGray16, EGray4, EGray2, EColor16};	

	for(TInt dstDispModeIndex = 0; dstDispModeIndex < TInt(sizeof(dstDispMode)/sizeof(dstDispMode[0])); dstDispModeIndex++)
		{
		// Test for each display mode
		for(TInt dispModeIndex = 0; dispModeIndex < TInt(sizeof(dispMode)/sizeof(dispMode[0])); dispModeIndex++)
			{
			INFO_PRINTF3(_L("Display Mode Index = %d/%d"), dstDispModeIndex, dispModeIndex);
		
			// Setup screen device to 16MA
			TSize size = TSize(640,200);
			iVirtualBmpDevice = CVirtualBitmapDevice::NewL(dstDispMode[dstDispModeIndex], size);
			CBitmapDevice& screenDevice = iVirtualBmpDevice->BitmapDevice();		

			CFbsBitGc* gc=NULL;
			User::LeaveIfError(screenDevice.CreateContext((CGraphicsContext*&)gc));
			TEST(gc!=NULL);
			CleanupStack::PushL(gc);
			for(TInt orientation=0;orientation<=CFbsBitGc::EGraphicsOrientationRotated270;orientation++)
				{
				gc->Reset();
				if (!gc->SetOrientation((CFbsBitGc::TGraphicsOrientation)orientation))
					continue;
				if (orientation&1 && dispMode[dispModeIndex]==EColor16M)
					{//zzz Skipping this case due to DEF120222 that causes the verification code to fail
					continue;
					}
				TSize screenSize = screenDevice.SizeInPixels();

				// Create Bitmap
				CFbsBitmap* bitmap32 = new(ELeave) CFbsBitmap;
				TEST(bitmap32!=NULL);
				CleanupStack::PushL(bitmap32);
				TSize bitmapSize = screenSize - TSize(20, 20); 	// Make bitmap smaller than screen by 20 pixels
				bitmap32->Create(bitmapSize, dispMode[dispModeIndex]);

				// Create Bitmap Device
				CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL(bitmap32);
				TEST(bmpDevice!=NULL);
				CleanupStack::PushL(bmpDevice);

				// Create GC for bitmap device and draw some graphics
				CFbsBitGc* bmpGc;
				User::LeaveIfError(bmpDevice->CreateContext(bmpGc));
				TEST(bmpGc!=NULL);
				CleanupStack::PushL(bmpGc);
				bmpGc->Clear();
				for (TInt i = bitmapSize.iWidth/2; i>0; --i)
					{
					bmpGc->SetPenColor(TRgb::Color256(i));	
					bmpGc->DrawRect(TRect(i,i,bitmapSize.iWidth - i, bitmapSize.iHeight - i));
					}

				if (aDoCompressed)
					{
					bitmap32->Compress();
					}

				gc->Clear();

				// Display whole bitmap with larger rect
				TPoint startPoint(10,10);
				TRect rect(0,0,bitmapSize.iWidth+50, bitmapSize.iHeight+50);
				SimpleBitBltAndTestL(*iVirtualBmpDevice, *gc, *bitmap32, dispMode[dispModeIndex], startPoint, rect);

				// Display whole bitmap
				startPoint = TPoint(10,10);
				rect = TRect(0,0,bitmapSize.iWidth, bitmapSize.iHeight);
				SimpleBitBltAndTestL(*iVirtualBmpDevice, *gc, *bitmap32, dispMode[dispModeIndex], startPoint, rect);

				// Display TL of bitmap
				startPoint = TPoint(10,10);
				rect = TRect(0,0,bitmapSize.iWidth/2, bitmapSize.iHeight/2);
				SimpleBitBltAndTestL(*iVirtualBmpDevice, *gc, *bitmap32, dispMode[dispModeIndex], startPoint, rect);

				// Display TR of bitmap
				startPoint = TPoint(bitmapSize.iWidth/2+10, 10);
				rect = TRect(bitmapSize.iWidth/2,0,bitmapSize.iWidth,bitmapSize.iHeight/2);
				SimpleBitBltAndTestL(*iVirtualBmpDevice, *gc, *bitmap32, dispMode[dispModeIndex], startPoint, rect);

				// Display BL of bitmap
				startPoint = TPoint(10, bitmapSize.iHeight/2+10);
				rect = TRect(0,bitmapSize.iHeight/2,bitmapSize.iWidth/2,bitmapSize.iHeight);
				SimpleBitBltAndTestL(*iVirtualBmpDevice, *gc, *bitmap32, dispMode[dispModeIndex], startPoint, rect);

				// Display BR of bitmap
				startPoint = TPoint(bitmapSize.iWidth/2+10,bitmapSize.iHeight/2+10);
				rect = TRect(bitmapSize.iWidth/2,bitmapSize.iHeight/2,bitmapSize.iWidth,bitmapSize.iHeight);
				SimpleBitBltAndTestL(*iVirtualBmpDevice, *gc, *bitmap32, dispMode[dispModeIndex], startPoint, rect);

				// Centre of bitmap
				startPoint = TPoint(bitmapSize.iWidth/4+10,bitmapSize.iHeight/4+10);
				rect = TRect(bitmapSize.iWidth/4,bitmapSize.iHeight/4,bitmapSize.iWidth/4+bitmapSize.iWidth/4,bitmapSize.iHeight/4+bitmapSize.iHeight/4);
				SimpleBitBltAndTestL(*iVirtualBmpDevice, *gc, *bitmap32, dispMode[dispModeIndex], startPoint, rect);

				CleanupStack::PopAndDestroy(3, bitmap32);
				}
			CleanupStack::PopAndDestroy(gc);
			delete iVirtualBmpDevice;
			iVirtualBmpDevice = NULL;
			}
		}
	}

//--------------
__CONSTRUCT_STEP__(BitBlt)

void CTBitBltStep::TestSetupL()
	{
	}

void CTBitBltStep::TestClose()
	{
	}
