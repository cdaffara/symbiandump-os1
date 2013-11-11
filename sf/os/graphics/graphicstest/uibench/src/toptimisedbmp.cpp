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

#include "toptimisedbmp.h"
#include "fbsmessage.h"
#include <test/tefunit.h>
#include <hal.h>

// The following line must be disabled for testing with the background compression code
// When enabled foreground bitmap compression will take place.
// #define FORCE_FOREGROUND_COMPRESSION

// Number of times to repeat the tests
const TInt KRepeatTestCount = 100;

// The source bitmaps, only use the bitmaps from UIBENCH
_LIT( KmbmFilesInRom, "z:\\system\\data\\uibench_*.mbm" );

// The 16bit rle compressed source bitmap on Z drive
_LIT( Kmbm16bppRleComprFileInRom, "z:\\system\\data\\rlecompr_16bpp.mbm" );

// The 32bit alpha rle compressed source bitmap on Z drive
_LIT( Kmbm32abppRleComprFileInRom, "z:\\system\\data\\rlecompr_32abpp.mbm" );

// The 16bit rle compressed source bitmap on C drive
_LIT( Kmbm16bppRleComprFileOnC, "c:\\compressedBitmaps\\rlecompr_16bpp.mbm" );

#ifdef _DEBUG
// The 32bit alpha  rle compressed source bitmap on C drive
_LIT( Kmbm32abppRleComprFileOnC, "c:\\compressedBitmaps\\rlecompr_32abpp.mbm" );
#endif

// The resouce mbm file
_LIT( KmbmResouceFile, "z:\\resource\\32bit20col.mbm" );

// The directory to copy the bitmaps to
_LIT( KDirectoryFilesOnC , "c:\\bitmapOptimisation\\" );

// The directory to copy the compressed bitmaps to
_LIT( KDirectoryComprBmpFilesOnC , "c:\\compressedBitmaps\\" );

CTOptimisedBmp::CTOptimisedBmp()
	{
	SetTestStepName(KTOptimisedBmpStep);
	}

CTOptimisedBmp::~CTOptimisedBmp()
	{
	// clear test files
	if (iFileMan)
		{
		TBuf <255> filePath;
		_LIT(KFormat, "%S*.mbm");
		filePath.Format(KFormat, &KDirectoryFilesOnC);
		iFileMan->Delete(filePath);
		
		//remove filepath for compressed bitmap
		filePath.Format(KFormat, &KDirectoryComprBmpFilesOnC);		
		iFileMan->Delete(filePath);
		
		delete iFileMan;
		}	
	iFs.RmDir(KDirectoryFilesOnC);
	
	//remove directory for compressed bitmap
	iFs.RmDir(KDirectoryComprBmpFilesOnC);	
	iFs.Close();
	}
		
/**
Override of base class virtual
 
@return - TVerdict code
*/
TVerdict CTOptimisedBmp::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	
	// copy all of the files in ROM z: to c:
	TInt err =  iFs.Connect();
	User::LeaveIfError(err);		
	iFs.ShareProtected();
	err =  iFs.MkDir(KDirectoryFilesOnC);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	//create directory to store rle-compressed bitmap	
	err =  iFs.MkDir(KDirectoryComprBmpFilesOnC);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
		
	iFileMan = CFileMan::NewL( iFs );
	
	err =  iFileMan->Copy( KmbmFilesInRom, KDirectoryFilesOnC );
	TESTL(err == KErrNone);
	
	//Copying compressed mbm files to C drive
	err =  iFileMan->Copy( Kmbm16bppRleComprFileInRom, KDirectoryComprBmpFilesOnC );
	TESTL(err == KErrNone);

	err =  iFileMan->Copy( Kmbm32abppRleComprFileInRom, KDirectoryComprBmpFilesOnC );
	TESTL(err == KErrNone);
	
	return TestStepResult();
	}
	
