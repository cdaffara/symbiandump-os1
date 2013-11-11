// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bitdev.h>
#include <e32math.h>
#include "tcompressed.h"

CTCompressed::CTCompressed(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	}

void CTCompressed::BlankBitmap(CFbsBitmapEx& aBitmap)
	{
	TSize size = aBitmap.SizeInPixels();
	TInt dataLength = CFbsBitmap::ScanLineLength(size.iWidth,aBitmap.DisplayMode()) * size.iHeight;
	aBitmap.LockHeap();
	Mem::FillZ((TUint8*)aBitmap.DataAddress(),dataLength);
	aBitmap.UnlockHeap();
	}

void CTCompressed::RunTestCaseL(TInt aCurTestCase)
    {
    ((CTCompressedStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTCompressedStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0571"));
		TRAPD(err, RunTestL());
		TEST(err == KErrNone);
		break;	
	case 2:
		((CTCompressedStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0572"));
		TRAP(err, DefectFix_EXT5DXGK6_L());
		TEST(err == KErrNone);
		break;	
	case 3:
		{
		((CTCompressedStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0500"));
		// Set size of bitmap large enough to warrant compression
		const TSize KBitmapSize(200,200);
		INFO_PRINTF1(_L("Test case for INC082713\r\n"));
		TRAP(err, DefectFix_INC082713_L(EColor16MU, KBitmapSize));
		TEST(err == KErrNone);		
		TRAP(err, DefectFix_INC082713_L(EColor16MA, KBitmapSize));
		TEST(err == KErrNone);
		}
		break;
	case 4:
		((CTCompressedStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0573"));
		TRAPD(err1, INC088856_TestL());
		TEST(err1 == KErrNone);
		break;
 	case 5:
 		((CTCompressedStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0615"));
		TestDestroyBitmapWhileBeingBackgroundCompressedL();
		break;
	case 6:
		((CTCompressedStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0616"));
		TestBackgroundCompressionThreadPriorityInheritanceL();
		break;
	case 7:
		((CTCompressedStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTCompressedStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;	
		}
	((CTCompressedStep*)iStep)->RecordTestResultL();
    }

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0571

	@SYMTestCaseDesc
	Tests compression of bitmaps in RAM and ROM.

	@SYMTestActions
	Four bitmaps are loaded. One compressed and one
	uncompressed from ROM and from RAM. The bitmap
	in RAM is tested if it can be compressed successfully.
	The bitmap in ROM should fail when tried to be compressed.
	All of the bitmaps are then tried to be BitBlt.	
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTCompressed::RunTestL()
    {
    TInt skipRomBitmapTests = EFalse;
	TUint32* romAddress = NULL;
	if(!CFbsBitmap::IsFileInRom(KRomNotCompressed, romAddress)) //any ROM bitmap
		{
		INFO_PRINTF2(_L("Skipping ROM bitmap tests since file \"%S\" is reported to not be a ROM bitmap."),
				&KRomNotCompressed);
		INFO_PRINTF1(_L("This should only occur on non-XIP ROMs, e.g. NAND ROMs, where ROM bitmaps aren't supported."));
		skipRomBitmapTests = ETrue;
		}
    
	TBuf<64> romNotCompressed(KRomNotCompressed);
	TBuf<64> romCompressed(KRomCompressed);
	TBuf<64> fileNotCompressed(KFileNotCompressed);
	TBuf<64> fileCompressed(KFileCompressed);

	TInt ret = KErrNone;
	CFbsBitmap rom;
	CFbsBitmap romcomp;

	if(!skipRomBitmapTests)
		{
		TEST(rom.Load(romNotCompressed, 0, EFalse)== KErrNone);
		TEST(romcomp.Load(romCompressed, 0, EFalse)== KErrNone);

		INFO_PRINTF1(_L("Check compressing of ROM bitmaps causes error.\r\n"));
		ret = rom.Compress();
		TEST(ret == KErrAccessDenied); //cannot compress bitmaps in ROM
		TEST(rom.IsCompressedInRAM());
		}
	
	CFbsBitmap file;
	CFbsBitmap filecomp;
	TEST(file.Load(fileNotCompressed, 0, EFalse)== KErrNone);
	TEST(filecomp.Load(fileCompressed, 0, EFalse)== KErrNone);
	
	INFO_PRINTF1(_L("Check compressing of RAM files is OK.\r\n"));
	CFbsBitmap ram;
	ret=ram.Load(fileNotCompressed,0,EFalse);
	TEST(ret == KErrNone);
	ret = ram.Compress();
	TEST(ret == KErrNone); //can compress bitmaps in RAM
	TEST(ram.IsCompressedInRAM());
	
	INFO_PRINTF1(_L("Check compressing a created EColor256 bitmap.\r\n"));
	CFbsBitmapEx created;
	ret=created.Create(TSize(200,200),EColor256);
	TEST(ret==KErrNone);
	
	// This makes sure we can compress it...
	BlankBitmap(created);

	ret = created.Compress();
	TEST(ret==KErrNone);
	TEST(created.IsCompressedInRAM());

	INFO_PRINTF1(_L("Check compressing a created EColor24MU bitmap.\r\n"));
	CFbsBitmapEx created24MU;
	ret=created24MU.Create(TSize(200,200),EColor16MU);
	TEST(ret==KErrNone);
	
	// This makes sure we can compress it...
	BlankBitmap(created24MU);

	ret = created24MU.Compress();
	TEST(ret==KErrNone);
	TEST(created24MU.IsCompressedInRAM());

	INFO_PRINTF1(_L("Try bitblt on all bitmaps"));
	for (TInt mode = EGray2; mode < EColorLast; mode++)
		{
		if(!skipRomBitmapTests)
			{
			INFO_PRINTF2(_L("BitBlt rom with %d display mode.\r\n"), mode);
			TestBitBltL(rom,      TDisplayMode(mode));
			INFO_PRINTF2(_L("BitBlt romcomp with %d display mode.\r\n"), mode);
			TestBitBltL(romcomp,  TDisplayMode(mode));
			}
		INFO_PRINTF2(_L("BitBlt file with %d display mode.\r\n"), mode);
		TestBitBltL(file,     TDisplayMode(mode));
		INFO_PRINTF2(_L("BitBlt filecomp with %d display mode.\r\n"), mode);
		TestBitBltL(filecomp, TDisplayMode(mode));
		INFO_PRINTF2(_L("BitBlt ram with %d display mode.\r\n"), mode);
		TestBitBltL(ram,      TDisplayMode(mode));
		INFO_PRINTF2(_L("BitBlt created with %d display mode.\r\n"), mode);
		TestBitBltL(created,  TDisplayMode(mode));
		INFO_PRINTF2(_L("BitBlt 24MU created bitmap with %d display mode.\r\n"), mode);
		TestBitBltL(created24MU,  TDisplayMode(mode));
		}
	}



/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0572

	@SYMTestCaseDesc
	Testcode to check for fix to defect EXT-5DXGK6 Bitmap 
	compression causes crash when loading skins.
	Its not a conclusive test as it depends on where the 
	bitmap is loaded into memory and if there is 
	memory allocated at the end of the bitmap.

	@SYMTestActions
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTCompressed::DefectFix_EXT5DXGK6_L()
	{
	CFbsBitmapEx bmp1;
	TSize size(1000,1);
	TInt ret=bmp1.Create(size,EGray2);
	TEST(ret==KErrNone);
	BlankBitmap(bmp1);
//
	// calculate the size of the destination scan line in bytes
	// + 31 to pad to 4 byte boundary; 8 bits in a byte
	TInt source_buffer_size = (size.iWidth + 31) / 8 ;
	TUint8* buffer = new(ELeave) TUint8[source_buffer_size];
	TPtr8 source_ptr(buffer,source_buffer_size,source_buffer_size);
	
	for(TInt ii = 0; ii<source_buffer_size; ++ii)
		{
		if(ii%2)
			source_ptr[ii] = 2;
		else
			source_ptr[ii] = 3;
		}

	source_ptr[source_buffer_size-1] = 5;
	source_ptr[source_buffer_size-2] = 5;
	source_ptr[source_buffer_size-3] = 5;
	source_ptr[source_buffer_size-4] = 4;
	bmp1.SetScanLine(source_ptr,0);

	delete [] buffer;
	bmp1.Compress();
	}

/**
 * @SYMTestCaseID GRAPHICS-FBSERV-0500
 *
 * @SYMDEF INC082713
 *
 * @SYMTestCaseDesc Test RLE compression algorithm of EColor16MU and EColor16MA bitmaps.
 *
 * @SYMTestPriority Critical
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions The following test checks a corner-case of the algorithm triggered by a specific pixel pattern.
 *
 * The corner case is triggered when the input to the compression function ends in a run of equal pixels, 
 * terminated by a single pixel that is different i.e. ...rrrrrrrrrrrrrrrg 
 * where we have a run of equal (red) pixels followed by a green pixel, which is the last pixel in the 
 * bitmap (or the scanline, depending on how the data is being compressed).
 *
 * This test case performs the following:
 * 1) Creates a bitmap in the display mode and of size specified in function args.
 * This size must be large enough to trigger the compression when requested.
 * 2) Creates a scanline the width of this bitmap, which is a run of equal pixels, and 
 * then sets the last pixel to be different.
 * 3) Sets the last scanline of the bitmap to the scanline we have just created. 
 * This means that the last scanline of the bitmap conforms to the corner-case pattern.
 * 4) Compresses the bitmap
 * 5) Checks the compression has been successful
 * 6) Retrieves the last scanline of the bitmap and compares it with the original one
 * to verify that the compression of data has been successful.
 *
 * @SYMTestExpectedResults
 * The compression should occur successfully without panicking.
 * The scanline retrieved from the compressed version of the bitmap should match
 * the original scanline created.
 */
void CTCompressed::DefectFix_INC082713_L(const TDisplayMode aDispMode, const TSize aBitmapSize)
	{
	// *** This function assumes that pixels are stored as 32 bit words ***
	// *** for EColor16MU and EColor16MA bitmaps                        ***
	switch(aDispMode)
		{
	case EColor16MU:
		INFO_PRINTF1(_L("Check compression algorithm corner case for EColor16MU bitmap.\r\n"));
		break;
	case EColor16MA:
		INFO_PRINTF1(_L("Check compression algorithm corner case for EColor16MA bitmap.\r\n"));
		break;
	default:
		INFO_PRINTF1(_L("Unsupported display mode for test"));
		return;
		}

	CFbsBitmap bitmap;
	TInt ret=bitmap.Create(aBitmapSize,aDispMode);
	TEST(ret==KErrNone);

	TInt widthInPixels = bitmap.SizeInPixels().iWidth;
	TInt heightInPixels = bitmap.SizeInPixels().iHeight;
	TInt scanLineByteLength = bitmap.ScanLineLength(widthInPixels, aDispMode);
	TUint32* buffer = (TUint32*)User::AllocLC(scanLineByteLength);

	// Breaking from a run of equal pixels
	// i.e. rrrrrrr...rrrrrrrrg
	for (TInt ii = 0; ii < widthInPixels; ii++)
		{
		buffer[ii] = 0xc6ebff;
		}
	// Last pixel differs - must be able to re-enter loop when at last pixel
	buffer[widthInPixels - 1] = 0xcee7ff;

	TPtr8 scanline((TUint8*)buffer ,scanLineByteLength,scanLineByteLength);

	bitmap.SetScanLine(scanline, heightInPixels - 1);

	INFO_PRINTF1(_L("Compressing bitmap"));
	ret = bitmap.Compress();
	TEST(ret==KErrNone);
	TEST(bitmap.IsCompressedInRAM());

	// Retrieve the scanline from the compressed bitmap and compare with original
	// in order to test whether the scanline is correctly compressed
	INFO_PRINTF1(_L("Retrieving scanline from compressed bitmap"));

	TUint32* readbackbuffer = (TUint32*)User::AllocLC(scanLineByteLength);
	TPtr8 readbackscanline((TUint8*)readbackbuffer ,scanLineByteLength,scanLineByteLength);
	readbackscanline.Fill(0xff);
	bitmap.GetScanLine(readbackscanline,TPoint(0,heightInPixels - 1),widthInPixels, aDispMode);

	INFO_PRINTF1(_L("Comparing scanlines\r\n"));
	// Compare the original scanline with the one retrieved from the compressed version
	switch(aDispMode)
		{
	case EColor16MU:
		{		
		TBool result = ETrue;
		for (TInt ii = 0; ii < widthInPixels; ii++)
			{
			// Must mask off top bits (as we can ignore them, since we have no alpha channel)
			TUint32 original = buffer[ii] & 0x00ffffff;
			TUint32 readback = readbackbuffer[ii] & 0x00ffffff;
			result = result && (original == readback);
			}
		TEST(result);
		}
		break;
	case EColor16MA:
		ret = Mem::Compare((TUint8*)buffer, scanLineByteLength, (TUint8*)readbackbuffer, scanLineByteLength);
		// Mem::Compare returns 0 if the two scanlines are equal
		TEST(ret == 0);
		break;
	default:
		INFO_PRINTF1(_L("Unsupported display mode for test"));
		break;
		}

	CleanupStack::PopAndDestroy(2, buffer);
	}

void CTCompressed::TestBitBltL(CFbsBitmap& aBmp, TDisplayMode aDispMode)
	{
	CFbsScreenDevice* scd = 0;
	TRAPD(ret, scd = CFbsScreenDevice::NewL(_L("scdv"),aDispMode));
	if (!scd)
		{
		WARN_PRINTF3(_L("Could not create screen device in %d mode, reason=%d.\r\n"), aDispMode, ret);
		return;
		}
	CleanupStack::PushL(scd);
	
	scd->ChangeScreenDevice(NULL);
	CFbsBitGc* sgc;
	if (scd->CreateContext((CGraphicsContext*&)sgc)==KErrNone) 
		{
		CleanupStack::PushL(sgc);

		TSize halfScreen = scd->SizeInPixels();
		halfScreen.iWidth /= 2;

		INFO_PRINTF1(_L("BitBlt..."));
		sgc->BitBlt(TPoint(halfScreen.iWidth,0),&aBmp);
		INFO_PRINTF1(_L("worked\r\n"));
		
		CleanupStack::PopAndDestroy(sgc);
		}
	else
		{
		INFO_PRINTF2(_L("Could not create context in %d mode.\r\n"), aDispMode);
		}
	CleanupStack::PopAndDestroy(scd);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0573

	@SYMDEF INC088856

	@SYMTestCaseDesc
	Verifies defect INC088856. Checks the 
	updated padding value before compression for
	256 gray scale and 64k colour.

	@SYMTestActions
	Creates a bitmap. Gets the bitmaps scanline length.
	Locks the heap. Retrieves the bitmaps data address.
	Unlocks the heap. Then for both 256 gray scale and 64k colour
	it checks the padding after compression matches before compression.
	
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTCompressed::INC088856_TestL()
	{
	INFO_PRINTF1(_L("Check updated Padding value in before compression \r\n"));
	
	INFO_PRINTF1(_L("Check updated Padding value for EGray256 \r\n"));
	DoUpdateOnPadding(TSize(5,3),EGray256);
	DoUpdateOnPadding(TSize(25,43),EGray256);

	INFO_PRINTF1(_L("Check updated Padding value for EColor64K \r\n"));
	DoUpdateOnPadding(TSize(5,3),EColor64K);
	DoUpdateOnPadding(TSize(25,43),EColor64K);
	}

void CTCompressed::DoUpdateOnPadding(const TSize aSize, const TDisplayMode aDispMode)
	{
	CFbsBitmapEx bmp1;
	TInt ret=bmp1.Create(aSize,aDispMode);
	TEST(ret==KErrNone);
	
	TInt dataLength = CFbsBitmap::ScanLineLength(aSize.iWidth, bmp1.DisplayMode()) * aSize.iHeight;
	
	bmp1.LockHeap();
	TUint8* srcBits = (TUint8*)bmp1.DataAddress();
	bmp1.UnlockHeap();
	TInt val=1,pos=0;
	switch (aDispMode)
		{
	case EGray256:
		{
		TUint8* bmpBits = srcBits;
		TUint8* bmpBitsLimit = srcBits + dataLength;
		while(bmpBits<bmpBitsLimit)
			{
			if(pos++ < aSize.iWidth)
				{
				*bmpBits++ = val;
				}
			else 
				{
				bmpBits += 3;
				val++;
				pos=0;
				}
			}
		}
		break;
	case EColor64K:
		{
		TUint16* bmpBits = (TUint16*)srcBits;
		TUint16* bmpBitsLimit = (TUint16*)(srcBits + dataLength);
		while(bmpBits<bmpBitsLimit)
			{
			if(pos++ < aSize.iWidth)
				{
				*bmpBits++ = val;
				}
			else 
				{
				bmpBits++;
				val++;
				pos=0;
				}
			}
		}
		break;
	default:
		break;
		}

	bmp1.Compress();
	TInt stride = bmp1.DataStride();
	TUint8* dataPtr = (TUint8*)bmp1.DataAddress();
	switch(aDispMode)
		{
	case EGray256:
		{
		for (TInt row = 0; row < aSize.iHeight - 1; ++row)
			{
			TEST(*dataPtr++ == stride - 1);
			TEST(*dataPtr++ == row + 1);
			}
		break;
		}
	case EColor64K:
		{
		for (TInt row = 0; row < aSize.iHeight - 1; ++row)
			{
			TEST(*dataPtr++ == stride / 2 - 1);
			TEST(*dataPtr++ == row + 1);
			TEST(*dataPtr++ == 0);
			}
		}
		break;
	default:
		break;
		}
	}

/**
 * @SYMTestCaseID GRAPHICS-FBSERV-0615
 *
 * @SYMDEF PDEF115511
 *
 * @SYMTestCaseDesc
 * Test Fbserv robustness when bitmaps are destroyed while they are being background compressed
 *
 * @SYMTestPriority High
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions
 * Preform various scenarios to create bitmaps, compress them in background and then destroy the 
 * bitmaps without waiting the compression to complete.
 *
 * @SYMTestExpectedResults
 * Fbserv does not crash because of misfirig its background compression active object
 */
void CTCompressed::TestDestroyBitmapWhileBeingBackgroundCompressedL()
	{
	const TInt KNumOfBitmaps = 1000;
	const TSize KSize(1,1);
	const TDisplayMode KMode = EColor64K;
	CFbsBitmap* bitmaps[KNumOfBitmaps];

	// TestCase #1
	// Create N bitmap instances, compress them in background and destroy them in-reverse order
	// without waiting compression to complete
	INFO_PRINTF2(_L("Create %d bitmaps,compress them in background and destroy in reverse order"), KNumOfBitmaps);

	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		bitmaps[idx] = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bitmaps[idx]);
		User::LeaveIfError(bitmaps[idx]->Create(KSize, KMode));
		}

	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		User::LeaveIfError(bitmaps[idx]->CompressInBackground());
		}

	// Delete bitmaps in reverse order
	CleanupStack::PopAndDestroy(KNumOfBitmaps, bitmaps[0]);

	// TestCase #2
	// Create N bitmap instances, compress them in background and destroy them in the compression order
	// without waiting compression to complete
	INFO_PRINTF2(_L("Create %d bitmaps,compress them in background and destroy in forward order"), KNumOfBitmaps);

	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		bitmaps[idx] = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bitmaps[idx]);
		User::LeaveIfError(bitmaps[idx]->Create(KSize, KMode));
		}

	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		User::LeaveIfError(bitmaps[idx]->CompressInBackground());
		}

	// Delete bitmaps in compression order
	CleanupStack::Pop(KNumOfBitmaps, bitmaps[0]);	
	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		delete bitmaps[idx];
		}
	
	// TestCase #3
	// Create N bitmap instances, compress them in background and destroy them in random order
	// without waiting compression to complete
	INFO_PRINTF2(_L("Create %d bitmaps,compress them in background and destroy in random order"), KNumOfBitmaps);

	TInt randomOrder[KNumOfBitmaps];
	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		bitmaps[idx] = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bitmaps[idx]);
		User::LeaveIfError(bitmaps[idx]->Create(KSize, KMode));
		randomOrder[idx] = idx;
		}
	// Randomise deletion order
	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		TInt swap = Math::Random()%KNumOfBitmaps;
		TInt tmp = randomOrder[idx];
		randomOrder[idx] = randomOrder[swap];
		randomOrder[swap] = tmp;
		}

	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		User::LeaveIfError(bitmaps[idx]->CompressInBackground());
		}

	// Delete bitmaps in random order
	CleanupStack::Pop(KNumOfBitmaps, bitmaps[0]);	
	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		delete bitmaps[randomOrder[idx]];
		bitmaps[randomOrder[idx]] = NULL;
		}
	// Test all bitmaps has been destroyed
	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		TEST(bitmaps[idx]==NULL);
		}

	// TestCase #4
	// Create bitmap, compress it in background and immediately destroyed without waiting compression
	// to complete
	INFO_PRINTF2(_L("Create bitmap,compress in background and immediately destroyed. Repeat %d times"), KNumOfBitmaps);

	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bitmap);
		User::LeaveIfError(bitmap->Create(KSize, KMode));
		User::LeaveIfError(bitmap->CompressInBackground());
		CleanupStack::PopAndDestroy(bitmap);
		}

	// TestCase #5
	// Similar from #4 but executed from two different threads
	INFO_PRINTF2(_L("Create bitmap,compress in background and immediately destroyed from 2 different threads. Repeat %d times"), KNumOfBitmaps);

	const TInt KStackSize = 4096;
	const TInt KMinHeapSize = 4096;
	const TInt KMaxHeapSize = 1024*1024;
	RThread t1;
	User::LeaveIfError(t1.Create(_L("bgCompThreadFunc#1"), CTCompressed::BgCompThreadFunc, KStackSize, KMinHeapSize, KMaxHeapSize, NULL));
	RThread t2;
	User::LeaveIfError(t2.Create(_L("bgCompThreadFunc#2"), CTCompressed::BgCompThreadFunc, KStackSize, KMinHeapSize, KMaxHeapSize, NULL));

	TRequestStatus s1;
	TRequestStatus s2;
	t1.Logon(s1);
	t2.Logon(s2);

	t1.Resume();
	t2.Resume();
	User::WaitForRequest(s1);
	User::WaitForRequest(s2);

	t1.Close();
	t2.Close();
	}

