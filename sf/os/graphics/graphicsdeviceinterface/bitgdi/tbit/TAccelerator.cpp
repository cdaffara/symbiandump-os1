// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <graphicsaccelerator.h>
#include "TAccelerator.h"
#include <graphics/fbsdefs.h>

class TAcceleratedBitmapSpecTest : public TAcceleratedBitmapSpec
	{
// Provides public access to TAcceleratedBitmapSpec
public:
	inline TAcceleratedBitmapSpecTest() : TAcceleratedBitmapSpec() {}
	inline TAcceleratedBitmapSpecTest(CFbsBitmap* aBitmap) : TAcceleratedBitmapSpec(aBitmap) {}
	inline TAcceleratedBitmapSpecTest(RHardwareBitmap aBitmap) : TAcceleratedBitmapSpec(aBitmap) {}
	// 
	inline TInt GetInfo(TAcceleratedBitmapInfo& aInfo) const { return TAcceleratedBitmapSpec::GetInfo(aInfo); }
	inline void Lock(TBitmapLockCount& aCount) { TAcceleratedBitmapSpec::Lock(aCount); }
	inline void Lock(TBitmapLockCount& aCount,TAcceleratedBitmapInfo& aInfo) { TAcceleratedBitmapSpec::Lock(aCount,aInfo); }
	inline void	Unlock(TBitmapLockCount& aCount)  { TAcceleratedBitmapSpec::Unlock(aCount); }
	};

const TInt KNumDisplayModes = 13;
static struct 
	{
	TDisplayMode iMode;
	TBool iEnabled;
	TInt iPixelShift;
	} TestDisplayModes[KNumDisplayModes] =	
   {  
		{EGray2,EFalse,0},
		{EGray4,EFalse,1},
		{EGray16,EFalse,2},
		{EGray256,EFalse,3},
		{EColor16,EFalse,2},
		{EColor256,EFalse,3},
		{EColor64K,EFalse,4},
		{EColor16M,EFalse,5},
		{ERgb,EFalse,5},
		{EColor4K,EFalse,4},
		{EColor16MU,EFalse,5},
		{EColor16MA,EFalse,5},
		{EColor16MAP,EFalse,5}
	};

_LIT(KRam32BitmapOnZ, "z:\\system\\data\\32RAM.mbm");
_LIT(KRamC24BitmapOnZ, "z:\\system\\data\\24RAMC.mbm");
_LIT(KRomC24Bitmap, "z:\\system\\data\\24ROMC.mbm");

//
// CTestBitmap
//

TInt CTestBitmap::ConstructL(TDisplayMode aDisplayMode,TSize aSize,TBool aHWBitmap)
	{
	Reset();

	iBitmap = new CFbsBitmap;
	if(iBitmap==NULL)
		User::Panic(_L("Bitmap not created"),KErrGeneral);

	TInt ret;
	if(aHWBitmap)
		ret = iBitmap->CreateHardwareBitmap(aSize,aDisplayMode,TUid());
	else
		ret = iBitmap->Create(aSize,aDisplayMode);

	if (ret == KErrNotSupported)
		{
		Reset();
		return ret;
		}

	User::LeaveIfError(ret);

	TRAPD(err,iDevice = CFbsBitmapDevice::NewL(iBitmap));
	if(err)
		User::Panic(_L("BitmapDevice not created"),err);

	err = iDevice->CreateContext(iGc);
	if(err)
		User::Panic(_L("BitmapDevice not created"),err);

	return KErrNone;
	}

void CTestBitmap::Reset()
	{
	delete iGc;
	iGc = 0;
	delete iDevice;
	iDevice = 0;
	delete iBitmap;
	iBitmap = 0;
	}

CTestBitmap::~CTestBitmap()
	{
	Reset();
	}

TBool CTestBitmap::Compare(CTestBitmap* aBitmap2)
	{	
	TInt width=iBitmap->SizeInPixels().iWidth;
	TInt pitch = iBitmap->DataStride();
	TInt height=iBitmap->SizeInPixels().iHeight;

	for(TInt yLoop=0;yLoop<height;yLoop++)
		{
		TInt offset=yLoop*pitch;
		TUint8* data1 = ((TUint8*)iBitmap->DataAddress())+offset;
		TUint8* data2 = ((TUint8*)aBitmap2->iBitmap->DataAddress())+offset;
		TUint8* dataLimit = data1 + CFbsBitmap::ScanLineLength(width, iBitmap->DisplayMode());
		while(data1<dataLimit)
			{
			if(*(data1++)!=*(data2++))
				{
				iBitmap->Save(_L("C:\\Compare1.mbm"));
				aBitmap2->iBitmap->Save(_L("C:\\Compare2.mbm"));
				return EFalse;
				}
			}
		}
	return(ETrue);
	}

//
// CTAccelerator
//

CTAccelerator::CTAccelerator(CTestStep* aStep):
	CTGraphicsBase(aStep), 
	iRandomSeed(1),
	iGraphicsAccelerator1(0),
	iGraphicsAccelerator2(0),
	iHwScreenDevice(0),
	iScreenGc(0)
	{
	TInt i;
	for(i=0;i<9;i++) 
		iFixedRegion.AddRect(TRect(i*16,i*8,i*16+32,i*8+32));
	for(i=0;i<9;i++) 
		iFixedRegion.AddRect(TRect(183-(i*16),i*8,199-(i*16),i*8+32));

	iDefaultRegion.AddRect(TRect(0,0,199,99));
	INFO_PRINTF1(_L("Graphics Accelerator Testing"));
	}

void CTAccelerator::Cleanup()
	{
	delete iGraphicsAccelerator1;
	iGraphicsAccelerator1 = 0;
	delete iGraphicsAccelerator2;
	iGraphicsAccelerator2 = 0;
	if(iHwScreenDevice)
		{
		delete iHwScreenDevice;
		iHwScreenDevice = 0;
		}
	if(iScreenGc)
		{
		delete iScreenGc;
		iScreenGc = 0;
		}
	iBitmap1.Reset();
	iBitmap2.Reset();
	iBitmap3.Reset();
	iBitmap4.Reset();
	iBitmap5.Reset();
	iBitmap6.Reset();
	iBitmap7.Reset();
	iTileBitmap.Reset();
	iAlphaBitmap.Reset();
	}

TInt CTAccelerator::Random(TInt aMax)	// aMax <= 0xFFFF
	{
	iRandomSeed *= 69069;
	TUint r = ((iRandomSeed>>16)*aMax)>>16;
	return r;
	}