/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTOptimisedBmp::doTestStepL()
	{
	
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0021

@SYMTestCaseDesc
The test determines how long it takes to compress bitmaps, and how long it takes to bitblt the original and the compressed bitmaps.

@SYMTestActions
1. Over a number of iterations, measure time taken to compress a bitmap loaded from mbm file.  Each iteration the mbm is loaded from scratch.
2. Over a number of iterations, measure time taken to blit an uncompressed bitmap loaded from mbm file. This mbm file is the same as in step 1.
3. Over a number of iterations, measure time taken to blit the compressed bitmap loaded from step 1.
4. Check that the blits of the uncompressed bitmap and the compressed bitmap are the same.

Each of these steps is performed with various screen display modes and various display modes of the bitmap.

@SYMTestExpectedResults
Test should pass and display total and per bitmap compression, bitblt for uncompressed and compressed time.
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0021"));
	CompressedBitmapTestL();	
	RecordTestResultL();
	
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0058

@SYMTestCaseDesc
The test determines how long it takes to load 16bpp compressed bitmaps.

@SYMDEF INC095318

@SYMTestPriority High

@SYMTestActions
Compare the results over time.

@SYMTestExpectedResults
Test should pass and logs the time taken to load compressed bitmap .
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0058"));
	TestDecompressBitmapL(Kmbm16bppRleComprFileOnC, _L("Load-Compressed-Bitmap-16bit"));
	RecordTestResultL();

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0059

@SYMTestCaseDesc
The test determines how long it takes to load 32 bpp compressed alpha bitmaps.

@SYMDEF DEF095361

@SYMTestPriority Low

@SYMTestActions
Compare the results over time.

@SYMTestExpectedResults
Test should pass and logs the time taken to load compressed bitmap .
*/
#ifdef _DEBUG
	//It uses Heap Allocation failure macro 
	//which is supported only for debug mode	

	SetTestStepID(_L("GRAPHICS-UI-BENCH-0059"));
	TestDecompressBitmapAltL(Kmbm32abppRleComprFileOnC, _L("Load-Compressed-Bitmap-32abit"));
	RecordTestResultL();
#endif
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0076"));
	GetScanLineTestL();
	RecordTestResultL();

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0085

@SYMTestCaseDesc
The test determines how long it takes to load a bitmap from the resource folder.

@SYMDEF DEF105049

@SYMTestPriority Medium

@SYMTestActions
Compare the results over time.

@SYMTestExpectedResults
Test should pass and display load time / bitmap.
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0085"));
	ResourceBitmapLoadL(KmbmResouceFile, 0, ETrue, _L("Resouce Bitmap Load"));
	RecordTestResultL();

	return TestStepResult();
	}
	
