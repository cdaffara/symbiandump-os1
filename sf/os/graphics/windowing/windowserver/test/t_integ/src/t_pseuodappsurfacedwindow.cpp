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
 @internalComponent
*/
#include <e32const.h>	//KNullUidValue
#include <pixelformats.h>
#include "t_pseudoappsurfacedwindow.h"

#include "t_testsurfacerasterizer.h"
#include "t_pseudoapputils.h"
#include "t_pseudoappscreen.h" //KScreenWindow
#include "t_pseudoappcfbsbitmapanim.h"
#include "t_pseudoappvganimation.h"
#include "t_pseudoappopenglanimation.h"
#include "t_pseudoappperformanceanim.h"


CTestSurfacedWindow* CTestSurfacedWindow::NewL(TInt aScreenNo, TInt aWindowNumber, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults,
							const TDesC& aConfigFileName, CTPseudoAppShared& aPseudoAppShared)
	{
	RDebug::Print(_L("Creating CTestSurfacedWindow class\n"));
    CTestSurfacedWindow* self = new (ELeave) CTestSurfacedWindow(aScreenNo, aWindowNumber, aPseudoAppShared);
    CleanupStack::PushL(self);
    self->ConstructL(aScreenNo, aWindowNumber, aMode, aFrameDuration, aScreenSize, aGceTestResults, aConfigFileName);
    CleanupStack::Pop(); // self;
    return self;
	}

void CTestSurfacedWindow::ConstructL(TInt aScreenNo, TInt aWindowNumber, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults,
							const TDesC& aConfigFileName)
	{
	RDebug::Print(_L("Constructing CTestSurfacedWindow for screen %d\n"), aScreenNo);

	//Create CIniData class for reading in values form ini files
	iUtils = CIniData::NewL(aConfigFileName);

	TSize surfaceSize;
	TInt surfacePresent = 0;
	TPixelFormat pixelFormat  = EFormatRgb;

	RDebug::Print(_L("Import configuration for ui window class and create it\n"));
	ImportWindowConfigL(aScreenNo, aWindowNumber, aMode, aFrameDuration, aScreenSize, aGceTestResults);

	RDebug::Print(_L("Import configuration for test surface class and create it\n"));
	ImportSurfaceConfigL(aScreenNo, aWindowNumber, aScreenSize, surfaceSize, surfacePresent, pixelFormat);
	
	RDebug::Print(_L("Surface height = %d Surface Width = %d\n"),surfaceSize.iHeight, surfaceSize.iWidth);

	RDebug::Print(_L("Import configuration for test animation class and create it\n"));
	ImportAnimConfigL(aScreenNo, aWindowNumber, aMode, surfaceSize, pixelFormat);

	if(surfacePresent)
		{
		RDebug::Print(_L("Create Surface Rasterizer\n"));
		iSurfaceRasterizer = CTestSurfaceRasterizer::NewL(iSurfaceId, iSurfaceScreenNumber, &iSurfaceManager);

		if(iSurfaceRasterizer == 0)
			{
			RDebug::Print(_L("Fatal error creating Surface Rasterizer, aborting\n"));
			User::Exit(0);
			}
		}

	iGceTestResults = aGceTestResults;
	}

CTestSurfacedWindow::CTestSurfacedWindow(TInt aScreenNo, TInt aWindowNumber, CTPseudoAppShared& aPseudoAppShared)
:iScreenNo(aScreenNo), iWindowNumber(aWindowNumber), iPseudoAppShared(aPseudoAppShared)
{
}

void CTestSurfacedWindow::UpdateL(SurfaceDetails& aSurfDetails)
	{
	//Test to see if the surfaced window has an animation associated with it
	if(iTestAnimation)
		   {
		   iTestAnimation->DrawL(iSurfaceRasterizer, aSurfDetails);
		   }

	//Draw the window last to avoid appearance of "black" surface
	iTestWindow->DrawL();
	}

CTestSurfacedWindow::~CTestSurfacedWindow()
	{
	delete iUtils;
	if(iSizeArray)
		{
		TInt noElements = iSizeArray->Count();
		iSizeArray->Delete(0, noElements);
		delete iSizeArray;
		iSizeArray = 0;
		}

	if(iPositionArray)
		{
		TInt noElements = iPositionArray->Count();
		iPositionArray->Delete(0, noElements);
		delete iPositionArray;
		iPositionArray = 0;
		}

	if(iLoadsarects)
		{
		TInt noElements = iLoadsarects->Count();
		iLoadsarects->Delete(0, noElements);
		delete iLoadsarects;
		iLoadsarects = 0;
		}

	if(iLoadsaParameters)
		{
		TInt noElements = iLoadsaParameters->Count();
		iLoadsaParameters->Delete(0, noElements);
		delete iLoadsaParameters;
		iLoadsaParameters = 0;
		}

	delete iSurfaceRasterizer;
	delete iTestWindow;
	delete iTestAnimation;

	DestroySurface();
	DestroySurfaceManager();
	}

