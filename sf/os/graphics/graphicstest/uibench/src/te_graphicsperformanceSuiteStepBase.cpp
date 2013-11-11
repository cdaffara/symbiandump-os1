// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "te_graphicsperformanceSuiteStepBase.h"
#include "te_graphicsperformanceSuiteDefs.h"
#include <hal.h>
#include <fbs.h>
#include <bitdrawinterfaceid.h>
#include <bmalphablend.h>
#include <bitdraw.h>
#include <graphics/gdi/gdiconsts.h>
#include <graphics/fbsdefs.h>

#ifdef _USE_PROFILER
#include <profiler.h>
#endif

#ifdef __WINS__
_LIT(KBitmapDrive, "c:");
#else
_LIT(KBitmapDrive, "e:");
#endif
_LIT(KBitmapPath, "\\uibench_sanity\\%S.mbm");

/**
Gets the size of the hardware display in pixels
@return TSize object containing the screen size
*/
TSize GetDisplaySizeInPixels()
	{
	TInt x;
	HAL::Get(HALData::EDisplayXPixels, x);	// Get number x pixel of screen
	TInt y;
	HAL::Get(HALData::EDisplayYPixels, y);	// Get number y pixel of screen
	return TSize(x,y);
	}

/**
Returns the size of the target used for off-screen drawing
@return TSize object containing the pixmap size.
*/
TSize GetPixmapSizeInPixels()
	{
	return TSize(1000, 1000);
	}

/**
Create a new virtual bitmap device
*/
CVirtualBitmapDevice*  CVirtualBitmapDevice::NewL(TDisplayMode aDisplayMode, TBool aForceOffscreen)
	{
	CVirtualBitmapDevice* self = new(ELeave) CVirtualBitmapDevice();
	CleanupStack::PushL(self);
	self->ConstructL(aDisplayMode, aForceOffscreen);	
	CleanupStack::Pop(self);
	return self;
	}
	
/*
@param aDisplayMode The displaymode of the bitmap to create.
@param aForceOffscreen If ETrue, a bitmap device is created instead of a screen device, regardless
          of whether the screen supports the display mode.
*/
void CVirtualBitmapDevice::ConstructL(TDisplayMode aDisplayMode, TBool aForceOffscreen)
	{	
	// Attempt to create a screen device if not asked to use offscreen bitmap.
	CFbsScreenDevice* screenDevice = NULL;
	TInt ret = KErrNone;
	
	if (!aForceOffscreen)
		{
		TRAP(ret, screenDevice = CFbsScreenDevice::NewL(_L("scdv"), aDisplayMode));
		}
	
	if (aForceOffscreen || ret != KErrNone)
		{		
		// Screen device cannot be created, or we didn't want one, so create an off screen bitmap device		
		iBitmap = new(ELeave) CFbsBitmap;
		TSize scsize= (aForceOffscreen) ? GetPixmapSizeInPixels() : GetDisplaySizeInPixels();
		iBitmap->Create(scsize, aDisplayMode);
		iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);		
		iIsScreenDevice = EFalse;
		}	
	else
		{
		screenDevice->SetAutoUpdate(ETrue);
		iBitmapDevice = screenDevice;
		iIsScreenDevice = ETrue;
		}	
	}

int CVirtualBitmapDevice::CreateContext(CGraphicsContext *&aGc)
	{	
	return iBitmapDevice->CreateContext(aGc);
	}

TSize CVirtualBitmapDevice::SizeInPixels() const 
	{
	return iBitmapDevice->SizeInPixels();
	}


TInt CVirtualBitmapDevice::isScreenDevice() 
	{
	return iIsScreenDevice;
	}

CVirtualBitmapDevice::~CVirtualBitmapDevice()
	{
	delete iBitmapDevice;
	delete iBitmap;	
	}


