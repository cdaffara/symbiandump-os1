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

#include "textendedbitmapcommon.h"
#include "textendedbitmaplegacy.h"
#include "tfbs.h"
#include <s32mem.h> 
#include <bautils.h>

const TUint8 KTestData[]		= "Extended bitmap test data 123456";
const TInt KTestDataSize		= sizeof(KTestData);
const TInt KTestWidth 			= 50;
const TInt KTestHeight			= 50;
const TSize KSizeInPixels 		= TSize(KTestWidth,KTestHeight);
const TDisplayMode KDisplayMode	= EColor64K;
const TInt KBpp = 2; // 2 bpp for EColor64K

// Test mbm files used by CTExtendedBitmapLegacy::TestApisThatCallResetL().
// These are used to check the operation of CFbsBitmap::Load() when used with
// extended bitmaps. 
_LIT(KRomBmpFilename, "z:\\system\\data\\rf1.mbm");
_LIT(KRamBitmapFilenameOnZ, "z:\\system\\data\\16RAM2.mbm");
_LIT(KRamBitmapFilename, "c:\\textendedbitmaplegacy_16RAM2.mbm");

LOCAL_D RFs	TheFs;

CTExtendedBitmapLegacy::CTExtendedBitmapLegacy(CTestStep* aStep):
	CTFbsBase(aStep, ETrue)
	{
	}

CTExtendedBitmapLegacy::~CTExtendedBitmapLegacy()
	{	
	((CTExtendedBitmapLegacyStep*)iStep)->CloseTMSGraphicsStep();
	}

