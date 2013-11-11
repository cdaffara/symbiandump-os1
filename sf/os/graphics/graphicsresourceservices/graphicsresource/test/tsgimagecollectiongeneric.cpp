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

#include "tsgimagecollectiongeneric.h"

CTSgImageCollectionGeneric::CTSgImageCollectionGeneric()
	{
	INFO_PRINTF1(_L("Graphics resource component test - RSgImageCollection Generic Tests.\r\n"));
	}

CTSgImageCollectionGeneric::~CTSgImageCollectionGeneric()
	{
	}

/** 
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTSgImageCollectionGeneric::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-RESOURCE-0015"));
	INFO_PRINTF1(_L("Creation of image collections.\r\n"));
	TestCreateImageCollectionL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0016"));
	INFO_PRINTF1(_L("Getting surface id of the image collection.\r\n"));
	TestGetImageCollectionSurfaceIdL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0017"));
	INFO_PRINTF1(_L("Getting the number of images in an image collection.\r\n"));
	TestGetCollectionImageNumberL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0018"));
	INFO_PRINTF1(_L("Getting information about the images in an image collection.\r\n"));
	TestGetImageInfoInCollectionL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0019"));
	INFO_PRINTF1(_L("Getting image at a specific location in an image collection.\r\n"));
	TestOpenImageInCollectionL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0020"));
	INFO_PRINTF1(_L("Mapping / Unmapping images in a collection.\r\n"));
	TestMapImageInCollectionL();
	RecordTestResultL();
		
	SetTestStepID(_L("GRAPHICS-RESOURCE-0021"));
	INFO_PRINTF1(_L("Creation of multiple image collections.\r\n"));
	TestCreateMultipleImageCollectionL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0034"));
	INFO_PRINTF1(_L("Creation of the image collection with invalid parameters\r\n"));
	TestCreateImageCollectionInvalidL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0035"));
	INFO_PRINTF1(_L("Creation of multiple image collections with invalid parameters.\r\n"));
	TestCreateMultipleImageCollectionsInvalidL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0036"));
	INFO_PRINTF1(_L("Getting information about the images in an image collection under various invalid conditions.\r\n"));
	TestGetImageCollectionInfoInvalidL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0037"));
	INFO_PRINTF1(_L("Opening images from a collection under various invalid conditions.\r\n"));
	TestOpenImageInCollectionInvalidL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0038"));
	INFO_PRINTF1(_L("Invalid mapping/unmapping tests on individual images in a collection\r\n"));
	TestMapUnmapImagesInCollectionInvalidL();
	RecordTestResultL();
	
#ifdef _DEBUG
/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0094
*/
	SetTestStepID(_L("GRAPHICS-RESOURCE-0094"));
	INFO_PRINTF1(_L("RSgImageCollection generic out of memory tests.\r\n"));
	TestOOM();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0068"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - Close() with invalid imagecollection handle\r\n"));
	TestPanicImageCollectionCloseInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0069"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - Close() with non null imagecollection handle and uninitialised driver\r\n"));
	TestPanicImageCollectionCloseNoDriverL();
    RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0070"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - SurfaceId() with invalid imagecollection handle\r\n"));
	TestPanicImageCollectionSurfaceIdInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0071"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - SurfaceId() with non null imagecollection handle and uninitialised driver\r\n"));
	TestPanicImageCollectionSurfaceIdNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0072"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - GetInfo() with invalid imagecollection handle\r\n"));
	TestPanicImageCollectionGetInfoInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0073"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - GetInfo() with non null imagecollection handle and uninitialised driver\r\n"));
	TestPanicImageCollectionGetInfoNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0074"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - Count() with invalid imagecollection handle\r\n"));
	TestPanicImageCollectionCountInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0075"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - Count() with non null imagecollection handle and uninitialised driver\r\n"));
	TestPanicImageCollectionCountNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0076"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - OpenImage() with invalid imagecollection handle\r\n"));
	TestPanicImageCollectionOpenImageInvalidHandleL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0077"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - OpenImage() with non null imagecollection handle and uninitialised driver\r\n"));
	TestPanicImageCollectionOpenImageNoDriverL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0086"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - Create() a single image collection with an uninitialised driver\r\n"));
	TestPanicImageCollectionCreateNoDriver1L();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0086"));
	INFO_PRINTF1(_L("RSgImageCollection generic panic test - Create() multiple image collections with an uninitialised driver\r\n"));
	TestPanicImageCollectionCreateNoDriver2L();
	RecordTestResultL();
#else
	INFO_PRINTF1(_L("Warning: Skipping the panic tests. \r\n"));
#endif	
	return TestStepResult();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0015
@SYMTestCaseDesc		Creates an image collection.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234 
@SYMFssID				RSgImageCollection::Create(const TSgImageInfo&, TInt)\n 
						RSgImageCollection::Close()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the image collection can be created.
@SYMTestActions			Initialise the graphics resource component. Construct a TSgImageInfo object
						and Initialise all the member variables. Call Create() on the RSgImageCollection
						with the image info and number of images.
