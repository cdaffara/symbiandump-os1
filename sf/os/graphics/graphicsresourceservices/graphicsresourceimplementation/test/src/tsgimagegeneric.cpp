// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Graphics Resource API Conformance Test Suite
*/


#include "tsgimagegeneric.h"

#include <e32math.h>

CTSgImageGeneric::CTSgImageGeneric(TBool aConformanceTests) :
	CTSgTestStepBase(aConformanceTests)
	{
	INFO_PRINTF1(_L("Graphics resource component test - RSgImage Generic Tests.\r\n"));
	}

CTSgImageGeneric::~CTSgImageGeneric()
	{
	DestroyImages();
	}

void CTSgImageGeneric::DestroyImages()
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
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0011"));
	INFO_PRINTF1(_L("Ensure all mandatory image pixel formats supported.\r\n"));
	TestCreateImageAllPixelFormatsL();
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
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0010"));
	INFO_PRINTF1(_L("Opening a handle to an image in both the current process and another process.\r\n"));
	TestOpenImageL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0100"));
	INFO_PRINTF1(_L("Creating images with negative and invalid data strides.\r\n"));
	TestCreateImageDataStrideL();
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
	TestGetInfoImageInvalidL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0092"));
	INFO_PRINTF1(_L("Getting Attributes about an invalid image.\r\n"));
	TestGetAttributesImageInvalidL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0048"));
	INFO_PRINTF1(_L("RSgImage generic stress test.\r\n"));
	TestStress1L();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0049")); 
	INFO_PRINTF1(_L("RSgImage generic multi-thread stress test.\r\n"));
	TestStress2L();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0104"));
	INFO_PRINTF1(_L("Opening a RSgImage in a second process with multiple threads.\r\n"));
	TestOpenImageMultithreadedL();
	RecordTestResultL();

#ifdef _DEBUG

	SetTestStepID(_L("GRAPHICS-RESOURCE-0052"));
	INFO_PRINTF1(_L("RSgImage generic panic test - GetInterface() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageGetInterfaceNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0054"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Close() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageCloseNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0056"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Id() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageIdNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0059"));
	INFO_PRINTF1(_L("RSgImage generic panic test - GetInfo() with non null image handle and uninitialised driver\r\n"));
	TestPanicImageGetInfoNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0082"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Open() with an uninitialised driver\r\n"));
	TestPanicImageOpenNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0083"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Create() from memory with an uninitialised driver\r\n"));
	TestPanicImageCreateNoDriver1L();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0084"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Create() from existing image with an uninitialised driver\r\n"));
	TestPanicImageCreateNoDriver2L();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0088"));
	INFO_PRINTF1(_L("RSgImage generic panic test - DrawableType() with an uninitialised driver\r\n"));	
	TestPanicImageDrawableTypeNoDriverL();
	RecordTestResultL();
	
	INFO_PRINTF1(_L("RSgImage generic panic test - GetAttribute() with an uninitialised driver\r\n")); 
	TestPanicImageGetAttributeNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0097"));
	INFO_PRINTF1(_L("RSgAttributeArray generic panic test - Out of bounds array index specified\r\n"));	
	TestPanicImageAttributeArrayInvalidIndexL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0098"));
	INFO_PRINTF1(_L("RSgAttributeArray generic panic test - Out of bounds array index specified\r\n"));	
	TestPanicImageAttributeArrayInvalidIndex2L();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0102"));
	INFO_PRINTF1(_L("RSgDriver/RSgImage multi processs Create, Open and Close test\r\n"));	
	TestCreateOpenCloseImageAndDriverInSecondThreadL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0052"));
	INFO_PRINTF1(_L("RSgImage generic panic test - GetInterface() with invalid image handle\r\n"));
	TestPanicImageGetInterfaceInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0053"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Close() with invalid image handle\r\n"));
	TestPanicImageCloseInvalidHandleL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0055"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Id() with invalid image handle\r\n"));
	TestPanicImageIdInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0057"));
	INFO_PRINTF1(_L("RSgImage generic panic test - Create() with invalid image handle\r\n"));
	TestPanicImageCreateInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0058"));
	INFO_PRINTF1(_L("RSgImage generic panic test - GetInfo() with invalid image handle\r\n"));
	TestPanicImageGetInfoInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0089"));
	INFO_PRINTF1(_L("RSgImage generic panic test - DrawableType() with invalid image handle\r\n"));	
	TestPanicImageDrawableTypeInvalidHandleL();
	RecordTestResultL();

	INFO_PRINTF1(_L("RSgImage generic panic test - GetInfo() with invalid image handle\r\n")); 
	TestPanicImageGetAttributeInvalidHandleL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0101"));
	INFO_PRINTF1(_L("RSgDriver generic panic test - Closing a driver with open resources\r\n"));	
	TestPanicDriverCloseOpenResourcesL();
	RecordTestResultL();
#else
	INFO_PRINTF1(_L("Warning: Skipping the panic tests in release build. \r\n"));
#endif	
	return TestStepResult();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0003
@SYMTestCaseDesc		Gets supported pixel formats on all platforms.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::GetPixelFormats(const TSgImageInfo&, TUidPixelFormat*, TInt&)
@SYMTestPriority		High
@SYMTestType			UT
@SYMTestPurpose			To ensure all the available pixel formats are returned 
						in the array.
@SYMTestActions			Initialise the graphics resource component.
						Construct an RArray to recieve the pixel formats.
						1. Retrieve pixel formats with an RArray<TInt>. Check the array size.
						2. Retrieve pixel formats when using an invalid usage bit value.
						3. Retrieve pixel formats supporting OpenGL ES.
						4. Retrieve pixel formats supporting OpenGL ES 2.
						5. Ensure the mandated pixel formats are available					
@SYMTestExpectedResults	The function must return:
						1. KErrNone
						2. KErrNone, but zero entries in the returned array.
						3. KErrNone, and a non-zero array size.
						4. KErrNone, and a non-zero array size.
						5. The returned pixel formats should cover the pixel formats 
						in the image compatibility table that works on all platforms.
 */
void CTSgImageGeneric::TestGetPixelFormatsL()
	{
	TestOpenDriverL();
	
	//Retrieve the pixel formats
	RArray<TInt> pixelFormatArray;
	CheckErrorL(KErrNone, RSgImage::GetPixelFormats(ESgUsageBitOpenVgImage, pixelFormatArray), (TText8*)__FILE__, __LINE__);
	TEST(pixelFormatArray.Count() > 0);
	pixelFormatArray.Reset();

	// invalid usage bit
	const TInt KInvalidUsageBit = 0x1000;
	CheckErrorL(KErrNone, RSgImage::GetPixelFormats(KInvalidUsageBit, pixelFormatArray), (TText8*)__FILE__, __LINE__);
	TEST(pixelFormatArray.Count() == 0);
	pixelFormatArray.Reset();

	//check all the pixel formats returned are in the common compatibility guarantees table
	TestGetPixelFormatCompatibilityGuaranteesL();

	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0004
@SYMTestCaseDesc		Creates an uninitialised image.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt) 
						RSgImage::IsNull()