TInt CTCompressed::BgCompThreadFunc(TAny*)
	{
	const TInt KNumOfBitmaps = 1000;
	const TSize KSize(1,1);
	const TDisplayMode KMode = EColor64K;

	// No need to worries with cleanup stack and trap as resources will be freed if the thread
	// running this functions dies, panics or leaves.

	RFbsSession::Connect();

	for (TInt idx=0; idx<KNumOfBitmaps; ++idx)
		{
		CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
		User::LeaveIfError(bitmap->Create(KSize, KMode));
		User::LeaveIfError(bitmap->CompressInBackground());
		delete bitmap;
		}

	RFbsSession::Disconnect();
	return KErrNone;
	}


/**
 * @SYMTestCaseID GRAPHICS-FBSERV-0616
 *
 * @SYMDEF INC123238
 *
 * @SYMTestCaseDesc
 * Test Fbserv bitmap's priority inversion when we use background compression against a long running thread
 *
 * @SYMTestPriority High
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions
 * Create a thread which creates a bitmap and makes a call to background compression. Once this thread resumed,
 * make the current thread as busy by doing some long running task and test the exit status of the bitmap compression thread.
 *
 * @SYMTestExpectedResults
 * Fbserv does priority inheritance and therefore protect against priority inversion, so the thread function will
 * do backgroud compression and the thread will exit successfully, which will pass the test. If it is not protect
 * against priority inversion with in 30 seconds(approximately) the test will fail without doing background compression.
 */