void CTAccelerator::RunTestCaseL(TInt aCurTestCase)
	{
	TInt i;
	((CTAcceleratorStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	
	switch(aCurTestCase)
		{
	case 1:
		((CTAcceleratorStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0021"));
		INFO_PRINTF1(_L("Testing TAcceleratedBitmapSpec with software bitmaps."));
		for(i=0; i<KNumDisplayModes; i++)
			if (TestDisplayModes[i].iEnabled)
				{
				TestAcceleratedBitmapSpecL(TestDisplayModes[i].iMode,EFalse);
				}
		break;
	case 2:
		((CTAcceleratorStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0022"));
		INFO_PRINTF1(_L("Testing graphics operations with software bitmaps."));
		for(i=0; i<KNumDisplayModes; i++)
			if (TestDisplayModes[i].iEnabled)
				{
				TestGraphicsOperations(TestDisplayModes[i].iMode,EFalse);
				}
		break;
	case 3:
		((CTAcceleratorStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0021"));
		INFO_PRINTF1(_L("Testing TAcceleratedBitmapSpec with hardware bitmaps."));
		for(i=0; i<KNumDisplayModes; i++)
			if (TestDisplayModes[i].iEnabled)
				{
				TestAcceleratedBitmapSpecL(TestDisplayModes[i].iMode,ETrue);
				}
		break;
	case 4:
		((CTAcceleratorStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0022"));
		INFO_PRINTF1(_L("Testing graphics operations with hardware bitmaps."));
		for(i=0; i<KNumDisplayModes; i++)
			if (TestDisplayModes[i].iEnabled)
				{
				TestGraphicsOperations(TestDisplayModes[i].iMode,ETrue);
				}
		break;
	case 5:
		((CTAcceleratorStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0023"));
		INFO_PRINTF1(_L("Testing graphics operations with hardware bitmap as screen."));
		for(i=0; i<KNumDisplayModes; i++)
			if (TestDisplayModes[i].iEnabled)	
				{
				TestGraphicsOperationsWithScreenL(TestDisplayModes[i].iMode,ETrue);
				}
		break;
	case 6:

/**
 @SYMTestCaseID          GRAPHICS-BITGDI-0112
*/
		((CTAcceleratorStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0112"));
		INFO_PRINTF1(_L("Testing passing of compressed info to accelerator"));
		TestAcceleratorInfo();
		break;
	case 7:
/**
 @SYMTestCaseID          GRAPHICS-BITGDI-0113
*/
		((CTAcceleratorStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0113"));
		INFO_PRINTF1(_L("Testing GetInfo() with an extended bitmap"));
		TestAcceleratorInfoForExtendedBitmapL();
		break;

	case 8:
		((CTAcceleratorStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTAcceleratorStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTAcceleratorStep*)iStep)->RecordTestResultL();
 	}

const TSize TestBitmapSize = TSize(199,99);
TSize TestScreenSize;
const TSize TestBitmapSize1 = TSize(165,53);


/**
 @SYMTestCaseID          GRAPHICS-BITGDI-0021
 @SYMCreationDate        21/11/2008
 @SYMAuthor              douglashetherington
 @SYMTestStatus          3. Released
 @SYMTestPriority        High
 @SYMTestCaseDesc        Checks that TAcceleratedBitmapSpec does not change the underlaying bitmaps data
 @SYMTestExpectedResults The bitmap data is not changed
 @SYMTestActions         Creates a bitmap, puts it into a TAcceleratedBitmapSpec then checks the pixel data is unchanged
 @SYMDEF                 
*/
void CTAccelerator::TestAcceleratedBitmapSpecL(TDisplayMode aDisplayMode,TBool aHWBitmap)
	{
	TInt ret=0; 
	
	INFO_PRINTF2(_L("  DisplayMode=%d"),aDisplayMode);

	// Create bitmap
	ret = iBitmap1.ConstructL(aDisplayMode,TestBitmapSize,aHWBitmap);
	INFO_PRINTF2(_L(" TestAcceleratedBitmapSpecL ret=%d"),ret);
	if(ret!=KErrNone)
		{
		WARN_PRINTF1(_L("  Not Supported."));
		return;
		}

	TAcceleratedBitmapSpecTest bitmapSpec(iBitmap1.iBitmap);
	TAcceleratedBitmapInfo info;

	// Check correct size	
	bitmapSpec.Lock(iBitmapLockCount);
	User::LeaveIfError(bitmapSpec.GetInfo(info));
	TEST(info.iSize==iBitmap1.iBitmap->SizeInPixels());
	INFO_PRINTF3(_L("Test Display Modes , Expected iPixelShift : %d, Actual iPixelShift: %d"),TestDisplayModes[aDisplayMode-1].iPixelShift,info.iPixelShift);
	TEST(info.iPixelShift == TestDisplayModes[aDisplayMode-1].iPixelShift);
	bitmapSpec.Unlock(iBitmapLockCount);

	TInt scanLineLength = iBitmap1.iBitmap->ScanLineLength(TestBitmapSize.iWidth,aDisplayMode);
	HBufC8* scanLineBuffer = HBufC8::NewMaxLC(scanLineLength);
	TPtr8 scanLine(scanLineBuffer->Des());

	TInt y,x;

	// fill bitmap with test data
	for(y=info.iSize.iHeight-1; y>=0; y--)
		{
		TUint8* ptr = &(scanLine)[0];
		TUint8* ptrLimit = ptr+scanLineLength;
		x = 42;
		while(ptr<ptrLimit)
			*(ptr++) = (TUint8)(y^(x++));
		iBitmap1.iBitmap->SetScanLine(scanLine,y);
		}
	
	// use TAcceleratedBitmapSpec to check contents
	bitmapSpec.Lock(iBitmapLockCount,info);
	for(y=0; y<info.iSize.iHeight; y++)
		{
		TUint8* ptr = info.iAddress+y*info.iLinePitch;
		TUint8* ptrLimit = ptr+scanLineLength;
		x = 42;
		while(ptr<ptrLimit)
			if(*(ptr++) != (TUint8)(y^(x++)))
				{
				ERR_PRINTF2(_L("!!!  Failed at compare A, line %d"),y);
				User::Leave(KErrGeneral);
				}
		}
	bitmapSpec.Unlock(iBitmapLockCount);

	// fill bitmap with test data (inverse of previous)
	for(y=info.iSize.iHeight-1; y>=0; y--)
		{
		TUint8* ptr = &(scanLine)[0];
		TUint8* ptrLimit = ptr+scanLineLength;
		x = 42;
		while(ptr<ptrLimit)
			*(ptr++) = (TUint8)~(y^(x++));
		iBitmap1.iBitmap->SetScanLine(scanLine,y);
		}
	
	// use TAcceleratedBitmapSpec to check contents
	bitmapSpec.Lock(iBitmapLockCount,info);
	for(y=0; y<info.iSize.iHeight; y++)
		{
		TUint8* ptr = info.iAddress+y*info.iLinePitch;
		TUint8* ptrLimit = ptr+scanLineLength;
		x = 42;
		while(ptr<ptrLimit)
			if(*(ptr++) != (TUint8)~(y^(x++)))
				{
				ERR_PRINTF2(_L("!!!  Failed at compare B, line %d"),y);
				User::Leave(KErrGeneral);
				}
		}
	bitmapSpec.Unlock(iBitmapLockCount);

	// End
	CleanupStack::PopAndDestroy();	//scanLineBuffer

	Cleanup();

	INFO_PRINTF1(_L("  OK"));
	}

class CTestFbsDevice : public CFbsDevice
	{
public:
	inline CGraphicsAccelerator* StealGraphicsAccelerator()
		{ CGraphicsAccelerator* acc = iGraphicsAccelerator; iGraphicsAccelerator = 0; return acc; };
	};


/**
 @SYMTestCaseID          GRAPHICS-BITGDI-0022
 @SYMCreationDate        21/11/2008
 @SYMAuthor              douglashetherington
 @SYMTestStatus          3. Released
 @SYMTestPriority        High
 @SYMTestCaseDesc        Tests various basic graphic operations
 @SYMTestExpectedResults Tests should perform graphics operations succesfully.
 @SYMTestActions         Creates a number of bitmaps then uses them in various graphic operations
 @SYMDEF                 
*/
void CTAccelerator::TestGraphicsOperations(TDisplayMode aDisplayMode,TBool aHWBitmap)
	{
	TInt ret=0;
	
	INFO_PRINTF2(_L("  DisplayMode=%d"),aDisplayMode);
	TRAPD(errCode, ret = iBitmap1.ConstructL(aDisplayMode,TestBitmapSize,aHWBitmap));
	if((ret!=KErrNone) || (errCode !=KErrNone) || (!iBitmap1.iDevice->GraphicsAccelerator()))
		{
		WARN_PRINTF1(_L("  Not Supported."));
		return;
		}

	iGraphicsAccelerator1 = ((CTestFbsDevice*)iBitmap1.iDevice)->StealGraphicsAccelerator();
	delete iGraphicsAccelerator1;
	iGraphicsAccelerator1 = 0;
	
	if(!iSubRegion.IsEmpty())
		iSubRegion.Clear();
	iSubRegion.AddRect(iBitmap1.iBitmap->SizeInPixels());
	iSubRegion.SubRegion(iFixedRegion);
	iSubRegionPtr = &iSubRegion;
	
	TRAP(errCode, ret = iBitmap2.ConstructL(aDisplayMode,TestBitmapSize,aHWBitmap));
	TEST(errCode== KErrNone);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	TRAP(errCode, ret = iBitmap3.ConstructL(aDisplayMode,TestBitmapSize,aHWBitmap));
	TEST(errCode== KErrNone);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	iGraphicsAccelerator1 = ((CTestFbsDevice*)iBitmap3.iDevice)->StealGraphicsAccelerator();

	TRAP(errCode, ret = iBitmap4.ConstructL(aDisplayMode,TestBitmapSize,aHWBitmap));
	TEST(errCode== KErrNone);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	TRAP(errCode, ret = iBitmap5.ConstructL(aDisplayMode,TestBitmapSize1,aHWBitmap));
	TEST(errCode== KErrNone);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	TRAP(errCode, ret = iBitmap6.ConstructL(aDisplayMode,TestBitmapSize,aHWBitmap));
	TEST(errCode== KErrNone);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	TRAP(errCode, ret = iBitmap7.ConstructL(aDisplayMode,TestBitmapSize,aHWBitmap));
	TEST(errCode== KErrNone);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);
	
	TRAP(errCode, ret = iBitmap8.ConstructL(aDisplayMode,TestBitmapSize,aHWBitmap));
	TEST(errCode== KErrNone);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	//BrushBitmap
    CFbsBitmap* iTile = new CFbsBitmap();
	TEST(iTile!=NULL);
	ret = iTile->Load(_L("z:\\system\\data\\tile.mbm"),0,aHWBitmap);
    TEST(ret==KErrNone);

	TSize tileSize = iTile->SizeInPixels();
	TDisplayMode tileDisplayMode = iTile->DisplayMode();

	TRAP(errCode, ret = iTileBitmap.ConstructL(tileDisplayMode,tileSize,aHWBitmap));
	TEST(errCode== KErrNone);
	if(ret== KErrNotSupported)
		{
		WARN_PRINTF1(_L("  Not Supported."));
		delete iTile;
		iTile = NULL;
		return;
		}
	iTileBitmap.iGc->BitBlt(TPoint(0,0),iTile);
	delete iTile;
	iTile = NULL;

	//Alpha bitmap
	TRAP(errCode, ret = iAlphaBitmap.ConstructL(EGray256,TestBitmapSize,aHWBitmap));
	TEST(errCode== KErrNone);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	iGraphicsAccelerator2 = ((CTestFbsDevice*)iBitmap4.iDevice)->StealGraphicsAccelerator();

	INFO_PRINTF1(_L("FilledRect"));
	TestFilledRect();

	resetColorBitmaps();

	INFO_PRINTF1(_L("FilledRectWithPattern "));
	TestFilledRectWithPattern();
	
	resetColorBitmaps();

	INFO_PRINTF1(_L("InvertRect")); 
	TestInvertRect();
	
	resetColorBitmaps();

	INFO_PRINTF1(_L("FadeRect "));
	TestFadeRect();
	
	resetColorBitmaps();

	INFO_PRINTF1(_L("BitBlt "));
	TestBitBlt();
	
	resetColorBitmaps();

	INFO_PRINTF1(_L("BitBltMasked "));
	TestBitBltMasked();
	
	resetColorBitmaps();

	INFO_PRINTF1(_L("BitBltAlphaBitmap"));
	TestBitBltAlphaBitmap();

	resetColorBitmaps();
	
	INFO_PRINTF1(_L("AlphaBlendTwoBitmaps "));
	TestAlphaBlendTwoBitmaps();
	
	resetColorBitmaps();
	
	INFO_PRINTF1(_L("AlphaBlendOneBitmap "));
	TestAlphaBlendOneBitmap();
	
	resetColorBitmaps();

	Cleanup();
	}


/**
 @SYMTestCaseID          GRAPHICS-BITGDI-0023
 @SYMCreationDate        21/11/2008
 @SYMAuthor              douglashetherington
 @SYMTestStatus          3. Released
 @SYMTestPriority        High
 @SYMTestCaseDesc        Tests various basic graphic operations draw directly to the screen device
 @SYMTestExpectedResults Tests should perform graphics operations succesfully.
 @SYMTestActions         Creates a number of bitmaps then uses them in various graphic operations drawing directly to the screen gc
 @SYMDEF                 
*/
void CTAccelerator::TestGraphicsOperationsWithScreenL(TDisplayMode aDisplayMode,TBool aHWBitmap)
	{
	TInt ret;

	INFO_PRINTF2(_L("  DisplayMode=%d."),aDisplayMode);

	ret = iBitmap1.ConstructL(aDisplayMode,TestBitmapSize,aHWBitmap);
	if((ret!=KErrNone) || (!iBitmap1.iDevice->GraphicsAccelerator()))
		{
		WARN_PRINTF1(_L("  Not Supported."));
		return;
		}

	TRAPD(err,iHwScreenDevice = CFbsScreenDevice::NewL(_L(""),aDisplayMode));
	if(err)
		User::Panic(_L("BitmapDevice not created"),err);
	iHwScreenDevice->ChangeScreenDevice(NULL);
	err = iHwScreenDevice->CreateContext(iScreenGc);
	if(err)
		User::Panic(_L("BitmapDevice not created"),err);

	TestScreenSize.SetSize(iHwScreenDevice->SizeInPixels().iWidth,iHwScreenDevice->SizeInPixels().iHeight);
	iDefaultScreenRegion.AddRect(TestScreenSize);

	iBitmap1.Reset();
	iBitmap1.ConstructL(aDisplayMode,TestScreenSize,aHWBitmap);
	iGraphicsAccelerator1 = ((CTestFbsDevice*)iBitmap1.iDevice)->StealGraphicsAccelerator();
	delete iGraphicsAccelerator1;
	iGraphicsAccelerator1 = 0;
	
	if(!iSubRegion.IsEmpty())
		iSubRegion.Clear();
	iSubRegion.AddRect(iBitmap1.iBitmap->SizeInPixels());
	iSubRegion.SubRegion(iFixedRegion);
	iSubRegionPtr = &iSubRegion;

	ret = iBitmap2.ConstructL(aDisplayMode,TestScreenSize,aHWBitmap);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	ret = iBitmap3.ConstructL(aDisplayMode,TestScreenSize,aHWBitmap);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	iGraphicsAccelerator1 = ((CTestFbsDevice*)iBitmap3.iDevice)->StealGraphicsAccelerator();

	ret = iBitmap4.ConstructL(aDisplayMode,TestScreenSize,aHWBitmap);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	ret = iBitmap5.ConstructL(aDisplayMode,TestBitmapSize1,aHWBitmap);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	ret = iBitmap6.ConstructL(aDisplayMode,TestScreenSize,aHWBitmap);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	ret = iBitmap7.ConstructL(aDisplayMode,TestScreenSize,aHWBitmap);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	//BrushBitmap
    CFbsBitmap* iTile = new CFbsBitmap();
	TEST(iTile!=NULL);
	ret = iTile->Load(_L("z:\\system\\data\\tile.mbm"),0,aHWBitmap);
    TEST(ret==KErrNone);

	TSize tileSize = iTile->SizeInPixels();
	TDisplayMode tileDisplayMode = iTile->DisplayMode();

	ret= iTileBitmap.ConstructL(tileDisplayMode,tileSize,aHWBitmap);
	if(ret == KErrNotSupported)
		{
		WARN_PRINTF1(_L("  Not Supported."));
		delete iTile;
		iTile = NULL;
		return;
		}
	iTileBitmap.iGc->BitBlt(TPoint(0,0),iTile);
	delete iTile;
	iTile = NULL;

	//Alpha bitmap
	ret = iAlphaBitmap.ConstructL(EGray256,TestBitmapSize,aHWBitmap);
	if(ret!=KErrNone)
		User::Panic(_L("Bitmap not created"),ret);

	iGraphicsAccelerator2 = ((CTestFbsDevice*)iBitmap4.iDevice)->StealGraphicsAccelerator();
	resetColorBitmaps();

	INFO_PRINTF1(_L("FilledRect in the screen"));
	TestScreenFilledRect();

	resetColorBitmaps();

	INFO_PRINTF1(_L("FilledRectWithPattern in the screen"));
	TestScreenFilledRectWithPattern();
	
	resetColorBitmaps();

	INFO_PRINTF1(_L("InvertRect in the screen")); 
	TestScreenInvertRect();
	
	resetColorBitmaps();

	INFO_PRINTF1(_L("FadeRect in the screen"));
	TestScreenFadeRect();
	
	resetColorBitmaps();

	INFO_PRINTF1(_L("BitBlt in the screen"));
	TestScreenBitBlt();
	
	resetColorBitmaps();

	INFO_PRINTF1(_L("BitBltMasked in the screen"));
	TestScreenBitBltMasked();
	
	resetColorBitmaps();

	INFO_PRINTF1(_L("BitBltAlphaBitmap in the screen"));
	TestScreenBitBltAlphaBitmap();

	resetColorBitmaps();

	INFO_PRINTF1(_L("AlphaBlendTwoBitmaps in the screen"));
	TestScreenAlphaBlendTwoBitmaps();

	resetColorBitmaps();
	
	
	INFO_PRINTF1(_L("AlphaBlendOneBitmap in the screen"));
	TestScreenAlphaBlendOneBitmap();

	resetColorBitmaps();
	
	INFO_PRINTF1(_L("Rotating the screen"));
	TestScreenRotation();

	resetColorBitmaps();

	INFO_PRINTF1(_L("Changing user display mode"));
	TestUserDisplayMode();

	resetColorBitmaps();

	Cleanup();
	}

void CTAccelerator::TestAcceleratorInfo()
	{
	TInt errCode;
	TInt ret=0;

	iBitmap5.iBitmap=new CFbsBitmap();
	TRAP(errCode, ret=iBitmap5.iBitmap->Load(KRam32BitmapOnZ,0));
	TEST(errCode==KErrNone);
	TEST(ret==KErrNone);
	TAcceleratedBitmapSpec spec(iBitmap5.iBitmap);
	TAcceleratedBitmapInfo info;
	iBitmap5.iBitmap->LockHeap();
	spec.GetInfo(info);
	iBitmap5.iBitmap->UnlockHeap();
	TEST(info.iLinePitch==iBitmap5.iBitmap->Header().iSizeInPixels.iWidth*iBitmap5.iBitmap->Header().iBitsPerPixel/8);

	TRAP(errCode, ret=iBitmap5.iBitmap->Compress());
	TEST(errCode==KErrNone);
	TEST(ret==KErrNone);
	iBitmap5.iBitmap->LockHeap();
	spec.GetInfo(info);
	iBitmap5.iBitmap->UnlockHeap();
	TEST(info.iLinePitch==-iBitmap5.iBitmap->Header().iCompression);

	TUint32* romAddress = NULL;
	if(CFbsBitmap::IsFileInRom(KRomC24Bitmap, romAddress))
		{
		TRAP(errCode, ret=iBitmap5.iBitmap->Load(KRomC24Bitmap,0));
		TEST(errCode==KErrNone);
		TEST(ret==KErrNone);
		iBitmap5.iBitmap->LockHeap();
		spec.GetInfo(info);
		iBitmap5.iBitmap->UnlockHeap();
		TEST(info.iLinePitch==-iBitmap5.iBitmap->Header().iCompression);
		}
	else
		{
		INFO_PRINTF2(_L("Skipping ROM bitmap test since file \"%S\" is reported to not be a ROM bitmap."),
				&KRomC24Bitmap);
		INFO_PRINTF1(_L("This should only occur on non-XIP ROMs, e.g. NAND ROMs, where ROM bitmaps aren't supported."));
		}

	TRAP(errCode, ret=iBitmap5.iBitmap->LoadAndCompress(KRamC24BitmapOnZ,0));
	TEST(errCode==KErrNone);
	TEST(ret==KErrNone);
	iBitmap5.iBitmap->LockHeap();
	spec.GetInfo(info);
	iBitmap5.iBitmap->UnlockHeap();
	TEST(info.iLinePitch==-iBitmap5.iBitmap->Header().iCompression);

	Cleanup();
	}

void CTAccelerator::TestAcceleratorInfoForExtendedBitmapL()
	{	
	const TSize KSizeInPixels(64,65);
	const TDisplayMode KDisplayMode	= EColor64K;
	const TUid KExtendedBitmapUid = {0x87654321};
	const TUint8 KTestData[] = "Extended bitmap test data 123456";
	const TInt KTestDataSize = sizeof(KTestData);
	
	CFbsBitmap* exBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(exBmp);
	TInt err = exBmp->CreateExtendedBitmap(KSizeInPixels, KDisplayMode, KExtendedBitmapUid, KTestData, KTestDataSize);
	TEST(err == KErrNone);
	
	TAcceleratedBitmapSpec exSpec(exBmp);
	TAcceleratedBitmapInfo info;
	exBmp->LockHeap();
	err = exSpec.GetInfo(info);
	TEST(err == KErrNone);
	exBmp->UnlockHeap();
	
	TEST(info.iDisplayMode == KDisplayMode);	
	TEST(info.iSize == KSizeInPixels);
	TEST(info.iLinePitch == -EProprietaryCompression);
	TEST(info.iPixelShift == KExtendedBitmapUid.iUid);
	TEST(info.iDataSize == KTestDataSize);
	
	CleanupStack::PopAndDestroy(exBmp);
	}

const TInt KNumTestFilledRect = 100;
const TInt KNumTestInvertRect = 100;
const TInt KNumTestBitBlt = 100;
const TInt KNumTestFadeRect= 1000;
const TInt KNumTestBitBltMasked= 100;
const TInt KNumTestBitBltAlphaBitmap= 100;
const TInt KNumTestAlphaBlendBitmaps= 100;

void CTAccelerator::TestFilledRect()
	{
	TInt gopBufferSize = KNumTestFilledRect*sizeof(TGopFilledRect);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect bitmapRect(0,0,TestBitmapSize.iWidth,TestBitmapSize.iHeight);
	TRect rect;

	iFixedRegionPtr = &iFixedRegion;

	iBitmap1.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);

	iBitmap2.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap2.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize.iWidth+20)-10;
		rect.iTl.iY = Random(TestBitmapSize.iHeight+20)-10;
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize.iWidth+20);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize.iHeight+20);

		
		TRgb colorRect(Random(256),Random(256),Random(256));

		iBitmap1.iGc->SetBrushColor(colorRect);
		iBitmap1.iGc->DrawRect(rect);

		iBitmap2.iGc->SetBrushColor(colorRect);
		iBitmap2.iGc->DrawRect(rect);

		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(bitmapRect);
		if( (rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			TGopFilledRect gop(rect,colorRect);
			gopDes.Append((TUint8*)&gop,sizeof(gop));
			TInt error = iGraphicsAccelerator1->Operation(gop);
			if(error == KErrNotSupported)
				{
				delete gopBuffer;
				WARN_PRINTF1(_L("  Not Supported."));
				return;
				}
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
	
		TEST(iBitmap2.Compare(&iBitmap3));
		}

	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
	delete gopBuffer;
	}

void CTAccelerator::TestFilledRectWithPattern()
	{
	TInt gopBufferSize = KNumTestInvertRect*sizeof(TGopFilledRectWithPattern);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect bitmapRect(0,0,TestBitmapSize.iWidth,TestBitmapSize.iHeight);
	TRect rect;

	iFixedRegionPtr = &iFixedRegion;

	iBitmap1.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iBitmap1.iGc->UseBrushPattern(iTileBitmap.iBitmap);
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	   
	iBitmap2.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap2.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap2.iGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iBitmap2.iGc->UseBrushPattern(iTileBitmap.iBitmap);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	TAcceleratedBitmapSpec patternBitmapSpec(iTileBitmap.iBitmap);
	TGopFillPattern gopFillPattern;
	gopFillPattern.iBitmap = patternBitmapSpec;

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize.iWidth);
		rect.iTl.iY = Random(TestBitmapSize.iHeight);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize.iWidth);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize.iHeight);

		TPoint brushOrigin(Random(TestBitmapSize.iWidth -5),Random(TestBitmapSize.iHeight-5));

		iBitmap1.iGc->SetBrushOrigin(brushOrigin);
		iBitmap1.iGc->DrawRect(rect);

		iBitmap2.iGc->SetBrushOrigin(brushOrigin);
		iBitmap2.iGc->DrawRect(rect);

		TEST(iBitmap1.Compare(&iBitmap2));
		
		rect.Intersection(bitmapRect);
		if( (rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			gopFillPattern.iOrigin = brushOrigin;
			TGopFilledRectWithPattern gop(rect,gopFillPattern);
			gopDes.Append((TUint8*)&gop,sizeof(gop));
			TInt error = iGraphicsAccelerator1->Operation(gop);
			if(error == KErrNotSupported)
				{
				delete gopBuffer;
				WARN_PRINTF1(_L("  Not Supported."));
				return;
				}
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
	iBitmap1.iGc->DiscardBrushPattern();
	delete gopBuffer;
	}

void CTAccelerator::TestInvertRect()
	{
	TInt gopBufferSize = KNumTestInvertRect*sizeof(TGopInvertRect);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect bitmapRect(0,0,TestBitmapSize.iWidth,TestBitmapSize.iHeight);
	TRect rect;

	iFixedRegionPtr = &iFixedRegion;

	iBitmap1.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap1.iGc->SetDrawMode(CGraphicsContext::EDrawModeNOTSCREEN);
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);

	iBitmap2.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap2.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap2.iGc->SetDrawMode(CGraphicsContext::EDrawModeNOTSCREEN);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize.iWidth+20)-10;
		rect.iTl.iY = Random(TestBitmapSize.iHeight+20)-10;
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize.iWidth+20);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize.iHeight+20);

		iBitmap1.iGc->DrawRect(rect);
		iBitmap2.iGc->DrawRect(rect);

		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(bitmapRect);
		if((rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			TGopInvertRect gop(rect);
			gopDes.Append((TUint8*)&gop,sizeof(gop));
			TInt error = iGraphicsAccelerator1->Operation(gop);
			if(error == KErrNotSupported)
				{
				delete gopBuffer;
				WARN_PRINTF1(_L("  Not Supported."));
				return;
				}
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
	delete gopBuffer;
	}

void CTAccelerator::TestFadeRect()
	{
	TInt gopBufferSize = KNumTestFadeRect*sizeof(TGopFadeRect);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect bitmapRect(0,0,TestBitmapSize.iWidth,TestBitmapSize.iHeight);
    TRect rect;
	
	iFixedRegionPtr = &iFixedRegion;

	iBitmap1.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap1.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);

	iBitmap2.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap2.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap2.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize.iWidth);
		rect.iTl.iY = Random(TestBitmapSize.iHeight);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize.iWidth);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize.iHeight);

		TUint8 blackMap = (TUint8)Random(256);
		TUint8 whiteMap = (TUint8)Random(256);

		RRegion region(rect);

		iBitmap1.iGc->SetFaded(ETrue);
		iBitmap1.iGc->SetFadingParameters(blackMap,whiteMap);
		iBitmap1.iGc->FadeArea((TRegion*)&region);

		iBitmap2.iGc->SetFaded(ETrue);
		iBitmap2.iGc->SetFadingParameters(blackMap,whiteMap);
		iBitmap2.iGc->FadeArea((TRegion*)&region);

		region.Close();

		TEST(iBitmap1.Compare(&iBitmap2));
		
		TGopFadeParams fadeParams;
		fadeParams.iScale = whiteMap - blackMap + 1;
		fadeParams.iOffset = blackMap;
		rect.Intersection(bitmapRect);

		if( (rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			TGopFadeRect gop(rect,fadeParams);
			gopDes.Append((TUint8*)&gop,sizeof(gop));
			TInt error = iGraphicsAccelerator1->Operation(gop);
			if(error == KErrNotSupported)
				{
				WARN_PRINTF1(_L("  Not Supported."));
				goto quitFade2;
				}
			}
		TEST(iBitmap2.Compare(&iBitmap3));
		}

	iGraphicsAccelerator2->Operation(gopDes);
	TEST(iBitmap3.Compare(&iBitmap4));
quitFade2:
	delete gopBuffer;
	iBitmap1.iGc->SetFaded(EFalse);
	iBitmap2.iGc->SetFaded(EFalse);
	}

void CTAccelerator::TestBitBlt()
	{
	TInt gopBufferSize = KNumTestBitBlt*sizeof(TGopBitBlt);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	//dest bitmap has different size
	TRect bitmapRect1(0,0,TestBitmapSize1.iWidth,TestBitmapSize1.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;

	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	TRgb color1(Random(256),Random(256),Random(256));

	iBitmap5.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap5.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap5.iGc->SetBrushColor(color1);
	iBitmap5.iGc->DrawRect(bitmapRect1);


	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize1.iWidth);
		rect.iTl.iY = Random(TestBitmapSize1.iHeight);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize1.iWidth);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize1.iHeight);

        TPoint pointDest(Random(TestBitmapSize.iWidth),Random(TestBitmapSize.iHeight));

		//bitblt with GC
		iBitmap1.iGc->BitBlt(pointDest,iBitmap5.iBitmap,rect);
		iBitmap2.iGc->BitBlt(pointDest,iBitmap5.iBitmap,rect);

		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(bitmapRect1);
		TSize maxSize=TestBitmapSize-pointDest;
		if (rect.Width()>maxSize.iWidth)
			rect.iBr.iX=rect.iTl.iX+maxSize.iWidth;
		if (rect.Height()>maxSize.iHeight)
			rect.iBr.iY=rect.iTl.iY+maxSize.iHeight;
		if(rect.IsEmpty())
			continue;

		if( (rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			TAcceleratedBitmapSpec bitmap5Spec(iBitmap5.iBitmap);
			TGopBitBlt gopBitBlt(pointDest,bitmap5Spec,rect);
			gopDes.Append((TUint8*)&gopBitBlt,sizeof(gopBitBlt));
			TInt error = iGraphicsAccelerator1->Operation(gopBitBlt);
			if(error == KErrNotSupported)
				{
				delete gopBuffer;
				WARN_PRINTF1(_L("  Not Supported."));
				return;
				}
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
	delete gopBuffer;
	}

void CTAccelerator::TestBitBltMasked()
	{
	TInt gopBufferSize = KNumTestBitBltMasked*sizeof(TGopBitBltMasked);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect bitmapRect(0,0,TestBitmapSize.iWidth,TestBitmapSize.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;

	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	//Mask bitmap
	iBitmap6.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap6.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	TRgb color1(Random(256),Random(256),Random(256));

	//Source bitmap
	iBitmap7.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap7.iGc->SetBrushColor(color1);
	iBitmap7.iGc->DrawRect(bitmapRect);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	TAcceleratedBitmapSpec bitmap7Spec(iBitmap7.iBitmap);
	TAcceleratedBitmapSpec bitmap6Spec(iBitmap6.iBitmap);

	TPoint dest;
	TRect rectMask;

	//clear the mask bitmap with white
	iBitmap6.iGc->SetBrushColor(TRgb(255,255,255));
	iBitmap6.iGc->DrawRect(bitmapRect);

	//Put randomly in the Bitmap Mask a numbers of black rects
	iBitmap6.iGc->SetBrushColor(TRgb(0,0,0));

	for(TInt n=0; n<10; n++)
		{
		rectMask.iTl.iX = Random(TestBitmapSize1.iWidth);
		rectMask.iTl.iY = Random(TestBitmapSize1.iHeight);
		rectMask.iBr.iX = rectMask.iTl.iX+1+Random(TestBitmapSize1.iWidth);
		rectMask.iBr.iY = rectMask.iTl.iY+1+Random(TestBitmapSize1.iHeight);
		iBitmap6.iGc->DrawRect(rectMask);
		}

	for(TInt i=0; i<100; i++)
		{
	    //random rect to blit
		rect.iTl.iX = Random(TestBitmapSize1.iWidth);
		rect.iTl.iY = Random(TestBitmapSize1.iHeight);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize1.iWidth);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize1.iHeight);

		//random point destination
		dest.iX = Random(TestBitmapSize1.iWidth);
		dest.iY = Random(TestBitmapSize1.iHeight);

		//do BitBltMasked with graphics contex
        iBitmap1.iGc->BitBltMasked(dest,iBitmap7.iBitmap,rect,iBitmap6.iBitmap,ETrue);
        iBitmap2.iGc->BitBltMasked(dest,iBitmap7.iBitmap,rect,iBitmap6.iBitmap,ETrue);
		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(bitmapRect);
		if(rect.IsEmpty())
			continue;

		if((rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			TGopBitBltMasked gopBitBltMasked(dest,bitmap7Spec,rect,bitmap6Spec);
			gopDes.Append((TUint8*)&gopBitBltMasked,sizeof(gopBitBltMasked));
			TInt error = iGraphicsAccelerator1->Operation(gopBitBltMasked);
			if(error == KErrNotSupported)
				{
				delete gopBuffer;
				WARN_PRINTF1(_L("  Not Supported."));
				return;
				}
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}	
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
	delete gopBuffer;
	}

void CTAccelerator::TestBitBltAlphaBitmap()
	{
	TInt gopBufferSize = KNumTestBitBltAlphaBitmap*sizeof(TGopBitBltAlphaBitmap);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect bitmapRect(0,0,TestBitmapSize.iWidth,TestBitmapSize.iHeight);

    TRect rect;
	iFixedRegionPtr = &iFixedRegion;
	
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	TRgb color = TRgb(Random(256),Random(256),Random(256));

	//Source bitmap
	iBitmap7.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap7.iGc->SetBrushColor(color);
	iBitmap7.iGc->DrawRect(bitmapRect);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	iAlphaBitmap.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	//create 10 gray level for the alpha bitmap
    for(TInt i=0; i<10;i++)
		{
		TRect tmpRect(0,10*i,200,10+10*i);
		TInt index = 255-(20*i);
		TRgb tmpRgb;
        iAlphaBitmap.iGc->SetBrushColor(tmpRgb.Gray256(index));
        iAlphaBitmap.iGc->DrawRect(tmpRect);
		}
    
	TAcceleratedBitmapSpec bitmap7Spec(iBitmap7.iBitmap);
	TAcceleratedBitmapSpec alphaBitmapSpec(iAlphaBitmap.iBitmap);

	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize.iWidth);
		rect.iTl.iY = Random(TestBitmapSize.iHeight);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize.iWidth);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize.iHeight);

		//Random destination point
		TPoint pointDest(Random(TestBitmapSize.iWidth),Random(TestBitmapSize.iHeight));

		//alpha blitting with Graphics Contex
		iBitmap1.iGc->BitBltMasked(pointDest,iBitmap7.iBitmap,rect,iAlphaBitmap.iBitmap,ETrue);
		iBitmap2.iGc->BitBltMasked(pointDest,iBitmap7.iBitmap,rect,iAlphaBitmap.iBitmap,ETrue);

		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(bitmapRect);
		//alpha blitting with Graphics accelerator
		TGopBitBltAlphaBitmap gopBitBltAlphaBitmap(pointDest,bitmap7Spec,rect,alphaBitmapSpec);
		gopDes.Append((TUint8*)&gopBitBltAlphaBitmap,sizeof(gopBitBltAlphaBitmap));
		TInt error = iGraphicsAccelerator1->Operation(gopBitBltAlphaBitmap);
		if(error == KErrNotSupported)
			{
			WARN_PRINTF1(_L("  Not Supported."));
			goto done;
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
done:
	delete gopBuffer;
	}
	
void CTAccelerator::TestAlphaBlendTwoBitmaps()
	{
	TInt gopBufferSize = KNumTestAlphaBlendBitmaps*sizeof(TGopAlphaBlendTwoBitmaps);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect bitmapRect(0,0,TestBitmapSize.iWidth,TestBitmapSize.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;
	
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	TRgb color = TRgb(Random(256),Random(256),Random(256));

	// First Source bitmap
	iBitmap7.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap7.iGc->SetBrushColor(color);
	iBitmap7.iGc->DrawRect(bitmapRect);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	
	// Get a new random color
	color = TRgb(Random(256),Random(256),Random(256));
	
	// Second Source bitmap
	iBitmap8.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap8.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap8.iGc->SetBrushColor(color);
	iBitmap8.iGc->DrawRect(bitmapRect);
	iBitmap8.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	iAlphaBitmap.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	//create 10 gray level for the alpha bitmap
	for(TInt i=0; i<10;i++)
		{
		TRect tmpRect(0,10*i,200,10+10*i);
		TInt index = 255-(20*i);
		TRgb tmpRgb;
		iAlphaBitmap.iGc->SetBrushColor(tmpRgb.Gray256(index));
		iAlphaBitmap.iGc->DrawRect(tmpRect);
		}
    
	TAcceleratedBitmapSpec bitmap7Spec(iBitmap7.iBitmap);
	TAcceleratedBitmapSpec bitmap8Spec(iBitmap8.iBitmap);
	TAcceleratedBitmapSpec alphaBitmapSpec(iAlphaBitmap.iBitmap);

	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize.iWidth - 1); 
		rect.iTl.iY = Random(TestBitmapSize.iHeight - 1);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize.iWidth - rect.iTl.iX);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize.iHeight - rect.iTl.iY);

		TPoint pointDest(Random(TestBitmapSize.iWidth),Random(TestBitmapSize.iHeight));
		TPoint pointSource(Random(TestBitmapSize.iWidth),Random(TestBitmapSize.iHeight));
		TPoint pointAlpha(Random(TestBitmapSize.iWidth - rect.Width() - 1),Random(TestBitmapSize.iHeight - rect.Height() - 1));

		//alpha blending Graphics Context - invalid parameter causes skip to next test
		TInt rc = iBitmap1.iGc->AlphaBlendBitmaps(pointDest,iBitmap7.iBitmap,iBitmap8.iBitmap,rect,pointSource,iAlphaBitmap.iBitmap,pointAlpha);
		if(rc!=KErrNone)
			{
			continue;
			}
		iBitmap2.iGc->AlphaBlendBitmaps(pointDest,iBitmap7.iBitmap,iBitmap8.iBitmap,rect,pointSource,iAlphaBitmap.iBitmap,pointAlpha);
		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(bitmapRect);
		//alpha blending with Graphics accelerator
		TGopAlphaBlendTwoBitmaps gopAlphaBlendTwoBitmaps(pointDest,bitmap7Spec,bitmap8Spec,rect,pointSource,alphaBitmapSpec,pointAlpha);
		gopDes.Append((TUint8*)&gopAlphaBlendTwoBitmaps,sizeof(gopAlphaBlendTwoBitmaps));
		TInt error = iGraphicsAccelerator1->Operation(gopAlphaBlendTwoBitmaps);
		if(error == KErrNotSupported)
			{
			WARN_PRINTF1(_L("  Not Supported."));
			goto done;
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
done:
	delete gopBuffer;
	}
	
void CTAccelerator::TestAlphaBlendOneBitmap()
	{
	TInt gopBufferSize = KNumTestAlphaBlendBitmaps*sizeof(TGopAlphaBlendOneBitmap);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect bitmapRect(0,0,TestBitmapSize.iWidth,TestBitmapSize.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;
	
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	TRgb color = TRgb(Random(256),Random(256),Random(256));

	// First Source bitmap
	iBitmap7.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap7.iGc->SetBrushColor(color);
	iBitmap7.iGc->DrawRect(bitmapRect);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	
	// Get a new random color
	color = TRgb(Random(256),Random(256),Random(256));

	iAlphaBitmap.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	//create 10 gray level for the alpha bitmap
	for(TInt i=0; i<10;i++)
		{
		TRect tmpRect(0,10*i,200,10+10*i);
		TInt index = 255-(20*i);
		TRgb tmpRgb;
		iAlphaBitmap.iGc->SetBrushColor(tmpRgb.Gray256(index));
		iAlphaBitmap.iGc->DrawRect(tmpRect);
		}
		  
 	TAcceleratedBitmapSpec bitmap7Spec(iBitmap7.iBitmap);
	TAcceleratedBitmapSpec alphaBitmapSpec(iAlphaBitmap.iBitmap);

	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize.iWidth - 1); 
		rect.iTl.iY = Random(TestBitmapSize.iHeight - 1);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize.iWidth - rect.iTl.iX);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize.iHeight - rect.iTl.iY);

		TPoint pointDest(Random(TestBitmapSize.iWidth),Random(TestBitmapSize.iHeight));
		TPoint pointAlpha(Random(TestBitmapSize.iWidth - rect.Width()),Random(TestBitmapSize.iHeight - rect.Height()));
		
		iBitmap1.iGc->DrawRect(iBitmap1.iBitmap->SizeInPixels());
		//alpha blending Graphics Context - wrong argument cause skip to next test
		TInt rc = iBitmap1.iGc->AlphaBlendBitmaps(pointDest,iBitmap7.iBitmap,rect,iAlphaBitmap.iBitmap,pointAlpha);
		if(rc!=KErrNone)
			{
			continue;
			}
		iBitmap2.iGc->AlphaBlendBitmaps(pointDest,iBitmap7.iBitmap,rect,iAlphaBitmap.iBitmap,pointAlpha);
		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(bitmapRect);
		//alpha blending with Graphics accelerator
		TGopAlphaBlendOneBitmap gopAlphaBlendOneBitmap(pointDest,iBitmap7.iBitmap,rect,iAlphaBitmap.iBitmap,pointAlpha);
		gopDes.Append((TUint8*)&gopAlphaBlendOneBitmap,sizeof(gopAlphaBlendOneBitmap));
		TInt error = iGraphicsAccelerator1->Operation(gopAlphaBlendOneBitmap);
		if(error == KErrNotSupported)
			{
			WARN_PRINTF1(_L("  Not Supported."));
			goto done;
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
done:
	delete gopBuffer;
	}

void CTAccelerator::TestScreenFilledRect()
	{
	TInt gopBufferSize = KNumTestFilledRect*sizeof(TGopFilledRect);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect screenRect(0,0,TestScreenSize.iWidth,TestScreenSize.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;

	//used with GC
	iBitmap1.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);

	iScreenGc->SetPenStyle(CGraphicsContext::ENullPen);
	iScreenGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iScreenGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestScreenSize.iWidth+20)-10;
		rect.iTl.iY = Random(TestScreenSize.iHeight+20)-10;
		rect.iBr.iX = rect.iTl.iX+1+Random(TestScreenSize.iWidth+20);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestScreenSize.iHeight+20);

		TRgb colorRect(Random(256),Random(256),Random(256));

		iBitmap1.iGc->SetBrushColor(colorRect);
		iBitmap1.iGc->DrawRect(rect);

		iScreenGc->SetBrushColor(colorRect);
		iScreenGc->DrawRect(rect);

		iBitmap2.iGc->BitBlt(TPoint(0,0),*iScreenGc,screenRect);
		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(screenRect);
		if( (rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			TGopFilledRect gop(rect,colorRect);
			gopDes.Append((TUint8*)&gop,sizeof(gop));
			TInt error = iGraphicsAccelerator1->Operation(gop);
			if(error == KErrNotSupported)
				{
				delete gopBuffer;
				WARN_PRINTF1(_L("  Not Supported."));
				return;
				}
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());	
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
	delete gopBuffer;
	}

void CTAccelerator::TestScreenFilledRectWithPattern()
	{
	TInt gopBufferSize = KNumTestInvertRect*sizeof(TGopFilledRectWithPattern);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect screenRect(0,0,TestScreenSize.iWidth,TestScreenSize.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;

	//used with GC
	iBitmap1.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap1.iGc->UseBrushPattern(iTileBitmap.iBitmap);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	   
	iScreenGc->SetPenStyle(CGraphicsContext::ENullPen);
	iScreenGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iScreenGc->UseBrushPattern(iTileBitmap.iBitmap);
	iScreenGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	iScreenGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	TAcceleratedBitmapSpec patternBitmapSpec(iTileBitmap.iBitmap);
	TGopFillPattern gopFillPattern;
	gopFillPattern.iBitmap = patternBitmapSpec;

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestScreenSize.iWidth);
		rect.iTl.iY = Random(TestScreenSize.iHeight);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestScreenSize.iWidth);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestScreenSize.iHeight);

		TPoint brushOrigin(Random(TestScreenSize.iWidth -5),Random(TestScreenSize.iHeight-5));

		iBitmap1.iGc->SetBrushOrigin(brushOrigin);
		iBitmap1.iGc->DrawRect(rect);

		iScreenGc->SetBrushOrigin(brushOrigin);
		iScreenGc->DrawRect(rect);

		iBitmap2.iGc->BitBlt(TPoint(0,0),*iScreenGc,screenRect);
		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(screenRect);
		if( (rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			gopFillPattern.iOrigin = brushOrigin;
			TGopFilledRectWithPattern gop(rect,gopFillPattern);
			gopDes.Append((TUint8*)&gop,sizeof(gop));
			TInt error = iGraphicsAccelerator1->Operation(gop);
			if(error == KErrNotSupported)
				{
				delete gopBuffer;
				WARN_PRINTF1(_L("  Not Supported."));
				return;
				}
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}

	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
	iBitmap1.iGc->DiscardBrushPattern();
	delete gopBuffer;
	}

void CTAccelerator::TestScreenInvertRect()
	{
	TInt gopBufferSize = KNumTestInvertRect*sizeof(TGopInvertRect);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect screenRect(0,0,TestScreenSize.iWidth,TestScreenSize.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;

	iBitmap1.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap1.iGc->SetDrawMode(CGraphicsContext::EDrawModeNOTSCREEN);
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);

	iScreenGc->SetPenStyle(CGraphicsContext::ENullPen);
	iScreenGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iScreenGc->SetDrawMode(CGraphicsContext::EDrawModeNOTSCREEN);
	iScreenGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestScreenSize.iWidth+20)-10;
		rect.iTl.iY = Random(TestScreenSize.iHeight+20)-10;
		rect.iBr.iX = rect.iTl.iX+1+Random(TestScreenSize.iWidth+20);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestScreenSize.iHeight+20);

		iBitmap1.iGc->DrawRect(rect);
		iScreenGc->DrawRect(rect);

		iBitmap2.iGc->BitBlt(TPoint(0,0),*iScreenGc,screenRect);
		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(screenRect);
		if((rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			TGopInvertRect gop(rect);
			gopDes.Append((TUint8*)&gop,sizeof(gop));
			TInt error = iGraphicsAccelerator1->Operation(gop);
			if(error == KErrNotSupported)
				{
				delete gopBuffer;
				WARN_PRINTF1(_L("  Not Supported."));
				return;
				}
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
	delete gopBuffer;
	}

void CTAccelerator::TestScreenFadeRect()
	{
	TInt gopBufferSize = KNumTestFadeRect*sizeof(TGopFadeRect);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect screenRect(0,0,TestScreenSize.iWidth,TestScreenSize.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;

	iBitmap1.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap1.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);

	iScreenGc->SetPenStyle(CGraphicsContext::ENullPen);
	iScreenGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iScreenGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iScreenGc->SetClippingRegion(iFixedRegionPtr);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestScreenSize.iWidth);
		rect.iTl.iY = Random(TestScreenSize.iHeight);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestScreenSize.iWidth);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestScreenSize.iHeight);

		TUint8 blackMap = (TUint8)Random(256);
		TUint8 whiteMap = (TUint8)Random(256);

		RRegion region(rect);

		iBitmap1.iGc->SetFaded(ETrue);
		iBitmap1.iGc->SetFadingParameters(blackMap,whiteMap);
		iBitmap1.iGc->FadeArea((TRegion*)&region);

		iScreenGc->SetFaded(ETrue);
		iScreenGc->SetFadingParameters(blackMap,whiteMap);
		iScreenGc->FadeArea((TRegion*)&region);

		iBitmap2.iGc->BitBlt(TPoint(0,0),*iScreenGc,screenRect);

		region.Close();
		TEST(iBitmap1.Compare(&iBitmap2));

		TGopFadeParams fadeParams;
		fadeParams.iScale = whiteMap - blackMap + 1;
		fadeParams.iOffset = blackMap;
		rect.Intersection(screenRect);

		if( (rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			TGopFadeRect gop(rect,fadeParams);
			gopDes.Append((TUint8*)&gop,sizeof(gop));
			TInt error = iGraphicsAccelerator1->Operation(gop);
			if(error == KErrNotSupported)
				{
				WARN_PRINTF1(_L("  Not Supported."));
				goto quitFade;
				}
			}
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	TEST(iBitmap3.Compare(&iBitmap4));
quitFade:
	delete gopBuffer;
	iBitmap1.iGc->SetFaded(EFalse);
	iScreenGc->SetFaded(EFalse);
	}