@SYMTestPriority		Critical
@SYMTestType			UT
@SYMTestPurpose			To ensure an uninitialised image can be created.
@SYMTestActions			Initialise the graphics resource component.
						Construct a well-defined TSgImageInfo object to work on all platforms.
						Create an image	with a NULL base address for the TSgImageInfo object.
@SYMTestExpectedResults	Create() should return KErrNone in all cases.
						The image data in the created images is uninitialised.
 */	
void CTSgImageGeneric::TestCreateImageUninitializedL()
	{	
	TestOpenDriverL();
	TSgImageInfo info;
	info.iSizeInPixels = TSize(100, 100);
	info.iUsage = ESgUsageBitOpenVgImage;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	RSgImage image;	
	CheckErrorL(KErrNone, image.Create(info, NULL, 0), (TText8*)__FILE__, __LINE__);
	TEST(!image.IsNull());
	image.Close();
	
	TestCloseDriver();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0005
@SYMTestCaseDesc		Creates an image from a block of memory.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt) 
						RSgImage::IsNull()
@SYMTestPriority		Critical
@SYMTestType			UT
@SYMTestPurpose			To ensure images can be created from a block of memory.
@SYMTestActions			Initialise the graphics resource component.
						Construct well-defined TSgImageInfo objects that work on all platforms.
						Create a block of memory with or without initialised data.
						Then create an image with the starting address of that memory.
@SYMTestExpectedResults	The function should return KErrNone in all cases.
						The user data starting at address aDataAddress will be populated into the new image.
 */	
void CTSgImageGeneric::TestCreateImageL()
	{	
	TestOpenDriverL();
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iUsage = ESgUsageBitOpenVgImage;
	
	RSgImage image;
	CheckErrorL(KErrNone, image.Create(info, KCrossImageData, KCrossImageDataStride), (TText8*)__FILE__, __LINE__);
	TEST(!image.IsNull());
	image.Close();
	
	//Ensure that it is possible to create an image of at least 2048x2048
    RSgImage image2;
    TSgImageInfo info2;
    info2.iSizeInPixels = TSize(2048, 2048);
    info2.iPixelFormat = EUidPixelFormatXRGB_8888;
    info2.iUsage = ESgUsageBitOpenVgImage;
    
    TEST(KErrNone == image2.Create(info2));
    image2.Close();
	
    info2.iSizeInPixels = TSize(KDiagonalImageSize, KDiagonalImageSize);
    // This should create an image with a green diagonal from top-left to bottom-right on white background
    TEST(KErrNone == image.Create(info2, iDiagonalImageData, KDiagonalImageDataStride));
    image.Close();
    // This should create an image with a green diagonal from top-right to bottom-left on white background
    TEST(KErrNone == image.Create(info2, iDiagonalImageData, -KDiagonalImageDataStride));
    image.Close();
    
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0011
@SYMTestCaseDesc		Creates an uninitialised image in all mandatory pixel formats.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt) 
@SYMTestPriority		High
@SYMTestType			UT
@SYMTestPurpose			To ensure all mandatory image pixel formats are supported.
@SYMTestActions			Initialise the graphics resource component. 
						Create an image	with a NULL base address for each mandatory pixel 
						format of Graphics Resource implementations.						
@SYMTestExpectedResults	The function should return KErrNone in all cases.
 */	
void CTSgImageGeneric::TestCreateImageAllPixelFormatsL()
	{	
	TestOpenDriverL();

	RSgImage image;
	const TSize KImageSize = TSize(8, 8);
	const TUint32 KImageUsage = ESgUsageBitOpenVgImage;
	TInt err = KErrNone;

	err = image.Create(TSgImageInfo(KImageSize, EUidPixelFormatA_8, KImageUsage), NULL, 0);
	TESTE(err == KErrNone, err);
	TEST(!image.IsNull());
	image.Close();
	
	err = image.Create(TSgImageInfo(KImageSize, EUidPixelFormatRGB_565, KImageUsage), NULL, 0);
	TESTE(err == KErrNone, err);
	TEST(!image.IsNull());
	image.Close();
	
	err = image.Create(TSgImageInfo(KImageSize, EUidPixelFormatXRGB_8888, KImageUsage), NULL, 0);
	TESTE(err == KErrNone, err);
	TEST(!image.IsNull());
	image.Close();
	
	err = image.Create(TSgImageInfo(KImageSize, EUidPixelFormatARGB_8888, KImageUsage), NULL, 0);
	TESTE(err == KErrNone, err);
	TEST(!image.IsNull());
	image.Close();
	
	err = image.Create(TSgImageInfo(KImageSize, EUidPixelFormatARGB_8888_PRE, KImageUsage), NULL, 0);
	TESTE(err == KErrNone, err);
	TEST(!image.IsNull());
	image.Close();
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0006
@SYMTestCaseDesc		Creates an image from an existing image.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const RSgImage&)
@SYMTestPriority		Critical
@SYMTestType			UT
@SYMTestPurpose			To ensure images can be created from existing images.
@SYMTestActions			Initialise the graphics resource component. 
						Create an image.
						Create another image with the attributes of this image as the TSgImageInfo.
@SYMTestExpectedResults	The function should return KErrNone.
						The new image should be identical to the old one. 
 */	
