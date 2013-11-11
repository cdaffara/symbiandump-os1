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

#include "trasterizer.h"
#include "testutils.h"
#include "examplerasterizer.h"
#include <fbs.h>
#include <gdi.h>
#include <s32mem.h>
#include <e32math.h>
#include <e32svr.h>
#include <u32hal.h>

const TDisplayMode KDisplayModes[] = {EGray256, EColor64K, EColor16MU, EColor16MAP};
const TInt KNumDisplayModes = sizeof(KDisplayModes)/sizeof(KDisplayModes[0]);
const TRgb KStripeColours[3] = {TRgb(255,255,0), TRgb(255,0,255), TRgb(0,255,255)};

CTRasterizer::CTRasterizer()
	{
	SetTestStepName(KTRasterizerStep);
	}

/**
@SYMTestCaseID		
	GRAPHICS-FBSERV-RASTERIZER-001

@SYMTestPriority
	High

@SYMPREQ
	PREQ2096

@SYMREQ
	REQ10860
	REQ10861 
	
@SYMTestCaseDesc
	Test that the example rasterizer dll "fbsrasterizer_test.dll" has been loaded.

@SYMTestActions	
	On the emulator check that the rasterizer setting is "fbsrasterizer_test.dll".
	Get a pointer to the rasterizer.

@SYMTestExpectedResults
	"fbsrasterizer_test.dll" has been loaded successfully
*/
void CTRasterizer::TestLoadRasterizerDllL()
	{
	INFO_PRINTF1(_L("TestLoadRasterizerDll"));
#ifdef __WINS__
	_LIT8(KTestRasterizerName, "fbsrasterizer_test.dll");
	TUint8* rasterizerSetting = NULL;
	UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalStringProperty, (TAny*)"FBSRASTERIZER_DLL", &rasterizerSetting);
	TESTL(rasterizerSetting != NULL);
	TESTL(TPtrC8(rasterizerSetting).CompareF(KTestRasterizerName) == 0);
#endif
	GetExampleRasterizerL();
	}

