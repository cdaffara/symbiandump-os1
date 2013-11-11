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

#include "tsgimagesw.h"
#include <graphics/sgimage_sw.h>

CTSgImageSw::CTSgImageSw()
	{
	INFO_PRINTF1(_L("Graphics resource component test - RSgImage Sw Implementation Tests.\r\n"));
	}

CTSgImageSw::~CTSgImageSw()
	{
	}

/** 
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTSgImageSw::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-RESOURCE-0041"));
	INFO_PRINTF1(_L("Getting supported pixel formats on software implementation.\r\n"));
	TestGetPixelFormatsSWL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0042"));
	INFO_PRINTF1(_L("Getting the interfaces implemented by CSgImageImpl class.\r\n"));
	TestGetImageInterfacesSWL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0043"));
	INFO_PRINTF1(_L("Getting the address information from the MSgImage_Sw interface.\r\n"));
	TestGetImageInterfaceAddressInfo();
	RecordTestResultL();
		
	return TestStepResult();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0041
@SYMTestCaseDesc		Gets supported pixel formats on software reference implementation.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMFssID				RSgImage::GetPixelFormats(const TSgImageInfo&, TUidPixelFormat*, TInt&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure all the available pixel formats are returned 
						in the array and the actual number of pixel formats in
						the input/output aCount parameter for the software
						reference implementation.
@SYMTestActions			Initialise the graphics resource component. Construct a
						TSgImageInfo object and Initialise the member variables.
						Construct a TInt array of size\n
						\t 1. NULL\n						
						\t 2. greater than the number of supported pixel formats\n
						\t 3. equal to the number of supported pixel formats\n
						\t 4. less than the number of supported pixel formats
@SYMTestExpectedResults	The function must return\n
						\t 1. KErrNone\n
						\t 2. KErrNone\n
						\t 3. KErrNone\n
						\t 4. KErrOverFlow\n
						The supported pixel formats are populated into the array
						and aCount stores the actual number of supported pixel formats.
						The returned pixel formats should cover the pixel formats
						in the image compatibility table that works on all platforms.
						All the returned pixel formats should appear on the image
						compatibility list for the software reference implementation.
@SYMTestStatus			Defined 
 */
void CTSgImageSw::TestGetPixelFormatsSWL()
	{
	TestOpenDriverL();
	//check all the pixel formats returned are in the common compatibility guarantees table
	TestGetPixelFormatCompatibilityGuaranteesL();
	
	//check the pixel formates supported by the reference implementation only
	CallGetPixelFormatsL(ESgCpuAccessNone, ESgUsageDirectGdiSource|ESgUsageCompositionSource|ESgUsageOpenGlesTexture2D|ESgUsageOpenVgImage|ESgUsageDirectGdiTarget, ETrue, KSgScreenIdMain);
	CheckPixelFormatPresent(EUidPixelFormatRGB_565);
	CheckPixelFormatPresent(EUidPixelFormatXRGB_8888);
	CheckPixelFormatPresent(EUidPixelFormatARGB_8888_PRE);
	
	TestCloseDriver();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0042
@SYMTestCaseDesc		Gets the interfaces implemented by CSgImageImpl class.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)\n 
						RSgImage::GetInterface()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the MSgImage_Sw interface can be returned from RSgImage.
@SYMTestActions			Initialise the graphics resource component. Create an image. Call
						GetInterface() to return the MSgImage_Sw interface.
@SYMTestExpectedResults	Both interfaces are returned successfully.
@SYMTestStatus			Defined 
 */
void CTSgImageSw::TestGetImageInterfacesSWL()
	{
	
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0043
@SYMTestCaseDesc		Gets the address information from the MSgImage_Sw interface.
@SYMPREQ				PREQ39
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)\n 
						RSgImage::Interface()\n 
						MSgImage_Sw::DataAddress()\n 
						MSgImage_Sw::DataStride()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the software solution of the RSgImage returns 
						the correct image address information.
@SYMTestActions			Initialise the graphics resource component. Create an image. Call
						GetInterface() to return the MSgImage_Sw interface. Call the DataAddress()
						and DataStride() functions on the interface. Then modify some random 
						byte within the data address of the image and read back to check if the
						byte has been modified.
@SYMTestExpectedResults	The byte should be successfully modified. 
@SYMTestStatus			Defined 
 */	
void CTSgImageSw::TestGetImageInterfaceAddressInfo()
	{
	
	}