void CTOptimisedBmp::CompressedBitmapTestL()
	{
	_LIT(KTestName1, "CompressBitmap");
	_LIT(KTestName2, "BlitUncompressedBitmap");
	_LIT(KTestName3, "BlitCompressedBitmap");
	
	// strings for writing target output
	_LIT(KTargetString, "_TGT=");
	_LIT(KSourceString, "_SRC=");

	// for each of the files in the directory
	// load, compress & compare size before and after
	TBufC<KMaxFileName> fileName(KDirectoryFilesOnC);
	TInt dirLen = KDirectoryFilesOnC().Length();
	CDir* fileList;
	TInt err = iFs.GetDir(KDirectoryFilesOnC, KEntryAttNormal, ESortBySize, fileList);
	User::LeaveIfError(err);
	CleanupStack::PushL(fileList);

#ifdef FORCE_FOREGROUND_COMPRESSION
	_LIT(KForceForegroundCompression, "Force Foreground Compression");
	INFO_PRINTF2(KForceForegroundCompression);
	RFbsSession* fbs = RFbsSession::GetSession();
	if (!fbs)
		{
		ERR_PRINTF1("Error: fbs cannot be NULL");
		SetTestStepError();
		}		
	fbs->SendCommand(EFbsCompress, 1); // force to compress the bitmaps in the foreground
#endif // FORCE_FOREGROUND_COMPRESSION



	CFbsBitmap* fbsBitmap = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsBitmap);
	CFbsBitmap* fbsCompressed = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsCompressed);

	for (TInt dispModeIndex = 0; dispModeIndex < KNumValidDisplayModes; dispModeIndex++)
		{
		SetScreenModeL(KValidDisplayModes[dispModeIndex], ETrue);
		iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
		TSize scrSize = iScreenDevice->SizeInPixels();
		TPoint left(0 ,0);
		TPoint right(scrSize.iWidth / 2, 0);
		for (TInt ii = 0; ii < fileList->Count(); ++ii)
			{
			// Load uncompressed bitmap
			const TEntry entry((*fileList)[ii]);
			TPtr fnamePtr = fileName.Des();
			fnamePtr.SetLength(dirLen);
			fnamePtr.Append(entry.iName);

			err = fbsBitmap->Load(fnamePtr, 0, EFalse);
			if (err != KErrNone)
				{
				INFO_PRINTF3(_L("Error %d loading bitmap (%S)"), err, &fnamePtr);
				fbsBitmap->Reset();
				continue;
				}

			TDisplayMode bitmapDispMode = fbsBitmap->DisplayMode();  //same display mode used for fbsCompressed
			TDisplayMode screenDispMode = iScreenDevice->BitmapDevice().DisplayMode();
			TInt rpt;

			// Test speed of compressing an image
			iProfiler->InitResults();
			for (rpt = 0; rpt < KRepeatTestCount; ++rpt)
				{
				// Load bitmap to be compressed
				err = fbsCompressed->Load(fnamePtr, 0, EFalse);
				if (err != KErrNone)
					{
					INFO_PRINTF4(_L("Error %d loading the bitmap (%S), rpt = %d"), err, &fnamePtr, rpt);
					fbsCompressed->Reset();
					break;
					}
				iProfiler->StartTimer();
				fbsCompressed->Compress();
				iProfiler->MarkResultSetL();
				// on last iteration, don't reset fbsCompressed
				if (rpt < KRepeatTestCount-1)
					{
					fbsCompressed->Reset();
					}
				}
			iProfiler->ResultsAnalysis(KTestName1, CFbsBitGc::EGraphicsOrientationNormal, bitmapDispMode, screenDispMode, KRepeatTestCount);

			// Clip rect size to half-screen width and screen height
			TSize sizeInPixels = fbsBitmap->SizeInPixels();
			if (sizeInPixels.iWidth > scrSize.iWidth / 2)
				sizeInPixels.iWidth = scrSize.iWidth / 2;
			if (sizeInPixels.iHeight > scrSize.iHeight)
				sizeInPixels.iHeight = scrSize.iHeight;
			TRect blitRect(TPoint(0,0), sizeInPixels);
			TInt numPixels = sizeInPixels.iHeight * sizeInPixels.iWidth;

			// Test pixel rate of blitting uncompressed bitmap
			iProfiler->InitResults();
			for (rpt = 0; rpt < KRepeatTestCount; ++rpt)
				{
				iGc->BitBlt(left, fbsBitmap, blitRect);
				}
			iProfiler->MarkResultSetL();
			iProfiler->ResultsAnalysisPixelRate(KTestName2, CFbsBitGc::EGraphicsOrientationNormal, bitmapDispMode, screenDispMode, KRepeatTestCount, numPixels);

			// Test pixel rate of blitting compressed bitmap
			iProfiler->InitResults();
			for (rpt = 0; rpt < KRepeatTestCount; ++rpt)
				{
				iGc->BitBlt(right, fbsCompressed, blitRect);
				}
			iProfiler->MarkResultSetL();
			iProfiler->ResultsAnalysisPixelRate(KTestName3, CFbsBitGc::EGraphicsOrientationNormal, bitmapDispMode, screenDispMode, KRepeatTestCount, numPixels);

			// Sanity check that uncompressed and compressed bitmaps display the same
			CBitmapDevice& bmpDevice = iScreenDevice->BitmapDevice();
			if (iScreenDevice->isScreenDevice())
				{
				TEST(((CFbsScreenDevice&)bmpDevice).RectCompare(TRect(left, sizeInPixels), (CFbsScreenDevice&)bmpDevice, TRect(right, sizeInPixels)));
				}
			else
				{
				TEST(((CFbsBitmapDevice&)bmpDevice).RectCompare(TRect(left, sizeInPixels), (CFbsBitmapDevice&)bmpDevice, TRect(right, sizeInPixels)));
				}
			fbsBitmap->Reset();
			fbsCompressed->Reset();

			// Save offscreen bitmap to mbm file
			TBuf<KMaxFileName> testFileName;
			testFileName.Append(KTestName1);
			testFileName.Append(KTargetString);
			TBuf<10> string;
			string.Format(_L("%d"),screenDispMode);
			testFileName.Append(string);
			testFileName.Append(KSourceString);
			string.Format(_L("%d"),bitmapDispMode);
			testFileName.Append(string);
			WriteTargetOutput(testFileName);
			}
		}

	CleanupStack::PopAndDestroy(3, fileList); // fbsCompressed, fbsBitmap, fileList
	