/**
@SYMTestCaseID		
	GRAPHICS-FBSERV-RASTERIZER-002

@SYMTestPriority
	High

@SYMPREQ
	PREQ2096

@SYMREQ
	REQ10860
	REQ10861 
	
@SYMTestCaseDesc
	Test that a scanline retrieved from the example rasterizer is what we expect.

@SYMTestActions		
	Create an extended bitmap and set its extended data to three colours followed by a TUint8.
	The example rasterizer interprets this data as the three colours in a tricolour flag drawn
	with either horizontal or vertical stripes depending on the value of the TUint8.	
	Draw a scanline of the extended bitmap by calling:
		- CFbsRasterizer::BeginBitmap()
		- CFbsRasterizer::ScanLine()
		- CFbsRasterizer::EndBitmap()

@SYMTestExpectedResults
	The scan line returned by the example rasterizer should be exactly the same as the
	corresponding scanline in the test data.
*/
void CTRasterizer::TestScanLineL()
	{	
	TBuf<32> displayModeDes(TestUtils::DisplayModeToString(iDisplayMode)); 
	INFO_PRINTF2(_L("TestScanLine, display mode %S"), &displayModeDes);
	
	const TInt KNumBeginBitmapCalls = 5;
	
	// Get the example rasterizer
	GetExampleRasterizerL();	
	
	// Test for both stripe styles, 0 for vertical stripes and 1 for horizontal stripes
	for (TInt stripeStyle = 0; stripeStyle < 2; stripeStyle++)
		{
		if (stripeStyle == 0)
			{
			INFO_PRINTF1(_L("... Vertical stripes"));
			}
		else
			{
			INFO_PRINTF1(_L("... Horizontal stripes"));
			}
				
		const TSize sizeInPixels(64,32);
		CFbsBitmap* extendedBitmap = CreateExtendedBitmapLC(KStripeColours, stripeStyle, sizeInPixels);					
			
		// Begin scanline access to the extended bitmap
		CFbsRasterizer::TBitmapDesc bitmapDesc;
		TInt64 bitmapId = extendedBitmap->SerialNumber();
		bitmapDesc.iSizeInPixels = sizeInPixels;
		bitmapDesc.iDispMode = iDisplayMode;
		bitmapDesc.iDataType = KUidExampleExtendedBitmap;
		bitmapDesc.iData = extendedBitmap->DataAddress();
		bitmapDesc.iDataSize = extendedBitmap->DataSize();
		extendedBitmap->BeginDataAccess();
		// test calling BeginBitmap more than once as each additional call should just increment as reference count
		for (TInt i = 0; i < KNumBeginBitmapCalls; i++)
			{			
			iRasterizer->BeginBitmap(bitmapId, bitmapDesc, NULL);
			}				
		
		TInt scanlineLength;
		TPoint startPixel;
		TRgb bufferColour;
		// The width or height of a stripe in the tricolour flag that the extended bitmap rasterizer draws
		TInt stripeSize = (stripeStyle == 0) ? sizeInPixels.iWidth/3 : sizeInPixels.iHeight/3; 				
		for (TInt h = 0; h < sizeInPixels.iHeight; h++)
			{
			// Get a full scanline from the extended bitmap using the rasterizer	
			startPixel = TPoint(0, h);
			scanlineLength = sizeInPixels.iWidth;
			const TUint8* scanline = reinterpret_cast<const TUint8*>(iRasterizer->ScanLine(bitmapId, startPixel, scanlineLength));
			TESTL(scanline != NULL);								
			
			// Make sure each pixel in the scanline is the colour that we expect						
			TRgb compareColour;
			
			for (TInt w = 0; w < scanlineLength; w++)
				{																			
				if (((stripeStyle == 0) && (w < stripeSize)) 
					|| ((stripeStyle == 1) && (h < stripeSize)))
					{					
					compareColour = KStripeColours[0]; // left or top
					}
				else if (((stripeStyle == 0) && (w >= sizeInPixels.iWidth-stripeSize)) 
					|| ((stripeStyle == 1) && (h >= sizeInPixels.iHeight-stripeSize)))
					{
					compareColour = KStripeColours[2]; // right or bottom
					}
				else
					{
					compareColour = KStripeColours[1]; // middle
					}					
				bufferColour = ExtractRgb(scanline, w, iDisplayMode);
				if (((iDisplayMode != EGray256) && (bufferColour != compareColour))
					|| ((iDisplayMode == EGray256) && (bufferColour != TRgb::Gray256(compareColour.Gray256()))))
					{					
					INFO_PRINTF3(_L("Pixel comparison failed for pixel %d,%d"), w, h);
					TEST(EFalse);
					}			
				}							
			}
		
		// End scanline access to the extended bitmap
		for (TInt i = 0; i < KNumBeginBitmapCalls; i++)
			{
			iRasterizer->EndBitmap(bitmapId);			
			}
		extendedBitmap->EndDataAccess();
		
		// Clean up
		CleanupStack::Pop(1, extendedBitmap);
		}
	}

