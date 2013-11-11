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

#include "tdirectgdi_test_step_base.h"
#include <graphics/directgdiextensioninterfaces.h>

//These reference and test bitmaps are used for image comparison
//These are saved at
_LIT(KRefPath, "\\img\\ref\\%S.mbm");
_LIT(KTestPath, "\\img\\test\\%S.mbm");

//The default image cache size to use for the tests.
const TInt KDriverImageCacheSizeTests = 0x400000;

/**
Position iterator constructor.
@param aStartX Start x position.
@param aEndX End x position.
@param aStepX Step for x position.
@param aStartY Start y position.
@param aEndY End y position.
@param aStepY Step for y position.
*/
TPositionIterator::TPositionIterator(TInt aStartX, TInt aEndX, TInt aStepX,
	TInt aStartY, TInt aEndY, TInt aStepY) :
	iStartX(aStartX), iEndX(aEndX), iStepX(aStepX),
	iStartY(aStartY), iEndY(aEndY), iStepY(aStepY),
	iPosX(aStartX), iPosY(aStartY), iIndexX(0), iIndexY(0)
	{
	}

/**
Begin iteration.
All needed variables are initialized to start iteration.
*/
void TPositionIterator::Begin()
	{
	iPosX = iStartX;
	iPosY = iStartY;
	iIndexX = 0;
	iIndexY = 0;
	}

/**
Next iteration.
Generates next position and position indices.
@return EFalse is returned if end of iterations else ETrue.
*/
TBool TPositionIterator::Next()
	{
	if(iPosX < iEndX)
		{
		iPosX += iStepX;
		iIndexX++;
		return ETrue;
		}
	else
		{
		if(iPosY < iEndY)
			{
			iPosX = iStartX;
			iIndexX = 0;
			iPosY += iStepY;
			iIndexY++;
			return ETrue;
			}
		else
			{
			return EFalse;
			}
		}
	}

CBitmapDevice* CTImageTarget::BitmapDevice() const
	{
	return iBitmapDevice;
	}

/**
Create a new bitgdi target
@param aPixelFormat The pixel format for the target
@param aSize The size of the target to create
@return On return, contains a pointer to a bitgdi target object
*/
CTBitGdiTarget*  CTBitGdiTarget::NewL(TUidPixelFormat aPixelFormat, const TSize& aSize)
	{
	CTBitGdiTarget* self = new(ELeave) CTBitGdiTarget();
	CleanupStack::PushL(self);
	self->ConstructL(aPixelFormat, aSize);
	CleanupStack::Pop(); // self
	return self;
	}

/**
Second phase constructor of CTBitGdiTarget. Creates a bitmap for use as a target.
@param aPixelFormat The pixel format for the target to be created
@param aSize The size of the target to be created
*/
void CTBitGdiTarget::ConstructL(TUidPixelFormat aPixelFormat, const TSize& aSize)
	{	
	// create a bitmap
	iBitmap = new(ELeave) CFbsBitmap;
	iBitmap->Create(aSize, TDisplayModeMapping::MapPixelFormatToDisplayMode(aPixelFormat));
	// create an off screen bitmap device
	iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
	}

/**
Create a bitgdi context
@param aGc A pointer to the created graphics context
@param aActivate ETrue to create and activate the context aGc, EFalse to just create it.
@return On return, contains a pointer to the created bitgdi context
*/
TInt CTBitGdiTarget::CreateContext(CTContextBase*& aGc, TBool aActivate)
	{
	TInt result = KErrGeneral;
	TRAP(result, aGc = CTBitGdiContext::NewL((CFbsBitmapDevice*)iBitmapDevice, aActivate));
   	return result;
   	}

/**
Sets the object to draw to a particular device 
@param aGc A pointer to the created graphics context
*/
TInt CTBitGdiTarget::Activate(CTContextBase*& aGc)
	{
	CTBitGdiContext* gc = (CTBitGdiContext*)aGc;
	// Activate the context on a rendering target.
	return gc->Activate(iBitmapDevice);
	}
/**
Get the size of the device area in pixels
@return On return, The width and height of the target, in pixels 
*/
TSize CTBitGdiTarget::SizeInPixels() const 
	{
	return iBitmapDevice->SizeInPixels();
	}


/**
Get a target FBsBitmap
@return The target FBsBitmap
*/
CFbsBitmap* CTBitGdiTarget::GetTargetFbsBitmapL()
	{
	return iBitmap;
	}

/**
Nothing to be implemented for BitGdi Finish()
*/
void CTBitGdiTarget::Finish ()
	{
	}


void CTBitGdiTarget::Close() 
	{	
	}


/**
Destructor of CTBitGdiTarget
*/
CTBitGdiTarget::~CTBitGdiTarget()
	{
	delete iBitmapDevice;
	delete iBitmap;
	}

/**
Default constructor.
*/
CTDirectGdiTarget::CTDirectGdiTarget()
	{
	}

/**
Create a new directgdi target
@param aPixelFormat The pixel format for the target
@param aSize The size of the target to create
@return On return, contains a pointer to a directgdi target object
*/
CTDirectGdiTarget*  CTDirectGdiTarget::NewL(TUidPixelFormat aPixelFormat, const TSize& aSize)
	{
	CTDirectGdiTarget* self = new(ELeave) CTDirectGdiTarget();
	CleanupStack::PushL(self);
	self->ConstructL(aPixelFormat, aSize);
	CleanupStack::Pop(); // self
	return self;
	}
