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

#include "tsgimagegeneric.h"
#include <hal.h>
#include <e32math.h>

CTSgImageGeneric::CTSgImageGeneric()
	{
	INFO_PRINTF1(_L("Graphics resource component test - RSgImage Generic Tests.\r\n"));
	}

CTSgImageGeneric::~CTSgImageGeneric()
	{
	TInt count = iTestImages.Count();
	for(TInt i=0; i<count; ++i)
		{
		iTestImages[i].Close();
		}	
	iTestImages.Reset();
	}

/** 
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTSgImageGeneric::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-RESOURCE-0003"));
	INFO_PRINTF1(_L("Getting pixel formats supported on all platforms.\r\n"));
	TestGetPixelFormatsL();
	RecordTestResultL();	

	SetTestStepID(_L("GRAPHICS-RESOURCE-0004"));
	INFO_PRINTF1(_L("Create an uninitialised image.\r\n"));
	TestCreateImageUninitializedL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0005"));
	INFO_PRINTF1(_L("Create an image from a block of memory.\r\n"));
	TestCreateImageL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0006"));
	INFO_PRINTF1(_L("Create an image from an existing image.\r\n"));
	TestCreateImageFromExistingImageL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0007"));
	INFO_PRINTF1(_L("Getting information about the image.\r\n"));
	TestGetImageInfoL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0008"));
	INFO_PRINTF1(_L("Retrieving drawable id of an image.\r\n"));
	TestGetImageDrawableIdL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0009"));
	INFO_PRINTF1(_L("Mapping and unmapping an image from its creator process.\r\n"));
	TestMapImageL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0010"));
	INFO_PRINTF1(_L("Opening a handle to an image in both the current process and another process.\r\n"));
	TestOpenImageL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0011"));
	INFO_PRINTF1(_L("Opening a handle to an image with an open mode.\r\n"));
	TestOpenImageInternalL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0044"));
	INFO_PRINTF1(_L("Getting the available interfaces from an image.\r\n"));	
	TestGetInterfaceL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0024"));
	INFO_PRINTF1(_L("Getting supported pixel formats with invalid parameters.\r\n"));
	TestGetPixelFormatsInvalidL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0025"));
	INFO_PRINTF1(_L("Opening an image with different invalid parameters.\r\n"));
	TestOpenImageInvalidL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0026"));
	INFO_PRINTF1(_L("Closing the handle to an image object multiple times.\r\n"));
	TestCloseImageManyTimesL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0027"));
	INFO_PRINTF1(_L("Closing the handle to an image object without opening it.\r\n"));
	TestCloseImageWithoutOpenL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0028"));
	INFO_PRINTF1(_L("Creating an image with various invalid parameters.\r\n"));
	TestCreateImageInvalidL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0039"));
	INFO_PRINTF1(_L("Getting information about an invalid image.\r\n"));
	TestGetInfoInvalidImageL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0030"));
	INFO_PRINTF1(_L("Mapping an image under various invalid conditions. \r\n"));
	TestMapImageInvalidL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0031"));
	INFO_PRINTF1(_L("Unmapping an image under various invalid conditions.\r\n"));
	TestUnmapImageInvalidL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0048"));
	INFO_PRINTF1(_L("RSgImage generic stress test.\r\n"));
	TestStress1L();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0049"));
	INFO_PRINTF1(_L("RSgImage generic multi-thread stress test.\r\n"));
	TestStress2L();
	RecordTestResultL();

#ifdef _DEBUG
/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0095
*/
	SetTestStepID(_L("GRAPHICS-RESOURCE-0095"));
	INFO_PRINTF1(_L("RSgImage generic Out of memory test.\r\n"));
	TestOOM();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0052"));
	INFO_PRINTF1(_L("RSgImage generic panic test - GetInterface() with invalid image handle\r\n"));
	TestPanicImageGetInterfaceInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0052"));
	INFO_PRINTF1(_L("RSgImage generic panic test - GetInterface() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageGetInterfaceNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0053"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Close() with invalid image handle\r\n"));
	TestPanicImageCloseInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0054"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Close() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageCloseNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0055"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Id() with invalid image handle\r\n"));
	TestPanicImageIdInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0056"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Id() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageIdNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0057"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Create() with invalid image handle\r\n"));
	TestPanicImageCreateInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0058"));
	INFO_PRINTF1(_L("RSgImage generic panic test - GetInfo() with invalid image handle\r\n"));
	TestPanicImageGetInfoInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0059"));
	INFO_PRINTF1(_L("RSgImage generic panic test - GetInfo() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageGetInfoNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0060"));
	INFO_PRINTF1(_L("RSgImage generic panic test - MapReadOnly() with invalid image handle\r\n"));
	TestPanicImageMapReadOnlyInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0061"));
	INFO_PRINTF1(_L("RSgImage generic panic test - MapReadOnly() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageMapReadOnlyNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0062"));
	INFO_PRINTF1(_L("RSgImage generic panic test - MapWriteOnly() with invalid image handle\r\n"));
	TestPanicImageMapWriteOnlyInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0063"));
	INFO_PRINTF1(_L("RSgImage generic panic test - MapWriteOnly() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageMapWriteOnlyNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0064"));
	INFO_PRINTF1(_L("RSgImage generic panic test - MapReadWrite() with invalid image handle\r\n"));
	TestPanicImageMapReadWriteInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0065"));
	INFO_PRINTF1(_L("RSgImage generic panic test - MapReadWrite() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageMapReadWriteNoDriverL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0066"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Unmap() with invalid image handle\r\n"));
	TestPanicImageUnmapInvalidHandleL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0067"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Unmap() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageUnmapNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0082"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Open() with an uninitialised driver\r\n"));
	TestPanicImageOpenNoDriver1L();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0093"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Open() with a mode flag with an uninitialised driver\r\n"));
	TestPanicImageOpenNoDriver2L();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0083"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Create() from memory with an uninitialised driver\r\n"));
	TestPanicImageCreateNoDriver1L();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0084"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Create() from existing image with an uninitialised driver\r\n"));
	TestPanicImageCreateNoDriver2L();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0085"));
	INFO_PRINTF1(_L("RSgImage generic panic test - GetPixelFormats() with an uninitialised driver\r\n"));	
	TestPanicImageGetPixelFormatsNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0088"));
	INFO_PRINTF1(_L("RSgImage generic panic test - DrawableType() with an uninitialised driver\r\n"));	
	TestPanicImageDrawableTypeNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0089"));
	INFO_PRINTF1(_L("RSgImage generic panic test - DrawableType() with invalid image handle\r\n"));	
	TestPanicImageDrawableTypeInvalidHandleL();
	RecordTestResultL();
#else
	INFO_PRINTF1(_L("Warning: Skipping the panic tests in release build. \r\n"));
#endif	
	return TestStepResult();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0003