/**
Implements the same method on CFbsScreenDevice to Update the screen. This only works on a screen device.
Off screen bitmaps do not have to be updated and no action will be taken in this case.
*/
void CVirtualBitmapDevice::Update()
	{
	if (iIsScreenDevice)
		{
		CFbsScreenDevice* screenDevice = static_cast<CFbsScreenDevice*>(iBitmapDevice);
		if (screenDevice)
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

/**
Returns the actual bitmap
*/
CFbsBitmap& CVirtualBitmapDevice::Bitmap()
	{
	return *iBitmap;
	}
		
/**
Create a new virtual draw device
*/
CVirtualDrawDevice*  CVirtualDrawDevice::NewL(TDisplayMode aDisplayMode)
	{
	CVirtualDrawDevice* self = new(ELeave) CVirtualDrawDevice();
	CleanupStack::PushL(self);
	self->ConstructL(aDisplayMode);
	CleanupStack::Pop(self);
	return self;
	}
	
void CVirtualDrawDevice::ConstructL(TDisplayMode aDisplayMode)
	{
	// Attempt to create a screen device
	CFbsDrawDevice* drawDevice = NULL;
	TRAPD(ret, drawDevice = CFbsDrawDevice::NewScreenDeviceL(KDefaultScreenNo, aDisplayMode));
	if (ret != KErrNone)
		{
		// Screen device cannot be created so create an off screen bitmap draw device
		iSize = GetDisplaySizeInPixels();
		iDrawDevice  = CFbsDrawDevice::NewBitmapDeviceL(iSize, aDisplayMode, ByteSize(iSize, aDisplayMode) / iSize.iHeight);
		iDeviceMemory = new (ELeave) TUint8[ByteSize(iSize, aDisplayMode)];
		iDrawDevice->SetBits(iDeviceMemory);
		iIsDrawDevice = EFalse;
		}
	else
		{
		iDrawDevice = drawDevice;
		iSize = iDrawDevice->SizeInPixels();
		iDrawDevice->SetAutoUpdate(ETrue);
		iIsDrawDevice = ETrue;
		}
	}

TInt CVirtualDrawDevice::ByteSize(const TSize& aSize, TDisplayMode aDisplayMode)
	{
	TInt wordSize = aSize.iWidth;
	switch(aDisplayMode)
		{
	case EGray2:
		wordSize = (wordSize + 31) / 32;
		break;
	case EGray4:
		wordSize = (wordSize + 15) / 16;
		break;
	case EGray16:
	case EColor16:
		wordSize = (wordSize + 7) / 8;
		break;
	case EGray256:
	case EColor256:
		wordSize = (wordSize + 3) / 4;
		break;
	case EColor4K:
	case EColor64K:
		wordSize = (wordSize + 1) / 2;
		break;
	case EColor16M:
		wordSize = (((wordSize * 3) + 11) / 12) * 3;
		break;
	case EColor16MU:
	case EColor16MA:
		//Should not be changed!
		break;
	default:
		break;
		};
	return wordSize * aSize.iHeight * 4;
	}

CFbsDrawDevice& CVirtualDrawDevice::DrawDevice()
	{
	return *iDrawDevice;
	}

CVirtualDrawDevice::~CVirtualDrawDevice()
	{
	delete iDrawDevice;
	delete[] iDeviceMemory;
	}

TBool CVirtualDrawDevice::IsDrawDevice()
	{
	return iIsDrawDevice;
	}

const TDesC& CTe_graphicsperformanceSuiteStepBase::ColorModeName(TDisplayMode aMode)
	{
	_LIT(KColor4K,"Color4K");
	_LIT(KColor64K,"Color64K");
	_LIT(KColor16M,"Color16M");
	_LIT(KColor16MU,"Color16MU");
	_LIT(KColor16MA,"Color16MA");
	_LIT(KColor16MAP,"Color16MAP");
	_LIT(KUnknown,"Unknown");
	switch(aMode)
		{
	case EColor4K:
		return KColor4K;
	case EColor64K:
		return KColor64K;
	case EColor16M:
		return KColor16M;
	case EColor16MU:
		return KColor16MU;
	case EColor16MA:
		return KColor16MA;
	case EColor16MAP:
		return KColor16MAP;
	default:
		return KUnknown;
		}
	}

/**
Implementation of CTestStep base class virtual
It is used for doing all initialisation common to derived classes in here.
Make it being able to leave if there are any errors here as there's no point in
trying to run a test step if anything fails.
The leave will be picked up by the framework.

@return - TVerdict
*/
TVerdict CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	
	// Create and install Active Scheduler in case tests require active objects
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	FbsStartup();
	TESTNOERRORL(RFbsSession::Connect());
	HAL::Get(HALData::ECPUSpeed,iCPUSpeed); 
	INFO_PRINTF2(_L("CPUSpeed: %i	kHz"),iCPUSpeed);
	
	// get input for tests from .ini file
	TEST(GetIntFromConfig(_L("Profiling"), _L("DoProfiling"), iDoProfiling));
	TEST(GetBoolFromConfig(_L("SanityCheck"), _L("Bitmaps"), iShowBitmaps));
	
	if (iDoProfiling>0)	
		{
		__INITPROFILER	
		}
			
	iProfiler = CTProfiler::NewL(*this);
	__UHEAP_MARK;	
	return TestStepResult();
	}

/**
Implementation of CTestStep base class virtual
It is used for doing all after test treatment common to derived classes in here.
Make it being able to leave
The leave will be picked up by the framework.

@return - TVerdict
*/
 TVerdict CTe_graphicsperformanceSuiteStepBase::doTestStepPostambleL()
	{
	delete iScheduler;
	iScheduler = NULL;
    delete iProfiler;
    iProfiler = NULL;
	return TestStepResult();
	}

CTe_graphicsperformanceSuiteStepBase::~CTe_graphicsperformanceSuiteStepBase()
	{	
	if (iDoProfiling>0)	
		{
		__CLEANUPPROFILER	
		}	
	delete iScreenDevice;	
	delete iGc;
	delete iDrawDevice;
	RFbsSession::Disconnect();
	__UHEAP_MARKEND;
	}

CTe_graphicsperformanceSuiteStepBase::CTe_graphicsperformanceSuiteStepBase()
	{
	}

CVirtualBitmapDevice* CTe_graphicsperformanceSuiteStepBase::ScreenDevice()
	{
	TEST(iScreenDevice!=NULL);	// Ensure it is valid
	return iScreenDevice;
	}

/**
Destroys and recreates the current target device.

@param aScreenMode The display mode of the target being created
@param aForceOffscreen If ETrue, an offscreen buffer is used as the target instead of the screen. The
         dimensions of the buffer are retrieved from GetPixmapSizeInPixels.
*/
void CTe_graphicsperformanceSuiteStepBase::SetScreenModeL(TDisplayMode aScreenMode, TBool aForceOffscreen)
	{
	delete iScreenDevice;
	iScreenDevice = NULL;

	iScreenDevice = CVirtualBitmapDevice::NewL(aScreenMode, aForceOffscreen);
	if (iScreenDevice->isScreenDevice())
		{
		INFO_PRINTF2(_L("SetScreenMode - supported: %S"),&ColorModeName(aScreenMode));
		}
	else
		{
		if (!aForceOffscreen)
			{
			INFO_PRINTF2(_L("SetScreenMode - not supported(using off screen bitmap instead): %S"),&ColorModeName(aScreenMode));
			}
		else
			{
			INFO_PRINTF2(_L("SetScreenMode - using off screen bitmap as requested: %S"),&ColorModeName(aScreenMode));
			}
		}
	delete iGc;
	iGc = NULL;
	User::LeaveIfError(iScreenDevice->CreateContext((CGraphicsContext*&)iGc));
	iScreenSize = iScreenDevice->SizeInPixels();
	}

void CTe_graphicsperformanceSuiteStepBase::SetDrawDeviceModeL(TDisplayMode aScreenMode)
	{
	delete iDrawDevice;
	iDrawDevice = NULL;

	iDrawDevice = CVirtualDrawDevice::NewL(aScreenMode);
	if (iDrawDevice->IsDrawDevice())
		{
		INFO_PRINTF2(_L("SetDrawDeviceMode - supported: %S"),&ColorModeName(aScreenMode));
		}
	else
		{
		INFO_PRINTF2(_L("SetDrawDeviceMode - not supported(using off screen bitmap instead): %S"),&ColorModeName(aScreenMode));
		}
	}

TInt CTe_graphicsperformanceSuiteStepBase::GetDrawDeviceInterfaceL(TInt aInterfaceId, TAny*& aInterface)
	{
	User::LeaveIfNull(iDrawDevice);
	return iDrawDevice->DrawDevice().GetInterface(aInterfaceId, aInterface);	
	}

void CTe_graphicsperformanceSuiteStepBase::ClearDrawDeviceL(TRgb aColor)
	{
	User::LeaveIfNull(iDrawDevice);
	CFbsDrawDevice& drawDevice = iDrawDevice->DrawDevice();
	TSize size = drawDevice.SizeInPixels();
	drawDevice.SetShadowMode(CFbsDrawDevice::ENoShadow);
	drawDevice.WriteRgbMulti(0,0,size.iWidth,size.iHeight,aColor,CGraphicsContext::EDrawModePEN);
	}

/**
Creates a bitmap for given size and display mode and leaves it on the cleanup stack

@return pointer to a created CFbsBitmap 
*/
CFbsBitmap* CTe_graphicsperformanceSuiteStepBase::CreateSoftwareBitmapLC(const TSize& aSize, TDisplayMode aMode)
	{
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(aSize, aMode));
	return bitmap;
	}
	