void CTestSurfacedWindow::RotateL(TInt aScreenNo, TInt aWindowNumber, const TSize& aScreenSize, TInt aFrameNumber)
	{
	if(iTestAnimation)
		{
		//Destroy the current surface and create a new surface with the new screen dimensions
		TSize surfaceSize;
		TInt surfacePresent = 0;
		TPixelFormat pixelFormat  = EFormatRgb;

		delete iSurfaceRasterizer;
		delete iTestAnimation;

		DestroySurface();

		ImportSurfaceConfigL(aScreenNo, aWindowNumber, aScreenSize, surfaceSize, surfacePresent, pixelFormat);

		RDebug::Print(_L("Surface height = %d Surface Width = %d\n"),surfaceSize.iHeight, surfaceSize.iWidth);

		RDebug::Print(_L("Import configuration for test animation class and create it\n"));
		ImportAnimConfigL(0, 1, EColor16MA, surfaceSize, pixelFormat);

		if(surfacePresent)
			{
			RDebug::Print(_L("Create Surface Rasterizer\n"));

			iSurfaceRasterizer = CTestSurfaceRasterizer::NewL(iSurfaceId, iSurfaceScreenNumber, &iSurfaceManager);

			if(iSurfaceRasterizer == 0)
				{
				RDebug::Print(_L("Fatal error creating Surface Rasterizer, aborting\n"));
				User::Exit(0);
				}
			}

		if(iTestAnimation->RotationSupported())
			{
			//Continue animation from the last unrotated frame number
			iTestAnimation->SetFrameNumber(aFrameNumber);
			}
		}

	iTestWindow->Rotate(aScreenSize);
	}

TBool CTestSurfacedWindow::RotationSupported()
	{
	if(iPseudoAppShared.RotationSupported(iScreenNo, iWindowNumber, iSurfaceId))
		{
		if(iTestAnimation)
			{
			return iTestAnimation->RotationSupported();
			}
		else
			{
			//If no test animation is present, rotation is possible
			return ETrue;
			}
		}
	else
		{
		return EFalse;
		}

	}
	