/**
Second phase constructor of CTDirectGdiTarget
@param aPixelFormat The pixel format for the target
@param aSize The size of the target to create
*/
void CTDirectGdiTarget::ConstructL(TUidPixelFormat aPixelFormat, const TSize& aSize)
	{	
	// open the driver
	User::LeaveIfError(CDirectGdiDriver::Open());
	iDGdiDriver = CDirectGdiDriver::Static();
	if (iDGdiDriver == NULL)
		User::Leave(KErrNoMemory);
	
	// set a large cache size (if available to do so) so the tests execute more quickly.
	MDirectGdiDriverCacheSize* driverCacheInterface = NULL;
	if (KErrNone == iDGdiDriver->GetInterface(TUid::Uid(KDirectGdiDriverCacheSizeUid), (TAny*&)driverCacheInterface))
		{
		User::LeaveIfError(driverCacheInterface->SetMaxImageCacheSize(KDriverImageCacheSizeTests));
		}
	
	iDGdiImageTarget = new (ELeave) RDirectGdiImageTarget(*iDGdiDriver);
	// create a bitmap which is used to save the image data from an RSgImage to a file.
	iBitmap = new(ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(aSize, TDisplayModeMapping::MapPixelFormatToDisplayMode(aPixelFormat)));
	iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
	// Set up image attributes
	iImageInfo.iSizeInPixels = aSize;
	iImageInfo.iPixelFormat = aPixelFormat;
	iImageInfo.iUsage = ESgUsageDirectGdiTarget;
	User::LeaveIfError(iRSgImage.Create(iImageInfo, NULL,0));
	User::LeaveIfError(iDGdiImageTarget->Create(iRSgImage));
	}


/**
Create a Direct GDI graphics context.

@param aGc	A reference to a pointer to the created graphics context. If return is anything 
			other than KErrNone then aGc is set to NULL.
@param aActivate ETrue to create and activate the context aGc, EFalse to just create it.

@return KErrNone, if successful; otherwise, another of the system-wide error codes.
*/
TInt CTDirectGdiTarget::CreateContext(CTContextBase*& aGc, TBool aActivate)
	{	
	TInt result = KErrGeneral;
	
	aGc = NULL;
	
    TRAP(result, aGc = CTestDirectGdiContext::NewL());
    CTestDirectGdiContext* gc = (CTestDirectGdiContext*)aGc;

    // If the GC is not NULL, make sure the return code indicates
    // success.
    if (aActivate && aGc)
    	{
		if(result != KErrNone)
			{
			return result;
			}		
		else
			{
			// Activate the context on a rendering target.
			result = gc->Activate(*iDGdiImageTarget);
			}
    	}	
	return result;
	}

/**
Sets the object to draw to a particular device 
@param aGc A pointer to the created graphics context
*/
TInt CTDirectGdiTarget::Activate(CTContextBase*& aGc)
	{
    CTestDirectGdiContext* gc = (CTestDirectGdiContext*)aGc;
	// Activate the context on a rendering target.
    return gc->Activate(*iDGdiImageTarget);
	}

/**
Get the size of the device area in pixels
@return On return, The width and height of the target, in pixels 
*/
TSize CTDirectGdiTarget::SizeInPixels() const 
	{
	return iImageInfo.iSizeInPixels;
	}

/**
Get a target FBsBitmap.
@return The target FBsBitmap
*/
CFbsBitmap* CTDirectGdiTarget::GetTargetFbsBitmapL()
	{
	// Create a copy of the RSgImage that is CPU-accessible for mapping.
	RSgImage cpuAccessibleImage;
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = iImageInfo.iSizeInPixels;
	imageInfo.iPixelFormat = iImageInfo.iPixelFormat;
	imageInfo.iCpuAccess = ESgCpuAccessReadOnly;
	User::LeaveIfError(cpuAccessibleImage.Create(imageInfo, iRSgImage));
	CleanupClosePushL(cpuAccessibleImage);
	
	iBitmap->BeginDataAccess();
	const TInt bitmapDataStride = iBitmap->DataStride();
	const TInt numScanlines = imageInfo.iSizeInPixels.iHeight;
	const TAny* sgImageDataAddress;
	TInt sgImageDataStride = 0;
	
	User::LeaveIfError(cpuAccessibleImage.MapReadOnly(sgImageDataAddress, sgImageDataStride));

    TUint32* bitmapDataAddress = iBitmap->DataAddress();
    for (TInt scanline = 0; scanline < numScanlines; ++scanline)
    	{
    	Mem::Copy((TUint8*)bitmapDataAddress + (scanline*bitmapDataStride), (TUint8*)sgImageDataAddress + (scanline*sgImageDataStride), bitmapDataStride);
    	}
    iBitmap->EndDataAccess();
    cpuAccessibleImage.Unmap();

	CleanupStack::PopAndDestroy(1);
    	
    return iBitmap;
	}

/**
Force completion of all batched rendering operations. Will not return
until rendering is complete.

@pre	Fully initialised image target.
*/
void CTDirectGdiTarget::Finish ()
	{	
	if (iDGdiDriver)
		iDGdiDriver->Finish();
	}


void CTDirectGdiTarget::Close() 
	{
	if (iDGdiImageTarget)
		iDGdiImageTarget->Close();
	}

/**
Destructor of CTDirectGdiTarget
*/
CTDirectGdiTarget::~CTDirectGdiTarget()
	{
	delete iBitmapDevice;
	delete iBitmap;
	if (iDGdiImageTarget)
		{
		iDGdiImageTarget->Close();	
		delete iDGdiImageTarget;
		}
	if (iDGdiDriver)
		iDGdiDriver->Close();
	iRSgImage.Close();
	}