void CTSgImageGeneric::TestCreateImageFromExistingImageL()
	{
	RSgImage image1;
	RSgImage image2;
	TSgImageInfo info;
	info.iUsage = ESgUsageBitOpenVgImage;
	
	TestOpenDriverL();		
	CreateImageL(image1);
	CleanupClosePushL(image1);
	TEST(KErrNone == image1.GetInfo(info));
	
	info.iUsage = ESgUsageBitOpenVgImage;
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
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::GetInfo(TSgImageInfo&)
@SYMTestPriority		Critical
@SYMTestType			UT
@SYMTestPurpose			To ensure the information about the image is correct.
@SYMTestActions			Initialise the graphics resource component.
						Construct a TSgImageInfo object and initialise all the member variables.
						Call GetInfo() and compare the resulting TSgImageInfo objects with previous ones.
@SYMTestExpectedResults	The function should return KErrNone.
						Information about the image object is stored in the aInfo parameter. 
						The retrieved TSgImageInfo objects should be identical to the original ones.
 */	
void CTSgImageGeneric::TestGetImageInfoL()
	{
	TestOpenDriverL();
	TSgImageInfo info;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iSizeInPixels = TSize(100, 100);
	info.iUsage = ESgUsageBitOpenVgImage;
	
	RSgImage image;
	CheckErrorL(KErrNone, image.Create(info, NULL, 0), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	
	TSgImageInfo info2;
	TEST(KErrNone == image.GetInfo(info2));
	TEST(CompareInfos(info, info2));
	CleanupStack::PopAndDestroy();
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0008
@SYMTestCaseDesc		Retrieves drawable id and drawable type of an image.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Id()
						RSgImage::DrawableType()
@SYMTestPriority		Critical
@SYMTestType			UT
@SYMTestPurpose			To ensure the globally unique drawable id of the image can be retrieved.
@SYMTestActions			Initialise the graphics resource component. Create an image. Call
						Id() to retrieve its drawable id.
@SYMTestExpectedResults	A valid drawable id should be successfully returned when the image has been opened.
                        KSgNullDrawableId should be returned if it has not; the default drawable type should
                        be KNullUid.
 */	
void CTSgImageGeneric::TestGetImageDrawableIdL()
	{	
	RSgImage image;
	TEST(KNullUid == image.DrawableType());
	TSgDrawableId id = image.Id();
	TEST(id == KSgNullDrawableId);
	TestOpenDriverL();	
	
	CreateImageL(image);
	id = image.Id();
	TEST(id != KSgNullDrawableId);	
	TEST(KSgImageTypeUid == image.DrawableType());

	image.Close();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0010
@SYMTestCaseDesc		Opens a handle to an image in both the current process and another process.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Open(const TSgDrawableId&)
@SYMTestPriority		Critical
@SYMTestType			UT
@SYMTestPurpose			To ensure image can be opened in both a second thread in the current 
						process and another process.
@SYMTestActions			Initialise the graphics resource component. Create an image and call
                        Open() on the image in the second thread in the current process and in 
                        another process. To ensure they are correct compare their Id and attributes.
@SYMTestExpectedResults	The function must return KErrNone in all cases. 
 */	
void CTSgImageGeneric::TestOpenImageL()
	{	
	TestOpenDriverL();
	
	RSgImage image1;
	TSgImageInfo info1;
	info1.iSizeInPixels = TSize(8, 8);
	info1.iUsage = ESgUsageBitOpenVgImage;
	info1.iPixelFormat = EUidPixelFormatRGB_565;
	CheckErrorL(KErrNone, image1.Create(info1, KCrossImageData, KCrossImageDataStride), (TText8*)__FILE__, __LINE__);
	
	const TSgDrawableId id1 = image1.Id();
	
	//opening image in the current thread
	RSgImage image2;
	TEST(KErrNone == image2.Open(id1));
	TEST(image1.Id() == image2.Id());
	
	TSgImageInfo getInfo1;
	TSgImageInfo getInfo2;
	
	image1.GetInfo(getInfo1);
	image2.GetInfo(getInfo2);
	
	TEST(CompareInfos(info1, getInfo1));
	TEST(getInfo1.iPixelFormat == getInfo2.iPixelFormat
		 && getInfo1.iSizeInPixels == getInfo2.iSizeInPixels
		 && getInfo1.iUsage == getInfo2.iUsage);
	
	if (iEnableConformanceTests)
		{
		//opening image in a second thread in the current process
		TSgThreadTestInfo threadInfo(id1, info1, ESgresSecondThreadOpenImage);
		TInt testResult = 0;
		TRAPD(err, testResult = CreateSecondThreadAndDoTestL(threadInfo));
		TEST(err == KErrNone);
		TEST(testResult >= 0);
		
		// Test the results from the second thread
		TEST(testResult & EFirstTestPassed);
		TEST(testResult & ESecondTestPassed);
		TEST(testResult & EThirdTestPassed);
		TEST(testResult & EFourthTestPassed);
		TEST(testResult & EFifthTestPassed);
		TEST(testResult & ESixthTestPassed);
		TEST(testResult & ESeventhTestPassed);
		TEST(testResult & EEighthTestPassed);
			
		//opening image in a second process	
		TSgProcessTestInfo processInfo(id1, getInfo1, ESgresSecondProcessOpenImage);
		TRAP(err, testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo));
		TEST(err == KErrNone);
		TEST(testResult >= 0);
		
		TEST(testResult & EFirstTestPassed);	
		TEST(testResult & ESecondTestPassed);
		TEST(testResult & EThirdTestPassed);
		TEST(testResult & EFourthTestPassed);
		TEST(testResult & EFifthTestPassed);
		TEST(testResult & ESixthTestPassed);
		TEST(testResult & ESeventhTestPassed);
		}
    
	image1.Close();
	image2.Close();
	
	TestCloseDriver();
	}

/**
Fake RSgImage extension interface used in negative RSgImage::GetInterface() Tests
 */
class MFake
    {
public:
    enum { EInterfaceUid = 0x88888888 };
    };

class MFakeNullUid
    {
public:
    enum { EInterfaceUid = KNullUidValue};
    };

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0044
@SYMTestCaseDesc		Gets an invalid/unsupported image extension interface
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt) 
						RSgImage::GetInterface()
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure the correct error code is returned when attempting
						to retrieve an invalid RSgImage extension interface.
@SYMTestActions			Initialise the graphics resource component. Create an image. 
						1. Declare an unsupported interface MFake and pass to GetInterface().
						2. Declare an interface MFakeNullUid which has a null uid and pass to GetInterface().
@SYMTestExpectedResults	1. GetInterface() should return KErrExtensionNotSupported.
						2. GetInterface() should return KErrArgument since null uids are not acceptable.
 */
void CTSgImageGeneric::TestGetInterfaceL()
	{
	TestOpenDriverL();
	
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageBitOpenVgImage;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	RSgImage image;	
	
	//fake interface
	MFake* smFake = reinterpret_cast<MFake*>(1);	
	MFake* smFake2 = reinterpret_cast<MFake*>(1);
	
	//Check Calls to Get Interface on Null Image Handle
	TEST(KErrBadHandle == image.GetInterface(smFake));
	TEST(KErrBadHandle == image.GetInterface(smFake2));
	TEST(!smFake);
	TEST(!smFake2);
	
	CheckErrorL(KErrNone, image.Create(info, NULL, 0), (TText8*)__FILE__, __LINE__);
	
	// check TSgDrawableId is not NULL
	TSgDrawableId id = image.Id();
	TEST(KSgNullDrawableId != id);

	smFake = reinterpret_cast<MFake*>(1);	
	smFake2 = reinterpret_cast<MFake*>(1);

	TEST(KErrExtensionNotSupported == image.GetInterface(smFake));
	TEST(!smFake);
	
	TEST(KErrExtensionNotSupported == image.GetInterface(smFake2));
	TEST(!smFake2);
	
	// null interface uid
	MFakeNullUid* smFake3 = NULL;
	TEST(KErrArgument == image.GetInterface(smFake3));
	TEST(!smFake3);

	image.Close();
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0024
@SYMTestCaseDesc		Gets supported pixel formats with invalid parameters.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::GetPixelFormats(TUint32, RArray<TInt>, 
						const TSgAttributeArrayBase*)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure the correct error messages are returned when user tries to
						call GetPixelFormats() with invalid parameters.
@SYMTestActions			Initialise the graphics resource component. Call RSgImage::GetPixelFormats with:
						1. Usage of 0
						2. Invalid attribute array
						3. Valid arguments
						4. Pixel formats array is not empty
@SYMTestExpectedResults	RSgImage::GetPixelFormats() should return:
						1. KErrArgument
						2. KErrNotSupported
						3. KErrNone
						4. KErrArgument
 */	
void CTSgImageGeneric::TestGetPixelFormatsInvalidL()
	{
	RArray<TInt> pixelFormats;
	CleanupClosePushL(pixelFormats);
	TSgAttributeArray<1> attribArray;
	attribArray[0].iUid = KNullUid;
	attribArray[0].iValue = 0;
	TEST(attribArray.Count() == 1);
	
	TestOpenDriverL();

	TEST(KErrArgument == RSgImage::GetPixelFormats(0, pixelFormats));
	pixelFormats.Reset();
		
	TEST(KErrNotSupported == RSgImage::GetPixelFormats(ESgUsageBitOpenVgImage, pixelFormats, &attribArray));
	pixelFormats.Reset();
	
	TEST(KErrNone == RSgImage::GetPixelFormats(ESgUsageBitOpenVgImage, pixelFormats));
	TEST(pixelFormats.Count() > 0);
	//Don't reset pixel formats for next test
	
	TEST(KErrArgument == RSgImage::GetPixelFormats(ESgUsageBitOpenVgImage, pixelFormats));
	pixelFormats.Reset();
	
	TestCloseDriver();
	CleanupStack::PopAndDestroy(&pixelFormats);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0025
@SYMTestCaseDesc		Opens an image with different invalid and valid parameters.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Open(const TSgDrawableId&)
@SYMTestPriority		Medium
@SYMTestType			UT
@SYMTestPurpose			Negative test to check correct error messages are returned when opening 
						image with different invalid parameters.
@SYMTestActions			Initialise the graphics resource component. Construct an RSgImage object. 
						Call the Open() function in both the current process and another process with:
						1. a non-null RSgImage handle
						2. null drawable id
						3. fake drawable id
						4. a valid RSgImage handle
						Do the same tests in a second thread and a second process.
@SYMTestExpectedResults	The function should return:
						1. KErrInUse
						2. KErrArgument
						3. KErrNotFound
						4. KErrNone
 */	
void CTSgImageGeneric::TestOpenImageInvalidL()
	{	
	TestOpenDriverL();
	//create a non-sharable image
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageBitOpenVgImage;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	RSgImage image;
	CheckErrorL(KErrNone, image.Create(info, KCrossImageData, KCrossImageDataStride), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(image);
	const TSgDrawableId id = image.Id();
	
	//Attempt to create the image again using the same object
	TEST(KErrInUse ==  image.Create(info, KCrossImageData, KCrossImageDataStride));
	
	//same thread
	//  non-empty handle
	RSgImage image1;
	CreateImageL(image1);
	CleanupClosePushL(image1);
	TEST(KErrInUse == image1.Open(id));
	image1.Close();
	
	//  non-existing drawable id
	TSgDrawableId fakeid = {0xFFFFFFFFFFFFFFFFU};
	CheckErrorL(KErrNotFound, image1.Open(fakeid), (TText8*)__FILE__, __LINE__);
	image1.Close();
	
	if (iEnableConformanceTests)
		{
		//different thread in the same process
		TSgThreadTestInfo threadInfo(id, info, ESgresSecondThreadOpenImageInvalid);
		TInt testResult = 0;
		TRAPD(err, testResult = CreateSecondThreadAndDoTestL(threadInfo));
		TEST(testResult >= 0);
		// Test the results from the second thread
		TEST(testResult & EFirstTestPassed);
		TEST(testResult & ESecondTestPassed);
		TEST(testResult & EThirdTestPassed);
		TEST(testResult & EFourthTestPassed);
		TEST(testResult & EFifthTestPassed);
		
		//different process	
		TSgProcessTestInfo processInfo(id, info, ESgresSecondProcessOpenImageInvalid);
		TRAP(err, testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo));
		TEST(testResult >= 0);
		TEST(testResult & EFirstTestPassed);	
		TEST(testResult & ESecondTestPassed);
		TEST(testResult & EThirdTestPassed);
		TEST(testResult & EFourthTestPassed);
		TEST(testResult & EFifthTestPassed);
		}
		
	CleanupStack::PopAndDestroy(2);
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0026
@SYMTestCaseDesc		Closes an image multiple times.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Close()
@SYMTestPriority		High
@SYMTestType			UT
@SYMTestPurpose			To check calling Close() multiple times after Open() does not cause
						errors or panics.
@SYMTestActions			Initialise the graphics resource component. Create an image and then
						call Close() three times on it.
@SYMTestExpectedResults	The function should not cause any errors or panics.
 */
void CTSgImageGeneric::TestCloseImageManyTimesL()
	{
	TestOpenDriverL();
	RSgImage image;
	CreateImageL(image);
	TEST(!image.IsNull());
	
	image.Close();
	TEST(image.IsNull());
	image.Close();
	TEST(image.IsNull());
	image.Close();
	TEST(image.IsNull());
	
	TestCloseDriver();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0027
@SYMTestCaseDesc		Closes an image object without opening it.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Close()
@SYMTestPriority		Medium
@SYMTestType			UT
@SYMTestPurpose			To check Close() does nothing if the image is not already opened.
@SYMTestActions			Initialise the graphics resource component. Declare an RSgImage object and
						call Close() on it.
@SYMTestExpectedResults	The function should not cause any errors or panics.
 */	
void CTSgImageGeneric::TestCloseImageWithoutOpenL()
	{	
	TestOpenDriverL();
	RSgImage image;
	TEST(image.IsNull());
	image.Close();
	TEST(image.IsNull());
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0028
@SYMTestCaseDesc		Creates an image with various invalid parameters.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt, const TSgAttributeArrayBase* = NULL)
						RSgImage::Create(const TSgImageInfo&, const RSgImage&, const TSgAttributeArrayBase* = NULL)
@SYMTestPriority		Medium
@SYMTestType			UT
@SYMTestPurpose			Negative test to check the function returns the correct error messages when 
						the parameters are invalid.
@SYMTestActions			Initialise the graphics resource component. Construct a TSgImageInfo object 
						and initialise the all the member variables with random values. Call the appropriate 
						Create() function with:
						1. invalid TSgImageInfo.iSizeInPixels
						2. unsupported TSgImageInfo Pixel Format
						3. the creating RSgImage handle is not null
						4. the aSgImage handle is null
						5. the size and pixel format in TSgImageInfo is not the same as that of aSgImage
						6. the pixel stride specified is 0 when bitmap data is given
@SYMTestExpectedResults	The function should return
						1. KErrArgument or KErrNotSupported for the overload taking an RSgImage
						2. KErrNotSupported
						3. KErrInUse
						4. KErrArgument
						5. KErrNotSupported
						6. KErrArgument
 */	
void CTSgImageGeneric::TestCreateImageInvalidL()
	{
	TestOpenDriverL();
	RSgImage image;
	
	RSgImage validImage;
	TSgImageInfo validInfo(TSize(8,8), EUidPixelFormatRGB_565, ESgUsageBitOpenVgImage);
	TEST(KErrNone == validImage.Create(validInfo, KCrossImageData, KCrossImageDataStride));
	
	//invalid info 1 - invalid width
	TSgImageInfo info1(TSize(-100,100), EUidPixelFormatRGB_565, ESgUsageBitOpenVgImage);
	TEST(KErrArgument == image.Create(info1, NULL, 0));
	image.Close();
	TEST(KErrArgument == image.Create(info1, KCrossImageData, KCrossImageDataStride));
	image.Close();
	//invalid height
	info1.iSizeInPixels = TSize(100, -100);
	TEST(KErrArgument == image.Create(info1, NULL, 0));
	image.Close();
	//Sizes don't match
	TEST(KErrNotSupported == image.Create(info1, validImage));
	image.Close();
	
	info1.iSizeInPixels = TSize(0,0);
    TEST(KErrArgument == image.Create(info1, NULL, 0));
    image.Close();
	TEST(KErrArgument == image.Create(info1, KCrossImageData, KCrossImageDataStride));
	image.Close();
	//Sizes don't match
	TEST(KErrNotSupported == image.Create(info1, validImage));
	image.Close();
	
	//Invalid usage
	info1.iUsage = KMaxTUint32;
	info1.iSizeInPixels = TSize(8,8);
	TEST(KErrNotSupported == image.Create(info1, KCrossImageData, KCrossImageDataStride));
	image.Close();
	TEST(KErrNotSupported == image.Create(info1, validImage));
	image.Close();
	
	//unsupported info 1 - invalid pixel format
	TSgImageInfo info2;
	info2.iSizeInPixels = TSize(8, 8);
	info2.iUsage = ESgUsageBitOpenVgImage;
	info2.iPixelFormat = EUidPixelFormatUnknown;
	TEST(KErrArgument == image.Create(info2, NULL, 0));
	image.Close();
	TEST(KErrArgument == image.Create(info2, KCrossImageData, KCrossImageDataStride));
	image.Close();
		
	//unsupported info 2 - large image size
	info2.iPixelFormat = ESgPixelFormatRGB_565;
	info2.iSizeInPixels = TSize(40000, 100);
	TEST(KErrTooBig == image.Create(info2, NULL, 0));
	info2.iSizeInPixels = TSize(100, 40000);
	TEST(KErrTooBig == image.Create(info2, NULL, 0));
	image.Close();
	
	//non-null handle
	CreateImageL(image);
	TSgImageInfo info3;
	info3.iSizeInPixels = TSize(8, 8);
	info3.iUsage = ESgUsageBitOpenVgImage;
	info3.iPixelFormat = ESgPixelFormatRGB_565;

	TInt ret = image.Create(info3, NULL, 16);	
	TEST(KErrInUse == ret);
	
	//non-null handle: create from an existing image
	RSgImage image1;
	CreateImageL(image1);
	TEST(KErrInUse == image1.Create(info3, image));
	image1.Close();
	
	//null existing image handle
	RSgImage image2;
	TEST(image2.IsNull());
	TEST(KErrArgument == image1.Create(info3, image2));
	
	//the size and pixel format in TSgImageInfo is not the same as that of aSgImage
	TSgImageInfo info4(TSize(100,100), ESgPixelFormatARGB_8888, ESgUsageBitOpenVgImage);
	TEST(KErrNotSupported == image1.Create(info4, image));
	
	image1.GetInfo(info3);
	info4.iSizeInPixels = info3.iSizeInPixels;
	info4.iPixelFormat = ESgPixelFormatXRGB_8888;
	TEST(KErrNotSupported == image1.Create(info4, image));
	
	image.Close();
	image1.Close();
	
	TSgImageInfo info5(TSize(8,8),  ESgPixelFormatRGB_565, ESgUsageBitOpenVgImage);
	
	TSgAttributeArray<1> attribArray;
	attribArray[0].iUid = KNullUid;
	attribArray[0].iValue = 0;
	
	TEST(KErrNotSupported == image1.Create(info5, KCrossImageData, KCrossImageDataStride, &attribArray));
	image1.Close();
	
	//Stride of 0
	TEST(KErrArgument == image1.Create(info5, KCrossImageData, 0));
	image1.Close();
	
	validImage.Close();
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0039
@SYMTestCaseDesc		Calls GetInfo() on an invalid image.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::GetInfo(TSgImageInfo&)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to check the correct error is returned when the client 
						tries to get information on an invalid image.
@SYMTestActions			Initialise the graphics resource component.
						Call GetInfo() on an image that is not initialised.
@SYMTestExpectedResults	GetInfo() should return KErrBadHandle
 */	
void CTSgImageGeneric::TestGetInfoImageInvalidL()
	{
	TestOpenDriverL();	
	RSgImage image;
	TSgImageInfo info(TSize(5,5), ESgPixelFormatARGB_8888, ESgUsageBitOpenVgImage);
	
	//uninitialised image
	TEST(KErrBadHandle == image.GetInfo(info));

	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0092
@SYMTestCaseDesc		Calls GetAttribute() on an invalid image.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::GetAttribute(TUid, TInt&)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to check the correct error is returned when the client
						tries to get attributes on an invalid image.
@SYMTestActions			Initialise the graphics resource component. 
						Call GetInfo on an image that is not initialised.
@SYMTestExpectedResults	GetAttribute() should return KErrBadHandle
 */	
void CTSgImageGeneric::TestGetAttributesImageInvalidL()
	{
	TestOpenDriverL();	
	RSgImage image;
	TSgImageInfo info(TSize(5,5), ESgPixelFormatARGB_8888, ESgUsageBitOpenVgImage);
	
	TUid uid = {0x12345678};
	TInt val = 0;
	TEST(KErrBadHandle == image.GetAttribute(uid, val));
	
	image.Close();
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0100
@SYMTestCaseDesc		Creates images using negative and invalid data strides.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(TUid, TInt&)
@SYMTestPriority		Medium
@SYMTestType			UT
@SYMTestPurpose			Positive test for using negative data strides.
						Negative test for using invalid data strides.
@SYMTestActions			Initialise the graphics resource component. 
						1. Create an image with a data stride that is smaller than the width 
						of the image.
						2. Create an image with a pointer to bitmap data, but 0 data stride.
						3. Create an image with a negative data stride equal to the width 
						of the image.
						4. Create an image with a negative data stride less than the width 
						of the image.
@SYMTestExpectedResults	1. Create() should return KErrArgument.
						2. Create() should return KErrArgument.
						3. Create() should successfully create the image and return KErrNone.
						4. Create() should return KErrArgument.
 */
void CTSgImageGeneric::TestCreateImageDataStrideL()
	{
	TestOpenDriverL();
	
	RSgImage image;
	TSgImageInfo info1(TSize(8,8), EUidPixelFormatRGB_565, ESgUsageBitOpenVgImage);

	TEST(KErrArgument == image.Create(info1, KCrossImageData, KCrossImageDataStride-1));
	image.Close();
	TEST(KErrArgument == image.Create(info1, KCrossImageData, 0));
	image.Close();
	
	TEST(KErrNone == image.Create(info1, KCrossImageData, -KCrossImageDataStride));
	image.Close();
	
	TEST(KErrArgument == image.Create(info1, KCrossImageData, -KCrossImageDataStride+1));
	image.Close();

	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0048
@SYMTestCaseDesc		RSgImage stress tests
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)
						RSgImage::Close() 
@SYMTestPriority		Medium
@SYMTestType			CT
@SYMTestPurpose			To ensure creating and destroying RSgImage multiple times work well.
@SYMTestActions			Create images 1000 times but only store KMaxImagesInArray of them 
						at one time in an RArray. When a new image is to be added to the 
						array, a random image from the array is removed.
@SYMTestExpectedResults	There should be no panics or leaves.
 */
void CTSgImageGeneric::TestStress1L()
	{
	TestOpenDriverL();
	TSgImageInfo info;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iSizeInPixels = TSize(100, 100);
	info.iUsage = ESgUsageBitOpenVgImage;
	
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
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Open(const TSgDrawableId&)
						RSgImage::Close() 
@SYMTestPriority		Medium
@SYMTestType			CT
@SYMTestPurpose			To ensure opening and destroying RSgImage multiple times and randomly
						in other threads work well.
@SYMTestActions			Create an image in the main thread. In a loop of 1000 times, randomly select
						one of the five threads and either open or close the image depending on the 
						state of the thread.
@SYMTestExpectedResults	There should be no panics or leaves.
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
	TSgThreadTestInfo threadInfo(id, info, ESgresMultipleThreadStressTest);
	
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
		User::LeaveIfError(thread.Create(threadName, SgTestSecondThread::ThreadStart, KDefaultStackSize, KSecondThreadMinHeapSize, KSecondThreadMaxHeapSize, &threadInfo));
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
	CleanupStack::PopAndDestroy(2, &image); // image, sem
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0051
@SYMTestCaseDesc		Calls RSgImage::GetInterface() with an invalid image handle
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::GetInterface(TUid, TAny*&)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling GetInterface() with an invalid image handle 
						will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call GetInterface() on the 
						second handle.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 3(ESgPanicBadDrawableHandle).
 */
void CTSgImageGeneric::TestPanicImageGetInterfaceInvalidHandleL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageGetInterfaceInvalidHandle);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageGetInterfaceInvalidHandleLThread");
	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0052
@SYMTestCaseDesc		Calls RSgImage::GetInterface() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::GetInterface(TUid, TAny*&)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling GetInterface() with a non-null handle and an 
						uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call GetInterface() on the second handle.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 1 (ESgPanicNoDriver).
 */
void CTSgImageGeneric::TestPanicImageGetInterfaceNoDriverL()
	{	
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageGetInterfaceNoDriver);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageGetInterfaceNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 1, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0053
@SYMTestCaseDesc		Calls RSgImage::Close() with an invalid image handle
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Close()
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling Close() with an invalid image handle will 
						cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call Close() on the second handle.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 3(ESgPanicBadDrawableHandle).
 */
void CTSgImageGeneric::TestPanicImageCloseInvalidHandleL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageCloseInvalidHandle);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCloseInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0054
@SYMTestCaseDesc		Calls RSgImage::Close() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Close()
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling Close() with a non-null handle and an 
						uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call Close() on the second handle.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 1 (ESgPanicNoDriver).
 */
void CTSgImageGeneric::TestPanicImageCloseNoDriverL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageCloseNoDriver);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCloseNoDriverL");
	CreateSecondThreadAndCheckPanicL(threadInfo, 1, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0055
@SYMTestCaseDesc		Calls RSgImage::Id() with an invalid image handle
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Id()
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling Id() with an invalid image handle will 
						cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call Id() on the second
						handle.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 3(ESgPanicBadDrawableHandle).
 */
void CTSgImageGeneric::TestPanicImageIdInvalidHandleL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageIdInvalidHandle);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageIdInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
 	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0056
@SYMTestCaseDesc		Calls RSgImage::Id() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Id()
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling Id() with a non-null handle and an 
						uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call Id() on the second handle.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 1 (ESgPanicNoDriver).
 */
void CTSgImageGeneric::TestPanicImageIdNoDriverL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageIdNoDriver);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageIdNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 1, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0057
@SYMTestCaseDesc		Calls RSgImage::Create() with an invalid image handle
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const RSgImage&)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling Create() with an invalid image handle will 
						cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call Create() on the second
						handle.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 3(ESgPanicBadDrawableHandle).
 */
void CTSgImageGeneric::TestPanicImageCreateInvalidHandleL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageCreateInvalidHandle);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCreateInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0058
@SYMTestCaseDesc		Calls RSgImage::GetInfo() with an invalid image handle
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::GetInfo(TSgImageInfo&)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling GetInfo() with an invalid image handle will 
						cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Call GetInfo() on the second
						handle.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 3(ESgPanicBadDrawableHandle).
 */
void CTSgImageGeneric::TestPanicImageGetInfoInvalidHandleL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageGetInfoInvalidHandle);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageGetInfoInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0059
@SYMTestCaseDesc		Calls RSgImage::GetInfo() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ2637
@SYMFssID				SgImage::GetInfo(TSgImageInfo&)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling GetInfo() with a non-null handle and an 
						uninitialised driver will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image in a second thread. 
						Declare another image and assign it to the current image handle. Close the current
						image so the second image handle becomes invalid. Close the graphics resource driver.
						Call GetInfo() on the second handle.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 1 (ESgPanicNoDriver).
 */
void CTSgImageGeneric::TestPanicImageGetInfoNoDriverL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageGetInfoNoDriver);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageGetInfoNoDriverL");
     CreateSecondThreadAndCheckPanicL(threadInfo, 1, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0082
@SYMTestCaseDesc		Calls RSgImage::Open() when the driver was not initialised.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Open(const TSgDrawableId&, TUint32)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling RSgImage::Open() with an uninitialised driver 
						will cause a panic.
@SYMTestActions			Do not initialise the graphics resource component and call RSgImage::Open() 
						in a second thread.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 1 (ESgPanicNoDriver).
 */
void CTSgImageGeneric::TestPanicImageOpenNoDriverL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageOpenNoDriver);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageOpenNoDriverL");
	CreateSecondThreadAndCheckPanicL(threadInfo, 1, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0083
@SYMTestCaseDesc		Creates an image from memory when the driver was not initialised.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const TAny*, TInt)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling RSgImage::Create() with an uninitialised 
						driver will cause a panic.
@SYMTestActions			Do not initialise the graphics resource component and call RSgImage::Open() 
						in a second thread.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 1 (ESgPanicNoDriver).
 */
void CTSgImageGeneric::TestPanicImageCreateNoDriver1L()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageCreateNoDriver1);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageCreateNoDriver1L");
	CreateSecondThreadAndCheckPanicL(threadInfo, 1, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0084
@SYMTestCaseDesc		Creates an image from an existing image when the driver was not initialised.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::Create(const TSgImageInfo&, const RSgImage&)
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling RSgImage::Create() with an uninitialised driver 
						will cause a panic.
@SYMTestActions			Do not initialise the graphics resource component and call RSgImage::Open() in 
						a second thread.
@SYMTestExpectedResults	Should panic in the second thread with panic code SGRES 1 (ESgPanicNoDriver).
 */
void CTSgImageGeneric::TestPanicImageCreateNoDriver2L()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageCreateNoDriver2);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageCreateNoDriver2L");
	CreateSecondThreadAndCheckPanicL(threadInfo, 1, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0088
@SYMTestCaseDesc		Calls RSgImage::DrawableType() when the driver was not initialised.
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::DrawableType()
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling RSgImage::DrawableType() with an uninitialised
						driver will cause a panic.
@SYMTestActions			Do not initialise the graphics resource component and call RSgImage::DrawableType()
						in a second thread.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 1 (ESgPanicNoDriver).
 */
void CTSgImageGeneric::TestPanicImageDrawableTypeNoDriverL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageDrawableTypeNoDriver);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageOpenNoDriverL");
	CreateSecondThreadAndCheckPanicL(threadInfo, 1, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0089
@SYMTestCaseDesc		Calls RSgImage::DrawableType() with an invalid image handle
@SYMPREQ				PREQ2637
@SYMFssID				RSgImage::DrawableType()
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			Negative test to ensure calling RSgImage::DrawableType() with an uninitialised 
						driver will cause a panic.
@SYMTestActions			Do not initialise the graphics resource component and call RSgImage::DrawableType()
						in a second thread.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 3 (ESgPanicBadDrawableHandle).
 */
void CTSgImageGeneric::TestPanicImageDrawableTypeInvalidHandleL()
	{
	TSgImageInfo info(TSize(),0,0);
	TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageDrawableTypeInvalidHandle);
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageOpenNoDriverL");
	CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID          GRAPHICS-RESOURCE-0090
@SYMTestCaseDesc        Calls RSgImage::DrawableType() with an invalid image handle
@SYMPREQ                PREQ2637
@SYMFssID               RSgImage::GetAttribute()
@SYMTestPriority        Low
@SYMTestType            UT
@SYMTestPurpose         Negative test to ensure calling RSgImage::GetAttribute() with an invalid handle
						will cause a panic.
@SYMTestActions         Initialise the graphics resource component and call RSgImage::GetAttribute()
						in a second thread with an invalid handle.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 3 (ESgPanicBadDrawableHandle).
 */
void CTSgImageGeneric::TestPanicImageGetAttributeInvalidHandleL()
    {
    TSgImageInfo info(TSize(),0,0);
    TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageGetAttributeInvalidHandle);
    TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
    _LIT(KTestName, "TestPanicImageGetAttributeInvalidHandleL");
    CreateSecondThreadAndCheckPanicL(threadInfo, 3, exitCategoryName, KTestName);
    }

/**
@SYMTestCaseID          GRAPHICS-RESOURCE-0091
@SYMTestCaseDesc        Calls RSgImage::GetAttribute() with no open driver
@SYMPREQ                PREQ2637
@SYMFssID               RSgImage::GetAttribute()
@SYMTestPriority        Low
@SYMTestType            UT
@SYMTestPurpose         Negative test to ensure calling RSgImage::GetAttribute() with an uninitialised
						driver will cause a panic.
@SYMTestActions         Do not initialise the graphics resource component and call RSgImage::GetAttribute()
						in a second thread.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 1 (ESgPanicNoDriver).
 */
void CTSgImageGeneric::TestPanicImageGetAttributeNoDriverL()
    {
    TSgImageInfo info(TSize(),0,0);
    TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicImageGetAttributeNoDriver);
    TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
    _LIT(KTestName, "TestPanicImageGetAttributeNoDriverL");
    CreateSecondThreadAndCheckPanicL(threadInfo, 1, exitCategoryName, KTestName);
    }

