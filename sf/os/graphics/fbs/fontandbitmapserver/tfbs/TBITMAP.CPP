// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <s32file.h>
#include <bautils.h>
#include <hal.h>
#include <test/tefunit.h>
#include <graphics/bitmapuid.h>
#include "../sfbs/UTILS.H"
#include "TBitmap.h"
#include "../sfbs/fbsbitmapasyncstreamer.h"
#include "fbsmessage.h"

// Symbian modification to help reduce unnecessary warnings 
#if (__ARMCC_VERSION >= 310000)
//turn off Warning:  #546-D: transfer of control bypasses initialization of variable, which is unnecessary.
#pragma diag_suppress 546
#endif
// End of Symbian modification



//
//
//Globals
//LOCAL_D RTest	TheTest(_L("Test Bitmap"));
LOCAL_D RFs		TheFs;
const TUint8	KCheckValue = 0x69;//pixel value used in the tests

_LIT(KBmpCompr12, "z:\\system\\data\\compr12.mbm");
_LIT(KBmpCompr16, "z:\\system\\data\\compr16.mbm");
_LIT(KBmpCompr24, "z:\\system\\data\\compr24.mbm");
_LIT(KBmpZCompr12, "z:\\system\\data\\racompr12.mbm");
_LIT(KBmpZCompr16, "z:\\system\\data\\racompr16.mbm");
_LIT(KBmpZCompr24, "z:\\system\\data\\racompr24.mbm");
_LIT(KBmpCCompr12, "c:\\racompr12.mbm");
_LIT(KBmpCCompr16, "c:\\racompr16.mbm");
_LIT(KBmpCCompr24, "c:\\racompr24.mbm");

_LIT(KRc_RamInRom8, "z:\\system\\data\\rc_ram8.mbm");
_LIT(KRc_RamInRom12, "z:\\system\\data\\rc_ram12.mbm");
_LIT(KRc_RamInRom16, "z:\\system\\data\\rc_ram16.mbm");
_LIT(KRc_RamInRom24, "z:\\system\\data\\rc_ram24.mbm");

_LIT(KRc_Ram8, "c:\\rc_ram8.mbm");
_LIT(KRc_Ram12, "c:\\rc_ram12.mbm");
_LIT(KRc_Ram16, "c:\\rc_ram16.mbm");
_LIT(KRc_Ram24, "c:\\rc_ram24.mbm");

_LIT(KRamBitmapOnZ, "z:\\system\\data\\16RAM2.mbm");
_LIT(KRamBitmap, "c:\\16RAM2.mbm");
_LIT(KRamBitmap2, "c:\\16RAM3.mbm");

_LIT(KMono_RamInRom_M1, "z:\\system\\data\\mono_m1.mbm");
_LIT(KMono_RamInRom_M2, "z:\\system\\data\\mono_m2.mbm");
_LIT(KMono_RamInRom_M4, "z:\\system\\data\\mono_m4.mbm");
_LIT(KMono_RamInRom_M8, "z:\\system\\data\\mono_m8.mbm");
_LIT(KMono_RamInRom_C4, "z:\\system\\data\\mono_c4.mbm");
_LIT(KMono_RamInRom_C8, "z:\\system\\data\\mono_c8.mbm");
_LIT(KMono_RamInRom_C12, "z:\\system\\data\\mono_c12.mbm");
_LIT(KMono_RamInRom_C16, "z:\\system\\data\\mono_c16.mbm");
_LIT(KMono_RamInRom_C24, "z:\\system\\data\\mono_c24.mbm");
_LIT(KMono_RamInRom_C32, "z:\\system\\data\\mono_c32.mbm");

_LIT(KMono_M1, "c:\\mono_m1.mbm");
_LIT(KMono_M2, "c:\\mono_m2.mbm");
_LIT(KMono_M4, "c:\\mono_m4.mbm");
_LIT(KMono_M8, "c:\\mono_m8.mbm");
_LIT(KMono_C4, "c:\\mono_c4.mbm");
_LIT(KMono_C8, "c:\\mono_c8.mbm");
_LIT(KMono_C12, "c:\\mono_c12.mbm");
_LIT(KMono_C16, "c:\\mono_c16.mbm");
_LIT(KMono_C24, "c:\\mono_c24.mbm");
_LIT(KMono_C32, "c:\\mono_c32.mbm");

_LIT(KRomBitmap,"z:\\system\\data\\tfbs.rbm");

//
//
//Create/Destroy test environment global functions

//Delete "aFullName" file.
LOCAL_C void DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone) 
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone) 
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}

//Copy test bitmaps from Z: to C: drive. RFs::Connect() had to be called already 
//on "TheFs" object.
LOCAL_C void CopyFilesOnC()
	{
	BaflUtils::CopyFile(TheFs, KTestBitmapOnZ, KTestBitmapOnC);
	BaflUtils::CopyFile(TheFs, KBmpZCompr12, KBmpCCompr12);
	BaflUtils::CopyFile(TheFs, KBmpZCompr16, KBmpCCompr16);
	BaflUtils::CopyFile(TheFs, KBmpZCompr24, KBmpCCompr24);
	BaflUtils::CopyFile(TheFs, KRc_RamInRom8, KRc_Ram8);
	BaflUtils::CopyFile(TheFs, KRc_RamInRom12, KRc_Ram12);
	BaflUtils::CopyFile(TheFs, KRc_RamInRom16, KRc_Ram16);
	BaflUtils::CopyFile(TheFs, KRc_RamInRom24, KRc_Ram24);
	BaflUtils::CopyFile(TheFs, KRamBitmapOnZ, KRamBitmap);
	BaflUtils::CopyFile(TheFs, KRamBitmapOnZ, KRamBitmap2);
	BaflUtils::CopyFile(TheFs, KMono_RamInRom_M1, KMono_M1);
	BaflUtils::CopyFile(TheFs, KMono_RamInRom_M2, KMono_M2);
	BaflUtils::CopyFile(TheFs, KMono_RamInRom_M4, KMono_M4);
	BaflUtils::CopyFile(TheFs, KMono_RamInRom_M8, KMono_M8);
	BaflUtils::CopyFile(TheFs, KMono_RamInRom_C4, KMono_C4);
	BaflUtils::CopyFile(TheFs, KMono_RamInRom_C8, KMono_C8);
	BaflUtils::CopyFile(TheFs, KMono_RamInRom_C12, KMono_C12);
	BaflUtils::CopyFile(TheFs, KMono_RamInRom_C16, KMono_C16);
	BaflUtils::CopyFile(TheFs, KMono_RamInRom_C24, KMono_C24);
	BaflUtils::CopyFile(TheFs, KMono_RamInRom_C32, KMono_C32);
	}

//Delete data files used by the test
LOCAL_C void DeleteFilesFromC()
	{
	DeleteDataFile(KTestBitmapOnC);
	DeleteDataFile(KBmpCCompr12);
	DeleteDataFile(KBmpCCompr16);
	DeleteDataFile(KBmpCCompr24);
	DeleteDataFile(KRc_Ram8);
	DeleteDataFile(KRc_Ram12);
	DeleteDataFile(KRc_Ram16);
	DeleteDataFile(KRc_Ram24);
	DeleteDataFile(KRamBitmap);
	DeleteDataFile(KRamBitmap2);
	DeleteDataFile(KMono_M1);
	DeleteDataFile(KMono_M2);
	DeleteDataFile(KMono_M4);
	DeleteDataFile(KMono_M8);
	DeleteDataFile(KMono_C4);
	DeleteDataFile(KMono_C8);
	DeleteDataFile(KMono_C12);
	DeleteDataFile(KMono_C16);
	DeleteDataFile(KMono_C24);
	DeleteDataFile(KMono_C32);
	}

//
//

void CTBitmap::DeleteScanLineBuffer()
	{
	RFbsSession* fbs = RFbsSession::GetSession();
	delete fbs->iScanLineBuffer;
	fbs->iScanLineBuffer = NULL;
	}