/**
@SYMTestCaseID		
	GRAPHICS-FBSERV-RASTERIZER-003

@SYMTestPriority
	High

@SYMPREQ
	PREQ2096

@SYMREQ
	REQ10860
	REQ10861 
	
@SYMTestCaseDesc
	Test that we can get scanlines from 3 extended bitmaps simultaneously using the example rasterizer.

@SYMTestActions	
	Load a normal bitmap.	
	Create 3 extended bitmaps and set their extended data to be vertically striped colours.	
	Draw a scanline of each extended bitmap by calling:
		- CFbsRasterizer::BeginBitmap() for each extended bitmap
		- CFbsRasterizer::ScanLine() for each extended bitmap
		- CFbsRasterizer::EndBitmap() for each extended bitmap
	Compare the first pixel of each scanline and the last pixel of each scanline against the
	expected colour.

@SYMTestExpectedResults
	The rasterizer should be able to handle rasterizing up to 3 extended bitmaps at once,
	all three scanlines should be returned with no errors.
	The colours compared should match exactly.
*/
void CTRasterizer::TestMultipleScanLinesL()
	{
	TBuf<32> displayModeDes(TestUtils::DisplayModeToString(iDisplayMode)); 
	INFO_PRINTF2(_L("TestMultipleScanLines, vertical stripes, display mode %S"), &displayModeDes);
	
	const TInt KNumBitmaps = 3;
	const TInt KBaseSize = 10;			
	RArray<CFbsBitmap*> bitmaps;
	CleanupClosePushL(bitmaps);	
	
	// Get the example rasterizer
	GetExampleRasterizerL();
	
	// Create the three differently sized extended bitmaps each coloured with vertical stripes 
	for (TInt i = 0; i < KNumBitmaps; i++)
		{
		const TSize KSizeInPixels = TSize(KBaseSize*(i+1), KBaseSize*(i+1));			
		CFbsBitmap* extendedBitmap = CreateExtendedBitmapLC(KStripeColours, EFalse, KSizeInPixels);		
		bitmaps.AppendL(extendedBitmap);			
		}
	
	// Begin scanline access to the extended bitmaps
	for (TInt i = 0; i < KNumBitmaps; i++)
		{
		bitmaps[i]->BeginDataAccess();
		
		CFbsRasterizer::TBitmapDesc bitmapDesc;
		TInt64 bitmapId = bitmaps[i]->SerialNumber();
		bitmapDesc.iSizeInPixels = bitmaps[i]->SizeInPixels();
		bitmapDesc.iDispMode = iDisplayMode;
		bitmapDesc.iDataType = KUidExampleExtendedBitmap;
		bitmapDesc.iData = bitmaps[i]->DataAddress();
		bitmapDesc.iDataSize = bitmaps[i]->DataSize();	
		iRasterizer->BeginBitmap(bitmapId, bitmapDesc, NULL);
		}
	
	// Get scanlines from each extended bitmap and check that the first and last pixel in each is the correct colour
	for (TInt h = 0; h < KBaseSize; h++)
		{
		for (TInt i = 0; i < KNumBitmaps; i++)
			{	
			const TUint8* scanline = reinterpret_cast<const TUint8*>(iRasterizer->ScanLine(bitmaps[i]->SerialNumber(), TPoint(0,h), KBaseSize*(i+1)));			
			TESTL(scanline != NULL);					
			
			// Test that the first pixel is the correct colour (the first pixel in each scanline should be KStripeColours[0]
			// as the bitmaps should have all been drawn with vertical stripes with the first stripe being KStripeColours[0]
			TRgb bufferColour = ExtractRgb(scanline, 0, iDisplayMode);			
			if (((iDisplayMode != EGray256) && (bufferColour != KStripeColours[0]))
				|| ((iDisplayMode == EGray256) && (bufferColour != TRgb::Gray256(KStripeColours[0].Gray256()))))	
				{
				INFO_PRINTF3(_L("Pixel comparison failed for bitmap %d, pixel 0,%d"), i, h);
				TEST(EFalse);
				}	
			// The last pixel should be KStripeColours[2]							
			bufferColour = ExtractRgb(scanline, (KBaseSize*(i+1))-1, iDisplayMode);
			if (((iDisplayMode != EGray256) && (bufferColour != KStripeColours[2]))
				|| ((iDisplayMode == EGray256) && (bufferColour != TRgb::Gray256(KStripeColours[2].Gray256()))))	
				{
				INFO_PRINTF4(_L("Pixel comparison failed for bitmap %d, pixel %d,%d"), i, (KBaseSize*(i+1))-1, h);
				TEST(EFalse);
				}
			}
		}
		
	// Unregister each extended bitmap
	for (TInt i = 0; i < KNumBitmaps; i++)
		{					
		iRasterizer->EndBitmap(bitmaps[i]->SerialNumber());
		bitmaps[i]->EndDataAccess();
		}
	
	// Cleanup
	CleanupStack::PopAndDestroy(KNumBitmaps+1);
	}