void CTestSurfacedWindow::ImportWindowConfigL(TInt aScreenNo, TInt aWindowNo, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults)
	{
	TBuf<KMaxUiBitmapNameLength>	tempStore;
	tempStore.Format(KScreenWindowNumber, aScreenNo, aWindowNo);

	TSize windowSize;
	TInt windowType;
	TInt windowChangeGeometry;
	TInt windowChangeBehavior;

	tempStore.Format(KScreenWindowNumber, aScreenNo, aWindowNo);

	READ_INI3A(tempStore,  type,  ui_window, ecom_window, empty_window,
	           windowType, EUiWindow, EEcomWindow, EEmptyWindow, iUtils);

	//viewfinder and empty windows are both catered for by the CTestEmptyWindow class
	switch(windowType)
		{
		case EEcomWindow:
			{
			TPtrC multiBitmapEcomFile;
			TInt multiBitmapEcomFileSize;

			READ_INI1A(tempStore, multi_bitmap_ecom_file, multiBitmapEcomFile, iUtils);
			READ_INI1A(tempStore, multi_bitmap_ecom_file_size, multiBitmapEcomFileSize, iUtils);

			READ_INI5A(tempStore, window_change_geometry, none, size, position, position_and_size,
			           position_and_size_with_opacity, windowChangeGeometry, ENoChanges, EWindowSize,
			           EWindowPosition, EWindowPositionAndSize, EWindowPositionAndSizeWithOpacity, iUtils);
			READ_INI3A(tempStore, window_change_behavior, none, incremental, cyclic, windowChangeBehavior,
			           ENoChange, EIncremental, ECyclic, iUtils);

		 	

			iSizeArray = ImportWindowSizeChangeParamsL(tempStore);
			iPositionArray = ImportWindowPositionChangeParamsL(tempStore);

			BuildDimensionArrayL(aScreenSize, aFrameDuration, windowChangeGeometry, windowChangeBehavior);

	 		iTestWindow = CTestEcomWindow::NewL(aScreenNo, aMode, multiBitmapEcomFile, multiBitmapEcomFileSize, aGceTestResults, iLoadsaParameters);
			break;
			}
		case EEmptyWindow:
	 		{
	 		iTestWindow = CTestEmptyWindow::NewL(aScreenNo, aMode, aScreenSize);
	 		break;
	 		}
	 	case EUiWindow:
	 		{
			TPtrC multiBitmapUiFile;
			TInt multiBitmapUiFileSize;
			TInt multiBitmapUiFileTransparency;

			READ_INI1A(tempStore, multi_bitmap_ui_file, multiBitmapUiFile, iUtils);
			READ_INI1A(tempStore, multi_bitmap_ui_file_size, multiBitmapUiFileSize, iUtils);
			READ_INI2A(tempStore, multi_bitmap_ui_file_transparency, True,  False,
	                   multiBitmapUiFileTransparency, ETrue, EFalse, iUtils);
			READ_INI5A(tempStore, window_change_geometry, none, size, position, position_and_size,
			           position_and_size_with_opacity, windowChangeGeometry, ENoChanges, EWindowSize,
			           EWindowPosition, EWindowPositionAndSize, EWindowPositionAndSizeWithOpacity, iUtils);
			READ_INI3A(tempStore, window_change_behavior, none, incremental, cyclic, windowChangeBehavior,
			           ENoChange, EIncremental, ECyclic, iUtils);


			iSizeArray = ImportWindowSizeChangeParamsL(tempStore);
			iPositionArray = ImportWindowPositionChangeParamsL(tempStore);

			BuildDimensionArrayL(aScreenSize, aFrameDuration, windowChangeGeometry, windowChangeBehavior);

			iLoadsarects = ImportWindowBitmapParamsL(tempStore, multiBitmapUiFileSize, aScreenSize);

			iTestWindow = CTestUiWindow::NewL(aScreenNo, aMode, multiBitmapUiFile, multiBitmapUiFileSize, iLoadsarects,
											  aFrameDuration,
											  multiBitmapUiFileTransparency,
											  aGceTestResults, iLoadsaParameters);
			break;
	 		}
	 	default:
	 		{
			break;	//Cannot reach default, Lint warning removal only
			}
		}
	}