void CTAccelerator::TestScreenBitBlt()
	{
	TInt gopBufferSize = KNumTestBitBlt*sizeof(TGopBitBlt);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect screenRect(0,0,TestScreenSize.iWidth,TestScreenSize.iHeight);
	//dest bitmap has different size
	TRect bitmapRect1(0,0,TestBitmapSize1.iWidth,TestBitmapSize1.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;

	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iScreenGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	TRgb color1(Random(256),Random(256),Random(256));

	iBitmap5.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap5.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap5.iGc->SetBrushColor(color1);
	iBitmap5.iGc->DrawRect(bitmapRect1);


	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize1.iWidth);
		rect.iTl.iY = Random(TestBitmapSize1.iHeight);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize1.iWidth);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize1.iHeight);

        TPoint pointDest(Random(TestScreenSize.iWidth),Random(TestScreenSize.iHeight));

		//bitblt with GC
		iBitmap1.iGc->BitBlt(pointDest,iBitmap5.iBitmap,rect);
		iScreenGc->BitBlt(pointDest,iBitmap5.iBitmap,rect);

		iBitmap2.iGc->BitBlt(TPoint(0,0),*iScreenGc,screenRect);
		TEST(iBitmap1.Compare(&iBitmap2));

		TInt extraWidth=(pointDest.iX+rect.Width())-TestScreenSize.iWidth;
		if (extraWidth>0)
			rect.iBr.iX-=extraWidth;
		TInt extraHeight=(pointDest.iY+rect.Height())-TestScreenSize.iHeight;
		if (extraHeight>0)
			rect.iBr.iY-=extraHeight;
		rect.Intersection(bitmapRect1);
		TSize maxSize=TestBitmapSize-pointDest;
		if (rect.Width()>maxSize.iWidth)
			rect.iBr.iX=rect.iTl.iX+maxSize.iWidth;
		if (rect.Height()>maxSize.iHeight)
			rect.iBr.iY=rect.iTl.iY+maxSize.iHeight;
		if(rect.IsEmpty())
			continue;

		if( (rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			TAcceleratedBitmapSpec bitmap5Spec(iBitmap5.iBitmap);
			TGopBitBlt gopBitBlt(pointDest,bitmap5Spec,rect);
			gopDes.Append((TUint8*)&gopBitBlt,sizeof(gopBitBlt));
			TInt error = iGraphicsAccelerator1->Operation(gopBitBlt);
			if(error == KErrNotSupported)
				{
				delete gopBuffer;
				WARN_PRINTF1(_L("  Not Supported."));
				return;
				}
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
	delete gopBuffer;
	}

void CTAccelerator::TestScreenBitBltMasked()
	{
	TInt gopBufferSize = KNumTestBitBltMasked*sizeof(TGopBitBltMasked);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect screenRect(0,0,TestScreenSize.iWidth,TestScreenSize.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;

	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iScreenGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	//Mask bitmap
	iBitmap6.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap6.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	TRgb color1(Random(256),Random(256),Random(256));

	//Source bitmap
	iBitmap7.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap7.iGc->SetBrushColor(color1);
	iBitmap7.iGc->DrawRect(iBitmap7.iBitmap->SizeInPixels());
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	TAcceleratedBitmapSpec bitmap7Spec(iBitmap7.iBitmap);
	TAcceleratedBitmapSpec bitmap6Spec(iBitmap6.iBitmap);

	TPoint dest;
	TRect rectMask;

	//Put randomly in the Bitmap Mask a numbers of black rects
	iBitmap6.iGc->SetBrushColor(TRgb(0,0,0));

	for(TInt n=0; n<3; n++)
		{
		rectMask.iTl.iX = Random(TestScreenSize.iWidth);
		rectMask.iTl.iY = Random(TestScreenSize.iHeight);
		rectMask.iBr.iX = rectMask.iTl.iX+1+Random(TestScreenSize.iWidth);
		rectMask.iBr.iY = rectMask.iTl.iY+1+Random(TestScreenSize.iHeight);
		iBitmap6.iGc->DrawRect(rectMask);
		}

	for(TInt i=0; i<100; i++)
		{
	    //random rect to blit
		rect.iTl.iX = Random(TestScreenSize.iWidth);
		rect.iTl.iY = Random(TestScreenSize.iHeight);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestScreenSize.iWidth);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestScreenSize.iHeight);

		//random point destination
		dest.iX = Random(TestScreenSize.iWidth);
		dest.iY = Random(TestScreenSize.iHeight);

        iBitmap1.iGc->BitBltMasked(dest,iBitmap7.iBitmap,rect,iBitmap6.iBitmap,ETrue);
        iScreenGc->BitBltMasked(dest,iBitmap7.iBitmap,rect,iBitmap6.iBitmap,ETrue);
		iBitmap2.iGc->BitBlt(TPoint(0,0),*iScreenGc,screenRect);
		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(screenRect);
		if(rect.IsEmpty())
			continue;

		if((rect.iBr.iX>rect.iTl.iX) && (rect.iBr.iY>rect.iTl.iY))
			{
			TGopBitBltMasked gopBitBltMasked(dest,bitmap7Spec,rect,bitmap6Spec);
			gopDes.Append((TUint8*)&gopBitBltMasked,sizeof(gopBitBltMasked));
			TInt error = iGraphicsAccelerator1->Operation(gopBitBltMasked);
			if(error == KErrNotSupported)
				{
				delete gopBuffer;
				WARN_PRINTF1(_L("  Not Supported."));
				return;
				}
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}	
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
	delete gopBuffer;
	}

void CTAccelerator::TestScreenBitBltAlphaBitmap()
	{
	TInt gopBufferSize = KNumTestBitBltAlphaBitmap*sizeof(TGopBitBltAlphaBitmap);
	TUint8* gopBuffer = new TUint8[gopBufferSize];
	TPtr8 gopDes = TPtr8(gopBuffer,0,gopBufferSize);

	TRect screenRect(0,0,TestScreenSize.iWidth,TestScreenSize.iHeight);

    TRect rect;
	iFixedRegionPtr = &iFixedRegion;
	
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iScreenGc->SetClippingRegion(iFixedRegionPtr);

	TRgb white;
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap3.iGc->SetBrushColor(white);
	iBitmap3.iGc->SetClippingRegion(iSubRegionPtr);

	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap4.iGc->SetBrushColor(white);
	iBitmap4.iGc->SetClippingRegion(iSubRegionPtr);

	TRgb color = TRgb(Random(256),Random(256),Random(256));

	//Source bitmap
	iBitmap7.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap7.iGc->SetBrushColor(color);
	iBitmap7.iGc->DrawRect(iBitmap7.iBitmap->SizeInPixels());
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	
	iAlphaBitmap.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);


    //create 10 gray level for the alpha bitmap
    for(TInt i=0; i<10;i++)
		{
		TRect tmpRect(0,10*i,200,10+10*i);
		TInt index = 255-(20*i);
		TRgb tmpRgb;
        iAlphaBitmap.iGc->SetBrushColor(tmpRgb.Gray256(index));
        iAlphaBitmap.iGc->DrawRect(tmpRect);
		}

	TAcceleratedBitmapSpec bitmap7Spec(iBitmap7.iBitmap);
	TAcceleratedBitmapSpec alphaBitmapSpec(iAlphaBitmap.iBitmap);

   	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize.iWidth);
		rect.iTl.iY = Random(TestBitmapSize.iHeight);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize.iWidth);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize.iHeight);

		//Random destination point
		TPoint pointDest(Random(TestScreenSize.iWidth),Random(TestScreenSize.iHeight));

		//alpha blitting with Graphics Contex
		iBitmap1.iGc->BitBltMasked(pointDest,iBitmap7.iBitmap,rect,iAlphaBitmap.iBitmap,ETrue);
		iScreenGc->BitBltMasked(pointDest,iBitmap7.iBitmap,rect,iAlphaBitmap.iBitmap,ETrue);
		iBitmap2.iGc->BitBlt(TPoint(0,0),*iScreenGc,screenRect);

		TEST(iBitmap1.Compare(&iBitmap2));

		rect.Intersection(screenRect);
		//alpha blitting with Graphics accelerator
		TGopBitBltAlphaBitmap gopBitBltAlphaBitmap(pointDest,bitmap7Spec,rect,alphaBitmapSpec);
		gopDes.Append((TUint8*)&gopBitBltAlphaBitmap,sizeof(gopBitBltAlphaBitmap));
		TInt error = iGraphicsAccelerator1->Operation(gopBitBltAlphaBitmap);
		if(error == KErrNotSupported)
			{
			WARN_PRINTF1(_L("  Not Supported."));
			goto done;
			}
		iBitmap3.iGc->DrawRect(iBitmap3.iBitmap->SizeInPixels());
		TEST(iBitmap2.Compare(&iBitmap3));
		}
	iGraphicsAccelerator2->Operation(gopDes);
	iBitmap4.iGc->DrawRect(iBitmap4.iBitmap->SizeInPixels());
	TEST(iBitmap3.Compare(&iBitmap4));