/**
Implementation of CTestStep base class virtual
It is used for doing all common initialisation to derived classes.
Make it being able to leave 
The leave will be picked up by the framework.
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTDirectGdiStepBase::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	
	// Create and install Active Scheduler in case tests require active objects
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	TPtrC targetPixelFormatInput;
	TPtrC sourcePixelFormatInput;
	TPtrC sourceResourcePixelFormatInput;
	
	// get input for tests from .ini file
	_LIT(KDirectgdiTestInput, "DirectgdiTestInput");
	_LIT(KTargetPixelFormatEnums, "TargetPixelFormatEnums ");
	_LIT(KSourcePixelFormatEnums, "SourcePixelFormatEnums ");
	_LIT(KSourceResourcePixelFormatEnums, "SourceResourcePixelFormatEnums ");
	_LIT(KDoRefImg, "DoRefImg");
	_LIT(KDoDirectGdi, "DoDirectGdi");
	_LIT(KRunOomTests, "RunOomTests");
	TEST(GetStringFromConfig(KDirectgdiTestInput, KTargetPixelFormatEnums, targetPixelFormatInput));
	TEST(GetStringFromConfig(KDirectgdiTestInput, KSourcePixelFormatEnums, sourcePixelFormatInput));
	TEST(GetStringFromConfig(KDirectgdiTestInput, KSourceResourcePixelFormatEnums, sourceResourcePixelFormatInput));
	TEST(GetIntFromConfig(KDirectgdiTestInput, KDoRefImg, iMakeRefImg));
	TEST(GetIntFromConfig(KDirectgdiTestInput, KDoDirectGdi, iUseDirectGdi));
	TEST(GetIntFromConfig(KDirectgdiTestInput, KRunOomTests, iDoOomTests));
	#ifndef _DEBUG
	if(iDoOomTests)
		{
		iDoOomTests = EFalse;
		INFO_PRINTF1(_L("WARNING: Can't run out of memory tests under a release build. OOM tests set to run in ini file so turning OOM tests off."));
		}
	#endif

	ConvertPixelFormats(targetPixelFormatInput, iTargetPixelFormatArray);
	ConvertPixelFormats(sourcePixelFormatInput, iSourcePixelFormatArray);
	ConvertPixelFormats(sourceResourcePixelFormatInput, iSourceResourcePixelFormatArray);
	iRunningOomTests = EFalse;

	Logger().ShareAuto();

	__UHEAP_MARK;		
	User::LeaveIfError(SgDriver::Open());
	User::LeaveIfError(RFbsSession::Connect());	
		
	return TestStepResult();
	}

/**
Convert the pixel format strings to pixel format enums
@param aPixelFormatInput The pixel format string for testing
@param aPixelFormatArray The array of converted pixel formats is returned here
*/
void CTDirectGdiStepBase::ConvertPixelFormats(TPtrC aPixelFormatInput, RArray<TUidPixelFormat>& aPixelFormatArray)
	{
	TPtrC tempBuf = aPixelFormatInput;
	TInt position = tempBuf.Find(_L(","));
			
	while(KErrNotFound != position)
		{
		aPixelFormatArray.Append(TDisplayModeMapping::ConvertPixelFormatStringToPixelFormat(tempBuf.Left(position)));
		tempBuf.Set(tempBuf.Mid(position + 2));
		position = tempBuf.Find(_L(","));
		}	

	if (position == KErrNotFound)
		{
		aPixelFormatArray.Append(TDisplayModeMapping::ConvertPixelFormatStringToPixelFormat(tempBuf));
		}
	}

/**
Implementation of CTestStep base class virtual
It is used for doing all after test treatment common to derived classes in here.
Make it being able to leave
The leave will be picked up by the framework.
@leave Gets system wide error code
@return - TVerdict
*/
 TVerdict CTDirectGdiStepBase::doTestStepPostambleL()
	{
	if(iFontStore)
		{
		if(iFontId)
			{
			iFontStore->RemoveFile(iFontId);
			iFontId = 0;
			}
		delete iFontStore;
		iFontStore = NULL;
		}
	delete iGc;
	iGc = NULL;
	delete iGc2;
	iGc2 = NULL;
	delete iGdiTarget;
	iGdiTarget = NULL;
	delete iGdiTarget2;
	iGdiTarget2 = NULL;	
	SgDriver::Close();
	RFbsSession::Disconnect();
	__UHEAP_MARKEND;
	delete iScheduler;
	iScheduler = NULL;
	return TestStepResult();
	}

CTDirectGdiStepBase::~CTDirectGdiStepBase()
	{
	if(iFontStore)
		{
		if(iFontId)
			{
			iFontStore->RemoveFile(iFontId);			
			}
		delete iFontStore;
		}
	delete iScheduler;
	delete iGc;
	delete iGc2;
	delete iGdiTarget;
	delete iGdiTarget2;
	iTargetPixelFormatArray.Close();
	iSourcePixelFormatArray.Close();
	iSourceResourcePixelFormatArray.Close();
	}

CTDirectGdiStepBase::CTDirectGdiStepBase()
	{
	}