void CTestSurfacedWindow::ImportSurfaceConfigL(TInt aScreenNo, TInt aWindowNo, const TSize& aScreenSize, TSize& aSurfaceSize, TInt& aSurfacePresent, TPixelFormat& aPixelFormat)
	{
	TBuf<KMaxUiBitmapNameLength>	tempStore;
	tempStore.Format(KScreenSurface, aScreenNo, aWindowNo);

	TInt shareScreenNumber;
	TInt shareWindowNumber;

	// Check to see if window is sharing surface with another window
	TBool shareSurface = TImportScreenConfig::GetShareSurfaceInfoL(aScreenNo, aWindowNo, shareScreenNumber, shareWindowNumber, iUtils);

	// Check to see if this window already has a SurfaceId allocated for it s
	iSurfaceId = iPseudoAppShared.GetSurfaceId(aScreenNo, aWindowNo);
	
	if(shareSurface && iSurfaceId.IsNull())
		{
		// SurfaceId not found, but check to see if the sharing window already has a SurfaceId allocated for it
		iSurfaceId = iPseudoAppShared.GetSurfaceId(shareScreenNumber, shareWindowNumber);
		}

	if(!iSurfaceId.IsNull())
		{
		// SurfaceId found - open surface to use with this window
		
		//Create a surface manager session
		CreateSurfaceManager();

		iSurfaceManager.OpenSurface(iSurfaceId);
		RSurfaceManager::TInfoBuf infoBuf;
		User::LeaveIfError(KErrNone == iSurfaceManager.SurfaceInfo(iSurfaceId, infoBuf));
		RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
		aSurfaceSize = info.iSize;
		if(info.iPixelFormat == EUidPixelFormatYUV_422Interleaved)
			{
			aPixelFormat =EFormatYuv;
			}
		aSurfacePresent = ETrue;
		shareSurface = ETrue;
		}
	else
		{
		// SurfaceId not found - create surface to use with this window

		READ_INI2A(tempStore, surface_present, True, False, aSurfacePresent, ETrue, EFalse, iUtils);
		READ_INI1A(tempStore, sneeky_surface_height, aSurfaceSize.iHeight, iUtils);
		READ_INI1A(tempStore, surface_width, aSurfaceSize.iWidth, iUtils);
	
		//Convert surface size percentages to pixel sizes
		if(aScreenSize.iWidth <= 0)
			{
			RDebug::Print(_L("surface_width is zero or less, quitting\n"));
			User::Exit(0);
			}
		else
			{
			aSurfaceSize.iWidth = aSurfaceSize.iWidth*aScreenSize.iWidth/100;
			}
	
		if(aScreenSize.iHeight <= 0)
			{
			RDebug::Print(_L("surface_height is zero or less, quitting\n"));
			User::Exit(0);
			}
		else
			{
			aSurfaceSize.iHeight = aSurfaceSize.iHeight*aScreenSize.iHeight/100;
			}

		if(!aSurfacePresent && !shareSurface)
			{
			RDebug::Print(_L("Screen %d, Window %d has no backgroung surface\n"),aScreenNo, aWindowNo);
			return;
			}

		RDebug::Print(_L("Setting up surface attributes\n"));
		TInt bytesPerPixel;
		TInt pixelFormat;
		RSurfaceManager::TSurfaceCreationAttributesBuf attribBuf;
		RSurfaceManager::TSurfaceCreationAttributes& buffer = attribBuf();
	    buffer.iMappable = ETrue;

		READ_INI4A(tempStore, surface_pixel_format, EUidPixelFormatARGB_8888, EUidPixelFormatRGB_565, EUidPixelFormatARGB_8888_PRE,
		           EUidPixelFormatYUV_422Interleaved, pixelFormat, EUidPixelFormatARGB_8888, EUidPixelFormatRGB_565, EUidPixelFormatARGB_8888_PRE,
		           EUidPixelFormatYUV_422Interleaved, iUtils);
		READ_INI1A(tempStore, surface_bytes_per_pixel, bytesPerPixel, iUtils);
		READ_INI1A(tempStore, surface_offset_to_first_buffer, buffer.iOffsetToFirstBuffer, iUtils);
		READ_INI1A(tempStore, surface_alignment, buffer.iAlignment, iUtils);
		READ_INI2A(tempStore, surface_contiguous, True, False, buffer.iContiguous, ETrue, EFalse, iUtils);
		READ_INI1A(tempStore, surface_buffer_number, buffer.iBuffers, iUtils);
	
		buffer.iPixelFormat = static_cast<TUidPixelFormat>(pixelFormat);
		iSurfaceBufferNumber = buffer.iBuffers;
		iSurfaceScreenNumber = aScreenNo;
		if(buffer.iPixelFormat == EUidPixelFormatYUV_422Interleaved)
			{
			aPixelFormat =EFormatYuv;
			}
	
		//Assign size attribute to surface size calculated above
		buffer.iSize = aSurfaceSize;
	
		//Calculate required stride
		buffer.iStride = buffer.iSize.iWidth * bytesPerPixel;
		RDebug::Print(_L("Stride is %d\n"),buffer.iStride);

		//Create a surface manager session
		CreateSurfaceManager();

		RDebug::Print(_L("Call to CreateSurface()\n"));
		TInt err = iSurfaceManager.CreateSurface(attribBuf, iSurfaceId);
		if (err != KErrNone)
			{
			RDebug::Print(_L("Error creating surface, aborting\n"));
			User::Exit(0);
			}
		}

	//Assign the created surface to be the window background - surface needs to be open to do this ?????
	//Window must be activated at this point for the correct screen mode to be applied
	RDebug::Print(_L("Set Background Surface on the window\n"));

	TInt ret = iTestWindow->Window()->SetBackgroundSurface(iSurfaceId);
	if(ret != KErrNone)
		{
		RDebug::Print(_L("Error associating surface with window, SetBackgroundSurface returns %d aborting\n"), ret);
	 	User::Exit(0);
	 	}

	// Update PseudoAppShared SurfaceId array
	iPseudoAppShared.SetSurfaceIdL(aScreenNo, aWindowNo, iSurfaceId);
	if(shareSurface)
		{
		iPseudoAppShared.SetSurfaceIdL(shareScreenNumber, shareWindowNumber, iSurfaceId);
		}
	}

