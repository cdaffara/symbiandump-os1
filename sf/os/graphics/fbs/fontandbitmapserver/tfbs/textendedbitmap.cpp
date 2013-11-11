// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "textendedbitmapcommon.h"
#include "textendedbitmap.h"
#include "examplerasterizer.h"
#include <graphics/fbsrasterizerclearcache.h>
#include <s32mem.h>

const TUint8 KRepeatTestData[] = "(This pattern is 32 bytes long!)";
const TInt KRepeatTestDataSize = 32;
const TInt KLargeDataSize = 0x8000 * KRepeatTestDataSize;

CTExtendedBitmap::CTExtendedBitmap(CTestStep* aStep):
	CTFbsBase(aStep, ETrue)
	{
	}

CTExtendedBitmap::~CTExtendedBitmap()
	{
	((CTExtendedBitmapStep*)iStep)->CloseTMSGraphicsStep();
	User::Free(iLargeData);
	}

void CTExtendedBitmap::ConstructL()
	{	
	TRAPD(err, iLargeData = static_cast<TUint8*>(User::AllocL(KLargeDataSize)));
	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("Error allocating large data buffer (size %d) in CTExtendedBitmap::ConstructL(), %d"), KLargeDataSize, err);
		User::Leave(err);
		}
	for (TInt i = 0; i < KLargeDataSize; i += KRepeatTestDataSize)
		{
		Mem::Copy(PtrAdd(iLargeData, i), KRepeatTestData, KRepeatTestDataSize);
		}
	
	// Save a pointer to the example rasterizer's MFbsRasterizerClearCache interface
	if (CFbsRasterizer* rasterizer = CFbsBitmap::Rasterizer())
		{		
		err = rasterizer->GetInterface(TUid::Uid(KUidFbsRasterizerClearCache), (TAny*&)iRasterizerClearCache);
		if (err != KErrNone) 
			{
			WARN_PRINTF1(_L("Failed to get a MFbsRasterizerClearCache interface for the current rasterizer, tests will panic during OOM testing due to the rasterizer cache not being cleared"));
			}
		}
	}