#ifdef FORCE_FOREGROUND_COMPRESSION
	fbs->SendCommand(EFbsCompress, 0); // resume normal background compression
#endif // FORCE_FOREGROUND_COMPRESSION
	}


void CTOptimisedBmp::StripeBitmap(CFbsBitmap& aBitmap) // Compression friendly bitmap filling
	{
	TSize size = aBitmap.SizeInPixels();
	TInt dataLength = CFbsBitmap::ScanLineLength(size.iWidth,aBitmap.DisplayMode()) * size.iHeight;

	aBitmap.LockHeap();
	TUint8* bmpBits = (TUint8*)aBitmap.DataAddress();
	aBitmap.UnlockHeap();
	TUint8* bmpBitsLimit = bmpBits + dataLength;
	TInt64 seed = aBitmap.Handle();

	if (aBitmap.DisplayMode() != EColor4K)
		{
		while (bmpBits < bmpBitsLimit)
			{
			TUint8* tempBmpBitsLimit = Min(bmpBitsLimit, bmpBits + (TUint8)Math::Rand(seed));
			while (bmpBits < tempBmpBitsLimit)
				*bmpBits++ = 0;
			tempBmpBitsLimit = Min(bmpBitsLimit, bmpBits + (TUint8)Math::Rand(seed));
			while (bmpBits < tempBmpBitsLimit)
				*bmpBits++ = 0xff;
			}
		}
	else
		{
		Mem::FillZ(bmpBits,dataLength);

		while (bmpBits < bmpBitsLimit)
			{
			TUint8* tempBmpBitsLimit = Min(bmpBitsLimit, bmpBits + (TUint8)(Math::Rand(seed) * 2));
			while (bmpBits < tempBmpBitsLimit)
				{
				*bmpBits++ = 0;
				*bmpBits++ = 0;
				}
			tempBmpBitsLimit = Min(bmpBitsLimit, bmpBits + (TUint8)Math::Rand(seed));
			while (bmpBits < tempBmpBitsLimit)
				{
				*bmpBits++ = 0xff;
				*bmpBits++ = 0x0f;
				}
			}		
		}
	}

/**
Logs the time taken to load the bitmap file identified by the filename.
@param aFileName the mbm file that is to be loaded
@param aTestCaseName the name of the test case
*/
void CTOptimisedBmp::TestDecompressBitmapL(const TDesC& aFileName, const TDesC& aTestCaseName)
	{
	TInt err = KErrNone;
	iProfiler->InitResults(); //initializes the timer
	for (TInt rpt = 0; rpt < KRepeatTestCount; ++rpt)
		{
		CFbsBitmap* fbsCompressed = new (ELeave) CFbsBitmap;
		CleanupStack::PushL( fbsCompressed );
		
		//Load mbm file identified by aFileName
		err = fbsCompressed->Load( aFileName, 0, EFalse ); //when there is no memory constraints
		TEST(err == KErrNone);

		fbsCompressed->Reset();
		CleanupStack::PopAndDestroy( fbsCompressed );
		
		iProfiler->MarkResultSetL();
		} // rpt loop
	
	//Logs the time taken to load the bmp file	
	iProfiler->ResultsAnalysis(aTestCaseName, 0, 0, 0, KRepeatTestCount);
	}