done:
	delete gopBuffer;
	}
	
void CTAccelerator::TestScreenAlphaBlendTwoBitmaps()
	{
	TRect screenRect(0,0,TestBitmapSize.iWidth,TestBitmapSize.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;
	
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	TRgb color = TRgb(Random(256),Random(256),Random(256));

	// First Source bitmap
	iBitmap7.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap7.iGc->SetBrushColor(color);
	iBitmap7.iGc->DrawRect(screenRect);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	
	// Get a new random color
	color = TRgb(Random(256),Random(256),Random(256));
	
	// Second Source bitmap
	iBitmap8.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap8.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap8.iGc->SetBrushColor(color);
	iBitmap8.iGc->DrawRect(screenRect);
	iBitmap8.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	iAlphaBitmap.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	//create 10 gray level for the alpha bitmap
	for(TInt i=0; i<10;i++)
		{
		TRect tmpRect(0,10*i,200,10+10*i);
		TInt index = 255-(20*i);
		TRgb tmpRgb;
		iAlphaBitmap.iGc->SetBrushColor(tmpRgb.Gray256(index));
		iAlphaBitmap.iGc->DrawRect(tmpRect);
		}
    
	TAcceleratedBitmapSpec bitmap7Spec(iBitmap7.iBitmap);
	TAcceleratedBitmapSpec bitmap8Spec(iBitmap8.iBitmap);
	TAcceleratedBitmapSpec alphaBitmapSpec(iAlphaBitmap.iBitmap);

	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	for(TInt n=0; n<100; n++)
		{
		rect.iTl.iX = Random(TestBitmapSize.iWidth - 1); 
		rect.iTl.iY = Random(TestBitmapSize.iHeight - 1);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize.iWidth - rect.iTl.iX);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize.iHeight - rect.iTl.iY);

		TPoint pointDest(Random(TestBitmapSize.iWidth),Random(TestBitmapSize.iHeight));
		TPoint pointSource(Random(TestBitmapSize.iWidth),Random(TestBitmapSize.iHeight));
		TPoint pointAlpha(Random(TestBitmapSize.iWidth - rect.Width()),Random(TestBitmapSize.iHeight - rect.Height()));

		//alpha blending Graphics Context - wrong argument cause skip to next test
		TInt error = iBitmap1.iGc->AlphaBlendBitmaps(pointDest,iBitmap7.iBitmap,iBitmap8.iBitmap,rect,pointSource,iAlphaBitmap.iBitmap,pointAlpha);
		// AlphaBlendBitmaps can return KErrArgument based on the input data - in that case, continue
		if(error!=KErrNone)
			{
			continue;
			}
		iScreenGc->AlphaBlendBitmaps(pointDest,iBitmap7.iBitmap,iBitmap8.iBitmap,rect,pointSource,iAlphaBitmap.iBitmap,pointAlpha);
		iBitmap2.iGc->BitBlt(TPoint(0,0),*iScreenGc,screenRect);
		TEST(iBitmap1.Compare(&iBitmap2));
		}
	}