/**
@SYMTestCaseID		
	GRAPHICS-FBSERV-RASTERIZER-004

@SYMTestPriority
	High

@SYMPREQ
	PREQ2096

@SYMREQ
	REQ10860
	REQ10861 
	
@SYMTestCaseDesc
	Test that the rasterizer returns portions of scanline correctly
	when a region of interest is specified.

@SYMTestActions	
	Create an extended bitmap.
	Rasterize it specifying a region of interest that has three regions that roughly
	form a squared off C shape.		

@SYMTestExpectedResults
	Check that scanlines returned are correct in the specified region of interest.
	(Anything outside the region of interest does not matter).
*/
void CTRasterizer::TestRegionOfInterestL()
	{
	TBuf<32> displayModeDes(TestUtils::DisplayModeToString(iDisplayMode)); 
	INFO_PRINTF2(_L("TestRegionOfInterest, horizontal stripes, display mode %S"), &displayModeDes);
	
	// Get the example rasterizer
	GetExampleRasterizerL();	
	
	// Create an extended bitmap with horizontal stripes
	const TSize sizeInPixels(24,24);
	CFbsBitmap* extendedBitmap = CreateExtendedBitmapLC(KStripeColours, ETrue, sizeInPixels);		
	
	// Create a region of interest in the shape of a square with a hole in the middle
	TRegionFix<4> regionOfInterest;
	const TInt KStripeSize = sizeInPixels.iHeight/3;
	const TInt KEdgeOffset = 2;	
	regionOfInterest.AddRect(TRect(TPoint(KEdgeOffset,KEdgeOffset), TSize(sizeInPixels.iWidth-(KEdgeOffset*2),KStripeSize)));	// top of the square
	regionOfInterest.AddRect(TRect(TPoint(KEdgeOffset,sizeInPixels.iHeight-KEdgeOffset-KStripeSize), TSize(sizeInPixels.iWidth-(KEdgeOffset*2),KStripeSize)));	// bottom of the square
	regionOfInterest.AddRect(TRect(TPoint(KEdgeOffset,KEdgeOffset+KStripeSize), TSize(KStripeSize,sizeInPixels.iHeight-(KEdgeOffset*2)-(KStripeSize*2))));	// left of the square
	regionOfInterest.AddRect(TRect(TPoint(sizeInPixels.iWidth-KEdgeOffset-KStripeSize,KEdgeOffset+KStripeSize), TSize(KStripeSize,sizeInPixels.iHeight-(KEdgeOffset*2)-(KStripeSize*2)))); // right of the square		
	TEST(regionOfInterest.CheckError() == EFalse);
		
	// Begin scanline access to the extended bitmap
	CFbsRasterizer::TBitmapDesc bitmapDesc;
	TInt64 bitmapId = extendedBitmap->SerialNumber();
	bitmapDesc.iSizeInPixels = sizeInPixels;
	bitmapDesc.iDispMode = iDisplayMode;
	bitmapDesc.iDataType = KUidExampleExtendedBitmap;
	bitmapDesc.iData = extendedBitmap->DataAddress();
	bitmapDesc.iDataSize = extendedBitmap->DataSize();
	extendedBitmap->BeginDataAccess();
	iRasterizer->BeginBitmap(bitmapId, bitmapDesc, &regionOfInterest);	
	
	TInt scanlineLength;
	TPoint startPixel;
	TRgb bufferColour;
	 			
	for (TInt h = 0; h < sizeInPixels.iHeight; h++)
		{
		// Get scanlines from the extended bitmap using the rasterizer	
		startPixel = TPoint(0, h);
		scanlineLength = sizeInPixels.iWidth;
		const TUint8* scanline = reinterpret_cast<const TUint8*>(iRasterizer->ScanLine(bitmapId, startPixel, scanlineLength));
		TESTL(scanline != NULL);								
		
		// Make sure each pixel in the scanline is the colour that we expect					
		for (TInt w = 0; w < sizeInPixels.iWidth; w++)
			{										
			bufferColour = ExtractRgb(scanline, w, iDisplayMode);
			
			// Pixels that lie inside the region should be set to a flag stripe colour, pixels
			// outside of the region should be white			
			if (regionOfInterest.Contains(TPoint(w,h)))
				{				
				if (((iDisplayMode != EGray256) && (bufferColour != KStripeColours[h/KStripeSize]))
					|| ((iDisplayMode == EGray256) && (bufferColour != TRgb::Gray256(KStripeColours[h/KStripeSize].Gray256()))))
					{
					INFO_PRINTF3(_L("Pixel comparison failed for pixel %d,%d"), w, h);
					TEST(EFalse);
					}			
				}
			else
				{
				if (bufferColour != KRgbWhite)
					{
					INFO_PRINTF3(_L("Pixel comparison failed for pixel %d,%d"), w, h);
					TEST(EFalse);
					}
				}
			}		
		}
	
	// End scanline access to the extended bitmap	
	iRasterizer->EndBitmap(bitmapId);
	extendedBitmap->EndDataAccess();
	
	// Clean up
	CleanupStack::Pop(1, extendedBitmap);
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
TVerdict CTRasterizer::doTestStepL()
	{
	TestLoadRasterizerDllL();
	
	// Tests that require testing in multiple display modes
	for (TInt i = 0; i < KNumDisplayModes; i++)
		{
		iDisplayMode = KDisplayModes[i];		
				
		TestScanLineL();
		TestMultipleScanLinesL();
		TestRegionOfInterestL();
		}
	
	return TestStepResult();
	}

/** Helper function for getting a TRgb colour value from a buffer given a pixel offset and 
a display mode.
@param aBuffer A buffer to extract the TRgb from
@param aPixelOffset The pixel position in the buffer to extract the TRgb from
@param aDispMode The display mode of the buffer
@return The TRgb value found at the pixel position specified
 */
TRgb CTRasterizer::ExtractRgb(const TUint8* aBuffer, TInt aPixelOffset, TDisplayMode aDispMode)
	{
	switch (aDispMode)
		{
	case EGray2:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 3));
		if (byte & (1 << (aPixelOffset & 7)))
			return KRgbWhite;
		return KRgbBlack;
		}
	case EGray4:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 2));
		byte >>= ((aPixelOffset & 3) << 1);
		return TRgb::Gray4(byte & 3);
		}
	case EGray16:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 1));
		if (aPixelOffset & 1)
			byte >>= 4;
		return TRgb::Gray16(byte & 0xf);
		}
	case EGray256:
		return TRgb::Gray256(*(aBuffer + aPixelOffset));
	case EColor16:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 1));
		if (aPixelOffset & 1)
			byte >>= 4;
		return TRgb::Color16(byte & 0xf);
		}
	case EColor256:
		return TRgb::Color256(*(aBuffer + aPixelOffset));
	case EColor4K:
		{
		TUint16 doubleByte = *(((TUint16*)aBuffer) + aPixelOffset);
		return TRgb::Color4K(doubleByte & 0xfff);
		}
	case EColor64K:
		{
		TUint16 doubleByte = *(((TUint16*)aBuffer) + aPixelOffset);
		return TRgb::Color64K(doubleByte);
		}
	case EColor16M:
		{
		aBuffer += aPixelOffset * 3;
		TInt value = *aBuffer++;
		value |= *aBuffer++ << 8;
		value |= *aBuffer << 16;
		return TRgb::Color16M(value);
		}
	case ERgb:
		return *(((TRgb*)aBuffer) + aPixelOffset);
	case EColor16MU:
		{
		// Note this is | with 0xFF000000 to match the example rasterizer which sets
		// the alpha to 0xFF when drawing using EColor16MU
		return TRgb::Color16MU((*(((TUint32*)aBuffer) + aPixelOffset)) | 0xFF000000);
		}
	case EColor16MA:
		{
		return TRgb::Color16MA(*(((TUint32*)aBuffer) + aPixelOffset));
		}	
	case EColor16MAP:
		{
		return TRgb::_Color16MAP(*(((TUint32*)aBuffer) + aPixelOffset));
		}	
	default:
		break;
		};
	return KRgbBlack;
	}

