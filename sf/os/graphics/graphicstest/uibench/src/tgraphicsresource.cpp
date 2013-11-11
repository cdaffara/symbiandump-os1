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

#include "tgraphicsresource.h"
#include "tdirectgditestbase.h"
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <graphics/sgimage_sw.h>
#endif

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
const TInt KIterationsToTest = 1000;
#endif

CTGraphicsResource::CTGraphicsResource()
	{
	SetTestStepName(KTGraphicsResource);
	
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	iImageInfo.iCpuAccess = ESgCpuAccessReadWrite;
	iImageInfo.iUsage = ESgUsageDirectGdiSource;
	iImageInfo.iPixelFormat = EUidPixelFormatRGB_565;
	iImageInfo.iShareable = ETrue;
	
	Mem::FillZ(iImageData, KMaxArraySize * sizeof(TUint16));
	TEST(KErrNone == SgDriver::Open());
#endif
	}

CTGraphicsResource::~CTGraphicsResource()
	{	
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	TInt temp = SgDriver::ResourceCount();
	SgDriver::Close();
#endif
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTGraphicsResource::doTestStepL()
	{
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    INFO_PRINTF1(_L("CTGraphicsResource can only be run with RSgImage legacy"));
    return TestStepResult();
#else
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0089"));
	SmallImageCreationSimpleL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0090"));
	LargeImageCreationSimpleL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0091"));
	ImageDuplicateL();	
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0092"));
	ImageDuplicateHandleL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0093"));
	ImageMapL();
	RecordTestResultL();
    CloseTMSGraphicsStep();
	return TestStepResult();
#endif	
	}

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
/**
Helper function.
Creates and destroys images of the same size KIterationsToTest times.

@param aWidth The width of the created bitmaps.
@param aHeight  The height of the created bitmaps.
@param aTestDescription The description of the test.
*/
void CTGraphicsResource::ImageCreationSimpleL(const TInt aWidth, const TInt aHeight, const TDesC& aTestDescription)
	{
	iImageInfo.iSizeInPixels = TSize(aWidth, aHeight);
	TInt sizeInBytes = aWidth*aHeight*2;
	RSgImage image;
	
	iProfiler->InitResults();	
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		TInt err = image.Create(iImageInfo, NULL, 0);
		TESTL(err == KErrNone);
		MSgImage_Sw* data;
		TESTNOERRORL(image.GetInterface(data));
		Mem::Fill(data->DataAddress(), sizeInBytes, 0xFF);
		image.Close();
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(aTestDescription, TDisplayModeMapping::MapPixelFormatToDisplayMode(iImageInfo.iPixelFormat), 0, 0, KIterationsToTest);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0089

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
The test determines how long it takes to create and destroy small simple RSgImage objects.

@SYMTestActions
Compare the results over time, and before and after changes to image construction and destruction code.

@SYMTestExpectedResults
Test should pass and display total test time and time per image
*/
void CTGraphicsResource::SmallImageCreationSimpleL()
	{
	ImageCreationSimpleL(32, 32, _L("Small-RSgImage-Create-64K-Simple"));
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0090

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
The test determines how long it takes to create and destroy large simple RSgImage objects.

@SYMTestActions
Compare the results over time, and before and after changes to image construction and destruction code.

@SYMTestExpectedResults
Test should pass and display total test time and time per image
*/
void CTGraphicsResource::LargeImageCreationSimpleL()
	{
	ImageCreationSimpleL(500, 500, _L("Large-RSgImage-Create-64K-Simple"));
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0091

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Tests how long it takes to duplicate an RSgImage.

@SYMTestActions
Compare the results over time, and before and after changes to bitmap duplication code.

@SYMTestExpectedResults
Test should pass and display total test time and time per image.
*/
void CTGraphicsResource::ImageDuplicateL()
	{
	//prepare an image to duplicate
	iImageInfo.iSizeInPixels = TSize(300, 300);
	
	RSgImage image;
	TInt err = image.Create(iImageInfo, iImageData, KImageDataStride);
	TESTL(err == KErrNone);
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		RSgImage image2;
		err = image2.Create(iImageInfo, image);
		TESTL(err == KErrNone);
		image2.Close();
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("RSgImage-Duplicate           "), TDisplayModeMapping::MapPixelFormatToDisplayMode(iImageInfo.iPixelFormat), 0, 0, KIterationsToTest);	
	
	image.Close();
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0092

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Tests how long it takes to duplicate an image handle.

@SYMTestActions
Compare the results over time, and before and after changes to image duplication code.

@SYMTestExpectedResults
Test should pass and display total test time and time per image.
*/
void CTGraphicsResource::ImageDuplicateHandleL()
	{
	//prepare an image to duplicate
	iImageInfo.iSizeInPixels = TSize(300, 300);
	
	RSgImage image;
	TInt err = image.Create(iImageInfo, iImageData, KImageDataStride);
	TESTL(err == KErrNone);
	TSgDrawableId id = image.Id();	
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		RSgImage image2;
		err = image2.Open(id);
		TESTL(KErrNone == err);
		image2.Close();
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("RSgImage-Duplicate-Handle"), TDisplayModeMapping::MapPixelFormatToDisplayMode(iImageInfo.iPixelFormat), 0, 0, KIterationsToTest);	
	
	image.Close();
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0093

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Measure performance of Map() and Unmap() of an RSgImage.

@SYMTestActions
Compare the results over time, and before and after changes to image mapping code.

@SYMTestExpectedResults
Test should pass and display total test time and time per image.
*/
void CTGraphicsResource::ImageMapL()
	{
	//prepare an image
	iImageInfo.iSizeInPixels = TSize(300, 300);
	
	RSgImage image;
	TInt err = image.Create(iImageInfo, iImageData, KImageDataStride);
	TESTL(err == KErrNone);
	
	const TAny* dataAddressRead;
	TAny* dataAddressWrite;
	TInt dataStride;
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		err = image.MapReadOnly(dataAddressRead, dataStride);
		TESTL(err == KErrNone);
		image.Unmap();
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("RSgImage-MapReadOnly          "), TDisplayModeMapping::MapPixelFormatToDisplayMode(iImageInfo.iPixelFormat), 0, 0, KIterationsToTest);	
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		err = image.MapWriteOnly(dataAddressWrite, dataStride);
		TESTL(err == KErrNone);
		image.Unmap();
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("RSgImage-MapWriteOnly          "), TDisplayModeMapping::MapPixelFormatToDisplayMode(iImageInfo.iPixelFormat), 0, 0, KIterationsToTest);
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		err = image.MapReadWrite(dataAddressWrite, dataStride);
		TESTL(err == KErrNone);
		image.Unmap();
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("RSgImage-MapReadWrite              "), TDisplayModeMapping::MapPixelFormatToDisplayMode(iImageInfo.iPixelFormat), 0, 0, KIterationsToTest);
	
	image.Close();
	}
#endif