/**
Copy a bitmap into another bitmap (generally in a different displaymode)
tiles destination bitmap with source
*/
void CTe_graphicsperformanceSuiteStepBase::CopyBitmapL(CFbsBitmap* aDst, CFbsBitmap* aSrc)
	{
	TSize srcSize = aSrc->SizeInPixels();
	TSize dstSize = aDst->SizeInPixels();
	CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(aDst);
	CleanupStack::PushL(dev);
	CFbsBitGc* gc = NULL;
	if ( 0 == dev->CreateContext(gc) )
		{
		CleanupStack::PushL(gc);
		TPoint point;
		gc->SetBrushColor(TRANSPARENT_BLACK);
		gc->SetBrushStyle(CGraphicsContext::ENullBrush);
		gc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
		gc->Clear();
		gc->SetDrawMode(CGraphicsContext::EDrawModePEN);
		for(point.iY=0; point.iY<dstSize.iHeight; point.iY+=srcSize.iHeight)
			{
			for(point.iX=0; point.iX<dstSize.iWidth; point.iX+=srcSize.iWidth)
				{
				gc->BitBlt(point, aSrc);
				}
			}
		CleanupStack::PopAndDestroy(gc);
		}
	CleanupStack::PopAndDestroy(dev);
	}

/**
Copy a source bitmap into a new bitmap with the specified display mode
*/
CFbsBitmap* CTe_graphicsperformanceSuiteStepBase::CopyIntoNewBitmapL(CFbsBitmap* aSrc, TDisplayMode aDisplayMode)
	{
	CFbsBitmap* dstBmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(dstBmp);
	TInt ret=dstBmp->Create(aSrc->SizeInPixels(), aDisplayMode);
	User::LeaveIfError(ret);
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(dstBmp);
	CleanupStack::PushL(bitmapDevice);
	CFbsBitGc* gc;
	ret = bitmapDevice->CreateContext(gc);
	User::LeaveIfError(ret);
	CleanupStack::PushL(gc);
	gc->BitBlt(TPoint(0,0), aSrc);
	CleanupStack::PopAndDestroy(2, bitmapDevice); // gc, bitmapDevice
	CleanupStack::Pop(dstBmp);
	return dstBmp;
	}