void CTAccelerator::TestScreenAlphaBlendOneBitmap()
	{
	TRect screenRect(0,0,TestBitmapSize.iWidth,TestBitmapSize.iHeight);

	TRect rect;
	iFixedRegionPtr = &iFixedRegion;
	
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);

	TRgb color = TRgb(Random(256),Random(256),Random(256));

	// First Source bitmap
	iBitmap7.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap7.iGc->SetBrushColor(color);
	iBitmap7.iGc->DrawRect(screenRect);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	
	// Get a new random color
	color = TRgb(Random(256),Random(256),Random(256));

	iAlphaBitmap.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	//create 10 gray level for the alpha bitmap
	for(TInt i=0; i<10;i++)
		{
		TRect tmpRect(0,10*i,200,10+10*i);
		TInt index = 255-(20*i);
		TRgb tmpRgb;
		iAlphaBitmap.iGc->SetBrushColor(tmpRgb.Gray256(index));
		iAlphaBitmap.iGc->DrawRect(tmpRect);
		}
    
	TAcceleratedBitmapSpec bitmap7Spec(iBitmap7.iBitmap);
	TAcceleratedBitmapSpec alphaBitmapSpec(iAlphaBitmap.iBitmap);

	iAlphaBitmap.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	for(TInt n=0; n<100; n++)
		{
		if(n==0) continue;
		rect.iTl.iX = Random(TestBitmapSize.iWidth - 1); 
		rect.iTl.iY = Random(TestBitmapSize.iHeight - 1);
		rect.iBr.iX = rect.iTl.iX+1+Random(TestBitmapSize.iWidth - rect.iTl.iX);
		rect.iBr.iY = rect.iTl.iY+1+Random(TestBitmapSize.iHeight - rect.iTl.iY);

		TPoint pointDest(Random(TestBitmapSize.iWidth),Random(TestBitmapSize.iHeight));
		TPoint pointAlpha(Random(TestBitmapSize.iWidth - rect.Width() - 1),Random(TestBitmapSize.iHeight - rect.Height() - 1));

		//alpha blending Graphics Context - wrong argument cause skip to next test
		TInt rc = iBitmap1.iGc->AlphaBlendBitmaps(pointDest,iBitmap7.iBitmap,rect,iAlphaBitmap.iBitmap,pointAlpha);
		if(rc!=KErrNone)
			{
			continue;
			}
		iScreenGc->AlphaBlendBitmaps(pointDest,iBitmap7.iBitmap,rect,iAlphaBitmap.iBitmap,pointAlpha);
		iBitmap2.iGc->BitBlt(TPoint(0,0),*iScreenGc,screenRect);
		TEST(iBitmap1.Compare(&iBitmap2));
		}
	}