/**
@SYMTestCaseID          GRAPHICS-RESOURCE-0097
@SYMTestCaseDesc        Access out of bound TSgAttibuteArray index
@SYMPREQ                PREQ2637
@SYMFssID               TSgAttributeArray
@SYMTestPriority        Low
@SYMTestType            UT
@SYMTestPurpose         Negative test to ensure calling an out of bound index on a TSgAttributeArray 
						will cause a panic.
@SYMTestActions         Create a TSgAttributeArray of size 5 and attempt to access an element higher 
						than the size.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 4 (ESgPanicBadAttributeArrayIndex).
 */
void CTSgImageGeneric::TestPanicImageAttributeArrayInvalidIndexL()
    {
    TSgImageInfo info(TSize(),0,0);
    TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicAttributeArrayInvalidIndex);
    TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
    _LIT(KTestName, "AttributeArrayInvalidIndexL");
    CreateSecondThreadAndCheckPanicL(threadInfo, 4, exitCategoryName, KTestName);
    }

/**
@SYMTestCaseID          GRAPHICS-RESOURCE-0098
@SYMTestCaseDesc        Access out of bound TSgAttibuteArray index 
@SYMPREQ                PREQ2637
@SYMFssID               TSgAttributeArray        
@SYMTestPriority        Low
@SYMTestType            UT
@SYMTestPurpose         Negative test to ensure calling an out of bound index on a TSgAttributeArray
						will cause a panic.
@SYMTestActions         Create a TSgAttributeArray of size 5 and attempt to access an element higher
						than the size.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 4 (ESgPanicBadAttributeArrayIndex).
 */