void CTestSurfacedWindow::ImportAnimConfigL(TInt aScreenNo, TInt aWindowNo, TDisplayMode aMode, const TSize& aSurfaceSize, TPixelFormat aPixelFormat)
	{
	TInt applicationType;
	TBuf<KMaxUiBitmapNameLength>	tempStore;
	tempStore.Format(KScreenAnimation, aScreenNo, aWindowNo);

	READ_INI5A(tempStore, application_type, none,   cfbsbitmap, openvg,  opengles,  performance,
               applicationType,  ENoApp, EFbsBitmap, EOpenVG, EOpenGLes, EPerformance, iUtils);

	if(applicationType == ENoApp)
		{
		RDebug::Print(_L("Screen %d, Window %d has no surface animation\n"),aScreenNo, aWindowNo);
		return;
		}

	TInt imageSize;
	TPtrC multiBitmapAnimFile;
	TInt verticalRate;
	TInt horizontalRate;

	//Read in the values from the ini file, taking specified action on failure
	READ_INI1A(tempStore, image_size, imageSize, iUtils);
	READ_INI1A(tempStore, multi_bitmap_anim_file, multiBitmapAnimFile, iUtils);
	READ_INI1A(tempStore, vertical_frames_per_traverse, verticalRate, iUtils);
	READ_INI1A(tempStore, horizontal_frames_per_traverse, horizontalRate, iUtils);

	//Ensure the animation step sizes are sub-multiples of the frame dimensions
	TImportScreenConfig::VerifyFramesPerTraverse(horizontalRate, aSurfaceSize.iWidth);
	TImportScreenConfig::VerifyFramesPerTraverse(verticalRate, aSurfaceSize.iHeight);

	//Convert image size percentage into a pixel size
	if(imageSize <= 0)
		{
		RDebug::Print(_L("image_size is zero or less, quitting\n"));
		User::Exit(0);
		}
	else
		{
		//Scale the bitmap to the short screen dimension
		if(aSurfaceSize.iWidth > aSurfaceSize.iHeight)
			{
			imageSize = imageSize*aSurfaceSize.iHeight/100;
			}
		else
			{
			imageSize = imageSize*aSurfaceSize.iWidth/100;
			}
		}

	iTestAnimation = CreateAnimationL(applicationType, TSize(imageSize,imageSize),
		                              aSurfaceSize, multiBitmapAnimFile, aMode,
		                              horizontalRate, verticalRate, aPixelFormat);
	}


CTestAnimation*  CTestSurfacedWindow::CreateAnimationL(TInt aAppType, const TSize& aAnimSize, const TSize& aSurfaceSize,
                                                       TPtrC& aBitmapFile, TDisplayMode aDisplayMode,
                                                       TInt aHorizontalRate, TInt aVerticalRate,
                                                       TPixelFormat aPixelFormat)
	{
	CTestAnimation* anim = 0;
	switch(aAppType)
		{
		case EFbsBitmap:
			{
			anim = CCFbsBitmapAnimation::NewL(aDisplayMode,
			                                  aAnimSize,
			                                  aSurfaceSize,
			                                  aBitmapFile,
			                                  aHorizontalRate,
			                                  aVerticalRate,
			                                  aPixelFormat);
			break;
			}

		case EOpenVG:
			{
			anim = CVgAnimation::NewL(iTestWindow->Window(),
									  aDisplayMode,
			                          aSurfaceSize,
			                          aHorizontalRate,
			                          aVerticalRate);
			break;
			}
		case EOpenGLes:
			{
			anim = COpenGLAnimation::NewL(iTestWindow->Window(),
										  aDisplayMode,
				                          aSurfaceSize,
				                          aHorizontalRate,
				                          aVerticalRate);
			break;
			}

		case EPerformance:
			{
			anim = CPerformanceAnimation::NewL();
			break;
			}
		case ENoApp:
			{
			break;
			}
		default:
			{
			break;	//Cannot reach the default, Lint warning removal purposes only
			}
		}
	return anim;
	}

CArrayFixFlat<TRect>* CTestSurfacedWindow::ImportWindowBitmapParamsL(const TDesC& tempStore, TInt aMultiBitmapUiFileSize, const TSize& aScreenSize)
	{
	TBuf<KMaxUiBitmapNameLength>	temp;
	TBuf<KMaxUiBitmapNameLength>	tempStore1;

	CArrayFixFlat<TRect>* loadsarects = new (ELeave) CArrayFixFlat<TRect>(1);

	for(TInt i=0; i<aMultiBitmapUiFileSize; i++)
		{
		TPtrC rectString;
		TRect aRect;

		tempStore1.Format(KUiBitmapScaling, i);
		temp = tempStore;
		temp.Append(tempStore1);

		TImportScreenConfig::ReadIni(temp, rectString, iUtils);

		if(TImportScreenConfig::ExtractRect(rectString, aRect, aScreenSize) == KErrNone)
			{
			loadsarects->ExtendL() = aRect;
			}
		else
			{
			RDebug::Print(_L("Cannot determine rectangle dimensions from string: %s\n"), rectString.Ptr());
			RDebug::Print(_L("Undefined behavior may result\n"));
			}
		}
	return loadsarects;
	}