/** Helper function for creating an extended bitmap. 
@param aColours A pointer to an array of three colours to use when creating the extended bitmap, 
these colours define the colours used when drawing the stripes in the extended bitmap.
@param aHorizontalStripe ETrue for horizontal stripes, EFalse for vertical stripes.
@param aSizeInPixels The size of the extended bitmap to create.
@return A pointer to a newly created extended bitmap that has been pushed on to the cleanup stack.
 */
CFbsBitmap* CTRasterizer::CreateExtendedBitmapLC(const TRgb* aColours, TBool aHorizontalStripe, TSize aSizeInPixels)
	{	
	// Set up the buffer containing the three TRgb values and one TUint8 value that the rasterizer expects
	TInt dataSize = (sizeof(TRgb)*3) + sizeof(TUint8); // estimate the size to be written
	TUint8* data = new (ELeave)TUint8[dataSize];
	CleanupArrayDeletePushL(data);
	RMemWriteStream writeStream;
	writeStream.Open(data, dataSize);
	CleanupClosePushL(writeStream);		
	writeStream << aColours[0]; 		
	writeStream << aColours[1];		
	writeStream << aColours[2];
	TUint8 stripe = aHorizontalStripe; // EFalse is vertical stripes, ETrue is horizontal stripes
	writeStream << stripe;
	dataSize = writeStream.Sink()->TellL(MStreamBuf::EWrite).Offset(); // get the actual size written
	CleanupStack::PopAndDestroy(&writeStream);					

	CFbsBitmap* extendedBitmap = new(ELeave) CFbsBitmap;
		
	TInt err = extendedBitmap->CreateExtendedBitmap(aSizeInPixels, 
													iDisplayMode,
													KUidExampleExtendedBitmap,
													data,
													dataSize);
	CleanupStack::PopAndDestroy(data);
	CleanupStack::PushL(extendedBitmap);
	TESTL(err == KErrNone);
			
	return extendedBitmap;
	}