void CTSgImageGeneric::TestPanicImageAttributeArrayInvalidIndex2L()
    {
    TSgImageInfo info(TSize(),0,0);
    TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadPanicAttributeArrayInvalidIndex2);
    TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
    _LIT(KTestName, "AttributeArrayInvalidIndexL");
    CreateSecondThreadAndCheckPanicL(threadInfo, 4, exitCategoryName, KTestName);
    }

/**
@SYMTestCaseID          GRAPHICS-RESOURCE-0101
@SYMTestCaseDesc        Close an RSgDriver with open resources
@SYMPREQ                PREQ2637
@SYMFssID				RSgDriver       
@SYMTestPriority        Low
@SYMTestType            UT
@SYMTestPurpose         Negative test to ensure closing a driver with open resources will cause a panic.
@SYMTestActions         Create an RSgImage on an open driver, then close the driver.
@SYMTestExpectedResults Should panic in the second thread with panic code SGRES 2 (ESgPanicUnclosedResources).
 */
void CTSgImageGeneric::TestPanicDriverCloseOpenResourcesL()
    {
    TSgImageInfo info(TSize(),0,0);
    TSgProcessTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondProcessPanicDriverCloseOpenResources);
    TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
    CreateSecondProcessAndCheckPanicL(threadInfo, 2, exitCategoryName, KSecondProcess);
    }