/**
Loads a bitmap from a file

@param aName the filename of the bitmap to load
@param aIndex the index of the bitmap to load in the file
*/
CFbsBitmap* CTe_graphicsperformanceSuiteStepBase::LoadBitmapL(const TDesC& aName, TInt aIndex)
	{
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Load(aName, aIndex));
	CleanupStack::Pop(bitmap);
	return bitmap;
	}

/**
Interpolates between the two TRgb values aHi and aLo including alpha channel, with the value aX and the denoinator aN
*/	
TRgb CTe_graphicsperformanceSuiteStepBase::InterpolateColour(TRgb aLo, TRgb aHi, TInt aX, TInt aN)
	{
	TInt y = aN - aX;

	TUint8 a = (TUint8)( (aHi.Alpha()*aX + aLo.Alpha()*y)/aN );
	TUint8 r = (TUint8)( (aHi.Red()*aX + aLo.Red()*y)/aN );
	TUint8 g = (TUint8)( (aHi.Green()*aX + aLo.Green()*y)/aN );
	TUint8 b = (TUint8)( (aHi.Blue()*aX + aLo.Blue()*y)/aN );
	
	return TRgb(r, g, b, a);
	}

/**
Draws a VerticalGradient onto a CFbsBitmap from top/color aLo to bottom/aHi
*/
void CTe_graphicsperformanceSuiteStepBase::VerticalGradientAlphaL(CFbsBitmap* aBitmap, TRgb aLo, TRgb aHi)
	{
	const TSize size = aBitmap->SizeInPixels();
	const TDisplayMode mode = aBitmap->DisplayMode();
	const TInt scanLineLength = CFbsBitmap::ScanLineLength(size.iWidth, mode);
	HBufC8* buffer = HBufC8::NewL(scanLineLength);
	CleanupStack::PushL(buffer);
	TPtr8 des = buffer->Des();
	des.SetLength(scanLineLength);
	for(TInt i=0; i<size.iHeight; i++)
		{
		TRgb color = InterpolateColour(aLo, aHi, i, size.iHeight);
		switch(mode)
			{
			case EGray256:
				{
				TUint8  g = color.Gray256();
				TUint8* p = (TUint8*)des.Ptr();
				for(TInt j=0; j<size.iWidth; j++)
					{						
					p[j] = g;
					}
				}
				break;
			case EColor64K:
				{
				TUint16  g = color._Color64K();
				TUint16* p = (TUint16*)des.Ptr();
				for(TInt j=0; j<size.iWidth/2; j++)
					{						
					p[j] = g;
					}
				}
				break;
			case EColor16MU:
				{
				TUint32 rgba = color._Color16MU();
				TUint32* p = (TUint32*)des.Ptr();
				for(TInt j=0; j<(size.iWidth/4); j++)
					{						
					p[j] = rgba;
					}
				}
				break;
			case EColor16MA:
				{
				TUint32 rgba = color._Color16MA();
				TUint32* p = (TUint32*)des.Ptr();
				for(TInt j=0; j<(size.iWidth/4); j++)
					{						
					p[j] = rgba;
					}
				}
				break;
			case EColor16MAP:
				{
				TUint32 rgba = color._Color16MAP();
				TUint32* p = (TUint32*)des.Ptr();
				for(TInt j=0; j<(size.iWidth/4); j++)
					{						
					p[j] = rgba;
					}
				}
				break;
			default:
				ASSERT(EFalse);
				break;
			}
		aBitmap->SetScanLine(des, i);
		}
	CleanupStack::PopAndDestroy(buffer);
	}

