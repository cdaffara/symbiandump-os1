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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "tdirectgditestbase.h"

#include <graphics/directgdidriver.h>
#include <graphics/directgdiextensioninterfaces.h>
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <graphics/sgimage_sw.h>
#endif
TSize GetDisplaySizeInPixels();

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
// Cache size in bytes for DirectGDI Image cache.
const TInt KDirectGdiImageCacheSize = 0x400000;
#endif

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE

/**
Implementation of CTestStep base class virtual.
Used to implement all DirectGDI-specific initialisation for any derived test-cases, and
reads settings from the configuration file.
It will leave if there are any failures as DirectGDI must be initialised correctly for
the tests to be run. The leave will be picked up by the framework.

@leave If initialisation of DirectGDI failed.
@return EPass if successful, otherwise a TVerdict error code.
*/
TVerdict CTDirectGdiTestBase::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	
	TPtrC targetPixelFormatInput;
	TEST(GetStringFromConfig(_L("PixelFormats"), _L("TargetPixelFormatEnums"), targetPixelFormatInput));
	TDisplayModeMapping::ConvertPixelFormats(targetPixelFormatInput, iTargetPixelFormatArray);

	TPtrC sourcePixelFormatInput;
	TEST(GetStringFromConfig(_L("PixelFormats"), _L("SourcePixelFormatEnums"), sourcePixelFormatInput));
	TDisplayModeMapping::ConvertPixelFormats(sourcePixelFormatInput, iSourcePixelFormatArray);
	
	InitializeDirectGdiL();	
	return TestStepResult();
	}

/**
Implementation of CTestStep base class virtual
Used for doing all post-test treatment common to derived DirectGDI classes, such as
shutting down DirectGDI.

@leave If there was a failure shutting down DirectGDI.
@return EPass if successful, otherwise a TVerdict error code.
*/
TVerdict CTDirectGdiTestBase::doTestStepPostambleL()
	{
	iTargetPixelFormatArray.Close();
	iSourcePixelFormatArray.Close();
	UninitializeDirectGdi();
	return CTe_graphicsperformanceSuiteStepBase::doTestStepPostambleL();
	}
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE

 CTDirectGdiTestBase::~CTDirectGdiTestBase()
	{
	}

CTDirectGdiTestBase::CTDirectGdiTestBase()
	{
	}

/**
Sets up DirectGDI for use in the test cases.

@leave If any failures occur initialising DirectGDI.
*/
void CTDirectGdiTestBase::InitializeDirectGdiL()
	{
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	SgDriver::Open();
		
	TESTNOERRORL(CDirectGdiDriver::Open());
	iDGdiDriver = CDirectGdiDriver::Static();
	if (iDGdiDriver == NULL)
		User::Leave(KErrNoMemory);
	
	// If using the DirectGDI OpenVG adapter, set a large image cache size to optimize performance.
	MDirectGdiDriverCacheSize* cacheInterface = NULL;
	if (KErrNone == iDGdiDriver->GetInterface(TUid::Uid(KDirectGdiDriverCacheSizeUid), (TAny*&)cacheInterface))
		{
		cacheInterface->SetMaxImageCacheSize(KDirectGdiImageCacheSize);
		}
	
	iDGdiImageTarget = new (ELeave) RDirectGdiImageTarget(*iDGdiDriver);
			
	// Set up most of RSgImage image info to be used for target
	iImageInfo.iSizeInPixels = GetPixmapSizeInPixels();
	iImageInfo.iUsage = ESgUsageDirectGdiTarget;
	
	iTs = CFbsTypefaceStore::NewL(NULL);	
	TESTNOERRORL(iTs->GetNearestFontToDesignHeightInPixels((CFont *&)iFont, TFontSpec(KFontTypeface, KFontSize)));
	TESTNOERRORL(iTs->GetNearestFontToDesignHeightInPixels((CFont *&)iLargeFont,TFontSpec(KFontTypeface, 40)));	
#endif
	}

