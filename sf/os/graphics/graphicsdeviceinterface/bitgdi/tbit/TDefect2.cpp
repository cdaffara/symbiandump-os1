// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <random.h>
#include <s32file.h>
#include <e32math.h>
#include <s32mem.h>
#include <graphics/gdi/gdiconsts.h>
#include <e32cmn.h>
#include "BITPANIC.H"
#include "TDefect2.h"
#include "fbsmessage.h"

//===================================================================
//In order to test multiple screens creation process on the Emulator, 
//you have to extend your epoc.ini file with the following lines
//_NewScreen_
//ScreenWidth 640
//ScreenHeight 480
//===================================================================

//
//
//Globals
LOCAL_D RFs		FServSession;

//
//
//Test file names
_LIT(KArchiveFileName, "C:\\FbsBitGcArchive.dat");

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

//
CTDefect2::CTDefect2(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iScrDev(NULL),
	iGc(NULL),
	iSize(0, 0),
	iCurrentMode(ENone),
	iBitmap(NULL),
	iBmpDevice(NULL)
	{
	}

CTDefect2::~CTDefect2()
	{
	DestroyFont();
	DeleteBitmap();
	DeleteGraphicsContext();
	DeleteBitmapDevice();
	DeleteScreenDevice();
	}

void CTDefect2::ConstructL()
	{
	User::LeaveIfError(::FServSession.Connect());
	}

void CTDefect2::RunTestCaseL(TInt aCurTestCase)
	{
	((CTDefect2Step*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	_LIT(KTest1,"SubTest %d: DEF039237");
	_LIT(KTest2,"SubTest %d: DEF039331");
	_LIT(KTest3,"SubTest %d: DEF039650");
	_LIT(KTest4,"SubTest %d: CFbsBitmap::GetPixel() performance");
	_LIT(KTest5,"SubTest %d: Rotate/Move text");
	_LIT(KTest6,"SubTest %d: SwapWidthAndHeight");
	_LIT(KTest8,"SubTest %d: Clear with non-zero origin");
	_LIT(KTest9,"SubTest %d: DEF115395: DrawBitmap & DrawBitmapMasked with a sourceRect out of the Bitmap bounds");
	_LIT(KTest10,"SubTest %d: INC119063: General test CopyRect rewrite for low color depth (8,4,2, pixels per byte)");
	_LIT(KTest11,"SubTest %d: INC120917: Dirty Mask Bitmap not remapped in BitBltMasked");
	_LIT(KTest12,"SubTest %d: Zero-sized brush pattern bitmaps");
	_LIT(KTest13,"SubTest %d: CopyRect with non-trivial alpha");
	_LIT(KTest14,"SubTest %d: INC119063: CopyRect reading past end of bitmap data at unallocated pages");
	_LIT(KTest15,"SubTest %d: INC128813 : CFbsBitGc::InternalizeL handle duplicate fail testing");
	_LIT(KTest16,"SubTest %d: DEF132331 : CFbsScreenDevice PixelsToTwips conversions faulty with large inputs");
	_LIT(KTest17,"SubTest %d: DEF137103: CFbsBitmapDevice::SetBits() should check bitmap size");
	switch(aCurTestCase)
		{
	case 1:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0066"));
		INFO_PRINTF2(KTest1,aCurTestCase);
		DEF039237L();
		break;
	case 2:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0067"));
		INFO_PRINTF2(KTest2,aCurTestCase);
		DEF039331L();
		break;
	case 3:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0068"));
		INFO_PRINTF2(KTest3,aCurTestCase);
		DEF039650L();
		break;
	case 4:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0069"));
		INFO_PRINTF2(KTest4,aCurTestCase);
		GetPixelPerformance1L();
		break;
	case 5:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0070"));
		INFO_PRINTF2(KTest5,aCurTestCase);
		RotateMoveTextL();
		break;
	case 6:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0071"));
		INFO_PRINTF2(KTest6,aCurTestCase);
		SwapWidthAndHeightL();
		break;
	case 7:
		// Test case moved to tmultiscreens.cpp
		break;
	case 8:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0073"));
		INFO_PRINTF2(KTest8,aCurTestCase);
		NonZeroOriginClearL();
		break;
	case 9:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0097"));
		INFO_PRINTF2(KTest9,aCurTestCase);
		DEF115395L();
		break;
	case 10:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0099"));
		INFO_PRINTF2(KTest10,aCurTestCase);
		ExerciseCopyRect();
		break;
	case 11:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0101"));
		INFO_PRINTF2(KTest11,aCurTestCase);
		TestDirtyMaskBitmapL();
		break;
	case 12:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0108"));
		INFO_PRINTF2(KTest12,aCurTestCase);
		ZeroSizedPatternBrushL();
		break;
	case 13:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0113"));
		INFO_PRINTF2(KTest13,aCurTestCase);
		CopyRectAlphaL();
		break;
	case 14: 	
		//Note: This test generates panic windows... you may need to put your test before it.
		//They will be closed at the end of the test set, but they may get in the way of your next test, sorry.
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0098"));
		INFO_PRINTF2(KTest14,aCurTestCase);
		CopyRectReadOutsideBitmap();
		break;
	case 15:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0110"));
		INFO_PRINTF2(KTest15,aCurTestCase);
		CFbsBitGcInternalizeLFailL();
		break;
	case 16:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0111"));
		INFO_PRINTF2(KTest16,aCurTestCase);
		PixelsToTwipsConversionCheck();
		break;
	case 17:
		((CTDefect2Step*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0112"));
		INFO_PRINTF2(KTest17,aCurTestCase);
		TestSetBitsL();
		break;

    case 18:
        INFO_PRINTF1(_L("PDEF141192: test CFbsBitGc::DrawBitmapMasked()"));
        INFO_PRINTF1(_L("Test Red Channel"));
        TestMaskForSelectedValuesL(0);
        INFO_PRINTF1(_L("Test Green Channel"));
        TestMaskForSelectedValuesL(1);
        INFO_PRINTF1(_L("Test Blue Channel"));
        TestMaskForSelectedValuesL(2);
        break;
        
	//Insert tests here

	case 19:
		((CTDefect2Step*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTDefect2Step*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTDefect2Step*)iStep)->RecordTestResultL();
	}

/**
 * @SYMTestCaseID GRAPHICS-BITGDI-0101
 *
 * @SYMDEF INC120917
 *
 * @SYMTestCaseDesc If mask bitmap has been dirtied (by resize/compress) 
 * 						then bitbltmasked should not read old memory contents
 *
 * @SYMTestPriority Normal
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions Create image and mask bitmap with  pattern and blit them for reference
 * 					create duplicate handle 
 * 					resize the mask on duplicate
 * 					verify that blit produces same result for same area on larger mask 
 * 					and change pattern
 * 					blit using duplicate handle and source
 * 					blit using original handle and source
 * 						in defect scenario these results are different (original same as reference)
 * 						in fix scenario they should be same (different to reference)
 * 					revalidate original handle reblit and verify now correct
 * 
 * 					repeat above steps: 
 * 					(this proves duplicate change causes original flag, not just duplication)
 * 					resize the mask on duplicate
 * 					verify that blit produces same result for same area on larger mask 
 * 					and change pattern
 * 					blit using duplicate handle and source
 * 					blit using original handle and source
 * 						in defect scenario these results are different (original same as reference)
 * 						in fix scenario they should be same (different to reference)
 * 					revalidate original handle reblit and verify now correct
 */	
void  CTDefect2::TestDirtyMaskBitmapL()
	{
	const TInt 	KWidth=120;
	const TInt	KHeight=50;
	const TInt	KBigger=50;
	enum
		{
		EStepBefore=0,
		EStepVerifyDupGrow,
		EStepDupDiffAfterRefill,
		EStepOriginalDiffAfterRefill,
		EStepOriginalDiffAfterRefillAndGetAddress,
		EStepDupDiffAfterRefill2,
		EStepOriginalDiffAfterRefill2,
		ENumSteps
		};
	TInt pushcount=0;
	//set up the target
	CFbsBitmap* targetBmp=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(targetBmp);pushcount++;	
	User::LeaveIfError(targetBmp->Create(TSize(KWidth,KHeight*ENumSteps), EColor16MU));	
	CFbsBitmapDevice* bmpDevice=CFbsBitmapDevice::NewL(targetBmp);
	CleanupStack::PushL(bmpDevice);pushcount++;	
	CBitmapContext* bmpContext;
	User::LeaveIfError(bmpDevice->CreateBitmapContext(bmpContext));
	CleanupStack::PushL(bmpContext);pushcount++;		
	bmpContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bmpContext->SetBrushColor(KRgbRed);	
	bmpContext->Clear();
	
	//set up source and mask
	CFbsBitmap* sourceBmp=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(sourceBmp);pushcount++;		
	User::LeaveIfError(sourceBmp->Create(TSize(KWidth,KHeight), EColor16MU));
	TUint32* sourceData=sourceBmp->DataAddress();
	TInt 	 sourceLength=(sourceBmp->DataStride()>>2)*KHeight;
	CFbsBitmap* maskBmp=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(maskBmp);pushcount++;		
	User::LeaveIfError(maskBmp->Create(TSize(KWidth,KHeight), EGray2));
	TUint32* maskDataStart=maskBmp->DataAddress();
	TInt 	 maskLength=(maskBmp->DataStride()>>2)*KHeight;

	TUint32* targetData=targetBmp->DataAddress();
	TInt targetCopySize=((targetBmp->DataStride()+3)>>2)*KHeight;

	TUint32 blueint=KRgbBlue.Color16MU();
	for (TInt fillIndex=0;fillIndex<sourceLength;fillIndex++)
		{
		sourceData[fillIndex]=blueint;
		}
	TInt maskPattern=0x12345678;
	for (TInt fillIndex=0;fillIndex<maskLength;fillIndex++)
		{
		maskDataStart[fillIndex]=maskPattern;
		}
	
	//Initial result of blit operation
	bmpContext->BitBltMasked(TPoint(0,KHeight*EStepBefore),sourceBmp,TSize(KWidth,KHeight),maskBmp,EFalse);
	
	//duplicate the mask handle
	CFbsBitmap* maskBmpDup=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(maskBmpDup);pushcount++;	
	maskBmpDup->Duplicate(maskBmp->Handle());
	TUint32* maskDataDup=maskBmpDup->DataAddress();
	TEST(maskDataDup==maskDataStart);	//not expected to fail
	//resize duplicate handle bigger
	maskBmpDup->Resize(TSize(KWidth+KBigger,KHeight));
	maskDataDup=maskBmpDup->DataAddress();
	TEST(maskDataDup!=maskDataStart);	//not expected to fail
	
	//At this point the duplicate handle is referencing a different address to the original handle.
	//With the defect in place this remains uncorrected, 
	//and is demonstrated by comparing the output contents of blitting using each of the handles. 
	
	//verify that the resizing of the mask preserved the original mask data, so the resize has no effect on small area blit
	bmpContext->BitBltMasked(TPoint(0,KHeight*EStepVerifyDupGrow),sourceBmp,TSize(KWidth,KHeight),maskBmpDup,EFalse);
	
	//If this cmp fails: blit or resize is not working properly on the duplicate handle - not caused by this defect (very unexpected)
	TEST(0==Mem::Compare((TUint8*)(targetData),targetCopySize*4,(TUint8*)(targetData+targetCopySize*EStepVerifyDupGrow),targetCopySize*4));
	
	//Change the mask using the address in the duplicate handle
	//As we want to fill the whole mask with a known pattern we need to recalculate the mask size.
	TInt newMaskLength=(maskBmpDup->DataStride()>>2)*KHeight;
	maskPattern=0xAAAAAAAA;
	for (TInt fillIndex=0;fillIndex<newMaskLength;fillIndex++)
		{	
		maskDataDup[fillIndex]=maskPattern;
		}
	
	//compare the results of blitting using this new mask using each of the two handles to the same mask
	bmpContext->BitBltMasked(TPoint(0,KHeight*EStepDupDiffAfterRefill),sourceBmp,TSize(KWidth,KHeight),maskBmpDup,EFalse);
	//This blit should not crash, even if the defect is present because the previous sized bitmap should not be released until all references have been updated.

	bmpContext->BitBltMasked(TPoint(0,KHeight*EStepOriginalDiffAfterRefill),sourceBmp,TSize(KWidth,KHeight),maskBmp,EFalse);

	//If this cmp fails: data change had no effect for duplicate handle (very unexpected)
	TEST(0!=Mem::Compare((TUint8*)(targetData),targetCopySize*4,(TUint8*)(targetData+targetCopySize*EStepDupDiffAfterRefill),targetCopySize*4));
	
	//Check that the result of both blits is the same.
	//If this cmp fails: defect is demonstrated. This is the main defect demonstration test.
	TEST(0==Mem::Compare((TUint8*)(targetData+targetCopySize*EStepDupDiffAfterRefill),targetCopySize*4,(TUint8*)(targetData+targetCopySize*EStepOriginalDiffAfterRefill),targetCopySize*4));
	
	//This revalidates the original handle, hiding the defect
	TUint32* maskDataRevalid=maskBmp->DataAddress();
	TEST(maskDataDup==maskDataRevalid);
	
	//This blit should not crash as the original handle has now been revalidated to point to allocated data. 
	bmpContext->BitBltMasked(TPoint(0,KHeight*EStepOriginalDiffAfterRefillAndGetAddress),sourceBmp,TSize(KWidth,KHeight),maskBmp,EFalse);
	//If this cmp fails: original was not revalidated by DataAddress - content is not same as duplicate
	TEST(0==Mem::Compare((TUint8*)(targetData+targetCopySize*EStepDupDiffAfterRefill),targetCopySize*4,(TUint8*)(targetData+targetCopySize*EStepOriginalDiffAfterRefillAndGetAddress),targetCopySize*4));

	//Extra testing... do we handle situation where the duplicated handle is changed more than once?

	//Basically repeats the test sequence above.
	
	//Repeat the resize even bigger using duplicate handle
	//resize duplicate handle bigger
	maskBmpDup->Resize(TSize(KWidth+KBigger,KHeight+KBigger));
	maskDataDup=maskBmpDup->DataAddress();
	TEST(maskDataDup!=maskDataRevalid);
	
	//Change the mask using the address in the duplicate handle
	//As we want to fill the whole mask with a known pattern we need to recalculate the mask size.
	newMaskLength=(maskBmpDup->DataStride()>>2)*(KHeight+KBigger);
	maskPattern=0x55555555;
	for (TInt fillIndex=0;fillIndex<newMaskLength;fillIndex++)
		{	
		maskDataDup[fillIndex]=maskPattern;
		}
	
	//compare the results of blitting using this new mask using each of the two handles to the same mask
	
	bmpContext->BitBltMasked(TPoint(0,KHeight*EStepDupDiffAfterRefill2),sourceBmp,TSize(KWidth,KHeight),maskBmpDup,EFalse);
	//This blit should not crash, even if the defect is present because the previous sized bitmap should not be released until all references have been updated.
	//If it does crash then this demonstrates the defect in that the original address is being used in error and has been freed and paged out
	bmpContext->BitBltMasked(TPoint(0,KHeight*EStepOriginalDiffAfterRefill2),sourceBmp,TSize(KWidth,KHeight),maskBmp,EFalse);

	//If this cmp fails: data change  had no effect for duplicate handle (very unexpected)
	TEST(0!=Mem::Compare((TUint8*)(targetData+targetCopySize*EStepDupDiffAfterRefill),targetCopySize*4,(TUint8*)(targetData+targetCopySize*EStepDupDiffAfterRefill2),targetCopySize*4));

	//Check that the result of both blits is the same.
	//If this cmp fails: defect is demonstrated. This is the main defect demonstration test.
	TEST(0==Mem::Compare((TUint8*)(targetData+targetCopySize*EStepDupDiffAfterRefill2),targetCopySize*4,(TUint8*)(targetData+targetCopySize*EStepOriginalDiffAfterRefill2),targetCopySize*4));
	
	
	CleanupStack::PopAndDestroy(pushcount,targetBmp);
	}

 /**
  * @SYMTestCaseID GRAPHICS-BITGDI-0097
  *
  * @SYMDEF DEF115395
  *
  * @SYMTestCaseDesc Robustness in the CFbsBitGc::DrawBitmap & CFbsBitGc::DrawBitmapMasked functions:
  * when a sourceRect is used to capture only a part of the bitmap,
  * we have to make sure that the sourceRect is not out of border of the image!
  * (if it's the case, we just draw nothing)
  *
  * @SYMTestPriority Normal
  *
  * @SYMTestStatus Implemented
  *
  * @SYMTestActions We create a 8x8 KRgbRed Bitmap and 8x8 Mask (color TRgb(30, 30, 30))
  * We try the 2 functions CFbsBitGc::DrawBitmap and CFbsBitGc::DrawBitmapMasked
  * with a out of bounds source rectangle on a white output image.
  * The output should stay all white.                
  *
  */  
void CTDefect2::DEF115395L()
	{
	const TSize size=TSize(8, 8);
	const TPoint origin=TPoint(0, 0);
	const TPoint vector=TPoint(2, 2);
	const TRect destRect=TRect(origin, size);
	const TRect srcRect=TRect(origin-vector, size);
	const TBool invertMask=EFalse;
	const TRgb maskColor=TRgb(30, 30, 30);
	TBool succeed=ETrue;

	// Bitmap creation
	CFbsBitmap* myBmp=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(myBmp);
	User::LeaveIfError(myBmp->Create(size, EColor16MU));
	CFbsBitmapDevice* bmpDevice=CFbsBitmapDevice::NewL(myBmp);
	CleanupStack::PushL(bmpDevice);
	CBitmapContext* bmpContext;
	User::LeaveIfError(bmpDevice->CreateBitmapContext(bmpContext));
	CleanupStack::PushL(bmpContext);
	bmpContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bmpContext->SetBrushColor(KRgbRed);
	bmpContext->Clear();
	CleanupStack::PopAndDestroy(2, bmpDevice); // bmpContext, bmpDevice

	// Mask creation
	CFbsBitmap* myMaskBmp=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(myMaskBmp);
	User::LeaveIfError(myMaskBmp->Create(size, EGray256));
	bmpDevice = CFbsBitmapDevice::NewL(myMaskBmp);
	CleanupStack::PushL(bmpDevice);     
	User::LeaveIfError(bmpDevice->CreateBitmapContext(bmpContext));
	CleanupStack::PushL(bmpContext);
	bmpContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bmpContext->SetBrushColor(maskColor);
	bmpContext->Clear();
	CleanupStack::PopAndDestroy(2, bmpDevice); // bmpContext, bmpDevice

	// Output Image
	CFbsBitmap* outputImg=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(outputImg);
	User::LeaveIfError(outputImg->Create(size, EColor16MU));
	bmpDevice = CFbsBitmapDevice::NewL(outputImg);
	CleanupStack::PushL(bmpDevice);
	User::LeaveIfError(bmpDevice->CreateBitmapContext(bmpContext));
	CleanupStack::PushL(bmpContext);
	bmpContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bmpContext->SetBrushColor(KRgbWhite);
	bmpContext->Clear();

	bmpContext->DrawBitmapMasked(destRect, myBmp, srcRect, myMaskBmp, invertMask);   
	TRgb pixelColor;
	TInt ii,jj;
	for (ii=0;ii<=size.iWidth;ii++)
		{
		for (jj=0;jj<=size.iHeight;jj++)
			{
			outputImg->GetPixel(pixelColor, TPoint(ii,jj));
			if (pixelColor!=KRgbWhite)
				{
				succeed=EFalse;
				break;
				}
			}
		if (!succeed)
			{
			break;
			}
		}

	bmpContext->Clear();
	bmpContext->DrawBitmap(destRect, myBmp, srcRect);
	for (ii=0;ii<=size.iWidth;ii++)
		{
		for (jj=0;jj<=size.iHeight;jj++)
			{
			outputImg->GetPixel(pixelColor, TPoint(ii,jj));
			if (pixelColor!=KRgbWhite)
				{
				succeed=EFalse;
				break;
				}
			}
		if (!succeed)
			{
			break;
			}
		}

	TEST(succeed);
	CleanupStack::PopAndDestroy(5);
}

/**  
   @SYMDEF PDEF141192, INC140310         

   @SYMTestCaseDesc Test that the alpha merge method CFbsBitGc::DrawBitmapMasked() works accurately.
                    
   @SYMTestPriority normal

   @SYMTestStatus Implemented

   @SYMTestActions Create an alpha capable bitmap and throw sample values at the merge function
 
   @SYMTestExpectedResults
   1a)     The merge resultant alpha values do not wrap through 256
   1b)     The merge resultant alpha values closely match an equivalent floating-point calculation
   2a)     The merge resultant colours do not wrap through 256
   2b)     The merge resultant colours do not exceed the resultant alpha if premultiplied output is expected
   2c)     The merge resultant colours closely match an equivalent floating-point calculation
   2d)     The other resultant colour values are not affected (stay black) 
   
   @param  aChannelControl which channel is controlled  (0=red, 1= green, 2=blue)
   
   The input tested format is EColor64K, but could easily be changesd to test other formats.
   The input mask is EGray256. If optimised internally, this could be changed to EGray16.
   The tested output format is EColor16MAP. This should not be changed. 
   
   This test is an optimised version of CTDefect2::TestMaskForAllCombinationL(TInt aChannelControl)
*/
void CTDefect2::TestMaskForSelectedValuesL(TInt aChannelControl)
     {
     const TSize size=TSize(1, 1);
 
     // Bitmap creation
     CFbsBitmap* srcBmp=new(ELeave) CFbsBitmap();
     CleanupStack::PushL(srcBmp); 
     User::LeaveIfError(srcBmp->Create(size, EColor64K)); 
     CFbsBitmapDevice* sourceBmpDevice=CFbsBitmapDevice::NewL(srcBmp);
     CleanupStack::PushL(sourceBmpDevice);
     CBitmapContext* sourceBmpContext;
     User::LeaveIfError(sourceBmpDevice->CreateBitmapContext(sourceBmpContext));
     CleanupStack::PushL(sourceBmpContext);    
     sourceBmpContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
     sourceBmpContext->Clear();
     CleanupStack::PopAndDestroy(2, sourceBmpDevice); // sourceBmpContext, sourceBmpDevice
     
     // Mask creation
     CFbsBitmap* maskBmp=new(ELeave) CFbsBitmap();
     CleanupStack::PushL(maskBmp); 
     User::LeaveIfError(maskBmp->Create(size, EGray256)); 
     CFbsBitmapDevice* maskBmpDevice=CFbsBitmapDevice::NewL(maskBmp);
     CleanupStack::PushL(maskBmpDevice);
     CBitmapContext* maskBmpContext;
     User::LeaveIfError(maskBmpDevice->CreateBitmapContext(maskBmpContext));
     CleanupStack::PushL(maskBmpContext);    
     maskBmpContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
     maskBmpContext->Clear();
     CleanupStack::PopAndDestroy(2, maskBmpDevice); // maskBmpContext, maskBmpDevice
          
     // Target Bitmap
     CFbsBitmap* targetBmp=new(ELeave) CFbsBitmap();
     CleanupStack::PushL(targetBmp); 
     User::LeaveIfError(targetBmp->Create(size, EColor16MAP)); 
     CFbsBitmapDevice* targetBmpDevice=CFbsBitmapDevice::NewL(targetBmp);
     CleanupStack::PushL(targetBmpDevice);
     CBitmapContext* targetBmpContext;
     User::LeaveIfError(targetBmpDevice->CreateBitmapContext(targetBmpContext));
     CleanupStack::PushL(targetBmpContext);    
     targetBmpContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
     targetBmpContext->Clear();
     
     TSize screenSize = targetBmpDevice->SizeInPixels();
     
      TInt channelMask; 
      TInt channelMul;

      if (aChannelControl<1)
          {
          channelMask=0xFF0000;   
          }
      else
          {
          if(aChannelControl==1)
              {
              channelMask=0x00FF00;   
              }
          else
              {
              channelMask=0x0000FF;   
              }
          }
      channelMul=channelMask&0x01010101;
      TInt previousTargetMask = -1;
      const TInt otherMask=0xFFFFFF^channelMask;
         
      for (TInt targetMask=0;targetMask<256;targetMask++)
         {
         //bkgrdChannel is background channel input value. 
         for (TInt targetChannel=0;targetChannel<=targetMask;targetChannel++) 
             {
             if(targetChannel>(targetMask-10)) // Optimise the loop to test values specific for this defect
                 {
                 TInt failsPerPass=10;

                 //srcMask is the source mask/alpha
                 for (TInt srcMask=0;srcMask<256;srcMask++)
                     {  
                     if((srcMask<=10)||(srcMask>=250)) // Optimise the loop to test values specific for this defect
                         {
                         TReal step;
                         if (aChannelControl == 1)
                             {
                             step = 255.0/63;  // Only for blue channel for EColor64K source colour
                             }
                         else
                             {
                             step = 255.0/31;                     
                             }
                         TReal srcChannelReal = 0.0;
                         for (TInt srcChannel=0;srcChannel<256;srcChannel=(TInt)srcChannelReal)
                             {
                             if (targetMask != previousTargetMask)
                                 {
                                 //uncomment it to print log for each targetMask loop
                                 //INFO_PRINTF2(_L("Processing source colours for targetMask=%i"),targetMask);
                                 previousTargetMask = targetMask;
                                 }
                             srcChannelReal += step;
                            
                             //test blending of one-pixel bitmap
                             DoMaskTestL(srcChannel, srcMask, targetMask, targetChannel, aChannelControl, srcBmp, maskBmp, targetBmp, targetBmpContext);

                             TBuf8<4> readBuffer;
                             TUint* readPixel = (TUint*)readBuffer.Ptr();
                             targetBmp->GetScanLine(readBuffer, TPoint(0, 0), 1, EColor16MAP); 
                             
                             //channelPixelValue is the channel pixel value as generated from the library code under test
                             TUint channelPixelValue=((*readPixel)&channelMask)/channelMul;
                             //alphaPixelValue is the alpha pixel value as generated from the library code under test
                             TUint alphaPixelValue = (*readPixel)>>24; 
                             
                             //check if channelPixelValue and alphaPixelValue match the expected ones
                             CheckValues(alphaPixelValue, channelPixelValue, failsPerPass, targetMask, targetChannel, srcMask, srcChannel, otherMask, readPixel);
                             } 
                         }
                     }
                     if (failsPerPass<0) 
                         {       //note that this count may be out by 1... 
                         INFO_PRINTF2(_L("Additional %i errors not reported in this pass."),-failsPerPass); 
                         }                                    
                 }
             }
         }         
     CleanupStack::PopAndDestroy(5);      
     }

void CTDefect2::CheckValues(TUint aAlphaPixelValue, TUint aChannelPixelValue, TInt& aFailsPerPass, TInt aTargetMask, TInt aTargetChannel, TInt aSrcMask, TInt aSrcChannel, TInt aOtherMask, TUint* aReadPixel)
   {
   const TReal KGross=1.95;
   TReal srcMultiplier;
   TReal alphaOutputValue;
   //source multiplier factor for alpha that can then be used to optimise non-multiplied input calculations.
   srcMultiplier=aSrcMask/255.0;
   //destination/background multiplier factor for alpha that can then be used to optimise non-multiplied input calculations.
   TReal destMultiplier=(aTargetMask/255.0)*(1.0-srcMultiplier);
   //alphaOutputValue is a floating-point calculation of the alpha output value using 255.0 as the scaling factor.
   alphaOutputValue=(srcMultiplier+destMultiplier)*255.0;

   //alphaDiff is the difference in alpha between pixel and float calcuation, i.e. the error. This can be less than 1 level of brightness, i.e. insignificant.
   TReal alphaDiff=0.0;                             
   alphaDiff=alphaOutputValue-aAlphaPixelValue;
   if (alphaDiff>KGross || alphaDiff<-KGross)
       {
       if (--aFailsPerPass>0)
           {
           LogColourEvent(aTargetMask,aTargetChannel,aSrcMask,-1,alphaOutputValue,aAlphaPixelValue,alphaDiff,_L("Big Alpha error: expected %f, got %f"),ETrue);                                     
           }                             
       }      
   //channelOutputValue is a floating-point calculation of the channel output value using 255.0 as the scaling factor.
   TReal channelOutputValue=aSrcChannel*srcMultiplier; 
   channelOutputValue+=aTargetChannel*(1.0-srcMultiplier);  
   
   if (aChannelPixelValue>aAlphaPixelValue)
       {
       if (--aFailsPerPass>0)  
           {
           LogColourEvent(aTargetMask,aTargetChannel,aSrcMask,aSrcChannel,aAlphaPixelValue,channelOutputValue,aChannelPixelValue,_L("Output multiplied colour exceeds alpha %f: expected %f got %f"),EFalse);                             
           INFO_PRINTF1(_L("channelPixelValue>alphaPixelValue"));                                 
           }               
       }  

   if((aOtherMask&(*aReadPixel))!=0)
       {
       if (--aFailsPerPass>0)  
           {
           LogColourEvent(aTargetMask,aTargetChannel,aSrcMask,aSrcChannel,aAlphaPixelValue,channelOutputValue,aChannelPixelValue,_L("Other colours damaged - NOT Zero"),EFalse);                             
           }               
       }  
       
   //channelDiff is the difference in channel between pixel and float calcuation, i.e. the error. This can be less than 1 level of brightness, i.e. insignificant.
    TReal channelDiff=channelOutputValue-aChannelPixelValue;

    if (channelDiff>KGross || channelDiff<-KGross)
        {
        if (--aFailsPerPass>0)
            {
            LogColourEvent(aTargetMask,aTargetChannel,aSrcMask,aSrcChannel,channelOutputValue,aChannelPixelValue,channelDiff,_L("Big Colour error: expected %f, got %f"),EFalse);      
            }                          
        }                                   
   }

/**
Test of Blending function DrawBitmapMasked(). 
@param aSrcChannel source channel 
@param aSrcMask source mask
@param aTargetMask target mask
@param aTargetChannel target channel
@param aChannelControl which channel is controlled  (0=red, 1= green, 2=blue)
@param aSrcBmp source bitmap
@param aMaskBmp mask bitmap
@param aTargetBmp target bitmap
@param aTargetBmpContext gc for target bitmap 
**/ 
void CTDefect2::DoMaskTestL(TInt aSrcChannel, TInt aSrcMask, TInt aTargetMask, TInt aTargetChannel, TInt aChannelControl, 
       CFbsBitmap* aSrcBmp, CFbsBitmap* aMaskBmp, CFbsBitmap* aTargetBmp, CBitmapContext* aTargetBmpContext)
   {
   const TSize size=TSize(1, 1);
   const TRect destRect=TRect(size);  
   const TRect srcRect=TRect(size);

   const TInt scanLineLengthEColor16MAP = CFbsBitmap::ScanLineLength(1, EColor16MAP);
   HBufC8* targetBuffer = HBufC8::NewL(scanLineLengthEColor16MAP);
   CleanupStack::PushL(targetBuffer);
   TPtr8 targetDes = targetBuffer->Des();
   targetDes.SetLength(scanLineLengthEColor16MAP);
   TUint32* targetP = (TUint32*)targetDes.Ptr();
   TRgb targetColour(0, 0);
   if (aChannelControl<1)
       {
       targetColour.SetRed(aTargetChannel);                  
       }
   else
       {
       if (aChannelControl==1)
           {
           targetColour.SetGreen(aTargetChannel);                                        
           }
       else
           {
           targetColour.SetBlue(aTargetChannel);                                        
           }
       }
   targetColour.SetAlpha(aTargetMask);
   *targetP = targetColour.Internal();
   aTargetBmp->SetScanLine(targetDes, 0);
   
   const TInt scanLineLengthEGray256 = CFbsBitmap::ScanLineLength(1, EGray256);
   HBufC8* maskBuffer = HBufC8::NewL(scanLineLengthEGray256);
   CleanupStack::PushL(maskBuffer);
   TPtr8 maskDes = maskBuffer->Des();
   maskDes.SetLength(scanLineLengthEGray256);
   TUint8* maskP = (TUint8*)maskDes.Ptr();
   *maskP = aSrcMask;
   aMaskBmp->SetScanLine(maskDes, 0);

   const TInt scanLineLengthEColor64K = CFbsBitmap::ScanLineLength(1, EColor64K);
   HBufC8* srcBuffer = HBufC8::NewL(scanLineLengthEColor64K);
   CleanupStack::PushL(srcBuffer);
   TPtr8 srcDes = srcBuffer->Des();
   srcDes.SetLength(scanLineLengthEColor64K);
   TUint16* srcP = (TUint16*)srcDes.Ptr();
   TRgb srcColour(0);
   if (aChannelControl<1)
       {
       srcColour.SetRed(aSrcChannel);
       }
   else
       {
       if (aChannelControl==1)
           {
           srcColour.SetGreen(aSrcChannel);
           }
       else
           {
           srcColour.SetBlue(aSrcChannel);
           }
       }
   
   *srcP = srcColour.Color64K();
   aSrcBmp->SetScanLine(srcDes, 0);
   aTargetBmpContext->DrawBitmapMasked(destRect, aSrcBmp, srcRect, aMaskBmp, EFalse);
   
   CleanupStack::PopAndDestroy(3);                                        
   }

/**  
  @SYMDEF PDEF141192, INC140310         

  @SYMTestCaseDesc Test that the alpha merge method CFbsBitGc::DrawBitmapMasked() works accurately.
                   
  @SYMTestPriority normal

  @SYMTestStatus Implemented

  @SYMTestActions Create an alpha capable bitmap and throw sample values at the merge function

  @SYMTestExpectedResults
  1a)     The merge resultant alpha values do not wrap through 256
  1b)     The merge resultant alpha values closely match an equivalent floating-point calculation
  2a)     The merge resultant colours do not wrap through 256
  2b)     The merge resultant colours do not exceed the resultant alpha if premultiplied output is expected
  2c)     The merge resultant colours closely match an equivalent floating-point calculation
  2d)     The other resultant colour values are not affected (stay black) 
  @param  aChannelControl which channel is controlled  (0=red, 1= green, 2=blue)
  
   The input tested format is EColor64K, but could easily be changesd to test other formats.
   The input mask is EGray256. If optimised internally, this could be changed to EGray16.
   The tested output format is EColor16MAP. This should not be changed. 

   This test takes up to 40 minutes to run per channel control. Therefore, it is only kept for local reference, not scheduled for ONB. 
   An optimised version CTDefect2::TestMaskForSelectedValuesL(TInt aChannelControl) is run instead for selected combinations of channel and mask values. 
*/
void CTDefect2::TestMaskForAllCombinationL(TInt aChannelControl)
   {
   const TSize size=TSize(1, 1);

   // Bitmap creation
   CFbsBitmap* srcBmp=new(ELeave) CFbsBitmap();
   CleanupStack::PushL(srcBmp); 
   User::LeaveIfError(srcBmp->Create(size, EColor64K)); 
   CFbsBitmapDevice* sourceBmpDevice=CFbsBitmapDevice::NewL(srcBmp);
   CleanupStack::PushL(sourceBmpDevice);
   CBitmapContext* sourceBmpContext;
   User::LeaveIfError(sourceBmpDevice->CreateBitmapContext(sourceBmpContext));
   CleanupStack::PushL(sourceBmpContext);    
   sourceBmpContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
   sourceBmpContext->Clear();
   CleanupStack::PopAndDestroy(2, sourceBmpDevice); // sourceBmpContext, sourceBmpDevice
   
   // Mask creation
   CFbsBitmap* maskBmp=new(ELeave) CFbsBitmap();
   CleanupStack::PushL(maskBmp); 
   User::LeaveIfError(maskBmp->Create(size, EGray256)); 
   CFbsBitmapDevice* maskBmpDevice=CFbsBitmapDevice::NewL(maskBmp);
   CleanupStack::PushL(maskBmpDevice);
   CBitmapContext* maskBmpContext;
   User::LeaveIfError(maskBmpDevice->CreateBitmapContext(maskBmpContext));
   CleanupStack::PushL(maskBmpContext);    
   maskBmpContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
   maskBmpContext->Clear();
   CleanupStack::PopAndDestroy(2, maskBmpDevice); // maskBmpContext, maskBmpDevice
        
   // Target Bitmap
   CFbsBitmap* targetBmp=new(ELeave) CFbsBitmap();
   CleanupStack::PushL(targetBmp); 
   User::LeaveIfError(targetBmp->Create(size, EColor16MAP)); 
   CFbsBitmapDevice* targetBmpDevice=CFbsBitmapDevice::NewL(targetBmp);
   CleanupStack::PushL(targetBmpDevice);
   CBitmapContext* targetBmpContext;
   User::LeaveIfError(targetBmpDevice->CreateBitmapContext(targetBmpContext));
   CleanupStack::PushL(targetBmpContext);    
   targetBmpContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
   targetBmpContext->Clear();
   
   TSize screenSize = targetBmpDevice->SizeInPixels();
   
    TInt channelMask; 
    TInt channelMul;

    if (aChannelControl<1)
        {
        channelMask=0xFF0000;   
        }
    else
        {
        if(aChannelControl==1)
            {
            channelMask=0x00FF00;   
            }
        else
            {
            channelMask=0x0000FF;   
            }
        }
    channelMul=channelMask&0x01010101;
    TInt previousTargetMask = -1;
    const TInt otherMask=0xFFFFFF^channelMask;
       
    for (TInt targetMask=0;targetMask<256;targetMask++)
       {
       //bkgrdChannel is background channel input value. 
       for (TInt targetChannel=0;targetChannel<=targetMask;targetChannel++) 
           {
           TInt failsPerPass=10;

            //srcMask is the source mask/alpha
           for (TInt srcMask=0;srcMask<256;srcMask++)
               {  
               TReal step;
               if (aChannelControl == 1)
                   {
                   step = 255.0/63; // Only for blue channel for EColor64K source colour
                   }
               else
                   {
                   step = 255.0/31;                     
                   }
               TReal srcChannelReal = 0.0;

               for (TInt srcChannel=0;srcChannel<256;srcChannel=(TInt)srcChannelReal)
                   {
                   if (targetMask != previousTargetMask)
                       {
                       INFO_PRINTF2(_L("Processing source colours for targetMask=%i"),targetMask);
                       previousTargetMask = targetMask;
                       }
                   srcChannelReal += step;
                   
                   //test blending of one-pixel bitmap
                   DoMaskTestL(srcChannel, srcMask, targetMask, targetChannel, aChannelControl, srcBmp, maskBmp, targetBmp, targetBmpContext);
                   
                   TBuf8<4> readBuffer;
                   TUint* readPixel = (TUint*)readBuffer.Ptr();
                   targetBmp->GetScanLine(readBuffer, TPoint(0, 0), 1, EColor16MAP);                

                   //channelPixelValue is the channel pixel value as generated from the library code under test
                   TUint channelPixelValue=((*readPixel)&channelMask)/channelMul;
                   //alphaPixelValue is the alpha pixel value as generated from the library code under test
                   TUint alphaPixelValue = (*readPixel)>>24; 
                   
                   //check if channelPixelValue and alphaPixelValue match the expected ones
                   CheckValues(alphaPixelValue, channelPixelValue, failsPerPass, targetMask, targetChannel, srcMask, srcChannel, otherMask, readPixel);
                    
               if (failsPerPass<0) 
                   {       //note that this count may be out by 1... 
                   INFO_PRINTF2(_L("Additional %i errors not reported in this pass."),-failsPerPass); 
                   }                  
               } 
           } 
           }
       }         
   CleanupStack::PopAndDestroy(5);      
   }

/**
This function is used to write to the log file
@param  aNonPreMulDestPixColor non pre multiplied destination pixel colour
@param  aPreMulSrcPixelColor pre multiplied source pixel colour
@param  aNonPreMulSrcPixelColor non pre multiplied source pixel colour
@param  aVal1 it contains the value of the first variable of the message to be displayed
@param  aVal2 it contains the value of the second variable of the message to be displayed
@param  aVal3 it contains the value of the third variable of the message to be displayed
@param  aMsg it contains the message to be printed to the log file
@param  aErr if true then the test case fails, if false test passes. log is printed in both the case. TEST does not abort, just reports test case failure
*/
void CTDefect2::LogColourEvent(TInt aPreMulDestPixColor,TInt aNonPreMulDestPixColor,TInt aPreMulSrcPixelColor,TInt aNonPreMulSrcPixelColor,TReal aVal1,TReal aVal2,TReal aVal3,TRefByValue<const TDesC> aMsg,TBool aErr)
   {
   TEST(aErr==EFalse); // if aErr=True, then the previous test step failed.
   INFO_PRINTF4(aMsg,aVal1,aVal2,aVal3);
   if (aNonPreMulSrcPixelColor<0)
       {
       INFO_PRINTF4(_L("Processing source colours for MDest=%i, CDest=%i, MSrc=%i"),aPreMulDestPixColor,aNonPreMulDestPixColor,aPreMulSrcPixelColor);
       }
   else
       {
       INFO_PRINTF5(_L("Processing colour set MDest=%i, CDest=%i, MSrc=%i, CSrc=%i"),aPreMulDestPixColor,aNonPreMulDestPixColor,aPreMulSrcPixelColor,aNonPreMulSrcPixelColor);
       }
   }

class TFunctionThread
	{
protected:
	TFunctionThread():iExitHow(ENotStarted)	
		{}
	TInt LaunchThreadFunction(const TDesC& aName);
private:
	static TInt TheThreadFunction(TAny*);
	virtual TInt	ThreadFunctionL()=0;
public:
	enum {
		ENotStarted,
		ERunning,	//should never see this
		EReturn,
		ELeave,
		EPanic,
		ETerminate,
		};
	TInt	iExitHow;
	TInt	iExitCode;	//Currently don't store the panic category string.
	};

TInt TFunctionThread::LaunchThreadFunction(const TDesC& aName)
	{
	RThread thrd;
	TRequestStatus stat;
	enum { //8kb to 2mb
		KMinHeapSize=0x2000,
		KMaxHeapSize=0x200000
		};
	TInt created=thrd.Create(aName,TheThreadFunction,KDefaultStackSize,KMinHeapSize,KMaxHeapSize,this);
	if (created<KErrNone)
		{
		iExitCode=created;
		return created;
		}
	thrd.SetPriority(EPriorityMuchMore);
	thrd.Logon(stat);
	User::SetJustInTime(EFalse);
	thrd.Resume();
	User::WaitForRequest(stat);
	if ( iExitHow!=ENotStarted || iExitHow==ERunning )
		{
		iExitCode=thrd.ExitReason();
		switch (thrd.ExitType())
			{
			case EExitKill:			iExitHow=EReturn;		break;
			case EExitPanic:		iExitHow=EPanic;		break;
			case EExitTerminate:	iExitHow=ETerminate;	break;
			default:
				ASSERT(EFalse);
			}
		}
	thrd.Close();
	User::SetJustInTime(ETrue);
	return KErrNone;
	}

TInt TFunctionThread::TheThreadFunction(TAny* aThis)
	{
	TFunctionThread* thisThis=(TFunctionThread*)aThis;
	if (thisThis==NULL)
		{
		User::Panic(_L("NoThis"),0x1);
		}
	TInt leaveErr = RFbsSession::Connect();
	if (leaveErr != KErrNone)
		{
		thisThis->iExitHow = ELeave;
		thisThis->iExitCode = leaveErr;
		return leaveErr;
		}
	thisThis->iExitHow=thisThis->ERunning;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt returnErr=KErrNone;
	TRAP(leaveErr, returnErr = thisThis->ThreadFunctionL());
	if (leaveErr)
		{
		thisThis->iExitHow=ELeave;
		thisThis->iExitCode=leaveErr;
		delete cleanup;
		RFbsSession::Disconnect();
		return leaveErr;
		}
	else
		{
		thisThis->iExitHow=EReturn;
		thisThis->iExitCode=returnErr;
		delete cleanup;
		RFbsSession::Disconnect();
		return returnErr;
		}
	}

/** This thread verifies whether a range of memory is accessible 
  The range is read sequentially until it panics, or the range is completed.
  It is useful to input a range of addresses where some are valid and some fail 
  in order to demonstrate an edge against which an algorithm that performs illegal reads can subsequently be tested.
  The FailOffset() returned index indicates the offset from the start at which the memory access caused a panic. 
 **/
class TTestMemThread:public TFunctionThread
	{
public:
	TTestMemThread(TUint32* aStartAddress,TUint32* aEndAddress);
	TInt FailOffset();
private:
	virtual TInt	ThreadFunctionL();
private:
	TUint32* iStartAddress;
	TUint32* iEndAddress;
	volatile TUint32* iLastAddressTried;
	volatile TUint32  iCopyValueHere;
	
	};

TTestMemThread::TTestMemThread(TUint32* aStartAddress,TUint32* aEndAddress):
	iStartAddress(aStartAddress),
	iEndAddress(aEndAddress),
	iLastAddressTried(NULL)
	{
	ASSERT(aStartAddress);
	ASSERT(aEndAddress);
	LaunchThreadFunction(_L("MemTest"));
	}

/**
 *	Returns the number of successful memory reads before a panic occurred
 * 	Or various error codes if the test didn't run or didn't panic.
 * 
 **/
TInt TTestMemThread::FailOffset()
	{
	if (iExitHow==EReturn)
		{
		return KErrCompletion;
		}
	else
		{
		if (iExitHow==EPanic)
			{
			if (iLastAddressTried)
				{
				TInt retval=iLastAddressTried-iStartAddress;
				if (iEndAddress-iStartAddress<0)
					{
					retval=-retval;
					}
				if (retval<0)
					{
					return KErrCorrupt;
					}
				else
					{
					return retval;
					}
				}
			else
				{
				return KErrNotFound;
				}
			}
		else
			{
			return -iExitHow*100; 
			}
		}
	}
/*
 * This thread function allows a test to verify that a particular address range 
 * is actually inaccessable.
 * I.e. that attempting to read from the given address range causes a panic.
 */
TInt TTestMemThread::ThreadFunctionL()
	{
	if (iStartAddress && iEndAddress)
		{
		TInt delta=1;
		if (iStartAddress>iEndAddress)
			{
			delta=-1;
			}
		for (TUint32 volatile* tryAddress=iStartAddress;tryAddress!=iEndAddress;tryAddress+=delta)
			{
			iLastAddressTried=tryAddress;
			iCopyValueHere=*tryAddress;
			}
		return 0;
		}
	return KErrArgument;
	}

/** This thread verifies the function of CopyRect
  A series of operations are performed on the given bitmap.
  If the operation panics the launcher is able to retrieve the parameters that caused the panic and report
  The FailOffset() returned index indicates the offset from the start at which the memory access caused a panic. 
 **/
class TTestCopyRectThread:public TFunctionThread
	{
public:
	TTestCopyRectThread(CFbsBitGc* aBitmapContext,TInt aStartX,TInt aStopX,TInt aY1,TInt aY2,TPoint aOffset);
private:
	virtual TInt	ThreadFunctionL();
private:
	CFbsBitGc* iBitmapContext;
	TInt iStartX;
	TInt iStopX;
	TInt iY1;
	TInt iY2;
	TPoint iOffset;
public:
	volatile TInt iCurrX;
	volatile TInt iCurrWidth;
	
	};

TTestCopyRectThread::TTestCopyRectThread(CFbsBitGc* aBitmapContext,TInt aStartX,TInt aStopX,TInt aY1,TInt aY2,TPoint aOffset):
	iBitmapContext(aBitmapContext),
	iStartX(aStartX),
	iStopX(aStopX),
	iY1(aY1),
	iY2(aY2),
	iOffset(aOffset)
	{
	LaunchThreadFunction(_L("CopyRectTest"));
	}
TInt	TTestCopyRectThread::ThreadFunctionL()
	{
	for (iCurrX=iStartX;iCurrX<iStopX;iCurrX++)
		{
		for (iCurrWidth=iStopX-iCurrX;iCurrWidth>0;iCurrWidth--)
			{
			iBitmapContext->CopyRect(iOffset,TRect(iCurrX,iY1,iCurrX+iCurrWidth,iY2));
		    }
		}
	return KErrNone;
}

class CTestExecuteLogger;

struct WrapCTestExecuteLogger
	{
	WrapCTestExecuteLogger():	iLogger(NULL)	{}
	WrapCTestExecuteLogger(CTestExecuteLogger&aLogger):	iLogger(&aLogger)	{}
	WrapCTestExecuteLogger(CTestExecuteLogger*aLogger):	iLogger(aLogger)	{}
	
	operator int()	{return (iLogger!=NULL);}
	CTestExecuteLogger& operator()() { return *iLogger;	}

	CTestExecuteLogger* iLogger;
	
	};
static const TInt KCopyTestHeight=20;
static const TInt KCopyTestWidth=256;	//should be some multiple of 32...
/** Compares the pixels in the source rectangle and its copy.
 * 	The copy is at an offset from the source.
 * 	The compare is not complete as this was found to be too slow.
 *  Instead only a few pixels on the left and right edges are compared.
 * 
 **/
TBool CmpBitmapRectL(CFbsBitmap& aTestBitmap,TRect aSource,TPoint aTrgOffset,WrapCTestExecuteLogger Logger=NULL,TBool aExpectedPass=EFalse)
	{
	//Comparing all the pixels in a large rectangle using GetPixel() is too slow. 
	//So now, only a few pixels on the edges of the rectangle are compared, 
	//and the ones in between are skipped.
	//Using any faster method to get the pixels is dubious 
	//as it it may ultimately use GetLine, which is the method under test...
	const TInt processSize=6;
	TBool returnState=ETrue;
	TUint16 printcompareresultbuffers[KCopyTestHeight][2][processSize*2+1]={{{0}}};
	TInt resultRow=KCopyTestHeight-aSource.Height();
	TInt skipMiddleStart=aSource.iTl.iX+processSize;
	TInt skipMiddleEnd=aSource.iBr.iX-processSize-1;
	if (skipMiddleStart>=skipMiddleEnd)
		{
		skipMiddleStart+=processSize+processSize;	//make skip irrelivent
		}
	//verify inputs valid 
	TRect bitmapRect=aTestBitmap.SizeInPixels();
	TRect sourceRectIntersect=aSource;
	TRect targetRect=aSource;
	targetRect.Move(aTrgOffset);
	TRect targetRectIntersect=targetRect;
	targetRectIntersect.Intersection(bitmapRect);
	sourceRectIntersect.Intersection(bitmapRect);
	ASSERT(sourceRectIntersect==aSource);
	ASSERT(targetRectIntersect==targetRect);
	
	for (TInt sy=aSource.iTl.iY;sy<aSource.iBr.iY;sy++)
		{
		TInt resultCol=0;
		for (TInt sx=aSource.iTl.iX;sx<aSource.iBr.iX;sx++)
			{
			if (sx==skipMiddleStart)
				{
				sx=skipMiddleEnd;
				}
			else
				{
				TRgb sourceColor;
				aTestBitmap.GetPixel(sourceColor,TPoint(sx,sy));
				TRgb targetColor;
				aTestBitmap.GetPixel(targetColor,TPoint(sx,sy)+aTrgOffset);
				if (sourceColor!=targetColor)
					{
					if (Logger)
						{
						returnState=EFalse;
						}
					else
						{
						return EFalse;
						}
					}
				if (Logger&&resultRow>=0)
					{
					printcompareresultbuffers[resultRow][0][resultCol]=L'@'+sourceColor.Color16(); 
					printcompareresultbuffers[resultRow][1][resultCol]=L'@'+targetColor.Color16(); 
					}
				}
				
			}
		resultRow++;
		}
	if (Logger && returnState!=aExpectedPass)
		{
		INFO_PRINTF4(_L("Case: srcx=%i targx=%i width=%i result compare has failed! Content dump follows: [src] [trg] "),
				aSource.iTl.iX,targetRect.iTl.iX,aSource.Width());
		for (TInt row=(KCopyTestHeight-aSource.Height()>0)?KCopyTestHeight-aSource.Height():0;row<KCopyTestHeight;row++)
			{
			INFO_PRINTF5(_L("Row: %2i %s  %s (%s)"),
					row,printcompareresultbuffers[row][0],printcompareresultbuffers[row][1],
					Mem::Compare(printcompareresultbuffers[row][0],processSize*2,printcompareresultbuffers[row][1],processSize*2)
						?L"differ":L"same"
						);
			
			}
		}
	return returnState;
	}

/**
 * @SYMTestCaseID GRAPHICS-BITGDI-0099
 *
 * @SYMDEF INC119063, pdef120353
 *
 * @SYMTestCaseDesc CopyRect could read bytes past the end of the source bitmap
 * 					This is a general test of the rewrite of CopyRect
 *					Note that this test says well within the boundaries of the bitmap,
 * 					and does not test the exception condition detected in the defect.
 * @SYMTestPriority Normal
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions 
 * 				Repeat the test for 4 affected bitmap resolutions: Gray2, Gray4, Gray16, Color16
 * 				Allocate a bitmap big enough to process the test
 * 				Repeat:
 * 					Fill the whole image with random data
 * 					Pixel compare the source rectangle wiith the target rectangle
 * 						- they should NOT be identical - very unlucky!
 * 					Pixel compare the left and right edge pixels outside the copy rect
 * 						- these should not be all identical - very unlucky!
 * 					Copyrect a region from the bottom half to the top half (not to edges)
 * 					Pixel compare the source rectangle wiith the target rectangle
 * 						- they should be identical - copy was successful
 * 					Pixel compare the left and right edge pixels outside the copy rect
 * 						- these should not be all identical - or too much was copied
 * 				Repeat for values of target x 0 to 32
 * 				Repeat for values of source x 0 to 32
 * 				Repeat for values of source/target width 1 to 96
 * 				Guess: 4x32x32x96 tests=400000 iterations
*/	
void CTDefect2::ExerciseCopyRect()
	{
	for (TInt mode=1;mode<EColorLast;mode++)
		{
		CFbsBitmap testBitmap;

		//The exact dimensions of this bitmap are not important...
		//The width should ensure that the range of tests work (max 32+32+96=160 pixels)
		//The height should be enough to ensure there is enough variability in the random data
		//Note that the smallest data used in a uniqueness test is a 1-pixel-width column half the height of the image. 
		//The whole height will be used if this is changed.
		if (testBitmap.Create(TSize(KCopyTestWidth,KCopyTestHeight*2),TDisplayMode(mode))<KErrNone)
			{
			continue;	//some mode numbers are bad!
			}
		
		
		SEpocBitmapHeader header1 = testBitmap.Header();
		//We could run this test for other modes, but the defect and fix only applies to low bit depths
		if (header1.iBitsPerPixel>4)
		{
		continue;	
		}
		INFO_PRINTF3(_L("Bitmap color depth: %i bits. Color flag: %i"),header1.iBitsPerPixel,header1.iColor);
   
 
		TUint32* bitmapData=testBitmap.DataAddress();
		TUint32* pastBitmapData=bitmapData+((header1.iBitmapSize-header1.iStructSize)>>2);

		TInt splitHeight=header1.iSizeInPixels.iHeight/2;
   
		CFbsBitGc* bitmapContext = NULL;
		CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(&testBitmap);
		CleanupStack::PushL(bitmapDevice);
		User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));
		CleanupStack::PushL(bitmapContext);
		TInt skippedCases=0;
		TInt totalCases=0;
		//These counts have been tweaked based on knowledge of the algorithm.
		//The target position is shifted in bits by up to 1 word (32 bits)
		//Bigger changes than this just change the target pointer and modulo the shift.
		//[I don't think that the target position influences the result, but it might]
		//The source position is shifted in bits by up to 1 word (32 bits)
		//Bigger changes than this just change the source pointer and modulo the shift.
		//The copy width is made up of a copy words element and a remainder bytes element.
		//The difference in shift element between source and target is used to shift the 
		//source to a temporary buffer.
		//So the copy width element is also influenced by the source position and target position.
		//Testing 96 possible bit widths should exercise it for zero and non-zero word counts adequately.
		const TInt
			KTargXMin=32,
			KTargXMax=32+32/header1.iBitsPerPixel,
			KSrcXMin=32,
			KSrcXMax=32+32/header1.iBitsPerPixel,
			KWidthMin=1,
			KWidthMax=96/header1.iBitsPerPixel;
		for (TInt targx=KTargXMin;targx<KTargXMax;targx++)
			{	
			for (TInt srcx=KSrcXMin;srcx<KSrcXMax;srcx++)
				{
				for (TInt width=KWidthMin;width<KWidthMax;width++)
					{
					totalCases++;
					TInt offsetx=targx-srcx;
					//fill the buffer with random data
					TInt64 randomSeed=MAKE_TINT64(0xdeadbeef,0xbaadf00d);
					Math::Rand(randomSeed);
					TInt maxRetries=3;
					TInt rndCount=0;
					while (--maxRetries>=0)
						{
						for (TUint32* fillData=bitmapData;fillData!=pastBitmapData;fillData++)
							{
							rndCount++;
							//Rand only sets 31 bits of the integer - no negative values - no top bit set! 
							//This solution wastes some of the seed, but it should still produce valid results. 
							TInt rnd=Math::Rand(randomSeed)^(Math::Rand(randomSeed)<<8);
							*fillData=rnd;
							}
						TInt colOffSrc=0;
						TInt colOffTrg=0;
						//verify that the data is different inside the copy area and outside each copy area
						if (TInt failCode=	CmpBitmapRectL(testBitmap,TRect(srcx,splitHeight,srcx+width,splitHeight*2),TPoint(offsetx,-splitHeight))
							+	2*CmpBitmapRectL(testBitmap,TRect(srcx-1,splitHeight,srcx,splitHeight*2),TPoint(offsetx,-splitHeight))
							+ 	4*CmpBitmapRectL(testBitmap,TRect(srcx+width,splitHeight,srcx+width+1,splitHeight*2),TPoint(offsetx,-splitHeight))
							)
						   {
						   //OK.. need to find another pair of bits where the source and target columns are non-identical
						   INFO_PRINTF5(_L("First random data failed... (code %i) Trying alternale columns: srcx=%i targx=%i width=%i"),failCode,srcx,targx,width);
						   for (TInt tryOffSrc=0;tryOffSrc+srcx<KCopyTestWidth-(KWidthMax+1) && (colOffSrc==0&&colOffTrg==0);tryOffSrc+=32)
							   for (TInt tryOffTrg=0;tryOffTrg+targx<KCopyTestWidth-(KWidthMax+1) && (colOffSrc==0&&colOffTrg==0);tryOffTrg+=32)
								   {
								   TInt offsetx=(tryOffTrg+targx)-(tryOffSrc+srcx);
								   TInt trysrcx=tryOffSrc+srcx;
								   if (	CmpBitmapRectL(testBitmap,TRect(trysrcx,splitHeight,trysrcx+width,splitHeight*2),TPoint(offsetx,-splitHeight))
									|	CmpBitmapRectL(testBitmap,TRect(trysrcx-1,splitHeight,trysrcx,splitHeight*2),TPoint(offsetx,-splitHeight))
									|	CmpBitmapRectL(testBitmap,TRect(trysrcx+width,splitHeight,trysrcx+width+1,splitHeight*2),TPoint(offsetx,-splitHeight))
									)
									   {
									   }
								   else
									   {
									   colOffSrc=tryOffSrc;
									   colOffTrg=tryOffTrg;
									   }
								   }
						   if (colOffSrc||colOffTrg)
							   {
							   offsetx=(colOffTrg+targx)-(colOffSrc+srcx);
							   INFO_PRINTF4(_L("Found a safe equivalent column:  srcx=%i targx=%i width=%i"),colOffSrc+srcx,colOffTrg+targx,width);
							   }
						   else
							   {
							   INFO_PRINTF4(_L("Didn't find a safe column! Trying next random numbers"),srcx,targx,width);
							   continue;	//Try next random number...
							   }
						   }
						TInt trysrcx=colOffSrc+srcx;
						//Copy the region in question
						bitmapContext->CopyRect(TPoint(offsetx,-splitHeight), TRect(trysrcx,splitHeight,trysrcx+width,splitHeight*2));
						//verify the copied region is an exact match
						TBool copySame=CmpBitmapRectL(testBitmap,TRect(trysrcx,splitHeight,trysrcx+width,splitHeight*2),TPoint(offsetx,-splitHeight));
						//verify the area outside has not been copied
						TBool leftSame=CmpBitmapRectL(testBitmap,TRect(trysrcx-1,splitHeight,trysrcx,splitHeight*2),TPoint(offsetx,-splitHeight));
						TBool rightSame=CmpBitmapRectL(testBitmap,TRect(trysrcx+width,splitHeight,trysrcx+width+1,splitHeight*2),TPoint(offsetx,-splitHeight));
						if (!copySame||leftSame||rightSame)
							{
							INFO_PRINTF4(_L("Case: srcx=%i targx=%i width=%i result compare has failed!"),srcx,targx,width);
							if (!copySame)
								{
								INFO_PRINTF1(_L("Copied data is not same as source data!"));
								}
							if (leftSame)
								{
								INFO_PRINTF1(_L("Data to left of copy is now same as source data!"));
								}
							if (rightSame)
								{
								INFO_PRINTF1(_L("Data to right of copy is now same as source data!"));
								}
							TEST(EFalse);
							}
						break;	//out of "try next random number" loop
						}
					if (maxRetries<0)
						{
						INFO_PRINTF4(_L("Case skipped TOO MANY TIMES because random data not unique: srcx=%i targx=%i width=%i."),srcx,targx,width);
						skippedCases++;
						}
					}
				}
			}
		CleanupStack::PopAndDestroy(bitmapContext);
	   	CleanupStack::PopAndDestroy(bitmapDevice);
	   	
	   	testBitmap.Reset();
		if (skippedCases)
			{
			INFO_PRINTF3(_L("%i / %i cases skipped because random picture data not unique enough! 10%% will cause test fail."),skippedCases,totalCases );
			TEST(skippedCases*10<totalCases);
			}
		}
	}
/**
 * @SYMTestCaseID GRAPHICS-BITGDI-0098
 *
 * @SYMDEF INC119063, PDEF120353
 *
 * @SYMTestCaseDesc CopyRect could previously read bytes past the end of the source bitmap
 * 					This would exception if the page is not mapped.
 * 					This particular image size allocation reproduces this exception.
 * 					Any changes to the allocation mechanism may mean the crash condition is not reproducable.
 * 					It is likely that it is changes to the allocator that have caused this defect to occur
 * 					in the first place.
 *
 * @SYMTestPriority Normal
 *
 * @SYMTestStatus Implemented
 *
 * @SYMTestActions Create 768x1280 1 bit image, (also 384x1280x2bit and 192x1280x4bit ?)
 * 			In a seperate thread try to read past the end of the data.
 * 			If this panics then the test is valid.
 * 			Now, make a number of copyrect calls 
 * 			that attempt to read data from near the end of the last scanline				   
 *			Expected result: these reads should not exception.
 * 			Enhancement to improve reproducability:
 * 			Occasionally, the "next location" is addressable when the test is run in the overnight script
 * 			To address this, the test will recurse and repeat, forcing the image allocation to a different address if this occurs
*/	
TBool CTDefect2::CopyRectReadOutsideBitmap(TInt aSingleMode /*=0*/,TInt aRetriesLeft /*=5*/)
	{
	TBool testShouldUltimatelyFail=EFalse;
	TInt startMode=aSingleMode?aSingleMode:1;
	TInt stopMode=aSingleMode?aSingleMode+1:EColorLast;
	for (TInt mode=startMode;mode<stopMode;mode++)
		{
		CFbsBitmap testBitmap;
		enum
			{
			EMonoPixelsWidth=768,
			EMonoBytesWidth=96,
			EMonoHeight=1280
			};
		TInt guessPixels=EMonoPixelsWidth;
		TInt guessBytes=CFbsBitmap::ScanLineLength(guessPixels,TDisplayMode(mode));
		if (guessBytes<=0 || reinterpret_cast<TDisplayMode&>(mode)==ERgb)
			{
			continue;	//mode number not good.
			}
		//want to generate a length of 768/8=96 bytes per scanline to repro the defect...
		while (guessBytes<EMonoBytesWidth)
			{
			if (guessBytes<=EMonoBytesWidth/2)
				{
				guessPixels*=2;
				}
			else
				{
				guessPixels++;
				}
			guessBytes=CFbsBitmap::ScanLineLength(guessPixels,TDisplayMode(mode));
			}
		while (guessBytes>EMonoBytesWidth)
			{
			if (guessBytes>=EMonoBytesWidth*2)
				{
				guessPixels>>=1;
				}
			else
				{
				guessPixels--;
				}
			guessBytes=CFbsBitmap::ScanLineLength(guessPixels,TDisplayMode(mode));
			}
		
		if (testBitmap.Create(TSize(guessPixels,EMonoHeight),TDisplayMode(mode))<KErrNone)
			{
			_LIT(KFail,"Failed to create bitmap for color depth: %S");
			INFO_PRINTF2(KFail,&ColorModeName(mode));
			if (aSingleMode)
				{
				testShouldUltimatelyFail=ETrue;
				}
			continue;	//some mode numbers are bad!
			}

		SEpocBitmapHeader header1 = testBitmap.Header();

		TUint32* bitmapData=testBitmap.DataAddress();
		TUint32* pastBitmapData=bitmapData+((header1.iBitmapSize-header1.iStructSize)>>2);

		//The defect and fix only applies to low bit depths, but it is quick enough to run it for all color depths
		if (aSingleMode)
			{
			User::After(1000000);	//Pause is in microseconds! //A small pause to try to get the bad operation completed
			INFO_PRINTF3(_L("Retry Bitmap: Address 0x%08x, past 0x%08x"),
					bitmapData,pastBitmapData		);
			
			}
		else
			{
			INFO_PRINTF7(_L("Bitmap: color depth: %i bits. Color mode= %i. Width=%i Height=%i Address 0x%08x, past 0x%08x"),
					header1.iBitsPerPixel,header1.iColor,
					header1.iSizeInPixels.iWidth,header1.iSizeInPixels.iHeight,
					bitmapData,pastBitmapData		);
			}
		TBool canGenerateDefectScenario=EFalse;

		
		//So now we hope that we have created a test bitmap on the bottom of a memory page,
		//as per the defect, but at different color depths as well.
		//Any number of factors could change the way this block is allocated and disguise the defect.
		//Equally, the next block of memory may be already mapped, and again the defect will not occurr.
		//There is no way to influence the result, so the next ting to do is to verify that an exception will occur.
		//Could assert that the address matches a 4K page, but I need to check if the page is mapped anyway!	
		TTestMemThread memThread(pastBitmapData-2,pastBitmapData+2);
		TInt failAt=memThread.FailOffset();
		if (failAt<0)
			{
			INFO_PRINTF2(_L("                 Scenario invalid - error code generated by test thread! %i"),failAt);
			}
		else if (failAt<=1)
			{
			INFO_PRINTF1(_L("                 Scenario invalid - didn't manage to read a safe location near the end of the bitmap!"));
			}
		else if (failAt>2)
			{
			INFO_PRINTF1(_L("                   Scenario invalid - managed to read right past the test location!"));
			}
		else
			{
			INFO_PRINTF1(_L("              Verified that end address is bad memory"));
			canGenerateDefectScenario=ETrue;
			}
		if (!canGenerateDefectScenario)
			{
			if (!aRetriesLeft || !CopyRectReadOutsideBitmap(mode,aRetriesLeft-1))	//repeat attempt with nested bitmap alocation to new address
				{
				//if this doesn't work out then the memory allocator is not like it was when the defect was raised!
				//so the test is no longer any good.
				//Can't return "Inconclusive" as other tests in the test set may reset the state.
				INFO_PRINTF2(_L("Error!!: Couldn't generate the defect scenario for the end address at 0x%08x."),(TInt)pastBitmapData);
				testShouldUltimatelyFail=ETrue;
				}
			}
		else
			{
			//These writes should succeed if the end of the bitmap is where it should be:
			//these colour values will be transferred in the copied data in the subsequent test
			//The test does not explicitly look for these patterns, 
			//but they are useful to identify in the copied data if manually debugging.
			pastBitmapData[-2]=0xBAADF00D;
			pastBitmapData[-1]=0xdeadbeef;

			CFbsBitGc* bitmapContext = NULL;
			CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(&testBitmap);
			CleanupStack::PushL(bitmapDevice);
			User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));
			CleanupStack::PushL(bitmapContext);
			//for the last scanline, try to copy various permutations of the last few pixels
			//note that there is no check to see if the "right" contents are copied, only that it doesn't abort.
			//In fact most of the bitmap is uninitialised...
			TTestCopyRectThread copythread(bitmapContext,guessPixels-64,guessPixels,EMonoHeight-5,EMonoHeight,TPoint(-5,-5));
			if (copythread.iExitHow!=copythread.EReturn)
				{
				testShouldUltimatelyFail=ETrue;
				INFO_PRINTF7(_L("Error: (probably panic) when using CopyRect(TPoint(%i,%i),TRect(%i,%i,%i,%i)"),
						-5,-5,
						copythread.iCurrX,EMonoHeight-5, copythread.iCurrX+copythread.iCurrWidth,EMonoHeight);
				_LIT(KLog,"Thread Exit Reason %d, expected %d");
				INFO_PRINTF3(KLog,copythread.iExitHow,copythread.EReturn);
				}

			CleanupStack::PopAndDestroy(bitmapContext);
			CleanupStack::PopAndDestroy(bitmapDevice);

			//For the purposes of verification, check that the memory location is still considered a valid scenario.
			//Again , the test is repeated nested if the scenario has become invalid
			TTestMemThread memThread2(pastBitmapData-2,pastBitmapData+2);
			TInt failAt=memThread2.FailOffset();
			canGenerateDefectScenario=EFalse;
			if (failAt<0)
				{
				INFO_PRINTF2(_L("                 After Test: Scenario invalid - error code generated by test thread! %i"),failAt);
				INFO_PRINTF1(_L("Warning: This may mean that bad copys may not have been detected??"));
				}
			else if (failAt<=1)
				{
				INFO_PRINTF1(_L("                 After Test: Scenario invalid - didn't manage to read a safe location near the end of the bitmap!"));
				INFO_PRINTF1(_L("Warning: This may mean that bad copys may not have been detected??"));
				}
			else if (failAt>2)
				{
				INFO_PRINTF1(_L("                  After Test: Scenario invalid - managed to read right past the test location!"));
				INFO_PRINTF1(_L("Warning: This may mean that bad copys may not have been detected??"));
				}
			else
				{
				INFO_PRINTF1(_L("              After Test: Verified that end address is still bad memory"));
				canGenerateDefectScenario=ETrue;
				}
			if (!canGenerateDefectScenario)
				{
				if (!aRetriesLeft || !CopyRectReadOutsideBitmap(mode,aRetriesLeft-1))	//repeat attempt with nested bitmap alocation to new address
					{
					//if this doesn't work out then the memory allocator is not like it was when the defect was raised!
					//so the test is no longer any good.
					//Can't return "Inconclusive" as other tests in the test set may reset the state.
					INFO_PRINTF2(_L("Warning!!: Couldn't maintain the defect scenario for the end address at 0x%08x after retries."),(TInt)pastBitmapData);
					INFO_PRINTF1(_L("Test may have read from end of bitmap after it was made accessible."));
					//Ultimately, this refuses to stabilise on ONB scenario, so I won't have this retry of the test fail.
					}
				}
				
			}
	   	testBitmap.Reset();
		}
	if (!aSingleMode)
		{
		TEST(!testShouldUltimatelyFail);
		}
	return (!testShouldUltimatelyFail);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0066
 
  @SYMDEF DEF039237            

  @SYMTestCaseDesc Memory leak in CFbsScreenDevice (occurs on Lubbock, but not WINS)
   
  @SYMTestPriority normal

  @SYMTestStatus Implemented

  @SYMTestActions Creates a screen device in each colour mode 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect2::DEF039237L()
	{
	__UHEAP_MARK;
	const TInt KNumDisplayModes = 11;
	const TDisplayMode KDisplayMode[KNumDisplayModes] =
		{ EGray2, EGray4, EGray16, EGray256, EColor16, EColor256,
		  EColor4K, EColor64K, EColor16M, EColor16MA, EColor16MAP };

	CFbsScreenDevice* device = NULL;
	TInt err = KErrNotSupported;
	TInt index;

	for (index=0; index<KNumDisplayModes && err==KErrNotSupported; ++index)
		{
		ASSERT(device==NULL);
		TRAP(err,device = CFbsScreenDevice::NewL(KNullDesC,KDisplayMode[index]));
		}
	--index;
	if (device)
		{
		_LIT(KLog,"Created device with color depth %S");
		INFO_PRINTF2(KLog,&ColorModeName(KDisplayMode[index]));
		}
	else
		{
		_LIT(KLog,"Failed to create screen device, last color depth tried %S gave return value %d");
		INFO_PRINTF3(KLog,&ColorModeName(KDisplayMode[index]),err);
		}

	User::LeaveIfError(err);
	ASSERT(device);

	delete device;
	__UHEAP_MARKEND; 
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0067
 
  @SYMDEF DEF039331         

  @SYMTestCaseDesc bitgdi fails to deal correctly with screen orientation changes 
    				
  @SYMTestPriority normal

  @SYMTestStatus Implemented

  @SYMTestActions 
  					DEF039604 - CFbsBitGc::InternalizeL/ExternalizeL - comments, version number
   					Default region externalization/internalization related code was removed, so
					I want to check that everything is OK calling 
					CFbsBitGc::ExternalizeL()/CFbsBitGc::InternalizeL().
					The test may also fail in the future if somebody changes CFbsBitGc data members
					and fail to update ExternalizeL/InternalizeL methods.		 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect2::DEF039331L()
	{
	CreateScrDevAndContextL();

	//Make sure that the archive file does not exist.
	FServSession.Delete(KArchiveFileName);

	RFileWriteStream wrStream;
	::CleanupClosePushL(wrStream);
	User::LeaveIfError(wrStream.Create(FServSession, KArchiveFileName, EFileWrite | EFileRead));

	iGc->ExternalizeL(wrStream);
	wrStream.CommitL();
	CleanupStack::PopAndDestroy();//wrStream

	RFileReadStream rdStream;
	::CleanupClosePushL(rdStream);
	User::LeaveIfError(rdStream.Open(FServSession, KArchiveFileName, EFileRead));
	iGc->InternalizeL(rdStream);
	CleanupStack::PopAndDestroy();//rdStream
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0068
 
  @SYMDEF DEF039650             

  @SYMTestCaseDesc CFbsDevice::RectCompare says don't match when they do
   
  @SYMTestPriority normal

  @SYMTestStatus Implemented

  @SYMTestActions Creates a bitmap, calls scanlinedata then checks that the scanline returns the correct amount of data
 
  @SYMTestExpectedResults Test should return correct number of bytes
*/
//DEF039650 - . 
void CTDefect2::DEF039650L()
	{
	enum {KHeight = 1};//Bitmaps height - in pixels
	TDisplayMode testMode[] =  {EGray2, EGray4, EGray16, EColor16, EColor4K, EColor64K, EColor16M, EColor16MU, EColor256};//tested display modes
	const TInt pixelPerByte[] ={8,      4,      2,       2,        0,        0,         0,         0,          1};
	const TInt bytePerPixel[] ={0,      0,      0,       0,        2,        2,         3,         4,          0};
	const TInt bitmapWidth[] = {22,     7,      11,      11,       3,        3,         1,         1,          10};//bitmaps width
	const TUint8 KTestVal1 = 0xA3;//test value
	const TUint8 KTestVal2 = 0xF7;//test value
	TUint8 scanLineData[100];//100 should be enough for all possible modes which are tested
	for(TInt ii=0;ii<TInt(sizeof(testMode)/sizeof(testMode[0]));ii++)
		{
		TSize size(bitmapWidth[ii], KHeight);//bitmap size - in pixels
		CreateBitmapL(size, testMode[ii]);
		//Initialize the bitmap scanline
		TInt scanLineLen = 0;
		if(pixelPerByte[ii])
			{
			scanLineLen = bitmapWidth[ii] / pixelPerByte[ii] + (bitmapWidth[ii] % pixelPerByte[ii] ? 1 : 0);
			}
		else
			{
			scanLineLen = bitmapWidth[ii] * bytePerPixel[ii];
			}
		const TInt KAddBytes = 10;
		Mem::Fill(scanLineData, scanLineLen, KTestVal1);
		TPtr8 p(scanLineData, scanLineLen, scanLineLen + KAddBytes);
		iBitmap->SetScanLine(p, 0);
		//Create bitmap device (KWidth, KHeight) size
		DeleteBitmapDevice();
		iBmpDevice = CFbsBitmapDevice::NewL(iBitmap);
		//Fill the scanLineData with control values
		Mem::Fill(scanLineData, sizeof(scanLineData), KTestVal2);
		//GetScanLine test - check the KAddBytes bytes
		iBmpDevice->GetScanLine(p, TPoint(), bitmapWidth[ii], testMode[ii]);
		for(TInt jj=0;jj<KAddBytes;jj++)
			{
			TEST(scanLineData[scanLineLen+jj] == KTestVal2);
			}
		}//end of - for(TInt ii=0;ii<TInt(sizeof(testMode)/sizeof(testMode[0]));ii++)
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0069
 
  @SYMDEF             

  @SYMTestCaseDesc Tests the performance of getting and setting pixels in different display modes
   
  @SYMTestPriority normal

  @SYMTestStatus Implemented

  @SYMTestActions draws a number opf pixels to a bitmap then reads them back. Time is recorded
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect2::GetPixelPerformance1L()
	{
	TDisplayMode mode[] = {EColor16MA, EColor16MAP, EColor256, EColor4K, EColor16M, EColor16MU, EColor64K, 
						   EGray256, EGray16, EGray4, EGray2, EColor16};
	_LIT(KLog,"Mode %S. Uncompressed bitmap. Time=%d");
	for(TInt ii=0;ii<TInt(sizeof(mode)/sizeof(mode[0]));ii++)
		{
		//Create bitmap
		enum {KWidth = 600, KHeight = 400};
		TSize size(KWidth, KHeight);
		CreateBitmapL(size, mode[ii]);
		TEST(!iBitmap->IsCompressedInRAM());
		//Fill bitmap
		TBitmapUtil bmpUtil(iBitmap);
		TTime now;
		now.UniversalTime();
		TInt64 seed = now.Int64();
		bmpUtil.Begin(TPoint(0, 0));
		for(TInt j1=0;j1<KWidth;j1++)
			{
			for(TInt j2=0;j2<KHeight;j2++)
				{
				bmpUtil.SetPos(TPoint(j1, j2));
				bmpUtil.SetPixel(Math::Rand(seed));
				}
			}
		bmpUtil.End();
		//Performance test
		TUint time = User::TickCount();
		TPoint pt;
		TRgb color(0);
		for(TInt jj=0;jj<size.iWidth;jj++)
			{
			for(TInt kk=0;kk<size.iHeight;kk++)
				{
				pt.iX = jj;
				pt.iY = kk;
				iBitmap->GetPixel(color, pt);
				}
			}
		time = User::TickCount() - time;
		INFO_PRINTF3(KLog,&ColorModeName(mode[ii]),time);
		//Destroy bitmap
		DeleteBitmap();
		}//end of - for(TInt ii=0;ii<TInt(sizeof(mode)/sizeof(mode[0]));ii++)
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0070
 
  @SYMDEF             

  @SYMTestCaseDesc Draws text to the screen device in different orientations
   
  @SYMTestPriority normal

  @SYMTestStatus Implemented

  @SYMTestActions Creates a screen device and gc, changes the gc orientation then draws some text to it 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect2::RotateMoveTextL()
	{
	TInt err=CreateScrDevAndContext(EColor64K);
	if (err!=KErrNone)
		err=CreateScrDevAndContext(EColor256);
	if (err!=KErrNone)
		err=CreateScrDevAndContext(EColor16MA);
	if (err!=KErrNone)
		err=CreateScrDevAndContext(EColor16MAP);
	TEST(err==KErrNone);
	DoRotateMoveTextL();
	}

void CTDefect2::DoRotateMoveTextL()
	{
	__ASSERT_ALWAYS(iScrDev, User::Invariant());
	__ASSERT_ALWAYS(iGc, User::Invariant());
	
	CreateFontL();

	const CFbsBitGc::TGraphicsOrientation KOrientation[] = 
		{
		CFbsBitGc::EGraphicsOrientationNormal,
		CFbsBitGc::EGraphicsOrientationRotated90,
		CFbsBitGc::EGraphicsOrientationRotated180,
		CFbsBitGc::EGraphicsOrientationRotated270
		};

	for(TInt ii=0;ii<TInt(sizeof(KOrientation)/sizeof(KOrientation[0]));++ii)
		{
		if(!iGc->SetOrientation(KOrientation[ii]))
			{
			continue;
			}
		_LIT(KRotation,"===EOrientation%S===");
		INFO_PRINTF2(KRotation,&RotationName(KOrientation[ii]));

		TSize size = iScrDev->SizeInPixels();
		RDebug::Print(_L("Size: %d, %d\r\n"), size.iWidth, size.iHeight);
		for(TInt x=-40;x<(size.iWidth+30);x+=27)
			{
			for(TInt y=-40;y<(size.iHeight+30);y+=23)
				{
				iGc->Clear();
				iGc->SetPenStyle(CGraphicsContext::ESolidPen);
				iGc->SetPenColor(TRgb(0x00, 0x00, 0x00));
				iGc->SetPenSize(TSize(1, 1));
				
				iGc->DrawText(_L("Test text"), TPoint(x, y));
				
				iScrDev->Update();
				}
			}
		}
	iGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0071
 
  @SYMDEF             

  @SYMTestCaseDesc Swaps width and height of a bitmap
   
  @SYMTestPriority normal

  @SYMTestStatus Implemented

  @SYMTestActions Test to swap the width and height of a bitmap
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect2::SwapWidthAndHeightL()
	{
	TDisplayMode testMode[] =  {EColor4K, EColor64K, EColor16M, EColor16MU,
								EColor256, EColor16MA, EColor16MAP, EColor16, EGray16, EGray4, EGray2};//tested display modes
	_LIT(KLog,"Mode %S");
	for(TInt ii=0;ii<TInt(sizeof(testMode)/sizeof(testMode[0]));ii++)
		{
		INFO_PRINTF2(KLog,&ColorModeName(testMode[ii]));
		TSize size(40, 12);//bitmap size - in pixels
		CreateBitmapL(size, testMode[ii]);//iBitmap - created
		//Create bitmap device
		DeleteBitmapDevice();
		iBmpDevice = CFbsBitmapDevice::NewL(iBitmap);
		TInt err = iBmpDevice->SwapWidthAndHeight();
		TEST(err == KErrNone);
		}//end of - for(TInt ii=0;ii<TInt(sizeof(testMode)/sizeof(testMode[0]));ii++)
	}


void CTDefect2::CreateScrDevAndContextL()
	{
	DeleteGraphicsContext();
	DeleteScreenDevice();
	TDisplayMode mode[] = {EColor256, EColor4K, EColor16M, EColor16MU, EColor64K,
						   EGray256, EGray16, EGray4, EGray2, EColor16, EColor16MA, EColor16MAP};
	TInt ii;
	TInt err = KErrNotSupported;
	for(ii=0;(ii<TInt(sizeof(mode)/sizeof(mode[0]))) && (err == KErrNotSupported);++ii)
		{
		err = CreateScrDevAndContext(mode[ii]);
		}
	TEST(err == KErrNone);
	}

TInt CTDefect2::CreateScrDevAndContext(TInt aScreenNo, TDisplayMode aDisplayMode)
	{
	DeleteGraphicsContext();
	DeleteScreenDevice();
	TRAPD(err, iScrDev = CFbsScreenDevice::NewL(aScreenNo, aDisplayMode));
	if ( !iScrDev )
		{
		TESTE( err == KErrNotSupported, err );
		return err;
		}
	TEST(err == KErrNone);
	TEST(iScrDev->ScreenNo() == aScreenNo);
	err = iScrDev->CreateContext((CGraphicsContext*&)iGc);
	if ( !iGc )
		{
		return err;
		}
	TEST(err == KErrNone);
	iGc->SetUserDisplayMode(aDisplayMode);
	iScrDev->ChangeScreenDevice(NULL);
	iScrDev->SetAutoUpdate(EFalse);
	iSize = iScrDev->SizeInPixels();
	iCurrentMode = aDisplayMode;
	return err;
	}

TInt CTDefect2::CreateScrDevAndContext(TDisplayMode aDisplayMode)
	{
	return CreateScrDevAndContext(KDefaultScreenNo,aDisplayMode);
	}

void CTDefect2::DeleteScreenDevice()
	{
	delete iScrDev;
	iScrDev = NULL;
	}

void CTDefect2::DeleteGraphicsContext()
	{
	delete iGc;
	iGc = NULL;
	}

void CTDefect2::CreateBitmapL(const TSize& aSize, TDisplayMode aMode)
	{
	DeleteBitmap();
	iBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(aSize, aMode));
	}

void CTDefect2::DeleteBitmap()
	{
	if(iBitmap)
		{
		iBitmap->Reset();
		}
	delete iBitmap;
	iBitmap = NULL;
	}

void CTDefect2::DeleteBitmapDevice()
	{
	delete iBmpDevice;
	iBmpDevice = NULL;
	}

void CTDefect2::CreateFontL()
	{
	CFbsFont* font = NULL;
	TFontSpec fs(_L("Swiss"), 12);
	User::LeaveIfError(iScrDev->GetNearestFontToDesignHeightInPixels(font, fs));
	iGc->UseFont(font);
	}

void CTDefect2::DestroyFont()
	{
	if(iGc)
		{
		iGc->DiscardFont();
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0073
 
  @SYMDEF             

  @SYMTestCaseDesc Clear with non-zero origin
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates a gc, setes its origin off (0,0) then clears it
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect2::NonZeroOriginClearL()
	{
	TInt err=CreateScrDevAndContext(EColor64K);
	if (err==KErrNotSupported)
		err=CreateScrDevAndContext(EColor16MA);
	if (err==KErrNotSupported)
		err=CreateScrDevAndContext(EColor16MAP);
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to create screen device, err=%d");
		INFO_PRINTF2(KLog,err);
		}
	TRgb setColor(TRgb::Color64K(13897));
	iGc->SetBrushColor(setColor);
	iGc->SetOrigin(TPoint(100,100));
	iGc->Clear();
	iScrDev->Update();

	//Check samples of the screen that it is the right color
	TSize size=iScrDev->SizeInPixels();
	TInt x;
	TInt y;

	for (x=0;x<size.iWidth;x+=10)
		{
		for (y=0;y<size.iHeight;y+=10)
			{
			TRgb gotColor;
			iScrDev->GetPixel(gotColor, TPoint(x,y));
			TEST(gotColor==setColor);
			}
		}
	}

/** 
@SYMTestCaseID		GRAPHICS-BITGDI-0108 

@SYMDEF				PDEF127874      

@SYMTestCaseDesc	Use pattern brush bitmaps with zero widths or zero height or both 

@SYMTestPriority	High 

@SYMTestStatus		Implemented 

@SYMTestActions		Creates 3 bitmaps: 
					One with zero width, one with zero height, one width zero width and height. 
					Each bitmap should automatically be cleared to white. 
					Clear the screen to black. 
					Set the GC to use patterned brush. 
					In turn, set the brush pattern to be one of the bitmap and draw a rectangle.                     
@SYMTestExpectedResults Each rectangle drawn should remain black as each pattern brush should not be used.   
*/ 
void CTDefect2::ZeroSizedPatternBrushL() 
	{ 
	// Create a source bitmap with zero width 
	CFbsBitmap* bmp1 = new (ELeave) CFbsBitmap(); 
	CleanupStack::PushL(bmp1); 
	User::LeaveIfError(bmp1->Create(TSize(0,1), EColor16MU)); 

	// Create a source bitmap with zero height 
	CFbsBitmap* bmp2 = new(ELeave) CFbsBitmap(); 
	CleanupStack::PushL(bmp2); 
	User::LeaveIfError(bmp2->Create(TSize(1,0), EColor16MU)); 

	// Create a source bitmap with zero width and height 
	CFbsBitmap* bmp3 = new (ELeave) CFbsBitmap(); 
	CleanupStack::PushL(bmp3); 
	User::LeaveIfError(bmp3->Create(TSize(0,0), EColor16MU)); 

	//Create a screen device & gc 
	CreateScrDevAndContextL(); 

	TRgb clearColor = KRgbBlack; 
	iGc->SetBrushColor(clearColor); 
	iGc->Clear(); 
	iScrDev->Update(); 
	iGc->SetPenStyle(CGraphicsContext::ENullPen); 
	iGc->SetBrushStyle(CGraphicsContext::EPatternedBrush); 
	TSize drawRectSize(10,3); 
	// Draw a rectangle using each bitmap as a brush pattern 
	// Each rectangle does not overlap 
	iGc->UseBrushPattern(bmp1); 
	iGc->DrawRect(TRect(TPoint(0,0),drawRectSize)); 

	iGc->UseBrushPattern(bmp2); 
	iGc->DrawRect(TRect(TPoint(0,4),drawRectSize)); 

	iGc->UseBrushPattern(bmp3); 
	iGc->DrawRect(TRect(TPoint(0,8),drawRectSize)); 

	iScrDev->Update(); 
	// Bitmaps should be cleared to white when created, so if they are drawn 
	// will show up different to cleared screen. 
	for(TInt y=0; y<12; ++y) 
		{ 
		for(TInt x=0; x<10; ++x) 
			{ 
			TRgb pixelColor; 
			iScrDev->GetPixel(pixelColor,TPoint(x,y)); 
			TEST(pixelColor==clearColor); 
			} 
		} 
	CleanupStack::PopAndDestroy(3, bmp1); 
	DeleteGraphicsContext(); 
	DeleteScreenDevice(); 
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-0110

@SYMDEF INC128813,PDEF129382

@SYMTestCaseDesc Test CFbsBitGc::InternalizeL() when the font duplicate called from 
CFbsBitGc::Internalize:() fails.
 
@SYMTestPriority Normal

@SYMTestStatus Implemented (udeb/ debug versions only)

@SYMTestActions Create and externalize a the CFbsBitGc*.
 				Then send a message to the font and bitmap server to force 
 				CFbsBitGcFont::Duplicate() calls to fail.
 				Internalize the CFbsBitGc*()
 				Then send a message to the font and bitmap server to allow
 				CFbsBitGcFont::Duplicate() calls to succeed.
 				If the test runs to completion without any panics, then it has passed.
*/
void CTDefect2::CFbsBitGcInternalizeLFailL()
	{
	// this code is based on test CTDefect2::DEF039331L();
#ifndef _DEBUG
	INFO_PRINTF1(_L("Cannot run CFbsBitGcInternalizeLFailL test because test needs udeb for fbserv"));
	return;
	
#else
	CreateScrDevAndContextL();

	//Make sure that the archive file does not exist.
	FServSession.Delete(KArchiveFileName);

	CBufFlat* flatBuf = CBufFlat::NewL(256);
	CleanupStack::PushL(flatBuf);
	RBufWriteStream wrStream(*flatBuf);
	::CleanupClosePushL(wrStream);
	
	CreateFontL();
	
	iGc->ExternalizeL(wrStream);
	wrStream.CommitL();
	CleanupStack::PopAndDestroy();//wrStream

	RBufReadStream rdStream(*flatBuf);
	::CleanupClosePushL(rdStream);

	RFbsSession::GetSession()->SendCommand(static_cast<TInt>(EFbsMessSetDuplicateFail),1);

	TInt ret=KErrNone;
	TRAP(ret, (void)(iGc->InternalizeL(rdStream)));
	INFO_PRINTF2(_L("CFbsBitGcInternalizeLFailL test returned %d: should have an error code"),ret);

	RFbsSession::GetSession()->SendCommand(static_cast<TInt>(EFbsMessSetDuplicateFail),0);

	//Without the fix there is a Kern-Exec3 panic.
	TEST (ETrue);
	CleanupStack::PopAndDestroy(2,flatBuf);
#endif
	}

/** 
@SYMTestCaseID		GRAPHICS-BITGDI-0111 

@SYMDEF				DEF132331

@SYMTestCaseDesc	Compare output from CFbsScreenDevice::HorizontalPixelsToTwips() and 
					VerticalPixelsToTwips() with the same methods in CWsScreenDevice with
					large input numbers. 

@SYMTestPriority	Normal 

@SYMTestStatus		Implemented 

@SYMTestActions		Creates an instance of CWsScreenDevice (instance of CFbsScreenDevice already prepared for
					other tests).  Call the pixels to twips and twips to pixels conversion methods on each with same 
					number.  Compare the resulting output.                       
@SYMTestExpectedResults Outputs should be within a small tolerance.   
*/ 
void CTDefect2::PixelsToTwipsConversionCheck() 
	{ 
	const TInt 	KHorizontalTestPixels=64000000;
	const TInt	KVerticalTestPixels=24000000;
	const TReal32	KTolerance=0.10; // Percent difference permitted
	TInt cFbsScreenDeviceResult = 1;
	TInt cWsScreenDeviceResult = 1;
	TReal32 percentDifference = 0.0;
	CreateScrDevAndContextL();
	TBool testResult = EFalse;

	RWsSession wsSession;
	if (KErrNone == wsSession.Connect())
		{
		CWsScreenDevice* wsScrDev = new (ELeave) CWsScreenDevice(wsSession);
		wsScrDev->Construct(0);
		
		// first compare results of the two class's HorizontalPixelsToTwips and pass 
		// the test if they are within tolerance
		cFbsScreenDeviceResult = iScrDev->HorizontalPixelsToTwips(KHorizontalTestPixels);
		cWsScreenDeviceResult = wsScrDev->HorizontalPixelsToTwips(KHorizontalTestPixels);
		percentDifference = Abs((TReal32)(cFbsScreenDeviceResult - cWsScreenDeviceResult)/cFbsScreenDeviceResult*100.0);
		testResult = (percentDifference < KTolerance);
		if ( !testResult )
			{
			_LIT(KHorizPixelsToTwipsDesc, "%d horizontal pixels converted to twips by screen device: FBS=%d, WS=%d");
			WARN_PRINTF4(KHorizPixelsToTwipsDesc, KHorizontalTestPixels, cFbsScreenDeviceResult, cWsScreenDeviceResult);
			}
		TEST( testResult );
		
		// convert the CWsScreenDevice result back again using each class's HorizontalTwipsToPixels
		// and pass the test if these results are within tolerance
		TInt numTwipsToTest = cWsScreenDeviceResult;
		cFbsScreenDeviceResult = iScrDev->HorizontalTwipsToPixels(numTwipsToTest);
		cWsScreenDeviceResult = wsScrDev->HorizontalTwipsToPixels(numTwipsToTest);
		percentDifference = Abs((TReal32)(cFbsScreenDeviceResult - cWsScreenDeviceResult)/cFbsScreenDeviceResult*100.0);
        testResult = (percentDifference < KTolerance);
        if ( !testResult )
			{
			_LIT(KHorizTwipsToPixelsDesc, "%d horizontal twips converted to pixels by screen device: FBS=%d, WS=%d");
			WARN_PRINTF4(KHorizTwipsToPixelsDesc, numTwipsToTest, cFbsScreenDeviceResult, cWsScreenDeviceResult);
            }
        TEST( testResult );

		// next compare results of the two class's VerticalPixelsToTwips and pass the test
		// if they are within tolerance
		cFbsScreenDeviceResult = iScrDev->VerticalPixelsToTwips(KVerticalTestPixels);
		cWsScreenDeviceResult= wsScrDev->VerticalPixelsToTwips(KVerticalTestPixels);
		percentDifference = Abs((TReal32)(cFbsScreenDeviceResult - cWsScreenDeviceResult)/cFbsScreenDeviceResult*100.0);
        testResult = (percentDifference < KTolerance);
        if ( !testResult )
            {
            _LIT(KVertPixelsToTwipsDesc, "%d vertical pixels converted to twips by screen device: FBS=%d, WS=%d");
            WARN_PRINTF4(KVertPixelsToTwipsDesc, KVerticalTestPixels, cFbsScreenDeviceResult, cWsScreenDeviceResult);
            }
        TEST( testResult );
		
		// convert the CWsScreenDevice result back again using each class's VerticalTwipsToPixels
		// and pass the test if these results are within tolerance
		numTwipsToTest = cWsScreenDeviceResult;
		cFbsScreenDeviceResult = iScrDev->VerticalTwipsToPixels(numTwipsToTest);
		cWsScreenDeviceResult = wsScrDev->VerticalTwipsToPixels(numTwipsToTest);
		percentDifference = Abs((TReal32)(cFbsScreenDeviceResult - cWsScreenDeviceResult)/cFbsScreenDeviceResult*100.0);
        testResult = (percentDifference < KTolerance);
        if ( !testResult )
            {
            _LIT(KVertTwipsToPixelsDesc, "%d vertical twips converted to pixels by screen device: FBS=%d, WS=%d");
            WARN_PRINTF4(KVertTwipsToPixelsDesc, numTwipsToTest, cFbsScreenDeviceResult, cWsScreenDeviceResult);
            }
        TEST( testResult );
		
		delete wsScrDev;
		wsSession.Close();
		}
	else
		{
		iStep->SetTestStepResult(EFail);
		_LIT(KMessage,"ERROR: Test Failed");
		Logger().LogExtra((TText8*)__FILE__, __LINE__, ESevrErr,KMessage);
		}
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-0113

@SYMDEF PDEF138111 

@SYMTestCaseDesc Test CFbsBitGc::CopyRect() with non-trivial alpha channel
 
@SYMTestPriority Normal

@SYMTestStatus Implemented

@SYMTestActions Create a 128 by 128 pixel bitmap with display mode EColor16MA. 
				Fill the upper half of the bitmap with semi-transparent gray. 
				Call CFbsBitGc::CopyRect() to copy the upper half to the lower half.
 				Sample one pixel from both the upper and lower half of the bitmap.

@SYMTestExpectedResults The two pixels should be the same. Without the fix the lower part
						would show a lighter shade of the gray.
*/
void CTDefect2::CopyRectAlphaL()
	{
	TSize size = TSize(128, 128);
	
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(size, EColor16MA));
	
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(bitmapDevice);	
	
	CFbsBitGc* bitmapContext = NULL;
	User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));
	CleanupStack::PushL(bitmapContext);
	
	bitmap->BeginDataAccess();
	Mem::Fill(bitmap->DataAddress(), 128 * 4 * 64, 0x80);
	bitmap->EndDataAccess();
	bitmapContext->CopyRect(TPoint(0, 64), TRect(0, 0, 128, 64));
	
	//sample a pixel from the lower part of the bitmap and it should be the same as the upper half 
	TRgb pixelColorUpper;
	bitmap->GetPixel(pixelColorUpper, TPoint(2, 3));
	TRgb pixelColorLower;
	bitmap->GetPixel(pixelColorLower, TPoint(80, 90));
	TEST(pixelColorUpper == pixelColorLower);
	
	CleanupStack::PopAndDestroy(3, bitmap);
	}

//

class TTestSetBitsThread: public TFunctionThread
	{
public:
	TTestSetBitsThread(TDisplayMode aDisplayMode, TBool aVerticalResize, TBool aScaling);
private:
	virtual TInt ThreadFunctionL();
private:
	TDisplayMode iDisplayMode;
	TBool iVerticalResize;
	TBool iScaling;
	};

TTestSetBitsThread::TTestSetBitsThread(TDisplayMode aDisplayMode, TBool aVerticalResize, TBool aScaling)
	: iDisplayMode(aDisplayMode), iVerticalResize(aVerticalResize), iScaling(aScaling)
	{
	TBuf<32> threadName;
	threadName.Format(_L("TestSetBits-%d-%d-%d"), TInt(aDisplayMode), TInt(aVerticalResize), TInt(aScaling));
	LaunchThreadFunction(threadName);
	}

TInt TTestSetBitsThread::ThreadFunctionL()
	{
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	User::LeaveIfError(bmp->Create(TSize(256, 256), iDisplayMode));
	CFbsBitmapDevice* bmpDev = CFbsBitmapDevice::NewL(bmp);
	CleanupStack::PushL(bmpDev);
	if (iScaling)
		{
		User::LeaveIfError(bmpDev->SetScalingFactor(TPoint(0, 0), 2, 2, 1, 1));
		}
	if (iVerticalResize)
		{
		User::LeaveIfError(bmp->Resize(TSize(256, 128)));
		}
	else
		{
		User::LeaveIfError(bmp->Resize(TSize(128, 256)));
		}
	bmpDev->DrawingBegin();
	bmpDev->DrawingEnd();
	CleanupStack::PopAndDestroy(2);
	return 0;
	}

/**
@SYMTestCaseID GRAPHICS-BITGDI-0112

@SYMDEF PDEF138375

@SYMTestCaseDesc Makes sure that CFbsBitmapDevice::SetBits() checks that the bitmap size is consistent.

@SYMTestPriority Normal

@SYMTestStatus Implemented

@SYMTestActions 1. Creates a bitmap and an associated bitmap device.
				2. Resizes the bitmap directly.
				3. Calls DrawingBegin()/DrawingEnd() on the bitmap device.

@SYMTestExpectedResults The call to DrawingBegin() should panic in debug builds.
*/
void CTDefect2::TestSetBitsL()
	{
#ifdef _DEBUG
	for (TBool verticalResize = 0; verticalResize <= 1; ++verticalResize)
		{
		for (TBool scaling = 0; scaling <= 1; ++scaling)
			{
			TTestSetBitsThread testThread1(EGray2, verticalResize, scaling);
			TEST(testThread1.iExitHow == TFunctionThread::EPanic);
			TEST(testThread1.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread2(EGray4, verticalResize, scaling);
			TEST(testThread2.iExitHow == TFunctionThread::EPanic);
			TEST(testThread2.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread3(EGray16, verticalResize, scaling);
			TEST(testThread3.iExitHow == TFunctionThread::EPanic);
			TEST(testThread3.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread4(EColor16, verticalResize, scaling);
			TEST(testThread4.iExitHow == TFunctionThread::EPanic);
			TEST(testThread4.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread5(EGray256, verticalResize, scaling);
			TEST(testThread5.iExitHow == TFunctionThread::EPanic);
			TEST(testThread5.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread6(EColor256, verticalResize, scaling);
			TEST(testThread6.iExitHow == TFunctionThread::EPanic);
			TEST(testThread6.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread7(EColor4K, verticalResize, scaling);
			TEST(testThread7.iExitHow == TFunctionThread::EPanic);
			TEST(testThread7.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread8(EColor64K, verticalResize, scaling);
			TEST(testThread8.iExitHow == TFunctionThread::EPanic);
			TEST(testThread8.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread9(EColor16M, verticalResize, scaling);
			TEST(testThread9.iExitHow == TFunctionThread::EPanic);
			TEST(testThread9.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread10(EColor16MU, verticalResize, scaling);
			TEST(testThread10.iExitHow == TFunctionThread::EPanic);
			TEST(testThread10.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread11(EColor16MA, verticalResize, scaling);
			TEST(testThread11.iExitHow == TFunctionThread::EPanic);
			TEST(testThread11.iExitCode == EBitgdiPanicInvalidBitmap);

			TTestSetBitsThread testThread12(EColor16MAP, verticalResize, scaling);
			TEST(testThread12.iExitHow == TFunctionThread::EPanic);
			TEST(testThread12.iExitCode == EBitgdiPanicInvalidBitmap);
			}
		}
#else
	WARN_PRINTF1(_L("Test skipped because it needs udeb build"));
#endif
	}

//--------------
__CONSTRUCT_STEP__(Defect2)

void CTDefect2Step::TestSetupL()
	{
	}
	
void CTDefect2Step::TestClose()
	{
	::DeleteDataFile(KArchiveFileName);
	}