/**
Create a name in the format of <TestImage / RefImage>_< TestCase Name >_<DisplayModeName>_<Orientation>_<PostFix(optional)>
@param aParams A structure object of parameters to be passed through
@param aTestName The test name to be generated 
@param aTestCaseName The test case name passed in, to be used in the resultant filename
@param aNamePostfix If this is not NULL, it is appended to filename with an underscore in front of it
*/
void CTDirectGdiStepBase::CreateFileName(TTestParams& aParams, TDes& aTestName, TPtrC& aTestCaseName, TDesC* aNamePostfix)
	{	
	if (iUseDirectGdi)
		{
		aTestName.Append(KDirectGc);
		}
	else
		{
		aTestName.Append(KBitGc);		
		}
	
	if (aParams.iDoCompressed)
		{
		aTestName.Append(KSeparator);	
		aTestName.Append(KCom);
		}

	aTestName.Append(KSeparator);	
	aTestName.Append(aTestCaseName);
	aTestName.Append(KSeparator);
	aTestName.Append(KTargetString);
	aTestName.Append(KSeparator);
	aTestName.Append(TDisplayModeMapping::ConvertPixelFormatToShortPixelFormatString(aParams.iTargetPixelFormat));
	
	if (aNamePostfix)
		{
		aTestName.Append(KSeparator);
		aTestName.Append(*aNamePostfix);
		}
	}

/**
@see DisplayTargetImageL(TUidPixelFormat, CTImageTarget*)
 */
void CTDirectGdiStepBase::DisplayTargetImageL(TUidPixelFormat aPixelFormat)
	{
	DisplayTargetImageL(aPixelFormat, iGdiTarget);
	}

/**
Bitblt an image to screen
@param aPixelFormat The pixel format for tests
@param aGdiTarget The target image to blit
*/
void CTDirectGdiStepBase::DisplayTargetImageL(TUidPixelFormat aPixelFormat, CTImageTarget* aGdiTarget)
	{
	// Ensure the image has been completly rendered.
	aGdiTarget->Finish();
	CFbsScreenDevice* screenDevice = CFbsScreenDevice::NewL(_L("scdv"), TDisplayModeMapping::MapPixelFormatToDisplayMode(aPixelFormat));
	TEST(screenDevice!=NULL);
	CleanupStack::PushL(screenDevice);
	
	CFbsBitGc* bitGc;
	screenDevice->CreateContext(bitGc);
	TEST(bitGc!=NULL);
	CleanupStack::PushL(bitGc);

 	TPoint startPoint = TPoint(0, 0);
	bitGc->Clear();
	bitGc->BitBlt(startPoint, aGdiTarget->GetTargetFbsBitmapL()); 		
	screenDevice->Update();

	CleanupStack::PopAndDestroy(2, screenDevice);	
	}