/**
Shuts down DirectGDI when test cases have finished.
*/
void CTDirectGdiTestBase::UninitializeDirectGdi()
	{	
	delete iContext;
	iContext = NULL;
	if (iDGdiImageTarget)
		{
		iDGdiImageTarget->Close();	
		delete iDGdiImageTarget;
		iDGdiImageTarget = NULL;
		}
	if (iDGdiDriver)
		{
		iDGdiDriver->Close();
		}
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	iSgImageTarget.Close();
#endif
	iTs->ReleaseFont(iFont);
	iTs->ReleaseFont(iLargeFont);
	delete iTs;
	iTs = NULL;
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	SgDriver::Close();
#endif
	}

 /**
Initialises a target and a DirectGDI Gc.

@param aPixelFormat The pixel format to be used for the target.
@return ETrue if the target supports the display mode, EFalse otherwise.
*/
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
TBool CTDirectGdiTestBase::SetTargetPixelFormatL(TDisplayMode aDisplayMode)
	{
	TUidPixelFormat pixelFormat = TDisplayModeMapping::MapDisplayModeToPixelFormat(aDisplayMode);
	TInt numSupportedFormats = 0;
	RSgImage::GetPixelFormats(iImageInfo,NULL,numSupportedFormats);
	TUidPixelFormat* supportedFormats = new (ELeave) TUidPixelFormat[numSupportedFormats];
	RSgImage::GetPixelFormats(iImageInfo,supportedFormats,numSupportedFormats);
    TBool supported = EFalse;
	for (TInt ii = numSupportedFormats-1; ii >=0; --ii)
		{
		if(supportedFormats[ii] == pixelFormat)
			{
			supported = ETrue;
			break;
			}
		}
	delete[] supportedFormats;

	if (supported)
		{
		INFO_PRINTF2(_L("SetTargetPixelFormat - supported: %S"),&ColorModeName(aDisplayMode));
		}
	else
		{
		INFO_PRINTF2(_L("SetTargetPixelFormat - not supported: %S"),&ColorModeName(aDisplayMode));
		return EFalse;
		}

	delete iContext;
	iContext = NULL;
	TESTL(iDGdiImageTarget != NULL);
	iDGdiImageTarget->Close();
	iSgImageTarget.Close();
	
	iImageInfo.iPixelFormat = pixelFormat;
	TESTNOERRORL(iSgImageTarget.Create(iImageInfo, NULL,0));
	iContext = CDirectGdiContext::NewL (*iDGdiDriver);
	TESTL(iContext != NULL);
	TESTNOERRORL(iDGdiDriver->GetError());
	if (KErrNone == iDGdiImageTarget->Create(iSgImageTarget))
		{
		// Activate the context on a rendering target.
		TESTNOERRORL(iDGdiDriver->GetError());
		iContext->Activate(*iDGdiImageTarget);
		}
	TESTNOERRORL(iDGdiDriver->GetError());
	return supported;
	}
#else
TBool CTDirectGdiTestBase::SetTargetPixelFormatL(TDisplayMode /*aDisplayMode*/)
    {
    return EFalse;
    }
#endif

/* 
Creates a new CFbsBitmap representing the RSgImage. It is down to the client to delete it.

@see CTDirectGdiTestBase::GetTargetAsBitmapL()
@return A pointer to the newly created bitmap.
@leave If there was a problem copying from the source to the target.
*/
CFbsBitmap* CTDirectGdiTestBase::GetTargetAsBitmapL()
	{
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	iDGdiDriver->Finish();

	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	TESTNOERRORL(bitmap->Create(iImageInfo.iSizeInPixels, TDisplayModeMapping::MapPixelFormatToDisplayMode(iImageInfo.iPixelFormat)));
	
	const TInt bitmapDataStride = bitmap->DataStride();
	const TInt numScanlines = iImageInfo.iSizeInPixels.iHeight;	
	
	MSgImage_Sw* sgImageDirectAccess;
	TESTNOERRORL(iSgImageTarget.GetInterface(sgImageDirectAccess));	
	
    const TInt sgImageDataStride = sgImageDirectAccess->DataStride();    
    sgImageDirectAccess->BeginDataAccess(ESgCpuAccessReadOnly);
    TUint32* sgImageDataAddress = (TUint32*)(sgImageDirectAccess->DataAddress());    
    bitmap->BeginDataAccess();
    TUint32* bitmapDataAddress = bitmap->DataAddress();
    for (TInt scanline = 0; scanline < numScanlines; ++scanline)
    	{
    	Mem::Copy((TUint8*)bitmapDataAddress + (scanline*bitmapDataStride), (TUint8*)sgImageDataAddress + (scanline*sgImageDataStride), bitmapDataStride);
    	}
    bitmap->EndDataAccess();
    sgImageDirectAccess->EndDataAccess();
    
	CleanupStack::Pop(1, bitmap);
	return bitmap;
#else
	return NULL;
#endif	
	}
	

