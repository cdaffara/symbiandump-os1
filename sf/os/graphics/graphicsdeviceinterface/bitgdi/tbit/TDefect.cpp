// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hal.h>
#include <bautils.h>
#include <bitdraw.h>
#include <s32mem.h>
#include "TDefect.h"
#include <graphics/bitmap.inl>
#include <graphics/gdi/gdiconsts.h>

_LIT(KRamBitmapOnZ, "z:\\system\\data\\16RAM.mbm");
_LIT(KRamCBitmapOnZ, "z:\\system\\data\\16RAMC.mbm");

_LIT(KRamBitmap, "c:\\16RAM.mbm");
_LIT(KRamCBitmap, "c:\\16RAMC.mbm");

_LIT(KZeroSizeMbmRam, "c:\\ZeroSizeRam.mbm");
_LIT(KZeroSizeMbmZ, "z:\\system\\data\\ZeroSizeRam.mbm");

_LIT(KRomBitmap, "z:\\system\\data\\16ROM.mbm");
_LIT(KRomCBitmap, "z:\\system\\data\\16ROMC.mbm");

_LIT(KRomRsc_RomMbm, "z:\\system\\data\\RomRsc_RomMbm.rsc");
_LIT(KRomRsc_RomMbm2, "z:\\system\\data\\RomRsc_RomMbm2.rsc");
_LIT(KRamRsc_RamMbmOnZ, "z:\\system\\data\\RamRsc_RamMbm.rsc");
_LIT(KRamRsc_RamMbm2OnZ, "z:\\system\\data\\RamRsc_RamMbm2.rsc");
_LIT(KRamRsc_RamMbm, "c:\\RamRsc_RamMbm.rsc");
_LIT(KRamRsc_RamMbm2, "c:\\RamRsc_RamMbm2.rsc");
_LIT(KRscFileHeader, "z:\\system\\data\\RscHeader2.bin");
_LIT(KRscFileData, "z:\\system\\data\\DummyRscFile.rsc");
_LIT(KRomRsc_RamMbm, "z:\\system\\data\\RomRsc_RamMbm.rsc");
_LIT(KRamRsc_RomMbmOnZ, "z:\\system\\data\\RamRsc_RomMbm.rsc");
_LIT(KRamRsc_RomMbm, "c:\\RamRsc_RomMbm.rsc");
_LIT(KRamRsc_RomMbm2OnZ, "z:\\system\\data\\RamRsc_RomMbm2.rsc");
_LIT(KRamRsc_RomMbm2, "c:\\RamRsc_RomMbm2.rsc");
_LIT(KRomRsc_RamMbm2, "z:\\system\\data\\RomRsc_RamMbm2.rsc");

_LIT(KComprRamBitmap16, "z:\\system\\data\\paint16ramc.mbm");
_LIT(KComprRomBitmap16, "z:\\system\\data\\paint16romc.mbm");
_LIT(KComprRamBitmap24, "z:\\system\\data\\paint24ramc.mbm");
_LIT(KComprRomBitmap24, "z:\\system\\data\\paint24romc.mbm");

_LIT(KComprRamBitmap8, "z:\\system\\data\\8ramc.mbm");
_LIT(KComprRomBitmap8, "z:\\system\\data\\8romc.mbm");
_LIT(KComprRamBitmap12, "z:\\system\\data\\12ramc.mbm");
_LIT(KComprRomBitmap12, "z:\\system\\data\\12romc.mbm");

_LIT(KTestBmp, "z:\\system\\data\\BmCTest.mbm");

_LIT(KBitBltSrcBmp, "z:\\system\\data\\BitBltSrc.mbm");
_LIT(KBitBltMaskedBmp, "z:\\system\\data\\BitBltMasked.mbm");

_LIT(KBmpCompr12, "z:\\system\\data\\compr12_2.mbm");
_LIT(KBmpCompr16, "z:\\system\\data\\compr16_2.mbm");
_LIT(KBmpZCompr12, "z:\\system\\data\\racompr12_2.mbm");
_LIT(KBmpZCompr16, "z:\\system\\data\\racompr16_2.mbm");
_LIT(KBmpCCompr12, "c:\\racompr12_2.mbm");
_LIT(KBmpCCompr16, "c:\\racompr16_2.mbm");

_LIT(KBmp16_1, "c:\\Mbm16_1.mbm");
_LIT(KBmp16_2, "c:\\Mbm16_2.mbm");
_LIT(KBmp16_3, "c:\\Mbm16_3.mbm");

_LIT(KRam24BitmapOnZ, "z:\\system\\data\\24RAM.mbm");
_LIT(KRamC24BitmapOnZ, "z:\\system\\data\\24RAMC.mbm");

_LIT(KRam24Bitmap, "c:\\24RAM.mbm");
_LIT(KRamC24Bitmap, "c:\\24RAMC.mbm");

_LIT(KRom24Bitmap, "z:\\system\\data\\24ROM.mbm");
_LIT(KRomC24Bitmap, "z:\\system\\data\\24ROMC.mbm");

//Note: Do not name functions with Defect numbers, instead give a meaningful name.

//
//
//Note: Some display modes will be scaled - see 
//CTDefect::CreateScreenDeviceL(TDisplayMode aDisplayMode, TAllowScaling aScaling),
//SetScalingFactor() call

//
//
//If we have to handle RAM located file with an embedded ROM mbm file section - 
//KRomMBMInRamRSC should be ETrue.
//If it is not allowed to do that - KRomMBMInRamRSC should be EFalse.
//#pragma warning(disable : 4127)   //conditional expression is constant
//Constant KRomMBMInRamRSc removed temporarily due to invalid test scenario
//LOCAL_D const TBool KRomMBMInRamRSC = EFalse;

//
//

//
//
//Globals
LOCAL_D RFs		TheFs;
//Delay time in microseconds, used at the end of some test methods
const TInt		KDelayTime = 1;//1500000;

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
LOCAL_C void CopyDataFiles()
	{
	BaflUtils::CopyFile(TheFs, KRamBitmapOnZ, KRamBitmap);
	BaflUtils::CopyFile(TheFs, KRamCBitmapOnZ, KRamCBitmap);
	BaflUtils::CopyFile(TheFs, KRamRsc_RamMbmOnZ, KRamRsc_RamMbm);
	BaflUtils::CopyFile(TheFs, KRamRsc_RamMbm2OnZ, KRamRsc_RamMbm2);
	BaflUtils::CopyFile(TheFs, KRamRsc_RomMbmOnZ, KRamRsc_RomMbm);
	BaflUtils::CopyFile(TheFs, KRamRsc_RomMbm2OnZ, KRamRsc_RomMbm2);
	BaflUtils::CopyFile(TheFs, KZeroSizeMbmZ, KZeroSizeMbmRam);
	BaflUtils::CopyFile(TheFs, KBmpZCompr12, KBmpCCompr12);
	BaflUtils::CopyFile(TheFs, KBmpZCompr16, KBmpCCompr16);
	BaflUtils::CopyFile(TheFs, KRamBitmap, KBmp16_1);
	BaflUtils::CopyFile(TheFs, KRamBitmap, KBmp16_2);
	BaflUtils::CopyFile(TheFs, KRamBitmap, KBmp16_3);
	BaflUtils::CopyFile(TheFs, KRam24BitmapOnZ, KRam24Bitmap);
	BaflUtils::CopyFile(TheFs, KRamC24BitmapOnZ, KRamC24Bitmap);
	}

//Delete data files used by the test
LOCAL_C void DeleteDataFiles()
	{
	DeleteDataFile(KRamBitmap);
	DeleteDataFile(KRamCBitmap);
	DeleteDataFile(KRamRsc_RamMbm);
	DeleteDataFile(KRamRsc_RamMbm2);
	DeleteDataFile(KRamRsc_RomMbm);
	DeleteDataFile(KRamRsc_RomMbm2);
	DeleteDataFile(KRam24Bitmap);
	DeleteDataFile(KRamC24Bitmap);
	DeleteDataFile(_L("c:\\temp.mbm"));
	DeleteDataFile(_L("c:\\temp2.mbm"));
	DeleteDataFile(KZeroSizeMbmRam );
	DeleteDataFile(KBmpCCompr12);
	DeleteDataFile(KBmpCCompr16);
	DeleteDataFile(KBmp16_1);
	DeleteDataFile(KBmp16_2);
	DeleteDataFile(KBmp16_3);
	}

//
//
//Global functions

//Return file size in bytes of "aFileName" file.
LOCAL_C TInt FileSizeL(const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Open(TheFs, aFileName, EFileRead));
	CleanupClosePushL(file);
	TInt size = 0;
	User::LeaveIfError(file.Size(size));
	CleanupStack::PopAndDestroy(&file);
	return size;
	}

//Creates CFbsBitGcBitmap instance and loads it with bitmap N#0 from "aFileName" file.
//CFbsBitGcBitmap instance will be pushed in the cleanup stack.
CFbsBitGcBitmap* LoadBitmapLC(const TDesC& aFileName)
	{
	CFbsBitGcBitmap* bmp = new (ELeave) CFbsBitGcBitmap;
	CleanupStack::PushL(bmp);
	User::LeaveIfError(bmp->Load(aFileName, 0));
	return bmp;
	}

//Creates "SIZE" CFbsBitGcBitmap instance and loads them with bitmap N#0 from "aBmpFiles" 
//files. CFbsBitGcBitmap instances will be pushed in the cleanup stack.
template <TInt SIZE> void LoadBitmapsLC(CFbsBitGcBitmap* (*aBmp)[SIZE], TPtrC aBmpFiles[])
	{
	for(TInt ii=0;ii<SIZE;ii++)
		{
		(*aBmp)[ii] = LoadBitmapLC(aBmpFiles[ii]);
		}
	}

//Displays "aBmp" array of bitmaps.
template <TInt SIZE> void DisplayBitmaps(CFbsBitGcBitmap* (*aBmp)[SIZE], 
										 CFbsScreenDevice* aDevice, 
										 CFbsBitGc* aGc)
	{
	for(TInt ii=0;ii<SIZE;ii++)
		{
		TSize size = (*aBmp)[ii]->SizeInPixels();
		aGc->Clear();
		aGc->DrawBitmap(TRect(size), (*aBmp)[ii]);
		aDevice->Update();
		User::After(KDelayTime);
		}
	}

//Destroys "aBmp" array of bitmaps.
template <TInt SIZE> void DestroyBitmaps(CFbsBitGcBitmap* (*aBmp)[SIZE])
	{
	for(TInt ii=(SIZE-1);ii>-1;ii--)
		{
		CleanupStack::PopAndDestroy((*aBmp)[ii]);
		}
	}

//Compares 2 bitmaps from "aBmp" array.
//The first bitmap index is aBmp1, the second bitmap index is aBmp2.
//The function compares bitmaps line by line and asserts their pixel values.
template <TInt SIZE, TInt MAXLEN> void CompareBitmaps(CFbsBitGcBitmap* (*aBmp)[SIZE], 
													  TDisplayMode aDispMode,
													  TInt aBmp1, 
													  TInt aBmp2,
													  CTGraphicsBase *aTest)
	{
	TUint8 line1[MAXLEN];
	TUint8 line2[MAXLEN];
	TPtr8 p1(line1, MAXLEN, MAXLEN);
	TPtr8 p2(line2, MAXLEN, MAXLEN);
	TSize size1 = (*aBmp)[aBmp1]->SizeInPixels();
	TSize size2 = (*aBmp)[aBmp2]->SizeInPixels();
	TInt minHeight = Min(size1.iHeight, size2.iHeight);
	TInt minWidth = Min(size1.iWidth, size2.iWidth);
	for(TInt ii=0;ii<minHeight;ii++)
		{
		(*aBmp)[aBmp1]->GetScanLine(p1, TPoint(0,ii), minWidth, aDispMode);
		(*aBmp)[aBmp2]->GetScanLine(p2, TPoint(0,ii), minWidth, aDispMode);
		aTest->TEST(p1.Length() == p2.Length());
		for(TInt j=0;j<p1.Length();j++)
			{
			aTest->TEST(line1[j] == line2[j]);
			}
		}
	}

//Compares the Alpha Channel Values of the First Pixel in the two bitmaps.
//The pixel value is obtained from DataAddress().
TBool CompareFirstPixelAlphaChannel(CFbsBitmap* aBmp1, CFbsBitmap* aBmp2)
	{
	return ( (*aBmp1->DataAddress() & 0xff000000) == (*aBmp2->DataAddress() & 0xff000000) );
	}

//
//

CTDefect::CTDefect(CTestStep* aStep) :
	CTGraphicsBase(aStep), 
	iDevice(NULL),
	iBmpDevice(NULL),
	iGc(NULL),
	iBitmap(NULL),
	iSize(0, 0),
	iCurrentMode(ENone)
	{
	}

CTDefect::~CTDefect()
	{
	DestroyFont();
	DeleteBitmap();
	DeleteGraphicsContext();
	DeleteBitmapDevice();
	DeleteScreenDevice();
	}

void CTDefect::ConstructL()
	{
	INFO_PRINTF1(_L("Create screen device"));
	CreateScreenDeviceL();

	TUint32* romAddress = NULL;
	if(!CFbsBitmap::IsFileInRom(KRomBitmap, romAddress)) //check any rom bitmap
		{
		INFO_PRINTF2(_L("Skipping tests of ROM bitmaps since file \"%S\" is reported to not be a ROM bitmap."),
				&KRomBitmap);
		INFO_PRINTF1(_L("This should only occur on non-XIP ROMs, e.g. NAND ROMs, where ROM bitmaps aren't supported."));
		iSkipRomBitmapTests = ETrue;
		}
	}

void CTDefect::CreateScreenDeviceL(TAllowScaling aScaling)
	{
	DeleteGraphicsContext();
	DeleteScreenDevice();
	TDisplayMode mode[] = {EColor256, EColor4K, EColor16M, EColor16MU, EColor64K, 
						   EGray256, EGray16, EGray4, EGray2, EColor16, EColor16MA, EColor16MAP};
	TInt ii;
	TInt err = KErrNotSupported;
	for(ii=0;(ii<TInt(sizeof(mode)/sizeof(mode[0]))) && (err == KErrNotSupported);++ii)
		{
		err = CreateScreenDeviceL(mode[ii], aScaling);
		}
	if (err==KErrNone)
		{
		_LIT(KLog,"Sucessfully created screen device %S");
		INFO_PRINTF2(KLog,&ColorModeName(mode[ii]));
		}
	TEST(err == KErrNone);
	}

TInt CTDefect::CreateScreenDeviceL(TDisplayMode aDisplayMode, TAllowScaling aScaling)
	{
	DeleteGraphicsContext();
	DeleteScreenDevice();
	TRAPD(err, iDevice=CFbsScreenDevice::NewL(KNullDesC,aDisplayMode));
	if(err == KErrNotSupported)
		{
		return err;
		}
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to create screen device %S return value %d");
		INFO_PRINTF3(KLog,&ColorModeName(aDisplayMode),err);
		}
	else
		{
		_LIT(KLog,"Created Screen Device  iDevice=0x%x");
		INFO_PRINTF2(KLog,iDevice);
		}
	TEST(err == KErrNone);
	err = iDevice->CreateContext((CGraphicsContext*&)iGc);
	TEST(err == KErrNone);
	if(aScaling == EScalingAllowed)
		{
		iDevice->SetScalingFactor(TPoint(31, 11), 2, 2, 1, 1);
		iGc->Activate(iDevice);
		}
	iGc->SetUserDisplayMode(aDisplayMode);
	iDevice->ChangeScreenDevice(NULL);
	iDevice->SetAutoUpdate(EFalse);
	iSize = iDevice->SizeInPixels();
	iCurrentMode = aDisplayMode;
	return err;
	}