void CTExtendedBitmapLegacy::RunFbsTestL(TInt aCurTestCase)
    {
    ((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0030"));
		TestDuplicateL();
		break;
	case 2:
		((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0031"));
		TestAccessDeniedL();
		break;
	case 3:
		((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0032"));
		TestNotSupportedL();
		break;
	case 4:
		((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0033"));
		TestTwipsL();		
		break;
	case 5:
		((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0034"));
		TestGettersL();
		break;
	case 6:
		((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0035"));
		TestGetScanLineL();
		break;
	case 7:
		((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0036"));
		TestApisThatCallResetL();
		break;
	case 8:
		((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0037"));
		TestApisThatLeaveL();
		break;
	case 9:
		((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0038"));
		TestNotSupportedNoReturnL();
		break;
	case 10:
		((CTExtendedBitmapLegacyStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-EXTENDEDBITMAP-0039"));
		TestGetPixelL();
		// Fall through as last test case
	default:
		SetLastTestCase();
		break;
		}
	((CTExtendedBitmapLegacyStep*)iStep)->RecordTestResultL();
    }
   

/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0030

@SYMTestCaseDesc
	Create an extended bitmap then duplicate it by calling CFbsBitmap::Duplicate()

@SYMPREQ 
	PREQ2096
	
@SYMREQ
	REQ10847

@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	Create an extended bitmap using test data and test Uid.
	Call Duplicate() on the extended bitmap.
	Retrieves and validates the bitmap size, display mode, Uid, data and data size.
		
@SYMTestExpectedResults
	Extended bitmap created and duplicated and information should match exactly.
*/
void CTExtendedBitmapLegacy::TestDuplicateL()
	{
	INFO_PRINTF1(_L("Duplicate - create a duplicate of an extended bitmap"));
	
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);
		
	CFbsBitmap* duplicateBmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(duplicateBmp);
	err = duplicateBmp->Duplicate(bmp->Handle());
	TESTNOERRORL(err);
	
	// check that the size, display mode etc are all correct
	TESTSIZEEQUALL(duplicateBmp->SizeInPixels(), KSizeInPixels);
	TESTEQUALL(duplicateBmp->DisplayMode(), KDisplayMode);
	TESTEQUALL(duplicateBmp->DisplayMode(), KDisplayMode);
	TESTEQUALL(duplicateBmp->ExtendedBitmapType(), KUidTestExtendedBitmap);
		
	duplicateBmp->BeginDataAccess();
			
	const TUint8* returnedDataAddress = reinterpret_cast<TUint8*>(duplicateBmp->DataAddress());
	TESTNOTEQUALL(returnedDataAddress, NULL);
	
	// compare the size of the data in the duplicate bitmap with the size of the original data, they should be the same
	TInt returnedDataSize = duplicateBmp->DataSize();
	TESTEQUALL(returnedDataSize, KTestDataSize);
	
	// compare the data returned from the duplicate bitmap with the original data, they should be the same
	TInt res = Mem::Compare(returnedDataAddress, returnedDataSize, KTestData, KTestDataSize);
	TESTEQUALL(res, 0);
	
	duplicateBmp->EndDataAccess(ETrue);	
	
	CleanupStack::PopAndDestroy(2, bmp);	
	}

/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0031

@SYMTestCaseDesc
	Test that all the legacy CFbsBitmap APIs (i.e. APIs that existed before PREQ2096)
	that should return KErrAccessDenied for an extended bitmap do so correctly.

@SYMPREQ 
	PREQ2096
	
@SYMREQ
	REQ10847
	REQ10856
	REQ10857
	
@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	Create an extended bitmap using test data and test Uid.
	Call all the legacy APIs that should return KErrAccessDenied:
		- Resize()
		- Compress()
		- CompressInBackground()
		- SwapWidthAndHeight()		
		
@SYMTestExpectedResults
	The legacy APIs tested should all return KErrAccessDenied.
*/
void CTExtendedBitmapLegacy::TestAccessDeniedL()
	{
	INFO_PRINTF1(_L("AccessDenied - test legacy CFbsBitmap APIs that should return KErrAccessDenied when used with an extended bitmap"));
		
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);

	INFO_PRINTF1(_L("Resize()"));
	err = bmp->Resize(TSize(100,100));
	TESTEXPECTEDERRORL(err, KErrAccessDenied);
	
	// This covers both versions of Compress() as one calls the other
	INFO_PRINTF1(_L("Compress()"));
	err = bmp->Compress();
	TESTEXPECTEDERRORL(err, KErrAccessDenied);
	
	// This covers both synchronous versions of CompressInBackground() as one calls the other
	INFO_PRINTF1(_L("CompressInBackground()"));
	err = bmp->CompressInBackground();
	TESTEXPECTEDERRORL(err, KErrAccessDenied);

	// This covers both asynchronous versions of CompressInBackground() as one calls the other
	INFO_PRINTF1(_L("CompressInBackground() (asynchronous)"));
	TRequestStatus requestStatus;
	bmp->CompressInBackground(requestStatus);
	User::WaitForRequest(requestStatus);
	TESTEXPECTEDERRORL(requestStatus.Int(), KErrAccessDenied);
	
	INFO_PRINTF1(_L("SwapWidthAndHeight()"));
	err = bmp->SwapWidthAndHeight();
	TESTEXPECTEDERRORL(err, KErrAccessDenied);	

	CleanupStack::PopAndDestroy(bmp);
	}

/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0032

@SYMTestCaseDesc
	Test that all the legacy CFbsBitmap APIs (i.e. APIs that existted before PREQ2096)
	that should return KErrNotSupported for an extended bitmap do so correctly, or
	if they have no return value and are not supported that they return cleanly.

@SYMPREQ 
	PREQ2096

@SYMREQ
	REQ10847
	REQ10856
	REQ10857
	
@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	Create an extended bitmap using test data and test Uid.
	Call all the legacy APIs that should return KErrNotSupported:		
		- Save()
		- GetPalette()
		- SetDisplayMode()							
		
@SYMTestExpectedResults
	The legacy APIs tested should all return KErrNotSupported where they have return 
	values, they should not cause errors where they are not supported but don't return
	an error value.
*/
void CTExtendedBitmapLegacy::TestNotSupportedL()
	{
	INFO_PRINTF1(_L("NotSupported - test legacy CFbsBitmap APIs that are not supported when used with an extended bitmap"));
		
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;	
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);
	
	// Methods that should return KErrNotSupported
	INFO_PRINTF1(_L("Save() - filename"));
	_LIT(KFilename, "C:\\temp.mbm");
	TESTEXPECTEDERRORL(bmp->Save(KFilename), KErrNotSupported);	
	
	INFO_PRINTF1(_L("Save() - RFile"));
	RFile file;
	err = file.Open(TheFs, KFilename, EFileWrite);
	TESTNOERRORL(err);
	CleanupClosePushL(file);
	TESTEXPECTEDERRORL(bmp->Save(file), KErrNotSupported);
	CleanupStack::PopAndDestroy(&file);
	
	INFO_PRINTF1(_L("GetPalette()"));
	CPalette* palette;
	TESTEXPECTEDERRORL(bmp->GetPalette(palette), KErrNotSupported);				
	
	INFO_PRINTF1(_L("SetDisplayMode()"));
	err = bmp->SetDisplayMode(EColor16MU);
	TESTEXPECTEDERRORL(err, KErrNotSupported);
		
			
	CleanupStack::PopAndDestroy(bmp);
	}

/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0033

@SYMTestCaseDesc
	Test that all CFbsBitmap methods that mention Twips work correctly with extended bitmaps.

@SYMPREQ 
	PREQ2096
	
@SYMREQ
	REQ10847
	REQ10857
	REQ10858

@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	Create an extended bitmap using test data and test Uid.
	Call all the legacy APIs that mention Twips:
		- SetSizeInTwips()
		- SizeInTwips()
		- HorizontalPixelsToTwips()
		- HorizontalTwipsToPixels()		
		- VerticalPixelsToTwips()
		- VerticalTwipsToPixels()		
		
@SYMTestExpectedResults
	All methods should work correctly with extended bitmaps.
*/
void CTExtendedBitmapLegacy::TestTwipsL()
	{
	INFO_PRINTF1(_L("Twips - test legacy CFbsBitmap Twips APIs work correctly when used with an extended bitmap"));
	const TSize KSizeInTwips(20,30);
		
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;	
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);
		
	INFO_PRINTF1(_L("SetSizeInTwips()"));
	bmp->SetSizeInTwips(KSizeInTwips);
	
	INFO_PRINTF1(_L("SizeInTwips()"));
	TESTSIZEEQUALL(bmp->SizeInTwips(), KSizeInTwips);
	
	INFO_PRINTF1(_L("HorizontalPixelsToTwips()"));
	TESTEQUALL(bmp->HorizontalPixelsToTwips(KSizeInPixels.iWidth), KSizeInTwips.iWidth);
	
	INFO_PRINTF1(_L("HorizontalTwipsToPixels()"));
	TESTEQUALL(bmp->HorizontalTwipsToPixels(KSizeInTwips.iWidth), KSizeInPixels.iWidth);
	
	INFO_PRINTF1(_L("VerticalPixelsToTwips()"));
	TESTEQUALL(bmp->VerticalPixelsToTwips(KSizeInPixels.iHeight), KSizeInTwips.iHeight);
	
	INFO_PRINTF1(_L("VerticalTwipsToPixels()"));
	TESTEQUALL(bmp->VerticalTwipsToPixels(KSizeInTwips.iHeight), KSizeInPixels.iHeight);
	
	const TDisplayMode displayMode[] = {EColor256, EColor4K, EColor16M, EColor16MU, EColor64K, 
						   EGray256, EGray16, EGray4, EGray2, EColor16, EColor16MA, EColor16MAP};
	TInt ii = 0;
	err = KErrNotSupported;
	CFbsScreenDevice* scd = NULL;
	for(;(ii<TInt(sizeof(displayMode)/sizeof(displayMode[0]))) && (err == KErrNotSupported);++ii)
		{
		TRAP(err, scd = CFbsScreenDevice::NewL(_L("scdv"),displayMode[ii]));
		}
	if (err != KErrNone)
		{
		_LIT(KLog,"Failed to create screen device %S return value %d");
		INFO_PRINTF3(KLog,&ColorModeName(displayMode[ii]),err);
		}
	else
		{
		_LIT(KLog,"Created Screen Device with mode %S");
		INFO_PRINTF2(KLog,&ColorModeName(displayMode[ii]));
		}
	TESTNOERRORL(err);
	// The size of screen that this is tested on will be different depending on where it is 
	// tested, so just check that this does not cause an error, don't check the result
	INFO_PRINTF1(_L("SetSizeInTwips() (screen device)"));
	bmp->SetSizeInTwips(scd);
	
	delete scd;
	CleanupStack::PopAndDestroy(1, bmp);
	}

/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0034

@SYMTestCaseDesc
	Test the simple getter CFbsBitmap APIs such as Handle().

@SYMPREQ 
	PREQ2096
	
@SYMREQ
	REQ10847
	REQ10857
	REQ10858

@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	Create an extended bitmap using test data and test Uid.
	Call the following simple getter APIs:		
		- InitialDisplayMode()
		- IsRomBitmap()
		- Handle()		
		- Header()		
		- IsLargeBitmap()
		- DataStride()
		- HardwareBitmapHandle()		
		- SerialNumber()
		- IsCompressedInRAM()
		- GetAllBitmapHandles()
		
@SYMTestExpectedResults
	No errors should occur.
*/
void CTExtendedBitmapLegacy::TestGettersL()
	{
	INFO_PRINTF1(_L("Test simple getter APIs"));

	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;		
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);	

	INFO_PRINTF1(_L("InitialDisplayMode()"));
	TESTEQUALL(bmp->InitialDisplayMode(), KDisplayMode);
		
	INFO_PRINTF1(_L("IsRomBitmap()"));
	TESTTRUEL(bmp->IsRomBitmap());
	
	INFO_PRINTF1(_L("Handle()"));
	TESTNOTEQUALL(bmp->Handle(), 0);
	
	INFO_PRINTF1(_L("Header()"));
	SEpocBitmapHeader header = bmp->Header();
	TESTSIZEEQUALL(header.iSizeInPixels, KSizeInPixels);
	TESTSIZEEQUALL(header.iSizeInTwips, TSize(0,0));
	TESTEQUALL(header.iBitsPerPixel, 16);
	TESTEQUALL(header.iColor, 1);
	TESTEQUALL(header.iPaletteEntries, 0);
	TESTEQUALL(header.iCompression, EProprietaryCompression);		
	
	INFO_PRINTF1(_L("IsLargeBitmap()"));
	TESTTRUEL(bmp->IsLargeBitmap());
	
	INFO_PRINTF1(_L("DataStride()"));
	TESTEQUALL(bmp->DataStride(), KSizeInPixels.iWidth*KBpp);
	
	INFO_PRINTF1(_L("HardwareBitmapHandle()"));
	TESTEQUALL(bmp->HardwareBitmapHandle(), 0);
	
	INFO_PRINTF1(_L("SerialNumber()"));
	TESTNOTEQUALL(bmp->SerialNumber(), 0);
	
	INFO_PRINTF1(_L("IsCompressedInRAM()"));
	TESTEQUALL(bmp->IsCompressedInRAM(), EFalse);
	
	// The touch count changes for a bitmap (not a ROM bitmap) when it is resized etc, it should not change
	// when Resize() is called on an extended bitmap 
	INFO_PRINTF1(_L("TouchCount()"));
	TESTEQUALL(bmp->TouchCount(), 0);
	bmp->Resize(TSize(100,100));
	TESTEQUALL(bmp->TouchCount(), 0);
	
	// Bitmaps (not ROM bitmaps) become volatile if DataAddress() is called outside BeginDataAccess() and EndDataAccess(),
	// should never be set for extended bitmaps
	INFO_PRINTF1(_L("IsVolatile()"));
	TESTEQUALL(bmp->IsVolatile(), EFalse);
	bmp->DataAddress();
	TESTEQUALL(bmp->IsVolatile(), EFalse);
	
	RArray<TInt> handles;
	CleanupClosePushL(handles);
	INFO_PRINTF1(_L("GetAllBitmapHandles()"));
	TESTNOERRORL(bmp->GetAllBitmapHandles(handles));
	TESTTRUEL(handles.Count() > 0);	
	
	CleanupStack::PopAndDestroy(2, bmp);
	}

/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0035

@SYMTestCaseDesc
	Test GetScanLine with extended bitmaps where no rasterizer is present.

@SYMPREQ 
	PREQ2096
	
@SYMREQ
	REQ10847
	REQ10857
	REQ10858

@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	Create an extended bitmap using test data and test Uid that is not supported by a rasterizer.
	Call GetScanLine().		
				
@SYMTestExpectedResults
	GetScanLine() should return a white scanline of the length requested, no errors should occur.
*/
void CTExtendedBitmapLegacy::TestGetScanLineL()
	{
	INFO_PRINTF1(_L("Test CFbsBitmap::GetScanLine() where there is no supported rasterizer for the passed extended bitmap Uid"));

	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);
		
	const TInt KScanlineLength = KTestWidth*KBpp;
	TBuf8<KScanlineLength> buf; 
	bmp->GetScanLine(buf, TPoint(0,0), KTestWidth, KDisplayMode);
	TESTEQUALL(buf.Size(), KScanlineLength);
	
	// Check that the scanline has been set to white
	for (TInt i = 0; i < KScanlineLength; i++)
		{		
		TESTEQUALL(buf[i], 255);
		}
	
	CleanupStack::PopAndDestroy(bmp);
	}

/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0036

@SYMTestCaseDesc
	Test an extended bitmap with all of the CFbsBitmap APIs that call Reset().

@SYMPREQ 
	PREQ2096
	
@SYMREQ
	REQ10847

@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	For each API:
		- Create an extended bitmap using test data and test Uid that is not supported by a rasterizer.
		- Call the API.
	The following APIs are tested:		
		- Load()
		- LoadAndCompress()
		- Internalize()
		- SetRomBitmapL()
				
@SYMTestExpectedResults
	All APIs tested should complete with no errors, the resultant CFbsBitmap
	should not be an extended bitmap as it will have been reset.
*/
void CTExtendedBitmapLegacy::TestApisThatCallResetL()
	{
	INFO_PRINTF1(_L("Test CFbsBitmap APIs that Call Reset()"));

	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);
	TESTEQUALL(bmp->ExtendedBitmapType(), KUidTestExtendedBitmap);

	INFO_PRINTF1(_L("Load()"));
	err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);
	TESTEQUALL(bmp->ExtendedBitmapType(), KUidTestExtendedBitmap); // should be an extended bitmap again
	TESTNOERRORL(bmp->Load(KRamBitmapFilename));
	TESTNOTEQUALL(bmp->ExtendedBitmapType(), KUidTestExtendedBitmap); // should no longer be an extended bitmap

	INFO_PRINTF1(_L("LoadAndCompress()"));
	err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);
	TESTEQUALL(bmp->ExtendedBitmapType(), KUidTestExtendedBitmap); // should be an extended bitmap again
	TESTNOERRORL(bmp->LoadAndCompress(KRamBitmapFilename));
	TESTNOTEQUALL(bmp->ExtendedBitmapType(), KUidTestExtendedBitmap); // should no longer be an extended bitmap

	INFO_PRINTF1(_L("InternalizeL()"));
	CBufFlat* buf = CBufFlat::NewL(512);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	bmp->ExternalizeL(writeStream); // externalize a normal bitmap
	writeStream.Close();
	err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);
	TESTEQUALL(bmp->ExtendedBitmapType(), KUidTestExtendedBitmap); // should be an extended bitmap again
	RBufReadStream readStream(*buf,0);
	bmp->InternalizeL(readStream);
	readStream.Close();
	TESTNOTEQUALL(bmp->ExtendedBitmapType(), KUidTestExtendedBitmap); // should no longer be an extended bitmap
	
	TUint32* romAddress = NULL;
	if(!CFbsBitmap::IsFileInRom(KRomBmpFilename, romAddress)) //check any rom bitmap
		{
		INFO_PRINTF2(_L("Skipping tests of ROM bitmaps since file \"%S\" is reported to not be a ROM bitmap."), &KRomBmpFilename);
		INFO_PRINTF1(_L("This should only occur on non-XIP ROMs, e.g. NAND ROMs, where ROM bitmaps aren't supported."));		
		}
	else
		{
		INFO_PRINTF1(_L("SetRomBitmapL()"));
		CFbsBitmapEx* romBitmap = new(ELeave) CFbsBitmapEx;
		CleanupStack::PushL(romBitmap);
		INFO_PRINTF2(_L("... attempting to load %S"), &KRomBmpFilename);
		err = romBitmap->Load(KRomBmpFilename);
		TESTNOERRORL(err);
		TInt size = 0;
		INFO_PRINTF1(_L("... calling SetRomBitmapL()"));
		bmp->SetRomBitmapL(romBitmap->BitmapAddress(), size);
		TESTNOTEQUALL(bmp->ExtendedBitmapType(), KUidTestExtendedBitmap); // should no longer be an extended bitmap
		TEST(bmp->SerialNumber() == -TInt64(static_cast<TUint32>(bmp->Handle()))); // A ROM bitmap's handle is its address pointer, its serial number is -ve the address pointer
		
		CleanupStack::PopAndDestroy(1, romBitmap);
		}
	
	CleanupStack::PopAndDestroy(2, bmp);
	}

/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0037

@SYMTestCaseDesc
	Test the CFbsBitmap APIs that should leave when called by an extended bitmap.

@SYMPREQ 
	PREQ2096
	
@SYMREQ
	REQ10847
	REQ10856
	REQ10857

@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	For each API:
		- Create an extended bitmap using test data and test Uid that is not supported by a rasterizer.
		- Call the API.
	The following APIs are tested:
		- ExternalizeL()
		- ExternalizeRectangleL()		
				
@SYMTestExpectedResults
	All APIs tested should leave with the error KErrNotSupported.
*/
void CTExtendedBitmapLegacy::TestApisThatLeaveL()
	{
	INFO_PRINTF1(_L("ApisThatLeave - test legacy CFbsBitmap APIs that leave when used with an extended bitmap"));
		
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;	
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);
	
	INFO_PRINTF1(_L("ExternalizeL()"));	
	CBufFlat* buf = CBufFlat::NewL(512);
	CleanupStack::PushL(buf);		
	RBufWriteStream writeStream(*buf,0);
	CleanupClosePushL(writeStream);
	TRAP(err, bmp->ExternalizeL(writeStream));
	TESTEXPECTEDERRORL(err, KErrNotSupported);	
		
	INFO_PRINTF1(_L("ExternalizeRectangleL()"));
	RBufWriteStream writeStream2(*buf,0);
	CleanupClosePushL(writeStream2);
	TRAP(err, bmp->ExternalizeRectangleL(writeStream2, TRect(10,10,100,100)));
	TESTEXPECTEDERRORL(err, KErrNotSupported);	
	
	CleanupStack::PopAndDestroy(4, bmp);
	}

/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0038

@SYMTestCaseDesc
	Test that APIs that do not return an error value or leave, yet are not supported
	for extended bitmaps, do not cause an error.

@SYMPREQ 
	PREQ2096
	
@SYMREQ
	REQ10847
	REQ10856
	REQ10857

@SYMTestPriority  
	High
	
@SYMTestStatus 
	Implemented
	
@SYMTestActions
	For each API:
		- Create an extended bitmap using test data and test Uid that is not supported by a rasterizer.
		- Call the API.
	The following APIs are tested:
		- PaletteAttributes()
		- SetPalette()		
				
@SYMTestExpectedResults
	All APIs tested should complete with no errors.
*/
void CTExtendedBitmapLegacy::TestNotSupportedNoReturnL()
	{
	INFO_PRINTF1(_L("NotSupportedNoReturn - test legacy CFbsBitmap APIs that are not supported when used with an extended bitmap but do not return errors"));
		
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;	
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);
	
	// Methods that are not supported and do not return errors values	
	INFO_PRINTF1(_L("PaletteAttributes()"));
	TBool modifiable;
	TInt numEntries;
	bmp->PaletteAttributes(modifiable, numEntries);
	
	INFO_PRINTF1(_L("SetPalette()"));	
	bmp->SetPalette(NULL);
	
	CleanupStack::PopAndDestroy(bmp);
	}

/**
@SYMTestCaseID
	GRAPHICS-FBSERV-EXTENDEDBITMAP-0039

@SYMTestCaseDesc
	Test GetPixel with extended bitmaps where no rasterizer is present 
	that supports the extended bitmap type.

@SYMCR
	CR1804

@SYMREQ
	REQ10858

@SYMTestPriority
	High

@SYMTestStatus
	Implemented

@SYMTestActions
	Create an extended bitmap using test data and test Uid that is not supported by a rasterizer.
	Call GetPixel().

@SYMTestExpectedResults
	GetPixel() should return white pixels at the points requested, no errors should occur.
*/
void CTExtendedBitmapLegacy::TestGetPixelL()
	{
	INFO_PRINTF1(_L("Test CFbsBitmap::GetPixel() where there is no supported rasterizer for the passed extended bitmap Uid"));

	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	TInt err = bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize);
	TESTNOERRORL(err);

	// Check that the absence of a rasteriser causes legacy off-screen rendering to
	// treat the extended bitmap as if all pixels were white.
	TRgb color;
	for (TInt y = 0; y < KTestHeight; ++y)
		{
		for (TInt x = 0; x < KTestWidth; ++x)
			{
			bmp->GetPixel(color, TPoint(x,y));
			TESTTRUEL(color == KRgbWhite);
			}
		}

	CleanupStack::PopAndDestroy(bmp);
	}

__CONSTRUCT_STEP__(ExtendedBitmapLegacy)

void CTExtendedBitmapLegacyStep::TestSetupL()
	{
	// Copy files needed by the tests to c:
	TheFs.Connect();
	TInt err = BaflUtils::CopyFile(TheFs, KRamBitmapFilenameOnZ, KRamBitmapFilename);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Error copying test mbm file from rom to ram: %d"), err);
		User::Leave(err);
		}
	}
	
void CTExtendedBitmapLegacyStep::TestClose()
	{
	// Delete files used by the tests from c:
	BaflUtils::DeleteFile(TheFs, KRamBitmapFilename);	
	TheFs.Close();
	}