@SYMTestExpectedResults	The function should return KErrNone and memory for the image objects
						are successfully allocated.
@SYMTestStatus			Implemented 
 */	
void CTSgImageCollectionGeneric::TestCreateImageCollectionL()
	{	
	TestOpenDriverL();	
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiTarget;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	RSgImageCollection collection;
	TEST(collection.IsNull());
	
	CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	TEST(!collection.IsNull());
	TEST(1 == SgDriver::ResourceCount());
	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0016
@SYMTestCaseDesc		Gets the surface id of the image collection.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9215
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImageCollection::SurfaceId()\n
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the surface id of the image collection can be retrieved successfully.
@SYMTestActions			Initialise the graphics resource component. Create an image collection and call
						SurfaceId() on it.
@SYMTestExpectedResults	A non-null surface id should be returned.
@SYMTestStatus			Implemented 
 */	
void CTSgImageCollectionGeneric::TestGetImageCollectionSurfaceIdL()
	{
	const TSurfaceId KNullSurfaceId = {0, 0, 0, 0};
	RSgImageCollection collection;
	TSurfaceId surfaceid = collection.SurfaceId();
	TEST(surfaceid == KNullSurfaceId);
	TestOpenDriverL();	
	CreateImageCollectionL(collection);
	CleanupClosePushL(collection);	
	surfaceid = collection.SurfaceId();
	TEST(!surfaceid.IsNull());

	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0017
@SYMTestCaseDesc		Gets the number of images in an image collection.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImageCollection::Count())\n
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the function returns the correct number of images.
@SYMTestActions			Initialise the graphics resource component. Declare an image collection. Call Count().
						Create an image collection. Call Count() again.
@SYMTestExpectedResults	First call should return zero. Second call should return the same value 
						as the one passed to the function at creation time.
@SYMTestStatus			Implemented 
 */	
void CTSgImageCollectionGeneric::TestGetCollectionImageNumberL()
	{	
	RSgImageCollection collection;
	TInt number = collection.Count();
	TEST(0 == number);
	TestOpenDriverL();	
	CreateImageCollectionL(collection);
	CleanupClosePushL(collection);	
	number = collection.Count();
	TEST(KNumImagesInCollection == number);
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0018
@SYMTestCaseDesc		Calls GetInfo() on an image collection.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImageCollection::GetInfo(TSgImageInfo&)\n
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure the information about the images in a collection can be correctly returned.
@SYMTestActions			Initialise the graphics resource component. Construct a TSgImageInfo object and
						Initialise all the member variables. Create an image collection. Call GetInfo() and
						compare the returned TSgImageInfo object with the original one.
@SYMTestExpectedResults	The function should return KErrNone and the returned information should be the
						same as the old one.
@SYMTestStatus			Implemented 
 */	
void CTSgImageCollectionGeneric::TestGetImageInfoInCollectionL()
	{
	TestOpenDriverL();
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiTarget;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	RSgImageCollection collection;
	CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	TEST(!collection.IsNull());
	
	TSgImageInfo info2;
	TEST(KErrNone == collection.GetInfo(info2));
	TEST(CompareInfos(info, info2));
	CleanupStack::PopAndDestroy();
	
	//get info with user attributes
	TSgUserAttribute testAttributes[2] = {{0x00008888, 1}, {0x00009999, 2}};
	info.iUserAttributes = testAttributes;
	info.iUserAttributeCount = 2;
	CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	TSgImageInfo info3;
	TSgUserAttribute testAttributes2[2] = {{0x00008888, 0}, {0x00009999, 0}};
	info3.iUserAttributes = testAttributes2;
	info3.iUserAttributeCount = 2;
	TEST(KErrNone == collection.GetInfo(info3));
	TEST(CompareInfos(info, info3));
	CleanupStack::PopAndDestroy();	
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0019
@SYMTestCaseDesc		Gets image at a specific location in an image collection.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImageCollection::OpenImage()\n
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure an image at a specific location can be returned.
@SYMTestActions			Initialise the graphics resource component. Create the image collection. Call OpenImage() to 
						return all the images in order. 
@SYMTestExpectedResults	All the images are returned successfully.
@SYMTestStatus			Implemented 
 */	
void CTSgImageCollectionGeneric::TestOpenImageInCollectionL()
	{	
	TestOpenDriverL();
	RSgImageCollection collection;
	CreateImageCollectionL(collection);
	CleanupClosePushL(collection);
	
	TSgImageInfo info1;
	collection.GetInfo(info1);
	
	for(TInt i=0; i<KNumImagesInCollection; ++i)
		{
		RSgImage image;
		CheckErrorL(KErrNone, collection.OpenImage(i, image), (TText8*)__FILE__, __LINE__);
		TSgImageInfo info2;
		CheckErrorL(KErrNone, image.GetInfo(info2), (TText8*)__FILE__, __LINE__);
		TEST(CompareInfos(info1, info2));
		image.Close();
		}
	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0020
@SYMTestCaseDesc		Maps and unmaps images in a collection.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9193
@SYMREQ					REQ9219
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImageCollection::OpenImage()\n 
						RSgImage::MapReadOnly(const TAny*&, TInt&) const\n
 						RSgImage::MapWriteOnly(TAny*&, TInt&)\n 
						RSgImage::MapReadWrite(TAny*&, TInt&)\n 
						RSgImage::Unmap ()\n
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure individual images in an image collection can be mapped and unmapped.
@SYMTestActions			Initialise the graphics resource component. Create the image collection. Use the 
						index operator to return all the images. Call MapXXX() and Unmap() functions on each 
						of them.
@SYMTestExpectedResults	All the functions should return KErrNone.
@SYMTestStatus			Implemented 
 */	
void CTSgImageCollectionGeneric::TestMapImageInCollectionL()
	{
	TestOpenDriverL();
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	
	RSgImageCollection collection;
	CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	TEST(!collection.IsNull());
	
	TAny* dataAddress;
	TInt dataStride;
	
	for(TInt i=0; i<collection.Count(); ++i)
		{
		RSgImage image;
		CheckErrorL(KErrNone, collection.OpenImage(i, image), (TText8*)__FILE__, __LINE__);
		TSgImageInfo imageInfo;
		CheckErrorL(KErrNone, image.GetInfo(imageInfo), (TText8*)__FILE__, __LINE__);
		
		//test MapReadWrite
		TInt mapResult = image.MapReadWrite(dataAddress, dataStride);
		TEST(KErrNone == mapResult);
		
		if(KErrNone == mapResult)
			{
			//modify the image, set first line to blue
			for(TInt x = 0; x < imageInfo.iSizeInPixels.iWidth; ++x)
				{
				*(TUint16*)PtrAdd(dataAddress, sizeof(TUint16) * x) = 0x1F;
				}
			
			//check the modification
			for(TInt x = 0; x < imageInfo.iSizeInPixels.iWidth; ++x)
				{
				TUint16 pixelValue = *(TUint16*)PtrAdd(dataAddress, sizeof(TUint16) * x);
				if(pixelValue != 0x1F)
					{
					TEST(EFalse);
					break;
					}
				}
			}		
		TEST(KErrNone == image.Unmap());	
		image.Close();		
		}
	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0021
@SYMTestCaseDesc		Creates multiple image collections.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImageCollection::Create(const TSgImageInfo&, TInt, RSgImageCollection*, TInt) \n
						RSgImageCollection::Close()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure multiple image collections can be created using the factory function.
@SYMTestActions			Initialise the graphics resource component. Construct multiple TSgImageInfo objects
						and Initialise all the member variables. Call the static Create() function.
@SYMTestExpectedResults	The function should return KErrNone and memory for the image objects
						are successfully allocated.
@SYMTestStatus			Implemented 
 */	
void CTSgImageCollectionGeneric::TestCreateMultipleImageCollectionL()
	{
	TestOpenDriverL();
	
	TSgImageInfo infos[3];
	infos[0].iCpuAccess = ESgCpuAccessReadWrite;
	infos[0].iPixelFormat = EUidPixelFormatXRGB_8888;
	infos[0].iSizeInPixels = TSize(50, 50);
	infos[0].iUsage = ESgUsageDirectGdiSource;
	
	infos[1].iCpuAccess = ESgCpuAccessNone;
	infos[1].iPixelFormat = EUidPixelFormatRGB_565;
	infos[1].iSizeInPixels = TSize(120, 120);
	infos[1].iUsage = ESgUsageDirectGdiTarget|ESgUsageCompositionSource;
	
	infos[2].iCpuAccess = ESgCpuAccessNone;
	infos[2].iPixelFormat = EUidPixelFormatARGB_8888_PRE;
	infos[2].iSizeInPixels = TSize(200, 200);
	infos[2].iUsage = ESgUsageDirectGdiTarget|ESgUsageCompositionSource;
	
	RSgImageCollection collections[3];
	CheckErrorL(KErrNone, RSgImageCollection::Create(infos, KNumImagesInCollection, collections, 3), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collections[0]);
	CleanupClosePushL(collections[1]);
	CleanupClosePushL(collections[2]);
	
	for(TInt i=0; i<3; ++i)
		{
		RSgImageCollection& collection = collections[i];
		TEST(!collection.IsNull());
		
		TSurfaceId surfaceid = collection.SurfaceId();
		TEST(!surfaceid.IsNull());
		
		TSgImageInfo info;
		TEST(KErrNone == collection.GetInfo(info));
		TSgImageInfo oldInfo = infos[i];
		
		TEST(CompareInfos(oldInfo, info));
		
		TInt imagecount = collection.Count();
		TEST(imagecount == KNumImagesInCollection);

		for (TInt j = 0; j < KNumImagesInCollection; ++j)
			{
			RSgImage image;
			CheckErrorL(KErrNone, collection.OpenImage(j, image), (TText8*)__FILE__, __LINE__);
			TEST(KErrNone == image.GetInfo(info));
			TEST(CompareInfos(oldInfo, info));
			image.Close();
			}
		}
	
	CleanupStack::PopAndDestroy(3);
	TestCloseDriver();
	}


/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0034
@SYMTestCaseDesc		Creates an image collection with invalid parameters
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImageCollection::Create(const TSgImageInfo, TInt)\n
						RSgImageCollection::Close()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check for different creation errors.
@SYMTestActions			Initialise the graphics resource component. Call Create():\n
 						\t 1. on a non-null RSgImageCollection object\n
 						\t 2. with an invalid TSgImageInfo\n
 						\t 3. with a negative or zero image count\n
						\t 4. with an unsupported TSgImageInfo\n
						\t 5. with an TSgImageInfo with a size info bigger than the system memory\n
						\t 6. with number of iUserAttributes more than KMaxHint(8)\n
@SYMTestExpectedResults	The function should return\n
						\t 1. KErrInUse\n
						\t 2. KErrArgument\n
						\t 3. KErrArgument\n
						\t 4. KErrNotSupported\n
						\t 5. KErrNoMemory\n
						\t 6. KErrOverflow
@SYMTestStatus			Implemented 
 */	
void CTSgImageCollectionGeneric::TestCreateImageCollectionInvalidL()
	{
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	
	RSgImageCollection collection;
	TestOpenDriverL();
	//non-empty handle
	CreateImageCollectionL(collection);
	CleanupClosePushL(collection);
	TEST(KErrInUse == collection.Create(info, KNumImagesInCollection));
	collection.Close();
	
	//invalid TSgImageInfo
	TSgImageInfo info1;
	info1.iSizeInPixels = TSize(-100, 100);
	info1.iPixelFormat = EUidPixelFormatRGB_565;
	info1.iCpuAccess = ESgCpuAccessReadWrite;
	
	TEST(KErrArgument == collection.Create(info1, KNumImagesInCollection));
	collection.Close();
	
	//negative or zero image count
	
	TEST(KErrArgument == collection.Create(info, 0));
	collection.Close();
	
	TEST(KErrArgument == collection.Create(info, -1));
	collection.Close();
	
	//unsupported TSgImageInfo
	TSgImageInfo info2;
	info2.iCpuAccess = ESgCpuAccessReadWrite;
	info2.iPixelFormat = EUidPixelFormatAP_88;
	info2.iSizeInPixels = TSize(10, 10);
	info2.iUsage = ESgUsageDirectGdiSource;
	
	TEST(KErrNotSupported == collection.Create(info2, KNumImagesInCollection));
	collection.Close();
	
	//number of iUserAttributes more than KMaxHint(8)
	TSgImageInfo info3;
	info3.iSizeInPixels = TSize(100, 100);
	info3.iPixelFormat = EUidPixelFormatRGB_565;
	info3.iCpuAccess = ESgCpuAccessNone;
	info3.iUsage = ESgUsageDirectGdiTarget;
	info3.iUserAttributes = new TSgUserAttribute[100];
	CleanupArrayDeletePushL(info3.iUserAttributes);
	info3.iUserAttributeCount = 100;
	TInt ret = collection.Create(info3, KNumImagesInCollection);
	if(iRunningOomTests)
		{
		TEST(KErrNoMemory == ret || KErrOverflow == ret);
		}
	else
		{
		TEST(KErrOverflow == ret);
		}
	
	CleanupStack::PopAndDestroy(2);
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0035
@SYMTestCaseDesc		Creates multiple image collections with invalid parameters.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9217
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImageCollection::Create(const TSgImageInfo[], TInt, RSgImageCollection[], TInt)\n
						RSgImageCollection::Close()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check for different creation errors when creating multiple image collections.
@SYMTestActions			Initialise the graphics resource component. Call the static Create():\n
						\t 1. on an array of RSgImageCollection objects with at least one non-null 
						RSgImageCollection object\n
						\t 2. with an array of TSgImageInfo objects with at least one invalid TSgImageInfo\n
						\t 3. with a negative or zero image count\n
						\t 4. with a negative or zero image collection count\n
						\t 5. with an array of TSgImageInfo objects with at least one unsupported TSgImageInfo\n
						\t 6. with an array of TSgImageInfo objects with at least one requesting immutable images\n
@SYMTestExpectedResults	The function should return\n
						\t 1. KErrInUse\n
						\t 2. KErrArgument\n
						\t 3. KErrArgument\n
						\t 4. KErrArgument\n
						\t 5. KErrNotSupported\n
						\t 6. KErrNotSupported
@SYMTestStatus			Partially Implemented 
 */
void CTSgImageCollectionGeneric::TestCreateMultipleImageCollectionsInvalidL()
	{
	//TSgImageInfo array
	TSgImageInfo info1;
	info1.iCpuAccess = ESgCpuAccessNone;
	info1.iPixelFormat = EUidPixelFormatXRGB_8888;
	info1.iSizeInPixels = TSize(50, 50);
	info1.iUsage = ESgUsageDirectGdiSource;
	
	TSgImageInfo info2;
	info2.iCpuAccess = ESgCpuAccessReadOnly;
	info2.iPixelFormat = EUidPixelFormatRGB_565;
	info2.iSizeInPixels = TSize(120, 120);
	info2.iUsage = ESgUsageScreenSource;
	
	TSgImageInfo info3;
	info3.iCpuAccess = ESgCpuAccessReadWrite;
	info3.iPixelFormat = EUidPixelFormatARGB_8888_PRE;
	info3.iSizeInPixels = TSize(200, 200);
	info3.iUsage = ESgUsageDirectGdiTarget;
	
	TSgImageInfo infos[] = {info1, info2, info3};
	
	//RSgImageCollection array
	const TInt numberOfCollections = 3;
	RSgImageCollection collections[numberOfCollections];
	
	TestOpenDriverL();
	//create with one of the image info being invalid
	infos[0].iSizeInPixels = TSize(-50, 50);
	TEST(KErrArgument == RSgImageCollection::Create(infos, KNumImagesInCollection, collections, numberOfCollections));
	//reset
	infos[0].iSizeInPixels = TSize(50, 50);
	
	//create with one of the image collection array element being non-null
	CreateImageCollectionL(collections[0]);
	CleanupClosePushL(collections[0]);
	TEST(KErrInUse == RSgImageCollection::Create(infos, KNumImagesInCollection, collections, numberOfCollections));
	//rest
	CleanupStack::PopAndDestroy();
	
	//create with a negative or zero image count
	TEST(KErrArgument == RSgImageCollection::Create(infos, 0, collections, numberOfCollections));
	TEST(KErrArgument == RSgImageCollection::Create(infos, -1, collections, numberOfCollections));
	
	//create with a negative or zero image collection count
	TEST(KErrArgument == RSgImageCollection::Create(infos, KNumImagesInCollection, collections, 0));
	TEST(KErrArgument == RSgImageCollection::Create(infos, KNumImagesInCollection, collections, -1));
	
	//create with one of the image info being unsupported
	infos[0].iPixelFormat = EUidPixelFormatAP_88;
	TEST(KErrNotSupported == RSgImageCollection::Create(infos, KNumImagesInCollection, collections, numberOfCollections));
	//reset
	infos[0].iPixelFormat = EUidPixelFormatXRGB_8888;
	
	//create with some of the image info objects specifying constant images
	TEST(KErrNotSupported == RSgImageCollection::Create(infos, KNumImagesInCollection, collections, numberOfCollections));
	
	//close all collections
	for(TInt i=0; i<numberOfCollections; ++i)
		{
		collections[i].Close();
		}
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0036
@SYMTestCaseDesc		Calls GetInfo() on an image collection under various invalid conditions.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImageCollection::GetInfo(TSgImageInfo&)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check the correct error messages are returned when GetInfo() is called 
						under various invalid conditions.
@SYMTestActions			Initialise the graphics resource component. Call GetInfo() on an image collection\n
						\t 1. that is uninitialised\n
						\t 2. TSgImageInfo has different user attributes Ids from the existing Ids.
@SYMTestExpectedResults	The function should return:\n
						\t 1. KErrBadHandle\n
						\t 2. KErrNotFound or KErrArgument
@SYMTestStatus			Implemented 
 */	
void CTSgImageCollectionGeneric::TestGetImageCollectionInfoInvalidL()
	{
	TestOpenDriverL();
	//uninitialised collection
	RSgImageCollection collection;
	TSgImageInfo info;
	TEST(KErrBadHandle == collection.GetInfo(info));
	
	//TSgImageInfo has different user attributes Ids from the existing Ids.
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiTarget;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	TSgUserAttribute testAttributes[] = {{0x00008888, 1}, {0x00009999, 2}};
	info.iUserAttributes = testAttributes;
	info.iUserAttributeCount = 2;
	CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	TSgImageInfo info3;
	TSgUserAttribute testAttributes2[1];
	info3.iUserAttributes = testAttributes2;
	info3.iUserAttributes[0].iUid = TUid::Uid(0x11111111);
	info3.iUserAttributeCount = 1;
	TInt result = collection.GetInfo(info3);
	TEST(KErrNotFound == result || KErrArgument == result);
	CleanupStack::PopAndDestroy();	
	
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0037
@SYMTestCaseDesc		Opens images from a collection under various invalid conditions.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				RSgImageCollection::OpenImage(TInt aIndex, RSgImage& aImage)
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure correct error messages are returned when client tries to 
						open an image inside an image collection under various invalid conditions.
@SYMTestActions			Initialise the graphics resource component. Call OpenImage():\n
						\t 1. on an uninitialised image collection\n
						\t 2. with a negative index\n
						\t 3. with an index that is bigger than the number of images in the collection\n
						\t 4. with non-null aImage\n
@SYMTestExpectedResults	The function should return:\n
						\t 1. KErrBadHandle\n
						\t 2. KErrArgument\n
						\t 3. KErrArgument\n
						\t 4. KErrInUse\n
@SYMTestStatus			Implemented 
 */	
void CTSgImageCollectionGeneric::TestOpenImageInCollectionInvalidL()
	{
	TestOpenDriverL();
	RSgImageCollection collection;
	RSgImage image;
	
	//uninitialised image collection
	TEST(KErrBadHandle == collection.OpenImage(0, image));
	
	CreateImageCollectionL(collection);	
	CleanupClosePushL(collection);
	//with a negative index
	TEST(KErrArgument == collection.OpenImage(-1, image));
	image.Close();
	
	//with an index that is bigger than the number of images in the collection
	TInt invalidIndex = collection.Count();
	TEST(KErrArgument == collection.OpenImage(invalidIndex, image));
	image.Close();
	
	//with non-null aImage handle
	CreateImageL(image);
	CleanupClosePushL(image);
	TEST(KErrInUse == collection.OpenImage(0, image));
	CleanupStack::PopAndDestroy(&image);
	
	CleanupStack::PopAndDestroy();
	TestCloseDriver();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0038
@SYMTestCaseDesc		Invalid map/unmap tests on individual images in a collection
@SYMPREQ				PREQ39
@SYMREQ					REQ9193
@SYMREQ					REQ9214
@SYMREQ					REQ9224 
@SYMREQ					REQ9233  
@SYMREQ					REQ9234
@SYMFssID				SgImageCollection::OpenImage(TInt aIndex, RSgImage& aImage)\n
						RSgImage::MapReadOnly(const TAny*&, TInt&) const\n
 						RSgImage::MapWriteOnly(TAny*&, TInt&)\n 
						RSgImage::MapReadWrite(TAny*&, TInt&)\n 
						RSgImage::Unmap()
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To check the images in an image collection can map/unmap the same way as standalone images.
@SYMTestActions			Initialise the graphics resource component. Initialise an image collection. Get individual 
						images from the collection and perform various negative map/unmap tests on them.
@SYMTestExpectedResults	The functions should behave in the same way as the tests defined previously for stand alone images.
@SYMTestStatus			Defined 
 */	
void CTSgImageCollectionGeneric::TestMapUnmapImagesInCollectionInvalidL()
	{
	TestOpenDriverL();
	
	//create a collection
	TSgImageInfo info;
	info.iSizeInPixels = TSize(8, 8);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iCpuAccess = ESgCpuAccessReadWrite;
	info.iShareable = ETrue;
	RSgImageCollection collection;
	CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	
	const TAny* dataAddressRead;
	TAny* dataAddressWrite;
	TInt dataStride;
	
	//map twice
	for(TInt i=0; i<KNumImagesInCollection; ++i)
		{
		RSgImage image;
		TEST(KErrNone == collection.OpenImage(i, image));
		TEST(KErrNone == image.MapReadOnly(dataAddressRead, dataStride));
		TEST(KErrInUse == image.MapReadOnly(dataAddressRead, dataStride));
		TEST(KErrNone == image.Unmap());
		
		TEST(KErrNone == image.MapWriteOnly(dataAddressWrite, dataStride));
		TEST(KErrInUse == image.MapWriteOnly(dataAddressWrite, dataStride));
		TEST(KErrNone == image.Unmap());
		
		TEST(KErrNone == image.MapReadWrite(dataAddressWrite, dataStride));
		TEST(KErrInUse == image.MapReadWrite(dataAddressWrite, dataStride));
		TEST(KErrNone == image.Unmap());
		image.Close();
		}
	CleanupStack::PopAndDestroy();
	
	//call MapReadOnly() on the images in a collection created without CPU access
	//flag ESgCpuAccessReadOnly or ESgCpuAccessReadWrite
	info.iUsage = ESgUsageDirectGdiTarget;
	info.iCpuAccess = ESgCpuAccessNone;
	CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	for(TInt i=0; i<KNumImagesInCollection; ++i)
		{
		RSgImage image;
		TEST(KErrNone == collection.OpenImage(i, image));
		TEST(KErrAccessDenied == image.MapReadOnly(dataAddressRead, dataStride));
		image.Close();
		}
	CleanupStack::PopAndDestroy();	
	
	info.iUsage = ESgUsageDirectGdiSource;
	info.iCpuAccess = ESgCpuAccessWriteOnly;
	CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	for(TInt i=0; i<KNumImagesInCollection; ++i)
		{
		RSgImage image;
		TEST(KErrNone == collection.OpenImage(i, image));
		TEST(KErrAccessDenied == image.MapReadOnly(dataAddressRead, dataStride));
		image.Close();
		}
	CleanupStack::PopAndDestroy();	
	
	//call MapWriteOnly() on the images in a collection created without CPU access
	//flag ESgCpuAccessWriteOnly or ESgCpuAccessReadWrite
	info.iUsage = ESgUsageDirectGdiTarget;
	info.iCpuAccess = ESgCpuAccessNone;
	CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	for(TInt i=0; i<KNumImagesInCollection; ++i)
		{
		RSgImage image;
		TEST(KErrNone == collection.OpenImage(i, image));
		TEST(KErrAccessDenied == image.MapWriteOnly(dataAddressWrite, dataStride));
		image.Close();
		}
	CleanupStack::PopAndDestroy();
	
	info.iCpuAccess = ESgCpuAccessReadOnly;
	TInt err = collection.Create(info, KNumImagesInCollection);
	if(KErrNotSupported != err)
		{
		User::LeaveIfError(err);
		CleanupClosePushL(collection);
		for(TInt i=0; i<KNumImagesInCollection; ++i)
			{
			RSgImage image;
			TEST(KErrNone == collection.OpenImage(i, image));
			TEST(KErrAccessDenied == image.MapWriteOnly(dataAddressWrite, dataStride));
			image.Close();
			}
		CleanupStack::PopAndDestroy();
		}	
	else
		{
		INFO_PRINTF1(_L("Warning: skipping test MapWriteOnly() on read-only collection.\r\n"));
		}
	
	//call MapReadWrite() on the images in a collection created without CPU access
	//flag ESgCpuAccessReadWrite
	info.iCpuAccess = ESgCpuAccessNone;
	CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	for(TInt i=0; i<KNumImagesInCollection; ++i)
		{
		RSgImage image;
		TEST(KErrNone == collection.OpenImage(i, image));
		TEST(KErrAccessDenied == image.MapReadWrite(dataAddressWrite, dataStride));
		image.Close();
		}
	CleanupStack::PopAndDestroy();

	info.iCpuAccess = ESgCpuAccessReadOnly;
    err = collection.Create(info, KNumImagesInCollection);
    if(KErrNotSupported != err)
    	{
    	CleanupClosePushL(collection);
    	for(TInt i=0; i<KNumImagesInCollection; ++i)
    		{
    		RSgImage image;
    		TEST(KErrNone == collection.OpenImage(i, image));
    		TEST(KErrAccessDenied == image.MapReadWrite(dataAddressWrite, dataStride));
    		image.Close();
    		}
    	CleanupStack::PopAndDestroy();
    	}
    else
    	{
    	INFO_PRINTF1(_L("Warning: skipping test MapReadWrite() on read-only collection.\r\n"));
    	}
	
    info.iUsage = ESgUsageDirectGdiSource;
    info.iCpuAccess = ESgCpuAccessWriteOnly;
    CheckErrorL(KErrNone, collection.Create(info, KNumImagesInCollection), (TText8*)__FILE__, __LINE__);
	CleanupClosePushL(collection);
	for(TInt i=0; i<KNumImagesInCollection; ++i)
		{
		RSgImage image;
		TEST(KErrNone == collection.OpenImage(i, image));
		TEST(KErrAccessDenied == image.MapReadWrite(dataAddressWrite, dataStride));
		image.Close();
		}
	CleanupStack::PopAndDestroy();

	TestCloseDriver();
	}

void CTSgImageCollectionGeneric::DoMemoryTestsL()
	{
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCreateImageCollectionL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetImageCollectionSurfaceIdL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetCollectionImageNumberL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetImageInfoInCollectionL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestOpenImageInCollectionL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestMapImageInCollectionL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCreateMultipleImageCollectionL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCreateImageCollectionInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestCreateMultipleImageCollectionsInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestGetImageCollectionInfoInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestOpenImageInCollectionInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	
	__UHEAP_MARK; SgDriver::AllocMarkStart();
	TestMapUnmapImagesInCollectionInvalidL();
	__UHEAP_MARKEND; SgDriver::AllocMarkEnd(0);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0068
@SYMTestCaseDesc		Calls RSgImageCollection::Close() with an invalid image collection handle
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::Close()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Close() with an invalid image collection handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image collection in a second thread. 
						Declare another image collection and assign it to the current image collection handle. Close 
						the current image collection so the second image collection handle becomes invalid. Call 
						Close() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 4(ESgPanicBadImageCollectionHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionCloseInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionCloseInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCollectionCloseInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 4, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0069
@SYMTestCaseDesc		Calls RSgImageCollection::Close() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::Close()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Close() with an invalid image collection handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image collection in a second thread. 
						Declare another image collection and assign it to the current image collection handle. Close 
						the current image collection so the second image collection handle becomes invalid. Close the 
						graphics resource driver. Call Close() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionCloseNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionCloseNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCollectionCloseNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0070
@SYMTestCaseDesc		Calls RSgImageCollection::SurfaceId() with an invalid image collection handle
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::SurfaceId()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling SurfaceId() with an invalid image collection handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image collection in a second thread. 
						Declare another image collection and assign it to the current image collection handle. Close 
						the current image collection so the second image collection handle becomes invalid. Call 
						SurfaceId() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 4(ESgPanicBadImageCollectionHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionSurfaceIdInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionSurfaceIdInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCollectionSurfaceIdInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 4, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0071
@SYMTestCaseDesc		Calls RSgImageCollection::SurfaceId() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::SurfaceId()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling SurfaceId() with an invalid image collection handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image collection in a second thread. 
						Declare another image collection and assign it to the current image collection handle. Close 
						the current image collection so the second image collection handle becomes invalid. Close the 
						graphics resource driver. Call SurfaceId() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionSurfaceIdNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionSurfaceIdNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCollectionSurfaceIdNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0072
@SYMTestCaseDesc		Calls RSgImageCollection::GetInfo() with an invalid image collection handle
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::GetInfo(TSgImageInfo&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling GetInfo() with an invalid image collection handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image collection in a second thread. 
						Declare another image collection and assign it to the current image collection handle. Close 
						the current image collection so the second image collection handle becomes invalid. Call 
						GetInfo() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 4(ESgPanicBadImageCollectionHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionGetInfoInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionGetInfoInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCollectionGetInfoInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 4, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0073
@SYMTestCaseDesc		Calls RSgImageCollection::GetInfo() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::GetInfo(TSgImageInfo&)\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling GetInfo() with an invalid image collection handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image collection in a second thread. 
						Declare another image collection and assign it to the current image collection handle. Close 
						the current image collection so the second image collection handle becomes invalid. Close the 
						graphics resource driver. Call GetInfo() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionGetInfoNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionGetInfoNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCollectionGetInfoNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0074
@SYMTestCaseDesc		Calls RSgImageCollection::Count() with an invalid image collection handle
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::Count()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Count() with an invalid image collection handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image collection in a second thread. 
						Declare another image collection and assign it to the current image collection handle. Close 
						the current image collection so the second image collection handle becomes invalid. Call 
						Count() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 4(ESgPanicBadImageCollectionHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionCountInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionCountInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCollectionCountInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 4, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0075
@SYMTestCaseDesc		Calls RSgImageCollection::Count() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::Count()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling Count() with an invalid image collection handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image collection in a second thread. 
						Declare another image collection and assign it to the current image collection handle. Close 
						the current image collection so the second image collection handle becomes invalid. Close the 
						graphics resource driver. Call Count() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionCountNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionCountNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCollectionCountNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0076
@SYMTestCaseDesc		Calls RSgImageCollection::OpenImage() with an invalid image collection handle
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::OpenImage()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling OpenImage() with an invalid image collection handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image collection in a second thread. 
						Declare another image collection and assign it to the current image collection handle. Close 
						the current image collection so the second image collection handle becomes invalid. Call 
						OpenImage() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 4(ESgPanicBadImageCollectionHandle).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionOpenImageInvalidHandleL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionOpenImageInvalidHandle, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCollectionOpenImageInvalidHandleL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 4, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0077
@SYMTestCaseDesc		Calls RSgImageCollection::OpenImage() with a non-null handle and an uninitialised driver
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::OpenImage()\n 
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling OpenImage() with an invalid image collection handle will cause a panic.
@SYMTestActions			Initialise the graphics resource component and create an image collection in a second thread. 
						Declare another image collection and assign it to the current image collection handle. Close 
						the current image collection so the second image collection handle becomes invalid. Close the 
						graphics resource driver. Call OpenImage() on the second handle.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionOpenImageNoDriverL()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionOpenImageNoDriver, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestImageCollectionOpenImageNoDriverL");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0086
@SYMTestCaseDesc		Creates an image collection when the driver was not initialised.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::Create(const TSgImageInfo, TInt)\n
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling RSgImageCollection::Create() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgImageCollection::Create() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionCreateNoDriver1L()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionCreateNoDriver1, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageCollectionCreateNoDriver1L");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0087
@SYMTestCaseDesc		Creates multiple image collections when the driver was not initialised.
@SYMPREQ				PREQ39
@SYMREQ					REQ9214
@SYMFssID				RSgImageCollection::Create(const TSgImageInfo[], TInt, RSgImageCollection[], TInt)\n
@SYMTestPriority		Critical
@SYMTestType			Unit Test
@SYMTestPurpose			To ensure calling RSgImageCollection::Create() with an uninitialised driver will cause a panic.
@SYMTestActions			Do not Initialise the graphics resource component and call RSgImageCollection::Create() in a second thread.
@SYMTestExpectedResults The function should panic in the second thread with panic code SGRES 5 (ESgPanicNoDriver).
@SYMTestStatus			Implemented 
 */
void CTSgImageCollectionGeneric::TestPanicImageCollectionCreateNoDriver2L()
	{
	TSgImageInfo info;
	TSgresTestInfo threadInfo = {KSgNullDrawableId, info, 0, ESgresSecondThreadPanicImageCollectionCreateNoDriver2, ETrue};
 	TExitCategoryName exitCategoryName(KSgTestGenericPanicCategory);
	_LIT(KTestName, "TestPanicImageCollectionCreateNoDriver2L");
 	CreateSecondThreadAndCheckPanicL(threadInfo, 5, exitCategoryName, KTestName);
	}