void CTDefect::RunTestCaseL(TInt aCurTestCase)
	{
	((CTDefectStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0045"));
		INFO_PRINTF1(_L("DEF014364"));
		DEF014364L();
		INFO_PRINTF1(_L("Cleared DEF014364\n"));
		break;
	case 2:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0046"));
		INFO_PRINTF1(_L("DEF017758"));
		DEF017758();
		INFO_PRINTF1(_L("Cleared DEF017758\n"));
		break;
	case 3:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0047"));
		INFO_PRINTF1(_L("16 bpp bitmap comperssion"));
		BitmapCompressionL(E16BppBmpType);
		break;
	case 4:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0047"));
		INFO_PRINTF1(_L("24 bpp bitmap comperssion"));
		BitmapCompressionL(E24BppBmpType);
		break;
	case 5:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0048"));
		INFO_PRINTF1(_L("New RSC file format"));
		NewRscFormatL();
		break;
	case 6:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0049"));
		INFO_PRINTF1(_L("INC031920 - 16 bpp bitmaps"));
		INC031920L(E16BppBmpType, ETrue);
		INC031920L(E16BppBmpType, EFalse);
		break;
	case 7:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0049"));
		INFO_PRINTF1(_L("INC031920 - 24 bpp bitmaps"));
		INC031920L(E24BppBmpType, ETrue);
		INC031920L(E24BppBmpType, EFalse);
		break;
	case 8:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0050"));
		INFO_PRINTF1(_L("8 bit bitmap compression - ROM"));
		Test8bitBmpComressionL();
		break;
	case 9:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0051"));
		INFO_PRINTF1(_L("12 bit bitmap compression - ROM"));
		Test12bitBmpComressionL();
		break;
	case 10:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0044"));
		INFO_PRINTF1(_L("ZeroSizeBitmap"));
		ZeroSizeBitmap();
		break;
	case 11:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0052"));
		INFO_PRINTF1(_L("DEF034134"));
		DEF034134L();
		break;
	case 12:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0053"));
		INFO_PRINTF1(_L("Test tiling occurs correctly"));
		TestTilingHorizontalAndVerticalL();
		break;
	case 13:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0054"));
		INFO_PRINTF1(_L("Borderline cases for DrawPie"));
		DrawPieBorderlineCasesL();
		break;
	case 14:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0055"));
		INFO_PRINTF1(_L("INC037380"));
		INC037380L();
		break;
	case 15:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0056"));
		INFO_PRINTF1(_L("INC037474"));
		INC037474L();
		break;
	case 16:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0058"));
		INFO_PRINTF1(_L("CFbsBitmap::SetDisplayMode() test"));
		TestSettingDisplayModeL();
		break;
	case 17:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0059"));
		INFO_PRINTF1(_L("CFbsBitmap::SetDisplayMode() test 2"));
		TestSettingDisplayMode2L();
		break;
	case 18:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0060"));
		INFO_PRINTF1(_L("CFbsBitmap::SwappingWidthAndHeight() test"));
		TestSwappingWidthAndHeightL();
		break;
	case 19:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0057"));
		INFO_PRINTF1(_L("INC037370"));
		INC037370L();
		break;
	case 20:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0061"));
		INFO_PRINTF1(_L("DEF038774"));
		DEF038774L();
		break;
	case 21:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0062"));
		INFO_PRINTF1(_L("INC070043L"));
		INC070043L();		
		break;
	case 22:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0043"));
		INFO_PRINTF1(_L("OOM test"));
		TestOom();	
		break;
	case 23:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0063"));
		INFO_PRINTF1(_L("Bitmaps Swapping Test"));
		BitmapsSwappingTest();
		break;
	case 24:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0064"));
		INFO_PRINTF1(_L("INC093055"));
		INC093055L();
		break;
	case 25:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0016"));
		INFO_PRINTF1(_L("PDEF103809"));
		PDEF103809L();
		break;
	case 26:
/**
@SYMTestCaseID		GRAPHICS-BITGDI-0018

@SYMDEF             DEF104856

@SYMTestCaseDesc    Test that the bitmap and mask are drawn correctly testing
	CFbsBitGc::DoBitBltMaskedFlicker in order to test the DEF104856 fix
	introduced in CFbsBitGc::DoBitBltMaskedFlicker method

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create compressed EColor64K bitmap and mask and draw bitmap calling
	CFbsBitGc::BitBltMasked with an offscreen bitmap device

@SYMTestExpectedResults The resulting bitmap is not altered or corrupted by the code module
**/
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0018"));
		INFO_PRINTF1(_L("BitBltMasked Flicker Test - DEF104856"));
		TestDoBltMaskedL(EColor64K, EColor64K, EColor64K, TSize(16,16), EFalse, ETrue);
		break;
	case 27:
/**
@SYMTestCaseID		GRAPHICS-BITGDI-0019

@SYMDEF             DEF104856

@SYMTestCaseDesc    Test that the bitmap and mask are drawn correctly testing
	CFbsBitGc::DoBitBltMaskedNonFlicker in order to test the DEF104856 fix
	introduced in CFbsBitGc::DoBitBltMaskedNonFlicker method

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create compressed EColor64K bitmap and mask and draw bitmap calling
	CFbsBitGc::BitBltMasked with a screen device and brush style EVerticalHatchBrush

@SYMTestExpectedResults The resulting bitmap is not altered or corrupted by the code module
**/
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0019"));
		INFO_PRINTF1(_L("BitBltMasked non Flicker Test - DEF104856"));
		TestDoBltMaskedL(EColor64K, EColor64K, EColor64K, TSize(16,16), ETrue, ETrue);
		break;
	case 28:
/**
@SYMTestCaseID		GRAPHICS-BITGDI-0092
 
@SYMDEF             DEF108338

@SYMTestCaseDesc    Test that the bitmap is drawn correctly by testing
					that the correct area of the mask is used for masking.
					CFbsBitGc::DoBitBltMaskedFlicker is called in this case.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create compressed EColor64K bitmap and EGray2 mask and draw bitmap calling
					CFbsBitGc::BitBltMasked with a screen device. The drawing is done in a loop
					to simulate the scrolling effect as in a browser.

@SYMTestExpectedResults All the stripes should pass the integrity check.
**/
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0092"));
		INFO_PRINTF1(_L("BitBltMasked Flicker - DEF108338"));
		TestDoBltMaskedL(EColor64K, EGray2, EColor16MU, TSize(96,96), EFalse, EFalse);
		break;
	case 29:
/**
@SYMTestCaseID		GRAPHICS-BITGDI-0093

@SYMDEF             DEF108338

@SYMTestCaseDesc    Test that the bitmap is drawn correctly by testing
					that the correct area of the mask is used for masking.
					CFbsBitGc::DoBitBltMaskedNonFlicker is called in this case.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create compressed EColor64K bitmap and EGray2 mask and draw bitmap calling
					CFbsBitGc::BitBltMasked with a screen device. The drawing is done in a loop
					to simulate the scrolling effect as in a browser.

@SYMTestExpectedResults All the stripes should pass the integrity check.
**/
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0093"));
		INFO_PRINTF1(_L("BitBltMasked Non Flicker - DEF108338"));
		//skip the test if the screen does not have a fast blit interface.
		if(CFbsDrawDevice::DisplayMode16M() == ENone)
			{
			INFO_PRINTF1(_L("Test skipped - The screen does not have a fast blit interface!"));
			break;
			}
		TestDoBltMaskedL(EColor64K, EGray2, EColor16MU, TSize(96,96), ETrue, EFalse);
		break;
	case 30:
/**
@SYMTestCaseID		GRAPHICS-BITGDI-0020

@SYMDEF             DEF103736

@SYMTestCaseDesc   	Test to check that EColor16MAP mode correctly draws bitmaps when using a EColor16MAP mask

@SYMTestPriority    Normal

@SYMTestStatus      Implemented

@SYMTestActions     Create compressed EColor16MAP bitmap and mask and draw bitmap calling
					CFbsBitGc::BitBltMasked with an offscreen bitmap device

@SYMTestExpectedResults The resulting bitmap is not altered or corrupted by the code module
**/
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0020"));
		INFO_PRINTF1(_L("BitBltMasked non Flicker Test ECOLOR16MAP - DEF103736"));
		TestDoBltMaskedL(EColor16MAP, EColor16MAP, EColor16MAP, TSize(16,16), EFalse, ETrue);
		break;
	case 31:
/**
@SYMTestCaseID		GRAPHICS-BITGDI-0094

@SYMDEF             PDEF108863

@SYMTestCaseDesc    Test that CFbsBitGc::DoBitBltAlpha() function behaves correctly
                    when source is EColor16MU, mask is EGray256, both source and mask are compressed
                    and the screen driver has no MFastBlit interface.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create compressed EColor16MU bitmap and EGray256 mask and draw bitmap calling
                    CFbsBitGc::BitBltMasked with a screen device. Where screen driver has no MFastBlit interface.

@SYMTestExpectedResults All the stripes should pass the integrity check.
**/
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0094"));
		INFO_PRINTF1(_L("PDEF108863 - DoBitBltAlpha for EColor16MU source, EGray256 mask and no MFastBlit interface"));
		TestDoBltMaskedL(EColor16MU, EGray256, EColor16MA, TSize(96,96), EFalse, EFalse);
		break;
	case 32:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0095"));
		INFO_PRINTF1(_L("PDEF110934 - DoBitBltMasked for mask height is less/greater than the bitmap height")); 
		TestDoBitBltMaskedSizeL();
		break;
	case 33: 
/** 
@SYMTestCaseID      GRAPHICS-BITGDI-0096

@SYMDEF             PDEF114309

@SYMTestCaseDesc    Test to check that EColor16MU mode correctly BitBltMasks when source and mask are of EColor16MA

@SYMTestPriority    Normal

@SYMTestStatus      Implemented

@SYMTestActions     Create source, mask and destination bitmaps of EColor16MA, EColor16MA and EColor16MU respectively
                    Call BitBltMaksed on the destination device and check the output

@SYMTestExpectedResults The resulting bitmap contents should match the calculated value
**/
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0096"));
        INFO_PRINTF1(_L("BitBltMasked for EColor16MU with source and mask of EColor16MA - PDEF114309"));
        TestBitBltMaskedEColor16MUL();
        break;
	case 34:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0102"));
		INFO_PRINTF1(_L("Externalize()/Internalize() of CFbsBitGcExtraData in CFbsBitmap - DEF120360"));
		ExternalizeInternalizeCFbsBitGCExtraData();
		break;
	case 35:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0086"));
        INFO_PRINTF1(_L("Test BitBlt with 16MU source & target copies rather than overwrites alpha channel - DEF121299"));
        TestBitBltOperationsOn16muUndefinedByteL();
        break;
	case 36:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0107"));
		INFO_PRINTF1(_L("Test DrawBitmap() with clipping and scaling - DEF126160"));
		TestClippedScaledBitmapL();
		break;
	case 37:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0109"));
		INFO_PRINTF1(_L("Test TestDrawBitmapTwipsNotSet() - DEF128522"));
		TestDrawBitmapTwipsNotSet();
		break;
	case 38:
		((CTDefectStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0119"));
		INFO_PRINTF1(_L("Test DrawBitmapMasked() with compressed bitmaps"));
		TestDrawBitmapMaskedL();
		break;    
	case 39:
		((CTDefectStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTDefectStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;				
		}
	((CTDefectStep*)iStep)->RecordTestResultL();
	}

void CTDefect::TestBitBltMaskedEColor16MUL() 
    { 
    TInt width = 128; 
    TInt height = 178; 
 
    TSize size(width, height); 
    TInt transpFactor = 100; 
 
    // Create a destination bitmap of EColro16MU and fill some color in it 
    CFbsBitmap* destBitmap = new CFbsBitmap(); 
    CleanupStack::PushL(destBitmap); 
    User::LeaveIfError(destBitmap->Create(size, EColor16MU)); 
    CFbsBitmapDevice* destDev = CFbsBitmapDevice::NewL(destBitmap); 
    CleanupStack::PushL(destDev); 
    CFbsBitGc* destGc = NULL; 
    User::LeaveIfError(destDev->CreateContext(destGc)); 
    CleanupStack::PushL(destGc); 
 
    destGc->SetPenStyle(CGraphicsContext::ENullPen); 
    destGc->SetBrushStyle(CGraphicsContext::ESolidBrush); 
    destGc->SetBrushColor(KRgbRed); 
    destGc->DrawRect(TRect(TPoint(), size)); 
 
    // Create a source bitmap fill it with some color and then draw it with some color with transparency on it 
    CFbsBitmap* srcBitmap = new (ELeave) CFbsBitmap; 
    CleanupStack::PushL(srcBitmap); 
    User::LeaveIfError(srcBitmap->Create(size, EColor16MA)); 
    CFbsBitmapDevice* srcDev = CFbsBitmapDevice::NewL(srcBitmap); 
    CleanupStack::PushL(srcDev); 
    CFbsBitGc* srcGc = NULL; 
    User::LeaveIfError(srcDev->CreateContext(srcGc)); 
    CleanupStack::PushL(srcGc); 
 
    srcGc->SetPenStyle(CGraphicsContext::ENullPen); 
    srcGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha); 
    srcGc->SetBrushStyle(CGraphicsContext::ESolidBrush); 
    srcGc->SetBrushColor(TRgb(0xff,0xff,0xff,transpFactor)); 
    srcGc->DrawRect(TRect(TPoint(), size)); 
 
    // Create a mask bitmap and fill with TRgb(30,30,30) 
    CFbsBitmap* maskBitmap = new (ELeave) CFbsBitmap; 
    CleanupStack::PushL(maskBitmap); 
    User::LeaveIfError(maskBitmap->Create(size, EColor16MA)); 
    CFbsBitmapDevice* maskDev = CFbsBitmapDevice::NewL(maskBitmap); 
    CleanupStack::PushL(maskDev); 
    CFbsBitGc* maskGc = NULL; 
    User::LeaveIfError(maskDev->CreateContext(maskGc)); 
    CleanupStack::PushL(maskGc); 
 
    maskGc->SetPenStyle(CGraphicsContext::ENullPen); 
    maskGc->SetBrushStyle(CGraphicsContext::ESolidBrush); 
    maskGc->SetBrushColor(TRgb( 30, 30, 30 )); 
    maskGc->DrawRect(size); 
 
    // Then call BitBltMaksed() on it 
    destGc->SetDrawMode(CGraphicsContext::EDrawModePEN); 
    destGc->SetBrushStyle(CGraphicsContext::ENullBrush); 
    destGc->BitBltMasked(TPoint(), srcBitmap, TRect(size), maskBitmap, ETrue); 
 
    // These are the values that we gave for source bitmaps 
    TUint32 srcPixel = 0x64FFFFFF;  // i,e TRgb(0xff,0xff,0xff,transpFactor) 
    TUint8 srcMask = 0x64;          // transpFactor 
    TUint32 destPixel = 0xFFFFFFFF; // White becasue nothing has drawn on it 
 
    // When we draw source bitmap with transparency, it actually alpha blends  
    // with the above parameters 
    srcPixel = AlphaBlendWithDestAlphaFF(srcPixel, srcMask, destPixel); 
 
    // For displaymode which has alpha, mask is inverted i,e 255-30 = 225 or 0xE1 
    srcMask = 0xE1;                 // mask bitmap's TRgb( 30, 30, 30 ) 
    destPixel = 0xFFFF0000;         // KRgbRed of destination bitmap 
    destPixel = AlphaBlendWithDestAlphaFF(srcPixel, srcMask, destPixel); 
 
    // Function to check the contents of the destination bitmap,  
    // if the INC111510 fix is not applied then this function will fail 
    CheckBitmapContents(destDev, width, height, destPixel); 
 
    CleanupStack::PopAndDestroy(9, destBitmap);  
    } 
 
// This function is for alpha blending when the destination alpha is FF 
TUint32 CTDefect::AlphaBlendWithDestAlphaFF(const TUint32 aSrcPixel, const TUint8 aMask, TUint32 aDestPixel) 
    { 
    if(aMask) 
        { 
        if(aMask == 0xFF) 
            return 0xff000000|aSrcPixel; 
         
        // This is the formula used for alpha blending 
        // (a)  (mask * src + (255 - mask) * dest) / 255           This is the ideal formula 
        // (b)  ((mask * (src - dest)) >> 8) + dest                A faster approximation to (a) 
        // (c)  ((mask * (256 + src - dest) >> 8) + dest - mask    Equivalent to (b) but can be used on multiple colors at a time 
         
        const TUint32 s_rb = aSrcPixel & 0x00FF00FF; 
        const TUint32 d_rb = aDestPixel & 0x00FF00FF; 
        const TUint32 mask2 = aMask | (aMask << 16); 
        const TUint32 rb = ((((aMask * ((0x01000100 + s_rb) - d_rb)) >> 8) + d_rb) - mask2) & 0x00FF00FF; 
 
        const TInt s_g = (aSrcPixel & 0xFF00) >> 8; 
        const TInt d_g = (aDestPixel & 0xFF00) >> 8; 
        const TInt g = ((aMask * (s_g - d_g)) >> 8) + d_g; 
 
        return(rb | (g<<8) | 0xff000000); 
        } 
 
    return aDestPixel; 
    } 
 
// This function is to check each pixel of the given bitmap 
void CTDefect::CheckBitmapContents(CFbsDevice* aDevice, TInt aWidth, TInt aHeight, TUint32 aExpectedValue) 
    { 
    TRgb color; 
    for(TInt y = 0; y < aHeight; ++y) 
        { 
        for(TInt x = 0; x < aWidth; ++x) 
            { 
            aDevice->GetPixel(color, TPoint(x, y)); 
            if(aExpectedValue != color.Internal()) 
                { 
                TEST(EFalse); 
                return; 
                } 
            } 
        } 
    }

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0043
 
  @SYMDEF INC042156L             

  @SYMTestCaseDesc Out of memory test
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Sets the next heap allocation to fail before attempting various bitmaps operations
 
  @SYMTestExpectedResults Allocation  should fail gracefullly
*/
void CTDefect::TestOom()
	{
	TInt err, tryCount = 0;

	INFO_PRINTF1(_L("INC042156L"));

	do
		{
		User::__DbgSetAllocFail(RHeap::EUser, RHeap::EFailNext, ++tryCount);
		User::__DbgMarkStart(RHeap::EUser);
		{
		TRAP(err, INC042156L());
		}
		User::__DbgMarkEnd(RHeap::EUser, 0);
		} 
	while(err == KErrNoMemory);

	if(err == KErrNone)
		{
		// Reset
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::ENone,1);
		}
	else
		{
		TEST(err == KErrNone);
		}

	INFO_PRINTF2(_L("- server succeeded at heap failure rate of %i"), tryCount);
	User::After(3000000);
	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0044
 
  @SYMDEF             

  @SYMTestCaseDesc tests that CBitwiseBitmap::SizeOfByteDataCompressed, SizeOfTwelveBitDataCompressed, SizeOfSixteenBitDataCompressed & 
  SizeOfTwentyFourBitDataCompressed can handle zero size bitmaps 
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Loads a zerosize bitmap the compresses it
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::ZeroSizeBitmap()
{
	DeleteBitmap();
	iBitmap = new (ELeave) CFbsBitmap;
	
	TInt err = iBitmap->Load(KZeroSizeMbmRam);
	TEST(err == KErrCorrupt);
	if(err != KErrCorrupt)
		{
		err =iBitmap->Compress();
		TEST(err == KErrNone);	
		}
}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0045
 
  @SYMDEF DEF014364            

  @SYMTestCaseDesc Tests opening of a corrupt bitmap and resizing to (0,0)
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions tries to open a corrupt bitmap then attempts to resize a new bitmap to (0,0) size
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::DEF014364L()
	{
	DeleteBitmap();
	iBitmap = new (ELeave) CFbsBitmap;
	TInt err = iBitmap->Load(_L("z:\\system\\data\\Corrupt000.wbmp"));
	//It should not be possible to open corrupted bitmap file.
	if (err!=KErrNotSupported)
		{
		_LIT(KLog,"Corrupt bitmap Corrupt000.wbmp gave wrong error code when opening  err=%d");
		INFO_PRINTF2(KLog,err);
		}
	TEST(err == KErrNotSupported);
	err = iBitmap->Create(TSize(20, 20), iCurrentMode);
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to create bitmap  err=%d");
		INFO_PRINTF2(KLog,err);
		}
	TEST(err == KErrNone);
	DeleteBitmapDevice();
	TRAP(err, iBmpDevice = CFbsBitmapDevice::NewL(iBitmap));
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to create bitmap device  err=%d");
		INFO_PRINTF2(KLog,err);
		}
	TEST(err == KErrNone);
	//It should not be possible to resize bitmap device to (0, 0) size.
	//Yes it should, setting size to 0,0 is necessary to maintain
	//6.1 application compatibility with 7.0s.
	err = iBmpDevice->Resize(TSize(0, 0));
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to resize bitmap  err=%d");
		INFO_PRINTF2(KLog,err);
		}
	TEST(err == KErrNone);
	//It should be possible to resize bitmap to (0, 0) size.
	err = iBitmap->Resize(TSize(0, 0));
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to resize bitmap2  err=%d");
		INFO_PRINTF2(KLog,err);
		}
	TEST(err == KErrNone);
	//The next call should not fail with "Divide by Zero" exception on WINS platform.
	iGc->DrawBitmap(TRect(0, 0, 10, 10), iBitmap, TRect(0, 0, 10, 10));
	}