/**
Initialises a target (or targets) and a graphics context (or contexts), depending on the value passed in aCase.
Deletes the previously used target(s) and context(s).
@param aPixelFormat The pixel format to be applied
@param aCase The type of test that this target is for, for example when testing with one context and two targets
this could be EOneContextTwoTargets_SamePixelType, the default value is EOneContextOneTarget.
@param aSize The size of the target(s) to be created
*/
void CTDirectGdiStepBase::SetTargetL(TUidPixelFormat aPixelFormat, const TContextTestCase aCase, const TSize& aSize)
	{
	if(!iRunningOomTests)
		{
		TBuf<KPixelFormatNameLength> pixelFormatName(TDisplayModeMapping::ConvertPixelFormatToPixelFormatString(aPixelFormat));
		INFO_PRINTF4(_L("SetTarget (using off screen bitmap): %S %dx%d"), &pixelFormatName, aSize.iWidth, aSize.iHeight);
		}
	
	delete iGc;
	iGc = NULL;	
	delete iGc2;
	iGc2 = NULL;	
	delete iGdiTarget;
	iGdiTarget = NULL;
	delete iGdiTarget2;
	iGdiTarget2 = NULL;
	TUidPixelFormat aPixelFormat2 = aPixelFormat;
	switch(aPixelFormat)
		{
		case EUidPixelFormatRGB_565:
			aPixelFormat2 = EUidPixelFormatARGB_8888_PRE;
			break;			
		case EUidPixelFormatARGB_8888_PRE:	
			aPixelFormat2 = EUidPixelFormatXRGB_8888;	
			break;			
		case EUidPixelFormatXRGB_8888:		
			aPixelFormat2 = EUidPixelFormatRGB_565;	
			break;
		}	
		
	switch(aCase)
		{
		case EOneContextOneTarget:				
			{
			if(iUseDirectGdi)
				{
				iGdiTarget = CTDirectGdiTarget::NewL(aPixelFormat, aSize);		
				}
			else
				{
				iGdiTarget = CTBitGdiTarget::NewL(aPixelFormat, aSize);
				}
			User::LeaveIfError(iGdiTarget->CreateContext(iGc));	
			}
			break;
		case EOneContextTwoTargets_SamePixelType:
			{			
			if(iUseDirectGdi)
				{
				iGdiTarget = CTDirectGdiTarget::NewL(aPixelFormat, aSize);		
				iGdiTarget2 = CTDirectGdiTarget::NewL(aPixelFormat, aSize);		
				}
			else
				{
				iGdiTarget = CTBitGdiTarget::NewL(aPixelFormat, aSize);
				iGdiTarget2 = CTBitGdiTarget::NewL(aPixelFormat, aSize);		
				}
			User::LeaveIfError(iGdiTarget->CreateContext(iGc));				
			}
			break;
		case EOneContextTwoTargets_DifferentPixelType:
			{	
			if(iUseDirectGdi)
				{
				iGdiTarget = CTDirectGdiTarget::NewL(aPixelFormat, aSize);		
				iGdiTarget2 = CTDirectGdiTarget::NewL(aPixelFormat2, aSize);		
				}
			else
				{
				iGdiTarget = CTBitGdiTarget::NewL(aPixelFormat, aSize);
				iGdiTarget2 = CTBitGdiTarget::NewL(aPixelFormat2, aSize);		
				}
			User::LeaveIfError(iGdiTarget->CreateContext(iGc));				
			}
			break;
		case ETwoContextsOneTarget:
			{
			if(iUseDirectGdi)
				{
				iGdiTarget = CTDirectGdiTarget::NewL(aPixelFormat, aSize);		
				}
			else
				{
				iGdiTarget = CTBitGdiTarget::NewL(aPixelFormat, aSize);
				}
			User::LeaveIfError(iGdiTarget->CreateContext(iGc, EFalse));	
			User::LeaveIfError(iGdiTarget->CreateContext(iGc2, EFalse));		
			}		
			break;
		case ETwoContextsTwoTargets_WithoutSharing_SamePixelType:
		case ETwoContextsTwoTargets_WithSharing_SamePixelType:
			{
			if(iUseDirectGdi)
				{
				iGdiTarget = CTDirectGdiTarget::NewL(aPixelFormat, aSize);		
				iGdiTarget2 = CTDirectGdiTarget::NewL(aPixelFormat, aSize);		
				}
			else
				{
				iGdiTarget = CTBitGdiTarget::NewL(aPixelFormat, aSize);
				iGdiTarget2 = CTBitGdiTarget::NewL(aPixelFormat, aSize);
				}
			User::LeaveIfError(iGdiTarget->CreateContext(iGc, EFalse));	
			User::LeaveIfError(iGdiTarget2->CreateContext(iGc2, EFalse));	
			}
			break;
		case ETwoContextsTwoTargets_WithoutSharing_DifferentPixelType:
		case ETwoContextsTwoTargets_WithSharing_DifferentPixelType:
			{
			if(iUseDirectGdi)
				{
				iGdiTarget = CTDirectGdiTarget::NewL(aPixelFormat, aSize);		
				iGdiTarget2 = CTDirectGdiTarget::NewL(aPixelFormat2, aSize);		
				}
			else
				{
				iGdiTarget = CTBitGdiTarget::NewL(aPixelFormat, aSize);
				iGdiTarget2 = CTBitGdiTarget::NewL(aPixelFormat2, aSize);
				}
			User::LeaveIfError(iGdiTarget->CreateContext(iGc, EFalse));	
			User::LeaveIfError(iGdiTarget2->CreateContext(iGc2, EFalse));	
			}
			break;									
		case EOneContextOneTarget_TwiceActivate:
			{
			if(iUseDirectGdi)
				{
				iGdiTarget = CTDirectGdiTarget::NewL(aPixelFormat, aSize);		
				}
			else
				{
				iGdiTarget = CTBitGdiTarget::NewL(aPixelFormat, aSize);
				}
			User::LeaveIfError(iGdiTarget->CreateContext(iGc));	
			}
			break;		
		}	

	// setup font store
	if(iFontStore)
		{
		if(iFontId)
			{
			iFontStore->RemoveFile(iFontId);
			iFontId = 0;
			}
		delete iFontStore;
		iFontStore = NULL;
		}
	
	iFontStore = CFbsTypefaceStore::NewL(iGdiTarget->BitmapDevice());

	_LIT(KEonFontFileName,"z:\\resource\\fonts\\eon14.gdr");
	TESTL(KErrNone == iFontStore->AddFile(KEonFontFileName, iFontId));
	
	// Reset VGImage cache - OpenVG DirectGDI only
	iVgImageCache = NULL;
	if (iUseDirectGdi)
		{
		TInt err = iGc->GetInterface(TUid::Uid(KDirectGdiVgImageCacheUid), (TAny*&) iVgImageCache);
		if (KErrNotSupported == err)
			{
			// Must be using software DirectGDI as this does not have a VGImage cache
			iUseSwDirectGdi = ETrue;
			}
		}
	}

/**
Get a font by looking for it in the font store. Only gets "DejaVu Sans Mono" at the moment.
@return Returns a pointer to the "DejaVu Sans Mono" font if it is found in the font store, returns NULL if it not found.
*/
CFont* CTDirectGdiStepBase::GetFont()
	{
	_LIT(KFontFamily, "DejaVu Sans Mono");

	CFont* font = NULL;
	TFontSpec spec(KFontFamily, 13);
	iFontStore->GetNearestFontInPixels(font, spec);
	return font;
	}

/**
Releases the passed font in the font store.
@param aFont A pointer to the font to be released.
 */
void CTDirectGdiStepBase::ReleaseFont(CFont* aFont)
	{
	if(aFont && iFontStore)
		{
		iFontStore->ReleaseFont(aFont);
		}
	}

/**
Create a file and save the target image to the file with a filename created from aTestCaseName and aNamePostfix
@param aParams A structure object of parameters to be passed through
@param aTestCaseName The name of the test case
@param aImageTarget The image target to be written out to the file
@param aNamePostfix A postfix to be appended to the test case name
@see CreateFileName()
@return KErrNone if successful, one of the system wide error codes otherwise
*/
TInt CTDirectGdiStepBase::WriteTargetOutput(TTestParams& aParams, TPtrC aTestCaseName, CTImageTarget* aImageTarget, TDesC* aNamePostfix)
	{
	// Finish is called for the OOM tests as well as the normal tests as it is responsible for
	// clearing up the pending image array. Images get left in the array and cause the OOM tests
	// for DrawResource to fail if Finish is not called.
	aImageTarget->Finish();
	
	if(!iRunningOomTests) // don't want to save test images when running our of memory twsts
		{
		TBuf<KFileNameLength> testFileName;
		CreateFileName(aParams, testFileName, aTestCaseName, aNamePostfix);
		TFileName mbmFile;
		TBuf<KFileNameLength> testPathName;
		#ifdef __WINS__
			testPathName.Append(_L("c:"));
		#else
			testPathName.Append(_L("e:"));
		#endif
	
		if (iMakeRefImg)
			{
			testPathName.Append(KRefPath);
			mbmFile.Format(testPathName, &testFileName);
			}
		else
			{
			testPathName.Append(KTestPath);
			mbmFile.Format(testPathName, &testFileName);
			}
		INFO_PRINTF1(testFileName);
		
		CFbsBitmap* targetBitmap = NULL;
		TRAPD(err, targetBitmap = aImageTarget->GetTargetFbsBitmapL());
		return (err != KErrNone) ? err : targetBitmap->Save(mbmFile);		
		}
	return KErrNone;
	}