/**
Create a checked board
@param aPixelFormat The pixel format for create the target bitmap
@param aSize The size of the bitmap
@param aChecksPerAxis Number of checks on X and Y.
 */
CFbsBitmap* CTe_graphicsperformanceSuiteStepBase::CreateCheckedBoardL(TDisplayMode aDisplayMode, TSize aSize, TSize aChecksPerAxis) const
	{
	
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	bitmap->Create(aSize, aDisplayMode);
	
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(bitmapDevice);
	
	CFbsBitGc* bitGc = NULL;
	User::LeaveIfError(bitmapDevice->CreateContext(bitGc));
	CleanupStack::PushL(bitGc);
	
	bitGc->Clear();
	bitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bitGc->SetPenStyle(CGraphicsContext::ENullPen);
	TPoint point(0,0);
	const TSize checkerSize((TReal)aSize.iWidth/aChecksPerAxis.iWidth,(TReal)aSize.iHeight/aChecksPerAxis.iHeight);
	TInt brushColour = 0;
	for(point.iY = 0; point.iY < aSize.iHeight; point.iY += checkerSize.iHeight)
		{
		for(point.iX = 0; point.iX < aSize.iWidth; point.iX += checkerSize.iWidth)
			{
			bitGc->SetBrushColor(KColor16Table[brushColour++ & 0x0F]);
			TRect rect(point, checkerSize);
			bitGc->DrawRect(rect);
			}
		}
	
	CleanupStack::PopAndDestroy(2, bitmapDevice);
	CleanupStack::Pop(bitmap);
	
	return bitmap;
	}