/**
@SYMTestCaseID          GRAPHICS-RESOURCE-0102
@SYMTestCaseDesc        Open/Close RSgDriver and Create/Close RSgImage in separate threads
@SYMPREQ                PREQ2637
@SYMFssID				RSgDriver, RSgImage      
@SYMTestPriority        High
@SYMTestType            UT
@SYMTestPurpose         To ensure that it is possible to Open/Close the RSgDriver from
						a separate thread, and to ensure that it is possible to Create/Open/Close 
						RSgImages from a seperate thread.
@SYMTestActions         1. Declare a RSgDriver, and pass to another thread to open.
						2. Declare a RSgImage, and pass to another thread to create.
						3. Pass created RSgImage to another thread to Open().
						4. Pass the image to another thread to Close().
						5. Pass the RSgDriver to another thread to Close()
@SYMTestExpectedResults All steps to succeed with no errors reported.
 */
void CTSgImageGeneric::TestCreateOpenCloseImageAndDriverInSecondThreadL()
    {
    TSgImageInfo info(TSize(),0,0);
    TSgThreadTestInfo threadInfo(KSgNullDrawableId, info, ESgresSecondThreadOpenPassedDriver);
    
    RSgDriver driver;
    threadInfo.iSgDriver = &driver;
    
    RSgImage image;
    threadInfo.iSgImage = &image;
    
    //Open the driver in another thread
    TEST(CreateSecondThreadAndDoTestL(threadInfo)==0);
   
    //Create the image in a thread
    threadInfo.iTestCase = ESgresSecondThreadCreatePassedImage;
    TEST(CreateSecondThreadAndDoTestL(threadInfo)==0);    
    
    //Now try opening the image in another thread
    RSgImage image2;
    threadInfo.iTestCase = ESgresSecondThreadOpenPassedImage;
    threadInfo.iDrawableId = image.Id();
    threadInfo.iSgImage = &image2;
    TEST(CreateSecondThreadAndDoTestL(threadInfo)==0);   
    
    //Close the "Opened" image in another thread
    threadInfo.iTestCase = ESgresSecondThreadClosePassedImage;
    TEST(CreateSecondThreadAndDoTestL(threadInfo)==0);
    
    //Close the "Created" image in another thread
    threadInfo.iTestCase = ESgresSecondThreadClosePassedImage;
    threadInfo.iSgImage = &image;
    TEST(CreateSecondThreadAndDoTestL(threadInfo)==0);
    
    //Close the driver in another thread
    threadInfo.iTestCase = ESgresSecondThreadClosePassedDriver;
    TEST(CreateSecondThreadAndDoTestL(threadInfo)==0);
    }

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0104
@SYMTestCaseDesc		Open a RSgImage using multiple threads.
@SYMDEF					ou1cimx1#197357
@SYMFssID				RSgImage      
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			To show that images can be opened safely when used with multiple threads in 
						separate processes.