@SYMTestCaseDesc		Gets supported pixel formats on all platforms.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::GetPixelFormats(const TSgImageInfo&, TUidPixelFormat*, TInt&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure all the available pixel formats are returned 
						in the array and the actual number of pixel formats in
						the input/output aCount parameter.
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
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestGetPixelFormatsL()
	{
	TestOpenDriverL();
	TSgImageInfo info;
	info.iSizeInPixels = TSize(10, 10);
	info.iCpuAccess = ESgCpuAccessNone;
	info.iUsage = ESgUsageDirectGdiSource;
	info.iShareable = EFalse;
	info.iScreenId = 0;
		
	TInt numPixelFormats = 0;
	//NULL array
	CheckErrorL(KErrNone, RSgImage::GetPixelFormats(info, NULL, numPixelFormats), (TText8*)__FILE__, __LINE__);
	TEST(numPixelFormats > 1);
	
	//array length bigger than available pixel formats
	TInt count = numPixelFormats + 10;
	TUidPixelFormat* pixelFormatArray = new(ELeave) TUidPixelFormat[count];
	CleanupArrayDeletePushL(pixelFormatArray);
	CheckErrorL(KErrNone, RSgImage::GetPixelFormats(info, pixelFormatArray, count), (TText8*)__FILE__, __LINE__);
	TEST(count == numPixelFormats);
	
	info.iScreenId = -1;
	CheckErrorL(KErrNone, RSgImage::GetPixelFormats(info, pixelFormatArray, count), (TText8*)__FILE__, __LINE__);
	TEST(count == numPixelFormats);
	
	info.iScreenId = 0;	
	CleanupStack::PopAndDestroy(pixelFormatArray);
	//array length is equal to the supported pixel formats
	count = numPixelFormats;
	pixelFormatArray = new(ELeave) TUidPixelFormat[count];
	CleanupArrayDeletePushL(pixelFormatArray);
	
	CheckErrorL(KErrNone, RSgImage::GetPixelFormats(info, pixelFormatArray, count), (TText8*)__FILE__, __LINE__);
	TEST(count == numPixelFormats);
	
	CleanupStack::PopAndDestroy(pixelFormatArray);
	//array length is smaller than the supported pixel formats
	count = numPixelFormats - 1;
	pixelFormatArray = new(ELeave) TUidPixelFormat[count];
	CleanupArrayDeletePushL(pixelFormatArray);
	TEST(KErrOverflow == RSgImage::GetPixelFormats(info, pixelFormatArray, count));
	TEST(count == numPixelFormats);
	CleanupStack::PopAndDestroy(pixelFormatArray);
	
	//check all the pixel formats returned are in the common compatibility guarantees table
	TestGetPixelFormatCompatibilityGuaranteesL();
	
	//reset the array used by the GetPixelFormats function to prevent memory leaks
	info.iCpuAccess = ESgCpuAccessReadWrite;
	info.iScreenId = 100;
	TEST(KErrNone == RSgImage::GetPixelFormats(info, NULL, count));
	TEST(count == 0);
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0004
@SYMTestCaseDesc		Creates an uninitialised image.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)\n 
						RSgImage::IsNull()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure an uninitialised image can be created.
@SYMTestActions			Initialise the graphics resource component. Construct well-defined 
						TSgImageInfo objects that work on all platforms. Create an image 
						with a NULL base address for each of the TSgImageInfo object.
@SYMTestExpectedResults	The function should return KErrNone in all cases. The created images
						are uninitialised.
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestCreateImageUninitializedL()
	{	
	TestOpenDriverL();
	TSgImageInfo info;
	info.iSizeInPixels = TSize(100, 100);
	info.iUsage = ESgUsageDirectGdiTarget;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	RSgImage image;	
	CheckErrorL(KErrNone, image.Create(info, NULL, 0), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0005
@SYMTestCaseDesc		Creates an image from a block of memory.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)\n 
						RSgImage::IsNull()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure images can be created from a block of memory.
@SYMTestActions			Initialise the graphics resource component. Construct well-defined 
						TSgImageInfo objects that work on all platforms. Create a block of
						memory with or without initialised data. Then create an image with 
						the starting address of that memory.
@SYMTestExpectedResults	The function should return KErrNone in all cases. The user data starting
						at address aDataAddress will be populated into the new image.
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestCreateImageL()
	{	
	TestOpenDriverL();
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iUsage = ESgUsageDirectGdiSource;
	info.iShareable = ETrue;
	
	RSgImage image;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	CleanupStack::PopAndDestroy();	
#ifdef __WINS__	
	//create an image with usage set to screen source
	TSize screenSize;
	if(HAL::Get(info.iScreenId, HALData::EDisplayXPixels, screenSize.iWidth) == KErrNone
		&& HAL::Get(info.iScreenId, HALData::EDisplayYPixels, screenSize.iHeight) == KErrNone)
		{
		info.iUsage = ESgUsageScreenSource;
		info.iScreenId = KSgScreenIdMain;
		info.iSizeInPixels = TSize(screenSize.iWidth, screenSize.iHeight);
		CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
		CleanupClosePushL(image);
		CleanupStack::PopAndDestroy();
		
		info.iSizeInPixels = TSize(screenSize.iHeight, screenSize.iWidth);
		CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
		CleanupClosePushL(image);
		CleanupStack::PopAndDestroy();
		}
#endif	
	//create an image with some user attributes
	TSgUserAttribute testAttributes[] = {{0x00008888, 1}, {0x00009999, 2}};
	info.iUserAttributes = testAttributes;
	info.iUserAttributeCount = 2;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	CleanupStack::PopAndDestroy();
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0006
@SYMTestCaseDesc		Creates an image from an existing image.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9192
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const RSgImage&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure images can be created from existing images.
@SYMTestActions			Initialise the graphics resource component. Create an image. Create another
						image with the attributes of this image as the TSgImageInfo.
@SYMTestExpectedResults	The function should return KErrNone. The new image should be identical to
						the old one. 
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestCreateImageFromExistingImageL()
	{
	RSgImage image1;
	RSgImage image2;
	TSgImageInfo info;
	
	TestOpenDriverL();		
	CreateImageL(image1);
	CleanupClosePushL(image1);
	TEST(KErrNone == image1.GetInfo(info));
	CheckErrorL(KErrNone, image2.Create(info, image1), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image2);
	TSgImageInfo info2;
	TEST(KErrNone == image2.GetInfo(info2));
	TEST(CompareInfos(info, info2));
	CleanupStack::PopAndDestroy(2);
	TestCloseDriver();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0007
@SYMTestCaseDesc		Calls GetInfo on an image.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::GetInfo(TSgImageInfo&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the information about the image is correct.
@SYMTestActions			Initialise the graphics resource component. Construct a TSgImageInfo object
						and Initialise all the member variables. Create images without and with
						user attributes. Call GetInfo() and compare the resulting TSgImageInfo objects 
						with the previous ones.
@SYMTestExpectedResults	The function should return KErrNone. Information about the image object
						is stored in the aInfo parameter. The retrieved TSgImageInfo objects should be 
						identical to the original ones.
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestGetImageInfoL()
	{
	TestOpenDriverL();
	TSgImageInfo info;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iSizeInPixels = TSize(100, 100);
	info.iUsage = ESgUsageDirectGdiTarget;
	
	RSgImage image;
	CheckErrorL(KErrNone, image.Create(info, NULL, 0), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	
	TSgImageInfo info2;
	TEST(KErrNone == image.GetInfo(info2));
	TEST(CompareInfos(info, info2));
	CleanupStack::PopAndDestroy();
	
	//get info of image with user attributes
	TSgUserAttribute testAttributes[] = {{0x00008888, 1}, {0x00009999, 2}};
	info.iUserAttributes = testAttributes;
	info.iUserAttributeCount = 2;
	CheckErrorL(KErrNone, image.Create(info, NULL, 0), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	TSgImageInfo info3;
	TSgUserAttribute testAttributes2[2];
	testAttributes2[0].iUid = TUid::Uid(0x00008888);
	testAttributes2[1].iUid = TUid::Uid(0x00009999);
	info3.iUserAttributes = testAttributes2;
	info3.iUserAttributeCount = 2;
	TEST(KErrNone == image.GetInfo(info3));
	TEST(CompareInfos(info, info3));	
	CleanupStack::PopAndDestroy();
	
	info.iShareable = ETrue;
	CheckErrorL(KErrNone, image.Create(info, NULL, 0), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	TEST(KErrNone == image.GetInfo(info3));
	TEST(CompareInfos(info, info3));
	CleanupStack::PopAndDestroy();
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0008
@SYMTestCaseDesc		Retrieves drawable id and drawable type of an image.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Id()\n
						RSgImage::DrawableType()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the globally unique drawable id of the image can be retrieved.
@SYMTestActions			Initialise the graphics resource component. Create an image. Call
						Id() to retrieve its drawable id.
@SYMTestExpectedResults	A valid drawable id should be successfully returned.
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestGetImageDrawableIdL()
	{	
	RSgImage image;
	TEST(KNullUid == image.DrawableType());
	TSgDrawableId id = image.Id();
	TEST(id == KSgNullDrawableId);
	TestOpenDriverL();	
	CreateImageL(image);
	CleanupClosePushL(image);
	id = image.Id();
	TEST(id != KSgNullDrawableId);	
	TEST(KSgImageTypeUid == image.DrawableType());

	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0009
@SYMTestCaseDesc		Maps and unmaps an image from its creator process.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9193
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::MapReadOnly(const TAny*&, TInt&) const\n
 						RSgImage::MapWriteOnly(TAny*&, TInt&)\n 
						RSgImage::MapReadWrite(TAny*&, TInt&)\n 
						RSgImage::Unmap()
						by its creator process.
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure an image can be successfully mapped and unmapped
@SYMTestActions			Initialise the graphics resource component. Create mappable images
						with different TSgImageInfo values. Call different map functions the Unmap()
						function on each of them.
@SYMTestExpectedResults	All the functions should return KErrNone.
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestMapImageL()
	{
	TestOpenDriverL();
	RSgImage image;
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iUsage = ESgUsageDirectGdiSource;
	info.iCpuAccess = ESgCpuAccessReadOnly;
	
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	
	//test MapReadOnly
	const TAny* dataAddress;
	TInt dataStride;	
	TInt mapResult = image.MapReadOnly(dataAddress, dataStride);
	TEST(KErrNone == mapResult);
	if(KErrNone == mapResult)
		{
		for (TInt y = 0; y < 8; ++y)
			for (TInt x = 0; x < 8; ++x)
				{
				TUint16 pixelValue = *(TUint16*)PtrAdd(dataAddress, sizeof(TUint16) * x + dataStride * y);
				if (pixelValue != 0x7E0)
					{
					TEST(EFalse);
					break;
					}
				}
		}
	TEST(KErrNone == image.Unmap());
	CleanupStack::PopAndDestroy(&image);
	
	//test MapWriteOnly
	TAny* dataAddress1;
	
	info.iCpuAccess = ESgCpuAccessWriteOnly;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	mapResult = image.MapWriteOnly(dataAddress1, dataStride);
	TEST(KErrNone == mapResult);
	//change the second line to red
	if(KErrNone == mapResult)
		{
		for(TInt x = 0; x < 8; ++x)
			{
			*(TUint16*)PtrAdd(dataAddress1, sizeof(TUint16) * x + dataStride * 1) = 0xF800;
			}
		}
	TEST(KErrNone == image.Unmap());
	
	//check if the modification is successful	
	//create another image from the existing one and map it as read only
	info.iCpuAccess = ESgCpuAccessReadOnly;
	RSgImage image1;
	CheckErrorL(KErrNone, image1.Create(info, image), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image1);
	mapResult = image1.MapReadOnly(dataAddress, dataStride);
	TEST(mapResult == KErrNone);
	if(mapResult == KErrNone)
		{
		for(TInt x = 0; x < 8; ++x)
			{
			TUint16 pixelValue = *(TUint16*)PtrAdd(dataAddress, sizeof(TUint16) * x + dataStride * 1);
			if(pixelValue != 0xF800)
				{
				TEST(EFalse);
				break;
				}
			}
		}
	
	TEST(KErrNone == image1.Unmap());
	CleanupStack::PopAndDestroy(2, &image);
	
	//test MapReadWrite
	info.iCpuAccess = ESgCpuAccessReadWrite;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	mapResult = image.MapReadWrite(dataAddress1, dataStride);
	TEST(KErrNone == mapResult);
	
	if(KErrNone == mapResult)
		{
		//modify the image, set first line to blue
		for(TInt x = 0; x < 8; ++x)
			{
			*(TUint16*)PtrAdd(dataAddress1, sizeof(TUint16) * x) = 0x1F;
			}
		
		//check the modification
		for(TInt x = 0; x < 8; ++x)
			{
			TUint16 pixelValue = *(TUint16*)PtrAdd(dataAddress1, sizeof(TUint16) * x);
			if(pixelValue != 0x1F)
				{
				TEST(EFalse);
				break;
				}
			}
		}
	
	TEST(KErrNone == image.Unmap());	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0010
@SYMTestCaseDesc		Opens a handle to an image in both the current process and another process.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Open(const TSgDrawableId&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure image can be opened in both a second thread in the current 
						process and another process.
@SYMTestActions			Initialise the graphics resource component. Create a shareable image and
						a non-shareable image. Open() the image in the second thread in the current 
						process and in another process.
						Check various image attributes.
@SYMTestExpectedResults	The function must return KErrNone in both cases.
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestOpenImageL()
	{	
	TestOpenDriverL();
	
	//shareable image
	RSgImage image1;
	TSgImageInfo info1;
	info1.iSizeInPixels = TSize(8, 8);
	info1.iUsage = ESgUsageDirectGdiSource;
	info1.iPixelFormat = EUidPixelFormatRGB_565;
	info1.iCpuAccess = ESgCpuAccessReadWrite;
	info1.iShareable = ETrue;
	CheckErrorL(KErrNone, image1.Create(info1, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image1);
	
	//non-shareable image
	RSgImage image2;
	TSgImageInfo info2;
	info2.iSizeInPixels = TSize(8, 8);
	info2.iUsage = ESgUsageDirectGdiSource;
	info2.iPixelFormat = EUidPixelFormatRGB_565;
	info2.iCpuAccess = ESgCpuAccessReadWrite;
	info2.iShareable = EFalse;	
	CheckErrorL(KErrNone, image2.Create(info2, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image2);
	
	const TSgDrawableId id1 = image1.Id();
	const TSgDrawableId id2 = image2.Id();	
	
	//opening image in the current thread
	RSgImage image3;
	TEST(KErrNone == image3.Open(id1));
	image3.Close();
	TEST(KErrNone == image3.Open(id2));
	image3.Close();
	
	//opening image in a second thread in the current process
	TSgresTestInfo threadInfo = {id1, info1, 0, ESgresSecondThreadOpenImage};
	TInt testResult = CreateSecondThreadAndDoTestL(threadInfo);	
	// Test the results from the second thread
	User::LeaveIfError(testResult);
	TEST(testResult & EFirstTestPassed);
	TEST(testResult & ESecondTestPassed);
	TEST(testResult & EThirdTestPassed);
	TEST(testResult & EFourthTestPassed);
	TEST(testResult & EFifthTestPassed);
	
	threadInfo.iDrawableId = id2;
	threadInfo.iImageInfo = info2;
	testResult = CreateSecondThreadAndDoTestL(threadInfo);	
	// Test the results from the second thread
	User::LeaveIfError(testResult);
	TEST(testResult & EFirstTestPassed);
	TEST(testResult & ESecondTestPassed);
	TEST(testResult & EThirdTestPassed);
	TEST(testResult & EFourthTestPassed);
	TEST(testResult & EFifthTestPassed);
		
	//opening image in a second process	
	TSgresTestInfo processInfo = {id1, info1, 0, ESgresSecondProcessOpenImage};
	testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo);	
	User::LeaveIfError(testResult);
	TEST(testResult & EFirstTestPassed);	
	TEST(testResult & ESecondTestPassed);
	TEST(testResult & EThirdTestPassed);
	TEST(testResult & EFourthTestPassed);

	CleanupStack::PopAndDestroy(2, &image1);
	TestCloseDriver();
	}

class MFake
	{
public:
	enum { EInterfaceUid = 0x88888888 };
	};

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0011
@SYMTestCaseDesc		Opens a handle to an image with mode flags.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Open(const TSgDrawableId&, TUint32)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure image can be opened with mode flags.
@SYMTestActions			Initialise the graphics resource component. Create an image and
						call Open() on the image with flag ESgDoNotRestrictUsage. Call
						GetInfo() on the opened image and check the usage bit. Declare another
						image object and call Open() on it with the Id of the previously
						opened handle and without mode flags. Call GetInfo() on this image
						and check the usage bit.
@SYMTestExpectedResults	Both infos should contain all the possible usages supported by
 						the underlying implementation.						
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestOpenImageInternalL()
	{
	TestOpenDriverL();
	
	//shareable image
	RSgImage shareableImage;
	TSgImageInfo shareableInfo;
	shareableInfo.iSizeInPixels = TSize(8, 8);
	shareableInfo.iUsage = ESgUsageDirectGdiSource;
	shareableInfo.iPixelFormat = EUidPixelFormatRGB_565;
	shareableInfo.iCpuAccess = ESgCpuAccessReadWrite;
	shareableInfo.iShareable = ETrue;	
	CheckErrorL(KErrNone, shareableImage.Create(shareableInfo, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(shareableImage);
		
	RSgImage image1;
	CheckErrorL(KErrNone, image1.Open(shareableImage.Id(), ESgDoNotRestrictUsage), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image1);
	TSgImageInfo info1;
	image1.GetInfo(info1);	
	TEST(ESgUsageDirectGdiSource | info1.iUsage);
	TEST(ESgUsageCompositionSource | info1.iUsage);
	TEST(ESgUsageOpenGlesTexture2D | info1.iUsage);
	TEST(ESgUsageOpenGles2Texture2D | info1.iUsage);
	TEST(ESgUsageOpenVgImage | info1.iUsage);
	
	RSgImage image2;
	CheckErrorL(KErrNone, image2.Open(image1.Id()), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image2);
	TSgImageInfo info2;
	image2.GetInfo(info2);
	TEST(ESgUsageDirectGdiSource | info2.iUsage);
	TEST(ESgUsageCompositionSource | info2.iUsage);
	TEST(ESgUsageOpenGlesTexture2D | info2.iUsage);
	TEST(ESgUsageOpenGles2Texture2D | info2.iUsage);
	TEST(ESgUsageOpenVgImage | info2.iUsage);
	
	//non-shareable image
	RSgImage nonshareableImage;
	TSgImageInfo nonshareableInfo;
	nonshareableInfo.iSizeInPixels = TSize(8, 8);
	nonshareableInfo.iUsage = ESgUsageDirectGdiSource;
	nonshareableInfo.iPixelFormat = EUidPixelFormatRGB_565;
	nonshareableInfo.iCpuAccess = ESgCpuAccessReadWrite;
	nonshareableInfo.iShareable = EFalse;	
	CheckErrorL(KErrNone, nonshareableImage.Create(nonshareableInfo, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(nonshareableImage);
	
	RSgImage image3;
	CheckErrorL(KErrNone, image3.Open(nonshareableImage.Id(), ESgDoNotRestrictUsage), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image3);
	TSgImageInfo info3;
	image3.GetInfo(info3);	
	TEST(ESgUsageDirectGdiSource | info3.iUsage);
	TEST(ESgUsageCompositionSource | info3.iUsage);
	TEST(ESgUsageOpenGlesTexture2D | info3.iUsage);
	TEST(ESgUsageOpenGles2Texture2D | info3.iUsage);
	TEST(ESgUsageOpenVgImage | info3.iUsage);
	
	RSgImage image4;
	CheckErrorL(KErrNone, image4.Open(image3.Id()), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image4);
	TSgImageInfo info4;
	image4.GetInfo(info4);
	TEST(ESgUsageDirectGdiSource | info4.iUsage);
	TEST(ESgUsageCompositionSource | info4.iUsage);
	TEST(ESgUsageOpenGlesTexture2D | info4.iUsage);
	TEST(ESgUsageOpenGles2Texture2D | info4.iUsage);
	TEST(ESgUsageOpenVgImage | info4.iUsage);
	
	CleanupStack::PopAndDestroy(6, &shareableImage);
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0044
@SYMTestCaseDesc		Gets the interfaces implemented by CSgImageImpl class.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)\n 
						RSgImage::GetInterface()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the extension interfaces supported by RSgImage can be
						returned. 
@SYMTestActions			Initialise the graphics resource component. Create an image. Call
						GetInterface() to return the MSgImage_Sw and MSgImage_Chunk interfaces.
						Perform various operations on the interfaces. Declare a fake interface
						MFake and call GetInterface() on the image handle.
@SYMTestExpectedResults	The operations on the MSgImage_Sw and MSgImage_Chunk interfaces should
						succeed. Trying to get the MFake interface should return KErrExtensionNotSupported.
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestGetInterfaceL()
	{
	TestOpenDriverL();
	
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	info.iShareable = ETrue;
	
	RSgImage image;	
	MSgImage_Sw* swInterface = NULL;
	TEST(KErrBadHandle == image.GetInterface(swInterface));
	
	CheckErrorL(KErrNone, image.Create(info, NULL, 0), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	
	// check TSgDrawableId is not NULL
	TSgDrawableId id = image.Id();
	TEST(KSgNullDrawableId != id);
	
	TAny* dataAddress1 = NULL;
	TInt dataStride1 = 0;
	TInt result1 = image.GetInterface(swInterface);
	TEST(KErrNone == result1);
	if(KErrNone == result1)
		{		
		// Fill the image with 0xF0
		TInt sizeInBytes = 8 * 8 * 2;
		TESTL(swInterface->BeginDataAccess(ESgCpuAccessNone) == KErrArgument);
		TEST(swInterface->BeginDataAccess(ESgCpuAccessWriteOnly) == KErrNone);
		dataAddress1 = swInterface->DataAddress();
		dataStride1 = swInterface->DataStride();
		Mem::Fill(dataAddress1, sizeInBytes, 0xF0);
		TEST(swInterface->EndDataAccess() == KErrNone);
		
		//Map the image as read only and check if the previous operation is successful
		const TAny* dataAddressRead;
		TInt dataStride2;	
		TInt mapResult = image.MapReadOnly(dataAddressRead, dataStride2);
		TEST(KErrNone == mapResult);
		TEST(dataAddress1 == dataAddressRead);
		TEST(dataStride1 == dataStride2);
		if(KErrNone == mapResult)
			{
			for (TInt y = 0; y < 8; ++y)
				for (TInt x = 0; x < 8; ++x)
					{
					TUint16 pixelValue = *(TUint16*)PtrAdd(dataAddressRead, sizeof(TUint16) * x + dataStride2 * y);
					if (pixelValue != 0xF0F0)
						{
						TEST(EFalse);
						break;
						}
					}
			}
		TEST(KErrNone == image.Unmap());
		}
	
	MSgImage_Chunk* smChunk = NULL;
	TInt result2 = image.GetInterface(smChunk);
	TEST(KErrNone == result2);
	if(KErrNone == result2)
		{
		TAny* address = PtrAdd(smChunk->DataChunk().Base(), smChunk->DataOffset());
		TInt datastride = smChunk->DataStride();
		if(KErrNone == result1)
			{
			TEST(address == dataAddress1);
			TEST(datastride == dataStride1);
			}
		}
	
	//fake interface
	MFake* smFake = NULL;
	TEST(KErrExtensionNotSupported == image.GetInterface(smFake));
	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0024
@SYMTestCaseDesc		Gets supported pixel formats with invalid parameters.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::GetPixelFormats(const TSgImageInfo&, TUidPixelFormat*, TInt&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the correct error messages are returned when user tries to
						call GetPixelFormats() with invalid parameters.
@SYMTestActions			Initialise the graphics resource component. Call RSgImage::GetPixelFormats with:\n
						\t 1. invalid TSgImageInfo object, it could be any combinations of the following:\n
						\t \t negative width or height\n
						\t \t mutability incompatible with the specified CPU access and usage\n
						\t \t screen id < -1\n
						\t \t usage flag ESgUsageScreenSource with screen id -1\n
						\t 2. invalid aCount
@SYMTestExpectedResults	The function should return KErrArgument in both cases.
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestGetPixelFormatsInvalidL()
	{
	TSgImageInfo info1;
	TInt count = 0;
	
	TestOpenDriverL();
	//negative width
	info1.iSizeInPixels = TSize(-100, 100);
	TEST(KErrArgument == RSgImage::GetPixelFormats(info1, NULL, count));
	
	//negative height
	TSgImageInfo info2;
	info2.iSizeInPixels = TSize(100, -100);
	TEST(KErrArgument == RSgImage::GetPixelFormats(info2, NULL, count));
	
	//invalid or non-existent screen id
	TSgImageInfo info5;
	info5.iSizeInPixels = TSize(8, 8);
	info5.iUsage = ESgUsageScreenSource;
	info5.iCpuAccess = ESgCpuAccessReadWrite;
	info5.iScreenId = -2;
	TEST(KErrArgument == RSgImage::GetPixelFormats(info5, NULL, count));
	info5.iScreenId = 100;
	TEST(KErrNone == RSgImage::GetPixelFormats(info5, NULL, count));
	TEST(count == 0);
	
	//usage flag ESgUsageScreenSource with screen id -1
	TSgImageInfo info6;
	info6.iSizeInPixels = TSize(8, 8);
	info6.iScreenId = -1;
	info6.iUsage = ESgUsageScreenSource;
	TEST(KErrArgument == RSgImage::GetPixelFormats(info6, NULL, count));
	
	//negative aCount
	TSgImageInfo info7;
	info7.iCpuAccess = ESgCpuAccessNone;
	info7.iShareable = EFalse;
	info7.iSizeInPixels = TSize(10, 10);
	CheckErrorL(KErrNone, RSgImage::GetPixelFormats(info7, NULL, count), (TText8*)__FILE__, __LINE__);
	
	if(count > 0)
		{
		TUidPixelFormat* pixelFormatArray = new(ELeave) TUidPixelFormat[count];
		if(!pixelFormatArray)
			{
			User::Leave(KErrNoMemory);
			}
		CleanupStack::PushL(pixelFormatArray);
		count = -100;
		TEST(KErrArgument == RSgImage::GetPixelFormats(info7, pixelFormatArray, count));
		
		CleanupStack::PopAndDestroy(pixelFormatArray);
		}
	
	//reset the array used by the GetPixelFormats function to prevent memory leaks
	TSgImageInfo info8;
	info8.iSizeInPixels = TSize(10, 10);
	info8.iCpuAccess = ESgCpuAccessReadWrite;
	info8.iUsage = ESgUsageDirectGdiSource;
	info8.iShareable = EFalse;
	info8.iScreenId = 100;
	TEST(KErrNone == RSgImage::GetPixelFormats(info8, NULL, count));
	TEST(count == 0);
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0025
@SYMTestCaseDesc		Opens an image with different invalid parameters.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Open(const TSgDrawableId&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check correct error messages are returned when opening image with 
						different invalid parameters.
@SYMTestActions			Initialise the graphics resource component. Construct an RSgImage object. 
						Call the Open() function in both the current process and another process with:\n
						\t 1. a non-null RSgImage handle\n
						\t 2. null drawable id\n
						\t 3. fake drawable id\n
						\t 4. a non-sharable RSgImage handle
						Do the same tests in a second thread and a second process.
@SYMTestExpectedResults	The function should return:\n
						\t 1. KErrInUse\n
						\t 2. KErrArgument\n
						\t 3. KErrNotFound\n
						\t 4. KErrNone in the same process and KErrPermissionDenied or KErrNotFound in the second process
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestOpenImageInvalidL()
	{	
	TestOpenDriverL();
	//create a non-sharable image
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiTarget;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iShareable = EFalse;
	
	RSgImage image;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	const TSgDrawableId id = image.Id();
	
	//same thread
	//  non-empty handle
	RSgImage image1;
	CreateImageL(image1);
	CleanupClosePushL(image1);
	TEST(KErrInUse == image1.Open(id));
	image1.Close();
	
	//  null drawable id
	CheckErrorL(KErrArgument, image1.Open(KSgNullDrawableId), (TText8*)__FILE__, __LINE__);
	image1.Close();
	
	//  non-existing drawable id
	TSgDrawableId fakeid = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
	CheckErrorL(KErrNotFound, image1.Open(fakeid), (TText8*)__FILE__, __LINE__);
	image1.Close();
	
	//  open a non-sharable image
	TEST(KErrNone == image1.Open(id));
	image1.Close();
	
	//different thread in the same process
	TSgresTestInfo threadInfo = {id, info, 0, ESgresSecondThreadOpenImageInvalid};
	TInt testResult = CreateSecondThreadAndDoTestL(threadInfo);
	User::LeaveIfError(testResult);
	// Test the results from the second thread
	TEST(testResult & EFirstTestPassed);
	TEST(testResult & ESecondTestPassed);
	TEST(testResult & EThirdTestPassed);
	TEST(testResult & EFourthTestPassed);
	TEST(testResult & EFifthTestPassed);
	
	//different process	
	TSgresTestInfo processInfo = {id, info, 0, ESgresSecondProcessOpenImageInvalid};
	testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo);	
	User::LeaveIfError(testResult);
	TEST(testResult & EFirstTestPassed);	
	TEST(testResult & ESecondTestPassed);
	TEST(testResult & EThirdTestPassed);
	TEST(testResult & EFourthTestPassed);	
	TEST(testResult & EFifthTestPassed);
		
	CleanupStack::PopAndDestroy(2);
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0026
@SYMTestCaseDesc		Closes an image multiple times.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Close()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check calling Close() multiple times after Open() does not cause
						errors or panics.
@SYMTestActions			Initialise the graphics resource component. Create an image and then
						call Close() three times on it.
@SYMTestExpectedResults	The function should not cause any errors or panics.
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestCloseImageManyTimesL()
	{
	TestOpenDriverL();
	RSgImage image;
	CreateImageL(image);
	
	image.Close();
	image.Close();
	image.Close();
	
	TestCloseDriver();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0027
@SYMTestCaseDesc		Closes an image object without opening it.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Close ()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check Close() does nothing if the image is not already opened.
@SYMTestActions			Initialise the graphics resource component. Declare an RSgImage object and
						call Close() on it.
@SYMTestExpectedResults	The function should not cause any errors or panics.
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestCloseImageWithoutOpenL()
	{	
	TestOpenDriverL();
	RSgImage image;
	image.Close();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0028
@SYMTestCaseDesc		Creates an image with various invalid parameters.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9192
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)\n 
						RSgImage::Create(const TSgImageInfo, const RSgImage&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check the function returns the correct error messages when the parameters
						are invalid.
@SYMTestActions			Initialise the graphics resource component. Construct a TSgImageInfo object 
						and Initialise the all the member variables with random values. Call the appropriate 
						Create() function with:\n
						\t 1. invalid TSgImageInfo\n
						\t 2. unsupported TSgImageInfo\n
						\t 3. the creating RSgImage handle is not null\n
						\t 4. the aSgImage handle is null\n
						\t 5. the size and pixel format in TSgImageInfo is not the same as that of aSgImage\n
						\t 6. number of iUserAttributes is more than KMaxHint (8)\n
@SYMTestExpectedResults	The function should return\n
						\t 1. KErrArgument\n
						\t 2. KErrNotSupported\n
						\t 3. KErrInUse\n
						\t 4. KErrArgument\n
						\t 5. KErrNotSupported\n
						\t 6. KErrOverflow\n
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestCreateImageInvalidL()
	{
	TestOpenDriverL();
	RSgImage image;
	
	//invalid info 1 - invalid size
	TSgImageInfo info1;
	info1.iSizeInPixels = TSize(-100, 100);
	TEST(KErrArgument == image.Create(info1, NULL, 0));
	
	//invalid info 2 - const image with writable cpu access flag
	info1.iUsage = ESgUsageDirectGdiSource;
	info1.iCpuAccess = ESgCpuAccessReadWrite;
	TEST(KErrArgument == image.Create(info1, NULL, 0));
	
	//invalid info 3 - the image is used as a screen source but screen id is set to -1
	info1.iUsage = ESgUsageScreenSource;
	info1.iScreenId = KSgScreenIdAny;
	TEST(KErrArgument == image.Create(info1, NULL, 0));
	
	//invalid info 4 - incorrect user attribute count
	info1.iScreenId = KSgScreenIdMain;
	TSgUserAttribute KTestAttributes[] = {{0x00008888, 1}, {0x00009999, 2}};
	info1.iUserAttributes = KTestAttributes;
	info1.iUserAttributeCount = 0;
	TEST(KErrArgument == image.Create(info1, NULL, 0));
	
	//unsupported info 1 - unsupported pixel format
	TSgImageInfo info2;
	info2.iSizeInPixels = TSize(10, 10);
	info2.iUsage = ESgUsageDirectGdiTarget;
	info2.iPixelFormat = EUidPixelFormatAP_88;
	TEST(KErrNotSupported == image.Create(info2, NULL, 0));

	//unsupported info 2 - usage set to screen source and size is not the same as screen size
	info2.iUsage = ESgUsageScreenSource|ESgUsageDirectGdiTarget;
	info2.iSizeInPixels = TSize(9999, 8888);
	TEST(KErrNotSupported == image.Create(info2, NULL, 0));

#ifdef __WINS__
	TSize screenSize;
	if(HAL::Get(info2.iScreenId, HALData::EDisplayXPixels, screenSize.iWidth) == KErrNone
		&& HAL::Get(info2.iScreenId, HALData::EDisplayYPixels, screenSize.iHeight) == KErrNone)
		{
		info2.iUsage = ESgUsageScreenSource;
		info2.iScreenId = KSgScreenIdMain;
		info2.iCpuAccess = ESgCpuAccessReadWrite;
		info2.iSizeInPixels = TSize(screenSize.iWidth + 10, screenSize.iHeight);
		TEST(KErrNotSupported == image.Create(info2, NULL, 0));
		
		info2.iSizeInPixels = TSize(screenSize.iWidth, screenSize.iHeight + 10);
		TEST(KErrNotSupported == image.Create(info2, NULL, 0));
		
		info2.iSizeInPixels = TSize(screenSize.iHeight, screenSize.iWidth + 10);
		TEST(KErrNotSupported == image.Create(info2, NULL, 0));
		}	
#endif
	
	//non-null handle
	CreateImageL(image);
	CleanupClosePushL(image);
	TSgImageInfo info3;
	info3.iSizeInPixels = TSize(8, 8);
	info3.iUsage = ESgUsageDirectGdiSource;
	info3.iPixelFormat = EUidPixelFormatRGB_565;
	info3.iCpuAccess = ESgCpuAccessReadWrite;
	TInt ret = image.Create(info3, NULL, 0);	
	TEST(KErrInUse == ret);
	
	//non-null handle: create from an existing image
	RSgImage image1;
	CreateImageL(image1);
	CleanupClosePushL(image1);
	TEST(KErrInUse == image1.Create(info3, image));
	CleanupStack::PopAndDestroy();
	
	//null existing image handle
	RSgImage image2;
	TEST(image2.IsNull());
	TEST(KErrArgument == image1.Create(info3, image2));
		
	//the size and pixel format in TSgImageInfo is not the same as that of aSgImage
	TSgImageInfo info4;
	info4.iSizeInPixels = TSize(100, 100);
	info4.iUsage = ESgUsageDirectGdiSource;
	info4.iCpuAccess = ESgCpuAccessReadWrite;
	TEST(KErrNotSupported == image1.Create(info4, image));
	
	info4.iSizeInPixels = info3.iSizeInPixels;
	info4.iPixelFormat = EUidPixelFormatXRGB_8888;
	TEST(KErrNotSupported == image1.Create(info4, image));
	
	//number of iUserAttributes more than KMaxHint(8)
	TSgImageInfo info5;
	info5.iSizeInPixels = TSize(100, 100);
	info5.iPixelFormat = EUidPixelFormatRGB_565;
	info5.iCpuAccess = ESgCpuAccessNone;
	info5.iUsage = ESgUsageDirectGdiTarget;
	info5.iShareable = ETrue;
	TSgUserAttribute testAttributes[] = {{0x00008888, 1}, {0x00009999, 2}};
	info5.iUserAttributes = testAttributes;
	info5.iUserAttributeCount = 100;	
	CheckErrorL(KErrOverflow, image1.Create(info5, NULL, 0), (TText8*)__FILE__, __LINE__);
	
	CleanupStack::PopAndDestroy(&image);
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0039
@SYMTestCaseDesc		Calls GetInfo() on an invalid image.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::GetInfo(TSgImageInfo&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check the correct error is returned when the client tries to get 
						information on an invalid image.
@SYMTestActions			Initialise the graphics resource component. Call GetInfo on an image:\n
						\t 1. that is not initialised\n
						\t 2. TSgImageInfo has different user attributes Ids from the existing Ids.
@SYMTestExpectedResults	The function should return:\n
						\t 1. KErrBadHandle\n
						\t 2. KErrNotFound or KErrArgument
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestGetInfoInvalidImageL()
	{
	TestOpenDriverL();	
	RSgImage image;
	TSgImageInfo info;
	
	//uninitialised image
	TEST(KErrBadHandle == image.GetInfo(info));
	
	//TSgImageInfo has different user attributes Ids from the existing Ids.
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iSizeInPixels = TSize(100, 100);
	info.iUsage = ESgUsageDirectGdiTarget;
	TSgUserAttribute testAttributes[] = {{0x00008888, 1}, {0x00009999, 2}};
	info.iUserAttributes = testAttributes;
	info.iUserAttributeCount = 2;
	CheckErrorL(KErrNone, image.Create(info, NULL, 0), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	TSgImageInfo info2;
	TSgUserAttribute testAttributes2[1];
	info2.iUserAttributes = testAttributes2;
	info2.iUserAttributes[0].iUid = TUid::Uid(0x11111111);
	info2.iUserAttributeCount = 1;
	TInt result = image.GetInfo(info2);
	TEST(KErrNotFound == result || KErrArgument == result);
	TEST(!CompareInfos(info, info2));
	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0030
@SYMTestCaseDesc		Maps an image under various invalid conditions. 
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9193
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::MapReadOnly(const TAny*&, TInt&) const\n 
						RSgImage::MapWriteOnly(TAny*&, TInt&)\n 
						RSgImage::MapReadWrite(TAny*&, TInt&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check the correct error messages are returned when different map
						functions are called under various invalid conditions.
@SYMTestActions			Initialise the graphics resource component. \n
						\t 1. call any map function on an uninitialised RSgImage\n
						\t 2. call any map function twice\n
						\t 3. call MapReadOnly() on an RSgImage created without CPU access
						flag ESgCpuAccessReadOnly or ESgCpuAccessReadWrite\n
						\t 4. call MapWriteOnly() on an RSgImage created without CPU access
						flag ESgCpuAccessWriteOnly or ESgCpuAccessReadWrite\n
						\t 5. call MapReadWrite() on an RSgImage created without CPU access
						flag ESgCpuAccessReadWrite\n
						\t 6. call any map function in a second process
@SYMTestExpectedResults	The function should return:\n
						\t 1. KErrBadHandle\n
						\t 2. KErrInUse\n
						\t 3. KErrAccessDenied\n
						\t 4. KErrAccessDenied\n
						\t 5. KErrAccessDenied\n
						\t 6. KErrPermissionDenied
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestMapImageInvalidL()
	{
	TestOpenDriverL();
	RSgImage image;
	const TAny* dataAddressRead;
	TAny* dataAddressWrite;
	TInt dataStride;
	
	//an uninitialised image
	TEST(KErrBadHandle == image.MapReadOnly(dataAddressRead, dataStride));
	TEST(KErrBadHandle == image.MapWriteOnly(dataAddressWrite, dataStride));
	TEST(KErrBadHandle == image.MapReadWrite(dataAddressWrite, dataStride));
	
	//map twice
	RSgImage image1;
	CreateImageL(image1);
	CleanupClosePushL(image1);
	TEST(KErrNone == image1.MapReadOnly(dataAddressRead, dataStride));
	TEST(KErrInUse == image1.MapReadOnly(dataAddressRead, dataStride));
	TEST(KErrNone == image1.Unmap());
	
	TEST(KErrNone == image1.MapWriteOnly(dataAddressWrite, dataStride));
	TEST(KErrInUse == image1.MapWriteOnly(dataAddressWrite, dataStride));
	TEST(KErrNone == image1.Unmap());
	
	TEST(KErrNone == image1.MapReadWrite(dataAddressWrite, dataStride));
	TEST(KErrInUse == image1.MapReadWrite(dataAddressWrite, dataStride));
	TEST(KErrNone == image1.Unmap());
	
	CleanupStack::PopAndDestroy();	
	
	//  Initialise image
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	info.iShareable = ETrue;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	TEST(KErrNone == image.MapReadWrite(dataAddressWrite, dataStride)); //first time should pass
	TEST(KErrInUse == image.MapReadWrite(dataAddressWrite, dataStride)); //second time should fail
	TEST(KErrNone == image.Unmap());
	image.Close();
	
	//call MapReadOnly() on an RSgImage created without CPU access
	//flag ESgCpuAccessReadOnly or ESgCpuAccessReadWrite
	info.iCpuAccess = ESgCpuAccessNone;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	TEST(KErrAccessDenied == image.MapReadOnly(dataAddressRead, dataStride));
	image.Close();
	
	info.iCpuAccess = ESgCpuAccessWriteOnly;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	TEST(KErrAccessDenied == image.MapReadOnly(dataAddressRead, dataStride));
	image.Close();
	
	//call MapWriteOnly() on an RSgImage created without CPU access
	//flag ESgCpuAccessWriteOnly or ESgCpuAccessReadWrite
	info.iCpuAccess = ESgCpuAccessNone;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	TEST(KErrAccessDenied == image.MapWriteOnly(dataAddressWrite, dataStride));
	image.Close();
	
	info.iCpuAccess = ESgCpuAccessReadOnly;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	TEST(KErrAccessDenied == image.MapWriteOnly(dataAddressWrite, dataStride));
	image.Close();
	
	//call MapReadWrite() on an RSgImage created without CPU access
	//flag ESgCpuAccessReadWrite
	info.iCpuAccess = ESgCpuAccessNone;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	TEST(KErrAccessDenied == image.MapReadWrite(dataAddressWrite, dataStride));
	image.Close();
	
	info.iCpuAccess = ESgCpuAccessReadOnly;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	TEST(KErrAccessDenied == image.MapReadWrite(dataAddressWrite, dataStride));
	image.Close();
	
	info.iCpuAccess = ESgCpuAccessWriteOnly;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	TEST(KErrAccessDenied == image.MapReadWrite(dataAddressWrite, dataStride));
	image.Close();
	
	info.iCpuAccess = ESgCpuAccessReadWrite;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	
	//call map in a second thread in the current process - should succeed
	TSgresTestInfo threadInfo = {image.Id(), info, 0, ESgresSecondThreadMapImage};	
	TInt testResult = CreateSecondThreadAndDoTestL(threadInfo);
	User::LeaveIfError(testResult);
	TEST(testResult & EFirstTestPassed);
	TEST(testResult & ESecondTestPassed);
	TEST(testResult & EThirdTestPassed);
	TEST(testResult & EFourthTestPassed);
	TEST(testResult & EFifthTestPassed);
	TEST(testResult & ESixthTestPassed);
	TEST(testResult & ESeventhTestPassed);
	
	//call map in a non-owner process
	TSgresTestInfo processInfo = {image.Id(), info, 0, ESgresSecondProcessMapImage};
	testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo);	
	User::LeaveIfError(testResult);	
	TEST(testResult & EFirstTestPassed);
	TEST(testResult & ESecondTestPassed);
	TEST(testResult & EThirdTestPassed);
	TEST(testResult & EFourthTestPassed);
	TEST(testResult & EFifthTestPassed);
	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0031
@SYMTestCaseDesc		Unmaps an image under various invalid conditions.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9193
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::MapReadOnly(const TAny*&, TInt&) const\n 
						RSgImage::MapWriteOnly(TAny*&, TInt&)\n 
						RSgImage::MapReadWrite(TAny*&, TInt&)\n 
						RSgImage::Unmap()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check the correct error messages are returned when the Unmap()
						function is called under various invalid conditions.
@SYMTestActions			Initialise the graphics resource component.\n
						\t 1. call Unmap() on an uninitialised image object\n
						\t 2. create an image with CPU access flag ESgCpuAccessNone and then unmap it\n
						\t 3. call Unmap() on an image that was not mapped.\n
						\t 4. call Unmap() on a mapped image in a second thread in the same process.\n
						\t 5. call Unmap() on an image in a non-owner process
@SYMTestExpectedResults	The function should return:\n
						\t 1. KErrBadHandle\n
						\t 2. KErrGeneral\n
						\t 3. KErrGeneral\n
						\t 4. KErrNone\n
						\t 5. KErrGeneral
@SYMTestStatus			Implemented 
 */	
void CTSgImageGeneric::TestUnmapImageInvalidL()
	{
	TestOpenDriverL();
	RSgImage image;
	
	//unitialized image
	TEST(KErrBadHandle == image.Unmap());
	
	//create an image
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iShareable = ETrue;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	const TAny* dataAddressRead;
	TInt dataStride;
	CheckErrorL(KErrNone, image.Create(info, KImageData, 16), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	
	//call on an unmapped image
	TEST(KErrGeneral == image.Unmap());
	
	//map in the current thread and try to unmap in another thread in the current process
	//should fail	
	TEST(KErrNone == image.MapReadOnly(dataAddressRead, dataStride));	
	TSgDrawableId id = image.Id();
	TSgImageInfo imageinfo;
	image.GetInfo(imageinfo);
	
	TSgresTestInfo threadInfo = {id, imageinfo, 0, ESgresSecondThreadUnmapImage};
	TInt testResult = CreateSecondThreadAndDoTestL(threadInfo);
	User::LeaveIfError(testResult);
	TEST(testResult & EFirstTestPassed);
	TEST(testResult & ESecondTestPassed);
	
	//image should already be unmapped
	TEST(KErrGeneral == image.Unmap());
	
	//map in the current thread and try to unmap in another process
	//should fail
	TEST(KErrNone == image.MapReadOnly(dataAddressRead, dataStride));	
	TSgresTestInfo processInfo = {id, imageinfo, 0, ESgresSecondProcessUnmapImage};
	testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo);	
	User::LeaveIfError(testResult);
	TEST(testResult & EFirstTestPassed);	
	TEST(testResult & ESecondTestPassed);
	
	//image should still be mapped
	TEST(KErrNone == image.Unmap());
	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0048
@SYMTestCaseDesc		RSgImage stress tests
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)\n
						RSgImage::Close() 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure creating and destroying RSgImage multiple times work well.
@SYMTestActions			Create images 1000 times but only store KMaxImagesInArray of them 
						at one time in an RArray. When a new image is to be added to the 
						array, a random image from the array is removed.
@SYMTestExpectedResults	There should be no panics or leaves.
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestStress1L()
	{
	TestOpenDriverL();
	TSgImageInfo info;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iSizeInPixels = TSize(100, 100);
	info.iUsage = ESgUsageDirectGdiTarget;
	
	TInt KMaxImagesInArray =40;
	TInt64 seed = 0;
	for (TInt count = 1000; count > 0; --count)
		{
		RSgImage image;
		CheckErrorL(KErrNone, image.Create(info, NULL, 0), (TText8*)__FILE__, __LINE__);
		CleanupClosePushL(image);
		iTestImages.AppendL(image);
		CleanupStack::Pop();
		if (iTestImages.Count() >= KMaxImagesInArray)
			{
			TInt i = Math::Rand(seed) % KMaxImagesInArray;
			iTestImages[i].Close();
			iTestImages.Remove(i);
			}
		}
	TInt count = iTestImages.Count();
	for(TInt i=0; i<count; ++i)
		{
		iTestImages[i].Close();
		}	
	iTestImages.Reset();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0049
@SYMTestCaseDesc		RSgImage multiple threads open and close stress tests
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImage::Open(const TSgDrawableId&)\n
						RSgImage::Close() 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure opening and destroying RSgImage multiple times and randomly
						in other threads work well.
@SYMTestActions			Create an image in the main thread. In a loop of 1000 times, randomly select
						one of the five threads and either open or close the image depending on the 
						state of the thread.
@SYMTestExpectedResults	There should be no panics or leaves.
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestStress2L()
	{
	TestOpenDriverL();
	RSgImage image;
	CreateImageL(image);
	CleanupClosePushL(image);
	const TSgDrawableId id = image.Id();
	
	TSgImageInfo info;
	TEST(KErrNone == image.GetInfo(info));
	TSgresTestInfo threadInfo = {id, info, 0, ESgresMultipleThreadStressTest, EFalse};
	
	//create a semaphore
	RSemaphore sem;
	User::LeaveIfError(sem.CreateGlobal(KSecondThreadSemaphore, 0, EOwnerThread));
	CleanupClosePushL(sem);
	
	//create threads
	const TInt KNoOfThreads = 5;
	RArray<RThread> threads;
	_LIT(KMultipleThreadName, "Multiple Threads Stress Test Thread ");
	for (TInt i = 0; i < KNoOfThreads; ++i)
		{
		TBuf<50> threadName(KMultipleThreadName);
		threadName.AppendNum(i);
		RThread thread;
		User::LeaveIfError(thread.Create(threadName, SecondThreadStart, KDefaultStackSize, &User::Heap(), &threadInfo));
		thread.SetPriority(EPriorityLess);
		threads.AppendL(thread);
		}
	// run threads
	for (TInt i = 0; i < KNoOfThreads; ++i)
		{
		threads[i].Resume();
		}
	// wait for the threads to terminate processing
	for (TInt i = 0; i < KNoOfThreads; ++i)
		{
		sem.Wait();
		}
	for (TInt i = 0; i < KNoOfThreads; ++i)
		{
		threads[i].Close();
		}
	CleanupStack::PopAndDestroy(2); // image, sem
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0051
@SYMTestCaseDesc		Calls RSgImage::GetInterface() with an invalid image handle
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::GetInterface(TUid, TAny*&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling GetInterface() with an invalid image handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call GetInterface() on the second
						handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 2(ESgPanicBadDrawableHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageGetInterfaceInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageGetInterfaceInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageGetInterfaceInvalidHandleLThread");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 2, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0052
@SYMTestCaseDesc		Calls RSgImage::GetInterface() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::GetInterface(TUid, TAny*&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling GetInterface() with a non-null handle and an uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call GetInterface() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageGetInterfaceNoDriverL()
	{	
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageGetInterfaceNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageGetInterfaceNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0053
@SYMTestCaseDesc		Calls RSgImage::Close() with an invalid image handle
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::Close()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Close() with an invalid image handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call Close() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 2(ESgPanicBadDrawableHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageCloseInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCloseInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCloseInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 2, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0054
@SYMTestCaseDesc		Calls RSgImage::Close() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::Close()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Close() with a non-null handle and an uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call Close() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageCloseNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCloseNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCloseNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0055
@SYMTestCaseDesc		Calls RSgImage::Id() with an invalid image handle
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::Id()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Id() with an invalid image handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call Id() on the second
						handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 2(ESgPanicBadDrawableHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageIdInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageIdInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageIdInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 2, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0056
@SYMTestCaseDesc		Calls RSgImage::Id() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::Id()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Id() with a non-null handle and an uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call Id() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageIdNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageIdNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageIdNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0057
@SYMTestCaseDesc		Calls RSgImage::Create() with an invalid image handle
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9192
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const RSgImage&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Create() with an invalid image handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call Create() on the second
						handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 2(ESgPanicBadDrawableHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageCreateInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCreateInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCreateInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0058
@SYMTestCaseDesc		Calls RSgImage::GetInfo() with an invalid image handle
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::GetInfo(TSgImageInfo&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling GetInfo() with an invalid image handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call GetInfo() on the second
						handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 2(ESgPanicBadDrawableHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageGetInfoInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageGetInfoInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageGetInfoInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0059
@SYMTestCaseDesc		Calls RSgImage::GetInfo() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				SgImage::GetInfo(TSgImageInfo&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling GetInfo() with a non-null handle and an uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call GetInfo() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageGetInfoNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageGetInfoNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageGetInfoNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0060
@SYMTestCaseDesc		Calls RSgImage::MapReadOnly with an invalid image handle
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9193
@SYMFssID				RSgImage::MapReadOnly(const TAny*&, TInt&) const\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling MapReadOnly() with an invalid image handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call MapReadOnly() on the second
						handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 2(ESgPanicBadDrawableHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageMapReadOnlyInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageMapReadOnlyInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageMapReadOnlyInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0061
@SYMTestCaseDesc		Calls RSgImage::MapReadOnly with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9193
@SYMFssID				RSgImage::MapReadOnly(const TAny*&, TInt&) const\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling MapReadOnly() with a non-null handle and an uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call MapReadOnly() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageMapReadOnlyNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageMapReadOnlyNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageMapReadOnlyNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0062
@SYMTestCaseDesc		Calls RSgImage::MapWriteOnly() with an invalid image handle
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9193
@SYMFssID				RSgImage::MapWriteOnly(TAny*&, TInt&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling MapWriteOnly() with an invalid image handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call MapWriteOnly() on the second
						handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 2(ESgPanicBadDrawableHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageMapWriteOnlyInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageMapWriteOnlyInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageMapWriteOnlyInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0063
@SYMTestCaseDesc		Calls RSgImage::MapWriteOnly() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9193
@SYMFssID				RSgImage::MapWriteOnly(TAny*&, TInt&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling MapWriteOnly() with a non-null handle and an uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call MapWriteOnly() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageMapWriteOnlyNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageMapWriteOnlyNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageMapWriteOnlyNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0064
@SYMTestCaseDesc		Calls RSgImage::MapReadWrite() with an invalid image handle
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9193
@SYMFssID				RSgImage::MapReadWrite(TAny*&, TInt&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling MapReadWrite() with an invalid image handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call MapReadWrite() on the second
						handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 2(ESgPanicBadDrawableHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageMapReadWriteInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageMapReadWriteInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageMapReadWriteInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0065
@SYMTestCaseDesc		Calls RSgImage::MapReadWrite() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9193
@SYMFssID				RSgImage::MapReadWrite(TAny*&, TInt&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling MapReadWrite() with a non-null handle and an uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call MapReadWrite() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageMapReadWriteNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageMapReadWriteNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageMapReadWriteNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0066
@SYMTestCaseDesc		Calls RSgImage::Unmap() with an invalid image handle
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMREQ					REQ9193
@SYMFssID				RSgImage::Unmap()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Unmap() with an invalid image handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call Unmap() on the second
						handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 2(ESgPanicBadDrawableHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageUnmapInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageUnmapInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageUnmapInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0067
@SYMTestCaseDesc		Calls RSgImage::Unmap() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::Unmap()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Unmap() with a non-null handle and an uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call Unmap() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageUnmapNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageUnmapNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageUnmapNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0082
@SYMTestCaseDesc		Calls RSgImage::Open() when the driver was not initialised.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::Open(const TSgDrawableId&, TUint32)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling RSgImage::Open() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgImage::Open() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageOpenNoDriver1L()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageOpenNoDriver1, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageOpenNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0083
@SYMTestCaseDesc		Creates an image from memory when the driver was not initialised.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling RSgImage::Create() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgImage::Open() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageCreateNoDriver1L()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCreateNoDriver1, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageCreateNoDriver1L");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0084
@SYMTestCaseDesc		Creates an image from an existing image when the driver was not initialised.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const RSgImage&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling RSgImage::Create() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgImage::Open() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageCreateNoDriver2L()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCreateNoDriver2, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageCreateNoDriver2L");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0085
@SYMTestCaseDesc		Calls RSgImage::GetPixelFormats() when the driver was not initialised.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::GetPixelFormats(const TSgImageInfo&, TUidPixelFormat*, TInt&) \n
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling RSgImage::GetPixelFormats() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgImage::GetPixelFormats() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageGetPixelFormatsNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageGetPixelFormatsNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageGetPixelFormatsNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0093
@SYMTestCaseDesc		Calls RSgImage::Open() with mode flags when the driver was not initialised.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::Open(const TSgDrawableId&, TUint32)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling RSgImage::Open() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgImage::Open() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageOpenNoDriver2L()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageOpenNoDriver2, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageOpenNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0088
@SYMTestCaseDesc		Calls RSgImage::DrawableType() when the driver was not initialised.
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::DrawableType()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling RSgImage::DrawableType() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgImage::DrawableType() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageDrawableTypeNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageDrawableTypeInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageOpenNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 2, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0089
@SYMTestCaseDesc		Calls RSgImage::DrawableType() with an invalid image handle
@SYMPREQ				PREQ39
@SYMREQ					REQ8809
@SYMREQ					REQ9175
@SYMFssID				RSgImage::DrawableType()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling RSgImage::DrawableType() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgImage::DrawableType() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES SGRES 2(ESgPanicBadDrawableHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageGeneric::TestPanicImageDrawableTypeInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageDrawableTypeNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageOpenNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

void CTSgImageGeneric::DoMemoryTestsL()
	{
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetPixelFormatsL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCreateImageUninitializedL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCreateImageL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCreateImageFromExistingImageL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetImageInfoL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetImageDrawableIdL(); 
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestMapImageL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestOpenImageL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetInterfaceL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetPixelFormatsInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestOpenImageInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCloseImageManyTimesL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCloseImageWithoutOpenL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCreateImageInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetInfoInvalidImageL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestMapImageInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestUnmapImageInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	}