//DEF023605 - Panic when bmp file is opened using Paint.
void CTDefect::DEF023605L()
	{
	DeleteBitmap();
	iBitmap = new (ELeave) CFbsBitmap;
	TInt err = iBitmap->Create(iSize, iCurrentMode);
	TEST(err == KErrNone);
	//It should not be possible to resize bitmap to (max_int, ...) size.
	err = iBitmap->Resize(TSize(0x7FFFFFFF, 16));
	TEST(err == KErrNone);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0046
 
  @SYMDEF DEF017758             

  @SYMTestCaseDesc Memory leak detected on target when using DirectScreenAccess
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 
  
  	//
	//The statement between __UHEAP_MARK and __UHEAP_END can cause memory leak!
	//Obviously we have some kind of first time memory allocation when the
	//method is called for the first time. Exactly the same method is called from
	//CFbsScreenDevice::ConstructL() method.
	//
	//To see the problem:
	//1. Comment HAL::Get() call into E32Main().
	//2. Build ARM4 UDEB version of TDefect and make sure it is used into rom image build
	//3. Build textshell rom image:
	//   buildrom assabet textshell_70s graphics_70s.iby graphicstests_70s.iby
	//4. Run TDefect app on the assabet.
	//   You will see a memory leak. If you uncomment HAL::Get() call into E32Main() and repeat 
	//   steps 1..4 - everything is OK.
	//PS: If you don't want to build all graphics test app, you may have to comment
	//    some IBY file includes into graphicstests_70s.iby file.
	//    If there is a memory leak - the test will still report status "OK", but you
	//    can see the memory leak message on the assabet. That's happening because 
	//    the memory leak appears after the last __UHEAP_MARKEND macro in E32Main().
	//
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::DEF017758()
	{
	TRAPD(err, DEF017758L());
	TEST(err == KErrNone);
	}

void CTDefect::DEF017758L()
	{
	RDebug::Print(_L("Getting video memory address\r\n"));
	RDebug::Print(_L("Start\r\n"));
	TInt address = 0;

	__UHEAP_MARK;
	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayMemoryAddress, address));
	__UHEAP_MARKEND;
	RDebug::Print(_L("End\n"));
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0047
 
  @SYMDEF             

  @SYMTestCaseDesc The main purpose of this method is to test RAM/ROM bitmap compression.
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 

	 - loads and displays ROM compressed bitmap.
	 - loads and displays ROM bitmap - no compression.
	 - loads and displays RAM compressed bitmap.
	 - loads and displays RAM bitmap - no compression.
	 - loads, compress and displays RAM bitmap.
	 - stretches, moves and resizes RAM compressed bitmap.
	 - compares byte by byte compressed and uncompressed bitmap images.
	 Note: if you want to have better visibility of the bitmaps shown on
	 the screen - change "KDelayTime" constant to be 1500000.
	 @param  TBitmapType aBitmapType - tested bitmaps type - 16 bpp, 24 bpp, ...
	 @return None
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::BitmapCompressionL(TBitmapType aBitmapType)
	{
	CreateScreenDeviceL();
	CreateFontL();

	TRect rc(10, 20, 610, 220);
	TBuf<100> text;
	TInt mbm_offset = 0;
	TInt err;
	TPtrC BitmapTypeName[ELastBmpType] = {_L("16 bpp"), _L("24 bpp")};
	typedef enum {ERAMFileType, ERAMCFileType, EROMFileType, EROMCFileType, ELastBmpFileType} TBitmapFileType;
	TPtrC BitmapFile[ELastBmpFileType][ELastBmpType] = 
		{
		{KRamBitmap(), KRam24Bitmap()},
		{KRamCBitmap(), KRamC24Bitmap()},
		{KRomBitmap(), KRom24Bitmap()},
		{KRomCBitmap(), KRomC24Bitmap()}
		};

	if(!iSkipRomBitmapTests)
		{
		//ROM compressed bitmap.
		text = BitmapTypeName[aBitmapType];
		text += _L(" ROM compressed bitmap.");
		DisplayBitmapL(BitmapFile[EROMCFileType][aBitmapType], 0, mbm_offset, text, rc);
	
		//ROM bitmap - no compression.
		text = BitmapTypeName[aBitmapType];
		text += _L(" ROM bitmap - no compression.");
		DisplayBitmapL(BitmapFile[EROMFileType][aBitmapType], 0, mbm_offset, text, rc);
		}

	//RAM compressed bitmap.
	text = BitmapTypeName[aBitmapType];
	text += _L(" RAM compressed bitmap.");
	DisplayBitmapL(BitmapFile[ERAMCFileType][aBitmapType], 0, mbm_offset, text, rc);

	//RAM bitmap - no compression.
	text = BitmapTypeName[aBitmapType];
	text += _L(" RAM bitmap - no compression.");
	DisplayBitmapL(BitmapFile[ERAMFileType][aBitmapType], 0, mbm_offset, text, rc);

	//RAM bitmap - no compression. Compress it!
	err = iBitmap->Compress();
	TEST(err == KErrNone);
	TEST(iBitmap->IsCompressedInRAM());
	text = BitmapTypeName[aBitmapType];
	text += _L(" RAM bitmap - compressed now.");
	DisplayBitmap(text, rc);
	err = iBitmap->Save(_L("c:\\temp.mbm"));
	TEST(err == KErrNone);

	//RAM compressed bitmap - stretch the bitmap.
	text = BitmapTypeName[aBitmapType];
	text += _L(" RAM compressed bitmap - stretch the bitmap.");
	TRect rc2(10, 20, 410, 150);
	DisplayBitmap(text, rc2);

	//RAM compressed bitmap - move the bitmap.
	text = BitmapTypeName[aBitmapType];
	text += _L(" RAM compressed bitmap - move the bitmap.");
	for(TInt j=0;j<10;j++)
		{
		TRect rc2(10 + j * 10, 20 + j * 5, 410 + j * 10, 150 + j * 5);
		DisplayBitmap(text, rc2);
		}

	//RAM compressed bitmap - resize the bitmap.
	iGc->Clear();
	err = iBitmap->Save(_L("c:\\temp2.mbm"));
	TEST(err == KErrNone);
	CFbsBitmap* resizedBitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(resizedBitmap);
	err = resizedBitmap->Load(_L("c:\\temp2.mbm"));
	TEST(err == KErrNone);
	err = resizedBitmap->Resize(TSize(300, 100));
	TEST(err == KErrNone);
	iGc->DrawBitmap(TRect(10, 20, 310, 120), resizedBitmap);
	text = BitmapTypeName[aBitmapType];
	text += _L(" RAM compressed bitmap - resize the bitmap.");
	iGc->DrawText(text, TPoint(10, 15));
	iDevice->Update();
	User::After(KDelayTime);
	CleanupStack::PopAndDestroy(resizedBitmap);

	iGc->Clear();
	iGc->DrawText(_L("Compare compressed and uncompressed bitmap data."), TPoint(10, 15));
	iDevice->Update();

	DestroyFont();

	CFbsBitmap* compressedBitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(compressedBitmap);
	err = compressedBitmap->Load(_L("c:\\temp.mbm"));
	TEST(err == KErrNone);

	CFbsBitmap* uncompressedBitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(uncompressedBitmap);
	err = uncompressedBitmap->Load(BitmapFile[ERAMFileType][aBitmapType]);
	TEST(err == KErrNone);

	//Compare compressed and uncompressed bitmaps.
	TEST(compressedBitmap->SizeInPixels() == uncompressedBitmap->SizeInPixels());
	TSize sizeInPixels = compressedBitmap->SizeInPixels();
	TInt BitmapBytesPerPixel[ELastBmpType] = {2, 3};
	HBufC8* compressedLine = HBufC8::NewLC(sizeInPixels.iWidth * BitmapBytesPerPixel[aBitmapType]);
	TPtr8 p1 = compressedLine->Des();
	HBufC8* uncompressedLine = HBufC8::NewLC(sizeInPixels.iWidth * BitmapBytesPerPixel[aBitmapType]);
	TPtr8 p2 = uncompressedLine->Des();
	TDisplayMode BitmapScreenMode[ELastBmpType] = {EColor64K, EColor16M};
	for(TInt ii=0;ii<sizeInPixels.iHeight;ii++)
		{
		compressedBitmap->GetScanLine(p1, TPoint(0,ii), sizeInPixels.iWidth, BitmapScreenMode[aBitmapType]);
		uncompressedBitmap->GetScanLine(p2, TPoint(0,ii), sizeInPixels.iWidth, BitmapScreenMode[aBitmapType]);
		TEST(p1 == p2);
		}

	CleanupStack::PopAndDestroy(uncompressedLine);
	CleanupStack::PopAndDestroy(compressedLine);
	CleanupStack::PopAndDestroy(uncompressedBitmap);
	CleanupStack::PopAndDestroy(compressedBitmap);

	User::After(KDelayTime);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0048
 
  @SYMDEF             

  @SYMTestCaseDesc The main purpose of this method is to test new resource files format.
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions  The new resoure files format is: RSC file header + RSC file section + MBM file section.
 The method consequently does:
 - loads and displays new RSC file format: RSC file in ROM. MBM file in ROM.
 - loads and displays new RSC file format: RSC file in RAM. MBM file in RAM.
 - loads and displays new RSC file format: RSC file in ROM. MBM file in ROM. 2 bitmaps.
 - loads and displays new RSC file format: RSC file in RAM. MBM file in RAM. 2 bitmaps.
 - loads and displays new RSC file format: RSC file in ROM. MBM file in RAM.
 Note: if you want to have better visibility of the bitmaps shown on
 the screen - change "KDelayTime" constant to be 1500000.
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::NewRscFormatL()
	{
	CreateScreenDeviceL();
	CreateFontL();

	TRect rc(10, 20, 610, 220);
	TPtrC text(_L("RSC file in ROM. MBM file in ROM."));
	TInt mbm_offset = ::FileSizeL(KRscFileHeader) + ::FileSizeL(KRscFileData);

	//RSC file in ROM. MBM file in ROM.
	if(!iSkipRomBitmapTests)
		DisplayBitmapL(KRomRsc_RomMbm, 0, mbm_offset, text, rc);
	
	//RSC file in RAM. MBM file in RAM.
	text.Set(_L("RSC file in RAM. MBM file in RAM."));
	DisplayBitmapL(KRamRsc_RamMbm, 0, mbm_offset, text, rc);
	
	TInt ii;
	TRect rect[] = {TRect(10, 20, 170, 100), TRect(10, 20, 35, 94)};

	//RSC file in ROM. MBM file in ROM. Multiple bitmaps.
	if(!iSkipRomBitmapTests)
		{
		text.Set(_L("RSC file in ROM. MBM file in ROM. Multiple bitmaps."));
		for(ii=0;ii<TInt(sizeof(rect)/sizeof(rect[0]));ii++)
			{
			DisplayBitmapL(KRomRsc_RomMbm2, ii, mbm_offset, text, rect[ii]);
			}
		}

	//RSC file in RAM. MBM file in RAM. Multiple bitmaps.
	text.Set(_L("RSC file in RAM. MBM file in RAM. Multiple bitmaps."));
	for(ii=0;ii<TInt(sizeof(rect)/sizeof(rect[0]));ii++)
		{
		DisplayBitmapL(KRamRsc_RamMbm2, ii, mbm_offset, text, rect[ii]);
		}

	//RSC file in ROM. MBM file in RAM.
	text.Set(_L("RSC file in ROM. MBM file in RAM."));
	DisplayBitmapL(KRomRsc_RamMbm, 0, mbm_offset, text, rc);
	
	//RSC file in ROM. MBM file in RAM. Multiple bitmaps.
	text.Set(_L("RSC file in ROM. MBM file in RAM. Multiple bitmaps."));
	for(ii=0;ii<TInt(sizeof(rect)/sizeof(rect[0]));ii++)
		{
		DisplayBitmapL(KRomRsc_RamMbm2, ii, mbm_offset, text, rect[ii]);
		}

	DestroyFont();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0049
 
  @SYMDEF             

  @SYMTestCaseDesc tests that 16bpp/24bpp bitmap compression works correctly
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates a compress bitmap and mask and draws to it
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::INC031920L(TBitmapType aBitmapType, TBool aRomBitmap)
	{
	if(iSkipRomBitmapTests && aRomBitmap)
		return;
	
	CreateScreenDeviceL();
	CreateFontL();

	TPtrC filename;
	switch(aBitmapType)
		{
	case E16BppBmpType:
		filename.Set(aRomBitmap ? KComprRomBitmap16() : KComprRamBitmap16());
		break;
	case E24BppBmpType:
		filename.Set(aRomBitmap ? KComprRomBitmap24() : KComprRamBitmap24());
		break;
	default:
		TEST(EFalse); //should never get here
		}

	CFbsBitmap* brush_bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(brush_bitmap);
	TInt err = brush_bitmap->Load(filename, 10);
	TEST(err == KErrNone);
	iGc->UseBrushPattern(brush_bitmap);

	TInt brush_style = CGraphicsContext::ENullBrush;
	for(TInt ii=0;ii<25;ii++)
		{
		if(++brush_style == CGraphicsContext::EDiamondCrossHatchBrush)
			{
			brush_style = CGraphicsContext::ESolidBrush;
			}
		iGc->SetBrushStyle(static_cast <CGraphicsContext::TBrushStyle> (brush_style));

		DeleteBitmap();
		iBitmap = new (ELeave) CFbsBitmap;
		err = iBitmap->Load(filename, ii);
		TEST(err == KErrNone);
		TSize size = iBitmap->SizeInPixels();

		CFbsBitmap* mask_bitmap = new (ELeave) CFbsBitmap;
		CleanupStack::PushL(mask_bitmap);
		err = mask_bitmap->Load(filename, (ii+3)<25 ? (ii+3) : (ii-3));
		TEST(err == KErrNone);

		TPoint pt(0, 0);
		TRect rc(0, 0, size.iWidth, size.iHeight);
		//If there is a problem with BitBltMasked - it can fail with "Access violation"
		//exception on the emulator.
		iGc->BitBltMasked(pt, iBitmap, rc, mask_bitmap, ETrue);
		iGc->DrawRect(rc);
		iDevice->Update();
		User::After(KDelayTime);

		CleanupStack::PopAndDestroy(mask_bitmap);
		}

	CleanupStack::PopAndDestroy(brush_bitmap);
	DestroyFont();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0050
 
  @SYMDEF             

  @SYMTestCaseDesc 8 bit bitmap compression - ROM
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates and draws to an 8bbp compressed bitmap
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::Test8bitBmpComressionL()
	{
	CreateScreenDeviceL();
	CreateFontL();

	TRect rc(10, 20, 610, 220);
	TPtrC text(_L("8 bpp RAM compressed bitmap."));
	DisplayBitmapL(KComprRamBitmap8, 0, 0, text, rc);

	if(!iSkipRomBitmapTests)
		{
		text.Set(_L("8 bpp ROM compressed bitmap."));
		DisplayBitmapL(KComprRomBitmap8, 0, 0, text, rc);
		}

	DestroyFont();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0051
 
  @SYMDEF             

  @SYMTestCaseDesc 12 bpp ROM compressed bitmap
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates and draws to an 12bbp compressed bitmap
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::Test12bitBmpComressionL()
	{
	CreateScreenDeviceL();
	CreateFontL();

	TRect rc(10, 20, 610, 220);
	TPtrC text(_L("12 bpp RAM compressed bitmap."));
	DisplayBitmapL(KComprRamBitmap12, 0, 0, text, rc);

	if(!iSkipRomBitmapTests)
		{
		text.Set(_L("12 bpp ROM compressed bitmap."));
		DisplayBitmapL(KComprRomBitmap12, 0, 0, text, rc);
		}

	DestroyFont();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0052
 
  @SYMDEF DEF034134           

  @SYMTestCaseDesc BMCONV has a limitation on the number of bitmaps it can use.
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Loads up a mbm contaiing 8 bitmaps and draws them
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
 void CTDefect::DEF034134L()
	{
	CreateScreenDeviceL();
	CreateFontL();

	TBuf<64> text;
	for(TInt ii=0;ii<8;ii++)
	//8 - it is the number of bitmaps in mbm file - see GenBitmaps.mk where this mbm file is generated
		{
		text.Format(_L("Bitmap %d"), ii+1);
		DeleteBitmap();
		iBitmap = new (ELeave) CFbsBitmap;
		TInt err = iBitmap->Load(KTestBmp, ii);
		TEST(err == KErrNone);
		TSize size = iBitmap->SizeInPixels();
		DisplayBitmap(text, TRect(20, 20, 20 + size.iWidth, 20 + size.iHeight));
		}

	DestroyFont();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0053
 
  @SYMDEF  DEF036156         

  @SYMTestCaseDesc When using a mask that's smaller than the bitmap being blitted, 
  the mask should be tiled horizontally and vertically until it's the correct size and then applied to the bitmap.
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates bitmap, mask and brush and tests that tiling occurs correctly
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::TestTilingHorizontalAndVerticalL()
	{
	TDisplayMode mode[] = {EColor256, EColor4K, EColor16M, EColor16MU, EColor64K, 
						   EGray256, EGray16, EGray4, EGray2, EColor16, EColor16MA, EColor16MAP};
	TInt err;

	//Create&load source bitmap
	CFbsBitmap* srcBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(srcBmp);
	err = srcBmp->Load(KBitBltSrcBmp, 0);
	TEST(err == KErrNone);
	TSize size1 = srcBmp->SizeInPixels();
	TPoint pt1(10, 20);
	TRect rc1(pt1.iX, pt1.iY, pt1.iX + size1.iWidth, pt1.iY + size1.iHeight);

	//Create&load mask bitmap
	CFbsBitmap* maskBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(maskBmp);
	err = maskBmp->Load(KBitBltMaskedBmp, 0);
	TEST(err == KErrNone);
	TSize size2 = maskBmp->SizeInPixels();
	TInt offset = 40;
	TPoint pt2(pt1.iX, pt1.iY + size1.iHeight + offset);
	TRect rc2(pt2.iX, pt2.iY, pt2.iX + size2.iWidth, pt2.iY + size2.iHeight);

	//Create&load brush bitmap
	CFbsBitmap* brush_bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(brush_bitmap);
	err = brush_bitmap->Load(KComprRamBitmap16, 10);
	TEST(err == KErrNone);

	for(TInt ii=0;ii<TInt(sizeof(mode)/sizeof(mode[0]));++ii)
		{
		err = CreateScreenDeviceL(mode[ii]);
		if(err == KErrNotSupported)
			{
			continue;
			}
		TEST(err == KErrNone);

		CreateFontL();

		iGc->Clear();

		iGc->DrawText(_L("Src bitmap"), TPoint(pt1.iX, pt1.iY - 2));
		iGc->DrawBitmap(rc1, srcBmp);

		iGc->DrawText(_L("Mask bitmap"), TPoint(pt2.iX, pt2.iY - 2));
		iGc->DrawBitmap(rc2, maskBmp);

		TBuf<64> text2;
		_LIT(KMaskMode,"BitBltMasked, Mode: E%S");
		text2.Format(KMaskMode, &ColorModeName(ii));
		iGc->DrawText(text2, TPoint(150, pt2.iY-2));

		TPoint pt(pt1.iX + size1.iWidth + offset, pt1.iY);
		TRect rc(0, 0, size1.iWidth, size1.iHeight);

		CGraphicsContext::TBrushStyle brushStyle[] = 
			{
			CGraphicsContext::ENullBrush,
			CGraphicsContext::ESolidBrush,
			CGraphicsContext::EPatternedBrush
			};

		TBuf<32> text[sizeof(brushStyle)/sizeof(brushStyle[0])] = 
			{
			_L("NonFlicker"),
			_L("NonFlickerSolid"),
			_L("NonFlickerPatterned")
			};

		iGc->UseBrushPattern(brush_bitmap);

		for(TInt j=0;j<TInt(sizeof(brushStyle)/sizeof(brushStyle[0]));pt.iX+=150,j++)
			{
			iGc->DrawText(text[j], TPoint(pt.iX, pt.iY - 2));
			iGc->SetBrushStyle(brushStyle[j]);
			iGc->BitBltMasked(pt, srcBmp, rc, maskBmp, ETrue);
			}

		iDevice->Update();
		User::After(500000);

		DestroyFont();
		}//end of - for(TInt ii=0;ii<TInt(sizeof(mode)/sizeof(mode[0]));++ii)

	CleanupStack::PopAndDestroy(brush_bitmap);
	CleanupStack::PopAndDestroy(maskBmp);
	CleanupStack::PopAndDestroy(srcBmp);
	DestroyFont();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0054
 
  @SYMDEF INC036348 + DEF081298            

  @SYMTestCaseDesc Pie drawing is incorrect in some borderline cases 
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions draws pie charts with boundry values and tests it is drawn correctly
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::DrawPieBorderlineCasesL()
	{
	CreateScreenDeviceL();
	CreateFontL();

	DrawPie(_L("INC036348 - Missing points"),			TRect(0, 0, 100, 100), TPoint(346, -462),	TPoint( 50, -462));
	DrawPie(_L("INC036348 - Extra points"),				TRect(0, 0, 100, 100), TPoint(562, -462),	TPoint(187, -462));
	DrawPie(_L("INC036348 - Draws different range 1"),	TRect(0, 0, 100, 100), TPoint(187, -462),	TPoint( 50, -462));
	DrawPie(_L("INC036348 - Draws different range 2"),	TRect(0, 0, 100, 100), TPoint( 50, 562),	TPoint(186, 562));

	const TInt rectLength = 300;
	const TInt rectWidth = 100;
	for (TInt y = 0; y <= rectWidth; y++)
		{
		DrawPie(_L("DEF081298"), TRect(0, 0, rectLength, rectWidth), TPoint(0, 0), TPoint(0, y));
		}
	for (TInt x = 0; x <= rectLength; x++)
		{
		DrawPie(_L("DEF081298"), TRect(0, 0, rectLength, rectWidth), TPoint(0, 0), TPoint(x, rectWidth));
		}
	for (TInt y = rectWidth; y >= 0; y--)
		{
		DrawPie(_L("DEF081298"), TRect(0, 0, rectLength, rectWidth), TPoint(0, 0), TPoint(rectLength, y));
		}
	for (TInt x = rectLength; x >= 0; x--)
		{
		DrawPie(_L("DEF081298"), TRect(0, 0, rectLength, rectWidth), TPoint(0, 0), TPoint(x, 0));
		}

	DestroyFont();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0055
 
  @SYMDEF Panic in bitgdi in Typhoon 30.            

  @SYMTestCaseDesc INC037380
   
  @SYMTestPriority normal

  @SYMTestStatus Implemented

  @SYMTestActions Test BitBltMasked works correctly with 64K bitmaps and EGray256 masks
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::INC037380L()
	{
	TInt ii;
	TInt err;
	//Create the screen device
	err = CreateScreenDeviceL(EColor64K);
	if (err == KErrNotSupported)
		err = CreateScreenDeviceL(EColor16MA);
	if (err == KErrNotSupported)
		err = CreateScreenDeviceL(EColor16MAP);
	if (err == KErrNotSupported)
		return;
	User::LeaveIfError(err);
	//Create source bitmap
	CFbsBitmap* srcBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(srcBmp);
	User::LeaveIfError(srcBmp->Create(TSize(500, 1), EColor16M));
	TBitmapUtil bmpUtil1(srcBmp);
	bmpUtil1.Begin(TPoint(0, 0));
	for(ii=0;ii<500;ii++)
		{
		bmpUtil1.SetPos(TPoint(ii,0));
		bmpUtil1.SetPixel(ii);
		}
	bmpUtil1.End();
	//Create mask bitmap
	CFbsBitmap* maskBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(maskBmp);
	User::LeaveIfError(maskBmp->Create(TSize(20, 1), EGray256));
	TBitmapUtil bmpUtil2(maskBmp);
	bmpUtil2.Begin(TPoint(0, 0));
	for(ii=0;ii<20;ii++)
		{
		bmpUtil1.SetPos(TPoint(ii,0));
		bmpUtil2.SetPixel(100 + ii);
		}
	bmpUtil2.End();
	//Create brush bitmap
	CFbsBitmap* brush_bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(brush_bitmap);
	User::LeaveIfError(brush_bitmap->Create(TSize(20, 1), EColor64K));
	TBitmapUtil bmpUtil3(brush_bitmap);
	bmpUtil3.Begin(TPoint(0, 0));
	for(ii=0;ii<20;ii++)
		{
		bmpUtil1.SetPos(TPoint(ii,0));
		bmpUtil3.SetPixel(200 + ii);
		}
	bmpUtil3.End();

	iGc->Clear();
	iGc->DrawBitmap(TRect(0, 20, 500, 21), srcBmp);
	iGc->DrawBitmap(TRect(0, 22, 20, 23), maskBmp);
	iGc->UseBrushPattern(brush_bitmap);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	//The following call panics if the fix doesn't exist.
	iGc->BitBltMasked(TPoint(0, 0), srcBmp, TRect(0, 0, 500, 1), maskBmp, ETrue);

	iDevice->Update();

	CleanupStack::PopAndDestroy(brush_bitmap);
	CleanupStack::PopAndDestroy(maskBmp);
	CleanupStack::PopAndDestroy(srcBmp);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0056
 
  @SYMDEF INC037474        

  @SYMTestCaseDesc CBitwiseBitmap::GenerateLineFromCompressedSixteenBitData() is very slow
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions measures the time the GenerateLineFromCompressedSixteenBitData method takes with different target modes
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::INC037474L()
	{
	TPtrC bmpFile[] =
		{
		KBmpCompr12(),
		KBmpCompr16(),
		KBmpCCompr12(),
		KBmpCCompr16()
		};
	TPtrC tt[] = {_L("rom"), _L("rom"), _L("ram"), _L("ram")};
	TBool inRam[] = {EFalse, EFalse, ETrue, ETrue};
	TInt bpp[] = {12, 16, 12, 16};
	TDisplayMode targetMode[] = {EColor4K, EColor64K, EColor4K, EColor64K};
	const TInt KMaxScanLineLen = 800;
	TBuf8<KMaxScanLineLen> scanLineBuf;
	for(TInt ii=0;ii<TInt(sizeof(bmpFile)/sizeof(bmpFile[0]));ii++)
		{
		if(iSkipRomBitmapTests && !inRam[ii])
			continue;
		
		CFbsBitGcBitmap* bmp = ::LoadBitmapLC(bmpFile[ii]);
		if(inRam[ii])
			{
			User::LeaveIfError(bmp->Compress());
			}
		//
		TDesC& des = tt[ii];
		INFO_PRINTF3(_L("%d bpp %S compressed bitmap, GetScanLine test"), bpp[ii], &des);
		TSize size = bmp->SizeInPixels();
		TEST(TInt(size.iWidth * sizeof(TInt16)) <= KMaxScanLineLen);
		bmp->LockHeap();
		CBitwiseBitmap* bmp2 = bmp->Address();
		TUint32* dataAddr = bmp->DataAddress();
		TLineScanningPosition scanLinePos(dataAddr);
		TPoint pt00(0, 0);
		TUint timeStart = User::TickCount();
		for(TInt j=0;j<100;j++)
			{
			TPoint startPixel(0, 0);
			for(TInt k=0;k<size.iHeight;k++)
				{
				startPixel.iY = k;
				bmp2->GetScanLine(scanLineBuf, startPixel, size.iWidth,
								  EFalse, pt00, targetMode[ii],
								  dataAddr, scanLinePos);
				}
			}
		TUint timeEnd = User::TickCount();
		INFO_PRINTF4(_L("%d bpp %S compressed bitmap, time = %d"), bpp[ii], &des, timeEnd - timeStart);
		bmp->UnlockHeap();
		//
		CleanupStack::PopAndDestroy(bmp);
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0057
 
  @SYMDEF INC037370            

  @SYMTestCaseDesc Corrupted compressed images 
   
  @SYMTestPriority normal

  @SYMTestStatus Implemented

  @SYMTestActions Creates some compressed bitmaps, resizes them and compares results
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
//INC037370 - Corrupted compressed images 
void CTDefect::INC037370L()
	{
	CreateScreenDeviceL();
	TPtrC bmpFile[] = {KBmp16_1(), KBmp16_2(), KBmp16_3()};
	CFbsBitGcBitmap* bmp[] = {NULL, NULL, NULL};
	const TInt bmpArrSize = sizeof(bmp) / sizeof(bmp[0]);
	//Load the bitmaps
	::LoadBitmapsLC<bmpArrSize>(&bmp, bmpFile);
	TInt ii;
	//Compress the bitmaps
	for(ii=0;ii<bmpArrSize;ii++)
		{
		bmp[ii]->Compress();
		}
	//Display the bitmaps
	::DisplayBitmaps<bmpArrSize>(&bmp, iDevice, iGc);
	//Resize bitmap[0]
	const TInt KResize = 100;
	TSize size = bmp[0]->SizeInPixels();
	size.iWidth -= KResize;
	size.iHeight -= KResize;
	bmp[0]->Resize(size);
	//Display the bitmaps again. If the defect is not fixed - bmp[0] image is corrupted!
	::DisplayBitmaps<bmpArrSize>(&bmp, iDevice, iGc);
	//Compare bmp[0] and bmp[1]. If the defect is not fixed - the test will fail!
	::CompareBitmaps<bmpArrSize, 1000>(&bmp, EColor64K, 0, 1, this);
	//Resize bitmap[1]
	size = bmp[1]->SizeInPixels();
	size.iWidth += KResize;
	size.iHeight += KResize;
	bmp[1]->Resize(size);
	//Display the bitmaps again. If the defect is not fixed - bmp[1] image is corrupted!
	::DisplayBitmaps<bmpArrSize>(&bmp, iDevice, iGc);
	//Compare bmp[1] and bmp[2]. If the defect is not fixed - the test will fail!
	::CompareBitmaps<bmpArrSize, 1000>(&bmp, EColor64K, 1, 2, this);
	//Destroy the bitmaps
	::DestroyBitmaps<bmpArrSize>(&bmp);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0058
 
  @SYMDEF             

  @SYMTestCaseDesc Tests setting of various display modes
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Sets display modes then displays bitmaps 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
//Test CFbsBitmap::SetDisplayMode()
void CTDefect::TestSettingDisplayModeL()
	{
	TDisplayMode mode[] = {ENone, EGray2, EGray4, EGray16, EGray256, EColor16, EColor256,
						   EColor64K, EColor16M, ERgb, EColor4K, EColor16MU, EColor16MA, EColor16MAP};
	_LIT(KLog,"Running test with display mode %S (index=%d)");
	for(TInt ii=0;ii<TInt(sizeof(mode)/sizeof(mode[0]));ii++)
		{
		if(mode[ii] == ENone || mode[ii] == ERgb || CreateScreenDeviceL(mode[ii]) != KErrNone)
			{
			continue;
			}
		INFO_PRINTF3(KLog,&ColorModeName(mode[ii]),ii);
		DestroyFont();
		CreateFontL();

		TRect rc(10, 20, 610, 220);

		DeleteBitmap();
		iBitmap = new (ELeave) CFbsBitmap;
		TInt err = iBitmap->Load(KRamBitmap, 0);
		TEST(err == KErrNone);
		TDisplayMode initialDisplayMode = iBitmap->InitialDisplayMode();
		TInt bmpMode;
		//From the initial display mode to EGray2 mode.
		for(bmpMode=initialDisplayMode;bmpMode!=ENone;bmpMode--)
			{
			DisplayBitmap(rc, mode[ii], TDisplayMode(bmpMode));
			}
		//From EGray2 mode to the initial display mode
		for(bmpMode=EGray2;bmpMode<=initialDisplayMode;bmpMode++)
			{
			DisplayBitmap(rc, mode[ii], TDisplayMode(bmpMode));
			}
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0059
 
  @SYMDEF             

  @SYMTestCaseDesc Tests setting of various display modes
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions tests display modes are set correctly by checking scanline data wlan
  
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::TestSettingDisplayMode2L()
	{
	// Try color mode 64K, 256 and 16MA
	TInt err=CreateScreenDeviceL(EColor64K);
	if (err!=KErrNone)
		err=CreateScreenDeviceL(EColor256);
	if (err!=KErrNone)
		err=CreateScreenDeviceL(EColor16MA);
	if (err!=KErrNone)
		err=CreateScreenDeviceL(EColor16MAP);
	User::LeaveIfError(err);
	DeleteBitmap();
	const TInt KBmpWidth = 100;
	//The bitmap data - for EColor16MU - the pixel with the largest width in bits
	const TInt bytePerPixel = 4;
	TUint8 srcBmpData[KBmpWidth * bytePerPixel];
	Mem::Fill(srcBmpData, sizeof(srcBmpData) / 2, 0xFF); //First half is white colored
	Mem::Fill(srcBmpData + sizeof(srcBmpData) / 2, sizeof(srcBmpData) / 2, 0x00);//Second half is black colored
	//The bitmap
	iBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(TSize(KBmpWidth, 1), EColor16MU));
	const TSize KSize = iBitmap->SizeInPixels();
	TPtr8 p(srcBmpData, KBmpWidth * bytePerPixel, KBmpWidth * bytePerPixel);
	iBitmap->SetScanLine(p, 0);
	TDisplayMode mode[] = {EColor16MAP, EColor16MA, EColor16MU, EColor16M, EColor64K,
						   EColor4K, EColor256, EGray256, EColor16,
						   EGray16, EGray4, EGray2};
	TInt ii;
	for(ii=0;ii<TInt(sizeof(mode)/sizeof(mode[0]));ii++)
		{
		SetAndCheckDisplayMode(mode[ii], KSize);
		}
	for(ii=((sizeof(mode)/sizeof(mode[0]))-1);ii!=-1;ii--)
		{
		SetAndCheckDisplayMode(mode[ii], KSize);
		}
	}

void CTDefect::SetAndCheckDisplayMode(TDisplayMode aMode, const TSize& aInitialSize)
	{
	TInt err = iBitmap->SetDisplayMode(aMode);
	TEST(err == KErrNone);
	TSize size = iBitmap->SizeInPixels();
	TEST(aInitialSize == size);
	TEST(iBitmap->DisplayMode() == aMode);
	INFO_PRINTF2(_L("Check scanline in display mode: %d"), aMode);
	CheckScanLine();
	}

void CTDefect::CheckScanLine()
	{
	TSize size = iBitmap->SizeInPixels();
	TInt j;
	TRgb whiteColor(0xFF, 0xFF, 0xFF);
	TPoint pt(0, 0);
	for(j=0;j<(size.iWidth/2);j++)
		{
		TRgb color;
		pt.iX = j;
		iBitmap->GetPixel(color, pt);
		TEST(color == whiteColor);
		}
	TRgb blackColor(0x00, 0x00, 0x00);
	for(j=(size.iWidth/2);j<size.iWidth;j++)
		{
		TRgb color;
		pt.iX = j;
		iBitmap->GetPixel(color, pt);
		if(color != blackColor)
			{
			ERR_PRINTF1(_L("Scan line error"));
			}
		TEST(color == blackColor);
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0060
 
  @SYMDEF             

  @SYMTestCaseDesc tests swapping a bitmaps width and height
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates a bitmap, swaps its width and height and tests for correct size
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::TestSwappingWidthAndHeightL()
	{
	// Try color mode 64K, 256 and 16MA
	TInt err=CreateScreenDeviceL(EColor64K);
	if (err!=KErrNone)
		err=CreateScreenDeviceL(EColor256);
	if (err!=KErrNone)
		err = CreateScreenDeviceL(EColor16MA);
	if (err!=KErrNone)
		err = CreateScreenDeviceL(EColor16MAP);
	User::LeaveIfError(err);
	DestroyFont();
	CreateFontL();

	DeleteBitmap();
	iBitmap = new (ELeave) CFbsBitmap;
	err = iBitmap->Load(KRamBitmap, 0);
	TEST(err == KErrNone);

	TSize size1_1 = iBitmap->SizeInPixels();
	TSize size1_2 = iBitmap->SizeInTwips();
	TRect rc(10, 20, 610, 220);
	TPtrC text(_L("Before swapping"));
	DisplayBitmap(text, rc);

	iBitmap->SwapWidthAndHeight();
	TSize size2_1 = iBitmap->SizeInPixels();
	TSize size2_2 = iBitmap->SizeInTwips();
	TEST(size1_1.iWidth == size2_1.iHeight && size1_1.iHeight == size2_1.iWidth);
	TEST(size1_2.iWidth == size2_2.iHeight && size1_2.iHeight == size2_2.iWidth);

	text.Set(_L("After swapping"));
	DisplayBitmap(text, rc);
	rc = TRect(10, 20, 210, 620);
	DisplayBitmap(text, rc);

	DeleteBitmap();
	iBitmap = new (ELeave) CFbsBitmap;
	iBitmap->Create(TSize(17, 1), EGray2);

	DeleteBitmap();
	iBitmap = new (ELeave) CFbsBitmap;
	iBitmap->Create(TSize(1, 17), EGray2);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0061
 
  @SYMDEF DEF038774             

  @SYMTestCaseDesc Buffer overflow when using CFbsBitmapDevice::GetScanLine and EGray2 disp.mode 
  					EGray4, EGray16, EColor16 modes also tested.
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates bitmap of various width depending on the colour mode and checks that its buffer 
  					does not get an overflow when written to
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::DEF038774L()
	{
	enum {KHeight = 1};//Bitmaps height - in pixels
	TDisplayMode testMode[] =  {EGray2, EGray4, EGray16, EColor16, EColor4K, EColor64K, EColor16M, EColor16MU, EColor16MA, EColor16MAP};//tested display modes
	const TInt pixelPerByte[] ={8,      4,      2,       2,        0,        0,         0,         0,		0};
	const TInt bytePerPixel[] ={0,      0,      0,       0,        2,        2,         3,         4,		4};
	const TInt bitmapWidth[] = {22,     7,      11,      11,       3,        3,         1,         1,		1};//bitmaps width
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
			scanLineLen = bitmapWidth[ii] / pixelPerByte[ii];
			}
		else
			{
			scanLineLen = bitmapWidth[ii] * bytePerPixel[ii];
			}
		Mem::Fill(scanLineData, scanLineLen, KTestVal1);
		TPtr8 p(scanLineData, scanLineLen, scanLineLen);
		iBitmap->SetScanLine(p, 0);
		//Create bitmap device (KWidth, KHeight) size
		DeleteBitmapDevice();
		iBmpDevice = CFbsBitmapDevice::NewL(iBitmap);
		//Fill the scanLineData with control values
		Mem::Fill(scanLineData, sizeof(scanLineData), KTestVal2);
		//GetScanLine test - check the (scanLineLen + 1) byte
		iBmpDevice->GetScanLine(p, TPoint(), bitmapWidth[ii], testMode[ii]);
		TEST(scanLineData[scanLineLen + 1] == KTestVal2);
		}
	}

/**
  @SYMTestCaseID	GRAPHICS-BITGDI-0086

  @SYMDEF DEF121299             

  @SYMTestCaseDesc Ensure alpha channel is not overwritten in 16MU BitBlt/DrawBitmap Operations.
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates 1x1 or 2x2 pixel Bitmaps with non 0x00/0xFF alpha channel to test BitBlt/DrawBitmap operations.
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::TestBitBltOperationsOn16muUndefinedByteL()
	{
	//Create a source bitmap and set the Alpha channel to a Non 0xFF/0x00 value.
	CFbsBitmap* srcbmp = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(srcbmp);
	User::LeaveIfError(srcbmp->Create(TSize(1,1), EColor16MU));
	
	*(srcbmp->DataAddress()) = 0x55000000;
	
	CFbsBitmap* destbmp = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(destbmp);
	//Create this one as 2x2 so DrawBitmap tests will work.
	User::LeaveIfError(destbmp->Create(TSize(2,2), EColor16MU));

	//Create Bitmap Device & Gc to perform BitBlt/DrawBmp operations.
	CFbsBitmapDevice* bmpdev = CFbsBitmapDevice::NewL(destbmp);
	CleanupStack::PushL(bmpdev);
	CFbsBitGc* bmpgc;
	User::LeaveIfError(bmpdev->CreateContext(bmpgc));
	CleanupStack::PushL(bmpgc);
	bmpgc->Activate(bmpdev);
	
	//Create Extra Mask Bitmap
	CFbsBitmap* bmpmask16mu = new(ELeave)CFbsBitmap();
	CleanupStack::PushL(bmpmask16mu);
	User::LeaveIfError(bmpmask16mu->Create(TSize(2,2), EColor16MU));
	
	//BitBlt
	INFO_PRINTF1(_L("Performing BitBlt() Test"));
	bmpgc->BitBlt(TPoint(0,0), srcbmp);
	TEST(CompareFirstPixelAlphaChannel(srcbmp, destbmp));
	
	//BitBltMasked
	INFO_PRINTF1(_L("Performing BitBltMasked() Test"));
	bmpgc->BitBltMasked(TPoint(0,0), srcbmp, TRect(TPoint(0,0), TSize(1,1)), srcbmp, EFalse);
	TEST(CompareFirstPixelAlphaChannel(srcbmp, destbmp));
		
	CleanupStack::PopAndDestroy(5, srcbmp);
	}


void CTDefect::DeleteBitmap()
	{
	if(iBitmap)
		{
		iBitmap->Reset();
		}
	delete iBitmap;
	iBitmap = NULL;
	}

void CTDefect::DeleteBitmapDevice()
	{
	delete iBmpDevice;
	iBmpDevice = NULL;
	}

void CTDefect::DeleteScreenDevice()
	{
	delete iDevice;
	iDevice = NULL;
	}

void CTDefect::DeleteGraphicsContext()
	{
	delete iGc;
	iGc = NULL;
	}

void CTDefect::DisplayBitmapL(const TDesC& aFileName, TInt aBitmapNo, TInt aBitmapOffset, const TDesC& aText, const TRect& aRc)
	{
	DeleteBitmap();
	iBitmap = new (ELeave) CFbsBitmap;
	TInt err = iBitmap->Load(aFileName, aBitmapNo, EFalse, aBitmapOffset);
	if(err)
		INFO_PRINTF3(_L("Error (%d) while loading bitmap (%S)"), err, &aFileName);
	TEST(err == KErrNone);
	DisplayBitmap(aText, aRc);
	}

void CTDefect::DisplayBitmap(const TDesC& aText, const TRect& aRc)
	{
	iGc->Clear();
	iGc->DrawBitmap(aRc, iBitmap);
	iGc->DrawText(aText, TPoint(10, 15));
	iDevice->Update();
	User::After(KDelayTime);
	}

void CTDefect::DisplayBitmap(const TRect& aRc, TDisplayMode aScreenMode, TDisplayMode aBmpMode)
	{
	if (aBmpMode==ENone || aBmpMode==ERgb)
		{
		return;
		}
	_LIT(KLog,"ScreenMode: %S, SetDisplayMode: %S");
	TBuf<128> text;
	text.Format(KLog,&ColorModeName(aScreenMode),&ColorModeName(aBmpMode));
	INFO_PRINTF1(text);
	TInt err = iBitmap->SetDisplayMode(aBmpMode);
	TEST(err == KErrNone);
	DisplayBitmap(text, aRc);
	}

void CTDefect::CreateFontL()
	{
	CFbsFont* font = NULL;
	TFontSpec fs(_L("Swiss"), 12);
	User::LeaveIfError(iDevice->GetNearestFontToDesignHeightInPixels(font, fs));
	iGc->UseFont(font);
	}

void CTDefect::DestroyFont()
	{
	if(iGc)
		{
		iGc->DiscardFont();
		}
	}

void CTDefect::DrawPie(const TDesC& aText, const TRect& aRc, const TPoint& aPtStart, const TPoint& aPtEnd)
	{
	const TRgb green(0x00, 0xFF, 0x00);
	const TRgb red(0xFF, 0x00, 0x00);
	const TRgb blue(0x00, 0x00, 0xFF);
	const TRgb white(0xFF, 0xFF, 0xFF);

	iGc->SetBrushColor(white);
	iGc->Clear();
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(green);
	iGc->DrawRect(aRc);
	iGc->SetPenColor(blue);
	iGc->SetBrushColor(red);
	iGc->DrawPie(aRc, aPtStart, aPtEnd);
	iGc->SetBrushColor(blue);
	iGc->DrawLine(
		TPoint(aRc.iTl.iX, aRc.iTl.iY + aRc.Height() / 2),
		TPoint(aRc.iTl.iX + aRc.Width(), aRc.iTl.iY + aRc.Height() / 2));
	iGc->DrawLine(
		TPoint(aRc.iTl.iX + aRc.Width() / 2, aRc.iTl.iY),
		TPoint(aRc.iTl.iX + aRc.Width() / 2, aRc.iTl.iY + aRc.Height()));
	iGc->DrawText(
		aText,
		TPoint(aRc.iTl.iX, aRc.iTl.iY + aRc.Height() + 15));
	iDevice->Update();
	User::After(10000);
	}

void CTDefect::CreateBitmapL(const TSize& aSize, TDisplayMode aMode)
	{
	DeleteBitmap();
	iBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Create(aSize, aMode));
	}


static void DisconnectFbsSession(TAny*)
	{
	RFbsSession::Disconnect();
	}

void CTDefect::INC042156L()
	{
	User::LeaveIfError(RFbsSession::Connect());
	CleanupStack::PushL(TCleanupItem(DisconnectFbsSession, NULL));

	TInt mbm_offset = 0;
	TBuf<100> text;
	TPtrC BitmapTypeName[ELastBmpType] = {_L("16 bpp"), _L("24 bpp")};
	typedef enum {ERAMFileType, ERAMCFileType, EROMFileType, EROMCFileType, ELastBmpFileType} TBitmapFileType;
	TPtrC BitmapFile[ELastBmpFileType][ELastBmpType] = 
		{
		{KRamBitmap(), KRam24Bitmap()},
		{KRamCBitmap(), KRamC24Bitmap()},
		{KRomBitmap(), KRom24Bitmap()},
		{KRomCBitmap(), KRomC24Bitmap()}
		};

	text = BitmapTypeName[E24BppBmpType];
	text += _L(" ROM compressed bitmap.");

	const TInt KWidth = 20;
	const TInt KHeight = 20;
	const TInt KAddLength = 10;
	CFbsBitmap* fbsBmp = NULL;

	//Create and compress RAM bitmap
	//The compression allows us to use the scanline buffer
	fbsBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsBmp);
	User::LeaveIfError(fbsBmp->Create(TSize(KWidth, KHeight), EColor64K));
	User::LeaveIfError(fbsBmp->Compress());
	TSize size = fbsBmp->SizeInPixels();

	TUint32* slptr=NULL;
	TPoint coord(0,0);
	TLineScanningPosition lineScanPos(fbsBmp->DataAddress());
	CFbsBitGcBitmap* srce = (CFbsBitGcBitmap*) fbsBmp; // CFbsBitmap
	CBitwiseBitmap* bmp = srce->Address();

	//Test GetScanLinePtr()
	User::LeaveIfError(bmp->GetScanLinePtr(slptr, size.iWidth, coord, fbsBmp->DataAddress(), lineScanPos));

	//Resize the RAM bitmap
	//The scanline buffer will be reallocated
	User::LeaveIfError(fbsBmp->Resize(TSize(KWidth + KAddLength, KHeight + KAddLength)));
	size = fbsBmp->SizeInPixels();

	slptr = NULL;
	lineScanPos.iSrcDataPtr = (TUint8*)fbsBmp->DataAddress();
	lineScanPos.iCursorPos = 0;
	lineScanPos.iScanLineBuffer = 0;
	srce = (CFbsBitGcBitmap*) fbsBmp; // CFbsBitmap
	bmp = srce->Address();

	//Test GetScanLinePtr()
	User::LeaveIfError(bmp->GetScanLinePtr(slptr, size.iWidth, coord, fbsBmp->DataAddress(), lineScanPos));
	//Destroy RAM bitmap internal data
	fbsBmp->Reset();

	//Load RAM compressed bitmap
	//The RAM bitmap is bigger than the previous one, so
	//the scanline buffer will be reallocated
	User::LeaveIfError(fbsBmp->Load(BitmapFile[ERAMCFileType][E24BppBmpType], 0, EFalse, mbm_offset));
	size = fbsBmp->SizeInPixels();
	TEST(size.iWidth > (KWidth + KAddLength));

	slptr = NULL;
	lineScanPos.iSrcDataPtr = (TUint8*)fbsBmp->DataAddress();
	lineScanPos.iCursorPos = 0;
	lineScanPos.iScanLineBuffer = 0;
	srce = (CFbsBitGcBitmap*) fbsBmp; // CFbsBitmap
	bmp = srce->Address();

	//Test GetScanLinePtr()
	User::LeaveIfError(bmp->GetScanLinePtr(slptr, size.iWidth, coord, fbsBmp->DataAddress(), lineScanPos));

	CleanupStack::PopAndDestroy(fbsBmp);
	CleanupStack::PopAndDestroy();//cleanupItem
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0062
 
  @SYMDEF INC070043            

  @SYMTestCaseDesc DrawBitmap() assumes always that source is of type EColor16MA
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests that drawing 16MU to 16MU uses the correct alpha values when required and not assume a transparent alpha
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::INC070043L()
	{
	const TInt KWidth = 100;
	const TInt KHeight = 100;
	const TInt KWidth11 = KWidth + 20;
	const TInt KHeight11 = KHeight + 20;

	//source bitmap
	CFbsBitmap* fbsSourceBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsSourceBmp);
	User::LeaveIfError(fbsSourceBmp->Create(TSize(KWidth, KHeight), EColor16MU));
	fbsSourceBmp->SetSizeInTwips(TSize(KWidth, KHeight));

	CFbsBitGc* theBitmapContextSrc = NULL;
	CFbsBitmapDevice* theBitmapDeviceSrc = CFbsBitmapDevice::NewL(fbsSourceBmp);
	CleanupStack::PushL(theBitmapDeviceSrc);
	theBitmapDeviceSrc->CreateContext(theBitmapContextSrc);
	CleanupStack::PushL(theBitmapContextSrc);
	
		//make a drawing
	theBitmapContextSrc -> SetBrushStyle(CGraphicsContext::ESolidBrush);
	theBitmapContextSrc -> SetPenColor(TRgb(0, 0, 255));
	theBitmapContextSrc -> DrawRect(TRect(0, 0, KWidth, KHeight));

	//destination bitmap
	CFbsBitmap* fbsDestBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsDestBmp);
	User::LeaveIfError(fbsDestBmp->Create(TSize(KWidth, KHeight), EColor16MU));
	fbsDestBmp->SetSizeInTwips(TSize(KWidth, KHeight));

	CFbsBitGc* theBitmapContextDest = NULL;
	CFbsBitmapDevice* theBitmapDeviceDest = CFbsBitmapDevice::NewL(fbsDestBmp);
	CleanupStack::PushL(theBitmapDeviceDest);
	theBitmapDeviceDest->CreateContext(theBitmapContextDest);
	CleanupStack::PushL(theBitmapContextDest);

	//bigger destination bitmap
	CFbsBitmap* fbsDestBmp1 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsDestBmp1);
	User::LeaveIfError(fbsDestBmp1->Create(TSize(KWidth11, KHeight11), EColor16MU));
	fbsDestBmp1->SetSizeInTwips(TSize(KWidth11, KHeight11));

	CFbsBitGc* theBitmapContextDest1 = NULL;
	CFbsBitmapDevice* theBitmapDeviceDest1 = CFbsBitmapDevice::NewL(fbsDestBmp1);
	CleanupStack::PushL(theBitmapDeviceDest1);
	theBitmapDeviceDest1->CreateContext(theBitmapContextDest1);
	CleanupStack::PushL(theBitmapContextDest1);

	
	//make a drawing
	INFO_PRINTF1(_L("Drawing from 16MU to 16MU memory bitmap"));
	theBitmapContextDest-> SetBrushStyle(CGraphicsContext::ESolidBrush);
	theBitmapContextDest -> SetPenColor(TRgb(0, 255, 0));
	theBitmapContextDest -> DrawRect(TRect(0, 0, KWidth, KHeight));
	
	
	theBitmapContextDest->DrawBitmap(TPoint(0, 0), fbsSourceBmp);

	CFbsBitGcBitmap* bmp1 = (CFbsBitGcBitmap*)fbsDestBmp;
	CFbsBitGcBitmap* bmp2 = (CFbsBitGcBitmap*)fbsSourceBmp;
	CFbsBitGcBitmap* bmp[] = {bmp1, bmp2};
	const TInt bmpArrSize = sizeof(bmp) / sizeof(bmp[0]);
	::CompareBitmaps<bmpArrSize, 1000>(&bmp, EColor16MU, 0, 1, this);

//	load from the file
	CFbsBitmap* fbsSourceBmp1 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsSourceBmp1);
	TInt err = fbsSourceBmp1->Load(_L("z:\\system\\data\\DrawBitmap16MU.MBM"));
	fbsSourceBmp1->SetSizeInTwips(TSize(KWidth, KHeight));
	
	//with compression
	CFbsBitmap* fbsSourceBmp2 = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(fbsSourceBmp2);
	err = fbsSourceBmp2->Load(_L("z:\\system\\data\\DrawBitmapComp16MU.MBM"));
	fbsSourceBmp2->SetSizeInTwips(TSize(KWidth, KHeight));
	
	INFO_PRINTF1(_L("Drawing from 16MU to 16MU bitmap"));
	theBitmapContextDest->DrawBitmap(TPoint(0, 0), fbsSourceBmp1);
	bmp1 = (CFbsBitGcBitmap*)fbsDestBmp;
	bmp2 = (CFbsBitGcBitmap*)fbsSourceBmp1;
	CFbsBitGcBitmap* bmp11[] = {bmp1, bmp2};
	const TInt bmpArrSize1 = sizeof(bmp11) / sizeof(bmp11[0]);
	::CompareBitmaps<bmpArrSize1, 1000>(&bmp11, EColor16MU, 0, 1, this);
	
//	drawing to the bigger destination
	INFO_PRINTF1(_L("Drawing from 16MU to 16MU bitmap, where source and distination bitmaps have a different size"));
	theBitmapContextDest1-> SetBrushStyle(CGraphicsContext::ESolidBrush);
	theBitmapContextDest1 -> SetPenColor(TRgb(0, 255, 0));
	theBitmapContextDest1 -> DrawRect(TRect(0, 0, KWidth11, KHeight11));
	theBitmapContextDest1->DrawBitmap(TPoint(0, 0), fbsSourceBmp1);
	bmp1 = (CFbsBitGcBitmap*)fbsDestBmp1;
	bmp2 = (CFbsBitGcBitmap*)fbsSourceBmp1;
	CFbsBitGcBitmap* bmp22[] = {bmp1, bmp2};
	const TInt bmpArrSize2 = sizeof(bmp22) / sizeof(bmp22[0]);
	::CompareBitmaps<bmpArrSize2, 1000>(&bmp22, EColor16MU, 0, 1, this);
	
	// drawing to the compressed bitmap	
	INFO_PRINTF1(_L("Drawing from 16MU to 16MU bitmap, where source bitmap is compressed"));
	theBitmapContextDest -> DrawRect(TRect(0, 0, KWidth, KHeight));
	theBitmapContextDest->DrawBitmap(TPoint(0, 0), fbsSourceBmp2);
	bmp1 = (CFbsBitGcBitmap*)fbsDestBmp;
	bmp2 = (CFbsBitGcBitmap*)fbsSourceBmp2;
	CFbsBitGcBitmap* bmp33[] = {bmp1, bmp2};
	const TInt bmpArrSize3 = sizeof(bmp33) / sizeof(bmp33[0]);
	::CompareBitmaps<bmpArrSize3, 1000>(&bmp33, EColor16MU, 0, 1, this);
	
	
	CleanupStack::PopAndDestroy(fbsSourceBmp2);
	CleanupStack::PopAndDestroy(fbsSourceBmp1);

	CleanupStack::PopAndDestroy(theBitmapContextDest1);
	CleanupStack::PopAndDestroy(theBitmapDeviceDest1);
	CleanupStack::PopAndDestroy(fbsDestBmp1);

	CleanupStack::PopAndDestroy(theBitmapContextDest);
	CleanupStack::PopAndDestroy(theBitmapDeviceDest);
	CleanupStack::PopAndDestroy(fbsDestBmp);

	CleanupStack::PopAndDestroy(theBitmapContextSrc);
	CleanupStack::PopAndDestroy(theBitmapDeviceSrc);
	CleanupStack::PopAndDestroy(fbsSourceBmp);
	}
	
	
/**
  @SYMTestCaseID GRAPHICS-BITGDI-0063
 
  @SYMDEF             

  @SYMTestCaseDesc The behaviour of SwapWidthAndHeight() is different for both Create()
					and Load() functions. This might be a defect but needs no bother unless 
					a solution is requested by clients.

					Lets suppose we have a bitmap of size (40, 12) and a CFbsBitmap object 
					contructed to hold it. When we use Create() and EColor16 for color depth 
					as in the following:
							iBitmap = new (ELeave) CFbsBitmap;
							iBitmap->Create(iSize, EColor16);
					the size of the bitmap internally will be 240bytes for EColor16. THE SIZE WILL
					BE DIFFERENT FOR EACH COLOR DEPTH.

					When applying SwapWidthAndHeight() the function finds that the size available
					(240Bytes) is less than the one needed after the swap (360Byes). This applies 
					to EColor16, EGray16, EGray4, EGray2 colors modes always and to the rest when 
					the dimenssions of the bitmap are irrelative (width too much bigger than height).
					Before the fix is added SwapWidthAndHeight() used to leave without applying the 
					swapping. The fix however reallocates memory to fit the new swapped bitmap.

					Now if we use Load() without Create() as in the following:
							iBitmap = new (ELeave) CFbsBitmap;
							iBitmap->Load(KBmp16_2, 0);
					the color depth of the system that best matches the bitmap's will be used. In 
					our case here it is EColor64K.So the size of the bitmap will be 960Bytes.Changing 
					the color depth via SetDisplayMode(...) will limit the modification to color 
					depths that are less than the one used for the bitmap, ie EColor64K. ONLY THE 
					COLOR DEPTH WILL CHANGE BUT THE SIZE OF THE BITMAP WILL NOT BE RECALCULATED. 
					This means that the size of the new swapped bitmap will always be less than the 
					original one with 960Bytes. That why SwapWidthAndHeight() will always pass with 
					Load() function, unlike using Create() which recalulates the new size.
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates bitmaps in various colour modes and blits 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTDefect::BitmapsSwappingTest()
	{
	CreateScreenDeviceL();	
	// All color depths are to be tested. Before the fix EColor16, EGray16, EGray4, EGray2 
	// used to fail the swapping.
	TDisplayMode testMode[] =  {EColor4K, EColor64K, EColor16M, EColor16MU, EColor256, EGray256,
								/**/ EColor16, EGray16, EGray4, EGray2 /**/};

	for(TInt ii=0;ii<TInt(sizeof(testMode)/sizeof(testMode[0]));ii++)
		{
		DeleteBitmap();
		iBitmap = new (ELeave) CFbsBitmap;
		User::LeaveIfError(iBitmap->Create(iSize, testMode[ii]));

		iGc->Clear();
		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->SetBrushColor(TRgb(0x00, 0x00, 0x00));
		// Apply the swap for the created bitmap with the specified color depth
		User::LeaveIfError(iBitmap->SwapWidthAndHeight());
		iGc->BitBlt(TPoint(0, 0), iBitmap);
		iDevice->Update();
		}
	}
	
/**
  @SYMTestCaseID GRAPHICS-BITGDI-0064
 
  @SYMDEF INC093055            

  @SYMTestCaseDesc BitGdi causes buffer overflow in WServ
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions tests the rotation of the screen and bitbltmasked. 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully and no overflow should occur. 
*/	

void CTDefect::INC093055L()
	{
	TInt si;
	TInt sj;
	TInt err;
	//Create the screen device
	err = CreateScreenDeviceL(EColor64K);
	if(err == KErrNotSupported)
		{
		return;
		}
	TSize bmpSize(700,700);
	User::LeaveIfError(err);
	//Create source bitmap
	CFbsBitmap* srcBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(srcBmp);
	User::LeaveIfError(srcBmp->Create(bmpSize, EColor16M));
	TBitmapUtil bmpUtil1(srcBmp);
	bmpUtil1.Begin(TPoint(0, 0));
	for(si=0;si<bmpSize.iWidth; ++si)
		{
		for(sj=0; sj < bmpSize.iHeight; ++sj)
			{
			bmpUtil1.SetPos(TPoint(si, sj));
			bmpUtil1.SetPixel(0xffff00);
			}
		}
	bmpUtil1.End();
	//Create mask bitmap
	CFbsBitmap* maskBmp = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(maskBmp);
	User::LeaveIfError(maskBmp->Create(bmpSize, EGray256));
	TBitmapUtil bmpUtil2(maskBmp);
	bmpUtil2.Begin(TPoint(0, 0));
	for(si=0;si<bmpSize.iWidth; ++si)
		{
		for(sj=0; sj < bmpSize.iHeight; ++sj)
			{
			bmpUtil2.SetPos(TPoint(si, sj));
			bmpUtil2.SetPixel(0xff00ff);
			}
		}
	bmpUtil2.End();
	CFbsDrawDevice *pDev;

	pDev = CFbsDrawDevice::NewScreenDeviceL(0, EColor64K);
	CleanupStack::PushL(pDev);
	pDev->InitScreen();
	TRect rect(TPoint(20,20), srcBmp->SizeInPixels());
	iGc->Clear();
	iGc->SetClippingRect(rect);
	iGc->BitBltMasked(rect.iTl, srcBmp, bmpSize, maskBmp, ETrue);
	iGc->CancelClippingRect();
	iDevice->Update();	
	User::After(2000000);
	pDev->SetOrientation(CFbsDrawDevice::EOrientationRotated90);
	iGc->Clear();
	iGc->SetClippingRect(rect);
	iGc->BitBltMasked(rect.iTl, srcBmp, bmpSize, srcBmp, ETrue);
	iGc->CancelClippingRect();
	iDevice->Update();
	User::After(2000000);
	pDev->SetOrientation(CFbsDrawDevice::EOrientationRotated180);
	iGc->Clear();
	iGc->SetClippingRect(rect);
	iGc->BitBltMasked(rect.iTl, srcBmp, bmpSize, maskBmp, ETrue);
	iGc->CancelClippingRect();
	iDevice->Update();
	User::After(2000000);
	pDev->SetOrientation(CFbsDrawDevice::EOrientationRotated270);	
	iGc->Clear();
	iGc->SetClippingRect(rect);
	iGc->BitBltMasked(rect.iTl, srcBmp, bmpSize, maskBmp, ETrue);
	iGc->CancelClippingRect();
	iDevice->Update();
	User::After(2000000);
	pDev->SetOrientation(CFbsDrawDevice::EOrientationNormal);
	CleanupStack::PopAndDestroy(pDev);
	CleanupStack::PopAndDestroy(maskBmp);
	CleanupStack::PopAndDestroy(srcBmp);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0016
 
  @SYMDEF PDEF103809            

  @SYMTestCaseDesc Test externalize/internalize of CFbsBitGc
   
  @SYMTestPriority Medium

  @SYMTestStatus Implemented

  @SYMTestActions Create CFbsBitGc but do not associate a font with it - externalize it - internalize it - check whether font has been created
@SYMTestExpectedResults no font created
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/

void CTDefect::PDEF103809L()
	{
	RFbsSession::Connect();

	CFbsScreenDevice* scr=NULL;		//To stop a warning
	TDisplayMode mode[] = {EColor64K, EColor16MA, EColor16MAP};
	TInt ii;
	TInt err = KErrNotSupported;
	for(ii=0;(ii<TInt(sizeof(mode)/sizeof(mode[0]))) && (err == KErrNotSupported);++ii)
		{
		TRAP(err,scr=CFbsScreenDevice::NewL(KDefaultScreenNo,mode[ii]));
		}
	if (err!=KErrNone)
		{
		if (err==KErrNotSupported)
			{
			_LIT(KLog,"Hardware doesn't support mode Color64K, Color16MA or EColor16MAP");
			INFO_PRINTF1(KLog);
			}
		else
			{
			_LIT(KLog,"Failed to created Screen Device %S, err=%d");
			INFO_PRINTF3(KLog,&ColorModeName(mode[ii]),err);
			}
		User::Leave(err);
		}
	_LIT(KLog,"Created Screen Device with mode %S");
	INFO_PRINTF2(KLog,&ColorModeName(mode[ii]));
	scr->SetAutoUpdate(ETrue);
	CleanupStack::PushL(scr);

	CFbsBitGc* gc = NULL;
	User::LeaveIfError(scr->CreateContext(gc));
	CleanupStack::PushL(gc);

	// before 
	TBool before = gc->IsFontUsed();
	INFO_PRINTF2(_L("before: %d"),before);
	TEST(before==EFalse); 
	
	// create buffer for save/restore
	CBufFlat* buf = CBufFlat::NewL(256);
	CleanupStack::PushL(buf);
	
	// save and then restore
	RBufWriteStream ws(*buf,0);
	gc->ExternalizeL(ws);
	ws.Close();
	RBufReadStream rs(*buf,0);	
	gc->InternalizeL(rs);
	rs.Close();

	// after
	TBool after = gc->IsFontUsed();
	INFO_PRINTF2(_L("after: %d"),after);
	TEST(after==EFalse);

	CleanupStack::PopAndDestroy(3, scr);
	RFbsSession::Disconnect();
	}

//Method Initialising src and mask bitmap for DEF104856, DEF108338, DEF103736, PDEF108863 and PDEF110934
void CTDefect::InitialiseSourceAndMaskLC(CFbsBitmap*& aSrc, TDisplayMode aSrcDispMode, CFbsBitmap*& aMask, TDisplayMode aMaskDispMode, TSize aPicSize, TInt aNumMaskBars, TInt aMaskBarHeight, TSize* aMaskSize)
	{
	TInt err;
    // create src bitmap object
	aSrc = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(aSrc);
	err = aSrc->Create(aPicSize, aSrcDispMode);

	// create mask bitmap object
	aMask = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(aMask);
	if(aMaskSize == NULL )
		{
		err = aMask->Create(aPicSize, aMaskDispMode);
		}
	else
		{
		err = aMask->Create(*aMaskSize, aMaskDispMode);
		}
	TEST(err == KErrNone);
	
	// initialise source
	TBitmapUtil srcUtil(aSrc);
	srcUtil.Begin(TPoint(0, 0));
	for(TInt y = 0; y < aPicSize.iHeight; ++y)
		{
		for(TInt x = 0; x < aPicSize.iWidth; ++x)
			{
			srcUtil.SetPos(TPoint(x, y));
			srcUtil.SetPixel(Color(KRgbRed, aSrcDispMode));
			}
		}
	srcUtil.End();
	err = aSrc->Compress();
	TEST(err == KErrNone);

    // initialise mask
    TBitmapUtil maskUtil(aMask);
    maskUtil.Begin(TPoint(0, 0));
	TUint32 maskColor;
    if(aMaskSize != NULL)
	    {
	    aPicSize = *aMaskSize;
	    }
	for(TInt y = 0; y < aPicSize.iHeight; ++y)
		{
		for(TInt x = 0; x < aPicSize.iWidth; ++x)
			{
			maskUtil.SetPos(TPoint(x, y));
 			if (((x / (aPicSize.iWidth / aNumMaskBars)) & 1) ^ ((y / aMaskBarHeight) & 1))
 				{
				maskColor = Color(KRgbWhite, aMaskDispMode);
 				}
 			else
 				{
				maskColor = Color(KRgbBlack, aMaskDispMode);
 				}
			maskUtil.SetPixel(maskColor);
			}
		}
	maskUtil.End();
	err = aMask->Compress();
	TEST(err == KErrNone);
	}

/**
Method to check images drawn for TestDoDrawBitmapMaskedL() and TestDoBitBltMaskedL() against expected images drawn.
This method will start checking from point (0,0) of the device drawn to.
@param aDevice Device drawn to
@param aWidth Width of area to check
@param aHeight Height of area to check
@param aPattern Pattern to determine which colour to start checking against
@param aBarHeight Expected height of alternating bars in pixels
*/
void CTDefect::CheckResults(CFbsDevice* aDevice, TInt aWidth, TInt aHeight, TInt aPattern, TInt aBarHeight)
	{
	TRgb color;
	TRgb checkColor = aPattern == 1? KRgbGreen : KRgbRed;
	
	for(TInt y = 0; y < aHeight; ++y)
		{
		for(TInt x = 0; x < aWidth; ++x)
			{
			aDevice->GetPixel(color, TPoint(x, y));
			if(color != checkColor)
				{
				TEST(EFalse);
				return;
				}
			}
		if (y % aBarHeight == aBarHeight - 1)
			{
			checkColor = checkColor == KRgbGreen ? KRgbRed : KRgbGreen;
			}
		}
	}

void CTDefect::TestDoBltMaskedL(TDisplayMode aSrcDispMode, TDisplayMode aMaskDispMode, TDisplayMode aDeviceDispMode, TSize aPicSize, TBool aUseScreenDevice, TBool aInvertMask, TSize* aMaskSize)
	{
	// create and initialise source and mask bitmap
	CFbsBitmap* src = NULL;
	CFbsBitmap* mask = NULL;
	// Number of vertical changes to mask colour in a row
	const TInt KNumMaskBars = 4;
	// Height of each bar in pixels
	const TInt KMaskBarHeight = 1;

	InitialiseSourceAndMaskLC(src, aSrcDispMode, mask, aMaskDispMode, aPicSize, KNumMaskBars, KMaskBarHeight, aMaskSize);
	TInt err;
	CFbsDevice* fbsDevice = NULL;
	if (aUseScreenDevice)
		{
		// create screen device
		TRAPD(err, fbsDevice = CFbsScreenDevice::NewL(KNullDesC, aDeviceDispMode));
		if (err == KErrNotSupported)
			{
			INFO_PRINTF1(_L("Display mode is not supported by scdv"));
			CleanupStack::PopAndDestroy(2, src);
			return;
			}
		}
	else
		{
	    // create device bitmap
		CFbsBitmap* deviceBitmap = new(ELeave) CFbsBitmap;
		err = deviceBitmap->Create(aPicSize, aDeviceDispMode);
		TEST(err == KErrNone);
		CleanupStack::PushL(deviceBitmap);
		// create bitmap device
		fbsDevice = CFbsBitmapDevice::NewL(deviceBitmap);
		}
	CleanupStack::PushL(fbsDevice);
	// create graphics context
	CFbsBitGc* gcContext = NULL;
	err = fbsDevice->CreateContext(gcContext);
	TEST(err == KErrNone);
	TEST(gcContext != NULL);
	CleanupStack::PushL(gcContext);
	gcContext->SetBrushColor(KRgbGreen);
	
	TInt barwidth = aPicSize.iWidth/KNumMaskBars;
	TSize barSize(barwidth, aPicSize.iHeight);
	TInt currentPattern = aInvertMask ? 0 : 1;
	for(TInt x = 0; x < aPicSize.iWidth; x += barwidth)
		{
		gcContext->Clear();
		gcContext->BitBltMasked(TPoint(0, 0), src, TRect(TPoint(x, 0), barSize), mask, aInvertMask);
		CheckResults(fbsDevice, barwidth, aPicSize.iHeight, currentPattern, KMaskBarHeight);
		currentPattern ^= 1;
		}
	if (aUseScreenDevice)
		{
		CleanupStack::PopAndDestroy(4, src); //  gcContext, fbsDevice, mask, src
		}
	else
		{
		CleanupStack::PopAndDestroy(5, src); // gcContext, fbsDevice, deviceBitmap, mask, src	
		}
	}

/**
@SYMTestCaseID		GRAPHICS-BITGDI-0095

@SYMDEF             PDEF110934

@SYMTestCaseDesc    Test that the bitmap is drawn correctly by testing
					mask size less or greater than the bitmap size. Three optimized calls tested for this
					CFbsBitGc::DoBitBltMaskedNonFlicker
					CFbsBitGc::DoBitBltMaskedFlicker
					CFbsBitGc::DoBitBltAlpha

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Create compressed EColor64K bitmap and EGray2 mask and draw bitmap calling
					CFbsBitGc::BitBltMasked with a screen device. The drawing is done in a loop
					to simulate the scrolling effect as in a browser.

@SYMTestExpectedResults All the stripes should pass the integrity check.
**/
void CTDefect::TestDoBitBltMaskedSizeL()
	{
	TSize maskSize(96,48);

	// Tests DoBitBltMaskedFlicker for which mask height is less than the source bitmap height
	TestDoBltMaskedL(EColor64K, EGray2, EColor16MU, TSize(96,96), EFalse, EFalse, &maskSize);
	
	if(CFbsDrawDevice::DisplayMode16M() != ENone)
		{
		// Tests DoBitBltMaskedNonFlicker for which mask height is less than the source bitmap height
		TestDoBltMaskedL(EColor64K, EGray2, EColor16MU, TSize(96,96), ETrue, EFalse, &maskSize);
		}
	
	// Tests DoBitBltAlpha for which mask height is less than the source bitmap height
	TestDoBltMaskedL(EColor16MU, EGray256, EColor16MU, TSize(96,96), EFalse, EFalse, &maskSize);
	
	//Change the mask height and check the optimization for maximum height of the source bitmap
	maskSize.SetSize(96,144);
	// Tests DoBitBltMaskedFlicker for which mask height is greater than the source bitmap height
	TestDoBltMaskedL(EColor64K, EGray2, EColor16MU, TSize(96,96), EFalse, EFalse, &maskSize);

	if(CFbsDrawDevice::DisplayMode16M() != ENone)
		{
		// Tests DoBitBltMaskedNonFlicker for which mask height is greater than the source bitmap height
		TestDoBltMaskedL(EColor64K, EGray2, EColor16MU, TSize(96,96), ETrue, EFalse, &maskSize);
		}
	
	// Tests DoBitBltAlpha for which mask height is greater than the source bitmap height
	TestDoBltMaskedL(EColor16MU, EGray256, EColor16MU, TSize(96,96), EFalse, EFalse, &maskSize);
	}

TUint32 CTDefect::Color(const TRgb& aColor, const TDisplayMode& aDisplayMode)
	{
	switch (aDisplayMode)
		{
	case EGray2:
		return aColor.Gray2();
	case EGray4:
		return aColor.Gray4();
	case EGray16:
		return aColor.Gray16();
	case EGray256:
		return aColor.Gray256();
	case EColor16:
		return aColor.Color16();
	case EColor256:
		return aColor.Color256();
	case EColor64K:
		return aColor.Color64K();
	case EColor16M:
		return aColor.Color16M();
	case EColor16MU:
		return aColor.Color16MU();
	case EColor16MA:
		return aColor.Color16MA();
	case EColor16MAP:
		return aColor.Color16MAP();
	default:
		__ASSERT_DEBUG(EFalse, User::Invariant());
		return 0;
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0102
 
  @SYMDEF ExternalizeInternalizeCFbsBitGCExtraData          

  @SYMTestCaseDesc Test externalize/internalize of shadow color in CFbsBitGc
   
  @SYMTestPriority Medium

  @SYMTestStatus Implemented

  @SYMTestActions Create CFbsBitGc and set the shadow color by calling SetShadowColor(). 
   Now - externalize it - internalize it - check the shadow color value matches the one that was set
   before externalize/internalize.
   
@SYMTestExpectedResults Shadow color value should be same after externalize/internalize.
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/

void CTDefect::ExternalizeInternalizeCFbsBitGCExtraData()
	{
	RFbsSession::Connect();

	TInt err;
	err = CreateScreenDeviceL(EColor64K);
	if (err == KErrNotSupported)
		err = CreateScreenDeviceL(EColor16MA);
	if (err == KErrNotSupported)
		err = CreateScreenDeviceL(EColor16MAP);
	if (err == KErrNotSupported)
		return;
	User::LeaveIfError(err);
	_LIT(KLog1,"Created %S Screen Device");
	INFO_PRINTF2(KLog1,&ColorModeName(iDevice->DisplayMode()));
	iDevice->SetAutoUpdate(ETrue);

	CFbsBitGc* gc = NULL;
	User::LeaveIfError(iDevice->CreateContext(gc));
	CleanupStack::PushL(gc);

	// before 
	const TRgb color = TRgb(10,20,30);
	gc->SetShadowColor(color);

	// create buffer for save/restore
	CBufFlat* buf = CBufFlat::NewL(256);
	CleanupStack::PushL(buf);
	
	// save and then restore
	RBufWriteStream ws(*buf,0);
	gc->ExternalizeL(ws);
	ws.Close();
	gc->SetShadowColor(TRgb(0,0,0));
	RBufReadStream rs(*buf,0);	
	gc->InternalizeL(rs);

	// after
	TRgb shadowColor;
	gc->GetShadowColor(shadowColor);
	TEST(color == shadowColor);
	rs.Close();

	CleanupStack::PopAndDestroy(2,gc);
	RFbsSession::Disconnect();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0107
 
  @SYMDEF DEF126160 Clipped scaled-down bitmaps drawn incorrectly using CFbsBitmap::DrawBitmap()          

  @SYMTestCaseDesc Test bitmap scaling when drawn with a clip rect applied.
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions - Create a CFbsBitmap and draw three horozontal stripes to it.
  - Draw the bitmap scaled down on the left of the screen using DrawBitmap(), and scaled down in the 
  same way but with a clip rect applied on the right of the screen.
  - Compare what is drawn on the left with what is drawn on the right (in the appropraite clip rect area only).
  - Repeat but with the bitmap scaled up.
  - Repeat with no scaling.
   
  @SYMTestExpectedResults What is drawn on the left should match what is drawn on the right 
  (in the appropraite clip rect area only).
  If this test was run without the fix for DEF126160, what is drawn clipped on the right when the bitmap is
  scaled down would not match what is drawn on the left.
*/
void CTDefect::TestClippedScaledBitmapL()
	{	
	const TDisplayMode KBitmapMode = EColor64K;
	const TDisplayMode KDisplayMode[] = {EColor256, EColor4K, EColor16M, EColor16MU, EColor64K, 
						   EGray256, EGray16, EGray4, EGray2, EColor16, EColor16MA, EColor16MAP};
	// Create a screen device and gc for drawing the test bitmap to
	TInt ii = 0;
	TInt err = KErrNotSupported;
	CFbsScreenDevice* scrDev = NULL;
	for(;(ii<TInt(sizeof(KDisplayMode)/sizeof(KDisplayMode[0]))) && (err == KErrNotSupported);++ii)
		{
		TRAP(err, scrDev = CFbsScreenDevice::NewL(0,KDisplayMode[ii]));
		}
	if (err != KErrNone)
		{
		_LIT(KLog,"Failed to create screen device %S return value %d");
		INFO_PRINTF3(KLog,&ColorModeName(KDisplayMode[ii]),err);
		User::Leave(err);
		}
	else
		{
		_LIT(KLog,"Created Screen Device with mode %S");
		INFO_PRINTF2(KLog,&ColorModeName(KDisplayMode[ii]));
		}
	
	User::LeaveIfNull(scrDev);
	scrDev->SetAutoUpdate(EFalse);
	CleanupStack::PushL(scrDev);
	
	// Set the bitmap size as one quarter of the screen
	TSize bmpSize(scrDev->SizeInPixels().iWidth/2, scrDev->SizeInPixels().iHeight/2); 

	CFbsBitGc* scrGc = NULL;
	User::LeaveIfError(scrDev->CreateContext(scrGc));
	CleanupStack::PushL(scrGc);
	
	// Create the test bitmap
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	User::LeaveIfError(bmp->Create(bmpSize, KBitmapMode));	
	
	CFbsBitmapDevice* bmpDev = CFbsBitmapDevice::NewL(bmp);
	User::LeaveIfNull(bmpDev);
	CleanupStack::PushL(bmpDev);
	
	CFbsBitGc* bmpGc;
	User::LeaveIfError(bmpDev->CreateContext(bmpGc));
	CleanupStack::PushL(bmpGc);
	
	// Draw a series of diagonal lines across the bitmap
	const TInt KNumTestColors = 7;
	const TRgb KTestColors[KNumTestColors] = {KRgbRed, KRgbBlue, KRgbGreen, KRgbYellow, KRgbMagenta, KRgbDarkGreen, KRgbDarkYellow};
	const TInt KPenSize = 20;	
	bmpGc->SetPenSize(TSize(KPenSize,KPenSize));	
	for (TInt i = -KPenSize; i < bmpSize.iWidth+bmpSize.iHeight; i += KPenSize)
		{
		bmpGc->SetPenColor(KTestColors[i%KNumTestColors]);
		bmpGc->DrawLine(TPoint(i,0), TPoint(0,i));
		}	
	
	TRect lhsRect;
	TRect rhsRect;
	TRect lhsCompareRect;
	TRect rhsClipRect;
	scrGc->Clear();
	
	// Scale down - this is the case that would fail without the fix for DEF126160
	INFO_PRINTF1(_L("- Scaling Down"));
	lhsRect.SetRect(TPoint(0,0), TSize(bmpSize.iWidth, bmpSize.iHeight/2));
	rhsRect = lhsRect;
	rhsRect.Move(scrDev->SizeInPixels().iWidth/2, 0);				
	lhsCompareRect.SetRect(TPoint(0,lhsRect.Height()/2), TSize(lhsRect.Width(), lhsRect.Height()/2));
	rhsClipRect = lhsCompareRect;
	rhsClipRect.Move(scrDev->SizeInPixels().iWidth/2, 0);				
	scrGc->DrawBitmap(lhsRect, bmp);
	scrGc->SetClippingRect(rhsClipRect);
	scrGc->DrawBitmap(rhsRect, bmp);
	scrDev->Update();
	TEST(scrDev->RectCompare(lhsCompareRect, *scrDev, rhsClipRect));
	
	scrGc->CancelClippingRect();
	scrGc->Clear();
	
	// Scale up
	INFO_PRINTF1(_L("- Scaling Up"));
	lhsRect.SetRect(TPoint(0,0), TSize(bmpSize.iWidth, bmpSize.iHeight*2));
	rhsRect = lhsRect;
	rhsRect.Move(scrDev->SizeInPixels().iWidth/2, 0);				
	lhsCompareRect.SetRect(TPoint(0,lhsRect.Height()/2), TSize(lhsRect.Width(), lhsRect.Height()/2));
	rhsClipRect = lhsCompareRect;
	rhsClipRect.Move(scrDev->SizeInPixels().iWidth/2, 0);				
	scrGc->DrawBitmap(lhsRect, bmp);
	scrGc->SetClippingRect(rhsClipRect);
	scrGc->DrawBitmap(rhsRect, bmp);
	scrDev->Update();
	TEST(scrDev->RectCompare(lhsCompareRect, *scrDev, rhsClipRect));
	
	scrGc->CancelClippingRect();
	scrGc->Clear();
	
	// No scaling
	INFO_PRINTF1(_L("- No Scaling"));
	lhsRect.SetRect(TPoint(0,0), bmpSize);
	rhsRect = lhsRect;
	rhsRect.Move(scrDev->SizeInPixels().iWidth/2, 0);				
	lhsCompareRect.SetRect(TPoint(0,lhsRect.Height()/2), TSize(lhsRect.Width(), lhsRect.Height()/2));
	rhsClipRect = lhsCompareRect;
	rhsClipRect.Move(scrDev->SizeInPixels().iWidth/2, 0);
	scrGc->DrawBitmap(lhsRect, bmp);
	scrGc->SetClippingRect(rhsClipRect);
	scrGc->DrawBitmap(rhsRect, bmp);
	scrDev->Update();
	TEST(scrDev->RectCompare(lhsCompareRect, *scrDev, rhsClipRect));	
	
	CleanupStack::PopAndDestroy(5, scrDev);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0109
 
  @SYMDEF DEF128522 EndDataAccess() not called when CFbsBitGc::DrawBitmap() returns          

  @SYMTestCaseDesc Test that EndDataAccess() is called correctly when a bitmap is drawn
  using CFbsBitGc::DrawBitmap(const TPoint&, const CFbsBitmap*) with twips not set.
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions 
  - Create a CFbsBitmap.
  - Draw using CFbsBitGc::DrawBitmap(const TPoint&, const CFbsBitmap*) without setting the twips.
  - Call CFbsBitmap::DataAddress() on the bitmap, this should mark the bitmap
  as volatile as DataAddress() was called outside a BeginDataAccess()/EndDataAccess() pair.
  - Call CFbsBitmap::IsVolatile() and check the result.
   
  @SYMTestExpectedResults 
  CFbsBitmap::IsVolatile() should return ETrue
*/
void CTDefect::TestDrawBitmapTwipsNotSet()
	{
	const TDisplayMode KBitmapMode = EColor64K;
		
	// Create a screen device and gc for drawing the test bitmap to
	const TDisplayMode KDisplayMode[] = {EColor256, EColor4K, EColor16M, EColor16MU, EColor64K, 
						   EGray256, EGray16, EGray4, EGray2, EColor16, EColor16MA, EColor16MAP};
	// Create a screen device and gc for drawing the test bitmap to
	TInt ii = 0;
	TInt err = KErrNotSupported;
	CFbsScreenDevice* scrDev = NULL;
	for(;(ii<TInt(sizeof(KDisplayMode)/sizeof(KDisplayMode[0]))) && (err == KErrNotSupported);++ii)
		{
		TRAP(err, scrDev = CFbsScreenDevice::NewL(0,KDisplayMode[ii]));
		}
	if (err != KErrNone)
		{
		_LIT(KLog,"Failed to create screen device %S return value %d");
		INFO_PRINTF3(KLog,&ColorModeName(KDisplayMode[ii]),err);
		User::Leave(err);
		}
	else
		{
		_LIT(KLog,"Created Screen Device with mode %S");
		INFO_PRINTF2(KLog,&ColorModeName(KDisplayMode[ii]));
		}
	
	User::LeaveIfNull(scrDev);
	CleanupStack::PushL(scrDev);		

	CFbsBitGc* scrGc = NULL;
	User::LeaveIfError(scrDev->CreateContext(scrGc));
	CleanupStack::PushL(scrGc);
	
	// Create a test bitmap
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bmp);
	User::LeaveIfError(bmp->Create(TSize(100,100), KBitmapMode));		
		
	// Draw the bitmap, note that twips have not been set so that DrawBitmap() returns
	// directly after the check for twips being set
	scrGc->DrawBitmap(TPoint(0,0), bmp);
	
	// Get the data address of the bitmap. If EndDataAccess() was called correctly
	// when DrawBitmap() returned, the bitmap will now be marked as volatile as DataAddress()
	// has been called outside a BeginDataAccess()/EndDataAccess() pair. If the bitmap is not
	// marked as volatile then EndDataAccess() was not called correctly in DrawBitmap() when
	// it returned.
	TUint32* data = bmp->DataAddress();
	TBool isVolatile = bmp->IsVolatile();	
	TEST(isVolatile);
	
	CleanupStack::PopAndDestroy(3, scrDev);
	}


/**
  @SYMTestCaseID GRAPHICS-BITGDI-0119
 
  @SYMDEF INC138992

  @SYMTestCaseDesc Test CFbsBitGC:: DrawBitmapMasked() with compressed source and mask bitmaps,
   where the target area is larger or smaller than source bitmap's size.
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions For each configuration:
	Create a compressed source and a compressed mask bitmap.
	Draw the bitmaps to the device, once with the target area smaller than the size of the source bitmap,
	once with the target area larger than the source bitmap.
	Compare the actual image drawn with that expected.
	
	All combinations of following configurations tested :
		Mask with display modes EGray2 and EGray256
		Device with display modes EColor64K and EColor16MAP
		Mask with sizes same as source bitmap and smaller than source bitmap	
 
  @SYMTestExpectedResults Images drawn should be as expected. 
*/	
void CTDefect::TestDrawBitmapMaskedL()
	{
	// Mask size is same as source bitmap size.  This covers same code paths as those using a larger mask size.
	INFO_PRINTF1(_L(" - with EGray2 mask, EColor64K source and EColor64K display"));
	TestDoDrawBitmapMaskedL(EColor64K, EGray2, EColor64K, TSize(96,48));

	INFO_PRINTF1(_L(" - with EGray256 mask, EColor64K source and EColor64K display"));
	TestDoDrawBitmapMaskedL(EColor64K, EGray256, EColor64K, TSize(96,48));

	INFO_PRINTF1(_L(" - with EGray2 mask, EColor64K source and EColor16MAP display"));
	TestDoDrawBitmapMaskedL(EColor64K, EGray2, EColor16MAP, TSize(96,48));

	INFO_PRINTF1(_L(" - with EGray256 mask, EColor64K source and EColor16MAP display"));
	TestDoDrawBitmapMaskedL(EColor64K, EGray256, EColor16MAP, TSize(96,48));

	// Mask size is smaller than source bitmap size.
	// This covers different code paths to above.
	TSize maskSize(48,24);

	INFO_PRINTF1(_L(" - with small EGray2 mask, EColor64K source and EColor64K display"));
	TestDoDrawBitmapMaskedL(EColor64K, EGray2, EColor64K, TSize(96,48), &maskSize);

	INFO_PRINTF1(_L(" - with small EGray256 mask, EColor64K source and EColor64K display"));
	TestDoDrawBitmapMaskedL(EColor64K, EGray256, EColor64K, TSize(96,48), &maskSize);

	INFO_PRINTF1(_L(" - with small EGray2 mask, EColor64K source and EColor16MAP display"));
	TestDoDrawBitmapMaskedL(EColor64K, EGray2, EColor16MAP, TSize(96,48), &maskSize);

	INFO_PRINTF1(_L(" - with small EGray256 mask, EColor64K source and EColor16MAP display"));
	TestDoDrawBitmapMaskedL(EColor64K, EGray256, EColor16MAP, TSize(96,48), &maskSize);
	}

/**
Tests drawing masked bitmaps with both stretched (twice original size) and squashed (half original size) destination rectangles
*/
void CTDefect::TestDoDrawBitmapMaskedL(TDisplayMode aSrcDispMode, TDisplayMode aMaskDispMode, TDisplayMode aDeviceDispMode, TSize aPicSize, TSize* aMaskSize)
	{
	// create and initialise source and mask bitmap
	CFbsBitmap* src = NULL;
	CFbsBitmap* mask = NULL;
	// Number of vertical changes to mask colour in a row
	const TInt KNumMaskBars = 2;
	// Height of each bar in pixels
	const TInt KMaskBarHeight = 2;

	InitialiseSourceAndMaskLC(src, aSrcDispMode, mask, aMaskDispMode, aPicSize, KNumMaskBars, KMaskBarHeight, aMaskSize);
	
	TInt err;
	CFbsScreenDevice* fbsDevice = NULL;
	// create screen device
	TRAP(err, fbsDevice = CFbsScreenDevice::NewL(KNullDesC, aDeviceDispMode));
	if (err == KErrNotSupported)
		{
		WARN_PRINTF1(_L("Display mode is not supported by scdv"));
		CleanupStack::PopAndDestroy(2, src);
		return;
		}
	CleanupStack::PushL(fbsDevice);
	// create graphics context
	CFbsBitGc* gcContext = NULL;
	err = fbsDevice->CreateContext(gcContext);
	TEST(err == KErrNone);
	TEST(gcContext != NULL);
	CleanupStack::PushL(gcContext);
	gcContext->SetBrushColor(KRgbGreen);

	const TInt KNumScales = 2;
	// Scale ratios for destination rects.
	const TReal32 KScale[KNumScales] = {0.5, 2.0};

	TInt barwidth = (aMaskSize ? aMaskSize->iWidth : aPicSize.iWidth)/KNumMaskBars;
	TSize barSize(barwidth, aPicSize.iHeight);

	for (TInt ii = 0; ii < KNumScales; ++ii)
		{
		TSize scaledBarSize(barwidth*KScale[ii], aPicSize.iHeight*KScale[ii]);
		TInt currentPattern = 1; //i.e. mask not inverted
		TRect scaledDestRect(TPoint(0,0),scaledBarSize);
		for(TInt x = 0; x < aPicSize.iWidth; x += barwidth)
			{
			gcContext->Clear();
			gcContext->DrawBitmapMasked(scaledDestRect,src,TRect(TPoint(x, 0), barSize), mask, EFalse);
			fbsDevice->Update();
			CheckResults(fbsDevice, scaledBarSize.iWidth, scaledBarSize.iHeight, currentPattern, KMaskBarHeight*KScale[ii]);
			currentPattern ^= 1;
			}
		}

	CleanupStack::PopAndDestroy(4, src); //  gcContext, fbsDevice, mask, src
	}

//--------------
__CONSTRUCT_STEP__(Defect)

void CTDefectStep::TestSetupL()
	{
	TheFs.Connect();
	::CopyDataFiles();
	}
	
void CTDefectStep::TestClose()
	{
	::DeleteDataFiles();	
	TheFs.Close();
	}