/**
Create a file and save the current target image to the file with a filename created from aTestCaseName and aNamePostfix
@param aParams A structure object of parameters to be passed through
@param aTestCaseName The name of the test case
@param aNamePostfix A postfix to be appended to the test case name
@see CreateFileName()
@return KErrNone if successful, one of the system wide error codes otherwise
*/
TInt CTDirectGdiStepBase::WriteTargetOutput(TTestParams& aParams, TPtrC aTestCaseName, TDesC* aNamePostfix)
	{
	return WriteTargetOutput(aParams, aTestCaseName, iGdiTarget, aNamePostfix);
	}

/**
Reset the graphics context
*/
void CTDirectGdiStepBase::ResetGc()
	{
	iGc->Reset();
	iGc->Clear();
	}

/**
Compares the target image pixel by pixel against the given colour. If a pixel with a  
different colour than aColour is found then the test will fail, otherwise the test will pass.
@param aColour A colour rgba value to find.
@return ETrue if test passed or EFalse if test failed.
*/
TBool CTDirectGdiStepBase::TestTargetL(const TRgb& aColour)
	{
	iGdiTarget->Finish();
		
	CFbsBitmap* bitmap = iGdiTarget->GetTargetFbsBitmapL();
	TInt width = bitmap->SizeInPixels().iWidth;
	TInt height = bitmap->SizeInPixels().iHeight;

	HBufC8* lineBuf = HBufC8::NewLC(width*4);
	TPtr8 linePtr(lineBuf->Des());

	TBool pass = ETrue;

	for(TInt line=0; line<height; line++)
		{
		bitmap->GetScanLine(linePtr, TPoint(0, line), width, EColor16MA);

		const TUint8* pPtr = linePtr.Ptr();
		for(TInt x=0; x<width; x++)
			{
			// EColor16MA pixel representation is 32-bit BGRA
			TRgb pColour(pPtr[2], pPtr[1], pPtr[0], pPtr[3]);
			pPtr += 4;
			
			if (iTestParams.iTargetPixelFormat == EUidPixelFormatXRGB_8888)
				{
				if ((pColour.Value() & 0xFFFFFF) != (aColour.Value() & 0xFFFFFF))
					{
					pass = EFalse;
					break; // break inner loop
					}
				}
			else if(pColour != aColour)
				{
				pass = EFalse;
				break; // break inner loop
				}
			}

		if(!pass)
			break; // break outer loop if test failed in inner loop
		}

	CleanupStack::PopAndDestroy(lineBuf);

	return pass;
	}

/**
Checks the graphics context for the passed error codes and logs an error if the codes do not match. 
If the tests are running using DirectGdi then aDirectGdiErrorCode is checked against the current error in the
graphics context, if the tests are running using BitGdi then aBitGdiErrorCode is checked against the current 
error in the graphics context.
@param aDirectGdiErrorCode The DirectGdi error code to check when the tests are running using DirectGdi
@param aBitGdiErrorCode The BitGdi error code to check when the tests are running using BitGdi
@param aFile The filename to use when reporting the error
@param aLine The line number to use when reporting the error
*/
void CTDirectGdiStepBase::CheckErrorsL(TInt aDirectGdiErrorCode, TInt aBitGdiErrorCode, const TText8* aFile, TInt aLine)
	{
	if(iUseDirectGdi)
		{
		TESTWITHFILENAMEANDLINENUMBERL(iGc->GetError() == aDirectGdiErrorCode, aFile, aLine);
		}
	else
		{
		TESTWITHFILENAMEANDLINENUMBERL(iGc->GetError() == aBitGdiErrorCode, aFile, aLine);
		}
	}

/**
Derived classes should override this method and add their calls to individual test cases in
the overridden version.
*/
void CTDirectGdiStepBase::RunTestsL()
	{
	}

/**
Runs the tests in RunTestsL checking for Out of Memory conditions and testing for memory leaks.
*/
void CTDirectGdiStepBase::RunOomTestsL()
	{
	if (!iDoOomTests)
		{
		// don't run the out of memory tests
		return;
		}
	TInt tryCount = 0;
	INFO_PRINTF1(_L("*****Running Out Of Memory Tests*****"));
	
	// save the current state of test step results
	TVerdict currentTestStepResult = TestStepResult();

	FOREVER
		{		
		iRunningOomTests = ETrue;
		TInt err = KErrNone;
		
		// count cells so we can know how many we leaked
		TInt cellsStart = User::CountAllocCells();
		
		__UHEAP_FAILNEXT(++tryCount);
		__UHEAP_MARK;
		
		TRAP(err, RunTestsL());

		TBool finishedCorrectly = EFalse;
		if ((err == KErrNone))
			{
			// claims to have finished correctly, and we're not failing every alloc
			finishedCorrectly = CheckForHeapFailNext();
			}		
		__UHEAP_RESET;
		TInt cellsEnd = User::CountAllocCells();
		if (cellsStart < cellsEnd)
			{
			// leaked.
			TInt leakedCells = cellsEnd - cellsStart;
			ERR_PRINTF3(_L("On loop number %d we leaked %d cells. About to cause panic."),tryCount,leakedCells);				
			}
		__UHEAP_MARKEND;
		
		// check to see if we finished all OOM testing successfully
		if ((err == KErrNone) && finishedCorrectly)
			{
			INFO_PRINTF2(_L(" - Test completed successfully after %d iterations."),tryCount);
			break;
			}
		}
	// restore test step result and ignore any test failures the out of memory tests produce
	SetTestStepResult(currentTestStepResult);
	iRunningOomTests = EFalse;
	}