void CTAccelerator::resetColorBitmaps()
	{
	TRgb color;
	iFixedRegionPtr = &iDefaultRegion;

	if(iHwScreenDevice)
		{
		iFixedRegionPtr = &iDefaultScreenRegion;
		iScreenGc->SetPenStyle(CGraphicsContext::ENullPen);
		iScreenGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iScreenGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
		iScreenGc->SetClippingRegion(iFixedRegionPtr);
		iScreenGc->SetBrushColor(color);
		iScreenGc->DrawRect(TestScreenSize);
		iScreenGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		}

	iBitmap1.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap1.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap1.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap1.iGc->SetBrushColor(color);
	TRect rect1(iBitmap1.iBitmap->SizeInPixels());
	iBitmap1.iGc->DrawRect(rect1);
	iBitmap1.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	iBitmap2.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap2.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap2.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap2.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap2.iGc->SetBrushColor(color);
	TRect rect2(iBitmap2.iBitmap->SizeInPixels());
	iBitmap2.iGc->DrawRect(rect2);
	iBitmap2.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	//GA1
	iBitmap3.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap3.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap3.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap3.iGc->SetBrushColor(color);
	TRect rect3(iBitmap3.iBitmap->SizeInPixels());
	iBitmap3.iGc->DrawRect(rect3);
	iBitmap3.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	//GA2
	iBitmap4.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap4.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap4.iGc->SetClippingRegion(iFixedRegionPtr);
	iBitmap4.iGc->SetBrushColor(color);
	TRect rect4(iBitmap4.iBitmap->SizeInPixels());
	iBitmap4.iGc->DrawRect(rect4);
	iBitmap4.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	iBitmap5.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap5.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap5.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap5.iGc->SetBrushColor(color);
	TRect rect5(iBitmap5.iBitmap->SizeInPixels());
	iBitmap5.iGc->DrawRect(rect5);
	iBitmap5.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	iBitmap6.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap6.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap6.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap6.iGc->SetBrushColor(color);
	TRect rect6(iBitmap6.iBitmap->SizeInPixels());
	iBitmap6.iGc->DrawRect(rect6);
	iBitmap6.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);

	iBitmap7.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    iBitmap7.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap7.iGc->SetBrushColor(color);
	TRect rect7(iBitmap7.iBitmap->SizeInPixels());
	iBitmap7.iGc->DrawRect(rect7);
	iBitmap7.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	
	iBitmap8.iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBitmap8.iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitmap8.iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iBitmap8.iGc->SetBrushColor(color);
	TRect rect8(iBitmap8.iBitmap->SizeInPixels());
	iBitmap8.iGc->DrawRect(rect8);
	iBitmap8.iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	}