@SYMTestActions			1. Process A, creates an RSgImage
						2. Process B, creates a secondary thread, which creates a handle to the image
						created in A.
						3. Process B, primary thread opens a handle to the same image.
						4. Process B, secondary thread closes the handle and terminates thread.
						5. Process B, primary thread waits for thread cleanup, then attempts to 
						access the data of the image by creating a copy of it.
@SYMTestExpectedResults All steps to succeed with no errors reported.
 */
void CTSgImageGeneric::TestOpenImageMultithreadedL()
	{
	TestOpenDriverL();
	RSgImage image;
	CreateImageL(image);
	CleanupClosePushL(image);
	
	TSgImageInfo info;
	TInt err = image.GetInfo(info);
	TESTE(err == KErrNone, err);
	
	TSgProcessTestInfo processInfo(image.Id(), info, ESgresSecondProcessOpenImageMultithreaded);
	TInt testResult = 0;
	TRAP(err, testResult = CreateSecondProcessAndDoTestL(KSecondProcess, processInfo));
	TESTE(err == KErrNone, err);
	TEST(testResult & EFirstTestPassed);
	TEST(testResult & ESecondTestPassed);
	TEST(testResult <= (EFirstTestPassed | ESecondTestPassed));

	CleanupStack::PopAndDestroy(1); // image
	TestCloseDriver();	
	}