/**
Tests that the passed condition aCondition is equal to ETrue and reports an error if it is not.
This method does not report errors when the tests are running in OOM mode.
@see testBooleanTrue()
@param aCondition The boolean value to be checked for being equal to ETrue
@param aFile The filename to use when reporting the error
@param aLine The line number to use when reporting the error
*/
void CTDirectGdiStepBase::testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine) 
	{
	if(!iRunningOomTests)
		{
		CTestStep::testBooleanTrue(aCondition, aFile, aLine, ETrue);
		}
	}

/**
A leaving version of testBooleanTrue().
@see testBooleanTrue()
*/
void CTDirectGdiStepBase::testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine) 
	{
	if(!iRunningOomTests)
		{
		CTestStep::testBooleanTrueL(aCondition, aFile, aLine, ETrue);
		}
	else
		{
		if(!aCondition)
			{
			User::Leave(TEST_ERROR_CODE);
			}
		}
	}

/**
A version of testBooleanTrue() that additionally writes an error code to the output if aCondition is not equal to ETrue.
@see testBooleanTrue()
@param aCondition The boolean value to be checked for being equal to ETrue
@param aErrorCode An error code to be reported if aCondition is EFalse
@param aFile The filename to use when reporting the error
@param aLine The line number to use when reporting the error
*/
void CTDirectGdiStepBase::testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine)
	{
	if(!iRunningOomTests)
		{
		if(!aCondition)			
			{
			SetTestStepResult(EFail);
			_LIT(KMessage,"Test Failed with error [%d]");
			Logger().LogExtra(aFile, aLine, ESevrErr, KMessage, aErrorCode);
			}
		}
	}

/**
A leaving version of testBooleanTrueWithErrorCode()
@see testBooleanTrueWithErrorCode()
@param aCondition The boolean value to be checked for being equal to ETrue
@param aErrorCode An error code to be reported if aCondition is EFalse
@param aFile The filename to use when reporting the error
@param aLine The line number to use when reporting the error
*/
void CTDirectGdiStepBase::testBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine)
	{
	if(!iRunningOomTests)
		{
		testBooleanTrueWithErrorCode(aCondition, aErrorCode, aFile, aLine);
		}
	else
		{
		if(!aCondition)
			{
			User::Leave(aErrorCode);
			}
		}
	}

/**
Creates a CFbsBitmap that has a pattern of differently coloured concentric rectangles on it for use in test cases.
@param aPixelFormat The pixel format for create the target bitmap
@param aSize The size of the bitmap to be created
*/
CFbsBitmap* CTDirectGdiStepBase::CreateConcentricRectsBitmapL(TUidPixelFormat aPixelFormat, const TSize& aSize)
	{
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);

	TESTNOERRORL(bitmap->Create(aSize, TDisplayModeMapping::MapPixelFormatToDisplayMode(aPixelFormat)));

	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
	TESTL(bitmapDevice!=NULL);
	CleanupStack::PushL(bitmapDevice);
	
	CFbsBitGc* bitGc = NULL;
	User::LeaveIfError(bitmapDevice->CreateContext(bitGc));
	TESTL(bitGc!=NULL);
	CleanupStack::PushL(bitGc);

	for (TInt i = aSize.iWidth/2; i>0; --i)
		{
		bitGc->SetPenColor(KColor16Table[i%16]);	
		bitGc->DrawRect(TRect(i,i,aSize.iWidth - i, aSize.iHeight - i));
		}

	CleanupStack::PopAndDestroy(2, bitmapDevice);
	CleanupStack::Pop(bitmap);
	
	return bitmap;
	}

/**
Creates a bitmap that contains a checked board pattern of coloured rectangles for use in test cases.
@param aPixelFormat The pixel format for create the target bitmap
@param aSize The size of the bitmap
@param aChecksPerAxis Number of checks on X and Y.
@param aGenAlpha If ETrue then generate pattern in alpha channel.
*/
CFbsBitmap* CTDirectGdiStepBase::CreateCheckedBoardBitmapL(TUidPixelFormat aPixelFormat,
		const TSize& aSize, const TSize& aChecksPerAxis, TBool aGenAlpha)
	{
	
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	TESTNOERRORL(bitmap->Create(aSize, TDisplayModeMapping::MapPixelFormatToDisplayMode(aPixelFormat)));
	
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(bitmapDevice);
	TESTL(bitmapDevice!=NULL);
	
	CFbsBitGc* bitGc = NULL;
	User::LeaveIfError(bitmapDevice->CreateContext(bitGc));
	CleanupStack::PushL(bitGc);
	TESTL(bitGc!=NULL);
	
	bitGc->Clear();
	bitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bitGc->SetPenStyle(CGraphicsContext::ENullPen);
	if(aGenAlpha)
		{
		bitGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
		}
	TPoint point(0,0);
	const TSize checkerSize(aSize.iWidth/aChecksPerAxis.iWidth,aSize.iHeight/aChecksPerAxis.iHeight);
	TInt brushColour = 0x00;
	for(point.iY = 0; point.iY < aSize.iHeight; point.iY += checkerSize.iHeight)
		{
		for(point.iX = 0; point.iX < aSize.iWidth; point.iX += checkerSize.iWidth)
			{
			TRgb colour = KColor16Table[brushColour++ & 0x0F];
			if(aGenAlpha)
				{
				// Note that this is converted internally to pre-multiplied alpha
				// for pre-multiplied alpha targets.
				colour.SetAlpha((brushColour*5) & 255);
				
				//Use the following line to simplify test for manual verification.
				//colour.SetAlpha(0x80);
				}
			bitGc->SetBrushColor(colour);
			TRect rect(point, checkerSize);
			bitGc->DrawRect(rect);
			}
		}
	CleanupStack::PopAndDestroy(2, bitmapDevice);
	CleanupStack::Pop(bitmap);
	
	return bitmap;
	}