void CTAccelerator::TestScreenRotation()
	{
	// Checks that clearing a rotated screen doesn't blow-up
	iScreenGc->SetOrientation(CFbsBitGc::EGraphicsOrientationRotated90);

	iScreenGc->Reset();
	// fill screen using graphics accelerator
	iScreenGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	iScreenGc->SetPenStyle(CFbsBitGc::ENullPen);
	iScreenGc->DrawRect(TRect(0,0,KMaxTInt/2,KMaxTInt/2));

	// restore orientation
	iScreenGc->SetOrientation(CFbsBitGc::EGraphicsOrientationNormal);
	iScreenGc->Reset();
	}

void CTAccelerator::TestUserDisplayMode()
	{
	iScreenGc->Reset();
	// Change to Gray4 mode
	iScreenGc->SetUserDisplayMode(EGray4);
	// fill screen using graphics accelerator
	iScreenGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	iScreenGc->SetBrushColor(TRgb(0x44,0x44,0x44)); // should get 'rounded up' to 0x55,0x55,0x55 when drawn
	iScreenGc->SetPenStyle(CFbsBitGc::ENullPen);
	iScreenGc->DrawRect(TRect(0,0,KMaxTInt/2,KMaxTInt/2));

	TRgb pixel;
	iHwScreenDevice->GetPixel(pixel,TPoint(0,0));
	TRgb checkValue(0x555555,0xff);
	if (iHwScreenDevice->DisplayMode()==EColor64K)
		checkValue=TRgb::Color64K(checkValue.Color64K());
	TEST(pixel==checkValue);  // check pixel color is that of a Gray4 one

	// Restore user display mode
	iScreenGc->Reset();
	}

static void CheckDisplayMode()
	{
	TInt i;
	for(i=0; i<KNumDisplayModes; ++i)
		{
		TDisplayMode mode = TestDisplayModes[i].iMode;
		CFbsScreenDevice* device = NULL;
		TInt err = KErrNone;
		TRAP(err, device = CFbsScreenDevice::NewL(_L("scdv"), mode));
		if (err == KErrNone)
			{
			TestDisplayModes[i].iEnabled = ETrue;
			delete device;
			}
		}
	}
	
//--------------
__CONSTRUCT_STEP__(Accelerator)

void CTAcceleratorStep::TestSetupL()
	{
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	
	CheckDisplayMode();
	}
	
void CTAcceleratorStep::TestClose()
	{
	RFbsSession::Disconnect();
	}