CArrayFixFlat<TInt>* CTestSurfacedWindow::ImportWindowSizeChangeParamsL(const TDesC& tempStore)
	{
	TBuf<KMaxUiBitmapNameLength> temp = tempStore;
	CArrayFixFlat<TInt>* intArray = new (ELeave) CArrayFixFlat<TInt>(1);

	TInt windowSizeValues;
	TBuf<KMaxUiBitmapNameLength>	tempStore1;

	READ_INI1A(tempStore, window_size_values, windowSizeValues, iUtils);

	TInt windowSize;
	for(TInt i=1; i<windowSizeValues+1; i++)
		{
		tempStore1.Format(KWindowSizes, i);
		temp = tempStore;
		temp.Append(tempStore1);
		TImportScreenConfig::ReadIni(temp, windowSize, iUtils);
		intArray->ExtendL() = windowSize;
		}

	return intArray;
	}

CArrayFixFlat<TInt>* CTestSurfacedWindow::ImportWindowPositionChangeParamsL(const TDesC& tempStore)
	{
	TBuf<KMaxUiBitmapNameLength> temp = tempStore;
	CArrayFixFlat<TInt>* intArray = new (ELeave) CArrayFixFlat<TInt>(1);

	TInt windowPositionValues;
	TBuf<KMaxUiBitmapNameLength>	tempStore1;

	READ_INI1A(tempStore, window_position_values, windowPositionValues, iUtils);

	for(TInt i=1; i<windowPositionValues+1; i++)
		{
		TPoint point;
		TPtrC windowPosition;
		tempStore1.Format(KWindowPositions, i);
		temp = tempStore;
		temp.Append(tempStore1);
		TImportScreenConfig::ReadIni(temp, windowPosition, iUtils);
		if(TImportScreenConfig::ExtractPoint(windowPosition, point) == KErrNone)
			{

			intArray->ExtendL() = point.iX;
			intArray->ExtendL() = point.iY;
			}
		else
			{
			RDebug::Print(_L("Cannot determine point dimensions from string: %s\n"), windowPosition.Ptr());
			RDebug::Print(_L("Undefined behavior may result\n"));
			}
		}
	return intArray;
	}

void CTestSurfacedWindow::CreateSurfaceManager()
	{
	RDebug::Print(_L("Create Surface Manager\n"));

	TInt ret;

	RDebug::Print(_L("Loading the device driver and Opening the Logical Channel\n"));
	ret = iSurfaceManager.Open();
	if(ret==KErrNone)
		{
		RDebug::Print(_L("Surface manager opened ok\n"));
		}
	else
		{
		RDebug::Print(_L("Error opening surface manager\n"));
		}
	}

void CTestSurfacedWindow::DestroySurfaceManager()
	{
	RDebug::Print(_L("Close the Logical Channel\n"));
	iSurfaceManager.Close();
	}

void CTestSurfacedWindow::DestroySurface()
	{
	// Remove SurfaceId entry from surface array
	iPseudoAppShared.RemoveSurfaceIdEntry(iScreenNo, iWindowNumber);

	RDebug::Print(_L("Closing surface\n"));
	TInt ret = iSurfaceManager.CloseSurface(iSurfaceId);

	if(ret!=KErrNone)
		{
		RDebug::Print(_L("Surface failed to close\n"));
		}
	}