/* 
Creates a new CFbsBitmap representing the screen/off-screen bitmap. It is the client's
responsibility to delete it.

@see CTDirectGdiTestBase::GetTargetAsBitmapL.
@return A pointer to the newly created bitmap.
@leave If there was a problem copying from the source to the target.
*/
CFbsBitmap* CTe_graphicsperformanceSuiteStepBase::GetTargetAsBitmapL()
	{
	iScreenDevice->Update();
	CFbsBitmap* bitmapCopy = CopyIntoNewBitmapL(&(iScreenDevice->Bitmap()), iScreenDevice->BitmapDevice().DisplayMode());
	return bitmapCopy;
	}

/**
Creates an mbm file showing the state of the target.

@param aName The name of the mbm file to produce. Any spaces are replaced by underscores
             for easier parsing.
@return KErrNone if bitmap output has been turned off in the config file, or if it was successful.
        KErrNoMemory if there is not enough memory to create the bitmap.
*/
TInt CTe_graphicsperformanceSuiteStepBase::WriteTargetOutput(TPtrC aName)
	{
	if (!iShowBitmaps)
		{
		return KErrNone;
		}
	
	CFbsBitmap* target = NULL;
	TRAPD(err, target = GetTargetAsBitmapL();)
	
	if (err != KErrNone)
		{
		return err;
		}

	TBuf<KMaxFileName> fileName;
	fileName.Append(aName);
	
	// replace spaces with underscores.
	TInt pos = fileName.Locate(TChar(' '));
	while(pos != KErrNotFound)
		{
		fileName[pos] = TChar('_');
		pos = fileName.Locate(TChar(' '));
		}	

	TFileName mbmFile;
	mbmFile.Append(KBitmapDrive);
	mbmFile.AppendFormat(KBitmapPath, &fileName);
		
	err = target->Save(mbmFile);
	delete target;	
	return err;
	}



void CTe_graphicsperformanceSuiteStepBase::ExtractListL(TPtrC aList, RArray<TPtrC>& aListItems)
    {
    TPtrC tempBuf = aList;
    TInt position = tempBuf.Find(_L(","));
            
    while(position != KErrNotFound)
        {
        aListItems.AppendL(tempBuf.Left(position));
        tempBuf.Set(tempBuf.Mid(position + 2));
        position = tempBuf.Find(_L(","));
        }   

    if (position == KErrNotFound)
        {
        aListItems.AppendL(tempBuf);
        }
    }