void CTCompressed::TestBackgroundCompressionThreadPriorityInheritanceL()
	{
	const TInt KStackSize = 4096;
	const TInt KMinHeapSize = 4096;
	const TInt KMaxHeapSize = 1024*1024;
	const TInt KMaxViewChangeTime = 2000;
	
	INFO_PRINTF1(_L("Test case for Background Compression Thread Priority Inheritance\r\n"));	
	//Create and start a thread which create a bitmap and calls CompressInBackground
	RThread thread1;
	User::LeaveIfError(thread1.Create(_L("CompressBgThreadFunc"), CTCompressed::CompressBgThreadFunc, KStackSize, KMinHeapSize, KMaxHeapSize, NULL));
	thread1.Resume();
	TUint startTime = User::TickCount();
	
	//Start a infinite running loop to make current thread as a busy and long running thread...
	for (;;)
		{
		TExitType exitType = thread1.ExitType();
		if(exitType != EExitPending)
			{
			TInt exitReason = thread1.ExitReason();
			TEST(exitType == EExitKill);
			TEST(exitReason == KErrNone);
			break;
			}
		TUint stopTime = User::TickCount();
		if((stopTime - startTime) > KMaxViewChangeTime) //Check for the busy task is running for more than 30 seconds...
			{
			TEST(EFalse);
			thread1.Terminate(KErrGeneral);
			break;
			}
		}
	thread1.Close();
	}

/*
 Thread function used from the test case TestBackgroundCompressionThreadPriorityInheritanceL
 */
TInt CTCompressed::CompressBgThreadFunc(TAny*)
	{
	const TSize KSize(100, 100);
	const TDisplayMode KMode = EColor64K;

	TInt err = RFbsSession::Connect();
	if (err != KErrNone)
		return err;
	CFbsBitmap* bitmap = new CFbsBitmap;
	if (!bitmap)
		return KErrNoMemory;
	err = bitmap->Create(KSize, KMode);
	if (err != KErrNone)
		return err;
	err = bitmap->CompressInBackground();
	if (err != KErrNone)
		return err;
	delete bitmap;

	RFbsSession::Disconnect();
	return KErrNone;
	}

//--------------
__CONSTRUCT_STEP__(Compressed)