void CTestSurfacedWindow::BuildDimensionArrayL(const TSize& aScreenSize, TInt aFrameDuration, TInt aWindowChangeGeometry, TInt aWindowChangeBehavior)
	{
	TInt indexX;
	TInt indexY;

	iLoadsaParameters = new (ELeave) CArrayFixFlat<TDrawParameters>(1);

	TDrawParameters drawParams;

	for(TInt i=0; i<aFrameDuration; i++)
		{
		if(i)
			{
			drawParams = (*iLoadsaParameters)[i - 1];
			}
		else
			{
			drawParams.iWindowSize = TSize((aScreenSize.iWidth * (*iSizeArray)[0])/100,
			                         	   (aScreenSize.iHeight * (*iSizeArray)[0])/100);
			drawParams.iWindowPos = TPoint((aScreenSize.iWidth * (*iPositionArray)[0])/100,
			                         	   (aScreenSize.iHeight * (*iPositionArray)[1])/100);
			drawParams.iBitmapScale = (*iSizeArray)[0];
			drawParams.iBitmapAlpha = 127;
			}

		switch(aWindowChangeBehavior)
			{
			case ECyclic:
				{
				switch(aWindowChangeGeometry)
					{
					case ENoChanges:
						{
						drawParams.iWindowSize = aScreenSize;
						break;
						}
					case EWindowSize:
						{
						if(i < aFrameDuration/2)
							{
							drawParams.iBitmapScale = (*iSizeArray)[0] + (2*((*iSizeArray)[1] - (*iSizeArray)[0])*i)/aFrameDuration;
							}
						else
							{
							drawParams.iBitmapScale = 2*((*iSizeArray)[1]) - (*iSizeArray)[0] + (2*((*iSizeArray)[0] - (*iSizeArray)[1])*i)/aFrameDuration;
							}

						drawParams.iWindowSize = TSize( (aScreenSize.iWidth*drawParams.iBitmapScale)/100,
														(aScreenSize.iHeight*drawParams.iBitmapScale)/100 );

						break;
						}
					case EWindowPosition:
						{
						ParamChangeCyclicPosition(drawParams, i, aFrameDuration, aScreenSize);
						break;
						}
					case EWindowPositionAndSize:
						{
						ParamChangeCyclicPositionAndSize(drawParams, i, aFrameDuration, aScreenSize);
						break;
						}
					default:
						{
						break;
						}
					}
				break;
				}
			case EIncremental:
				{
				switch(aWindowChangeGeometry)
					{
					case ENoChanges:
						{
						break;
						}
					case EWindowSize:
						{
						drawParams.iBitmapScale = (*iSizeArray)[ (i*(iSizeArray->Count()))/aFrameDuration];
						drawParams.iWindowSize = TSize( (aScreenSize.iWidth*drawParams.iBitmapScale)/100,
														(aScreenSize.iHeight*drawParams.iBitmapScale)/100 );
						break;
						}
					case EWindowPosition:
						{
						indexX = 2*((((i*(iPositionArray->Count()/2))))/aFrameDuration);
						indexY = 2*((((i*(iPositionArray->Count()/2))))/aFrameDuration) + 1;
						drawParams.iWindowPos.iX = (aScreenSize.iWidth * (*iPositionArray)[indexX])/100;
						drawParams.iWindowPos.iY = (aScreenSize.iHeight * (*iPositionArray)[indexY])/100;
						break;
						}
					case EWindowPositionAndSize:
						{
						drawParams.iBitmapScale = (*iSizeArray)[ (i*(iSizeArray->Count()))/aFrameDuration];
						drawParams.iWindowSize = TSize( (aScreenSize.iWidth*drawParams.iBitmapScale)/100,
														(aScreenSize.iHeight*drawParams.iBitmapScale)/100 );

						indexX = 2*((((i*(iPositionArray->Count()/2))))/aFrameDuration);
						indexY = 2*((((i*(iPositionArray->Count()/2))))/aFrameDuration) + 1;
						drawParams.iWindowPos.iX = (aScreenSize.iWidth * (*iPositionArray)[indexX])/100;
						drawParams.iWindowPos.iY = (aScreenSize.iHeight * (*iPositionArray)[indexY])/100;
						break;
						}
					case EWindowPositionAndSizeWithOpacity:
						{
						ParamChangeOpacity(i, aFrameDuration, drawParams);
						drawParams.iBitmapScale = (*iSizeArray)[ (i*(iSizeArray->Count()))/aFrameDuration];
						drawParams.iWindowSize = TSize( (aScreenSize.iWidth*drawParams.iBitmapScale)/100,
														(aScreenSize.iHeight*drawParams.iBitmapScale)/100 );

						indexX = 2*((((i*(iPositionArray->Count()/2))))/aFrameDuration);
						indexY = 2*((((i*(iPositionArray->Count()/2))))/aFrameDuration) + 1;
						drawParams.iWindowPos.iX = (aScreenSize.iWidth * (*iPositionArray)[indexX])/100;
						drawParams.iWindowPos.iY = (aScreenSize.iHeight * (*iPositionArray)[indexY])/100;
						break;
						}
					default:
						{
						break;
						}
					}
				break;
				}

			case ENoChange:
			default:
				{
				break;
				}
			}

		iLoadsaParameters->ExtendL() = drawParams;
		}
	}

void CTestSurfacedWindow::ParamChangeCyclicPosition(TDrawParameters& aDrawParams, TInt aFrameCounter, TInt aFrameDuration, const TSize& aScreenSize)
	{
	TInt increment = (aScreenSize.iHeight * (100 - (*iSizeArray)[0]) +
	                  aScreenSize.iWidth  * (100 - (*iSizeArray)[0]))/(aFrameDuration * 25);

	if(aFrameCounter < (aFrameDuration/2 - 1))
		{
		CalcNewWindowLocation(aScreenSize, increment, 1, aDrawParams);
		}
	else if(aFrameCounter != (aFrameDuration - 1))
		{
		CalcNewWindowLocation(aScreenSize, increment, -1, aDrawParams);
		}
	else
		{
		aDrawParams.iWindowPos.iY -= increment;
		}
	}