void CTExtendedBitmap::RunFbsTestL(TInt aCurTestCase)
    {    
    ((CTExtendedBitmapStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTExtendedBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0001"));
		CreateExtendedBitmapL();
		break;
	case 2:
		((CTExtendedBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0002"));
		CreateLargeExtendedBitmapL();
		break;
	case 3:
		((CTExtendedBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0003"));
		CreateUsingInitializerL();
		break;
	case 4:
		((CTExtendedBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0004"));
		ScanLineL(EFalse);
		break;
	case 5:
		((CTExtendedBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0004"));
		ScanLineL(ETrue);
		break;
	case 6:
		((CTExtendedBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0040"));
		TestGetPixelL(EFalse);
		break;
	case 7:
		((CTExtendedBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0040"));
		TestGetPixelL(ETrue);
		break;
	case 8:
		((CTExtendedBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0042"));
		TestGetVerticalScanLineL(EFalse);
		break;
	case 9:
		((CTExtendedBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0042"));
		TestGetVerticalScanLineL(ETrue);

		// Fall through as last test case
	default:
		SetLastTestCase();		
		break;
		}
	((CTExtendedBitmapStep*)iStep)->RecordTestResultL();
    }


/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0001

	@SYMTestCaseDesc
	Create an extended bitmap and retrieve data

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10849
	REQ10850
	REQ10851
	REQ10852
	REQ10853
	REQ10854
	REQ10855

	@SYMTestPriority  
	High
	
	@SYMTestStatus 
	Implemented
	
	@SYMTestActions
	Creates an extended bitmap using test data and test Uid;
	Retrieves and validates the data size, data and Uid.
		
	@SYMTestExpectedResults
	Extended bitmap created and information correctly retrieved.
*/
void CTExtendedBitmap::CreateExtendedBitmapL()
	{
	INFO_PRINTF1(_L("Create an extended bitmap"));

	const TUint8 KTestData[]		= "Extended bitmap test data 123456";
	const TInt KTestDataSize		= sizeof(KTestData);
	const TSize KSizeInPixels 		= TSize(50,50);
	const TDisplayMode KDisplayMode	= EColor64K;

	CFbsBitmap* bmp = new(ELeave)CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt res = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(res);

	bmp->BeginDataAccess();
	
	TInt returnedDataSize = bmp->DataSize();
	TESTEQUALL(KTestDataSize, returnedDataSize);
	INFO_PRINTF3(_L("Test data size on creation: %i. Test data size returned: %i"), KTestDataSize, returnedDataSize);
	
	const TUint8* returnedDataAddress = reinterpret_cast<TUint8*>(bmp->DataAddress());
	res = Mem::Compare(returnedDataAddress,returnedDataSize, KTestData, KTestDataSize);
	TESTEQUALL(0, res);
	
	TUid returnedUid = bmp->ExtendedBitmapType();
	TESTEQUALL(KUidTestExtendedBitmap, returnedUid);

	bmp->EndDataAccess(ETrue);
	
	CleanupStack::PopAndDestroy(bmp);
	}


/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0002

	@SYMTestCaseDesc
	Create an extended bitmap with data sized 1MB

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847
	REQ10863

	@SYMTestPriority  
	High
	
	@SYMTestStatus 
	Implemented
	
	@SYMTestActions
	Create an extended bitmap with a buffer that is 1MB.
		
	@SYMTestExpectedResults
	No errors or panics should occur.
*/
void CTExtendedBitmap::CreateLargeExtendedBitmapL()
	{
	INFO_PRINTF1(_L("Create an extended bitmap with data size 1MB"));	
	
	const TSize KSizeInPixels = TSize(50,50);
	const TDisplayMode KDisplayMode	= EColor64K;	
	
	CFbsBitmap* bmp = new(ELeave)CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt res = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, iLargeData, KLargeDataSize);
	TESTNOERRORL(res);

	bmp->BeginDataAccess();
	
	TInt returnedDataSize = bmp->DataSize();
	TESTEQUALL(KLargeDataSize, returnedDataSize);
	INFO_PRINTF3(_L("Test data size on creation: %i. Test data size returned: %i"), KLargeDataSize, returnedDataSize);
	
	const TUint8* returnedDataAddress = reinterpret_cast<TUint8*>(bmp->DataAddress());
	res = Mem::Compare(returnedDataAddress, returnedDataSize, iLargeData, KLargeDataSize);
	TESTEQUALL(0, res);
	
	TUid returnedUid = bmp->ExtendedBitmapType();
	TESTEQUALL(KUidTestExtendedBitmap, returnedUid);

	bmp->EndDataAccess(ETrue);
	
	CleanupStack::PopAndDestroy(bmp);
	}


/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0003

	@SYMTestCaseDesc
	Create an extended bitmap using an MFbsExtendedBitmapInitializer

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ

	@SYMTestPriority  
	High
	
	@SYMTestStatus 
	Implemented
	
	@SYMTestActions
	Create an extended bitmap using an MFbsExtendedBitmapInitializer, with a buffer that is 1MB.
		
	@SYMTestExpectedResults
	Extended bitmap created and information correctly retrieved. 
*/
void CTExtendedBitmap::CreateUsingInitializerL()
	{
	INFO_PRINTF1(_L("Create an extended bitmap using an MFbsExtendedBitmapInitializer"));	
	
	const TSize KSizeInPixels = TSize(50,50);
	const TDisplayMode KDisplayMode	= EColor64K;	
	
	CFbsBitmap* bmp = new(ELeave)CFbsBitmap;
	CleanupStack::PushL(bmp);
	CTestExtendedBitmapInitializer* initializer = new(ELeave) CTestExtendedBitmapInitializer(iLargeData, KLargeDataSize);
	CleanupStack::PushL(initializer);
	TInt res = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KLargeDataSize, *initializer);
	CleanupStack::PopAndDestroy(initializer);
	TESTNOERRORL(res);
	
	bmp->BeginDataAccess();
	
	TInt returnedDataSize = bmp->DataSize();
	TESTEQUALL(KLargeDataSize, returnedDataSize);
	INFO_PRINTF3(_L("Test data size on creation: %i. Test data size returned: %i"), KLargeDataSize, returnedDataSize);
	
	const TUint8* returnedDataAddress = reinterpret_cast<TUint8*>(bmp->DataAddress());
	res = Mem::Compare(returnedDataAddress, returnedDataSize, iLargeData, KLargeDataSize);
	TESTEQUALL(0, res);
	
	TUid returnedUid = bmp->ExtendedBitmapType();
	TESTEQUALL(KUidTestExtendedBitmap, returnedUid);

	bmp->EndDataAccess(ETrue);
	
	CleanupStack::PopAndDestroy(bmp);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0004

	@SYMTestCaseDesc
	Create an extended bitmap and retrieve scanline

	@SYMPREQ 
	PREQ2096
	
	@SYMREQ
	REQ10847

	@SYMTestPriority  
	High
	
	@SYMTestStatus 
	Implemented
	
	@SYMTestActions
	Creates an extended bitmap using data compatible with the
	example rasterizer.
	If (aUseDuplicateBitmap == ETrue) use CFbsBitmap::Duplicate()
	to create a duplicate of the extended bitmap just created and 
	complete the test using the duplicate extended bitmap. 
	Get a scanline using CFbsBitmap::GetScanLine() and check if
	it is correct, depending on the presence of the rasterizer.
		
	@SYMTestExpectedResults
	Extended bitmap created and scanline correctly retrieved.
*/
void CTExtendedBitmap::ScanLineL(TBool aUseDuplicateBitmap)
	{
	INFO_PRINTF1(_L("Get a scanline from an extended bitmap"));	

	const TRgb KColors[] 	= {TRgb(0,0,0), TRgb(255,0,0), TRgb(255,255,0)};
	// Small size needed for this test O(x^2 * y) iterations run every time
	const TSize KSizeInPixels 		= TSize(8,5); 
	const TDisplayMode KDisplayMode	= EColor64K;
	const TUint8 horizontalStripes = 1;

	// Check to see if we have an example bitmap rasterizer available for this test run	
	iRasterizerAvailable = (CFbsBitmap::Rasterizer() != NULL);
	if (iRasterizerAvailable)
		{
		INFO_PRINTF1(_L("Testing WITH the example rasterizer - Rasterizer Available"));	
		}
	else
		{
		INFO_PRINTF1(_L("Testing WITHOUT the example rasterizer - Rasterizer NOT Available"));
		}
	
	CleanupStack::PushL(TCleanupItem(ClearRasterizerCache, iRasterizerClearCache));

	TInt dataSize = sizeof(KColors)+sizeof(horizontalStripes); // estimate the data size
	TUint8* data = new(ELeave) TUint8[dataSize];
	CleanupStack::PushL(data);	
	
	// Write the colours to be used in the extended bitmap to the data
	RMemWriteStream ws;
	ws.Open(data, dataSize);
	CleanupClosePushL(ws);
	ws << KColors[0] << KColors[1] << KColors[2] << horizontalStripes;
	dataSize = ws.Sink()->TellL(MStreamBuf::EWrite).Offset(); // get the actual size written
	CleanupStack::PopAndDestroy(1, &ws);
	
	// Create the extended bitmap to be used a a brush
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt res = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidExampleExtendedBitmap, data, dataSize);
	TESTNOERRORL(res);
		
	if (aUseDuplicateBitmap)
		{
		INFO_PRINTF1(_L("Testing using a duplicated extended bitmap"));
		CFbsBitmap* duplicateBmp = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(duplicateBmp);
		res = duplicateBmp->Duplicate(bmp->Handle());
		TESTNOERRORL(res);
		bmp = duplicateBmp;
		}

	TInt byteWidth = CFbsBitmap::ScanLineLength(KSizeInPixels.iWidth, KDisplayMode);
	TUint8* buffer = new(ELeave) TUint8[byteWidth + 1];
	CleanupArrayDeletePushL(buffer);

	const TUint8 KCheckValue = 0x69; // Pixel value used as guard at the end of the buffer used
	buffer[byteWidth] = KCheckValue; 
	TPtr8 scanLine(buffer,byteWidth,byteWidth);

	TInt x, y;
	TInt firstStripe, lastStripe;
	TInt* s;
	if(horizontalStripes)
		{
		firstStripe = KSizeInPixels.iHeight/3;
		lastStripe = KSizeInPixels.iHeight - firstStripe;
		s = &y;
		}
	else
		{
		firstStripe = KSizeInPixels.iWidth/3;
		lastStripe = KSizeInPixels.iWidth - firstStripe;
		s = &x;
		}

	for(y = 0; y < KSizeInPixels.iHeight; ++y)
		{
		for(TInt length = 1; length <= KSizeInPixels.iWidth; ++length)
			{
			for(TInt pos = KSizeInPixels.iWidth - length; pos >= 0; --pos)
				{
				bmp->GetScanLine(scanLine, TPoint(pos,y), length, KDisplayMode);
				for(x = pos + length - 1; x >= pos; --x)
					{
					TRgb bufferColor = ExtractRgb(buffer, x-pos, KDisplayMode);
					
					if(!iRasterizerAvailable)
						{
						TESTCOLOREQUALL(bufferColor, KRgbWhite);
						}
					else if (*s < firstStripe)
						{
						// First stripe
						TESTCOLOREQUALL(bufferColor, KColors[0]);
						}
					else if (*s >= lastStripe)
						{
						// Last stripe
						TESTCOLOREQUALL(bufferColor, KColors[2]);
						}
					else
						{
						// Middle stripe
						TESTCOLOREQUALL(bufferColor, KColors[1]);
						}							
					}
				}
			}
		}
	
	// Check that the guard byte is still intact now that the tests have been run
	TEST(buffer[byteWidth] == KCheckValue); 
	
	if (aUseDuplicateBitmap)
		{
		CleanupStack::PopAndDestroy(1);
		}
	CleanupStack::PopAndDestroy(4);
	}


/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0040

	@SYMTestCaseDesc
	Create an extended bitmap and retrieve pixels

	@SYMPREQ 
	CR1804
	
	@SYMREQ
	REQ10858

	@SYMTestPriority  
	High
	
	@SYMTestStatus 
	Implemented
	
	@SYMTestActions
	Creates an extended bitmap using data compatible with the
	example rasterizer.
	If (aUseDuplicateBitmap == ETrue) use CFbsBitmap::Duplicate()
	to create a duplicate of the extended bitmap just created and 
	complete the test using the duplicate extended bitmap. 
	Get pixel values using CFbsBitmap::GetPixel() and check if
	they are correct, depending on the presence of the rasterizer.
		
	@SYMTestExpectedResults
	Extended bitmap created and pixels correctly retrieved.
*/
void CTExtendedBitmap::TestGetPixelL(TBool aUseDuplicateBitmap)
	{
	INFO_PRINTF1(_L("Get pixel values from an extended bitmap"));	

	const TRgb KColors[] 	= {TRgb(0,0,0), TRgb(255,0,0), TRgb(255,255,0)};
	const TSize KSizeInPixels 		= TSize(8,5); 
	const TDisplayMode KDisplayMode	= EColor64K;
	const TUint8 KHorizontalStripes = 1;

	// Check to see if we have an example bitmap rasterizer available for this test run	
	iRasterizerAvailable = (CFbsBitmap::Rasterizer() != NULL);
	if (iRasterizerAvailable)
		{
		INFO_PRINTF1(_L("Testing WITH the example rasterizer - Rasterizer Available"));	
		}
	else
		{
		INFO_PRINTF1(_L("Testing WITHOUT the example rasterizer - Rasterizer NOT Available"));
		}
	
	CleanupStack::PushL(TCleanupItem(ClearRasterizerCache, iRasterizerClearCache));

	TInt dataSize = sizeof(KColors)+sizeof(KHorizontalStripes); // estimate the data size
	TUint8* data = new(ELeave) TUint8[dataSize];
	CleanupStack::PushL(data);	
	
	// Write the colours to be used in the extended bitmap to the data
	RMemWriteStream ws;
	ws.Open(data, dataSize);
	CleanupClosePushL(ws);
	ws << KColors[0] << KColors[1] << KColors[2] << KHorizontalStripes;
	dataSize = ws.Sink()->TellL(MStreamBuf::EWrite).Offset(); // get the actual size written
	CleanupStack::PopAndDestroy(1, &ws);
	
	// Create the extended bitmap
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt res = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidExampleExtendedBitmap, data, dataSize);
	TESTNOERRORL(res);
		
	if (aUseDuplicateBitmap)
		{
		INFO_PRINTF1(_L("Testing using a duplicated extended bitmap"));
		CFbsBitmap* duplicateBmp = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(duplicateBmp);
		res = duplicateBmp->Duplicate(bmp->Handle());
		TESTNOERRORL(res);
		bmp = duplicateBmp;
		}

	TInt firstStripe = KSizeInPixels.iHeight/3;
	TInt lastStripe = KSizeInPixels.iHeight - firstStripe;
	TRgb color;
	for(TInt y = 0; y < KSizeInPixels.iHeight; ++y)
		{
		for(TInt x = 0; x < KSizeInPixels.iWidth; ++x)
			{
			bmp->GetPixel(color, TPoint(x,y));

			if(!iRasterizerAvailable)
				{
				TESTCOLOREQUALL(color, KRgbWhite);
				}
			else if (y < firstStripe)
				{
				// First stripe
				TESTCOLOREQUALL(color, KColors[0]);
				}
			else if (y >= lastStripe)
				{
				// Last stripe
				TESTCOLOREQUALL(color, KColors[2]);
				}
			else
				{
				// Middle stripe
				TESTCOLOREQUALL(color, KColors[1]);
				}							
			}
		}
	
	if (aUseDuplicateBitmap)
		{
		CleanupStack::PopAndDestroy(1);
		}
	CleanupStack::PopAndDestroy(3);
	}



/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0042

@SYMTestCaseDesc
	Create an extended bitmap and retrieve vertical scanline

@SYMPREQ 
	DEF141346
	
@SYMTestPriority  
	High

@SYMTestStatus 
	Implemented

@SYMTestActions
	Create an extended bitmap using data compatible with the
	example rasterizer.
	If (aUseDuplicateBitmap == ETrue) use CFbsBitmap::Duplicate()
	to create a duplicate of the extended bitmap just created and 
	complete the test using the duplicate extended bitmap. 
	Get a vertical scanline using CFbsBitmap::GetVerticalScanLine() and check
	if it is correct, depending on the presence of the rasterizer.

@SYMTestExpectedResults
	Extended bitmap created and scanline correctly retrieved.
*/
void CTExtendedBitmap::TestGetVerticalScanLineL(TBool aUseDuplicateBitmap)
	{
	INFO_PRINTF1(_L("Get a vertical scanline from an extended bitmap"));	

		const TRgb KColors[] 	= {TRgb(0,0,0), TRgb(255,0,0), TRgb(255,255,0)};
		const TSize KSizeInPixels 		= TSize(5,8); 
		const TDisplayMode KDisplayMode	= EColor64K;
		const TUint8 KHorizontalStripes = 1;

		// Check to see if we have an example bitmap rasterizer available for this test run	
		iRasterizerAvailable = (CFbsBitmap::Rasterizer() != NULL);
		if (iRasterizerAvailable)
			{
			INFO_PRINTF1(_L("Testing WITH the example rasterizer - Rasterizer Available"));	
			}
		else
			{
			INFO_PRINTF1(_L("Testing WITHOUT the example rasterizer - Rasterizer NOT Available"));
			}
		
		CleanupStack::PushL(TCleanupItem(ClearRasterizerCache, iRasterizerClearCache));

		TInt dataSize = sizeof(KColors)+sizeof(KHorizontalStripes); // estimate the data size
		TUint8* data = new(ELeave) TUint8[dataSize];
		CleanupStack::PushL(data);	
		
		// Write the colours to be used in the extended bitmap to the data
		RMemWriteStream ws;
		ws.Open(data, dataSize);
		CleanupClosePushL(ws);
		ws << KColors[0] << KColors[1] << KColors[2] << KHorizontalStripes;
		dataSize = ws.Sink()->TellL(MStreamBuf::EWrite).Offset(); // get the actual size written
		CleanupStack::PopAndDestroy(1, &ws);
		
		// Create the extended bitmap
		CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bmp);
		TInt res = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidExampleExtendedBitmap, data, dataSize);
		TESTNOERRORL(res);
			
		if (aUseDuplicateBitmap)
			{
			INFO_PRINTF1(_L("Testing using a duplicated extended bitmap"));
			CFbsBitmap* duplicateBmp = new(ELeave) CFbsBitmap;
			CleanupStack::PushL(duplicateBmp);
			res = duplicateBmp->Duplicate(bmp->Handle());
			TESTNOERRORL(res);
			bmp = duplicateBmp;
			}

		TInt byteWidth = CFbsBitmap::ScanLineLength(KSizeInPixels.iHeight, KDisplayMode);
		TUint8* buffer = new(ELeave) TUint8[byteWidth + 1];
		CleanupArrayDeletePushL(buffer);

		const TUint8 KCheckValue = 0x69; // Pixel value used as guard at the end of the buffer used
		buffer[byteWidth] = KCheckValue; 
		TPtr8 scanLine(buffer,byteWidth,byteWidth);

		TInt firstStripe = KSizeInPixels.iHeight/3;
		TInt lastStripe = KSizeInPixels.iHeight - firstStripe;
		TRgb color;
		for(TInt x = 0; x < KSizeInPixels.iWidth; ++x)
			{
			bmp->GetVerticalScanLine(scanLine, x, KDisplayMode);
			for(TInt y = 0; y < KSizeInPixels.iHeight; ++y)
				{
				TRgb bufferColor = ExtractRgb(buffer, y, KDisplayMode);
				if(!iRasterizerAvailable)
					{
					TESTCOLOREQUALL(bufferColor, KRgbWhite);
					}
				else if (y < firstStripe)
					{
					// First stripe
					TESTCOLOREQUALL(bufferColor, KColors[0]);
					}
				else if (y >= lastStripe)
					{
					// Last stripe
					TESTCOLOREQUALL(bufferColor, KColors[2]);
					}
				else
					{
					// Middle stripe
					TESTCOLOREQUALL(bufferColor, KColors[1]);
					}							
				}
			}

		// Check that the guard byte is still intact now that the tests have been run
		TEST(buffer[byteWidth] == KCheckValue); 

		if (aUseDuplicateBitmap)
			{
			CleanupStack::PopAndDestroy(1);
			}
		CleanupStack::PopAndDestroy(4);
	}

//--------------
__CONSTRUCT_STEP__(ExtendedBitmap)

/** Method to be used with a TCleanupItem for clearing the extended bitmap rasterizer cache
from the cleanup stack. Calls the MExampleRasterizerClearCache::ClearCache() method on the
passed CFbsRasterizer object if the MExampleRasterizerClearCache extension interface is available
for that object.
@param aPtr A pointer to the current CFbsRasterizer object, or NULL if no rasterizer
object is available.
 */
void CTExtendedBitmap::ClearRasterizerCache(TAny* aPtr)
	{	
	if (aPtr)
		{
		static_cast<MFbsRasterizerClearCache*>(aPtr)->ClearCache();
		}
	}