CTBitmap::CTBitmap(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iBmpHandler(NULL),
	iBitmap(NULL),
	iAs(NULL)
	{
	__ASSERT_DEBUG(CActiveScheduler::Current(), User::Invariant());
	CFbsBitmap bmp;
	iTestBitmapName = KTestBitmapOnZ;
	TInt ret=bmp.Load(iTestBitmapName,ETfbs,NULL);
	TEST2(ret, KErrNone);
	bmp.Reset();

	ret = iFs.Connect();
	TEST2(ret, KErrNone);

	// Expand session scanline buffer cache to avoid spurious alloc heavens later on
	const TInt bufferSize = 1024;

	ret = bmp.Create(TSize(bufferSize,1),EColor256);
	TEST2(ret, KErrNone);

	ret = bmp.Compress();
	TEST2(ret, KErrNone);

	TUint8* buffer = new TUint8[bufferSize];
	TEST(buffer != NULL);

	TPtr8 scanline(buffer,bufferSize,bufferSize);
	bmp.GetScanLine(scanline,TPoint(0,0),bufferSize,EColor256);
	delete [] buffer;
	bmp.Reset();
	
	TUint32* romAddress = NULL;
	if(!CFbsBitmap::IsFileInRom(KRomBitmap, romAddress)) //any ROM bitmap
		{
		INFO_PRINTF2(_L("Skipping ROM bitmap tests since file \"%S\" is reported to not be a ROM bitmap."),
				&KRomBitmap);
		INFO_PRINTF1(_L("This should only occur on non-XIP ROMs, e.g. NAND ROMs, where ROM bitmaps aren't supported."));
		iSkipRomBitmapTests = ETrue;
		}
	}

CTBitmap::~CTBitmap()
	{
	iFs.Close();
	}

void CTBitmap::RunTestCaseL(TInt aCurTestCase)
    {
    ((CTBitmapStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:		
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0550"));
		TwoComprLines();
		break;
	case 2:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0517"));
	// they are fast hence at the beginning
		CheckScanlineL();
		break;
	case 3:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0534"));
		Compress();
		break;
	case 4:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0518"));
		StreamBitmap();
		break;
	case 5:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0519"));
		StreamRectangle();
		break;
	case 6:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0523"));
		LoadInSegments();
		break;
	case 7:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0524"));
		SaveInSegments();
		break;
	case 8:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0525"));
		MonoDetect();
		break;
	case 9:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0526"));
		ChunkPileL();
		break;
	case 10:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0527"));
		LargeBitmaps();
		break;
	case 12:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0528"));
		HugeBitmaps();
		break;
	case 13:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0529"));
		Resize();
		break;
	case 14:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0520"));
		GetScanline();
		break;
	case 15:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0521"));
		SetScanline();
		break;
	case 16:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0522"));
		StretchScanline();
		break;
	case 17:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0530"));
		BitmapUtil();
		break;
	case 18:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0531"));
		InvalidBitmap();
		break;
	case 19:
		((CTBitmapStep*)iStep)->SetTestStepID(_L(" GRAPHICS-FBSERV-0533"));
		SaveAndLoadFast();
		break;
	case 20:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0532"));
		SaveAndLoad();
		break;
	case 21:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0535"));
		Color256BitmapUtil();
		break;
	case 22:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0536"));
		HardwareBitmaps();
		break;
	case 23:
		((CTBitmapStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
	//	BitmapTiming(); 
		break;
	case 24:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0537"));
		INC036062L();
		break;
	case 25:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0538"));
		PerformanceTest_INC036062L();
		break;
	case 26:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0539"));
		INC037474L();
		break;
	case 27:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0540"));
		DEF038286L();
		break;
	case 28:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0541"));
		DEF038286_2L();
		break;
	case 29:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0542"));
		MonohromeL();
		break;
	case 30:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0543"));
		GetVerticalScanLineL();
		break;
	case 31:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0544"));
		GetVerticalScanLinePerformanceL();
		break;
	case 32:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0545"));
		DEF071843_16MAL();
		((CTBitmapStep*)iStep)->RecordTestResultL();
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0546"));	
		DEF071843_16MUL();
		((CTBitmapStep*)iStep)->RecordTestResultL();
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0547"));
		DEF071843_16ML();
#ifdef _DEBUG
		((CTBitmapStep*)iStep)->RecordTestResultL();
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0549"));
		//It uses Heap Allocation failure macro 
		//which is supported only for debug mode	
		DEF095361_16MAL();
#endif		
		break;
	case 33:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0548"));
		DEF074623_16MAL();
		break;
	case 34:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0551"));
		TestPaletteCompressionL();
		break;
	case 35:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0552"));
		TestSynchronousBackgroundPaletteCompressionL();
		break;
	case 36:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0553"));
		TestAsynchronousBackgroundPaletteCompressionL();
		break;
	case 37:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0501"));
		GetAllBitmapsIdsL();
		break;
	case 38:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0554"));
		CheckBadBmp();
		break; 
	case 39:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0516"));
		CheckSetDisplayModeL();
		break; 	
	case 40:
/**
	@SYMTestCaseID GRAPHICS-FBSERV-0619
*/
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0619"));
		MultiThreadedTestsL();
		break;
	case 41:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0612"));
		TestBgCompressL();
		break;
	case 42:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0613"));
		TestBgCompressInUseL();
		break;
	case 43:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0614"));
		TestDisconnectWithBitmapL();
		break;
	case 44:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0617"));
		TestTouchedAndVolatileL();
		break;
	case 45:
		((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0618"));
		TestBitmapWhiteFillL();
		break;
	case 46:
#ifdef _DEBUG
	    ((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0661"));
	    TestBitmapUtilWithUnsupportedBitmaps();
#endif
        break;
	case 47:
		((CTBitmapStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTBitmapStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
	default:
		((CTBitmapStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		break;
		}
		
	((CTBitmapStep*)iStep)->RecordTestResultL();
    }
   
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0516

	@SYMTestCaseDesc
	Checks that the display mode can be set 
	to various modes, including EColor16MAP
	
	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/  
void CTBitmap::CheckSetDisplayModeL()
	{
	INFO_PRINTF1(_L("Check SetDisplayMode EColor16MAP"));

	CFbsBitmap bmp;
	User::LeaveIfError(bmp.Create(TSize(4,4),EColor16MA));
	TInt ret=bmp.SetDisplayMode(EColor16MAP);
	TEST(ret==KErrNone);
	bmp.Reset();

	User::LeaveIfError(bmp.Create(TSize(4,4),EColor16MU));
	ret=bmp.SetDisplayMode(EColor16MAP);
	TEST(ret==KErrNone);
	bmp.Reset();

	User::LeaveIfError(bmp.Create(TSize(4,4),EColor64K));
	ret=bmp.SetDisplayMode(EColor16MAP);
	TEST(ret==KErrArgument);
	bmp.Reset();

	User::LeaveIfError(bmp.Create(TSize(4,4),EColor16MAP));
	ret=bmp.SetDisplayMode(EColor16MAP);
	TEST(ret==KErrNone);

	ret=bmp.SetDisplayMode(EColor16MU);
	TEST(ret==KErrNone);
	bmp.Reset();
	
	}    

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0517

	@SYMTestCaseDesc
	Compares a pixel retrieved with GetPixel and GetScanLine

	@SYMTestActions
	Zero fills a buffer which has an uneven length of bytes and
	gets a pixel from a point which should coincide with
	that of a point retrieved with scan line
		
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::CheckScanlineL()
	{
	INFO_PRINTF1(_L("Scanline sanity check"));

	CFbsBitmap bmp;
	User::LeaveIfError(bmp.Create(TSize(32,1),EGray2));

	// try using a buffer that isn't a whole number of words long. 
	TUint8 buffer[3];
	Mem::FillZ(buffer,3);
	TPtr8 scanline(buffer,3,3);
	bmp.GetScanLine(scanline,TPoint(0,0),24,EGray2);

	TRgb pixel(KRgbRed);
	bmp.GetPixel(pixel,TPoint(0,0));

	// getscanline and getpixel should agree
	TEST(TRgb::Gray2(buffer[0] & 0x01) == pixel);

	bmp.Reset();

	
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0518

	@SYMTestCaseDesc
	Streams different bitmap size of different colour depth.

	@SYMTestActions

	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::StreamBitmap()
	{
	INFO_PRINTF1(_L("Test Bitmap Streaming"));

	DoStreamBitmapSizes(EGray2);
	DoStreamBitmapSizes(EGray4);
	DoStreamBitmapSizes(EGray16);
	DoStreamBitmapSizes(EGray256);
	DoStreamBitmapSizes(EColor16);
	DoStreamBitmapSizes(EColor256);
	DoStreamBitmapSizes(EColor4K);
	DoStreamBitmapSizes(EColor64K);
	DoStreamBitmapSizes(EColor16M);
	DoStreamBitmapSizes(EColor16MU);
	DoStreamBitmapSizes(EColor16MAP);
	}

void CTBitmap::DoStreamBitmapSizes(TDisplayMode aDispMode)
	{
	const TSize smallStreamBitmapSize(32,10);
	const TSize largeStreamBitmapSize(256,256);
	const TSize oddStreamBitmapSize(31,2);

	DoStreamBitmap(smallStreamBitmapSize,aDispMode,ETrue);
	DoStreamBitmap(smallStreamBitmapSize,aDispMode,EFalse);
	DoStreamBitmap(largeStreamBitmapSize,aDispMode,ETrue);
	DoStreamBitmap(largeStreamBitmapSize,aDispMode,EFalse);
	DoStreamBitmap(oddStreamBitmapSize,aDispMode,ETrue);
	DoStreamBitmap(oddStreamBitmapSize,aDispMode,EFalse);
	}

void CTBitmap::DoStreamBitmap(const TSize& aSize,TDisplayMode aDispMode,TBool aBlank)
	{
	__UHEAP_MARK;

	CFbsBitmapEx bmp;
	TInt ret = bmp.Create(aSize,aDispMode);
	TEST2(ret, KErrNone);

	if (aBlank)
		BlankBitmap(bmp);
	else
		FillBitmap(bmp);

	CDirectFileStore* writestore=NULL;

	TRAP(ret,writestore=CDirectFileStore::ReplaceL(iFs,KTempFilename,EFileStream|EFileWrite));
	TEST2(ret, KErrNone);

	TUidType uidtype(KDirectFileStoreLayoutUid,KMultiBitmapFileImageUid);

	TRAP(ret,writestore->SetTypeL(uidtype));
	TEST2(ret, KErrNone);

	RStoreWriteStream writestrm;
	TStreamId headerid(0);

	TRAP(ret,headerid=writestrm.CreateL(*writestore));
	TEST2(ret, KErrNone);

	TRAP(ret,writestore->SetRootL(headerid));
	TEST2(ret, KErrNone);

	TRAP(ret,bmp.ExternalizeL(writestrm));
	TEST2(ret, KErrNone);

	writestrm.Close();
	delete writestore;

	CFbsBitmapEx bmpst;
	CDirectFileStore* readstore=NULL;

	TRAP(ret,readstore=CDirectFileStore::OpenL(iFs,KTempFilename,EFileStream|EFileRead));
	TEST2(ret, KErrNone);

	RStoreReadStream readstrm;
	headerid=readstore->Root();

	TRAP(ret,readstrm.OpenL(*readstore,headerid));
	TEST2(ret, KErrNone);

	TRAP(ret,bmpst.InternalizeL(readstrm));
	TEST2(ret, KErrNone);

	readstrm.Close();
	delete readstore;

	CompareBitmaps(bmp,bmpst,EColor16M);

	iFs.Delete(KTempFilename);
	bmp.Reset();
	bmpst.Reset();
	User::Heap().Check();
	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0519

	@SYMTestCaseDesc
	Streams bitmap rectangles of different colour depth
	to file store and reads them back again.

	@SYMTestActions
	Bitmap rectangles are externalized to file store and
	read back again. ScanLine is used and compared to GetPixel
	from the bitmap read back from file store.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::StreamRectangle()
	{
	INFO_PRINTF1(_L("Test Rectangle Streaming"));

	DoStreamRectangle(EGray2,ETrue);
	DoStreamRectangle(EGray2,EFalse);
	DoStreamRectangle(EGray4,ETrue);
	DoStreamRectangle(EGray4,EFalse);
	DoStreamRectangle(EGray16,ETrue);
	DoStreamRectangle(EGray16,EFalse);
	DoStreamRectangle(EGray256,ETrue);
	DoStreamRectangle(EGray256,EFalse);
	DoStreamRectangle(EColor16,ETrue);
	DoStreamRectangle(EColor16,EFalse);
	DoStreamRectangle(EColor256,ETrue);
	DoStreamRectangle(EColor256,EFalse);
	DoStreamRectangle(EColor4K,ETrue);
	DoStreamRectangle(EColor4K,EFalse);
	DoStreamRectangle(EColor64K,ETrue);
	DoStreamRectangle(EColor64K,EFalse);
	DoStreamRectangle(EColor16M,ETrue);
	DoStreamRectangle(EColor16M,EFalse);
	DoStreamRectangle(EColor16MU,ETrue);
	DoStreamRectangle(EColor16MU,EFalse);
	DoStreamRectangle(EColor16MAP,ETrue);
	DoStreamRectangle(EColor16MAP,EFalse);
	}

void CTBitmap::DoStreamRectangle(TDisplayMode aDispMode,TBool aBlank)
	{
	__UHEAP_MARK;
	CFbsBitmapEx bmp;
	TInt ret = bmp.Create(TSize(160,80),aDispMode);
	TEST2(ret, KErrNone);

	if (aBlank)
		BlankBitmap(bmp);
	else
		FillBitmap(bmp);

	CDirectFileStore* writestore=NULL;
	TRAP(ret,writestore=CDirectFileStore::ReplaceL(iFs,KTempFilename,EFileStream|EFileWrite));
	TEST2(ret, KErrNone);

	TUidType uidtype(KDirectFileStoreLayoutUid,KMultiBitmapFileImageUid);
	TRAP(ret,writestore->SetTypeL(uidtype));
	TEST2(ret, KErrNone);

	RStoreWriteStream writestrm;
	TStreamId headerid(0);
	TRAP(ret,headerid=writestrm.CreateL(*writestore));
	TEST2(ret, KErrNone);

	TRAP(ret,writestore->SetRootL(headerid));
	TEST2(ret, KErrNone);

	TRAP(ret,bmp.ExternalizeRectangleL(writestrm,TRect(40,20,120,60)));
	TEST2(ret, KErrNone);

	writestrm.Close();
	delete writestore;

	CFbsBitmapEx bmpst;
	CDirectFileStore* readstore=NULL;
	TRAP(ret,readstore=CDirectFileStore::OpenL(iFs,KTempFilename,EFileStream|EFileRead));
	TEST2(ret, KErrNone);

	RStoreReadStream readstrm;
	headerid=readstore->Root();
	TRAP(ret,readstrm.OpenL(*readstore,headerid));
	TEST2(ret, KErrNone);

	TRAP(ret,bmpst.InternalizeL(readstrm));
	TEST2(ret, KErrNone);

	readstrm.Close();
	delete readstore;

	TInt scanLineLength = CFbsBitmap::ScanLineLength(80,aDispMode);
	TUint8* buffer = new TUint8[scanLineLength];
	TUint8* bufferst = new TUint8[scanLineLength];
	Mem::FillZ(buffer,scanLineLength);
	Mem::FillZ(bufferst,scanLineLength);
	TPtr8 scanline(buffer,scanLineLength,scanLineLength);
	TPtr8 scanlinest(bufferst,scanLineLength,scanLineLength);

	for(TInt yy=0;yy<40;yy++)
		{
		bmp.GetScanLine(scanline,TPoint(40,yy+20),80,aDispMode);
		bmpst.GetScanLine(scanlinest,TPoint(0,yy),80,aDispMode);
		
		if(aDispMode != EColor16MU)
			{
			TInt memRet = Mem::Compare(buffer,scanLineLength,bufferst,scanLineLength);
			TEST(memRet == 0);
			}
		else
			{
			TInt pixelsToCompare = scanLineLength / 4;
			TUint8* bufferCur = buffer;
			TUint8* bufferstCur = bufferst;
			
			//top byte is unused, and would be more accurate to ignore it
			for(TInt ii = 0; ii < pixelsToCompare; ii++)
				{
				TInt memRet = Mem::Compare(bufferCur,3,bufferstCur,3);
				TEST(memRet == 0);
				bufferCur += 4;
				bufferstCur += 4;
				}
			}	
		}
	delete [] bufferst;
	delete [] buffer;

	iFs.Delete(KTempFilename);
	bmp.Reset();
	bmpst.Reset();
	User::Heap().Check();
	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0520

	@SYMTestCaseDesc
	Calls GetScanLine for different screen modes with
	the bitmap compressed and uncompressed and checks
	if the pixels are correct.

	@SYMTestActions

	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::GetScanline()
	{
	INFO_PRINTF1(_L("Test GetScanline"));

	const TSize size1(257,5);
	const TSize size2(33,17);

	INFO_PRINTF1(_L("EGray2"));
	DoGetScanline(size1,EGray2);
	DoGetScanline(size2,EGray2);
	DoGetScanlineAlt(EGray2);

	INFO_PRINTF1(_L(" EGray4"));
	DoGetScanline(size1,EGray4);
	DoGetScanline(size2,EGray4);
	DoGetScanlineAlt(EGray4);

	INFO_PRINTF1(_L(" EGray16"));
	DoGetScanline(size1,EGray16);
	DoGetScanline(size2,EGray16);
	DoGetScanlineAlt(EGray16);

	INFO_PRINTF1(_L(" EGray256"));
	DoGetScanline(size1,EGray256);
	DoGetScanline(size2,EGray256);
	DoGetScanlineAlt(EGray256);

	INFO_PRINTF1(_L(" EColor16"));
	DoGetScanline(size1,EColor16);
	DoGetScanline(size2,EColor16);
	DoGetScanlineAlt(EColor16);

	INFO_PRINTF1(_L(" EColor256"));
	DoGetScanline(size1,EColor256);
	DoGetScanline(size2,EColor256);
	DoGetScanlineAlt(EColor256);

	INFO_PRINTF1(_L(" EColor4K"));
	DoGetScanline(size1,EColor4K);
	DoGetScanline(size2,EColor4K);
	DoGetScanlineAlt(EColor4K);

	INFO_PRINTF1(_L(" EColor64K"));
	DoGetScanline(size1,EColor64K);
	DoGetScanline(size2,EColor64K);
	DoGetScanlineAlt(EColor64K);

	INFO_PRINTF1(_L(" EColor16M"));
	DoGetScanline(size1,EColor16M);
	DoGetScanline(size2,EColor16M);
	DoGetScanlineAlt(EColor16M);

	INFO_PRINTF1(_L(" EColor16MA"));
	DoGetScanline(size1,EColor16MA);
	DoGetScanline(size2,EColor16MA);
	DoGetScanlineAlt(EColor16MA);

	INFO_PRINTF1(_L(" EColor16MU"));
	DoGetScanline(size1,EColor16MU);
	DoGetScanline(size2,EColor16MU);
	DoGetScanlineAlt(EColor16MU);

	INFO_PRINTF1(_L(" EColor16MAP"));
	DoGetScanline(size1,EColor16MAP);
	DoGetScanline(size2,EColor16MAP);
	DoGetScanlineAlt(EColor16MAP);

	INFO_PRINTF1(_L("\n"));
	}

void CTBitmap::DoGetScanline(const TSize& aSize,TDisplayMode aDispMode)
	{
	__UHEAP_MARK;
	CFbsBitmapEx bmp;
	TInt ret = bmp.Create(aSize,aDispMode);
	TEST2(ret, KErrNone);
	FillBitmap(bmp);

	DoGetScanlineCheck1(bmp, bmp, aSize, aDispMode);
	DoGetScanlineCheck2(bmp, bmp, aSize, aDispMode);
	DoGetScanlineCheck3(bmp, aDispMode);

	ret = bmp.Create(aSize,aDispMode);
	TEST2(ret, KErrNone);

	StripeBitmap(bmp);
	CFbsBitmapEx ref;
	ret = ref.Create(aSize,aDispMode);
	TEST2(ret, KErrNone);

	CopyBitmap(ref, bmp);
	ret = bmp.Compress();
	TEST2(ret, KErrNone);

	DoGetScanlineCheck1(bmp, ref, aSize, aDispMode);

	ref.Reset();
	bmp.Reset();
	User::Heap().Check();
	__UHEAP_MARKEND;
	}

void CTBitmap::DoGetScanlineCheck1(CFbsBitmap& aBitmap,CFbsBitmap& aReference,const TSize& aSize,TDisplayMode aDispMode)
	{
	TInt byteWidth = CFbsBitmap::ScanLineLength(aSize.iWidth,aDispMode);
	TUint8* buffer = new TUint8[byteWidth + 1];
	buffer[byteWidth] = KCheckValue;
	TPtr8 scanline(buffer,byteWidth,byteWidth);

	for(TInt xStart = 0; xStart <= 33; xStart++)
		{
		for (TInt xLength = 1; xLength <= 33; xLength++)
			{
			TInt xEnd = xStart + xLength;
			if (xEnd <= aSize.iWidth)
				{
				aBitmap.GetScanLine(scanline,TPoint(xStart,0),xLength,aDispMode);
				for(TInt xx = xStart; xx < xEnd; xx++)
					{
					TRgb color;
					aReference.GetPixel(color,TPoint(xx,0));
					TRgb bufferColor = ExtractRgb(buffer,xx - xStart,aDispMode);
					TEST(bufferColor == color);
					}
				}
			}
		}

	for(TInt yy = 0; yy < aSize.iHeight; yy++)
		{
		aBitmap.GetScanLine(scanline,TPoint(0,yy),aSize.iWidth,aDispMode);
		for(TInt xx = 0; xx < aSize.iWidth; xx++)
			{
			TRgb color;
			aReference.GetPixel(color,TPoint(xx,yy));
			TRgb bufferColor = ExtractRgb(buffer,xx,aDispMode);
			TEST(bufferColor == color);
			}
		}

	TEST(buffer[byteWidth] == KCheckValue);
	delete [] buffer;
	}

void CTBitmap::DoGetScanlineCheck2(CFbsBitmap& aBitmap,CFbsBitmap& aReference,const TSize& aSize,TDisplayMode aDispMode)
	{
	TInt byteHeight = CFbsBitmap::ScanLineLength(aSize.iHeight,ERgb);
	TUint8* buffer = new TUint8[byteHeight];
	TPtr8 scanline(buffer,byteHeight,byteHeight);

	TInt xx;
	for(xx = 0; xx < aSize.iWidth; xx++)
		{
		aBitmap.GetVerticalScanLine(scanline,xx,aDispMode);
		for(TInt yy = 0; yy < aSize.iHeight; yy++)
			{
			TRgb color;
			aReference.GetPixel(color,TPoint(xx,yy));
			TRgb bufferColor = ExtractRgb(buffer,yy,aDispMode);
			TEST(bufferColor == color);
			}
		}

	for(xx = 0; xx < aSize.iWidth; xx++)
		{
		aBitmap.GetVerticalScanLine(scanline,xx,ERgb);
		for(TInt yy = 0; yy < aSize.iHeight; yy++)
			{
			TRgb color;
			aReference.GetPixel(color,TPoint(xx,yy));
			TRgb bufferColor = ExtractRgb(buffer,yy,ERgb);
			TEST(bufferColor == color);
			}
		}

	delete [] buffer;
	}

void CTBitmap::DoGetScanlineCheck3(CFbsBitmap& aBitmap,TDisplayMode aDispMode)
	{
	TUint32 wordline=0xffffffff;
	TPtr8 scanline((TUint8*)&wordline,4,4);
	aBitmap.Create(TSize(31,1),aDispMode);
	aBitmap.LockHeap();
	Mem::Fill(aBitmap.DataAddress(),CFbsBitmap::ScanLineLength(31,aDispMode),0xff);
	aBitmap.UnlockHeap();
	aBitmap.SetScanLine(scanline,0);
	TUint32 expected=0;

	for(TInt zz=1;zz<31;zz++)
		{
		expected|=1<<(zz-1);
		wordline=0;
		aBitmap.GetScanLine(scanline,TPoint(0,0),zz,EGray2);
		TUint32 actual=wordline&expected;
		TEST(actual==expected);
		}
	}

/* this function calls CheckScanLine with various screen modes, and with the bitmap
   compressed, and not compressed
 */
void CTBitmap::DoGetScanlineAlt(TDisplayMode aDispMode)
	{
	__UHEAP_MARK;

	CFbsBitmapEx bmp;
	TInt ret = bmp.Create(TSize(256,3),aDispMode);
	TEST2(ret, KErrNone);

	FillBitmap(bmp);

	CheckScanLine(bmp,bmp,EGray2);
	CheckScanLine(bmp,bmp,EGray4);
	CheckScanLine(bmp,bmp,EGray16);
	CheckScanLine(bmp,bmp,EGray256);
	CheckScanLine(bmp,bmp,EColor16);
	CheckScanLine(bmp,bmp,EColor256);
	CheckScanLine(bmp,bmp,EColor4K);
	CheckScanLine(bmp,bmp,EColor64K);
	CheckScanLine(bmp,bmp,EColor16M);
	CheckScanLine(bmp,bmp,ERgb);
	CheckScanLine(bmp,bmp,EColor16MU);
	CheckScanLine(bmp,bmp,EColor16MAP);

	CFbsBitmapEx ref;
	ret = ref.Create(bmp.SizeInPixels(),aDispMode);
	TEST2(ret, KErrNone);
	CopyBitmap(ref,bmp);
	bmp.Compress();

	CheckScanLine(bmp,ref,EGray2);
	CheckScanLine(bmp,ref,EGray4);
	CheckScanLine(bmp,ref,EGray16);
	CheckScanLine(bmp,ref,EGray256);
	CheckScanLine(bmp,ref,EColor16);
	CheckScanLine(bmp,ref,EColor256);
	CheckScanLine(bmp,ref,EColor4K);
	CheckScanLine(bmp,ref,EColor64K);
	CheckScanLine(bmp,ref,EColor16M);
	CheckScanLine(bmp,ref,ERgb);
	CheckScanLine(bmp,ref,EColor16MU);
	CheckScanLine(bmp,ref,EColor16MAP);

	User::Heap().Check();
	__UHEAP_MARKEND;
	}

void CTBitmap::CheckScanLine(CFbsBitmapEx& aBitmap,CFbsBitmapEx& aReference,TDisplayMode aDispMode)
	{
	const TSize bmpSize(aBitmap.SizeInPixels());
	TInt byteWidth = CFbsBitmap::ScanLineLength(bmpSize.iWidth,aDispMode);
	TUint8* buffer = new TUint8[byteWidth + 1];
	buffer[byteWidth] = KCheckValue;
	TPtr8 scanLine(buffer,byteWidth,byteWidth);

	aBitmap.GetScanLine(scanLine,TPoint(0,1),bmpSize.iWidth,aDispMode);

	for(TInt xx = 0; xx < bmpSize.iWidth; xx++)
		{
		TRgb color;
		aReference.GetPixel(color,TPoint(xx,1));
		TRgb bufferColor = ExtractRgb(buffer,xx,aDispMode);
		switch (aDispMode)
			{
		case EGray2:
			if (color.Gray4() == 0 || color.Gray4() == 3) // doesn't check in great detail because dithering occurs
				TEST(bufferColor.Gray2() == color.Gray2());
			break;
		case EGray4:
			if (aBitmap.DisplayMode() != EGray16) // doesn't check EGray16 because dithering occurs
				TEST(bufferColor.Gray4() == color.Gray4());
			break;
		case EGray16:
			TEST(bufferColor.Gray16() == color.Gray16());
			break;
		case EGray256:
			TEST(bufferColor.Gray256() == color.Gray256());
			break;
		case EColor16:
			TEST(bufferColor.Color16() == color.Color16());
			break;
		case EColor256:
			TEST(bufferColor.Color256() == color.Color256());
			break;
		case EColor4K:
			TEST(bufferColor.Color4K() == color.Color4K());
			break;
		case EColor64K:
			TEST(bufferColor.Color64K() == color.Color64K());
			break;
		case EColor16M:
			TEST(bufferColor.Color16M() == color.Color16M());
			break;
		case ERgb:
			TEST(bufferColor == color);
			break;
		case EColor16MU:
			if (aReference.DisplayMode()==EColor16MAP || aReference.DisplayMode()==EColor16MA)
				color.SetAlpha(0xff);
			CompareColourLikeness(bufferColor, color);			
			break;
		case EColor16MAP:
			//Compare the TRgb values.  For an exact match to work the comparison would
			//need to be done in PMA space.
			color.SetInternal(color.Color16MAP());
			bufferColor.SetInternal(bufferColor.Color16MAP());
			CompareColourLikeness(bufferColor, color);			
			break;	
		default:
			break;
			};
		}

	TEST(buffer[byteWidth] == KCheckValue);
	delete [] buffer;
	}

/**
Compares two TRgbs and tests if they are similar colour.
This is required because there may be some rounding errors when converting bitmaps from one display mode to another
or due to alpha blending calculations.

@param aSrc is the source colour
@param aDest is the destination colour
@param aDifference use this value for colour difference.  Value of 0 use KColourDifference;
*/
void CTBitmap::CompareColourLikeness(const TRgb& aSrc, const TRgb& aDest, TInt aDifference)
	{
	const TInt KColourDifference = 2;	// Acceptable difference to check for
	TInt diff;
	
	if (aDifference==0)
		diff=KColourDifference;
	else
		diff=aDifference;
	
	TInt redDiff = Abs(aSrc.Red() - aDest.Red());
	TInt greenDiff = Abs(aSrc.Green() - aDest.Green());	
	TInt blueDiff = Abs(aSrc.Blue() - aDest.Blue());	
	TInt alphaDiff = Abs(aSrc.Alpha() - aDest.Alpha());	
	
	// Test each channel
	TEST(redDiff <= diff);
	TEST(greenDiff <= diff);
	TEST(blueDiff <= diff);
	TEST(alphaDiff <= diff);
	
	// the code below is useful for a breakpoint, to inspect the differences, in case of failure
/**	
	if ((redDiff > diff)||(greenDiff > diff)||(blueDiff > diff)||(alphaDiff > diff))
		TInt temp=1;
*/
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0521

	@SYMTestCaseDesc
	Sets a scanline of random values into
	bitmaps of different color depths

	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::SetScanline()
	{
	INFO_PRINTF1(_L("Test SetScanline"));

	DoSetScanline(EGray2);
	DoSetScanline(EGray4);
	DoSetScanline(EGray16);
	DoSetScanline(EGray256);
	DoSetScanline(EColor16);
	DoSetScanline(EColor256);
	DoSetScanline(EColor4K);
	DoSetScanline(EColor64K);
	DoSetScanline(EColor16M);
	DoSetScanline(EColor16MU);
	DoSetScanline(EColor16MAP);
	}

void CTBitmap::DoSetScanline(TDisplayMode aDispMode)
	{
	__UHEAP_MARK;
	CFbsBitmap bmp;

	TInt ret = bmp.Create(TSize(256,2),aDispMode);
	TEST2(ret, KErrNone);

	TInt scanLineLength = CFbsBitmap::ScanLineLength(256,aDispMode);
	TUint8* buffer = new TUint8[scanLineLength];
	TEST(buffer != NULL);
	TPtr8 scanline(buffer,scanLineLength,scanLineLength);

	TUint8* bufptr = buffer;
	TInt64 seed = bmp.Handle();
	for(TInt count = 0; count < scanLineLength; count++)
		*bufptr++ = (TUint8)Math::Rand(seed);

	bmp.SetScanLine(scanline,0);
	bmp.SetScanLine(scanline,1);

	bmp.LockHeap();
	TUint8* data = (TUint8*)bmp.DataAddress();
	bmp.UnlockHeap();
	ret = Mem::Compare(buffer,scanLineLength,data,scanLineLength);
	TEST(ret == 0);

	data += scanLineLength;
	ret = Mem::Compare(buffer,scanLineLength,data,scanLineLength);
	TEST(ret == 0);

	delete [] buffer;
	bmp.Reset();
	User::Heap().Check();
	__UHEAP_MARKEND;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0522

	@SYMTestCaseDesc
	Stretches a scan line to make it
	double as long for different colour depths
	tested on both small and large bitmaps.

	@SYMTestActions

	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::StretchScanline()
	{
	INFO_PRINTF1(_L("Test StretchScanline"));

	const TSize smallBitmapSize(226,2);
	const TSize largeBitmapSize(1026,40);

	INFO_PRINTF1(_L("EGray2"));
	DoStretchScanline(smallBitmapSize,EGray2);
	DoStretchScanline(largeBitmapSize,EGray2);

	INFO_PRINTF1(_L(" EGray4"));
	DoStretchScanline(smallBitmapSize,EGray4);
	DoStretchScanline(largeBitmapSize,EGray4);

	INFO_PRINTF1(_L(" EGray16"));
	DoStretchScanline(smallBitmapSize,EGray16);
	DoStretchScanline(largeBitmapSize,EGray16);

	INFO_PRINTF1(_L(" EGray256"));
	DoStretchScanline(smallBitmapSize,EGray256);
	DoStretchScanline(largeBitmapSize,EGray256);

	INFO_PRINTF1(_L(" EColor16"));
	DoStretchScanline(smallBitmapSize,EColor16);
	DoStretchScanline(largeBitmapSize,EColor16);

	INFO_PRINTF1(_L(" EColor256"));
	DoStretchScanline(smallBitmapSize,EColor256);
	DoStretchScanline(largeBitmapSize,EColor256);

	INFO_PRINTF1(_L(" EColor4K"));
	DoStretchScanline(smallBitmapSize,EColor4K);
	DoStretchScanline(largeBitmapSize,EColor4K);

	INFO_PRINTF1(_L(" EColor64K"));
	DoStretchScanline(smallBitmapSize,EColor64K);
	DoStretchScanline(largeBitmapSize,EColor64K);

	INFO_PRINTF1(_L(" EColor16M"));
	DoStretchScanline(smallBitmapSize,EColor16M);
	DoStretchScanline(largeBitmapSize,EColor16M);

	INFO_PRINTF1(_L(" EColor16MU"));
	DoStretchScanline(smallBitmapSize,EColor16MU);
	DoStretchScanline(largeBitmapSize,EColor16MU);
	
	INFO_PRINTF1(_L(" EColor16MA"));
	DoStretchScanline(smallBitmapSize,EColor16MA);
	DoStretchScanline(largeBitmapSize,EColor16MA);

	INFO_PRINTF1(_L(" EColor16MAP"));
	DoStretchScanline(smallBitmapSize,EColor16MAP);
	DoStretchScanline(largeBitmapSize,EColor16MAP);
	
	DoStretchScanline(smallBitmapSize,EColor16MAP,EColor16M, ETrue);
	DoStretchScanline(largeBitmapSize,EColor16MAP,EColor16M, ETrue);
	DoStretchScanline(smallBitmapSize,EColor16MAP,EColor16MU, ETrue);
	DoStretchScanline(largeBitmapSize,EColor16MAP,EColor16MU, ETrue);
	DoStretchScanline(smallBitmapSize,EColor16MAP,EColor16MA, ETrue);
	DoStretchScanline(largeBitmapSize,EColor16MAP,EColor16MA, ETrue);
	DoStretchScanline(smallBitmapSize,EColor16MAP,EColor64K, ETrue);
	DoStretchScanline(largeBitmapSize,EColor16MAP,EColor64K, ETrue);
	DoStretchScanline(smallBitmapSize,EColor16MAP,EColor4K, ETrue);
	DoStretchScanline(largeBitmapSize,EColor16MAP,EColor4K, ETrue);
	DoStretchScanline(smallBitmapSize,EColor16MAP,EColor256, ETrue);
	DoStretchScanline(largeBitmapSize,EColor16MAP,EColor256, ETrue);
	
	DoStretchScanline(smallBitmapSize,EColor16M,EColor16MAP, ETrue);
	DoStretchScanline(largeBitmapSize,EColor16M,EColor16MAP, ETrue);
	DoStretchScanline(smallBitmapSize,EColor16MU,EColor16MAP, ETrue);
	DoStretchScanline(largeBitmapSize,EColor16MU,EColor16MAP, ETrue);
	DoStretchScanline(smallBitmapSize,EColor16MA,EColor16MAP, ETrue);
	DoStretchScanline(largeBitmapSize,EColor16MA,EColor16MAP, ETrue);
	DoStretchScanline(smallBitmapSize,EColor64K,EColor16MAP, ETrue);
	DoStretchScanline(largeBitmapSize,EColor64K,EColor16MAP, ETrue);
	DoStretchScanline(smallBitmapSize,EColor4K,EColor16MAP, ETrue);
	DoStretchScanline(largeBitmapSize,EColor4K,EColor16MAP, ETrue);
	DoStretchScanline(smallBitmapSize,EColor256,EColor16MAP, ETrue);
	DoStretchScanline(largeBitmapSize,EColor256,EColor16MAP, ETrue);
	
	DoStretchScanline(smallBitmapSize,EColor16MU,EColor16M, ETrue);
	DoStretchScanline(largeBitmapSize,EColor16MU,EColor16M, ETrue);
	
	INFO_PRINTF1(_L("\n"));
	}

void CTBitmap::DoStretchScanline(const TSize& aSize,TDisplayMode aDispMode)
	{
	DoStretchScanline(aSize,aDispMode,aDispMode, EFalse);
	}

/* this function is used to determine an acceptable colour difference based on the screen mode 
   and the alpha value.  This is because when converting from PMA to non pre-multiplied, there
   can be large errors, especially if alpha is small.
 */	
TInt CTBitmap::ColorDifference(TDisplayMode aMode, TInt aAlpha)
	{
	switch (aMode)
		{
	case EGray2:// Monochrome display mode (1 bpp)
		return 0xff>>1;
	case EGray4:// Four grayscales display mode (2 bpp)
		return 0xff>>2;
	case EGray16:// 16 grayscales display mode (4 bpp)
		return 0xff>>2;
	case EGray256:// 256 grayscales display mode (8 bpp)
		return 0xff>>2;
	case EColor16:// Low colour EGA 16 colour display mode (4 bpp)
		return 0xff>>2;
	case EColor256: // 256 colour display mode (8 bpp)
		return 0xff>>2;
	case EColor64K: // 64,000 colour display mode (16 bpp) 
		return 0xff>>5;
	case EColor16M: // True colour display mode (24 bpp)
		return 0xff>>5;
	case EColor4K: // 4096 colour display (12 bpp).
		return 0xff>>4;
	case EColor16MU:// True colour display mode (32 bpp, but top byte is unused and unspecified)
		return 0xff>>5;
	case EColor16MA:// Display mode with alpha (24bpp colour plus 8bpp alpha)
		return 0xff>>6;
	case EColor16MAP:// Pre-multiplied Alpha display mode (24bpp color multiplied with the alpha channel value, plus 8bpp alpha)
		if (aAlpha==0xff)
			return 0xff>>4;
		else
			//make inaccuracy a function of alpha since the colour will the premultiplied, and then
			//unpremultiplied.  For small alpha this can be quite large.
			if (aAlpha==0)
				return 0xff;
			else
				return Max(0xff>>4,Min(0xff,(0xff/aAlpha)+0xf));//value corrected by trial and error, with minimum of 15.
		}
	return 0xff>>7;	
	}

void CTBitmap::DoStretchScanline(const TSize& aSize,TDisplayMode aSrcMode,TDisplayMode aDstMode, TBool aUseCompareLikeness)
	{
	__UHEAP_MARK;
	CFbsBitmapEx bmp;
	CBitwiseBitmap* bmpadd=NULL;

	TInt ret = bmp.Create(aSize,aSrcMode);
	TEST2(ret, KErrNone);

	FillBitmap(bmp);
	bmpadd = bmp.BitmapAddress();
	bmp.LockHeap();
	TUint32* base = bmp.DataAddress();
	bmp.UnlockHeap();

	TInt scanLineLength = CFbsBitmap::ScanLineLength(aSize.iWidth,aSrcMode);
	TInt scanLineDestLength = CFbsBitmap::ScanLineLength(aSize.iWidth,aDstMode)*2;
	TUint8* buffer = new TUint8[scanLineLength];
	TUint8* doubleBuffer = new TUint8[scanLineDestLength];
	TPtr8 scanline(buffer,scanLineLength,scanLineLength);
	TPtr8 doubleScanline(doubleBuffer,scanLineDestLength,scanLineDestLength);

	TInt yy;
	for(yy = 0; yy < aSize.iHeight; yy++)
		{
		bmp.GetScanLine(scanline,TPoint(0,yy),aSize.iWidth,aSrcMode);
		bmpadd->StretchScanLine(doubleScanline,TPoint(0,yy),0,aSize.iWidth * 2,aSize.iWidth * 2,0,aSize.iWidth,TPoint(0,0),aDstMode,base);
		for(TInt xx = 0; xx < aSize.iWidth; xx++)
			{
			TRgb scanlineColor = ExtractRgb(buffer,xx,aSrcMode);
			TRgb firstStretchColor = ExtractRgb(doubleBuffer,xx * 2,aDstMode);
			TRgb secondStretchColor = ExtractRgb(doubleBuffer,(xx * 2) + 1,aDstMode);
			TRgb reportedColor;
			bmpadd->GetPixel(reportedColor,TPoint(xx,yy),base,NULL);
			if (!aUseCompareLikeness)
				{
				TEST(scanlineColor == firstStretchColor);
				TEST(firstStretchColor == secondStretchColor);
				TEST(secondStretchColor == reportedColor);
				}
			else
				{
				if (aDstMode==EColor16M||aDstMode==EColor16MU||aDstMode==EColor64K||aDstMode==EColor4K||aDstMode==EColor256)
					{
					//there is no alpha in the destination
					scanlineColor.SetAlpha(0xff);
					reportedColor.SetAlpha(0xff);	
					}	
				CompareColourLikeness(scanlineColor, firstStretchColor, ColorDifference(aDstMode,scanlineColor.Alpha()));
				CompareColourLikeness(firstStretchColor, secondStretchColor,ColorDifference(aDstMode,scanlineColor.Alpha()));
				CompareColourLikeness(secondStretchColor, reportedColor,ColorDifference(aDstMode,scanlineColor.Alpha()));
				}
			}
		}

	for(yy = 0; yy < aSize.iHeight; yy++)
		{
		bmp.GetScanLine(scanline,TPoint(0,yy),aSize.iWidth,aSrcMode);
		bmpadd->StretchScanLine(doubleScanline,TPoint(0,yy),0,aSize.iWidth / 2,aSize.iWidth / 2,0,aSize.iWidth,TPoint(0,0),aDstMode,base);
		for(TInt xx = 0; xx < aSize.iWidth; xx += 2)
			{
			TRgb scanlineColor = ExtractRgb(buffer,xx,aSrcMode);
			TRgb compressColor = ExtractRgb(doubleBuffer,xx / 2,aDstMode);
			TRgb reportedColor;
			bmpadd->GetPixel(reportedColor,TPoint(xx,yy),base,NULL);
			if (!aUseCompareLikeness)
				{
				TEST(scanlineColor == compressColor);
				TEST(compressColor == reportedColor);
				}
			else
				{
				if (aDstMode==EColor16M||aDstMode==EColor16MU||aDstMode==EColor64K||aDstMode==EColor4K||aDstMode==EColor256)
					{
					//there is no alpha in the destination
					scanlineColor.SetAlpha(0xff);
					reportedColor.SetAlpha(0xff);	
					}
				CompareColourLikeness(scanlineColor, compressColor,ColorDifference(aDstMode,scanlineColor.Alpha()));
				CompareColourLikeness(compressColor, reportedColor,ColorDifference(aDstMode,scanlineColor.Alpha()));
				}
			}
		}

	delete [] buffer;
	delete [] doubleBuffer;
	buffer = NULL;
	doubleBuffer = NULL;

	TInt rgbScanLineLength = aSize.iWidth * sizeof(TRgb) * 2;
	TUint8* rgbBuffer = new TUint8[rgbScanLineLength];
	TPtr8 rgbScanline(rgbBuffer,rgbScanLineLength,rgbScanLineLength);

	for(yy = 0; yy < aSize.iHeight; yy++)
		{
		bmpadd->StretchScanLine(rgbScanline,TPoint(0,yy),0,aSize.iWidth * 2,aSize.iWidth * 2,0,aSize.iWidth,TPoint(0,0),ERgb,base);
		for(TInt xx = 0; xx < aSize.iWidth; xx++)
			{
			TRgb firstStretchColor = ExtractRgb(rgbBuffer,xx * 2,ERgb);
			TRgb secondStretchColor = ExtractRgb(rgbBuffer,(xx * 2) + 1,ERgb);
			TRgb reportedColor;
			bmpadd->GetPixel(reportedColor,TPoint(xx,yy),base,NULL);
			if (!aUseCompareLikeness)
				{
				TEST(firstStretchColor == secondStretchColor);
				TEST(secondStretchColor == reportedColor);
				}
			else
				{
				CompareColourLikeness(firstStretchColor, secondStretchColor);
				CompareColourLikeness(secondStretchColor, reportedColor);
				}
			}
		}

	for(yy = 0; yy < aSize.iHeight; yy++)
		{
		bmpadd->StretchScanLine(rgbScanline,TPoint(0,yy),0,aSize.iWidth / 2,aSize.iWidth / 2,0,aSize.iWidth,TPoint(0,0),ERgb,base);
		for(TInt xx = 0; xx < aSize.iWidth; xx += 2)
			{
			TRgb compressColor = ExtractRgb(rgbBuffer,xx / 2,ERgb);
			TRgb reportedColor;
			bmpadd->GetPixel(reportedColor,TPoint(xx,yy),base,NULL);
			if (!aUseCompareLikeness)
				{
				TEST(compressColor == reportedColor);
				}
			else
				{
				CompareColourLikeness(compressColor, reportedColor);
				}
			}
		}

	delete [] rgbBuffer;
	rgbBuffer = NULL;

	bmp.Reset();
	User::Heap().Check();
	DeleteScanLineBuffer();
	__UHEAP_MARKEND;
	}

TRgb CTBitmap::ExtractRgb(TUint8* aBuffer,TInt aPixelOffset,TDisplayMode aDispMode)
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
		return TRgb::Color16MU(*(((TUint32*)aBuffer) + aPixelOffset));
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
	
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0523

	@SYMTestCaseDesc
	Streams a bitmap in segments and compares it
	to an expected bitmap
	
	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::LoadInSegments()
	{
	INFO_PRINTF1(_L("Test Segmented Loading"));

	__UHEAP_MARK;
	CFbsBitmapEx bmp;
	TInt ret=bmp.Load(iTestBitmapName,ETfbs,NULL);
	TEST2(ret, KErrNone);
	CFbsBitmapAsyncStreamer* bmploader=NULL;
	TRAP(ret,bmploader=CFbsBitmapAsyncStreamer::NewL(CFbsBitmapAsyncStreamer::ELoad));
	TEST2(ret, KErrNone);
	TInt slines=0;
	ret=bmploader->Load(iTestBitmapName,ETfbs,slines);
	TEST2(ret, KErrNone);
	TEST(slines>0);
	CFbsBitmap* newbmp=NULL;
	TBool done=EFalse;
	do	{
		TEST(newbmp==NULL);
		TRAP(ret,done=bmploader->LoadScanLinesL(slines/10,newbmp));
		TEST2(ret, KErrNone);
		}
	while(!done);

	TEST(newbmp!=NULL);
	CompareBitmaps(bmp,(CFbsBitmapEx&)*newbmp,EColor16M);
	delete bmploader;
	delete newbmp;

	TRAP(ret,iBmpHandler=CFbsBitmapAsyncStreamer::NewL(CFbsBitmapAsyncStreamer::ELoad));
	TEST2(ret, KErrNone);
	ret=iBmpHandler->Load(iTestBitmapName,ETfbs,slines);
	TEST2(ret, KErrNone);
	TEST(slines>0);
	CIdle* idleobj=CIdle::New(-20);
	TEST(idleobj!=NULL);
	TCallBack callback(BmpLoadCallBack,this);
	idleobj->Start(callback);
	
	CActiveScheduler* as = CActiveScheduler::Current();
	__ASSERT_DEBUG(as, User::Invariant());
	
	TRAP(ret,as->Start());
	TEST2(ret, KErrNone);
	
	CompareBitmaps(bmp,(CFbsBitmapEx&)*iBitmap,EColor16M);

	delete iBmpHandler;
	delete iBitmap;
	bmp.Reset();
	delete idleobj;
	User::Heap().Check();
	__UHEAP_MARKEND;

	
	}

TInt CTBitmap::BmpLoadCallBack(TAny* aPtr)
	{
	CTBitmap* tbmp=(CTBitmap*)aPtr;
	tbmp->INFO_PRINTF1(_L("*"));
	TBool done=EFalse;
	TRAPD(ret,done=tbmp->iBmpHandler->LoadScanLinesL(10,tbmp->iBitmap));
	tbmp->TEST2(ret, KErrNone);
	if(done)
		{
		tbmp->TEST(tbmp->iBitmap!=NULL);
		CActiveScheduler::Stop();
		}
	else 
		{
		tbmp->TEST(tbmp->iBitmap==NULL);
		}
	return(ETrue);
	}
	
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0524

	@SYMTestCaseDesc
	A bitmap is saved in segments and then read back from file store
	and compared to an expected bitmap

	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/	
void CTBitmap::SaveInSegments()
	{
	INFO_PRINTF1(_L("Test Segmented Saving"));

	__UHEAP_MARK;
	CFbsBitmapEx bmp;
	TInt ret=bmp.Load(iTestBitmapName,ETfbs,NULL);
	TEST2(ret, KErrNone);
	CFbsBitmapAsyncStreamer* bmpsaver=NULL;
	TRAP(ret,bmpsaver=CFbsBitmapAsyncStreamer::NewL(CFbsBitmapAsyncStreamer::ESave));
	TEST2(ret, KErrNone);
	TInt slines=0;
	TInt32 id;
	ret=bmpsaver->Save(KTempFilename,&bmp,id,slines);
	TEST2(ret, KErrNone);
	TEST(id==0);
	TEST(slines>0);
	TBool done=EFalse;
	do	{
		TRAP(ret,done=bmpsaver->SaveScanLinesL(slines/10));
		TEST2(ret, KErrNone);
		}
	while(!done);
	delete bmpsaver;
	CFbsBitmapEx bmp2;
	ret=bmp2.Load(KTempFilename,id,NULL);
	TEST2(ret, KErrNone);
	TUint8 buffer[40];
	TUint8 bufferst[40];
	TPtr8 scanline(&buffer[0],40,40);
	TPtr8 scanlinest(&bufferst[0],40,40);
	TInt yy=0;
	for(;yy<80;yy++)
		{
		bmp.GetScanLine(scanline,TPoint(0,yy),160,EGray4);
		bmp2.GetScanLine(scanlinest,TPoint(0,yy),160,EGray4);
		for(TInt count=0;count<40;count++)
			TEST(buffer[count]==bufferst[count]);
		}
	bmp2.Reset();
	iFs.Delete(KTempFilename);

	CActiveScheduler* as = CActiveScheduler::Current();
	__ASSERT_DEBUG(as, User::Invariant());

	TRAP(ret,iBmpHandler=CFbsBitmapAsyncStreamer::NewL(CFbsBitmapAsyncStreamer::ESave));
	TEST2(ret, KErrNone);
	ret=iBmpHandler->Save(KTempFilename,&bmp,id,slines);
	TEST2(ret, KErrNone);
	TEST(id==0);
	TEST(slines>0);
	CIdle* idleobj=CIdle::New(-20);
	TEST(idleobj!=NULL);
	TCallBack callback(BmpSaveCallBack,this);
	idleobj->Start(callback);
	TRAP(ret,as->Start());
	TEST2(ret, KErrNone);
	delete iBmpHandler;
	ret=bmp2.Load(KTempFilename,id,NULL);
	TEST2(ret, KErrNone);
	for(yy=0;yy<80;yy++)
		{
		bmp.GetScanLine(scanline,TPoint(0,yy),160,EGray4);
		bmp2.GetScanLine(scanlinest,TPoint(0,yy),160,EGray4);
		for(TInt count=0;count<40;count++)
			TEST(buffer[count]==bufferst[count]);
		}
	bmp.Reset();
	delete idleobj;
	iFs.Delete(KTempFilename);
	User::Heap().Check();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("\r\n"));
	
	}

TInt CTBitmap::BmpSaveCallBack(TAny* aPtr)
	{
	CTBitmap* tbmp=(CTBitmap*)aPtr;
	tbmp->INFO_PRINTF1(_L("*"));
	TBool done=EFalse;
	TRAPD(ret,done=tbmp->iBmpHandler->SaveScanLinesL(10));
	tbmp->TEST2(ret, KErrNone);
	if(done) 
		{
		CActiveScheduler::Stop();
		}
	return(ETrue);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0525

	@SYMTestCaseDesc
	Tests if a bitmap is monochrone for different
	color depths

	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::MonoDetect()
	{
	INFO_PRINTF1(_L("Test Mono Detection"));

	DoMonoDetect(EGray2);
	DoMonoDetect(EGray4);
	DoMonoDetect(EGray16);
	DoMonoDetect(EGray256);
	DoMonoDetect(EColor16);
	DoMonoDetect(EColor256);
	DoMonoDetect(EColor4K);
	DoMonoDetect(EColor64K);
	DoMonoDetect(EColor16M);
	DoMonoDetect(EColor16MU);
	DoMonoDetect(EColor16MAP);
	}

void CTBitmap::DoMonoDetect(TDisplayMode aDisplayMode)
	{
	CFbsBitmapEx bmp;
	TInt ret = bmp.Create(TSize(100,3),aDisplayMode);
	TEST2(ret, KErrNone);

	CBitwiseBitmap* bmpadd = bmp.BitmapAddress();
	bmp.LockHeap();
	TUint32* base = bmp.DataAddress();
	bmp.UnlockHeap();
	TUint32* data = bmpadd->ScanLineAddress(base,1);

	if(aDisplayMode == EColor16M)
		{
		*(data+1) = 0x0000FFFF;
		*(data+2) = 0xFFFFFF00;
		//Pixel at (data + 1). Bytes (3) and (2) are part of the pixel at (data) addr.
		//LSB                        MSB
		//3        2        1        0
		//FF       FF       00       00  
		//Pixel at (data + 2). Byte (3) is part of the pixel at (data + 1) addr.
		//LSB                        MSB
		//3        2        1        0
		//00       FF       FF       FF
		}
	else
		{
		*(data+1) = 0;
		}
	TBool mono = bmp.IsMonochrome();
	TEST(mono);

	*(data+1) = 0x1234abcd;
	mono = bmp.IsMonochrome();
	if (aDisplayMode == EGray2)
		TEST(mono);
	else
		TEST(!mono);

	if(aDisplayMode == EColor16M)
		{
		*(data+1) = 0xffffffff;
		*(data+2) = 0xffffffff;
		}
	else
		{
		*(data+1) = 0xffffffff;
		}
	mono=bmp.IsMonochrome();
	TEST(mono);

	bmp.Reset();
	}


#if defined(__WINS__)
// these are patchable constants defined in fbscli.dll but on the emulator
// it's not possible to import them so they are re-defined here
const TInt KFbServLargeChunkGrowByShifter = 1;
const TInt KFbServLargeChunkSizeShifter = 2;
const TInt KFbServLargeChunkMinPhysicalSize = 0x4000;
#elif defined(__X86GCC__)
IMPORT_D extern const TInt KFbServLargeChunkGrowByShifter;
IMPORT_D extern const TInt KFbServLargeChunkSizeShifter;
IMPORT_D extern const TInt KFbServLargeChunkMinPhysicalSize;
#endif

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0526

	@SYMTestCaseDesc
	Allocates different sized chunks of memory
	in a loop and frees them

	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::ChunkPileL()
	{
	INFO_PRINTF1(_L("Test Chunk Pile"));

	TInt virtualSize = 0x4000000;
	RChunk chunk;
	TInt ret = chunk.CreateDisconnectedGlobal(_L("TFbsLargeChunk"), 0, 0, virtualSize);
	TEST2(ret, KErrNone);
	CleanupClosePushL(chunk);
	TInt pageSize = 0;
	HAL::Get(HAL::EMemoryPageSize, pageSize);
	TEST(pageSize == 4096);
	TUint8* chunkSmallBase = chunk.Base() + pageSize;
	TUint8* chunkLargeBase = chunk.Base() + (virtualSize >> KFbServLargeChunkSizeShifter);

	CChunkPile* cp=NULL;
	TRAP(ret, cp = CChunkPile::NewL(chunk));
	TEST2(ret, KErrNone);
	TEST(cp!=NULL);
	CleanupStack::PushL(cp);
	TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);

	TUint8* chunkalloc[16];
	TInt size[16];
	for (TInt i = 0; i < 64; ++i)
		{
		chunkalloc[0] = cp->Alloc(size[0] = 512);
		TEST(chunkalloc[0] == chunkSmallBase);
		chunkalloc[1] = cp->Alloc(size[1] = 8);
		TEST(chunkalloc[1] == chunkalloc[0] + size[0]);
		chunkalloc[2] = cp->Alloc(size[2] = 128);
		TEST(chunkalloc[2] == chunkalloc[1] + size[1]);
		chunkalloc[3] = cp->Alloc(size[3] = 372);
		TEST(chunkalloc[3] == chunkalloc[2] + size[2]);
		chunkalloc[4] = cp->Alloc(size[4] = 256);
		TEST(chunkalloc[4] == chunkalloc[3] + size[3]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);
		// now we should have [0], [1], [2], [3], [4]

		cp->Free(chunkalloc[2]);
		cp->Free(chunkalloc[3]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);
		chunkalloc[5] = cp->Alloc(size[5] = size[2] + size[3]);
		TEST(chunkalloc[5] == chunkalloc[2]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);
		// now we should have [0], [1], [5], [4]

		chunkalloc[6] = cp->Alloc(size[6] = 4);
		TEST(chunkalloc[6] == chunkalloc[4] + size[4]);
		chunkalloc[7] = cp->Alloc(size[7] = 1024);
		TEST(chunkalloc[7] == chunkalloc[6] + size[6]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);
		cp->Free(chunkalloc[7]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);
		// now we should have [0], [1], [5], [4], [6]

		cp->Free(chunkalloc[1]);
		cp->Free(chunkalloc[0]);
		cp->Free(chunkalloc[4]);
		cp->Free(chunkalloc[5]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);
		chunkalloc[8] = cp->Alloc(size[8] = size[0] + size[1] + size[4] + size[5] - 32);
		TEST(chunkalloc[8] == chunkalloc[0]);
		chunkalloc[9] = cp->Alloc(size[9] = 32);
		TEST(chunkalloc[9] == chunkalloc[8] + size[8]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);
		// now we should have [8], [9], [6]

		chunkalloc[10] = cp->Alloc(size[10] = KFbServLargeChunkMinPhysicalSize - size[6] - size[8] - size[9]);
		TEST(chunkalloc[10] == chunkalloc[6] + size[6]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);
		chunkalloc[11] = cp->Alloc(size[11] = 16);
		TEST(chunkalloc[11] == chunkalloc[10] + size[10]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + (pageSize << KFbServLargeChunkGrowByShifter));
		chunkalloc[12] = cp->Alloc(size[12] = ((pageSize << KFbServLargeChunkGrowByShifter) - 16) / 2);
		TEST(chunkalloc[12] == chunkalloc[11] + size[11]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + (pageSize << KFbServLargeChunkGrowByShifter));
		chunkalloc[13] = cp->Alloc(size[13] = size[12]);
		TEST(chunkalloc[13] == chunkalloc[12] + size[12]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + (pageSize << KFbServLargeChunkGrowByShifter));
		chunkalloc[14] = cp->Alloc(size[14] = pageSize << KFbServLargeChunkGrowByShifter);
		TEST(chunkalloc[14] == chunkalloc[13] + size[13]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + 2 * (pageSize << KFbServLargeChunkGrowByShifter));
		chunkalloc[15] = cp->Alloc(size[15] = (pageSize << KFbServLargeChunkGrowByShifter) + 4);
		TEST(chunkalloc[15] == chunkalloc[14] + size[14]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + 4 * (pageSize << KFbServLargeChunkGrowByShifter));
		// now we should have [8], [9], [6], [10], [11], [12], [13], [14], [15]

		cp->Free(chunkalloc[15]);
		cp->Free(chunkalloc[14]);
		cp->Free(chunkalloc[13]);
		cp->Free(chunkalloc[12]);
		cp->Free(chunkalloc[11]);
		cp->Free(chunkalloc[10]);
		cp->Free(chunkalloc[9]);
		cp->Free(chunkalloc[8]);
		cp->Free(chunkalloc[6]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);

		TEST(cp->Alloc(virtualSize) == NULL);

		TInt largeSize = 4 * pageSize;
		chunkalloc[0] = cp->Alloc(size[0] = 2 * largeSize);
		TEST(chunkalloc[0] == chunkLargeBase);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[0]);
		chunkalloc[1] = cp->Alloc(2 * largeSize + 1);
		size[1] = 2 * largeSize + pageSize;
		TEST(chunkalloc[1] == chunkalloc[0] + size[0]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[0] + size[1]);
		chunkalloc[2] = cp->Alloc(size[2] = 3 * largeSize + pageSize);
		TEST(chunkalloc[2] == chunkalloc[1] + size[1]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[0] + size[1] + size[2]);
		// now we should have [0], [1], [2]

		cp->Free(chunkalloc[1]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[0] + size[2]);
		chunkalloc[3] = cp->Alloc(largeSize + 1);
		size[3] = largeSize + pageSize;
		TEST(chunkalloc[3] == chunkalloc[1]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[0] + size[2] + size[3]);
		chunkalloc[4] = cp->Alloc(size[4] = largeSize);
		TEST(chunkalloc[4] == chunkalloc[3] + size[3]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[0] + size[2] + size[3] + size[4]);
		chunkalloc[5] = cp->Alloc(size[5] = 64 * largeSize);
		TEST(chunkalloc[5] == chunkalloc[2] + size[2]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[0] + size[2] + size[3] + size[4] + size[5]);
		// now we should have [0], [3], [4], [2], [5]

		cp->Free(chunkalloc[0]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[2] + size[3] + size[4] + size[5]);
		cp->Free(chunkalloc[2]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[3] + size[4] + size[5]);
		cp->Free(chunkalloc[3]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[4] + size[5]);
		cp->Free(chunkalloc[4]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize + size[5]);
		cp->Free(chunkalloc[5]);
		TEST(chunk.Size() == KFbServLargeChunkMinPhysicalSize);
		}

	CleanupStack::PopAndDestroy(2, &chunk); // cp, chunk
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0527

	@SYMTestCaseDesc
	Creates large bitmaps of different colour depth. 

	@SYMTestActions
	The bitmaps allocated are too big so
	the allocation should fail.
	
	@SYMTestExpectedResults
	KErrNoMemory when allocating the bitmaps. 
*/
void CTBitmap::LargeBitmaps()
	{
	INFO_PRINTF1(_L("Test Large Bitmaps"));

	TMemoryInfoV1Buf membuf;
	UserHal::MemoryInfo(membuf);
	const TReal maxmem = membuf().iTotalRamInBytes;

	TReal realSize = 0;
	TInt ret = Math::Sqrt(realSize,maxmem);
	TEST2(ret, KErrNone);
	TInt32 size = 0;
	ret = Math::Int(size,realSize);
	TEST2(ret, KErrNone);

	DoLargeBitmaps(TSize(size * 8,size),EGray2);
	DoLargeBitmaps(TSize(size * 4,size),EGray4);
	DoLargeBitmaps(TSize(size * 2,size),EGray16);
	DoLargeBitmaps(TSize(size,size),EGray256);
	DoLargeBitmaps(TSize(size * 2,size),EColor16);
	DoLargeBitmaps(TSize(size,size),EColor256);
	DoLargeBitmaps(TSize(size / 2,size),EColor4K);
	DoLargeBitmaps(TSize(size / 2,size),EColor64K);
	DoLargeBitmaps(TSize(size / 3,size),EColor16M);
	DoLargeBitmaps(TSize(size / 4,size),EColor16MU);
	DoLargeBitmaps(TSize(size / 4,size),EColor16MAP);
	}

void CTBitmap::DoLargeBitmaps(const TSize& aSize,TDisplayMode aDisplayMode)
	{
	CFbsBitmapEx bmp1;
	TInt ret = bmp1.Create(aSize,aDisplayMode);
	TEST2(ret, KErrNoMemory);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0528

	@SYMTestCaseDesc
	Check error handling for bitmaps above pixel and byte size limits

	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::HugeBitmaps()
	{
	// Check error handling for bitmaps above pixel and byte size limits
	INFO_PRINTF1(_L("Test Huge Bitmaps"));

	const TInt KLargePixelSize = 50000;

	DoHugeBitmaps(TSize(KLargePixelSize * 8,KLargePixelSize),EGray2);
	DoHugeBitmaps(TSize(KLargePixelSize * 4,KLargePixelSize),EGray4);
	DoHugeBitmaps(TSize(KLargePixelSize * 2,KLargePixelSize),EGray16);
	DoHugeBitmaps(TSize(KLargePixelSize,KLargePixelSize),EGray256);
	DoHugeBitmaps(TSize(KLargePixelSize * 2,KLargePixelSize),EColor16);
	DoHugeBitmaps(TSize(KLargePixelSize,KLargePixelSize),EColor256);
	DoHugeBitmaps(TSize(KLargePixelSize / 2,KLargePixelSize),EColor4K);
	DoHugeBitmaps(TSize(KLargePixelSize / 2,KLargePixelSize),EColor64K);
	DoHugeBitmaps(TSize(KLargePixelSize / 3,KLargePixelSize),EColor16M);
	DoHugeBitmaps(TSize(KLargePixelSize / 4,KLargePixelSize),EColor16MU);
	DoHugeBitmaps(TSize(KLargePixelSize / 4,KLargePixelSize),EColor16MAP);
	}

void CTBitmap::DoHugeBitmaps(const TSize& aSize,TDisplayMode aDisplayMode)
	{
	const TInt KMaxPixelSize = KMaxTInt / 4 + 1;

	CFbsBitmap bmp;

	TInt ret = bmp.Create(TSize(aSize.iWidth / 2,aSize.iHeight / 2),aDisplayMode);
	TEST2(ret, KErrNoMemory);

	ret = bmp.Create(aSize,aDisplayMode);
	TEST2(ret, KErrTooBig);

	ret = bmp.Create(TSize(1,KMaxPixelSize),aDisplayMode);
	TEST2(ret, KErrTooBig);

	ret = bmp.Create(TSize(KMaxPixelSize,1),aDisplayMode);
	TEST2(ret, KErrTooBig);

	ret = bmp.Create(TSize(KMaxPixelSize,KMaxPixelSize),aDisplayMode);
	TEST2(ret, KErrTooBig);

	ret = bmp.Create(TSize(KMaxTInt,KMaxTInt),aDisplayMode);
	TEST2(ret, KErrTooBig);


	ret = bmp.Create(TSize(1,1),aDisplayMode);
	TEST2(ret, KErrNone);

	ret = bmp.Resize(TSize(aSize.iWidth / 2,aSize.iHeight / 2));
	TEST2(ret, KErrNoMemory);

	ret = bmp.Resize(aSize);
	TEST2(ret, KErrTooBig);

	ret = bmp.Resize(TSize(1,KMaxPixelSize));
	TEST2(ret, KErrTooBig);

	ret = bmp.Resize(TSize(KMaxPixelSize,1));
	TEST2(ret, KErrTooBig);

	ret = bmp.Resize(TSize(KMaxPixelSize,KMaxPixelSize));
	TEST2(ret, KErrTooBig);

	ret = bmp.Resize(TSize(KMaxTInt,KMaxTInt));
	TEST2(ret, KErrTooBig);

	bmp.Reset();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0529

	@SYMTestCaseDesc
	Resizes a bitmap to a larger size and
	checks if the new bitmap is as expected
	
	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::Resize()
	{
	INFO_PRINTF1(_L("Test Resizing"));

	DoResize(EGray2);
	DoResize(EGray4);
	DoResize(EGray16);
	DoResize(EGray256);
	DoResize(EColor16);
	DoResize(EColor256);
	DoResize(EColor4K);
	DoResize(EColor64K);
	DoResize(EColor16M);
	DoResize(EColor16MU);
	DoResize(EColor16MAP);
	}

void CTBitmap::DoResize(TDisplayMode aDispMode)
	{
	const TSize KBitmapOriginalSize(256,24);
	const TSize KBitmapLargeSize(288,32);
	const TSize KBitmapSmallSize(1,1);

	CFbsBitmapEx bmp;
	TInt ret = bmp.Create(KBitmapOriginalSize,aDispMode);
	TEST2(ret, KErrNone);
	bmp.SetSizeInTwips(KBitmapOriginalSize);
	TEST(bmp.SizeInPixels() == KBitmapOriginalSize);
	TEST(bmp.SizeInTwips() == KBitmapOriginalSize);

	CBitwiseBitmap* bb = bmp.BitmapAddress();
	bmp.LockHeap();
	TUint32* base = bmp.DataAddress();
	bmp.UnlockHeap();

	TInt scanLineLength = CFbsBitmap::ScanLineLength(256,aDispMode);
	TInt blockStart = scanLineLength / 4;
	TInt blockLength = scanLineLength / 2;

	TInt row;
	for(row = 8; row < 16; row++)
		{
		TUint8* bmpadd = (TUint8*)bb->ScanLineAddress(base,row);
		Mem::FillZ(bmpadd + blockStart,blockLength);
		}

	ret = bmp.Resize(KBitmapLargeSize);
	TEST2(ret, KErrNone);
	TEST(bmp.SizeInPixels() == KBitmapLargeSize);
	TEST(bmp.SizeInTwips() == KBitmapLargeSize);
	bb = bmp.BitmapAddress();
	bmp.LockHeap();
	base = bmp.DataAddress();
	bmp.UnlockHeap();
	scanLineLength = CFbsBitmap::ScanLineLength(288,aDispMode);

	for(row = 0; row < 8; row++)
		{
		TUint8* bmpadd=(TUint8*)bb->ScanLineAddress(base,row);
		CheckWhite(bmpadd,scanLineLength,aDispMode);
		}

	for(row = 16; row < 32; row++)
		{
		TUint8* bmpadd=(TUint8*)bb->ScanLineAddress(base,row);
		CheckWhite(bmpadd,scanLineLength,aDispMode);
		}

	for(row = 8; row < 16; row++)
		{
		TUint8* bmpadd=(TUint8*)bb->ScanLineAddress(base,row);
		CheckWhite(bmpadd,blockStart,aDispMode);

		TUint8* ptr = bmpadd + blockStart;
		TUint8* ptrLimit = ptr + blockLength;
		while (ptr < ptrLimit)
			TEST(*ptr++ == 0);

		CheckWhite(ptr,scanLineLength - blockStart - blockLength,aDispMode);
		}

	ret = bmp.Resize(KBitmapSmallSize);
	TEST2(ret, KErrNone);
	TEST(bmp.SizeInPixels() == KBitmapSmallSize);
	TEST(bmp.SizeInTwips() == KBitmapSmallSize);

	ret = bmp.Resize(KBitmapOriginalSize);
	TEST2(ret, KErrNone);
	TEST(bmp.SizeInPixels() == KBitmapOriginalSize);
	TEST(bmp.SizeInTwips() == KBitmapOriginalSize);

	bmp.Reset();
	}

void CTBitmap::CheckWhite(TUint8* aData,TInt aDataLength,TDisplayMode aDispMode)
	{
	TUint8* dataLimit = aData + aDataLength;

	if (aDispMode != EColor4K)
		{
		while (aData < dataLimit)
			TEST(*aData++ == 0xff);
		}
	else
		{
		while (aData < dataLimit)
			{
			TEST(*aData == 0xff || *aData == 0x0f);
			aData++;
			}
		}
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0530

	@SYMTestCaseDesc
	Creates BitmapUtil objects and sets the
	current position to different positions
	and retrieves and sets the pixels in these
	and compares it to another Bitmap Util object
	
	@SYMTestActions

	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::BitmapUtil()
	{
	INFO_PRINTF1(_L("Test Bitmap Util"));

	TSize bmpUtilSize1(256,4);
	TSize bmpUtilSize2(256,1);
	TSize bmpUtilSize3(256,256);
	TSize bmpUtilSize4(1024,1024);

	INFO_PRINTF1(_L("256x4.. "));
	DoBitmapUtil(bmpUtilSize1,EGray2);
	DoBitmapUtil(bmpUtilSize1,EGray4);
	DoBitmapUtil(bmpUtilSize1,EGray16);
	DoBitmapUtil(bmpUtilSize1,EGray256);
	DoBitmapUtil(bmpUtilSize1,EColor16);
	DoBitmapUtil(bmpUtilSize1,EColor256);
	DoBitmapUtil(bmpUtilSize1,EColor4K);
	DoBitmapUtil(bmpUtilSize1,EColor64K);
	DoBitmapUtil(bmpUtilSize1,EColor16M);
	DoBitmapUtil(bmpUtilSize1,EColor16MU);
	DoBitmapUtil(bmpUtilSize1,EColor16MAP);

	INFO_PRINTF1(_L("256x1.. "));
	DoBitmapUtil(bmpUtilSize2,EGray2);
	DoBitmapUtil(bmpUtilSize2,EGray4);
	DoBitmapUtil(bmpUtilSize2,EGray16);
	DoBitmapUtil(bmpUtilSize2,EGray256);
	DoBitmapUtil(bmpUtilSize2,EColor16);
	DoBitmapUtil(bmpUtilSize2,EColor256);
	DoBitmapUtil(bmpUtilSize2,EColor4K);
	DoBitmapUtil(bmpUtilSize2,EColor64K);
	DoBitmapUtil(bmpUtilSize2,EColor16M);
	DoBitmapUtil(bmpUtilSize2,EColor16MU);
	DoBitmapUtil(bmpUtilSize2,EColor16MAP);

	INFO_PRINTF1(_L("256x256..\n"));
	DoBitmapUtil(bmpUtilSize3,EGray2);
	DoBitmapUtil(bmpUtilSize3,EGray4);
	DoBitmapUtil(bmpUtilSize3,EGray16);
	DoBitmapUtil(bmpUtilSize3,EGray256);
	DoBitmapUtil(bmpUtilSize3,EColor16);
	DoBitmapUtil(bmpUtilSize3,EColor256);
	DoBitmapUtil(bmpUtilSize3,EColor4K);
	DoBitmapUtil(bmpUtilSize3,EColor64K);
	DoBitmapUtil(bmpUtilSize3,EColor16M);
	DoBitmapUtil(bmpUtilSize3,EColor16MU);
	DoBitmapUtil(bmpUtilSize3,EColor16MAP);
		
    INFO_PRINTF1(_L("Testing wastage bytes in 16M scanlines...\n"));
	bmpUtilSize1.SetSize(5,13);
	bmpUtilSize2.SetSize(6,14);
	bmpUtilSize3.SetSize(7,15);
	bmpUtilSize4.SetSize(8,16);
	DoBitmapUtil(bmpUtilSize1,EColor16M);
	DoBitmapUtil(bmpUtilSize2,EColor16M);
	DoBitmapUtil(bmpUtilSize3,EColor16M);
	DoBitmapUtil(bmpUtilSize4,EColor16M);

    /*
	 * Test code commented out as it results in test case failure. 
	 * Problem recorded in defect DEF001233.
	 *
	INFO_PRINTF1(_L("Testing wastage bits in Gray16 scanlines...\n\n"));
	bmpUtilSize1.SetSize(4,13);
	bmpUtilSize2.SetSize(5,14);
	bmpUtilSize3.SetSize(6,15);
	bmpUtilSize4.SetSize(7,16);
	DoBitmapUtil(bmpUtilSize1,EGray16);
	DoBitmapUtil(bmpUtilSize2,EGray16);
	DoBitmapUtil(bmpUtilSize3,EGray16);
	DoBitmapUtil(bmpUtilSize4,EGray16);
	 */
	}

void CTBitmap::DoBitmapUtil(const TSize& aSize,TDisplayMode aDispMode)
	{
	const TPoint startPoint(aSize.iWidth / 3,aSize.iHeight / 3);
	const TPoint zeroPoint(0,0);

	CFbsBitmapEx bmp1;
	TInt ret=bmp1.Create(aSize,aDispMode);
	TEST2(ret, KErrNone);
	FillBitmap(bmp1);


	const TInt numPixels = ((aSize.iWidth + 31) & ~0x1f) * aSize.iHeight;
	TInt count1;
	CFbsBitmapEx bmp2;

	ret=bmp2.Create(aSize,aDispMode);
	TEST2(ret, KErrNone);

	TBitmapUtil util1(&bmp1);
	util1.Begin(zeroPoint);
	TBitmapUtil util2(&bmp2);
	util2.Begin(zeroPoint,util1);

	TPoint currentPosition;
	for (currentPosition.iY = 0; currentPosition.iY < aSize.iHeight; currentPosition.iY++)
		for (currentPosition.iX = 0; currentPosition.iX < aSize.iWidth; currentPosition.iX++)
			{
			util1.SetPos(currentPosition);
			util2.SetPos(currentPosition);
			TUint32 pixelData = util1.GetPixel();
			util2.SetPixel(pixelData);
			}
	util1.End();
	util2.End();
	CompareBitmaps(bmp1,bmp2,EColor16M);
	bmp2.Reset();

	ret=bmp2.Create(aSize,aDispMode);
	TEST2(ret, KErrNone);
	util1.Begin(startPoint);
	util2.Begin(startPoint,util1);
	for (count1 = 0; count1 < numPixels; count1++)
		{
		util2.SetPixel(util1);
		util1.IncXPos();
		util2.IncXPos();
		}
	util1.End();
	util2.End();
	CompareBitmaps(bmp1,bmp2,EColor16M);
	bmp2.Reset();

	ret=bmp2.Create(aSize,aDispMode);
	TEST2(ret, KErrNone);
	util1.Begin(zeroPoint);
	util2.Begin(zeroPoint,util1);
	for (currentPosition.iX = 0; currentPosition.iX < aSize.iWidth; currentPosition.iX++)
		{
		for (currentPosition.iY = 0; currentPosition.iY < aSize.iHeight; currentPosition.iY++)
			{
			util2.SetPixel(util1);
			util1.IncYPos();
			util2.IncYPos();
			}
		util1.IncXPos();
		util2.IncXPos();
		}
	util1.End();
	util2.End();
	CompareBitmaps(bmp1,bmp2,EColor16M);
	bmp2.Reset();

	ret=bmp2.Create(aSize,aDispMode);
	TEST2(ret, KErrNone);
	util1.Begin(startPoint);
	util2.Begin(startPoint,util1);
	for (count1 = 0; count1 < numPixels; count1++)
		{
		util2.SetPixel(util1);
		util1.DecXPos();
		util2.DecXPos();
		}
	util1.End();
	util2.End();
	CompareBitmaps(bmp1,bmp2,EColor16M);
	bmp2.Reset();

	ret=bmp2.Create(aSize,aDispMode);
	TEST2(ret, KErrNone);
	util1.Begin(zeroPoint);
	util2.Begin(zeroPoint,util1);
	for (currentPosition.iX = 0; currentPosition.iX < aSize.iWidth; currentPosition.iX++)
		{
		for (currentPosition.iY = 0; currentPosition.iY < aSize.iHeight; currentPosition.iY++)
			{
			util2.SetPixel(util1);
			util1.DecYPos();
			util2.DecYPos();
			}
		util1.DecXPos();
		util2.DecXPos();
		}
	util1.End();
	util2.End();
	CompareBitmaps(bmp1,bmp2,EColor16M);
	bmp2.Reset();

	bmp1.Reset();
	}

void CTBitmap::BitmapTiming()
	{
	INFO_PRINTF1(_L("Test Timing"));

	CFbsBitmap bmp;
	TInt ret=bmp.Load(iTestBitmapName,ETfbs,NULL);
	TEST2(ret, KErrNone);

	CDirectFileStore* writestore=NULL;
	TRAP(ret,writestore=CDirectFileStore::ReplaceL(iFs,KTempFilename,EFileStream|EFileWrite));
	TEST2(ret, KErrNone);
	TUidType uidtype(KDirectFileStoreLayoutUid,KMultiBitmapFileImageUid);
	TRAP(ret,writestore->SetTypeL(uidtype));
	TEST2(ret, KErrNone);
	RStoreWriteStream writestrm;
	TStreamId headerid(0);
	TRAP(ret,headerid=writestrm.CreateL(*writestore));
	TEST2(ret, KErrNone);
	TRAP(ret,writestore->SetRootL(headerid));
	TEST2(ret, KErrNone);
	INFO_PRINTF1(_L("Save time: "));
	TUint startTime=User::TickCount();
	for(TInt count=0;count<5000;count++)
		{
		TRAP(ret,bmp.ExternalizeL(writestrm));
		if(ret) break;
		}
	TUint stopTime=User::TickCount();
	TEST2(ret, KErrNone);
	writestrm.Close();
	delete writestore;
	INFO_PRINTF2(_L("%d ticks\r\n"),stopTime-startTime);

	CDirectFileStore* readstore=NULL;
	TRAP(ret,readstore=CDirectFileStore::OpenL(iFs,KTempFilename,EFileStream|EFileRead));
	TEST2(ret, KErrNone);
	RStoreReadStream readstrm;
	headerid=readstore->Root();
	INFO_PRINTF1(_L("Load time: "));
	startTime=User::TickCount();
	for(TInt count2=0;count2<5000;count2++)
		{
		TRAP(ret,readstrm.OpenL(*readstore,headerid));
		if(ret) break;
		TRAP(ret,bmp.InternalizeL(readstrm));
		if(ret) break;
		readstrm.Close();
		}
	stopTime=User::TickCount();
	TEST2(ret, KErrNone);
	delete readstore;
	INFO_PRINTF2(_L("%d ticks\r\n"),stopTime-startTime);

	bmp.Reset();
	iFs.Delete(KTempFilename);

	
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0531

	@SYMTestCaseDesc
	Tries to load non existing bitmaps from the
	Z-drive.

	@SYMTestActions

	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::InvalidBitmap()
	{
	INFO_PRINTF1(_L("Test Invalid Bitmap"));

	__UHEAP_MARK;
	CFbsBitmapEx bmp;

	TInt ret=bmp.Load(_L("z:\\tfbsx.mbm"),ETfbs,NULL);
	TEST2(ret, KErrNotFound);
	ret=bmp.Load(_L("z:\\tfbs.xbm"),ETfbs,NULL);
	TEST2(ret, KErrNotFound);
	ret=bmp.Load(iTestBitmapName,999,NULL);
	TEST2(ret, KErrEof);
	ret=bmp.Load(iTestBitmapName,-1,NULL);
	TEST2(ret, KErrEof);

	bmp.Reset();
	User::Heap().Check();
	__UHEAP_MARKEND;

	
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0532

	@SYMTestCaseDesc
	Creates bitmaps and saves them to the C-drive
	and loads them back again and compares them to
	the original bitmaps. 

	@SYMTestActions
	The test uses different colour depths and sizes
	of the bitmaps. The sizes of the bitmaps are
	large.

	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::SaveAndLoad()
	{
	INFO_PRINTF1(_L("Test Save/Load Bitmap"));

	const TInt sizes[] = { 1,2,3,4,5,7,8,9,15,16,17,31,32,33,100 };
	const TInt numSizes = sizeof(sizes) / sizeof(TInt);
	TSize size;

	for (TInt widthIndex = 0; widthIndex < numSizes; widthIndex++)
		{
		INFO_PRINTF3(_L("Index %d/%d\r"), widthIndex + 1, numSizes);
		size.iWidth = sizes[widthIndex];

		for (TInt heightIndex = 0; heightIndex < numSizes; heightIndex++)
			{
			size.iHeight = sizes[heightIndex];

			DoSaveAndLoad(size,EGray2,EFalse);
			DoSaveAndLoad(size,EGray2,ETrue);
			DoSaveAndLoad(size,EGray4,EFalse);
			DoSaveAndLoad(size,EGray4,ETrue);
			DoSaveAndLoad(size,EGray16,EFalse);
			DoSaveAndLoad(size,EGray16,ETrue);
			DoSaveAndLoad(size,EGray256,EFalse);
			DoSaveAndLoad(size,EGray256,ETrue);
			DoSaveAndLoad(size,EColor16,EFalse);
			DoSaveAndLoad(size,EColor16,ETrue);
			DoSaveAndLoad(size,EColor256,EFalse);
			DoSaveAndLoad(size,EColor256,ETrue);
			DoSaveAndLoad(size,EColor4K,EFalse);
			DoSaveAndLoad(size,EColor4K,ETrue);
			DoSaveAndLoad(size,EColor64K,EFalse);
			DoSaveAndLoad(size,EColor64K,ETrue);
			DoSaveAndLoad(size,EColor16M,EFalse);
			DoSaveAndLoad(size,EColor16M,ETrue);
			DoSaveAndLoad(size,EColor16MU,EFalse);
			DoSaveAndLoad(size,EColor16MU,ETrue);
			DoSaveAndLoad(size,EColor16MAP,EFalse);
			DoSaveAndLoad(size,EColor16MAP,ETrue);
			}
		}

	INFO_PRINTF1(_L("\r\n"));
	
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0533

	@SYMTestCaseDesc
	Creates bitmaps and saves them to the C-drive
	and loads them back again and compares them to
	the original bitmaps. 

	@SYMTestActions
	The test uses different colour depths and sizes
	of the bitmaps. The sizes of the bitmaps are
	small.

	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::SaveAndLoadFast()
	{
	INFO_PRINTF1(_L("Test Save/Load Fast Bitmap"));

	// Keep this array small as the test time increases with n^2
	// This way we can put in bigger sizes...
	const TInt sizes[] = { 501, 107, 213 };
	const TInt numSizes = sizeof(sizes) / sizeof(TInt);
	TSize size;

	for (TInt widthIndex = 0; widthIndex < numSizes; widthIndex++)
		{
		INFO_PRINTF3(_L("Index %d/%d\r"), widthIndex + 1, numSizes);
		size.iWidth = sizes[widthIndex];

		for (TInt heightIndex = 0; heightIndex < numSizes; heightIndex++)
			{
			size.iHeight = sizes[heightIndex];

			DoSaveAndLoad(size,EGray2,EFalse);
			DoSaveAndLoad(size,EGray2,ETrue);
			DoSaveAndLoad(size,EGray4,EFalse);
			DoSaveAndLoad(size,EGray4,ETrue);
			DoSaveAndLoad(size,EGray16,EFalse);
			DoSaveAndLoad(size,EGray16,ETrue);
			DoSaveAndLoad(size,EGray256,EFalse);
			DoSaveAndLoad(size,EGray256,ETrue);
			DoSaveAndLoad(size,EColor16,EFalse);
			DoSaveAndLoad(size,EColor16,ETrue);
			DoSaveAndLoad(size,EColor256,EFalse);
			DoSaveAndLoad(size,EColor256,ETrue);
			DoSaveAndLoad(size,EColor4K,EFalse);
			DoSaveAndLoad(size,EColor4K,ETrue);
			DoSaveAndLoad(size,EColor64K,EFalse);
			DoSaveAndLoad(size,EColor64K,ETrue);
			DoSaveAndLoad(size,EColor16M,EFalse);
			DoSaveAndLoad(size,EColor16M,ETrue);
			DoSaveAndLoad(size,EColor16MU,EFalse);
			DoSaveAndLoad(size,EColor16MU,ETrue);
			DoSaveAndLoad(size,EColor16MAP,EFalse);
			DoSaveAndLoad(size,EColor16MAP,ETrue);
			}
		}

	INFO_PRINTF1(_L("\r\n"));
	
	}


void CTBitmap::DoSaveAndLoad(const TSize& aSize,TDisplayMode aDispMode,TBool aBlank)
	{
	CFbsBitmapEx bmp1;
	bmp1.Create(aSize,aDispMode);

	if (aBlank)
		BlankBitmap(bmp1);
	else
		FillBitmap(bmp1);

	TInt ret = bmp1.Save(KTempFilename);
	TEST2(ret, KErrNone);

	CFbsBitmapEx bmp2;
	ret = bmp2.Load(KTempFilename);
	TEST2(ret, KErrNone);

	CompareBitmaps(bmp1,bmp2,EColor16M);

	ret = bmp2.Compress();
	TEST2(ret, KErrNone);

	CompareBitmaps(bmp1,bmp2,EColor16M);

	iFs.Delete(KTempFilename);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0534

	@SYMTestCaseDesc
	Compresses bitmaps to ROM and RAM

	@SYMTestActions
	Tries to compress a bitmap in ROM and expects it to fail. 
	Compresses a bitmap in RAM and expect it to succeed. 
	Compression tested when executed in the background.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::Compress()
	{
	INFO_PRINTF1(_L("Test Compression"));

	CFbsBitmap bmp;
	RFbsSession* fbs = RFbsSession::GetSession();
	TRequestStatus rs;

	if(!iSkipRomBitmapTests)
		{
		INFO_PRINTF1(_L("check compressing of ROM files causes error"));

		TInt ret=bmp.Load(KRomBitmap,ETfbs,NULL);
		TEST2(ret, KErrNone);

		ret = bmp.Compress();
		TEST2(ret, KErrAccessDenied); //cannot compress bitmaps in ROM

		ret = bmp.CompressInBackground();
		TEST2(ret, KErrAccessDenied); //cannot compress bitmaps in ROM

		bmp.CompressInBackground(rs);
		User::WaitForRequest(rs);
		TEST2(rs.Int(), KErrAccessDenied); //cannot compress bitmaps in ROM

		ret = bmp.Save(KTempFilename);
		TEST2(ret, KErrNone);
		bmp.Reset();

		ret = bmp.Load(KTempFilename);
		TEST2(ret, KErrNone);

		ret = bmp.Compress();
		TEST2(ret, KErrNone);
		TEST(bmp.IsCompressedInRAM());
		bmp.Reset();


		ret = bmp.Load(KTempFilename);
		TEST2(ret, KErrNone);

		ret = bmp.CompressInBackground();
		TEST2(ret, KErrNone);

		fbs->SendCommand(EFbsCompress, 0); // force background compression to execute

		TEST(bmp.IsCompressedInRAM());
		bmp.Reset();


		ret = bmp.Load(KTempFilename);
		TEST2(ret, KErrNone);

		bmp.CompressInBackground(rs);
		User::WaitForRequest(rs); // wait until background compression is finished
		TEST2(rs.Int(), KErrNone);

		TEST(bmp.IsCompressedInRAM());
		bmp.Reset();


		iFs.Delete(KTempFilename);

		}

	INFO_PRINTF1(_L("check compressing of RAM files is OK"));
	
		{
		TInt ret=bmp.Load(KTestBitmapOnC,ETfbs,NULL);
		if(ret != KErrNone)
			{
			INFO_PRINTF1(_L("Could not load "));
			INFO_PRINTF1(KTestBitmapOnC);
			INFO_PRINTF1(_L(" Ensure is it copied over and rerun test\n"));
			User::Panic(_L(""), ret);
			}
		
		ret = bmp.Compress();
		TEST2(ret, KErrNone);

		TEST(bmp.IsCompressedInRAM());
			
		ret = bmp.Save(KTempFilename);
		TEST2(ret, KErrNone);
		bmp.Reset();


		ret = bmp.Load(KTempFilename);
		TEST2(ret, KErrNone);

		ret = bmp.Compress();
		TEST2(ret, KErrNone);

		TEST(bmp.IsCompressedInRAM());
		bmp.Reset();


		ret = bmp.Load(KTempFilename);
		TEST2(ret, KErrNone);

		ret = bmp.CompressInBackground();
		TEST2(ret, KErrNone);

		fbs->SendCommand(EFbsCompress, 0); // force background compression to execute

		TEST(bmp.IsCompressedInRAM());
		bmp.Reset();


		ret = bmp.Load(KTempFilename);
		TEST2(ret, KErrNone);

		bmp.CompressInBackground(rs);
		fbs->SendCommand(EFbsCompress, 0); // force background compression to execute
		User::WaitForRequest(rs);
		TEST2(rs.Int(), KErrNone);

		TEST(bmp.IsCompressedInRAM());
		bmp.Reset();

		iFs.Delete(KTempFilename);
		}

		
		INFO_PRINTF1(_L("check compressing a created bitmap"));
		{
		INFO_PRINTF1(_L("1\r\n"));
		CFbsBitmapEx bitmap;
		TInt ret=bitmap.Create(TSize(200,200),EColor256);
		TEST2(ret, KErrNone);
		
		INFO_PRINTF1(_L("2\r\n"));
		BlankBitmap(bitmap);
		
		ret = bitmap.Compress();
		TEST2(ret, KErrNone);
		
		INFO_PRINTF1(_L("3\r\n"));
		
		TEST(bitmap.IsCompressedInRAM());
		}
	
	}

void CTBitmap::BlankBitmap(CFbsBitmapEx& aBitmap)
	{
	TSize size = aBitmap.SizeInPixels();
	TInt dataLength = CFbsBitmap::ScanLineLength(size.iWidth,aBitmap.DisplayMode()) * size.iHeight;
	aBitmap.LockHeap();
	Mem::FillZ((TUint8*)aBitmap.DataAddress(),dataLength);
	aBitmap.UnlockHeap();
	}

void CTBitmap::FillBitmap(CFbsBitmapEx& aBitmap)
	{
	TSize size = aBitmap.SizeInPixels();
	TInt dataLength = CFbsBitmap::ScanLineLength(size.iWidth,aBitmap.DisplayMode()) * size.iHeight;

	aBitmap.LockHeap();
	TUint8* bmpBits = (TUint8*)aBitmap.DataAddress();
	aBitmap.UnlockHeap();
	TUint8* bmpBitsLimit = bmpBits + dataLength;
	TInt64 seed = aBitmap.Handle();

	switch (aBitmap.DisplayMode())
		{
		case EColor4K:
			Mem::FillZ(bmpBits,dataLength);

			while (bmpBits < bmpBitsLimit)
				{
				*bmpBits++ = (TUint8)Math::Rand(seed);
				*bmpBits++ = (TUint8)(Math::Rand(seed) & 0x0f);
				}	
			break;
		case EColor16MAP:
			while (bmpBits < bmpBitsLimit)
				{
				TUint32 alpha = (TUint8)Math::Rand(seed);
				TUint32 red = 	(TUint8)Math::Rand(seed);
				TUint32 green = (TUint8)Math::Rand(seed);
				TUint32 blue =  (TUint8)Math::Rand(seed);
				red = ((red * alpha)/255)&0xff;
				green  = ((green * alpha)/255)&0xff;
				blue = ((blue * alpha)/255)&0xff;
				*(TUint32*)bmpBits = (alpha<<24)|(red<<16)|(green<<8)|blue;
				bmpBits+=4;
				}
			break;
		default:
			while (bmpBits < bmpBitsLimit)
				*bmpBits++ = (TUint8)Math::Rand(seed);
			break;	
		}	
	}

void CTBitmap::StripeBitmap(CFbsBitmapEx& aBitmap) // Compression friendly bitmap filling
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

void CTBitmap::CompareBitmaps(CFbsBitmapEx& aBmp1,CFbsBitmapEx& aBmp2,TDisplayMode aDispMode)
	{
	// We will compare bitmaps that we draw beginning at four different offsets into the bitmap
	const TInt KTestBitmapOrigins = 4;
	const TSize bmpSize = aBmp1.SizeInPixels();
	TEST(bmpSize == aBmp2.SizeInPixels());
	TInt originIndex = 0;
	
	// Call DoCompareBitmaps() for four different origins
	for(TInt ii=0;ii<KTestBitmapOrigins;ii++)
		{
		TInt xCoord = originIndex*(bmpSize.iWidth/4);
		TInt yCoord = originIndex*(bmpSize.iHeight/4);
		DoCompareBitmaps(aBmp1,aBmp2,aDispMode,TPoint(xCoord, yCoord), bmpSize);
		originIndex++;
		}
	}
	
void CTBitmap::DoCompareBitmaps(CFbsBitmapEx& aBmp1,CFbsBitmapEx& aBmp2,TDisplayMode aDispMode, TPoint aLineOrigin, TSize aBmpSize)
	{
	// Compares two bitmaps line by line
	const TInt KBitmapWidth = aBmpSize.iWidth;
	const TInt KStartRow = aLineOrigin.iY;
	const TInt KBitmapHeight = aBmpSize.iHeight;
	
	HBufC8* buf1 = HBufC8::NewMax(KBitmapWidth * 4);
	HBufC8* buf2 = HBufC8::NewMax(KBitmapWidth * 4);
	TEST(buf1 && buf2);

	TPtr8 des1(buf1->Des());
	TPtr8 des2(buf2->Des());
	
	for (TInt row = KStartRow; row < KBitmapHeight; row++) 
		{
		aBmp1.GetScanLine(des1,aLineOrigin,KBitmapWidth,aDispMode); 
		aBmp2.GetScanLine(des2,aLineOrigin,KBitmapWidth,aDispMode);

		const TInt ret = des1.Compare(des2);
		if (ret!=0)
			{
			 INFO_PRINTF2(_L("Bitmap comparison failed, row %d differernt!\n"), row);
			 aBmp1.Save(KFail1Filename);
			 aBmp2.Save(KFail2Filename);
			}
		TEST(ret == 0);
		}

	delete buf1;
	delete buf2;
	}

void CTBitmap::CopyBitmap(CFbsBitmapEx& aBmp1,const CFbsBitmapEx& aBmp2)
	{
	TSize bmp1size = aBmp1.SizeInPixels();
	TSize bmp2size = aBmp2.SizeInPixels();
	TInt bmp1dataLength = CFbsBitmap::ScanLineLength(bmp1size.iWidth,aBmp1.DisplayMode()) * bmp1size.iHeight;
	TInt bmp2dataLength = CFbsBitmap::ScanLineLength(bmp2size.iWidth,aBmp2.DisplayMode()) * bmp2size.iHeight;
	TEST(bmp1dataLength == bmp2dataLength);
	aBmp1.LockHeap();
	TUint32* bmp1Bits = aBmp1.DataAddress();
	aBmp1.UnlockHeap();
	aBmp2.LockHeap();
	TUint32* bmp2Bits = aBmp2.DataAddress();
	aBmp2.UnlockHeap();
	Mem::Copy((TUint8*)bmp1Bits,(TUint8*)bmp2Bits,bmp1dataLength);
	}


/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0535

	@SYMTestCaseDesc
	Loads and copies bitmaps and does a
	fuzzy compare on the bitmaps using 
	scan lines

	@SYMTestActions

	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::Color256BitmapUtil()
	{
	INFO_PRINTF1(_L("Test CFbsColor256BitmapUtil"));

	CFbsBitmapEx bmpRefDitherFs;
	TInt ret=bmpRefDitherFs.Load(_L("z:\\system\\data\\8bitDitherFS.mbm"));
	TEST2(ret, KErrNone);

	CFbsBitmapEx bmpRefNoDither;
	ret=bmpRefNoDither.Load(_L("z:\\system\\data\\8bitNoDither.mbm"));
	TEST2(ret, KErrNone);

	TSize size = bmpRefNoDither.SizeInPixels();

	CFbsColor256BitmapUtil* util = NULL;
	TRAP(ret, util=CFbsColor256BitmapUtil::NewL(NULL));
	TEST2(ret, KErrNone);

	CFbsBitmapEx bmp2;
	ret = bmp2.Create(size,EColor256);
	TEST2(ret, KErrNone);

	CFbsBitmapEx bmp1;

	INFO_PRINTF1(_L("EColor256 "));

	ret = util->CopyBitmap(&bmp2,&bmpRefNoDither,CFbsColor256BitmapUtil::ENoDither);
	TEST2(ret, KErrNone);
	CompareBitmaps(bmpRefNoDither,bmp2,EColor16M);
	ret = util->CopyBitmap(&bmp2,&bmpRefDitherFs,CFbsColor256BitmapUtil::EFloydSteinberg);
	TEST2(ret, KErrNone);
	CompareBitmaps(bmpRefDitherFs,bmp2,EColor16M);

	INFO_PRINTF1(_L("EColor4K "));

	ret=bmp1.Load(_L("z:\\system\\data\\12bit.mbm"));
	TEST2(ret, KErrNone);
	ret = util->CopyBitmap(&bmp2,&bmp1,CFbsColor256BitmapUtil::ENoDither);
	TEST2(ret, KErrNone);
	CompareBitmaps(bmpRefNoDither,bmp2,EColor16M);
	ret = util->CopyBitmap(&bmp2,&bmp1,CFbsColor256BitmapUtil::EFloydSteinberg);
	TEST2(ret, KErrNone);
	FuzzyCompareBitmapsColor256(bmpRefNoDither,bmp2,23);

	INFO_PRINTF1(_L("EColor64K "));

	ret=bmp1.Load(_L("z:\\system\\data\\16bit.mbm"));
	TEST2(ret, KErrNone);
	ret = util->CopyBitmap(&bmp2,&bmp1,CFbsColor256BitmapUtil::ENoDither);
	TEST2(ret, KErrNone);
	CompareBitmaps(bmpRefNoDither,bmp2,EColor16M);
	ret = util->CopyBitmap(&bmp2,&bmp1,CFbsColor256BitmapUtil::EFloydSteinberg);
	TEST2(ret, KErrNone);
	FuzzyCompareBitmapsColor256(bmpRefNoDither,bmp2,28);

	INFO_PRINTF1(_L("EColor16M\r\n"));

	ret=bmp1.Load(_L("z:\\system\\data\\24bit.mbm"));
	TEST2(ret, KErrNone);
	ret = util->CopyBitmap(&bmp2,&bmp1,CFbsColor256BitmapUtil::ENoDither);
	TEST2(ret, KErrNone);
	CompareBitmaps(bmpRefNoDither,bmp2,EColor16M);
	ret = util->CopyBitmap(&bmp2,&bmp1,CFbsColor256BitmapUtil::EFloydSteinberg);
	TEST2(ret, KErrNone);
	FuzzyCompareBitmapsColor256(bmpRefNoDither,bmp2,32);

	INFO_PRINTF1(_L("EColor16MU\r\n"));

	ret=bmp1.Load(_L("z:\\system\\data\\32bit.mbm"));
	TEST2(ret, KErrNone);
	ret = util->CopyBitmap(&bmp2,&bmp1,CFbsColor256BitmapUtil::ENoDither);
	TEST2(ret, KErrNone);
	CompareBitmaps(bmpRefNoDither,bmp2,EColor16M);
	ret = util->CopyBitmap(&bmp2,&bmp1,CFbsColor256BitmapUtil::EFloydSteinberg);
	TEST2(ret, KErrNone);
	FuzzyCompareBitmapsColor256(bmpRefNoDither,bmp2,32);
	
	CFbsBitmapEx bmp3;
	ret = bmp3.Create(size,EColor16MAP);
	TEST2(ret, KErrNone);
	CopyBitmap(bmp3,bmp1);
	TEST2(ret, KErrNone);
	ret = util->CopyBitmap(&bmp2,&bmp3,CFbsColor256BitmapUtil::ENoDither);
	TEST2(ret, KErrNone);
	//Do not compare bitmaps since there is no reference EColor16MAP image
	//CompareBitmaps(bmpRefNoDither,bmp2,EColor16M); - testing ensures that
	//copying succeeds, not accuracy
	ret = util->CopyBitmap(&bmp2,&bmp3,CFbsColor256BitmapUtil::EFloydSteinberg);
	TEST2(ret, KErrNone);
	//Do not compare bitmaps since there is no reference EColor16MAP image
	//FuzzyCompareBitmapsColor256(bmpRefNoDither,bmp2,32); - testing ensures that
	//copying succeeds, not accuracy
	bmp3.Reset();	

//	bmp2.Save(_L("c:\\8bitSaved.mbm"));

	bmp1.Reset();
	bmp2.Reset();
	bmpRefNoDither.Reset();
	bmpRefDitherFs.Reset();
	delete util;

	
	}

void CTBitmap::FuzzyCompareBitmapsColor256(CFbsBitmapEx& aBmp1,CFbsBitmapEx& aBmp2,TUint aDifLimit)
	{
	TSize bmp1size = aBmp1.SizeInPixels();
	TSize bmp2size = aBmp2.SizeInPixels();
	TInt bmp1dataLength = CFbsBitmap::ScanLineLength(bmp1size.iWidth,aBmp1.DisplayMode()) * bmp1size.iHeight;
	TInt bmp2dataLength = CFbsBitmap::ScanLineLength(bmp2size.iWidth,aBmp2.DisplayMode()) * bmp2size.iHeight;
	TEST(bmp1dataLength == bmp2dataLength);
	aBmp1.LockHeap();
	TUint8* bmp1Bits = (TUint8*)aBmp1.DataAddress();
	aBmp1.UnlockHeap();
	aBmp2.LockHeap();
	TUint8* bmp2Bits = (TUint8*)aBmp2.DataAddress();
	aBmp2.UnlockHeap();

	TUint totalDifference = 0;
	TUint difference = 0;
	while(bmp1dataLength--)
		{
		difference = TRgb::Color256((TUint)*(bmp1Bits++)).Difference(TRgb::Color256((TUint)*(bmp2Bits++)));
		totalDifference+=difference;
		}
	TUint avDif = totalDifference/bmp2dataLength;
	TEST(avDif<=aDifLimit);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0536

	@SYMTestCaseDesc
	Creates a hardware bitmap with a size and 
	display mode for different display modes. 

	@SYMTestActions
	It checks some bitmap attributes, bitmapHandle,
	if it is a large bitmap and if the bitmap is in ROM.
	Creates a 'normal' bitmap with the same size as the 
	hardware one. Checks it can write to the hardware 
	bitmap. Checks reuse of a CFbsBitmap for mixture of 
	hardware and normal bitmaps works.

	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::HardwareBitmaps()
	{
	static const TDisplayMode modes[] =
		{
		EGray2,
		EGray4,
		EGray16,
		EGray256,
		EColor16,
		EColor256,
		EColor64K,
		EColor16M,
		EColor16MU,
		EColor4K,
		EColor16MAP,
		ENone
		};

	INFO_PRINTF1(_L("Test Hardware Bitmaps"));

	CFbsBitmapEx bitmap1;
	// Large bitmap threshold increased from 4K to 64K
	TSize size(300,300);
	TDisplayMode displayMode;

	// Try and find a display mode that hardware bitmaps support
	TInt ret = KErrNotSupported;
	TInt i=0;
	while((displayMode=modes[i])!=ENone)
		{
		ret = bitmap1.CreateHardwareBitmap(size,displayMode,TUid::Uid(0));
		if(ret!=KErrNotSupported)
			{
			TEST2(ret, KErrNone);
			break;
			}
		i++;
		}

	if(ret==KErrNotSupported)
		{
		// Hardware bitmaps not supported on this device
		WARN_PRINTF1(_L("...Not Supported"));
		return;
		}

	// Check some bitmap attributes
	TEST(bitmap1.HardwareBitmapHandle()!=NULL);
	TEST(!bitmap1.IsLargeBitmap());
	TEST(!bitmap1.IsRomBitmap());

	// Create a 'normal' bitmap the same size as the hardware one
	CFbsBitmapEx bitmap2;
	ret = bitmap2.Create(size,displayMode);
	TEST2(ret, KErrNone);
	TEST(bitmap2.HardwareBitmapHandle()==NULL);
	TEST(bitmap2.IsLargeBitmap());

	// Check we can write to the hardware bitmap OK
	FillBitmap(bitmap1);
	CopyBitmap(bitmap1,bitmap2);
	CompareBitmaps(bitmap1,bitmap2,EColor16M);

	// Discard hardware bitmap
	bitmap1.Reset();

	// Check reuse of a CFbsBitmap for mixture of hardware and normal bitmaps works
	ret = bitmap2.CreateHardwareBitmap(size,displayMode,TUid::Uid(0));
	TEST2(ret, KErrNone);
	ret = bitmap2.Create(TSize(10,10),displayMode);
	TEST2(ret, KErrNone);
	ret = bitmap2.CreateHardwareBitmap(size,displayMode,TUid::Uid(0));
	TEST2(ret, KErrNone);
	ret = bitmap2.Create(size,displayMode);
	TEST2(ret, KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0537

	@SYMTestCaseDesc
	GetScanLine test
	
	@SYMDEF INC036062

	@SYMTestActions
	For each possible display mode it creates a test bitmap
	and initialize it with the data from the gray bitmap.
	Then for each possible display mode it calls GetScanLine 
	on the gray and	test bitmaps and compares the lines.
		
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::INC036062L()
//INC036062  -  Optimise masked bitblit for 1 bit greyscale mask case 
	{
	INFO_PRINTF1(_L("INC036062"));
	TDisplayMode displayMode[] = 
		{
		EGray2, EGray4, EGray16, EGray256, EColor16, EColor256, 
		EColor64K, EColor16M, EColor4K, EColor16MU, EColor16MAP
		};
	//Two EGray2 test bitmaps with sizes (11, 11) and (121, 121).
	TSize size[] = 
		{
		TSize(11, 11), TSize(121, 121)
		};
	//Masks for for the last scanline byte (probably partially initialized)
	TUint8 mask[sizeof(size)/sizeof(size[0])][sizeof(displayMode)/sizeof(displayMode[0])] = 
		{
		//EGray2 EGray4 EGray16 EGray256 EColor16 EColor256 EColor64K EColor16M EColor4K EColor16MU
		{0x07,   0x3F,  0x0F,   0xFF,    0x0F,    0xFF,     0xFF,     0xFF,     0x0F,    0xFF},
		{0x01,   0x03,  0x0F,   0xFF,    0x0F,    0xFF,     0xFF,     0xFF,     0x0F,    0xFF}
		};
	for(TInt i=0;i<TInt(sizeof(size)/sizeof(size[0]));i++)
		{
		//Create EGray2 bitmap
		CFbsBitmap* bmp = CreateBitmapLC(size[i], EGray2);
		//GetScanLine test - for each possible display mode: create a test bitmap
		//and initialize it with the data from the gray bitmap.
		//After that: for each possible display mode - call GetScanLine on the gray and
		//test bitmaps and compare the lines.
		for(TInt m=0;m<TInt(sizeof(displayMode)/sizeof(displayMode[0]));m++)
			{
			HBufC8* row1 = HBufC8::NewLC(1000);
			TPtr8 prow1 = row1->Des();
			HBufC8* row2 = HBufC8::NewLC(1000);
			TPtr8 prow2 = row2->Des();
			//Create test bitmap
			CFbsBitmap* tbmp = CreateBitmapLC(size[i], displayMode[m], bmp);
			//Check test bitmap
			for(TInt m2=0;m2<TInt(sizeof(displayMode)/sizeof(displayMode[0]));m2++)
				{
				for(TInt y=0;y<size[i].iHeight;y++)
					{
					bmp->GetScanLine(prow1, TPoint(0, y), size[i].iWidth, displayMode[m2]);
					tbmp->GetScanLine(prow2, TPoint(0, y), size[i].iWidth, displayMode[m2]);
					TInt len = prow1.Length();
					TEST(len == prow2.Length());
					TEST(len > 0);
					TUint8 b1 = prow1[len - 1];
					TUint8 b2 = prow2[len - 1];
					if(mask[i][m2] != 0xFF)
						{//Not all bits from the last byte are used.
						TEST((b1 & mask[i][m2]) == (b2 & mask[i][m2]));
						prow1.SetLength(len - 1);
						prow2.SetLength(len - 1);
						}
					TEST(prow1 == prow2);
					}//end of - for(TInt y=0;y<size[i].iHeight;y++)
				}//end of - for(TInt m2=0;m2<(sizeof(displayMode)/sizeof(displayMode[0]));m2++)
			CleanupStack::PopAndDestroy(tbmp);
			CleanupStack::PopAndDestroy(row2);
			CleanupStack::PopAndDestroy(row1);
			}//end of - for(TInt m=0;m<(sizeof(displayMode)/sizeof(displayMode[0]));m++)
		CleanupStack::PopAndDestroy(bmp);
		}//end of - for(TInt i=0;i<(sizeof(size)/sizeof(size[0]));i++)
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0538

	@SYMTestCaseDesc
	Performance test of INC036062 (GetScanLine test)

	@SYMDEF INC036062

	@SYMTestActions
		
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::PerformanceTest_INC036062L()
//INC036062  -  Optimise masked bitblit for 1 bit greyscale mask case 
	{
	INFO_PRINTF1(_L("INC036062-Performance Test"));
	TSize size(143, 143);
	//Create EGray2 bitmap
	CFbsBitmap* bmpGray2 = CreateBitmapLC(size, EGray2);
	//Create EColor64 bitmap
	CFbsBitmap* bmpEColor64 = CreateBitmapLC(size, EColor64K);
	//Performance test
	TDisplayMode displayMode[] = 
		{
		EGray2, EGray4, EGray16, EGray256, EColor16, EColor256, 
		EColor64K, EColor16M, EColor4K, EColor16MU, EColor16MAP
		};
	TInt i;
	HBufC8* row = HBufC8::NewLC(1000);
	TPtr8 prow = row->Des();
	for(TInt m=0;m<TInt(sizeof(displayMode)/sizeof(displayMode[0]));m++)
		{
		TUint time[2] = {0, 0};
		CFbsBitmap* pbmp[2] = {bmpGray2, bmpEColor64};
		for(TInt j=0;j<2;j++)
			{
			time[j] = User::TickCount();
			for(i=0;i<200;i++)
				{
				for(TInt y=0;y<size.iHeight;y++)
					{
					pbmp[j]->GetScanLine(prow, TPoint(0, y), size.iWidth, displayMode[m]);
					}
				}
			time[j] = User::TickCount() - time[j];
			}
		//"Time+" - time with optimized GetScanLine for EGray2 bitmaps
		//"Time-" - time with GetScanLine for all bitmaps which colour is not EGray2.
		INFO_PRINTF4(_L("GetScanLine. Mode=%d, Time+ =%d, Time- =%d\r\n"), displayMode[m], time[0], time[1]);
		}
	CleanupStack::PopAndDestroy(row);
	CleanupStack::PopAndDestroy(bmpEColor64);
	CleanupStack::PopAndDestroy(bmpGray2);
	}

CFbsBitmap* CTBitmap::CreateBitmapLC(const TSize& aSize, TDisplayMode aMode)
	{
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	User::LeaveIfError(bmp->Create(aSize, aMode));
	//Initialize the bitmap
	TBitmapUtil bmpUtil(bmp);
	bmpUtil.Begin(TPoint(0, 0));
	for(TInt y=0;y<aSize.iHeight;bmpUtil.IncYPos(),y++)
		{
		for(TInt x=0;x<aSize.iWidth;bmpUtil.IncXPos(),x++)
			{
			TUint32 color = 0;
			switch(aMode)
				{
				case EGray2:
					color = Math::Random() % 2;
					break;
				case EColor64K:
					color = Math::Random() % 0x10000;
					break;
				case EColor16MA:
					color = Math::Random();
					break;
				default:
					break;
				}
			bmpUtil.SetPixel(color);
			}
		}
	bmpUtil.End();
	return bmp;
	}

CFbsBitmap* CTBitmap::CreateBitmapLC(const TSize& aSize, TDisplayMode aMode, const CFbsBitmap* aSrcBmp)
	{
	CFbsBitmap* tbmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(tbmp);
	User::LeaveIfError(tbmp->Create(aSize, aMode));
	//Initialize test bitmap
	HBufC8* row = HBufC8::NewLC(1000);
	TPtr8 prow = row->Des();
	for(TInt y=0;y<aSize.iHeight;y++)
		{
		aSrcBmp->GetScanLine(prow, TPoint(0, y), aSize.iWidth, aMode);
		tbmp->SetScanLine(prow, y);
		}
	CleanupStack::PopAndDestroy(row);
	return tbmp;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0539

	@SYMTestCaseDesc
	Loads bitmaps from ROM and RAM
	and does a GetScanLine on the bitmaps
	and measures the time it takes to do
	the GetScanLine.
	
	@SYMDEF INC037474
	
	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::INC037474L()
//INC037474 - SS66 - CBitwiseBitmap::GenerateLineFromCompressedSixteenBitData() is very slow
	{
	TPtrC bmpFile[] =
		{
		KBmpCompr12(),
		KBmpCompr16(),
		KBmpCompr24(),
		KBmpCCompr12(),
		KBmpCCompr16(),
		KBmpCCompr24()
		};
	TPtrC tt[] = {_L("rom"), _L("rom"), _L("rom"), _L("ram"), _L("ram"), _L("ram")};
	TBool inRam[] = {EFalse, EFalse, EFalse, ETrue, ETrue, ETrue};
	TInt bpp[] = {12, 16, 24, 12, 16, 24};
	TDisplayMode targetMode[] = {EColor4K, EColor64K, EColor16M, EColor4K, EColor64K, EColor16M};
	const TInt KMaxScanLineLen = 800;
	TBuf8<KMaxScanLineLen> scanLineBuf;
	for(TInt i=0;i<TInt(sizeof(bmpFile)/sizeof(bmpFile[0]));i++)
		{
		if(iSkipRomBitmapTests && !inRam[i])
			continue;
		
		CFbsBitmap* bmp = LoadBitmapLC(bmpFile[i]);
		if(inRam[i])
			{
			User::LeaveIfError(bmp->Compress());
			}
		//
		TDesC& des = tt[i];
		INFO_PRINTF3(_L("%d bpp %S compressed bitmap, GetScanLine test\r\n"), bpp[i], &des);
		TSize size = bmp->SizeInPixels();
		TEST(TInt(size.iWidth * sizeof(TInt16)) <= KMaxScanLineLen);
		TUint timeStart = User::TickCount();
		enum {KTestTimes = 15};//Greater KTestTimes values make the test is too slow!
		for(TInt j=0;j<KTestTimes;j++) 
			{
			TPoint startPixel(0, 0);
			for(TInt k=0;k<size.iHeight;k++)
				{
				startPixel.iY = k;
				bmp->GetScanLine(scanLineBuf, startPixel, size.iWidth, targetMode[i]);
				}
			}
		TUint timeEnd = User::TickCount();
		INFO_PRINTF4(_L("%d bpp %S compressed bitmap, time = %d\r\n"), bpp[i], &des, timeEnd - timeStart);
		//
		CleanupStack::PopAndDestroy(bmp);
		}
	}

CFbsBitmap* CTBitmap::LoadBitmapLC(const TDesC& aFileName)
	{
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	const TInt err = bmp->Load(aFileName, 0);
	TEST(err == KErrNone);
	User::LeaveIfError(err);
	return bmp;
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0540

	@SYMTestCaseDesc
	Test to verify defect DEF038286.
	
	@SYMDEF DEF038286

	@SYMTestActions
	Initialized a test bitmap, compresses it
	and performs a GetScanLine on the compressed
	bitmap. Checks the color value against an
	expected value.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::DEF038286L()
//DEF038286 - CBitwiseBitmap::GenerateLineFromCompressedSixteenBitData returns wrong clr value	
	{
	INFO_PRINTF1(_L("DEF038286 fix test"));
	//Create test bitmap
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	const TInt width = 16;
	const TSize size(width, 1);
	User::LeaveIfError(bmp->Create(size, EColor64K));
	//Test bitmap data
	TUint8 colorVal[width * 2] = 
	//	 0          1          2          3          4          5          6          7
		{0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
	//	 8          9          10         11         12         13         14         15
	     0x00,0x00, 0x00,0x00, 0x00,0x00, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF};
	//Initialize test bitmap
	TPtr8 ptr(colorVal, width * 2, width * 2);
	bmp->SetScanLine(ptr, 0);
	//Compress test bitmap
	User::LeaveIfError(bmp->Compress());
	//Check the test data
	TUint8 colorVal2[width * 2];
	TPtr8 ptr2(colorVal2, width * 2, width * 2);
	//test 1
	colorVal2[0] = colorVal2[1] = colorVal2[2] = colorVal2[3] = 0x12;
	bmp->GetScanLine(ptr2, TPoint(10, 0), 2, EColor64K);
	TEST(colorVal2[0] == colorVal[10 * 2] && colorVal2[1] == colorVal[10 * 2 + 1] &&
		  colorVal2[2] == colorVal[11 * 2] && colorVal2[3] == colorVal[11 * 2 + 1]);
	//test 2
	colorVal2[0] = colorVal2[1] = colorVal2[2] = colorVal2[3] = 0x12;
	bmp->GetScanLine(ptr2, TPoint(10, 0), 1, EColor64K);
	TEST(colorVal2[0] == colorVal[10 * 2] && colorVal2[1] == colorVal[10 * 2 + 1]);
	//test 3
	colorVal2[0] = colorVal2[1] = colorVal2[2] = colorVal2[3] = 0x12;
	bmp->GetScanLine(ptr2, TPoint(11, 0), 1, EColor64K);
	TEST(colorVal2[0] == colorVal[11 * 2] && colorVal2[1] == colorVal[11 * 2 + 1]);

	//Destroy test bitmap and data
	CleanupStack::PopAndDestroy(bmp);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0541

	@SYMTestCaseDesc
	Test to verify defect DEF038286.
	
	@SYMDEF DEF038286

	@SYMTestActions
	Initialized a test bitmap, compresses it
	and performs a GetScanLine on the compressed
	bitmap. Checks the colour value against an
	expected value.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::DEF038286_2L()
//DEF038286 - CBitwiseBitmap::GenerateLineFromCompressedSixteenBitData returns wrong clr value
//24 bpp bitmap test
	{
	INFO_PRINTF1(_L("DEF038286 fix test. 24 bpp bitmap."));
	//Create test bitmap
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	const TInt width = 16;
	const TSize size(width, 1);
	User::LeaveIfError(bmp->Create(size, EColor16M));
	//Test bitmap data
	TUint8 colorVal[width * 3] = 
	//	 0               1               2               3               4               5               6			     7
		{0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
	//	 8               9               10              11              12              13              14              15
	     0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF};
	//Initialize test bitmap
	TPtr8 ptr(colorVal, width * 3, width * 3);
	bmp->SetScanLine(ptr, 0);
	//Compress test bitmap
	User::LeaveIfError(bmp->Compress());
	//Check the test data
	TUint8 colorVal2[width * 3];
	TPtr8 ptr2(colorVal2, width * 3, width * 3);
	//test 1
	colorVal2[0] = colorVal2[1] = colorVal2[2] = colorVal2[3] = colorVal2[4] = colorVal2[5] = 0x12;
	bmp->GetScanLine(ptr2, TPoint(10, 0), 2, EColor16M);
	TEST(colorVal2[0] == colorVal[10 * 3] && colorVal2[1] == colorVal[10 * 3 + 1] && colorVal2[2] == colorVal[10 * 3 + 2] &&
		  colorVal2[3] == colorVal[11 * 3] && colorVal2[4] == colorVal[11 * 3 + 1] && colorVal2[5] == colorVal[11 * 3 + 2]);
	//test 2
	colorVal2[0] = colorVal2[1] = colorVal2[2] = colorVal2[3] = colorVal2[4] = colorVal2[5] = 0x12;
	bmp->GetScanLine(ptr2, TPoint(10, 0), 1, EColor16M);
	TEST(colorVal2[0] == colorVal[10 * 3] && colorVal2[1] == colorVal[10 * 3 + 1] && colorVal2[2] == colorVal[10 * 3 + 2]);
	//test 3
	colorVal2[0] = colorVal2[1] = colorVal2[2] = colorVal2[3] = colorVal2[4] = colorVal2[5] = 0x12;
	bmp->GetScanLine(ptr2, TPoint(11, 0), 1, EColor16M);
	TEST(colorVal2[0] == colorVal[11 * 3] && colorVal2[1] == colorVal[11 * 3 + 1] && colorVal2[2] == colorVal[11 * 3 + 2]);

	//Destroy test bitmap and data
	CleanupStack::PopAndDestroy(bmp);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0542

	@SYMTestCaseDesc
	Tests the function CFbsBitmap::IsMonochrome()

	@SYMTestActions
	Loads uncompressed bitmaps and checks
	them if they are monochrome
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::MonohromeL()
	{
	INFO_PRINTF1(_L("Monohrome bitmaps"));
	TPtrC bitmap_ram[] = 
		{
		KMono_M1(),
		KMono_M2(),
		KMono_M4(),
		KMono_M8(),
		KMono_C4(),
		KMono_C8(),
		KMono_C12(),
		KMono_C16(),
		KMono_C24(),
		KMono_C32()
		};
	TInt i;
	//Check monohrome bitmaps
	//1 bpp bitmap is always monochrome
	for(i=0;i<TInt(sizeof(bitmap_ram)/sizeof(bitmap_ram[0]));i++)
		{
		CFbsBitmap* uncompressedBitmap = NULL;
		LoadAndNotCompressBitmapLC(uncompressedBitmap, bitmap_ram[i]);
		TEST(!uncompressedBitmap->IsCompressedInRAM());
		TBool monochrome = uncompressedBitmap->IsMonochrome();
		TEST(monochrome);
		CleanupStack::PopAndDestroy(uncompressedBitmap);
		}
	//Modify monohrome bitmaps and check them
	//1 bpp bitmap is always monochrome
	for(i=0;i<TInt(sizeof(bitmap_ram)/sizeof(bitmap_ram[0]));i++)
		{
		CFbsBitmap* uncompressedBitmap = NULL;
		LoadAndNotCompressBitmapLC(uncompressedBitmap, bitmap_ram[i]);
		TEST(!uncompressedBitmap->IsCompressedInRAM());
		//Change something into the bitmap data
		uncompressedBitmap->LockHeap(ETrue);
		TUint32* data = uncompressedBitmap->DataAddress();
		*data = 0x12345678;
		uncompressedBitmap->UnlockHeap(ETrue);
		//Check the bitmap
		TBool monochrome = uncompressedBitmap->IsMonochrome();
		TEST(i > 0 ? !monochrome : monochrome);
		CleanupStack::PopAndDestroy(uncompressedBitmap);
		}
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0543

	@SYMTestCaseDesc
	Tests the function CFbsBitmap::GetVerticalScanLine

	@SYMTestActions
	Gets the bitmap's vertical scanline starting at 
	a specified x co-ordinate.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::GetVerticalScanLineL()
	{
	INFO_PRINTF1(_L("GetVerticalScanLine()"));
	TDisplayMode displayMode[] = 
		{
		EGray2,
		EGray4,
		EGray16,
		EGray256,
		EColor16,
		EColor256,
		EColor64K,
		EColor16M,
		ERgb,
		EColor4K,
		EColor16MU,
		EColor16MAP,
		};
	CFbsBitmap* uncompressedBitmap = NULL;
	LoadAndNotCompressBitmapLC(uncompressedBitmap, KRamBitmap2);
	TEST(!uncompressedBitmap->IsCompressedInRAM());
	TSize sizeInPixels = uncompressedBitmap->SizeInPixels();
	HBufC8* vertScanLine = HBufC8::NewLC(sizeInPixels.iHeight * 4);
	TPtr8 p = vertScanLine->Des();
	TDes8& des = p;
	for(TInt i=0;i<TInt(sizeof(displayMode)/sizeof(displayMode[0]));i++)
		{
		TBuf<80> buf;
		buf.Format(_L("--mode=%d\r\n"), displayMode[i]);
		INFO_PRINTF1(buf);
		for(TInt j=0;j<sizeInPixels.iWidth;j++)
			{
			uncompressedBitmap->GetVerticalScanLine(des, j, displayMode[i]);
			}
		}
	CleanupStack::PopAndDestroy(vertScanLine);
	CleanupStack::PopAndDestroy(uncompressedBitmap);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0544
	
	@SYMTestCaseDesc
	Tests the performance of the function 
	CFbsBitmap::GetVerticalScanLine

	@SYMTestActions
	Gets the bitmap's vertical scanline starting at 
	a specified x co-ordinate and measures the time
	it takes.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::GetVerticalScanLinePerformanceL()
	{
	INFO_PRINTF1(_L("GetVerticalScanLine() performance"));
	TDisplayMode displayMode[] = 
		{
		EGray2,
		EGray4,
		EGray16,
		EGray256,
		EColor16,
		EColor256,
		EColor64K,
		EColor16M,
		ERgb,
		EColor4K,
		EColor16MU,
		EColor16MAP
		};
	CFbsBitmap* uncompressedBitmap = NULL;
	LoadAndNotCompressBitmapLC(uncompressedBitmap, KRamBitmap2);
	TEST(!uncompressedBitmap->IsCompressedInRAM());
	TSize sizeInPixels = uncompressedBitmap->SizeInPixels();
	HBufC8* vertScanLine = HBufC8::NewLC(sizeInPixels.iHeight * 4);
	TPtr8 p = vertScanLine->Des();
	TDes8& des = p;
	TUint time;
	for(TInt i=0;i<TInt(sizeof(displayMode)/sizeof(displayMode[0]));i++)
		{
		time = User::TickCount();
		for(TInt j=0;j<sizeInPixels.iWidth;j++)
			{
			uncompressedBitmap->GetVerticalScanLine(des, j, displayMode[i]);
			}
		INFO_PRINTF3(_L("Vert. line. Mode=%d, Time=%d\r\n"), displayMode[i], User::TickCount() - time);
		}
	CleanupStack::PopAndDestroy(vertScanLine);
	CleanupStack::PopAndDestroy(uncompressedBitmap);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0545

	@SYMTestCaseDesc
	Test to verify defect DEF071843
	
	@SYMDEF DEF071843
	
	@SYMTestActions
	Creates a test bitmap. Initializes it 
	with test data using SetScanLine. Saves 
	bitmap and then reloads it and compares 
	it against expected bitmap.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::DEF071843_16MAL()
	{
	INFO_PRINTF1(_L("DEF071843 fix test. Saving and loading 16MA 32bpp bitmap."));
	// Create test bitmap
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	const TInt width = 12;
	const TSize size(width, 8);
	User::LeaveIfError(bmp->Create(size, EColor16MA));

	//Test bitmap data - 8 rows of 12 pixels each containing various run lengths
	TUint8 colorVal0[width * 4] =  // runs: 12
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};
	TUint8 colorVal1[width * 4] =  // runs: 1 1 1 1 1 1 1 1 1 1 1 1
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  1,1,1,1,  2,2,2,2,  3,3,3,3,  4,4,4,4,  5,5,5,5,  6,6,6,6,  7,7,7,7,  8,8,8,8,  9,9,9,9,  10,10,10,10,  11,11,11,11};
	TUint8 colorVal2[width * 4] =  // runs: 1 1 2 1 1 3 1 1 1
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  1,1,1,1,  2,2,2,2,  2,2,2,2,  3,3,3,3,  4,4,4,4,  5,5,5,5,  5,5,5,5,  5,5,5,5,  6,6,6,6,  7,7,7,7,  8,8,8,8};
	TUint8 colorVal3[width * 4] =   // runs: 1 2 1 3 1 4
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  1,1,1,1,  1,1,1,1,  2,2,2,2,  3,3,3,3,  3,3,3,3,  3,3,3,3,  4,4,4,4,  5,5,5,5,  5,5,5,5,  5,5,5,5,  5,5,5,5};
	TUint8 colorVal4[width * 4] =   // runs: 2 1 2 2 2 3
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  0,0,0,0,  1,1,1,1,  2,2,2,2,  2,2,2,2,  3,3,3,3,  3,3,3,3,  4,4,4,4,  4,4,4,4,  5,5,5,5,  5,5,5,5,  5,5,5,5};
	TUint8 colorVal5[width * 4] =   // runs: 3 1 3 2 3
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  0,0,0,0,  0,0,0,0,  1,1,1,1,  2,2,2,2,  2,2,2,2,  2,2,2,2,  3,3,3,3,  3,3,3,3,  4,4,4,4,  4,4,4,4,  4,4,4,4};
	TUint8 colorVal6[width * 4] =   // runs: 4 1 1 1 1 4
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  1,1,1,1,  2,2,2,2,  3,3,3,3,  4,4,4,4,  5,5,5,5,  5,5,5,5,  5,5,5,5,  5,5,5,5};
	TUint8 colorVal7[width * 4] =   // runs: 1 1 1 1 4 1 1 1 1
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  1,1,1,1,  2,2,2,2,  3,3,3,3,  4,4,4,4,  4,4,4,4,  4,4,4,4,  4,4,4,4,  5,5,5,5,  6,6,6,6,  7,7,7,7,  8,8,8,8};

	//Initialize test bitmap
	TPtr8 line0(colorVal0, width * 4, width * 4);
	bmp->SetScanLine(line0, 0);
	TPtr8 line1(colorVal1, width * 4, width * 4);
	bmp->SetScanLine(line1, 1);
	TPtr8 line2(colorVal2, width * 4, width * 4);
	bmp->SetScanLine(line2, 2);
	TPtr8 line3(colorVal3, width * 4, width * 4);
	bmp->SetScanLine(line3, 3);
	TPtr8 line4(colorVal4, width * 4, width * 4);
	bmp->SetScanLine(line4, 4);
	TPtr8 line5(colorVal5, width * 4, width * 4);
	bmp->SetScanLine(line5, 5);
	TPtr8 line6(colorVal6, width * 4, width * 4);
	bmp->SetScanLine(line6, 6);
	TPtr8 line7(colorVal7, width * 4, width * 4);
	bmp->SetScanLine(line7, 7);

	// Save bitmap then reload and compare
	_LIT(KTest16MABmpFile, "c:\\test16ma.mbm");
	bmp->Save(KTest16MABmpFile);	
	//
	CFbsBitmap* bmp2 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp2);
	TInt err = bmp2->Load(KTest16MABmpFile);
	TEST2(err, KErrNone);
	//
	CompareBitmaps((CFbsBitmapEx&)*bmp, (CFbsBitmapEx&)*bmp2,EColor16MA);
	
	// Destroy test bitmaps
	CleanupStack::PopAndDestroy(2, bmp); // bmp2
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0546

	@SYMTestCaseDesc
	Test to verify defect DEF071843
	
	@SYMDEF DEF071843
	
	@SYMTestActions
	Creates a test bitmap. Initializes it 
	with test data using SetScanLine. Saves 
	bitmap and then reloads it and compares 
	it against expected bitmap.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::DEF071843_16MUL()
	{
	INFO_PRINTF1(_L("DEF071843 fix test. Compression improvements for 16MU 32bpp bitmap."));
	// Create test bitmap
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	const TInt width = 12;
	const TSize size(width, 8);
	User::LeaveIfError(bmp->Create(size, EColor16MU));

	//Test bitmap data - 8 rows of 12 pixels each containing various run lengths
	//Format is b,g,r,a where a (alpha) is unused, hence 255.
	TUint8 colorVal0[width * 4] =  // runs: 12
	//	 0           1           2           3           4           5           6           7           8           9           10          11
		{0,0,0,255,  0,0,0,255,  0,0,0,255,  0,0,0,255,  0,0,0,255,  0,0,0,255,  0,0,0,255,  0,0,0,255,  0,0,0,255,  0,0,0,255,  0,0,0,255,  0,0,0,255};
	TUint8 colorVal1[width * 4] =  // runs: 1 1 1 1 1 1 1 1 1 1 1 1
	//	 0           1           2           3           4           5           6           7           8           9           10          11
		{0,0,0,255,  1,1,1,255,  2,2,2,255,  3,3,3,255,  4,4,4,255,  5,5,5,255,  6,6,6,255,  7,7,7,255,  8,8,8,255,  9,9,9,255,  10,10,10,255,  11,11,11,255};
	TUint8 colorVal2[width * 4] =  // runs: 1 1 2 1 1 3 1 1 1
	//	 0           1           2           3           4           5           6           7           8           9           10          11
		{0,0,0,255,  1,1,1,255,  2,2,2,255,  2,2,2,255,  3,3,3,255,  4,4,4,255,  5,5,5,255,  5,5,5,255,  5,5,5,255,  6,6,6,255,  7,7,7,255,  8,8,8,255};
	TUint8 colorVal3[width * 4] =   // runs: 1 2 1 3 1 4
	//	 0           1           2           3           4           5           6           7           8           9           10          11
		{0,0,0,255,  1,1,1,255,  1,1,1,255,  2,2,2,255,  3,3,3,255,  3,3,3,255,  3,3,3,255,  4,4,4,255,  5,5,5,255,  5,5,5,255,  5,5,5,255,  5,5,5,255};
	TUint8 colorVal4[width * 4] =   // runs: 2 1 2 2 2 3
	//	 0           1           2           3           4           5           6           7           8           9           10          11
		{0,0,0,255,  0,0,0,255,  1,1,1,255,  2,2,2,255,  2,2,2,255,  3,3,3,255,  3,3,3,255,  4,4,4,255,  4,4,4,255,  5,5,5,255,  5,5,5,255,  5,5,5,255};
	TUint8 colorVal5[width * 4] =   // runs: 3 1 3 2 3
	//	 0           1           2           3           4           5           6           7           8           9           10          11
		{0,0,0,255,  0,0,0,255,  0,0,0,255,  1,1,1,255,  2,2,2,255,  2,2,2,255,  2,2,2,255,  3,3,3,255,  3,3,3,255,  4,4,4,255,  4,4,4,255,  4,4,4,255};
	TUint8 colorVal6[width * 4] =   // runs: 4 1 1 1 1 4
	//	 0           1           2           3           4           5           6           7           8           9           10          11
		{0,0,0,255,  0,0,0,255,  0,0,0,255,  0,0,0,255,  1,1,1,255,  2,2,2,255,  3,3,3,255,  4,4,4,255,  5,5,5,255,  5,5,5,255,  5,5,5,255,  5,5,5,255};
	TUint8 colorVal7[width * 4] =   // runs: 1 1 1 1 4 1 1 1 1
	//	 0           1           2           3           4           5           6           7           8           9           10          11
		{0,0,0,255,  1,1,1,255,  2,2,2,255,  3,3,3,255,  4,4,4,255,  4,4,4,255,  4,4,4,255,  4,4,4,255,  5,5,5,255,  6,6,6,255,  7,7,7,255,  8,8,8,255};

	//Initialize test bitmap
	TPtr8 line0(colorVal0, width * 4, width * 4);
	bmp->SetScanLine(line0, 0);
	TPtr8 line1(colorVal1, width * 4, width * 4);
	bmp->SetScanLine(line1, 1);
	TPtr8 line2(colorVal2, width * 4, width * 4);
	bmp->SetScanLine(line2, 2);
	TPtr8 line3(colorVal3, width * 4, width * 4);
	bmp->SetScanLine(line3, 3);
	TPtr8 line4(colorVal4, width * 4, width * 4);
	bmp->SetScanLine(line4, 4);
	TPtr8 line5(colorVal5, width * 4, width * 4);
	bmp->SetScanLine(line5, 5);
	TPtr8 line6(colorVal6, width * 4, width * 4);
	bmp->SetScanLine(line6, 6);
	TPtr8 line7(colorVal7, width * 4, width * 4);
	bmp->SetScanLine(line7, 7);

	// Save bitmap then reload and compare
	_LIT(KTest16MUBmpFile, "c:\\test16mu.mbm");
	bmp->Save(KTest16MUBmpFile);	
	//
	CFbsBitmap* bmp2 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp2);
	TInt err = bmp2->Load(KTest16MUBmpFile);
	TEST2(err, KErrNone);
	//
	CompareBitmaps((CFbsBitmapEx&)*bmp, (CFbsBitmapEx&)*bmp2, EColor16MU);
	
	// Destroy test bitmaps
	CleanupStack::PopAndDestroy(2, bmp); // bmp2
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0547

	@SYMTestCaseDesc
	Test to verify defect DEF071843
	
	@SYMDEF DEF071843
	
	@SYMTestActions
	Creates a test bitmap. Initializes it 
	with test data using SetScanLine. Saves 
	bitmap and then reloads it and compares 
	it against expected bitmap.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::DEF071843_16ML()
	{
	INFO_PRINTF1(_L("DEF071843 fix test. Compression improvements for 16M 24bpp bitmap."));
	// Create test bitmap
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	const TInt width = 12;
	const TSize size(width, 8);
	User::LeaveIfError(bmp->Create(size, EColor16M));

	//Test bitmap data - 8 rows of 12 pixels each containing various run lengths
	TUint8 colorVal0[width * 3] =  // runs: 12
	//	 0       1       2       3       4       5       6       7       8       9       10      11
		{0,0,0,  0,0,0,  0,0,0,  0,0,0,  0,0,0,  0,0,0,  0,0,0,  0,0,0,  0,0,0,  0,0,0,  0,0,0,  0,0,0};
	TUint8 colorVal1[width * 3] =  // runs: 1 1 1 1 1 1 1 1 1 1 1 1
	//	 0       1       2       3       4       5       6       7       8       9       10      11
		{0,0,0,  1,1,1,  2,2,2,  3,3,3,  4,4,4,  5,5,5,  6,6,6,  7,7,7,  8,8,8,  9,9,9,  10,10,10,  11,11,11};
	TUint8 colorVal2[width * 3] =  // runs: 1 1 2 1 1 3 1 1 1
	//	 0       1       2       3       4       5       6       7       8       9       10      11
		{0,0,0,  1,1,1,  2,2,2,  2,2,2,  3,3,3,  4,4,4,  5,5,5,  5,5,5,  5,5,5,  6,6,6,  7,7,7,  8,8,8};
	TUint8 colorVal3[width * 3] =   // runs: 1 2 1 3 1 4
	//	 0       1       2       3       4       5       6       7       8       9       10      11
		{0,0,0,  1,1,1,  1,1,1,  2,2,2,  3,3,3,  3,3,3,  3,3,3,  4,4,4,  5,5,5,  5,5,5,  5,5,5,  5,5,5};
	TUint8 colorVal4[width * 3] =   // runs: 2 1 2 2 2 3
	//	 0       1       2       3       4       5       6       7       8       9       10      11
		{0,0,0,  0,0,0,  1,1,1,  2,2,2,  2,2,2,  3,3,3,  3,3,3,  4,4,4,  4,4,4,  5,5,5,  5,5,5,  5,5,5};
	TUint8 colorVal5[width * 3] =   // runs: 3 1 3 2 3
	//	 0       1       2       3       4       5       6       7       8       9       10      11
		{0,0,0,  0,0,0,  0,0,0,  1,1,1,  2,2,2,  2,2,2,  2,2,2,  3,3,3,  3,3,3,  4,4,4,  4,4,4,  4,4,4};
	TUint8 colorVal6[width * 3] =   // runs: 4 1 1 1 1 4
	//	 0       1       2       3       4       5       6       7       8       9       10      11
		{0,0,0,  0,0,0,  0,0,0,  0,0,0,  1,1,1,  2,2,2,  3,3,3,  4,4,4,  5,5,5,  5,5,5,  5,5,5,  5,5,5};
	TUint8 colorVal7[width * 3] =   // runs: 1 1 1 1 4 1 1 1 1
	//	 0       1       2       3       4       5       6       7       8       9       10      11
		{0,0,0,  1,1,1,  2,2,2,  3,3,3,  4,4,4,  4,4,4,  4,4,4,  4,4,4,  5,5,5,  6,6,6,  7,7,7,  8,8,8};

	//Initialize test bitmap
	TPtr8 line0(colorVal0, width * 3, width * 3);
	bmp->SetScanLine(line0, 0);
	TPtr8 line1(colorVal1, width * 3, width * 3);
	bmp->SetScanLine(line1, 1);
	TPtr8 line2(colorVal2, width * 3, width * 3);
	bmp->SetScanLine(line2, 2);
	TPtr8 line3(colorVal3, width * 3, width * 3);
	bmp->SetScanLine(line3, 3);
	TPtr8 line4(colorVal4, width * 3, width * 3);
	bmp->SetScanLine(line4, 4);
	TPtr8 line5(colorVal5, width * 3, width * 3);
	bmp->SetScanLine(line5, 5);
	TPtr8 line6(colorVal6, width * 3, width * 3);
	bmp->SetScanLine(line6, 6);
	TPtr8 line7(colorVal7, width * 3, width * 3);
	bmp->SetScanLine(line7, 7);

	// Save bitmap then reload and compare
	_LIT(KTest16MBmpFile, "c:\\test16m.mbm");
	bmp->Save(KTest16MBmpFile);	
	//
	CFbsBitmap* bmp2 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp2);
	TInt err = bmp2->Load(KTest16MBmpFile);
	TEST2(err, KErrNone);
	//
	CompareBitmaps((CFbsBitmapEx&)*bmp, (CFbsBitmapEx&)*bmp2, EColor16M);
	
	// Destroy test bitmaps
	CleanupStack::PopAndDestroy(2, bmp); // bmp2
	}
	
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0548

	@SYMTestCaseDesc
	Test to verify defect DEF074623 (Alpha bit)

	@SYMDEF DEF074623

	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::DEF074623_16MAL()
	{
	INFO_PRINTF1(_L("DEF074623 fix test For Alpha Bit."));
	// Create test bitmap
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	const TInt iBitmapWidth = 10;
	const TInt iBitmapHeight = 10;

	
	TInt rowIndex;
	TInt colIndex;
	TSize rectSize(iBitmapWidth,iBitmapHeight);
	TRect srcRect(0,0,iBitmapWidth,iBitmapHeight);
	User::LeaveIfError(bmp->Create(rectSize, EColor16MU));
	srcRect.SetSize(rectSize);
	TBitmapUtil bmpUtil(bmp);
	bmpUtil.Begin(TPoint(0,0));
	for (rowIndex=0;rowIndex<iBitmapHeight;++rowIndex)
		{
		TInt red=0;
		TInt blue=255;
		TInt green=0;
		bmpUtil.SetPos(TPoint(0,rowIndex));
		for (colIndex=0;colIndex<iBitmapWidth;++colIndex)
			{
			TRgb rgb(red,green,blue);
			bmpUtil.SetPixel(rgb.Color16MA());
			}
		bmpUtil.IncXPos();
		red-=10;
		blue+=10;
		green-=10;
		if(red <= 0)
			{
			red+=255;
			}
		if(blue >= 255)
			{
			blue-=255;
			}
		if(green <= 0)
			{
			green+=255;
			}
		}
	bmpUtil.End();
	
	bmp->SetDisplayMode(EColor16MA);
	
	for(TInt yy = 0; yy < iBitmapHeight; yy++)
		{
		
		for(TInt xx = 0; xx < iBitmapWidth; xx++)
			{
			TRgb bmpcol;
			bmp->GetPixel(bmpcol,TPoint(xx,yy));
			TEST(bmpcol.Alpha() == 255);
			}
		}
		
	CleanupStack::PopAndDestroy(bmp);	

	}
	
#ifdef _DEBUG
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0549

	@SYMTestCaseDesc
	Test to verify defect DEF095361
	
	@SYMDEF DEF095361
	
	@SYMTestActions
	Creates a test bitmap. Initializes it 
	with test data using SetScanLine. Compresses
	the bitmap. Saves bitmap and then reloads it 
	and compares it against expected bitmap.
	Simulates heap allocation failure of memory.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::DEF095361_16MAL()
	{
	INFO_PRINTF1(_L("DEF095361 fix test. Compressing, Saving and loading 16MA 32bpp bitmap."));
	// Create test bitmap
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	const TInt width = 12;
	const TSize size(width, 8);
	User::LeaveIfError(bmp->Create(size, EColor16MA));

	//Test bitmap data - 8 rows of 12 pixels each containing various run lengths
	TUint8 colorVal0[width * 4] =  // runs: 12
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};
	TUint8 colorVal1[width * 4] =  // runs: 1 1 1 1 1 1 1 1 1 1 1 1
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  1,1,1,1,  2,2,2,2,  3,3,3,3,  4,4,4,4,  5,5,5,5,  6,6,6,6,  7,7,7,7,  8,8,8,8,  9,9,9,9,  10,10,10,10,  11,11,11,11};
	TUint8 colorVal2[width * 4] =  // runs: 1 1 2 1 1 3 1 1 1
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  1,1,1,1,  2,2,2,2,  2,2,2,2,  3,3,3,3,  4,4,4,4,  5,5,5,5,  5,5,5,5,  5,5,5,5,  6,6,6,6,  7,7,7,7,  8,8,8,8};
	TUint8 colorVal3[width * 4] =   // runs: 1 2 1 3 1 4
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  1,1,1,1,  1,1,1,1,  2,2,2,2,  3,3,3,3,  3,3,3,3,  3,3,3,3,  4,4,4,4,  5,5,5,5,  5,5,5,5,  5,5,5,5,  5,5,5,5};
	TUint8 colorVal4[width * 4] =   // runs: 2 1 2 2 2 3
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  0,0,0,0,  1,1,1,1,  2,2,2,2,  2,2,2,2,  3,3,3,3,  3,3,3,3,  4,4,4,4,  4,4,4,4,  5,5,5,5,  5,5,5,5,  5,5,5,5};
	TUint8 colorVal5[width * 4] =   // runs: 3 1 3 2 3
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  0,0,0,0,  0,0,0,0,  1,1,1,1,  2,2,2,2,  2,2,2,2,  2,2,2,2,  3,3,3,3,  3,3,3,3,  4,4,4,4,  4,4,4,4,  4,4,4,4};
	TUint8 colorVal6[width * 4] =   // runs: 4 1 1 1 1 4
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,  1,1,1,1,  2,2,2,2,  3,3,3,3,  4,4,4,4,  5,5,5,5,  5,5,5,5,  5,5,5,5,  5,5,5,5};
	TUint8 colorVal7[width * 4] =   // runs: 1 1 1 1 4 1 1 1 1
	//	 0         1         2         3         4         5         6         7         8         9         10        11
		{0,0,0,0,  1,1,1,1,  2,2,2,2,  3,3,3,3,  4,4,4,4,  4,4,4,4,  4,4,4,4,  4,4,4,4,  5,5,5,5,  6,6,6,6,  7,7,7,7,  8,8,8,8};

	//Initialize test bitmap
	TPtr8 line0(colorVal0, width * 4, width * 4);
	bmp->SetScanLine(line0, 0);
	TPtr8 line1(colorVal1, width * 4, width * 4);
	bmp->SetScanLine(line1, 1);
	TPtr8 line2(colorVal2, width * 4, width * 4);
	bmp->SetScanLine(line2, 2);
	TPtr8 line3(colorVal3, width * 4, width * 4);
	bmp->SetScanLine(line3, 3);
	TPtr8 line4(colorVal4, width * 4, width * 4);
	bmp->SetScanLine(line4, 4);
	TPtr8 line5(colorVal5, width * 4, width * 4);
	bmp->SetScanLine(line5, 5);
	TPtr8 line6(colorVal6, width * 4, width * 4);
	bmp->SetScanLine(line6, 6);
	TPtr8 line7(colorVal7, width * 4, width * 4);
	bmp->SetScanLine(line7, 7);

	//Compress bitmap using RLE Compression
	bmp->Compress(ERLECompression);
 
	// Save bitmap then reload and compare
	_LIT(KTest16MABmpFile, "c:\\testDEF095361_16ma.mbm");
	bmp->Save(KTest16MABmpFile);	
	//
	CFbsBitmap* bmp2 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp2);
	
	RFbsSession* fbs = RFbsSession::GetSession();
	
	TInt count;
	for (count = 1; ; count++)
		{
		fbs->SendCommand(EFbsMessDefaultAllocFail,count);

		TInt ret = bmp2->Load(KTest16MABmpFile, 0, EFalse);

		if (ret == KErrNoMemory)
			{
				// Do Nothing
			}
		else if (ret == KErrNone)
			{
			break;
			}
		else 
			TEST2(ret, KErrNone);
		}

	fbs->SendCommand(EFbsMessDefaultAllocFail,0);
	
	//
	CompareBitmaps((CFbsBitmapEx&)*bmp, (CFbsBitmapEx&)*bmp2,EColor16MA);
	
	// Destroy test bitmaps
	CleanupStack::PopAndDestroy(2, bmp); // bmp2
	}
#endif
void CTBitmap::LoadAndCompressBitmapLC(CFbsBitmap*& aBmp, const TDesC& aBmpFile)
	{
	aBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(aBmp);
	TInt err = aBmp->Load(aBmpFile);
	TEST2(err, KErrNone);
	err = aBmp->Compress();
	TEST2(err, KErrNone);
	TEST(aBmp->IsCompressedInRAM());
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0550

	@SYMTestCaseDesc
	Compresses a bitmap containing two lines

	@SYMTestActions
	Creates a test bitmap with two lines. Compresses it (16 bpp)
	and checks if compressed successfully
	
	@SYMTestExpectedResults
	Test should pass
*/

void CTBitmap::TwoComprLines()
	{
	INFO_PRINTF1(_L("16 bpp compressed bitmap - 2 lines"));
	//Create test bitmap
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	const TInt width = 3;
	const TSize size(width, 2);
	User::LeaveIfError(bmp->Create(size, EColor64K));
	//Test bitmap data
	TUint8 colorVal[width * 2] = 
	//	 0          1          2        
		{0x13,0x13, 0x13,0x13, 0x13,0x13};
	//Initialize test bitmap
	TPtr8 ptr(colorVal, width * 2, width * 2);
	bmp->SetScanLine(ptr, 0);
	bmp->SetScanLine(ptr, 1);
	//Compress test bitmap
	User::LeaveIfError(bmp->Compress());
	//Check the test data
	TUint8 colorVal2[width * 2];
	TPtr8 ptr2(colorVal2, width * 2, width * 2);
	//test 1
	bmp->GetScanLine(ptr2, TPoint(0, 0), 3, EColor64K);
	TEST(ptr2 == ptr);
	bmp->GetScanLine(ptr2, TPoint(0, 1), 3, EColor64K);
	TEST(ptr2 == ptr);
	//Destroy test bitmap and data
	CleanupStack::PopAndDestroy(bmp);
	}

void CTBitmap::LoadAndNotCompressBitmapLC(CFbsBitmap*& aBmp, const TDesC& aBmpFile)
	{
	aBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(aBmp);
	TInt err = aBmp->Load(aBmpFile);
	TEST2(err, KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0551

	@SYMTestCaseDesc
	The test verifies the 
	CFbsBitmap::Compress((TBitmapfileCompressionScheme aScheme) API.
	It loads bitmaps from ROM & saves (temporarily) to C-drive. 
	Then it loads from C-drive into a new bitmap.

	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::TestPaletteCompressionL()
	{
	INFO_PRINTF1(_L("Test Palette Compression"));
	_LIT(KmbmFileInRom, "z:\\system\\data\\24bitskin.mbm");
	RFbsSession* fbs = RFbsSession::GetSession();
	//create bitmap
	CFbsBitmap* fbsCompressed = new(ELeave) CFbsBitmap;
	CFbsBitmap* fbsUnCompressed = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmFileInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force foreground compression)
	fbsCompressed->Compress(EPaletteCompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression == EGenericPaletteCompression);
	//compare with original
	INFO_PRINTF1(_L("Compare Palette Compressed bmp with original"));
	CompareBitmaps((CFbsBitmapEx&)*fbsCompressed, (CFbsBitmapEx&)*fbsUnCompressed, EColor16M);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);

	//Test that the RLE option works.
	//------------------------------------
	//create bitmap
	fbsCompressed = new(ELeave) CFbsBitmap ;
	fbsUnCompressed = new(ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmFileInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force foreground compression)
	fbsCompressed->Compress(ERLECompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	//test that RLE compression occurred
	TEST(fbsCompressed->Header().iCompression == ETwentyFourBitRLECompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);

	//Test that unsuitable bitmaps are not palette compressed (too many colours).
	//----------------------------------------------------------------------------------------
	_LIT(KmbmMultiColourMBMInRom, "z:\\system\\data\\24bit.mbm");
	//create bitmap
	fbsCompressed = new(ELeave) CFbsBitmap;
	fbsUnCompressed = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmMultiColourMBMInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force foreground compression)
	fbsCompressed->Compress(EPaletteCompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression != EGenericPaletteCompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);
	
	//Test that unsuitable bitmaps are not palette compressed (too few BPP)
	//---------------------------------------------------------------------------------
	//create bitmap
	fbsCompressed = new(ELeave) CFbsBitmap ;
	fbsUnCompressed = new(ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmMultiColourMBMInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force foreground compression)
	fbsCompressed->Compress(EPaletteCompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression != EGenericPaletteCompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);

	
	// Test that 12bitmap cannot pallete compress but will fall back to RLE
	// --------------------------------------------------------------------
	_LIT(Kmbm12BitMBMInRom, "z:\\system\\data\\file.mbm");
	fbsCompressed = new(ELeave) CFbsBitmap ;
	fbsUnCompressed = new(ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(Kmbm12BitMBMInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force foreground compression)
	fbsCompressed->Compress(EPaletteCompressionWithRLEFallback);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	TBitmapfileCompression comp = fbsCompressed->Header().iCompression;
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression != EGenericPaletteCompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);
	

	// Test that multi colour bitmap cannot pallete compress but will fall back to RLE
	// -------------------------------------------------------------------------------
	fbsCompressed = new(ELeave) CFbsBitmap ;
	fbsUnCompressed = new(ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmMultiColourMBMInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force foreground compression)
	fbsCompressed->Compress(EPaletteCompressionWithRLEFallback);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	comp = fbsCompressed->Header().iCompression;
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression != EGenericPaletteCompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);
	
	
	
	//Test various BPP & bitmap widths
	//----------------------------------------
	INFO_PRINTF1(_L("Test assorted BPP & colour counts"));
	_LIT(Kmbm16NarrowFileInRom, "z:\\system\\data\\16bitskin.mbm");
	TestAssortedBPPandColourDepthL(Kmbm16NarrowFileInRom, fbs);
	_LIT(Kmbm24bit6colFileInRom, "z:\\system\\data\\24bit6col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm24bit6colFileInRom, fbs);
	_LIT(Kmbm24bit2colFileInRom, "z:\\system\\data\\24bit2col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm24bit2colFileInRom, fbs);
	_LIT(Kmbm24bit3colFileInRom, "z:\\system\\data\\24bit3col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm24bit3colFileInRom, fbs);
	_LIT(Kmbm24bit20colFileInRom, "z:\\system\\data\\24bit20col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm24bit20colFileInRom, fbs);
	_LIT(Kmbm16bit6colFileInRom, "z:\\system\\data\\16bit6col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm16bit6colFileInRom, fbs);
	_LIT(Kmbm16bit2colFileInRom, "z:\\system\\data\\16bit2col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm16bit2colFileInRom, fbs);
	_LIT(Kmbm16bit3colFileInRom, "z:\\system\\data\\16bit3col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm16bit3colFileInRom, fbs);
	_LIT(Kmbm16bit20colFileInRom, "z:\\system\\data\\16bit20col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm16bit20colFileInRom, fbs);
	_LIT(Kmbm32BitSkinFileInRom, "z:\\system\\data\\32bitskin.mbm");
	TestAssortedBPPandColourDepthL(Kmbm32BitSkinFileInRom, fbs);
	_LIT(Kmbm32bit6colFileInRom, "z:\\system\\data\\32bit6col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm32bit6colFileInRom, fbs);
	_LIT(Kmbm32bit2colFileInRom, "z:\\system\\data\\32bit2col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm32bit2colFileInRom, fbs);
	_LIT(Kmbm32bit3colFileInRom, "z:\\system\\data\\32bit3col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm32bit3colFileInRom, fbs);
	_LIT(Kmbm32bit20colFileInRom, "z:\\system\\data\\32bit20col.mbm");
	TestAssortedBPPandColourDepthL(Kmbm32bit20colFileInRom, fbs);
	INFO_PRINTF1(_L("Test Palette Compression - COMPLETE"));
	}

void CTBitmap::TestAssortedBPPandColourDepthL(const TDesC& aFilename, RFbsSession* aFbs)
	{
	//create bitmap
	CFbsBitmap* fbsCompressed = new (ELeave) CFbsBitmap ;
	CFbsBitmap* fbsUnCompressed = new (ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(aFilename, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force foreground compression)
	fbsCompressed->Compress(EPaletteCompression);
	aFbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression == EGenericPaletteCompression);
	//compare with original
	INFO_PRINTF1(_L("Compare Palette Compressed bmp with original"));
	CompareBitmaps((CFbsBitmapEx&)*fbsCompressed, (CFbsBitmapEx&)*fbsUnCompressed, EColor16M);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);
	}
	
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0552

	@SYMTestCaseDesc
	The test verifies the 
	CFbsBitmap::CompressInBackground((TBitmapfileCompressionScheme aScheme)
	API. It loads bitmaps from ROM & saves (temporarily) to C-drive. 
	Then it loads from C-drive into a new bitmap.

	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::TestSynchronousBackgroundPaletteCompressionL()
	{
	INFO_PRINTF1(_L("Test Background Palette Compression"));
	_LIT(KmbmFileInRom, "z:\\system\\data\\24bitskin.mbm");
	RFbsSession* fbs = RFbsSession::GetSession();
	//create bitmap
	CFbsBitmap* fbsCompressed = new(ELeave) CFbsBitmap ;
	CFbsBitmap* fbsUnCompressed = new(ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmFileInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force immediate compression)
	fbsCompressed->CompressInBackground(EPaletteCompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression == EGenericPaletteCompression);
	//compare with original
	INFO_PRINTF1(_L("Compare Palette Compressed bmp with original"));
	CompareBitmaps((CFbsBitmapEx&)*fbsCompressed, (CFbsBitmapEx&)*fbsUnCompressed, EColor16M);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);
	
	//Test that the RLE option works.
	//------------------------------------
	//create bitmap
	fbsCompressed = new(ELeave) CFbsBitmap ;
	fbsUnCompressed = new(ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmFileInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force immediate compression)
	fbsCompressed->CompressInBackground(ERLECompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	//test that RLE compression occurred
	TEST(fbsCompressed->Header().iCompression == ETwentyFourBitRLECompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);

	//Test that unsuitable bitmaps are not palette compressed (too many colours).
	//----------------------------------------------------------------------------------------
	_LIT(KmbmMultiColourMBMInRom, "z:\\system\\data\\24bit.mbm");
	//create bitmap
	fbsCompressed = new(ELeave) CFbsBitmap ;
	fbsUnCompressed = new(ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmMultiColourMBMInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force immediate compression)
	fbsCompressed->CompressInBackground(EPaletteCompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression != EGenericPaletteCompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);
	
	//Test that unsuitable bitmaps are not palette compressed (too few BPP)
	//---------------------------------------------------------------------------------
	//create bitmap
	fbsCompressed = new(ELeave) CFbsBitmap ;
	fbsUnCompressed = new(ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmMultiColourMBMInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force immediate compression)
	fbsCompressed->CompressInBackground(EPaletteCompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression != EGenericPaletteCompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);
	INFO_PRINTF1(_L("Test Background Palette Compression - COMPLETE"));
	}
	
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0553

	@SYMTestCaseDesc
	The test verifies the 
	CFbsBitmap::CompressInBackground(TRequestStatus& aRequestStatus, TBitmapfileCompressionScheme aScheme)
	API. It loads bitmaps from ROM & saves (temporarily) to C-drive. 
	Then it loads from C-drive into a new bitmap.

	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/	
void CTBitmap::TestAsynchronousBackgroundPaletteCompressionL()
	{
	INFO_PRINTF1(_L("Test Asynch background Palette Compression"));
	TRequestStatus status ;
	_LIT(KmbmFileInRom, "z:\\system\\data\\24bitskin.mbm");
	RFbsSession* fbs = RFbsSession::GetSession();
	//create bitmap
	CFbsBitmap* fbsCompressed = new(ELeave) CFbsBitmap ;
	CFbsBitmap* fbsUnCompressed = new(ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmFileInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force immediate compression)
	fbsCompressed->CompressInBackground(status, EPaletteCompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrNone);
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression == EGenericPaletteCompression);
	//compare with original
	INFO_PRINTF1(_L("Compare Palette Compressed bmp with original"));
	CompareBitmaps((CFbsBitmapEx&)*fbsCompressed, (CFbsBitmapEx&)*fbsUnCompressed, EColor16M);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);
	
	//Test that the RLE option works.
	//------------------------------------
	//create bitmap
	fbsCompressed = new (ELeave) CFbsBitmap ;
	fbsUnCompressed = new (ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmFileInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force immediate compression)
	fbsCompressed->CompressInBackground(status, ERLECompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrNone);
	//test that RLE compression occurred
	TEST(fbsCompressed->Header().iCompression == ETwentyFourBitRLECompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);

	//Test that unsuitable bitmaps are not palette compressed (too many colours).
	//----------------------------------------------------------------------------------------
	_LIT(KmbmMultiColourMBMInRom, "z:\\system\\data\\24bit.mbm");
	//create bitmap
	fbsCompressed = new (ELeave) CFbsBitmap ;
	fbsUnCompressed = new (ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmMultiColourMBMInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force immediate compression)
	fbsCompressed->CompressInBackground(status, EPaletteCompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrNotSupported);
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression != EGenericPaletteCompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);
	
	//Test that unsuitable bitmaps are not palette compressed (too few BPP)
	//---------------------------------------------------------------------------------
	//create bitmap
	fbsCompressed = new(ELeave) CFbsBitmap ;
	fbsUnCompressed = new(ELeave) CFbsBitmap ;
	CleanupStack::PushL(fbsCompressed);
	CleanupStack::PushL(fbsUnCompressed);
	TEST2(fbsUnCompressed->Load(KmbmMultiColourMBMInRom, 0, EFalse), KErrNone);
	fbsUnCompressed->Save(KTempFilename);
	//load bitmap
	TEST2(fbsCompressed->Load(KTempFilename, 0, EFalse), KErrNone);
	//compress bitmap (force immediate compression)
	fbsCompressed->CompressInBackground(status, EPaletteCompression);
	fbs->SendCommand(EFbsCompress, 0); //force background compression to execute
	User::WaitForRequest(status);
	TEST2(status.Int(), KErrNotSupported);
	//test that palette compression occurred
	TEST(fbsCompressed->Header().iCompression != EGenericPaletteCompression);
	CleanupStack::PopAndDestroy(2);
	iFs.Delete(KTempFilename);
	INFO_PRINTF1(_L("Test Asynch background Palette Compressio - COMPLETE"));
	}

/**
@SYMTestCaseID GRAPHICS-FBSERV-0501

@SYMTestType UT

@SYMTestPriority Medium

@SYMCR DKRN-6QGJPT

@SYMTestCaseDesc Get all bitmap handles before and after loading a bitmap. 

@SYMTestActions
1) Get all bitmap handles and store number of bitmaps obtained
2) Load a new bitmap
3) Get all bitmap handles and store number of bitmaps obtained

@SYMTestExpectedResults
The number of bitmap handles retrieved after loading the bitmap should be one more than prior to loading the bitmap.
*/
void CTBitmap::GetAllBitmapsIdsL()
	{
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	
	RArray <TInt> bitmapHandleArray;
	CleanupClosePushL(bitmapHandleArray);
	
	// Get current number of bitmap handles
	User::LeaveIfError(bitmap->GetAllBitmapHandles(bitmapHandleArray));
	const TInt numBitmapHandles = bitmapHandleArray.Count();
	
	// Load another bitmap
	CFbsBitmap* bmp = LoadBitmapLC(KRc_Ram8);
	
	// Get current number of bitmap handles
	User::LeaveIfError(bitmap->GetAllBitmapHandles(bitmapHandleArray));
	const TInt newNumBitmapHandles = bitmapHandleArray.Count();
	TEST(numBitmapHandles == newNumBitmapHandles - 1);	// Ensure there is one more bitmap handle from the bitmap we just added
	
	const TInt handle = bmp->Handle();
	
	TInt foundHandle = 0;
	for (TInt count = newNumBitmapHandles-1; count>=0; --count)
		{
		if (handle == bitmapHandleArray[count])
			{
			foundHandle++;
			}
		}
	TEST(foundHandle==1);	// Ensure only one instance of this handle is found in the array
	CleanupStack::PopAndDestroy(3, bitmap);	// bmp, bitmapHandleArray, bitmap	
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0554

	@SYMTestCaseDesc
	Test Code For Corrupted MBM File.
	
	@SYMTestActions
	Loads a specific bitmap from a multi-bitmap 
	file from ROM and tests if it is corrupted.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTBitmap::CheckBadBmp()
{
	INFO_PRINTF1(_L("MalformedBmp"));
	_LIT(KmalmbmFile0, "z:\\system\\data\\00_grbmap.mbm");
	_LIT(KmalmbmFile1, "z:\\system\\data\\01_totalstramsize!.mbm");
	_LIT(KmalmbmFile2, "z:\\system\\data\\02_headersize.mbm");
	_LIT(KmalmbmFile3, "z:\\system\\data\\03_width!.mbm");
	_LIT(KmalmbmFile4, "z:\\system\\data\\04_height!.mbm");
	_LIT(KmalmbmFile5, "z:\\system\\data\\05_bitsperpixel.mbm");
	_LIT(KmalmbmFile6, "z:\\system\\data\\bad_bmpsize.mbm");
	_LIT(KmalmbmFile7, "z:\\system\\data\\bad_colormode.mbm");
	_LIT(KmalmbmFile8, "z:\\system\\data\\bad_compression.mbm");
	_LIT(KmalmbmFile9, "z:\\system\\data\\bad_paletteentries.mbm"); 

	CFbsBitmap* fbsmalBmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsmalBmp);

	//load bitmap
	TEST2(fbsmalBmp->Load(KmalmbmFile0, 0, EFalse), KErrNone);
	TEST2(fbsmalBmp->Load(KmalmbmFile1, 0, EFalse), KErrCorrupt);
	TEST2(fbsmalBmp->Load(KmalmbmFile2, 0, EFalse), KErrCorrupt);
	TEST2(fbsmalBmp->Load(KmalmbmFile3, 0, EFalse), KErrCorrupt);
	TEST2(fbsmalBmp->Load(KmalmbmFile4, 0, EFalse), KErrCorrupt);
	TEST2(fbsmalBmp->Load(KmalmbmFile5, 0, EFalse), KErrNotSupported);
	TEST2(fbsmalBmp->Load(KmalmbmFile6, 0, EFalse), KErrCorrupt);
	TEST2(fbsmalBmp->Load(KmalmbmFile7, 0, EFalse), KErrCorrupt);
	TEST2(fbsmalBmp->Load(KmalmbmFile8, 0, EFalse), KErrCorrupt);
	TEST2(fbsmalBmp->Load(KmalmbmFile9, 0, EFalse), KErrNotSupported);

	CleanupStack::PopAndDestroy(fbsmalBmp);
}

/**
@SYMTestCaseID			GRAPHICS-FBSERV-0613

@SYMDEF					PDEF111181

@SYMTestCaseDesc		Test the return code of operations on bitmaps in the background compression queue

@SYMTestPriority		Critical

@SYMTestType			UT

@SYMTestStatus			Implemented

@SYMTestActions			1. Creates a bitmap
						2. Calls CompressInBackground()
						3. Call Resize
						4. Waits until compression is finished

@SYMTestExpectedResults The call to resize should return with KErrInUse since the compression is in progress
*/

void CTBitmap::TestBgCompressInUseL()
	{
	INFO_PRINTF1(_L("Test the return code of operations on bitmaps in the background compression queue"));
	
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);

	TInt ret = bmp->Create(TSize(256, 256), EGray256); // create a solid grey bitmap
	TEST2 (ret,KErrNone);
	TRequestStatus status;
	bmp->CompressInBackground(status);
	TInt resizeRet = bmp->Resize(TSize(512,512));
	
	TEST2 (resizeRet,KErrInUse);
	User::WaitForRequest(status);
	TEST2 (status.Int(), KErrNone);
	CleanupStack::PopAndDestroy(bmp);
	}


//------------------------------------------------------------------------------

LOCAL_D const TInt KTestBitmapWidth = 256; // this affects BitmapCheckCompressed()
LOCAL_D const TInt KTestBitmapHeight = 256;
LOCAL_D const TInt KTestBitmapResizeHeight = 512;
LOCAL_D const TInt KMaxRLERunLength = 127;

LOCAL_C void SetBitmapDataWithVerticalGradient(TUint8* aDataPtr)
	{
	for (TInt y = 0; y < KTestBitmapHeight; ++y)
		for (TInt x = 0; x < KTestBitmapWidth; ++x)
			*aDataPtr++ = y;
	}

LOCAL_C TBool BitmapDataCheckVerticalGradient(const TUint8* aDataPtr)
	{
	for (TInt y = 0; y < KTestBitmapHeight; ++y)
		for (TInt x = 0; x < KTestBitmapWidth; ++x)
			if (*aDataPtr++ != y)
				return EFalse;
	return ETrue;
	}

LOCAL_C void SetBitmapDataWithHorizontalGradient(TUint8* aDataPtr)
	{
	for (TInt y = 0; y < KTestBitmapHeight; ++y)
		for (TInt x = 0; x < KTestBitmapWidth; ++x)
			*aDataPtr++ = x;
	}

LOCAL_C TBool BitmapDataCheckHorizontalGradient(const TUint8* aDataPtr)
	{
	for (TInt y = 0; y < KTestBitmapHeight; ++y)
		for (TInt x = 0; x < KTestBitmapWidth; ++x)
			if (*aDataPtr++ != x)
				return EFalse;
	return ETrue;
	}

LOCAL_C TInt ResizeTestBitmap(CFbsBitmap* aBitmap)
	{
	return aBitmap->Resize(TSize(KTestBitmapWidth, KTestBitmapResizeHeight));
	}

LOCAL_C TBool BitmapCheckResized(const CFbsBitmap* aBitmap)
	{
	if (aBitmap->SizeInPixels() != TSize(KTestBitmapWidth, KTestBitmapResizeHeight))
		return EFalse;
	TUint8* dataPtr = (TUint8*)aBitmap->DataAddress();
	for (TInt y = 0; y < KTestBitmapHeight; ++y)
		for (TInt x = 0; x < KTestBitmapWidth; ++x)
			if (*dataPtr++ != y)
				return EFalse;
	for (TInt y = KTestBitmapHeight; y < KTestBitmapResizeHeight; ++y)
		for (TInt x = 0; x < KTestBitmapWidth; ++x)
			if (*dataPtr++ != 0xFF)
				return EFalse;
	return ETrue;
	}

LOCAL_C TInt CompressTestBitmap(CFbsBitmap* aBitmap)
	{
	return aBitmap->Compress();
	}

LOCAL_C TBool BitmapCheckCompressed(const CFbsBitmap* aBitmap)
	{
	if (aBitmap->SizeInPixels() != TSize(KTestBitmapWidth, KTestBitmapHeight))
		return EFalse;
	// check RLE compression on vertical gradient
	TUint8* dataPtr = (TUint8*)aBitmap->DataAddress();
	for (TInt y = 0; y < KTestBitmapHeight - 1; ++y)
		{
		// this works when the width of the bitmap is 256, or twice the maximum encodable run length, which is 128
		if (*dataPtr++ != KMaxRLERunLength)
			return EFalse;
		if (*dataPtr++ != y)
			return EFalse;
		if (*dataPtr++ != KMaxRLERunLength)
			return EFalse;
		if (*dataPtr++ != y)
			return EFalse;
		}
	return ETrue;
	}

struct TTestThreadParams
	{
	TInt iBitmapHandle;
	TInt (*iBitmapFunction)(CFbsBitmap*); // operation to perform on the bitmap
	TBool (*iCheckFunction)(const CFbsBitmap*); // check that the operation was successfully performed
	};

LOCAL_C void AlternateTestThreadL(const TTestThreadParams& aParams)
	{
	RFbsSession* fbs = RFbsSession::GetSession();
	if (fbs->ResourceCount() != 0)
		User::Leave(KErrGeneral);
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	User::LeaveIfError(bmp->Duplicate(aParams.iBitmapHandle));
	if (fbs->ResourceCount() != 1)
		User::Leave(KErrGeneral);
	if (!BitmapDataCheckVerticalGradient((TUint8*)bmp->DataAddress()))
		User::Leave(KErrGeneral);
	User::LeaveIfError((*aParams.iBitmapFunction)(bmp));
	if (fbs->ResourceCount() != 1)
		User::Leave(KErrGeneral);
	if (aParams.iCheckFunction != NULL && !(*aParams.iCheckFunction)(bmp))
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(bmp);
	if (fbs->ResourceCount() != 0)
		User::Leave(KErrGeneral);
	}

LOCAL_C TInt AlternateTestThread(TAny* aParams)
	{
	__UHEAP_MARK;
	TInt ret = RFbsSession::Connect();
	if (ret != KErrNone)
		return ret;
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (cleanupStack == NULL)
		return KErrNoMemory;
	TRAP(ret, AlternateTestThreadL(*static_cast<TTestThreadParams*>(aParams)));
	delete cleanupStack;
	RFbsSession::Disconnect();
	__UHEAP_MARKEND;
	return ret;
	}

void CTBitmap::MultiThreadedTestsL()
	{
	((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0503"));
	MultiThreadedResizeTestL();
	((CTBitmapStep*)iStep)->RecordTestResultL();
	((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0504"));
	MultiThreadedCompressTestL();
#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
	((CTBitmapStep*)iStep)->RecordTestResultL();
	((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0505"));
	MultiThreadedLockHeapTestL();
#endif
	}

/**
@SYMTestCaseID			GRAPHICS-FBSERV-0503

@SYMDEF					DEF090812

@SYMTestCaseDesc		Tests behaviour of CFbsBitmap::DataAddress() and CFbsBitmap::Resize() when called concurrently by different threads

@SYMTestPriority		High

@SYMTestType			UT

@SYMTestStatus			Implemented

@SYMTestActions			1. The 1st thread will create a bitmap, store a pointer to the bitmap data, launch the 2nd thread and then wait for it to terminate
						2. The 2nd thread will resize the same bitmap and exit
						3. The 1st thread will manipulate the bitmap data using the stored pointer and then re-fetch the bitmap data

@SYMTestExpectedResults	The 1st thread should be able both to manipulate the old bitmap data and check the resized bitmap data
*/
void CTBitmap::MultiThreadedResizeTestL()
	{
	INFO_PRINTF1(_L("Test Resize() with Multiple Threads"));
	DoMultiThreadedTestL(_L("AlternateResizeThread"), ResizeTestBitmap, BitmapCheckResized);
	}

/**
@SYMTestCaseID			GRAPHICS-FBSERV-0504

@SYMDEF					DEF090812

@SYMTestCaseDesc		Tests behaviour of CFbsBitmap::DataAddress() and CFbsBitmap::Compress() when called concurrently by different threads

@SYMTestPriority		High

@SYMTestType			UT

@SYMTestStatus			Implemented

@SYMTestActions			1. The 1st thread will create a bitmap, store a pointer to the bitmap data, launch the 2nd thread and then wait for it to terminate
						2. The 2nd thread will compress the same bitmap and exit
						3. The 1st thread will manipulate the bitmap data using the stored pointer and then re-fetch the bitmap data

@SYMTestExpectedResults	The 1st thread should be able both to manipulate the old bitmap data and check the compressed bitmap data
*/
void CTBitmap::MultiThreadedCompressTestL()
	{
	INFO_PRINTF1(_L("Test Compress() with Multiple Threads"));
	DoMultiThreadedTestL(_L("AlternateCompressThread"), CompressTestBitmap, BitmapCheckCompressed);
	}

void CTBitmap::DoMultiThreadedTestL(const TDesC& aThreadName, TInt (*aBitmapFunction)(CFbsBitmap*), TBool (*aCheckFunction)(const CFbsBitmap*))
	{
	CFbsBitmap* bmp = new CFbsBitmap;
	ASSERT_NOT_NULL(bmp);
	CleanupStack::PushL(bmp);
	TInt ret = bmp->Create(TSize(KTestBitmapWidth, KTestBitmapHeight), EGray256);
	ASSERT_EQUALS(ret, KErrNone);
	TUint8* dataPtr = (TUint8*)bmp->DataAddress();
	SetBitmapDataWithVerticalGradient(dataPtr);
	TTestThreadParams params;
	params.iBitmapHandle = bmp->Handle();
	params.iBitmapFunction = aBitmapFunction;
	params.iCheckFunction = aCheckFunction;
	RThread thread;
	ret = thread.Create(aThreadName, AlternateTestThread, KDefaultStackSize, 0x2000, 0x2000, &params);
	ASSERT_EQUALS(ret, KErrNone);
	TRequestStatus status;
	thread.Logon(status);
	thread.Resume();
	User::WaitForRequest(status);
	TExitType exitType = thread.ExitType();
	TInt exitReason = thread.ExitReason();
	TEST(exitType == EExitKill);
	TEST(exitReason == KErrNone);
	thread.Close();
	TEST(BitmapDataCheckVerticalGradient(dataPtr));
	SetBitmapDataWithHorizontalGradient(dataPtr);
	TEST(BitmapDataCheckHorizontalGradient(dataPtr));
	TEST((*aCheckFunction)(bmp));
	CleanupStack::PopAndDestroy(bmp);
	User::Heap().Check();
	}

#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
_LIT(KFBSERVClientPanicCategory, "FBSCLI");

LOCAL_C TInt LockHeapTestBitmap(CFbsBitmap* aBitmap)
	{
	aBitmap->LockHeap();
	return KErrNone;
	}

/**
@SYMTestCaseID			GRAPHICS-FBSERV-0505

@SYMDEF					DEF090812

@SYMTestCaseDesc		Tests behaviour of CFbsBitmap::LockHeap() when called concurrently by different threads

@SYMTestPriority		High

@SYMTestType			UT

@SYMTestStatus			Implemented

@SYMTestActions			1. The 1st thread will create a bitmap, call LockHeap() on it, launch the 2nd thread and then wait for it to terminate
						2. The 2nd thread will also call LockHeap() on the same bitmap
						3. The 1st thread will check that the 2nd thread has panicked

@SYMTestExpectedResults	Panic FBSCLI 22
*/
void CTBitmap::MultiThreadedLockHeapTestL()
	{
	INFO_PRINTF1(_L("Test LockHeap() Panic with Multiple Threads"));
	CFbsBitmap* bmp = new CFbsBitmap;
	ASSERT_NOT_NULL(bmp);
	CleanupStack::PushL(bmp);
	TInt ret = bmp->Create(TSize(KTestBitmapWidth, KTestBitmapHeight), EGray256);
	ASSERT_EQUALS(ret, KErrNone);
	SetBitmapDataWithVerticalGradient((TUint8*)bmp->DataAddress());
	bmp->LockHeap();
	TTestThreadParams params;
	params.iBitmapHandle = bmp->Handle();
	params.iBitmapFunction = LockHeapTestBitmap;
	params.iCheckFunction = NULL;
	RThread thread;
	ret = thread.Create(_L("AlternateLockHeapThread"), AlternateTestThread, KDefaultStackSize, 0x2000, 0x2000, &params);
	ASSERT_EQUALS(ret, KErrNone);
	TRequestStatus status;
	thread.Logon(status);
	thread.Resume();
	User::WaitForRequest(status);
	TExitType exitType = thread.ExitType();
	TExitCategoryName exitCategory = thread.ExitCategory();
	TInt exitReason = thread.ExitReason();
	TEST(exitType == EExitPanic);
	TEST(exitCategory == KFBSERVClientPanicCategory);
	TEST(exitReason == EFbsPanicBadHeapLock);
	thread.Close();
	CleanupStack::PopAndDestroy(bmp);
	User::Heap().Check();
	}
#endif

//--------------

class CStopTestBgCompress: public CAsyncOneShot
	{
public:
	CStopTestBgCompress(): CAsyncOneShot(CActive::EPriorityIdle - 1) {} // priority must be less than CFbsSessionHelper
	void RunL() { CActiveScheduler::Stop(); }
	};

LOCAL_C TInt TestBgCompressThread(TAny* /*aParams*/)
	{
	const TInt KBmpSize = 0x100000 + sizeof(SEpocBitmapHeader);
	// assuming run-length encoding every scan line gets compressed into 8 runs of 128 bytes each
	// 2 bytes per encoded run results in 16 bytes per scan line so the compressed bitmap is large (>16K)
	// a few bytes added at the end for the compression bookmark are rounded up to a page
	const TInt KBmpCompressedSize = 0x5000;
	TInt ret = KErrNone;
	__UHEAP_MARK;
	CTrapCleanup* trap = CTrapCleanup::New();
	TRequestStatus status;
	if (trap == NULL)
		{
		ret = KErrNoMemory;
		goto exitNoTrap;
		}
	CActiveScheduler* scheduler = new CActiveScheduler;
	if (scheduler == NULL)
		{
		ret = KErrNoMemory;
		goto exitNoScheduler;
		}
	CActiveScheduler::Install(scheduler);
	CStopTestBgCompress* stop = new CStopTestBgCompress;
	if (stop == NULL)
		{
		ret = KErrNoMemory;
		goto exitNoStop;
		}
	ret = RFbsSession::Connect();
	if (ret != KErrNone)
		goto exitNoFbs;
	CFbsBitmapEx* bmp = new CFbsBitmapEx;
	if (bmp == NULL)
		{
		ret = KErrNoMemory;
		goto exitNoBmp;
		}
	CFbsBitmapEx* bmp2 = new CFbsBitmapEx;
	if (bmp2 == NULL)
		{
		ret = KErrNoMemory;
		goto exitNoBmp2;
		}
	ret = bmp->Create(TSize(1024, 1024), EGray256); // create a solid white bitmap
	if (ret != KErrNone)
		goto exitAll;
	ret = bmp2->Duplicate(bmp->Handle()); // duplicate the bitmap handle
	if (ret != KErrNone)
		goto exitAll;
	bmp->CompressInBackground(status);
	User::WaitForRequest(status);
	if (status != KErrNone)
		{
		ret = status.Int();
		goto exitAll;
		}
	if (bmp->BitmapAddress()->Header().iBitmapSize != KBmpSize || bmp->BitmapAddress() != bmp2->BitmapAddress())
		{
		ret = KErrGeneral;
		goto exitAll;
		}
	stop->Call();
	CActiveScheduler::Start(); // now CFbsSessionHelper should clean both bmp and bmp2 and free the dirty bitmap data
	if (!bmp->BitmapAddress()->IsCompressedInRAM() || bmp->BitmapAddress()->Header().iBitmapSize > KBmpCompressedSize || bmp->BitmapAddress() != bmp2->BitmapAddress())
		ret = KErrGeneral;
exitAll:
	delete bmp2;
exitNoBmp2:
	delete bmp;
exitNoBmp:
	RFbsSession::Disconnect();
exitNoFbs:
	delete stop;
exitNoStop:
	delete scheduler;
exitNoScheduler:
	delete trap;
exitNoTrap:
	__UHEAP_MARKEND;
	return ret;
	}

/**
@SYMTestCaseID			GRAPHICS-FBSERV-0612

@SYMDEF					INC109293

@SYMTestCaseDesc		Tests memory usage with CFbsBitmap::CompressInBackground()

@SYMTestPriority		High

@SYMTestType			UT

@SYMTestStatus			Implemented

@SYMTestActions			1. Creates a bitmap
						2. Calls CompressInBackground()
						3. Waits until compression is finished
						4. Gives the active scheduler the chance to run the session's helper

@SYMTestExpectedResults	Efficient use of memory
*/
void CTBitmap::TestBgCompressL()
	{
	INFO_PRINTF1(_L("Test memory usage with CFbsBitmap::CompressInBackground()"));
	RThread thread;
	TInt ret = thread.Create(_L("TestBgCompressThread"), TestBgCompressThread, KDefaultStackSize, 0x2000, 0x2000, NULL);
	ASSERT_EQUALS(ret, KErrNone);
	TRequestStatus status;
	thread.Logon(status);
	thread.Resume();
	User::WaitForRequest(status);
	TExitType exitType = thread.ExitType();
	TInt exitReason = thread.ExitReason();
	TEST(exitType == EExitKill);
	TEST(exitReason == KErrNone);
	if (exitType != EExitKill || exitReason != KErrNone)
		{
		INFO_PRINTF3(_L("Exit Type = %d, Exit Reason = %d"), TInt(exitType), exitReason);
		}
	thread.Close();
	}

/**
@SYMTestCaseID			GRAPHICS-FBSERV-0618

@SYMDEF					DEF123068

@SYMTestCaseDesc		Tests that small and large bitmaps are white-filled upon creation.

@SYMTestPriority		High

@SYMTestType			UT

@SYMTestStatus			Implemented

@SYMTestActions			1. Create a small bitmap
						2. Create a large bitmap

@SYMTestExpectedResults	Both bitmaps are filled with white
*/
void CTBitmap::TestBitmapWhiteFillL()
	{
	INFO_PRINTF1(_L("Test white fill of CFbsBitmap upon creation."));
	const TDisplayMode KDispMode = EColor16MAP;
	RPointerArray<CFbsBitmap> bmpArray;
	const TSize KBitmapSmallSize(4,4);
	const TSize KBitmapLargeSize(200,200);
	
	// Create small CFbsBitmap
	CFbsBitmap* bmp = new (ELeave) CFbsBitmap;;
	TInt ret = bmp->Create(KBitmapSmallSize,KDispMode);
	TEST2(ret, KErrNone);
	bmpArray.AppendL(bmp);
	// Create large CFbsBitmap
	bmp = new (ELeave) CFbsBitmap;;
	ret = bmp->Create(KBitmapLargeSize,KDispMode);
	TEST2(ret, KErrNone);
	bmpArray.AppendL(bmp);
	// Create small hardware bitmap
	bmp = new (ELeave) CFbsBitmap;;
	ret = bmp->CreateHardwareBitmap(KBitmapSmallSize, KDispMode, KNullUid);
	if (ret == KErrNotSupported)
		{
		INFO_PRINTF1(_L("WhiteFill test skipped for small hardware bitmaps as they are not supported"));
		delete bmp;
		}
	else
		{
		TEST2(ret, KErrNone);
		bmpArray.AppendL(bmp);
		}
	// Create large hardware bitmap
	bmp = new (ELeave) CFbsBitmap;;
	ret = bmp->CreateHardwareBitmap(KBitmapLargeSize, KDispMode, KNullUid);
	if (ret == KErrNotSupported)
		{
		INFO_PRINTF1(_L("WhiteFill test skipped for large hardware bitmaps as they are not supported"));
		delete bmp;
		}
	else
		{
		TEST2(ret, KErrNone);
		bmpArray.AppendL(bmp);
		}
	
	for (TInt ii = 0; ii < bmpArray.Count(); ++ii)
		{
		bmp = bmpArray[ii];
		bmp->BeginDataAccess();
		TUint32* base = bmp->DataAddress();
		TSize size = bmp->SizeInPixels();
		TInt scanLineLength = CFbsBitmap::ScanLineLength(size.iWidth,KDispMode);
		TInt dataLength = scanLineLength * size.iHeight;
		CheckWhite((TUint8*)base,dataLength,KDispMode);
		bmp->EndDataAccess(ETrue);
		delete bmp;
		}
	bmpArray.Close();
	}

//--------------

// The code for regression test case GRAPHICS-FBSERV-0614 must be in a separate thread
// because it has to call RFbsSession::Disconnect() and without the fix it panics.
LOCAL_C TInt TestDisconnectWithBitmapThread(TAny* /*aParams*/)
	{
	CTrapCleanup* trap = CTrapCleanup::New();
	if (trap == NULL)
		return KErrNoMemory;
	CActiveScheduler* scheduler = new CActiveScheduler;
	if (scheduler == NULL)
		{
		delete trap;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);
	TInt ret = RFbsSession::Connect();
	if (ret != KErrNone)
		{
		delete scheduler;
		delete trap;
		return ret;
		}
	CFbsBitmap* bmp = new CFbsBitmap;
	if (bmp == NULL)
		{
		RFbsSession::Disconnect();
		delete scheduler;
		delete trap;
		return KErrNoMemory;
		}
	ret = bmp->Create(TSize(256, 256), EGray256); // create a solid white bitmap
	if (ret != KErrNone)
		{
		RFbsSession::Disconnect();
		delete scheduler;
		delete trap;
		return ret;
		}
	RFbsSession::Disconnect();
	delete scheduler;
	delete trap;
	return ret;
	}

/**
@SYMTestCaseID			GRAPHICS-FBSERV-0614

@SYMDEF					INC113118

@SYMTestCaseDesc		Tests behaviour when RFbsSession::Disconnect() is called and there is a CFbsBitmap not yet deleted

@SYMTestPriority		High

@SYMTestType			UT

@SYMTestStatus			Implemented

@SYMTestActions			1. Creates and installs an active scheduler so that CFbsSessionHelper can be activated
						2. Calls RFbsSession::Connect()
						3. Creates a bitmap
						4. Calls RFbsSession::Disconnect()

@SYMTestExpectedResults	The bitmap should be destroyed automatically and no panics should occur
*/
void CTBitmap::TestDisconnectWithBitmapL()
	{
	INFO_PRINTF1(_L("Test RFbsSession::Disconnect() with a CFbsBitmap not yet deleted"));
	RThread thread;
	TInt ret = thread.Create(_L("TestDisconnectWithBitmapThread"), TestDisconnectWithBitmapThread, KDefaultStackSize, 0x2000, 0x2000, NULL);
	ASSERT_EQUALS(ret, KErrNone);
	TRequestStatus status;
	thread.Logon(status);
	thread.Resume();
	User::WaitForRequest(status);
	TExitType exitType = thread.ExitType();
	TInt exitReason = thread.ExitReason();
	TEST(exitType == EExitKill);
	TEST(exitReason == KErrNone);
	thread.Close();
	}

/**
@SYMTestCaseID			GRAPHICS-FBSERV-0617

@SYMPREQ				PREQ39

@SYMTestCaseDesc		Test that bitmaps have been touched when data changes.
						Test that bitmaps become volatile when DataAddress is called outside of 
						calls to  BeginDataAccess()	and EndDataAccess().

@SYMTestStatus			Implemented

@SYMTestPriority		High

@SYMTestActions			Create a bitmap.
						Call each public API.
						Check that the touch count increases when the bitmap data has been modified.
						Check whether bitmap is volatile.
@SYMTestExpectedResults	Touch count only increases for the following functions:
	 						Resize,
	 						SetDisplayMode,
	 						SetScanLine,
	 						InternalizeL,
	 						SwapWidthAndHeight,
	 						DataAddress() called inside a Begin() and End(EFalse) pairing.
	 					Bitmap is volatile only when DataAddress() called outside
	 					a Begin() and End(EFalse) pairing.
*/
void CTBitmap::TestTouchedAndVolatileL()
	{
	INFO_PRINTF1(_L("Test bitmaps become touched or volatile when data changes."));
	CFbsBitmap* bmp = new CFbsBitmap;
	TEST(bmp != NULL);
	TInt touchCount = 0;
	TInt newTouchCount = 0;
	CleanupStack::PushL(bmp);
	
	// Resize
	User::LeaveIfError(bmp->Create(TSize(4,4),EColor16MA));
	touchCount = bmp->TouchCount();
	TInt ret = bmp->Resize(TSize(5,5));
	TEST(ret == KErrNone);
	newTouchCount = bmp->TouchCount();
	TEST(newTouchCount == touchCount+1);
	TEST(!bmp->IsVolatile());
	touchCount = bmp->TouchCount();
	ret = bmp->Resize(TSize(3,3));
	TEST(ret == KErrNone);
	newTouchCount = bmp->TouchCount();
	TEST(newTouchCount == touchCount+1);
	TEST(!bmp->IsVolatile());
	touchCount = bmp->TouchCount();
	ret = bmp->Resize(TSize(3,3));
	TEST(ret == KErrNone);
	newTouchCount = bmp->TouchCount();
	TEST(newTouchCount == touchCount+1);
	TEST(!bmp->IsVolatile());
	bmp->Reset();

	// SetDisplayMode
	User::LeaveIfError(bmp->Create(TSize(4,4),EColor16MA));
	touchCount = bmp->TouchCount();
	ret = bmp->SetDisplayMode(EColor16MAP);
	TEST(ret == KErrNone);
	newTouchCount = bmp->TouchCount();
	TEST(newTouchCount == touchCount+1);
	TEST(!bmp->IsVolatile());
	bmp->Reset();

	// SetScanLine
	TInt scanLineLength = CFbsBitmap::ScanLineLength(256,EColor16MA);
	TUint8* buffer = new TUint8[scanLineLength];
	TEST(buffer != NULL);
	TPtr8 scanline(buffer,scanLineLength,scanLineLength);
	User::LeaveIfError(bmp->Create(TSize(4,4),EColor16MA));
	touchCount = bmp->TouchCount();
	bmp->SetScanLine(scanline, 1);
	newTouchCount = bmp->TouchCount();
	TEST(newTouchCount == touchCount+1);
	TEST(!bmp->IsVolatile());
	delete[] buffer;
	bmp->Reset();

	// SwapWidthAndHeight
	User::LeaveIfError(bmp->Create(TSize(4,6),EColor16MA));
	touchCount = bmp->TouchCount();
	ret = bmp->SwapWidthAndHeight();
	TEST(ret == KErrNone);
	newTouchCount = bmp->TouchCount();
	TEST(newTouchCount == touchCount+1);
	TEST(!bmp->IsVolatile());
	bmp->Reset();

	// InternalizeL
	// Requires data to be externalised first
	User::LeaveIfError(bmp->Create(TSize(4,6),EColor16MA));
	touchCount = bmp->TouchCount();
	CDirectFileStore* writestore = CDirectFileStore::ReplaceL(iFs,KTempFilename,EFileStream|EFileWrite);
	TEST(writestore != NULL);
	TUidType uidtype(KDirectFileStoreLayoutUid,KMultiBitmapFileImageUid);
	writestore->SetTypeL(uidtype);
	RStoreWriteStream writestrm;
	TStreamId headerid(0);
	headerid = writestrm.CreateL(*writestore);
	writestore->SetRootL(headerid);
	bmp->ExternalizeL(writestrm);
	writestrm.Close();
	delete writestore;
	bmp->Reset();
	// Now internalise data to reset bitmap.
	CDirectFileStore* readstore = CDirectFileStore::OpenL(iFs,KTempFilename,EFileStream|EFileRead);
	RStoreReadStream readstrm;
	headerid = readstore->Root();
	readstrm.OpenL(*readstore,headerid);
	bmp->InternalizeL(readstrm);
	readstrm.Close();
	delete readstore;
	newTouchCount = bmp->TouchCount();
	TEST(newTouchCount == touchCount+1);
	TEST(!bmp->IsVolatile());
	bmp->Reset();
	
	
	// DataAddress
	// 1. Call as read-only (by passing ETrue into End())
	// Expect: touch count doesn't increase & bitmap not volatile
	User::LeaveIfError(bmp->Create(TSize(4,6),EColor16MA));
	touchCount = bmp->TouchCount();
	bmp->BeginDataAccess();
	bmp->DataAddress(); // ignore returned value for this test.
	bmp->EndDataAccess(ETrue);
	newTouchCount = bmp->TouchCount();
	TEST(newTouchCount == touchCount);
	TEST(!bmp->IsVolatile());
	// 2. Call as writeable  (by passing EFalse into End())
	// Expect: touch count increases & bitmap not volatile
	touchCount = newTouchCount;
	bmp->BeginDataAccess();
	bmp->DataAddress(); // ignore returned value for this test.
	bmp->EndDataAccess(EFalse);
	newTouchCount = bmp->TouchCount();
	TEST(newTouchCount == touchCount+1);
	TEST(!bmp->IsVolatile());
	// 3. Call without Begin/End pairing
	// Expect: bitmap is volatile
	bmp->DataAddress(); // ignore returned value for this test.
	TEST(bmp->IsVolatile());
	bmp->Reset();

	CleanupStack::PopAndDestroy(bmp);
	}

#ifdef _DEBUG
/*
 * Creates an extended bitmap and it tries to use it with TBitmapUtil, which should cause a panic
 */
LOCAL_C void DoBitmapUtilWithExtendedBitmapThreadL()
    {  
    const TUint8 KTestData[]        = "Extended bitmap test data 123456";
    const TInt KTestDataSize        = sizeof(KTestData);
    const TSize KSizeInPixels       = TSize(50,50);
    const TDisplayMode KDisplayMode = EColor64K;

    CFbsBitmap* bmp = new(ELeave)CFbsBitmap;
    CleanupStack::PushL(bmp);
     
    const TUid KUidTestExtendedBitmap   = TUid::Uid(0xFFFFFFFF);
    User::LeaveIfError(bmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KUidTestExtendedBitmap, KTestData, KTestDataSize));
     
    TBitmapUtil util(bmp); // this will cause a panic
     
    CleanupStack::PopAndDestroy(bmp);
    }

/*
 * Creates a compressed bitmap and it tries to use it with TBitmapUtil, which should cause a panic
 */
LOCAL_C void DoBitmapUtilWithCompressedBitmapThreadL()
    {
    CFbsBitmap*  bmp = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bmp);
    bmp->Load(KRamBitmap);
    User::LeaveIfError(bmp->Compress());
    if(!bmp->IsCompressedInRAM())
        {
        User::Leave(KErrArgument);
        }
     
    TBitmapUtil util(bmp); // this will cause a panic
     
    CleanupStack::PopAndDestroy(bmp);
    }

typedef void (*TFunctionPtr)();

LOCAL_C TInt BitmapUtilWithUnsupportedBitmapThreadFunc(TAny* aFunctionPtr)
    {
    TInt ret = RFbsSession::Connect();
    if(ret != KErrNone)
        {
        return ret;
        }
    
    CTrapCleanup* trap = CTrapCleanup::New();
    if (!trap)
         {
         RFbsSession::Disconnect();
         return KErrNoMemory;
         }
    
    TRAP(ret, ((TFunctionPtr)aFunctionPtr)()); // this function can leave
    
    delete trap;
    RFbsSession::Disconnect();
    
    return ret;
    }
/*
 * Creates a thread, and the test scenario for GRAPHICS-FBSERV-0661 is run in aThreadFunction, which 
 * causes a panic. Checks if the thread has been terminated with panic with correct category and panic code.
 */
void CTBitmap::TestBitmapUtilWithUnsupportedBitmap(const TDesC& aThreadName, TAny* aFunctionPtr)
    {
    RThread thread;
    TInt ret = thread.Create(aThreadName, BitmapUtilWithUnsupportedBitmapThreadFunc, KDefaultStackSize, 0x2000, 0x2000, aFunctionPtr);
    TEST(ret == KErrNone);
    TRequestStatus status;
    thread.Logon(status);
    thread.Resume();
    User::WaitForRequest(status);
    TExitType exitType = thread.ExitType();
    TExitCategoryName exitCategory = thread.ExitCategory();
    TInt exitReason = thread.ExitReason();
    TEST(exitType == EExitPanic);
    TEST(exitCategory == KFBSERVClientPanicCategory);
    TEST(exitReason == EFbsPanicInvalidBitmapType);
    thread.Close();   
    }

/**
@SYMTestCaseID          GRAPHICS-FBSERV-0661

@SYMTestCaseDesc        Test the use of TBitmapUtil with extended and compressed bitmaps. 
                        NOTE: this is a debug only test.

@SYMTestStatus          Implemented

@SYMTestPriority        High

@SYMTestActions         Create extended bitmap, and TBitmapUtil to process it. 
                        Create bitmap and compress it,and TBitmapUtil to process it. 

@SYMTestExpectedResults Panic FBSCLI 29
*/
void CTBitmap::TestBitmapUtilWithUnsupportedBitmaps()
    {
    INFO_PRINTF1(_L("Test TBitmapUtil with unsupported bitmaps"));
    
    // extended bitmap
    TestBitmapUtilWithUnsupportedBitmap(_L("BitmapUtilWithExtentedBitmapThread"), (TAny*)&DoBitmapUtilWithExtendedBitmapThreadL);
    
    // compressed bitmap
    TestBitmapUtilWithUnsupportedBitmap(_L("BitmapUtilWithCompressedBitmapThread"), (TAny*)&DoBitmapUtilWithCompressedBitmapThreadL);  
    }
#endif //_DEBUG

//--------------
__CONSTRUCT_STEP__(Bitmap)

void CTBitmapStep::TestSetupL()
	{
	TheFs.Connect();
	::CopyFilesOnC();
	}
	
void CTBitmapStep::TestClose()
	{
	::DeleteFilesFromC();		//clean up data files used by this test - must be done before call to End() - DEF047652
	TheFs.Close();
	}