/**
Create a black and white checked bitmap
@param aPixelFormat The pixel format to use when creating the target bitmap
@param aSize The size of the bitmap to create
@param aChecksPerAxis Number of checks to draw in the X and Y directions
@param aIsHardwareBitmap If ETrue, creates a hardware CFbsBitmap.
*/
CFbsBitmap* CTDirectGdiStepBase::CreateBlackWhiteBitmapL(TUidPixelFormat aPixelFormat,
		const TSize& aSize, const TSize& aChecksPerAxis)
	{
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	
	TESTNOERRORL(bitmap->Create(aSize, TDisplayModeMapping::MapPixelFormatToDisplayMode(aPixelFormat)));
	
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(bitmapDevice);
	TESTL(bitmapDevice!=NULL);

	CFbsBitGc* bitGc = NULL;
	bitmapDevice->CreateContext(bitGc);
	CleanupStack::PushL(bitGc);
	TESTL(bitGc!=NULL);

	bitGc->Clear();
	bitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bitGc->SetPenStyle(CGraphicsContext::ENullPen);
	TPoint point(0,0);	
	const TSize checkerSize(aSize.iWidth/aChecksPerAxis.iWidth,aSize.iHeight/aChecksPerAxis.iHeight);
	for(point.iY = 0; point.iY < aSize.iHeight; point.iY += checkerSize.iHeight)
		{
		TBool isBlack = ETrue;
		for(point.iX = 0; point.iX < aSize.iWidth; point.iX += checkerSize.iWidth)
			{
			if(isBlack)
				bitGc->SetBrushColor(KRgbBlack);
			else 
				bitGc->SetBrushColor(KRgbWhite);
			TRect rect(point, checkerSize);
			bitGc->DrawRect(rect);
			isBlack = EFalse;
			}
		}	
	CleanupStack::PopAndDestroy(2, bitmapDevice);
	CleanupStack::Pop(bitmap);	
	return bitmap;
	}

/**
Create a bitmap designed for masking tests.
@param aPixelFormat The pixel format to use when creating the bitmap
@param aSize The size of the bitmap to create
*/
CFbsBitmap* CTDirectGdiStepBase::CreateMaskingPixmapL (
		TUidPixelFormat aPixelFormat, 
		const TSize& aSize)
	{
	
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	TESTL(KErrNone == bitmap->Create(aSize, TDisplayModeMapping::MapPixelFormatToDisplayMode(aPixelFormat)));
	
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(bitmapDevice);
	TESTL(bitmapDevice!=NULL);
	
	CFbsBitGc* bitGc = NULL;
	bitmapDevice->CreateContext(bitGc);
	CleanupStack::PushL(bitGc);
	TESTL(bitGc!=NULL);
	
	bitGc->Clear();
	bitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bitGc->SetPenStyle(CGraphicsContext::ENullPen);

	bitGc->SetBrushColor(TRgb(0x808080));
	TRect rect(aSize);
	bitGc->DrawRect(rect);

	bitGc->SetBrushColor(TRgb(0x202020));
	TRect rect2(0, 0, aSize.iWidth, aSize.iHeight>>2);
	bitGc->DrawRect(rect2);
	
	TRect rect3(0, 0, aSize.iWidth>>3, aSize.iHeight);
	bitGc->DrawRect(rect3);

	CleanupStack::PopAndDestroy(2, bitmapDevice);
	CleanupStack::Pop(bitmap);
	return bitmap;
	}

/** 
Stack cleanup helper function to use when the cache needs to be reset
from the cleanup stack.
This method calls MVgImageCache::ResetCache() on the passed MVgImageCache object
@param aPtr A pointer to an image cache object, MVgImageCache*, to be reset
*/
void CTDirectGdiStepBase::ResetCache(TAny* aPtr)
	{
	MVgImageCache* cache = reinterpret_cast <MVgImageCache*> (aPtr);
	cache->ResetCache();
	}

/** 
Stack cleanup helper function to use when the pen size needs to be reset to (1,1)
from the cleanup stack. This is needed when SetPenSize() is called when testing the
DirectGdi SW version as setting a pen size other then (1,1) causes memory to be
created that is not freed until SetPenSize(1,1) is called (or the related graphics
engine is destroyed).
This method calls CTContextBase::SetPenSize(1,1) on the passed CTContextBase object
@param aPtr A pointer to a test context object, CTContextBase*.
*/
void CTDirectGdiStepBase::ResetPenSize(TAny* aPtr)
	{
	if (CTContextBase* context = reinterpret_cast <CTContextBase*> (aPtr))
		context->SetPenSize(TSize(1,1));
	}
