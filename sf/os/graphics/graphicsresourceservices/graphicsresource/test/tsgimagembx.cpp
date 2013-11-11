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

#include "tsgimagembx.h"

CTSgImageMbx::CTSgImageMbx()
	{
	INFO_PRINTF1(_L("Graphics resource component test - RSgImage MBX Tests.\r\n"));
	}

CTSgImageMbx::~CTSgImageMbx()
	{
	}

/** 
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTSgImageMbx::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-RESOURCE-0040"));
	INFO_PRINTF1(_L("Getting supported pixel formats on the MBX hardware.\r\n"));
	TestGetPixelFormatsMBXL();
	RecordTestResultL();
	

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0096
*/
	SetTestStepID(_L("GRAPHICS-RESOURCE-0096"));
	INFO_PRINTF1(_L("Getting the interfaces implemented on the MBX hardware.\r\n"));
	TestGetImageInterfacesMBXL();
	RecordTestResultL();
	
	return TestStepResult();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0040
@SYMTestCaseDesc		Gets supported pixel formats on the MBX hardware.
@SYMPREQ				PREQ39
@SYMREQ 				REQ8809
@SYMREQ 				REQ9175
@SYMFssID				RSgImage::GetPixelFormats(const TSgImageInfo&, TUidPixelFormat*, TInt&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure all the available pixel formats are returned 
						in the array and the actual number of pixel formats in
						the input/output aCount parameter on the MBX hardware.
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
						compatibility list for the MBX driver.
@SYMTestStatus			Defined 
 */
void CTSgImageMbx::TestGetPixelFormatsMBXL()
	{
	TestOpenDriverL();
	//check all the pixel formats returned are in the common compatibility guarantees table
	TestGetPixelFormatCompatibilityGuaranteesL();
	
	//check the pixel formates supported by the MBX hardware only
	CallGetPixelFormatsL(ESgCpuAccessNone, ESgUsageDirectGdiSource|ESgUsageCompositionSource|ESgUsageOpenGlesTexture2D|ESgUsageOpenVgImage|ESgUsageDirectGdiTarget, ETrue, KSgScreenIdMain);
	CheckPixelFormatPresent(EUidPixelFormatRGB_565);
	CheckPixelFormatPresent(EUidPixelFormatXRGB_8888);
	CheckPixelFormatPresent(EUidPixelFormatARGB_8888_PRE);
	
	TestCloseDriver();
	}

void CTSgImageMbx::TestGetImageInterfacesMBXL()
	{

	}