void CTestSurfacedWindow::ParamChangeCyclicPositionAndSize(TDrawParameters& aDrawParams, TInt aFrameCounter, TInt aFrameDuration, const TSize& aScreenSize)
	{
	TInt incrementX = 2*aScreenSize.iWidth/aFrameDuration;
	TInt incrementY = 2*aScreenSize.iHeight/aFrameDuration;
	TSize incrementSize(incrementX, incrementY);
	TInt eigth = aFrameDuration/8;

	if(aFrameCounter < eigth)
		{
		aDrawParams.iWindowSize -= incrementSize;
		aDrawParams.iWindowPos.iY += incrementY;
		aDrawParams.iWindowPos.iX = 0;
		}
	else if( (aFrameCounter >= eigth) && (aFrameCounter < 2*eigth) )
		{
		aDrawParams.iWindowSize -= incrementSize;
		aDrawParams.iWindowPos.iY += incrementY;
		aDrawParams.iWindowPos.iX += 2*incrementX;
		}
	else if( (aFrameCounter >= 2*eigth) && (aFrameCounter < 3*eigth) )
		{
		aDrawParams.iWindowSize -= incrementSize;
		aDrawParams.iWindowPos.iY -= 2*incrementY;
		aDrawParams.iWindowPos.iX += incrementX;
		}
	else if( (aFrameCounter >= 3*eigth) && (aFrameCounter < 4*eigth) )
		{
		aDrawParams.iWindowSize -= incrementSize;
		aDrawParams.iWindowPos.iY = 0;
		aDrawParams.iWindowPos.iX -= 3*incrementX;
		}
	else if( (aFrameCounter >= 4*eigth) && (aFrameCounter < 5*eigth) )
		{
		aDrawParams.iWindowSize += incrementSize;
		aDrawParams.iWindowPos.iY = 0;
		aDrawParams.iWindowPos.iX += 3*incrementX;
		}
	else if( (aFrameCounter >= 5*eigth) && (aFrameCounter < 6*eigth) )
		{
		aDrawParams.iWindowSize += incrementSize;
		aDrawParams.iWindowPos.iY += 2*incrementY;
		aDrawParams.iWindowPos.iX -= incrementX;
		}
	else if( (aFrameCounter >= 6*eigth) && (aFrameCounter < 7*eigth) )
		{
		aDrawParams.iWindowSize += incrementSize;
		aDrawParams.iWindowPos.iY -= incrementY;
		aDrawParams.iWindowPos.iX -= 2*incrementX;
		}
	else
		{
		aDrawParams.iWindowSize += incrementSize;
		aDrawParams.iWindowPos.iY -= incrementY;
		aDrawParams.iWindowPos.iX = 0;
		}
	}

void CTestSurfacedWindow::ParamChangeOpacity(TInt aFrameCounter, TInt aFrameDuration, TDrawParameters& aDrawParams)
	{
	TInt alpha = (512 * aFrameCounter)/(aFrameDuration);
	if(alpha > 255)
		{
		alpha = 511 - alpha;
		}

	aDrawParams.iBitmapAlpha = alpha;
	}

void CTestSurfacedWindow::CalcNewWindowLocation(const TSize& aScreensize, TInt aIncrement, TInt aDirection,TDrawParameters& aDrawParams)
	{
	if(aDrawParams.iWindowPos.iX <= 0)
		{
		aDrawParams.iWindowPos.iX = 0;
		aDrawParams.iWindowPos.iY += aIncrement*aDirection;
		}
	if((aDrawParams.iWindowPos.iY + aDrawParams.iWindowSize.iHeight) >= aScreensize.iHeight)
		{
		aDrawParams.iWindowPos.iY = aScreensize.iHeight - aDrawParams.iWindowSize.iHeight;
		aDrawParams.iWindowPos.iX += aIncrement*aDirection;
		}
	if((aDrawParams.iWindowPos.iX + aDrawParams.iWindowSize.iWidth) >= aScreensize.iWidth)
		{
		aDrawParams.iWindowPos.iX = aScreensize.iWidth - aDrawParams.iWindowSize.iWidth;
		aDrawParams.iWindowPos.iY -= aIncrement*aDirection;
		}
	if(aDrawParams.iWindowPos.iY <= 0)
		{
		aDrawParams.iWindowPos.iY = 0;
		aDrawParams.iWindowPos.iX -= aIncrement*aDirection;
		}
	}