#ifdef _DEBUG
/**
This test will only work correctly under UDEB builds of the OS.
It is same as CTOptimisedBmp::TestDecompressBitmapL function except that it also simulates the 
lack of memory use case while loading bitmap.

@param aFileName the mbm file that is to be loaded
@param aTestCaseName the name of the test case
*/
void CTOptimisedBmp::TestDecompressBitmapAltL(const TDesC& aFileName, const TDesC& aTestCaseName)
	{
	TInt err = KErrNone;
	RFbsSession* fbsSession = RFbsSession::GetSession();
	if (!fbsSession)
		{
		INFO_PRINTF1(_L("Error: fbs cannot be NULL"));
		ASSERT_NOT_NULL(fbsSession);
		}
	else
		{
		CFbsBitmap* fbsCompressed = new (ELeave) CFbsBitmap;
		CleanupStack::PushL( fbsCompressed );
		iProfiler->InitResults(); //initializes the timer
		for (TInt rpt = 0; rpt < KRepeatTestCount; ++rpt)
			{
			//Load mbm file identified by aFileName
			//Consider no memory case
			for (TInt count = 1; ; count++)
				{
				//Explicitly make the memory fails to check how will it loads bmp
				fbsSession->SendCommand(EFbsMessDefaultAllocFail,count);
				fbsSession->SendCommand(EFbsMessDefaultMark);
				fbsSession->SendCommand(EFbsMessUserMark);

				err = fbsCompressed->Load( aFileName, 0, EFalse );

				if (err == KErrNoMemory)
					{
					fbsSession->SendCommand(EFbsMessDefaultMarkEnd);
					fbsSession->SendCommand(EFbsMessUserMarkEnd);
					}
				else if (err == KErrNone)
					{
					fbsSession->SendCommand(EFbsMessUserMarkEnd);
					break;
					}
				}
			iProfiler->MarkResultSetL();
				fbsCompressed->Reset();
			} // rpt loop

		CleanupStack::PopAndDestroy( fbsCompressed );
		//Logs the time taken to load the bmp file	
		iProfiler->ResultsAnalysis(aTestCaseName, 0, 0, 0, KRepeatTestCount);
		}
	}
#endif

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0076

@SYMDEF DEF105392

@SYMTestCaseDesc
Tests the performance of GetScanLine function for various display modes.
Also it tests the performance of various GetScanLineColorXX
(where XX => display mode, for eg. GetScanLineColor64K)
and TScanLineDecompressor::CopyPixel functions.
To test the performance of GetScanLineColorXX and other functions for any display mode, it creates
a bitmap of that display mode and passes it to CTOptimisedBmp::ScanlineTestL function
along with the display mode which was used for creating it and
the other display mode which will be used for reading this bitmap while logging the performance.
@SYMTestActions
Compare the results over time, and before and after changes to GetScanLine and CopyPixel code.
@SYMTestExpectedResults
Test should pass and display average time
*/
void CTOptimisedBmp::GetScanLineTestL()
	{
	CFbsBitmap bmp;
	const TSize bmpSize(22,5);
	_LIT(KTestCaseName,"GetScanLine");
	const TDisplayMode KTestDisplayModes[] = {EColor4K, EColor64K, EColor16M, EColor16MU, EColor16MA, EColor16MAP};
	const TInt KTestDisplayModeCnt = sizeof(KTestDisplayModes)/sizeof(TDisplayMode);

	for(TInt srcDispModeIndex = 0; srcDispModeIndex < KTestDisplayModeCnt; srcDispModeIndex++)
		{
		TDisplayMode displayMode = KTestDisplayModes[srcDispModeIndex];
		User::LeaveIfError(bmp.Create(bmpSize,displayMode));
		StripeBitmap(bmp);
		bmp.Compress();
		for(TInt destDispModeIndex = 0; destDispModeIndex < KTestDisplayModeCnt; destDispModeIndex++)
			{
			ScanlineTestL(bmp,displayMode,KTestDisplayModes[destDispModeIndex],KTestCaseName);
			}
		bmp.Reset();
		}
	}

/**
It reads a bitmap (aBitmap) of given display mode (aSrcDisplayMode) as a bitmap of
another display mode (aDestDisplayMode) and logs the performance for it.
@param aBitmap a bitmap
@param aSrcDisplayMode The used display mode when bitmap was created. 
						It is passed in this function just for logging purpose.
@param aDestDisplayMode The display mode used for reading the bitmap aBitmap
@param aTestCaseName The name of the test case
*/
void CTOptimisedBmp::ScanlineTestL(const CFbsBitmap& aBitmap, TDisplayMode aSrcDisplayMode, TDisplayMode aDestDisplayMode, const TDesC& aTestCaseName)
	{
	const TSize bmpSize(aBitmap.SizeInPixels());
	TInt byteWidth = CFbsBitmap::ScanLineLength(bmpSize.iWidth,aDestDisplayMode);
	TUint8* buffer = new (ELeave) TUint8[byteWidth];
	CleanupStack::PushL(buffer);
	TPtr8 scanLine(buffer,byteWidth,byteWidth);

	iProfiler->InitResults();
	for(TInt count=0;count<KRepeatTestCount;count++)
		{
		// For performance testing on same scanline, read the few pixels from bitmap.
		// And for a comprehensive testing, vary the start pixel and the number of pixels to get.
		for(TInt xStart = 0; xStart <= 11; xStart++)
			{
			for (TInt xLength = 1; xLength <= 11; xLength++)
				{
				TInt xEnd = xStart + xLength;
				if (xEnd <= bmpSize.iWidth)
					{
					aBitmap.GetScanLine(scanLine,TPoint(xStart,0),xLength,aDestDisplayMode);
					}
				}
			}
		// Test the performance of reading different scanlines.
		for(TInt yy = 0; yy < bmpSize.iHeight; yy++)
			{
			aBitmap.GetScanLine(scanLine,TPoint(0,yy),bmpSize.iWidth,aDestDisplayMode);
			}

		iProfiler->MarkResultSetL();
		}

	iProfiler->ResultsAnalysis(aTestCaseName, 0, aSrcDisplayMode, aDestDisplayMode, KRepeatTestCount);

	CleanupStack::PopAndDestroy(buffer);
	}

/**
Logs the time taken to load the bitmap file identified by the filename and file id.
@param aFileName the mbm or rsc file that is to be loaded
@param aId The bitmap identifier
@param aShareIfLoaded Specifies whether or not the loaded bitmap will be made available for sharing between font and bitmap server clients.
@param aTestCaseName the name of the test case
*/
void CTOptimisedBmp::ResourceBitmapLoadL(const TDesC& aFileName, TInt32 aId, TBool aShareIfLoaded, const TDesC& aTestCaseName)
	{
	TInt err = KErrNone;
	iProfiler->InitResults(); //initializes the timer
	for (TInt rpt = 0; rpt < KRepeatTestCount; ++rpt)
		{
		CFbsBitmap* resourceBitmap = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(resourceBitmap);
		iProfiler->StartTimer();
		//Load mbm or rsc file identified by aFileName
		err = resourceBitmap->Load( aFileName, aId, aShareIfLoaded ); //when there is no memory constraints
		iProfiler->MarkResultSetL();
		TEST(err == KErrNone);
		resourceBitmap->Reset();
		CleanupStack::PopAndDestroy( resourceBitmap );
		}
	//Logs the time taken to load the bmp file
	iProfiler->ResultsAnalysis(aTestCaseName, 0, 0, 0, KRepeatTestCount);
	}
