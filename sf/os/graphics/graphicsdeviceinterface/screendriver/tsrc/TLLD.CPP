// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <hal.h>
#include <bitdraw.h>
#include "Tlld.h"
#include <bitdrawinterfaceid.h>
#include <bmalphablend.h>
#include <graphics/lookuptable.h>
#include <graphics/blendingalgorithms.h>
#include <graphics/gdi/gdiconsts.h>
#include "BMDRAW.H"

GLREF_C TInt ByteSize(TDisplayMode aDisplayMode,TInt aWidth);

TInt KNumberDisplayModes1 = sizeof (TestDisplayMode1) / sizeof (TestDisplayMode1[0]);

#if defined(SYMBIAN_USE_FAST_FADING)
const TBool KFastFading = ETrue;
#else
const TBool KFastFading = EFalse;
#endif

//these are for EColor16MAP testing
const TInt KInaccuracyLimit = 15;
const TInt KUserDispModes = 2;
//The array below is used in CTLowLevel::TestWriteRGBAlpha() to step through display modes,
//to ensure adequate test coverage.
const TDisplayMode UserDisplayModes[KUserDispModes] =
	{
	ENone,
	EColor16MAP,
	};
const TInt KMaskFill =3;
const TUint32 MaskFill[KMaskFill] =
	{
	0x00,
	0xff,
	0x3A,
	};

TInt DisplayMode2Index(TDisplayMode aMode)
	{
	TInt i;
	for(i=0;i<KNumDispModes;i++)
		{
		if(TestDisplayMode[i] == aMode)
			break;
		}
	return i;
	}

inline TInt AbsDiff(TInt aValue1, TInt aValue2)
	{
	return (aValue1 < aValue2) ? (aValue2-aValue1) : (aValue1-aValue2);
	}

//CTLowLevel implementation is shared between TLLD and TLLD2 test apps.
//The change was made because the original TLLD test app took too much time (over 0.5 hour)
//and usually was terminated with a TIMEOUT on LUBBOCK device.
CTLowLevel::CTLowLevel(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iDrawDevice(NULL),
	iBits(NULL),
	iBuf(NULL),
	iDispMode(ENone),
	iUserDispMode(ENone),
	iOrientation(CFbsDrawDevice::EOrientationNormal),
	iSize(TSize(0,0)),
	iLongWidth(0),
	iTestNo(0),
	iIteration(0),
	iReportIteration(0),
	iTotalReportIterations(0),
	iFuzzyMatch(EFalse),
	iBlendTestColors(ETrue),
	iUseFastFade(EFalse)
	{
	iColorConvertor[ENone] = &iNullConvertor;
	iColorConvertor[EGray2] = &iGray2Convertor;
	iColorConvertor[EGray4] = &iGray4Convertor;
	iColorConvertor[EGray16] = &iGray16Convertor;
	iColorConvertor[EGray256] = &iGray256Convertor;
	iColorConvertor[EColor16] = &iColor16Convertor;
	iColorConvertor[EColor256] = &iColor256Convertor;
	iColorConvertor[EColor4K] = &iColor4KConvertor;
	iColorConvertor[EColor64K] = &iColor64KConvertor;
	iColorConvertor[EColor16M] = &iColor16MConvertor;
	iColorConvertor[ERgb] = &iColor16MConvertor;
	iColorConvertor[EColor16MU] = &iColor16MUConvertor;
	iColorConvertor[EColor16MA] = &iColor16MAConvertor;
	iColorConvertor[EColor16MAP] = &iColor16MAPConvertor;
	iOrientation = CFbsDrawDevice::EOrientationNormal;
	iOrientationEnd = CFbsDrawDevice::EOrientationRotated90;
	}

CTLowLevel::~CTLowLevel()
	{
	Reset();
	}

void CTLowLevel::Reset()
	{
	delete iDrawDevice;
	iDrawDevice = NULL;

	delete iBits;
	iBits = NULL;
	delete iBuf;
	iBuf = NULL;
	iDispMode = ENone;
	iSize.SetSize(0,0);
	iLongWidth = 0;
	}

void CTLowLevel::RunTestCaseL(TInt aCurTestCase)
	{
    ((CTLowLevelStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		INFO_PRINTF1(_L("Bitmap device EGray2"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0004
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0004"));
		TestBitmapDraw(EGray2,TSize(128,100));
		break;
	case 2:
		INFO_PRINTF1(_L("Bitmap device EGray4"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0005
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0005"));
		TestBitmapDraw(EGray4,TSize(112,100));
		break;
	case 3:
		INFO_PRINTF1(_L("Bitmap device EGray16"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0006
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0006"));
		TestBitmapDraw(EGray16,TSize(104,100));
		break;
	case 4:
		INFO_PRINTF1(_L("Bitmap device EGray256"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0007
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0007"));
		TestBitmapDraw(EGray256,TSize(104,100));
		break;
	case 5:
		INFO_PRINTF1(_L("Bitmap device EColor16"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0008
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0008"));
		TestBitmapDraw(EColor16,TSize(104,100));
		break;
	case 6:
		INFO_PRINTF1(_L("Bitmap device EColor256"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0009
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0009"));
		TestBitmapDraw(EColor256,TSize(102,100));
		break;
	case 7:
		INFO_PRINTF1(_L("Bitmap device EColor4K"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0010
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0010"));
		TestBitmapDraw(EColor4K,TSize(100,100));
		break;
	case 8:
		INFO_PRINTF1(_L("Bitmap device EColor64K"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0011
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0011"));
		TestBitmapDraw(EColor64K,TSize(100,100));
		break;
	case 9:
		INFO_PRINTF1(_L("Bitmap device EColor16M"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0012
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0012"));
		TestBitmapDraw(EColor16M,TSize(102,100));
		break;
	case 10:
		INFO_PRINTF1(_L("Bitmap device EColor16MU"));
		iFuzzyMatch=ETrue;
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0013
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0013"));
		TestBitmapDraw(EColor16MU,TSize(102,100));
		iFuzzyMatch=EFalse;
		break;
	case 11:
		INFO_PRINTF1(_L("Bitmap device EColor16MA"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0014
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0014"));
		TestBitmapDraw(EColor16MA,TSize(102,100));
		break;
	case 12:
		INFO_PRINTF1(_L("Bitmap device EColor16MAP"));
		iFuzzyMatch=ETrue;
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0015
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0015"));
		TestBitmapDraw(EColor16MAP,TSize(102,100));
		iFuzzyMatch=EFalse;
		break;
	case 13:
		INFO_PRINTF1(_L("User display mode mapping"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0016
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0016"));
		TestUserDisplayModeMapping();
		((CTLowLevelStep*)iStep)->RecordTestResultL();
		break;
	default:
		if(iOrientation <= iOrientationEnd)
			{
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0017
*/
			((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0017"));
			INFO_PRINTF2(_L("Screen device : %S"), &DisplayModeNames1[iCurScreenDeviceModeIndex]);
			TDisplayMode display = TestDisplayMode1[iCurScreenDeviceModeIndex++];
			TestScreenDrawL(display);
			if(iCurScreenDeviceModeIndex >= KNumberDisplayModes1)
				{
				iCurScreenDeviceModeIndex = 0;
				iOrientation ++;
				}
			}
		else
			{
            ((CTLowLevelStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTLowLevelStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			((CTLowLevelStep*)iStep)->RecordTestResultL();
			}
		break;
		}
	}

/* this function is used for likeness matching, using a fixed inaccuracy */
void CTLowLevel::CheckMatch(TUint32 aFirst,TUint32 aSecond)
	{
	TBool fail=EFalse;
	if (iFuzzyMatch==EFalse)
		fail|=Check(aFirst==aSecond);
	else
		{
		TUint8* val1=static_cast<TUint8*>(static_cast<void*>(&aFirst));
		TUint8* val2=static_cast<TUint8*>(static_cast<void*>(&aSecond));
		fail|=Check(AbsDiff(*val1,*val2)<KInaccuracyLimit);
		fail|=Check(AbsDiff(*(val1+1),*(val2+1))<KInaccuracyLimit);
		fail|=Check(AbsDiff(*(val1+2),*(val2+2))<KInaccuracyLimit);
		fail|=Check(AbsDiff(*(val1+3),*(val2+3))<KInaccuracyLimit);
		}
	if (fail)
		{
		_LIT(KLog,"The values 0x%x and 0x%x don't match, fuzzyMatch=%d (limit=%d)");
		INFO_PRINTF5(KLog,aFirst,aSecond,iFuzzyMatch,KInaccuracyLimit);
		}
	}

void CTLowLevel::TestScreenDrawL(TDisplayMode aDisplayMode)
	{
	TUint startTime=User::TickCount();

	Reset();

	iDispMode = aDisplayMode;

	INFO_PRINTF1(_L("\n"));
	INFO_PRINTF2(_L("Testing Screen driver \"%S\""), &(DisplayModeNames[::DisplayMode2Index(aDisplayMode)]));
	INFO_PRINTF1(_L("\r\n"));
	TInt address = NULL;
	TSize size(0,0);

	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayMemoryAddress,address));
	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayXPixels,size.iWidth));
	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayYPixels,size.iHeight));
	ASSERT(size.iWidth > 0 && size.iHeight > 0 && address != NULL);

	TPckgBuf<TScreenInfoV01> info;
	info().iScreenAddressValid = ETrue;
	info().iScreenAddress = REINTERPRET_CAST(TAny*,address);
	info().iScreenSize = size;

	TRAPD(ret,iDrawDevice = CFbsDrawDevice::NewScreenDeviceL(info(),aDisplayMode));

	if (ret == KErrNotSupported)
		{
		INFO_PRINTF1(_L("Not supported\r\n"));
		return;
		}
	else if (ret != KErrNone)
		User::Panic(_L("Draw device create"),ret);

	iDrawDevice->InitScreen();
	iDrawDevice->SetDisplayMode(iDrawDevice);
	iDrawDevice->SetAutoUpdate(EFalse);
	TBool orientation[4];
	iDrawDevice->OrientationsAvailable(orientation);

	TBool orientationSupported = iDrawDevice->SetOrientation(CFbsDrawDevice::TOrientation(iOrientation));
	if (orientationSupported)
		{
		Check(orientation[iOrientation]);
		if (iOrientation & 1)
			iSize = TSize(size.iHeight,size.iWidth);
		else
			iSize = size;
		iLongWidth = LongWidth(iSize.iWidth,aDisplayMode);
		Test();
		}
	else
		{
		Check(!orientation[iOrientation]);
		INFO_PRINTF1(_L("Orientation not supported\r\n"));
		}

	INFO_PRINTF2(_L("Testing time=%d"), (User::TickCount() - startTime) * 100 / 64);
	}

void CTLowLevel::TestBitmapDraw(TDisplayMode aDisplayMode,const TSize& aSize)
	{
	TUint startTime=User::TickCount();
	Reset();
	iDispMode = aDisplayMode;
	iSize = aSize;
	iLongWidth = LongWidth(aSize.iWidth,aDisplayMode);
	TSize size(0,0);
	INFO_PRINTF1(KNullDesC);

	const TInt byteSize=ByteSize()*iSize.iHeight;
	_LIT(KNoMem,"Not enough memory for bitmap bits");
	iBits = new TUint8[byteSize];
	if(!iBits)
		{
		User::Panic(KNoMem,KErrNoMemory);
		}

	iBuf = new TUint32[byteSize];
	if(!iBuf)
		{
		User::Panic(KNoMem,KErrNoMemory);
		}

	TPckgBuf<TScreenInfoV01> info;
	info().iScreenSize = aSize;
	info().iScreenAddress = NULL;
	info().iScreenAddressValid = ETrue;

	TRAPD(ret, iDrawDevice = CFbsDrawDevice::NewBitmapDeviceL(info(), aDisplayMode, ByteSize() ));
	if (ret == KErrNotSupported)
		{
		INFO_PRINTF1(_L("Not supported\r\n"));
		return;
		}
	else if (ret != KErrNone)
	{
		User::Panic(_L("Draw device create"),ret);
	}
	iDrawDevice->SetAutoUpdate(EFalse);
	//Initialize the iDrowDevice object, if successful val=KErrNone
	TInt val=iDrawDevice->InitScreen();
	TEST(val==KErrNone);
	iDrawDevice->CFbsDrawDevice::SetBits(iBits);
	iDrawDevice->CFbsDrawDevice::ShadowBuffer(10,iBuf);
	iDrawDevice->CFbsDrawDevice::SetUserDisplayMode(iDispMode);
	iDrawDevice->SetDisplayMode(iDrawDevice);
	iDrawDevice->Update();
	iDrawDevice->SetUserDisplayMode(iDispMode);
	iDrawDevice->SetBits(iBits);
	Test();
	TBool orientation[4];
	iDrawDevice->OrientationsAvailable(orientation);
	TBool orientationSupported = iDrawDevice->SetOrientation(CFbsDrawDevice::TOrientation(iOrientation));
	if (orientationSupported)
		{
		Check(orientation[iOrientation]);
		if (iOrientation & 1)
			iSize = TSize(size.iHeight,size.iWidth);
		else
			iSize = size;
		iLongWidth = LongWidth(iSize.iWidth,aDisplayMode);
		}
	else
		{
		Check(!orientation[iOrientation]);
		INFO_PRINTF1(_L("Orientation not supported\r\n"));
		}
	INFO_PRINTF2(_L("Testing time=%d"), (User::TickCount() - startTime) * 100 / 64);
	}

TInt RgbComponent(TRgb aRgb, TInt aRgbIndex)
	{
	return(aRgbIndex==0?aRgb.Red():(aRgbIndex==1?aRgb.Green():aRgb.Blue()));
	}
	
TBool CheckNormalizedValue(TRgb aReadCol, TRgb aNormalizedCol1, TRgb aNormalizedCol2, TRgb aNormalizedCol3, TRgb aNormalizedCol4, TInt aRgbIndex)
	{
	
	const TInt KErrorMargin=5;
	TInt minCol=Min(RgbComponent(aNormalizedCol1,aRgbIndex),
					Min(RgbComponent(aNormalizedCol2,aRgbIndex),
						Min(RgbComponent(aNormalizedCol3,aRgbIndex),RgbComponent(aNormalizedCol4,aRgbIndex))))-KErrorMargin;
	TInt maxCol=Max(RgbComponent(aNormalizedCol1,aRgbIndex),
					Max(RgbComponent(aNormalizedCol2,aRgbIndex),
						Max(RgbComponent(aNormalizedCol3,aRgbIndex),RgbComponent(aNormalizedCol4,aRgbIndex))))+KErrorMargin;
	TInt readComponent=RgbComponent(aReadCol,aRgbIndex);
	return(readComponent>=minCol && readComponent<=maxCol);
	}

void CTLowLevel::CheckNormalizedRgb(TRgb aReadRgb, TRgb aCheckRgb, TDisplayMode aDevDisplayMode, TDisplayMode aUserDisplayMode, TBool aWriteRgbAlphaLine)
	{
	TRgb normalized1=aCheckRgb;
	Normalize(normalized1,aUserDisplayMode);
	if (aDevDisplayMode==EColor16MAP)
		{
		const TInt KNormalizeErrorMargin=3;
		TRgb normalizedPMA1=aCheckRgb;
// Allow error margin for blending errors before calculating value normalized to user display mode
		normalizedPMA1.SetRed(Min(0xFF,normalizedPMA1.Red()+KNormalizeErrorMargin));
		normalizedPMA1.SetGreen(Min(0xFF,normalizedPMA1.Green()+KNormalizeErrorMargin));
		normalizedPMA1.SetBlue(Min(0xFF,normalizedPMA1.Blue()+KNormalizeErrorMargin));
//
		Normalize(normalizedPMA1,aUserDisplayMode);
		TRgb normalizedPMA3=normalizedPMA1;
		normalizedPMA1=TRgb::Color16MAP(normalizedPMA1.Color16MAP());
		TRgb normalizedPMA2=aCheckRgb;
		normalizedPMA2=TRgb::Color16MAP(normalizedPMA2.Color16MAP());
		normalizedPMA2.SetRed(Max(0,normalizedPMA2.Red()-KNormalizeErrorMargin));
		normalizedPMA2.SetGreen(Max(0,normalizedPMA2.Green()-KNormalizeErrorMargin));
		normalizedPMA2.SetBlue(Max(0,normalizedPMA2.Blue()-KNormalizeErrorMargin));
		Normalize(normalizedPMA2,aUserDisplayMode);
		TEST(CheckNormalizedValue(aReadRgb,normalizedPMA1,normalizedPMA2,normalizedPMA3,normalized1,0) &&
				CheckNormalizedValue(aReadRgb,normalizedPMA1,normalizedPMA2,normalizedPMA3,normalized1,1) &&
				CheckNormalizedValue(aReadRgb,normalizedPMA1,normalizedPMA2,normalizedPMA3,normalized1,2));
		}
	else
		{
		if (aDevDisplayMode==EColor64K && aWriteRgbAlphaLine)
			{
	// In EColor64K the WriteRgbAlphaLine code maps to native display mode first, before mapping to
	// user mode then back again to device display mode. So use normalized2 to check for that case
			TRgb normalized2=aCheckRgb;
			Normalize(normalized2,aDevDisplayMode);
			Normalize(normalized2,aUserDisplayMode);
			Normalize(normalized2,aDevDisplayMode);
			TEST(aReadRgb.Color16MU()==normalized2.Color16MU());
			}
		else
			{
			Normalize(normalized1,aDevDisplayMode);
			TEST(aReadRgb.Color16MU()==normalized1.Color16MU());
			}
		}
	}
	
void CTLowLevel::PrepareDestPixel(TDisplayMode aDevDisplayMode, TRgb& aRgb, TInt aDstAlpha)
	{
	CGraphicsContext::TDrawMode drawMode;
	if (aDevDisplayMode==EColor16MAP)
		{
		aRgb.SetAlpha(aDstAlpha);
		drawMode=CGraphicsContext::EDrawModeWriteAlpha;
		}
	else
		{
		aRgb.SetAlpha(0xFF);
		drawMode=CGraphicsContext::EDrawModePEN;
		}
	iDrawDevice->WriteRgbMulti(0,0,1,1,aRgb,drawMode);
	}

void CTLowLevel::CheckMappedRgb(TDisplayMode aDevDisplayMode, TDisplayMode aUserDisplayMode, TRgb aRgb)
	{
	const TInt KMaxAlphaModes=5;
	TInt alphaModeValues[KMaxAlphaModes]={0xFF,0xC0,0x80,0x40,0};
	TInt dstAlphaModeCount=aDevDisplayMode==EColor16MAP?KMaxAlphaModes:1;
	for(TInt dstAlphaMode=0;dstAlphaMode<dstAlphaModeCount;dstAlphaMode++)
		{
		TInt dstAlpha=alphaModeValues[dstAlphaMode];
		for(TInt alphaMode=0;alphaMode<KMaxAlphaModes;alphaMode++)
			{
			TInt alpha=alphaModeValues[alphaMode];
			if ((aUserDisplayMode==EColor16 || aUserDisplayMode==EColor256) && (alpha!=0xFF || dstAlpha!=0xFF))
				continue;	// Mapping to EColor16 or EColor256 with fuzzy blends and PMA losses impossible to check for accurately
			PrepareDestPixel(aDevDisplayMode,aRgb,dstAlpha);
			aRgb.SetAlpha(alpha);
			iDrawDevice->WriteRgb(0,0,aRgb,CGraphicsContext::EDrawModePEN);
			TRgb readRgb=iDrawDevice->ReadPixel(0,0);
			CheckNormalizedRgb(readRgb,aRgb,aDevDisplayMode,aUserDisplayMode,EFalse);
//
			PrepareDestPixel(aDevDisplayMode,aRgb,dstAlpha);
			iDrawDevice->WriteRgbMulti(0,0,1,1,aRgb,CGraphicsContext::EDrawModePEN);
			readRgb=iDrawDevice->ReadPixel(0,0);
			CheckNormalizedRgb(readRgb,aRgb,aDevDisplayMode,aUserDisplayMode,EFalse);
//
			PrepareDestPixel(aDevDisplayMode,aRgb,dstAlpha);
			TUint32 writeBuffer[1];
			writeBuffer[0]=aRgb.Internal();
			TUint8 mask[1]={0xFF};
			iDrawDevice->WriteRgbAlphaLine(0,0,1,(TUint8*)writeBuffer,mask,CGraphicsContext::EDrawModePEN);
			readRgb=iDrawDevice->ReadPixel(0,0);
			CheckNormalizedRgb(readRgb,aRgb,aDevDisplayMode,aUserDisplayMode,ETrue);
			}
		}
	}

void CTLowLevel::TestUserDisplayModeMapping()
	{
	TInt address = NULL;
	TSize size;
	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayMemoryAddress,address));
	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayXPixels,size.iWidth));
	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayYPixels,size.iHeight));
	ASSERT(size.iWidth > 0 && size.iHeight > 0 && address != NULL);

	TPckgBuf<TScreenInfoV01> info;
	info().iScreenAddressValid = ETrue;
	info().iScreenAddress = REINTERPRET_CAST(TAny*,address);
	info().iScreenSize = size;
//
	for (TInt nDispModeDev = 0; nDispModeDev < KNumDispModes; nDispModeDev++)
		{
		TDisplayMode dispModeDev = TestDisplayMode[nDispModeDev];
		if (!TDisplayModeUtils::IsDisplayModeColor(dispModeDev) || TDisplayModeUtils::NumDisplayModeColors(dispModeDev)<65536)
			continue; // Older modes have their quirks we don't want to mess with
		Reset();
		TRAPD(ret,iDrawDevice = CFbsDrawDevice::NewScreenDeviceL(info(),dispModeDev));
		if (ret == KErrNotSupported)
			continue;
		for (TInt nDispMode = 0; nDispMode < KNumDispModes; nDispMode++)
			{
			TDisplayMode userDispMode = TestDisplayMode[nDispMode];
			INFO_PRINTF3(_L("Testing devMode=%d, userMode=%d"), dispModeDev, userDispMode);
			iDrawDevice->SetUserDisplayMode(userDispMode);
			TUint rgbVal=0;
			FOREVER
				{
				CheckMappedRgb(dispModeDev,userDispMode,rgbVal);
				if (rgbVal==0xFFFFFF)
					break;
				rgbVal+=0x010305;
				if (rgbVal>0xFFFFFF)	// We want to make sure we test 0xFFFFFF as a special case
					rgbVal=0xFFFFFF;
				}
			}
		}
	}

void CTLowLevel::Test()
	{
	_LIT(KOrientation,"Orientation: %S");
	TBuf<32> buf;
	buf.Format(KOrientation,&RotationName(iOrientation));
	INFO_PRINTF1(buf);

	iTestNo = 1;
	iIteration = 0;
	iReportIteration = 1;
	iTotalReportIterations = 1;
	TestParams();

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	iTotalReportIterations = KNumDispModes;
	TestReadLine();

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	iTotalReportIterations = KNumShadowModes * KNumDrawModes;
	if(KFastFading && (iDispMode == EColor64K ||iDispMode == EColor16MU || iDispMode == EColor16MA || iDispMode == EColor16MAP))
		{
		iUseFastFade = ETrue;
		}
	TestWriteRgb();

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	TestWriteLine();

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	TestWriteBinary();

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	iTotalReportIterations = 4;
	TestWriteRGBAlpha();

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	iTotalReportIterations = KNumShadowModes;
	TestShadow();
	if(KFastFading && (iDispMode == EColor64K|| iDispMode == EColor16MU || iDispMode == EColor16MA || iDispMode == EColor16MAP))
		{
		iUseFastFade = EFalse;
		}

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	iTotalReportIterations = 1;
	TestWriteAlphaLineEx();

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	iTotalReportIterations = 1;
	TestWriteAlphaLineNoShadowEx();

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	iTotalReportIterations = 1;
	TestWriteMaskLineNoShadowEx();

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	iTotalReportIterations = 1;
	TRAPD(err,((CTLowLevelStep*)iStep)->RecordTestResultL(););
    	if (err!=KErrNone)
    		INFO_PRINTF1(_L("Failed to record test result"));

	((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0003"));
	TestFastBlendBitmapMasked();
	TRAPD(err1,((CTLowLevelStep*)iStep)->RecordTestResultL(););
    	if (err1!=KErrNone)
    		INFO_PRINTF1(_L("Failed to record test result"));

	iTestNo++;
	iIteration = 0;
	iReportIteration = 1;
	iTotalReportIterations = KNumBlendingColors;
	((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0002"));
	//TestWriteRgbOutlineAndShadow(); // commented out pending case resolution #327407
	TRAP(err,((CTLowLevelStep*)iStep)->RecordTestResultL(););
    	if (err!=KErrNone)
    		INFO_PRINTF1(_L("Failed to record test result"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0001
*/
	((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0001"));
	}

void CTLowLevel::TestParams()
	{
	Check(iDrawDevice->SizeInPixels()==iSize);
	Check(iDrawDevice->DisplayMode()==iDispMode);
	Check(iDrawDevice->LongWidth()==iLongWidth);
	Check(iDrawDevice->ScanLineBuffer()!=NULL);
	Check(iLongWidth%(iDrawDevice->ScanLineBytes())==0
			|| iDrawDevice->ScanLineBytes() == iLongWidth * 2
			|| iDrawDevice->ScanLineBytes() == iLongWidth * 3
			|| iDrawDevice->ScanLineBytes() == iLongWidth * 4);
	TInt hT = iDrawDevice->HorzTwipsPerThousandPixels();
	Check(hT >= 0);
	TInt vT = iDrawDevice->VertTwipsPerThousandPixels();
	Check(vT >= 0);
	Report();
	}

void CTLowLevel::TestReadLine()
	{
	TInt byteSize = ByteSize();
	TUint8* writeBuffer = new TUint8[byteSize];
	TUint8* readBuffer = new TUint8[iSize.iWidth * sizeof(TRgb)];
	Check(writeBuffer != NULL);
	Check(readBuffer != NULL);

	for (TInt nDispMode = 0; nDispMode < KNumDispModes; nDispMode++)
		{
		TDisplayMode dispMode = TestDisplayMode[nDispMode];
		iDrawDevice->SetUserDisplayMode(dispMode);

		if (dispMode == EColor16MU || dispMode == EColor16MAP || iDispMode == EColor16MU || iDispMode == EColor16MAP)
			iFuzzyMatch=ETrue;
		else
			iFuzzyMatch=EFalse;
		for (TInt cnt=0;cnt<2;cnt++)
			{
			if (cnt==0)  //first time
				iDrawDevice->SetUserDisplayMode(dispMode);
			else
				iDrawDevice->SetUserDisplayMode(iDispMode);

			for (TInt nRect = 0; nRect < KNumTestRects; nRect++)
				{
				TRect rect = TestRect[nRect];
				for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
					{
					Clear(KRgbWhite);

					FillBuffer(writeBuffer, byteSize, iDispMode, ETrue);
					Mem::FillZ(readBuffer,byteSize);

					//we select EDrawModeWriteAlpha because do not test blending here
					iDrawDevice->WriteLine(rect.iTl.iX,yy,rect.Width(),(TUint32*)writeBuffer,CGraphicsContext::EDrawModeWriteAlpha);
					iDrawDevice->ReadLine(rect.iTl.iX,yy,rect.Width(),(TUint32*)readBuffer,dispMode);

					CheckBuffer(writeBuffer,iDispMode,readBuffer,dispMode,rect.Width());

					Mem::FillZ(readBuffer,byteSize);

					//we select EDrawModeWriteAlpha because do not test blending here
					iDrawDevice->WriteLine(iSize.iWidth-rect.Width(),yy,rect.Width(),(TUint32*)writeBuffer,CGraphicsContext::EDrawModeWriteAlpha);
					iDrawDevice->ReadLine(iSize.iWidth-rect.Width(),yy,rect.Width(),(TUint32*)readBuffer,dispMode);

					CheckBuffer(writeBuffer,iDispMode,readBuffer,dispMode,rect.Width());
					iIteration++;
					}
				}
			}
		if (iDispMode != EColor16MU && iDispMode != EColor16MAP)
			iFuzzyMatch=EFalse;
		Report();
		}
	delete [] writeBuffer;
	delete [] readBuffer;
	}

void CTLowLevel::TestWriteRgb()
	{
	for (TInt shadowMode = 0; shadowMode < KNumShadowModes; shadowMode++)
		{
		for (TInt nMode = 0; nMode < KNumDrawModes; nMode++)
			{
			for (TInt nRect = 0; nRect < KNumTestRects; nRect++)
				{
				for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
					{
					for (TInt nColor = 0; nColor < KNumTestColors; nColor++)
						{

						//for modes other than EColor16MAP skip the new colours which have alpha, and cause
						//test failures.  There are two types of colours which need to be skipped, nColor,
						//and nBackColor.  The additional colours were added specificially to test EColor16MAP mode,
						//so the test coverage compared to previously is not reduced for non EColor16MAP modes.
						if (((nColor>=KMaxNon16Colours)|| (nBackColor>=KMaxNon16BackColours)) && (iDispMode!= EColor16MAP))
							continue;

						TRgb bakCol = TestBackground[nBackColor];
						Clear(bakCol);

						CGraphicsContext::TDrawMode dMode = TestDrawMode[nMode];

						if ((iDispMode==EColor16MAP)&&(dMode!=CGraphicsContext::EDrawModePEN))
							continue;


						TRect rect = TestRect[nRect];
						TRgb col = TestColor[nColor];

						iDrawDevice->CFbsDrawDevice::SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
						iDrawDevice->CFbsDrawDevice::SetFadingParameters(100,200);
						iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
						iDrawDevice->WriteRgb(rect.iTl.iX,rect.iTl.iY,col,dMode);

						CheckRgb(rect.iTl,col,bakCol,dMode,shadowMode);
						CheckBackground(TRect(rect.iTl,TSize(1,1)),bakCol);

						Clear(bakCol);

						iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
						iDrawDevice->WriteRgbMulti(rect.iTl.iX,rect.iTl.iY,rect.Width(),rect.Height(),col,dMode);

						CheckRgb(rect,col,bakCol,dMode,shadowMode);
						CheckBackground(rect,bakCol);
						iIteration++;
						}
					}
				}
			Report();
			}
		}
	}

void CTLowLevel::TestWriteLine()
	{
	TInt byteSize = ByteSize();
	TUint8* backBuffer = new TUint8[byteSize];
	TUint8* writeBuffer = new TUint8[byteSize];
	TUint8* copyBuffer = new TUint8[byteSize];
	TUint8* readBuffer = new TUint8[byteSize];
	Check(backBuffer != NULL);
	Check(writeBuffer != NULL);
	Check(copyBuffer != NULL);
	Check(readBuffer != NULL);

	for (TInt shadowMode = 0; shadowMode < KNumShadowModes; shadowMode++)
		{
		for (TInt nMode = 0; nMode < KNumDrawModes; nMode++)
			{
			CGraphicsContext::TDrawMode dMode = TestDrawMode[nMode];
			for (TInt nRect = 0; nRect < KNumTestRects; nRect++)
				{
				for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
					{
					//skip over new colours with alpha for modes other than EColor16MAP
					//these were causing test failures
					if ((nBackColor>=KMaxNon16BackColours) && (iDispMode!= EColor16MAP))
							continue;

					TRgb bakCol = TestBackground[nBackColor];
					Clear(bakCol);
					TRect rect = TestRect[nRect];
					/*TBuf<128> buf;		//Some extra logging that might be useful if this test fails
					_LIT(KLog1,"Shadow=%d, DrawMode=%d, Rect=(%d,%d,%d,%d), Color=%d");
					buf.Format(KLog1,shadowMode,dMode,rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iBr.iY,nBackColor);
					INFO_PRINTF1(buf);*/
					for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
						{
						iDrawDevice->CFbsDrawDevice::SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
						iDrawDevice->CFbsDrawDevice::SetFadingParameters(100,200);
						iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
						iDrawDevice->ReadLine(rect.iTl.iX,yy,rect.Width(),(TUint32*)backBuffer,iDispMode);

						if (nRect != 7)
							{
							// make sure the alpha value is 0xFF when not blending the buffer into 16MU destination
							TBool noAlpha16MU = dMode != CGraphicsContext::EDrawModePEN;
							FillBuffer(writeBuffer, byteSize, iDispMode, noAlpha16MU);
							}
						else // Special check for losing leading 0s in 1 bpp mode at 31 pixel offset
							{
							Mem::Fill(writeBuffer,byteSize,0xff);
							writeBuffer[0] = 0xfe;
							}
						Mem::Copy(copyBuffer,writeBuffer,byteSize);
						iDrawDevice->WriteLine(rect.iTl.iX,yy,rect.Width(),(TUint32*)writeBuffer,dMode);
						Shadow(copyBuffer,byteSize,shadowMode);

						Mem::FillZ(readBuffer,byteSize);
						iDrawDevice->ReadLine(rect.iTl.iX,yy,rect.Width(),(TUint32*)readBuffer,iDispMode);
						CheckLine(copyBuffer,readBuffer,backBuffer,rect.Width(),dMode,iDispMode);

						iIteration++;
						}
					CheckBackground(rect,bakCol);
					}
				}
			Report();
			}
		}
	delete [] backBuffer;
	delete [] writeBuffer;
	delete [] copyBuffer;
	delete [] readBuffer;
	}

void CTLowLevel::TestWriteBinary()
	{
	TInt byteSize = ByteSize();
	TInt wordSize = (byteSize + 3) / 4;
	TUint32* writeBuffer = new TUint32[wordSize];
	Check(writeBuffer != NULL);

	for (TInt shadowMode = 0; shadowMode < KNumShadowModes; shadowMode++)
		{
		for (TInt nMode = 0; nMode < KNumDrawModes; nMode++)
			{
			for (TInt nRect = 0; nRect < KNumTestRects; nRect++)
				{
				for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
					{
					for (TInt nColor = 0; nColor < KNumTestColors; nColor++)
						{
						if (((nColor>=KMaxNon16Colours)|| (nBackColor>=KMaxNon16BackColours)) && (iDispMode!= EColor16MAP))
							continue;

						TRect rect = TestRect[nRect];
						if (rect.Width() > 32)
							{
							rect.iBr.iX = rect.iTl.iX + 32;
							}
						if (rect.Width() < 1)
							rect.iBr.iX = rect.iTl.iX + 1;

						TRgb bakCol = TestBackground[nBackColor];
						TRgb col = TestColor[nColor];
						CGraphicsContext::TDrawMode dMode = TestDrawMode[nMode];

						if ((iDispMode==EColor16MAP)&&(dMode!=CGraphicsContext::EDrawModePEN))
							continue;

						Clear(bakCol);
						iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
						FillBuffer((TUint8*)writeBuffer, byteSize, EGray2);
						iDrawDevice->WriteBinary(rect.iTl.iX,rect.iTl.iY,writeBuffer,rect.Width(),rect.Height(),col,dMode);
						CheckBinary(rect,writeBuffer,col,bakCol,dMode,shadowMode,ETrue,EFalse);
						CheckBackground(rect,bakCol);

						Clear(bakCol);
						iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
						FillBuffer((TUint8*)writeBuffer, byteSize, EGray2);
						iDrawDevice->WriteBinaryLine(rect.iTl.iX,rect.iTl.iY,writeBuffer,rect.Width(),col,dMode);
						CheckBinary(TRect(rect.iTl,TSize(rect.Width(),1)),writeBuffer,col,bakCol,dMode,shadowMode,EFalse,EFalse);
						CheckBackground(TRect(rect.iTl.iX,rect.iTl.iY,rect.iBr.iX,rect.iTl.iY + 1),bakCol);

						Clear(bakCol);
						iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
						FillBuffer((TUint8*)writeBuffer, byteSize, EGray2);
						iDrawDevice->WriteBinaryLineVertical(rect.iTl.iX,rect.iTl.iY,writeBuffer,rect.Height(),col,dMode,EFalse);
						CheckBinary(TRect(rect.iTl,TSize(1,rect.Height())),writeBuffer,col,bakCol,dMode,shadowMode,EFalse,EFalse);
						CheckBackground(TRect(rect.iTl.iX,rect.iTl.iY,rect.iTl.iX + 1,rect.iBr.iY),bakCol);

						Clear(bakCol);
						iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
						FillBuffer((TUint8*)writeBuffer, byteSize, EGray2);
						iDrawDevice->WriteBinaryLineVertical(rect.iTl.iX,rect.iBr.iY - 1,writeBuffer,rect.Height(),col,dMode,ETrue);
						CheckBinary(TRect(rect.iTl,TSize(1,rect.Height())),writeBuffer,col,bakCol,dMode,shadowMode,EFalse,ETrue);
						CheckBackground(TRect(rect.iTl.iX,rect.iTl.iY,rect.iTl.iX + 1,rect.iBr.iY),bakCol);

						iIteration++;
						}
					}
				}
			Report();
			}
		}
	delete [] writeBuffer;
	}

void CTLowLevel::TestWriteAlphaLineEx()
	{
	TAny* interface = NULL;
	TInt err = iDrawDevice->GetInterface(KFastBlitInterfaceID, interface);
	if(err == KErrNone)
		{
		INFO_PRINTF1(_L("START ---->TestWriteAlphaLineEx"));
		TSize size = TSize(30,30);
		TRect rect = TRect(size);

		TUint16* writeBuffer = new TUint16[size.iWidth];
		TUint8* maskBuffer =  new TUint8[size.iWidth];

		TInt nOffset = sizeof(TUint16)*size.iWidth/2;

		Mem::Fill(writeBuffer,nOffset,0xff);
		Mem::Fill((((TUint8*)writeBuffer)+nOffset),nOffset,0x00);
		Mem::Fill(maskBuffer,size.iWidth/2,0x00);
		Mem::Fill((maskBuffer+size.iWidth/2),size.iWidth/2,0xff);

		MFastBlit* fastBlit = reinterpret_cast<MFastBlit*>(interface);
		Clear(KRgbFadedBlack);
		iDrawDevice->SetShadowMode(CFbsDrawDevice::EFade);

		for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
			{
			fastBlit->WriteAlphaLineEx(rect.iTl.iX,yy,rect.Width(),0,(TUint32*)writeBuffer,EColor64K,0,(TUint32*)maskBuffer,MAlphaBlend::EShdwBefore);
			iIteration++;
			}
		CheckRgb(rect,KRgbBlack,KRgbFadedBlack,CGraphicsContext::EDrawModePEN,2);
		CheckBackground(rect,KRgbFadedBlack);

		Report();
		INFO_PRINTF1(_L("END ---->TestWriteAlphaLineEx"));
		delete [] writeBuffer;
		delete [] maskBuffer;
		}
	}

void CTLowLevel::TestWriteAlphaLineNoShadowEx()
	{
	TAny* interface = NULL;
	TInt err = iDrawDevice->GetInterface(KFastBlitInterfaceID, interface);
	if(err == KErrNone)
		{
		INFO_PRINTF1(_L("START ---->TestWriteAlphaLineNoShadowEx"));
		TSize size = TSize(30,30);
		TRect rect = TRect(size);

		TUint16* writeBuffer = new TUint16[size.iWidth];
		Check(writeBuffer != NULL);
		TUint8* maskBuffer =  new TUint8[size.iWidth];
		Check(maskBuffer != NULL);
		TInt nOffset = sizeof(TUint16) * size.iWidth;

		Mem::Fill(writeBuffer,nOffset,0xff);
		Mem::Fill(maskBuffer,size.iWidth/2,0x8e);
		Mem::Fill((maskBuffer+size.iWidth/2),size.iWidth/2,0xff);

		MFastBlit* fastBlit = reinterpret_cast<MFastBlit*>(interface);

		Clear(KRgbWhite);
		for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
			{
			fastBlit->WriteAlphaLineEx(rect.iTl.iX,yy,rect.Width(),0,(TUint32*)writeBuffer,EColor64K,0,(TUint32*)maskBuffer,MAlphaBlend::EShdwBefore);
			iIteration++;
			}
		CheckRgb(rect,KRgbWhite,KRgbWhite,CGraphicsContext::EDrawModePEN,0);

		Report();
		INFO_PRINTF1(_L("END ---->TestWriteAlphaLineNoShadowEx"));
		delete [] writeBuffer;
		delete [] maskBuffer;
		}
	}

void CTLowLevel::TestWriteMaskLineNoShadowEx()
	{
	TAny* interface = NULL;
	TInt err = iDrawDevice->GetInterface(KFastBlitInterfaceID, interface);
	if(err == KErrNone)
		{
		INFO_PRINTF1(_L("START ---->TestWriteMaskLineNoShadowEx"));
		TSize size = TSize(30,30);
		TRect rect = TRect(size);

		TUint16* writeBuffer = new TUint16[size.iWidth];
		Check(writeBuffer != NULL);
		TUint8* maskBuffer =  new TUint8[size.iWidth];
		Check(maskBuffer != NULL);

		TInt nOffset = sizeof(TUint16) * size.iWidth;

		Mem::Fill(writeBuffer,nOffset,0xff);
		Mem::Fill(maskBuffer,size.iWidth/2,0x8e);
		Mem::Fill((maskBuffer+size.iWidth/2),size.iWidth/2,0xff);

		MFastBlit* fastBlit = reinterpret_cast<MFastBlit*>(interface);

		Clear(KRgbWhite);
		for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
			{
			fastBlit->WriteMaskLineEx(rect.iTl.iX,yy,rect.Width(),0,(TUint32*)writeBuffer,EColor64K,0,(TUint32*)maskBuffer,EFalse);
			iIteration++;
			}
		CheckRgb(rect,KRgbWhite,KRgbWhite,CGraphicsContext::EDrawModePEN,0);

		Report();
		INFO_PRINTF1(_L("END ---->TestWriteMaskLineNoShadowEx"));
		delete [] writeBuffer;
		delete [] maskBuffer;
		}
	}

/**
Overwrite the pixel in the buffer with a given mode with a value already in that
mode.
@param aX		The offset in pixels from the start of the buffer.
@param aPtr		The start of the buffer.
@param aMode	The display mode of the buffer.
@param aValue	The new pixel value.
*/
void CTLowLevel::WriteBinaryValue(TUint32 aX, TAny* const aPtr, 
		TDisplayMode aMode, TUint32 aValue) const
	{
	TUint32* buffer32 = (TUint32*)aPtr;
	TInt shift;
	TUint32 mask;

	switch (aMode)
		{
	case EGray2:
		buffer32 += aX >> 5;
		shift = aX & 0x1F;
		mask = 0x1;
		break;
	case EGray4:
		buffer32 += aX >> 4;
		shift = (aX & 0xF) << 1;
		mask = 0x3;
		break;
	case EGray16:
	case EColor16:
		buffer32 += aX >> 3;
		shift = (aX & 0x7) << 2;
		mask = 0xF;
		break;
	case EGray256:
	case EColor256:
		buffer32 += aX >> 2;
		shift = (aX & 0x3) << 3;
		mask = 0xFF;
		break;
	case EColor4K:
		buffer32 += aX >> 1;
		shift = (aX & 0x1) << 4;
		mask = 0xFFF;
		break;
	case EColor64K:
		buffer32 += aX >> 1;
		shift = (aX & 0x1) << 4;
		mask = 0xFFFF;
		break;
	case EColor16M:
		{
		// This mode requires special handling, because shifts and masks
		// won't work.
		TUint8* buffer8 = ((TUint8*)aPtr) + (aX * 3);
		*buffer8++ = aValue & 0xFF;
		*buffer8++ = (aValue >> 8) & 0xFF;
		*buffer8++ = (aValue >> 16) & 0xFF;
		// Return early as the buffer has been updated.
		return;
		}
	case EColor16MU:
		buffer32 += aX;
		shift = 0;
		mask = 0xFFFFFFFF;
		aValue |= 0xFF000000;	// Force alpha to opaque.
		break;
	default:
		buffer32 += aX;
		shift = 0;
		mask = 0xFFFFFFFF;
		break;
		};

	// Write pixel value into right part of word.
	*buffer32 = (*buffer32 & ~(mask << shift)) | ((aValue & mask) << shift);
	}

/**
Copy contents of one buffer over another, depending on a per-pixel mask bit.
@param aSrc		Source pixel buffer
@param aSrcMode	Display mode of source
@param aDst		Destination pixel buffer
@param aDstMode	Display mode of destination
@param aMsk		Mask buffer. Must be in EGray2 format
@param aWidth	Number of pixels to copy
@param aInvert	If ETrue, copy source where mask bit is clear, else copy where
	it is set.
*/
void CTLowLevel::MaskedBlendBuffer(TAny* aSrc, TDisplayMode aSrcMode, TAny* aDst,
		TDisplayMode aDstMode, TUint32* aMsk, TInt aWidth, TBool aInvert)
	{
	TColorConvertor& srcConvertor = ColorConvertor(aSrcMode);
	TColorConvertor& dstConvertor = ColorConvertor(aDstMode);

	for (TInt xx = 0; xx < aWidth; xx++)
		{
		TUint32 pixelMask = ExtractBinaryValue(xx, aMsk, EGray2);
		if (aInvert)
			{
			pixelMask = !pixelMask;
			}

		if (pixelMask)
			{
			TUint32 dst32 = ExtractBinaryValue(xx, (TUint32*)aDst, aDstMode);
			TUint32 src32 = ExtractBinaryValue(xx, (TUint32*)aSrc, aSrcMode);

			switch (aSrcMode)
				{
				// Only blend for source modes with alpha 
				case EColor16MAP:
				case EColor16MA:
					{
					// Convert source and destination pixel values to 16MAP
					if (aDstMode != EColor16MAP)
						{
						dst32 = dstConvertor.Color(dst32).Color16MAP();
						}
					if (aSrcMode != EColor16MAP)
						{
						src32 = srcConvertor.Color(src32).Color16MAP();
						}
					// Both params must be 16MAP, output is likewise
					dst32 = PMAPixelBlend(dst32, src32);
					// Convert 16MAP to final format
					dst32 = dstConvertor.Index(TRgb::Color16MAP(dst32));
					}
					break;
				// Anything else is a copy (with format conversion) 
				default:
					dst32 = dstConvertor.Index(srcConvertor.Color(src32));
					break;
				}

			WriteBinaryValue(xx, aDst, aDstMode, dst32);
			}
		}
	}

/**
Returns the minimum number of bytes to hold the given number of pixels,
rounded up to the next word boundary.
@param aPixels	Number of pixels
@param aMode	Display mode of pixels
@return	Number of bytes to hold pixels
*/
TUint32 CTLowLevel::BytesForPixels(TUint32 aPixels, TDisplayMode aMode)
	{
	TUint32 bytes = LongWidth(aPixels, aMode);

	switch (aMode)
		{
		case EGray2:
			bytes /= 8;
			break;
		case EGray4:
			bytes /= 4;
			break;
		case EGray16:
		case EColor16:
			bytes /= 2;
			break;
		case EColor4K:
		case EColor64K:
			bytes *= 2;
			break;
		case EColor16M:
			bytes *= 3;
			break;
		case EColor16MU:
		case EColor16MA:
		case EColor16MAP:
			bytes *= 4;
			break;
		}
	return bytes;
	}

/**
Compare the actual blend results with the expected ones, allowing for some
blending errors. Both buffers must be in aMode format
@param aActual		Start of actual results
@param aExpected	Start of expected results
@param aWidth		Number of pixels to check
@param aMode		Display mode of the pixels in the buffers
@param aBlended		ETrue if pixels were blended, EFalse if they were opaque
@return	ETrue if buffers compared OK, EFalse if there was a mismatch.
*/
TBool CTLowLevel::CompareBlendMaskResults(TAny* aActual, TAny* aExpected, 
		TUint32 aWidth, TDisplayMode aMode, TBool aBlended, TDisplayMode aSrcMode)
	{
	if (aBlended)
		{
		// There can be blending rounding errors, so allow for these. In general
		// allow for one bit of error, taking into account the precision of
		// each component.
		TInt maxRBErr;
		TInt maxGErr;
		switch (aMode)
			{
			case EColor4K:
				// All components are four bits
				maxRBErr = maxGErr = 17;
				break;
			case EColor64K:
				// Six bits for green, five for the others, so error
				// varies.
				maxRBErr = 9;
				maxGErr = 5;
				break;
			default:
				// For 16MAP, it will be dependent on the alpha value.
				maxRBErr = maxGErr = 1;
				break;
			}

		// Compare each pixel, allowing for the error in each component.
		for (TInt xx = 0; xx < aWidth; xx++)
			{
			TRgb exp = ExtractRgbValue(xx, (TUint8*)aExpected, aMode);
			TRgb act = ExtractRgbValue(xx, (TUint8*)aActual, aMode);
			
			if (aMode == EColor16MAP)
				{
				// Take into account that components have been premultiplied
				TInt alpha = exp.Alpha();
				if (alpha > 0)
					{
					maxRBErr = maxGErr = (0xFF + alpha - 1) / alpha;
					}
				if (aSrcMode == EColor16MA && (maxGErr < 3 || maxRBErr < 3))
					{
					maxGErr = 3;
					maxRBErr = 3;
					}
				}
			
			if (AbsDiff(exp.Red(), act.Red()) > maxRBErr ||
					AbsDiff(exp.Green(), act.Green()) > maxGErr ||
					AbsDiff(exp.Blue(), act.Blue()) > maxRBErr ||
					exp.Alpha() != act.Alpha())
				{
				INFO_PRINTF4(_L("At %d, expected 0x%08.8x, got 0x%08.8x"), 
						xx, exp.Internal(), act.Internal());
				return EFalse;
				}
			}
		return ETrue;
		}
	else
		{
		// For non-alpha sources there is no blending, so results
		// should be exact.
		TUint32 stride = BytesForPixels(aWidth, aMode);
		return (Mem::Compare((TUint8*)aActual, stride, (TUint8*)aExpected, stride) == 0);
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
	thisThis->iExitHow=thisThis->ERunning;
	TInt returnErr = KErrNone;
	TRAPD(leaveErr,returnErr=thisThis->ThreadFunctionL());
	if (leaveErr)
		{
		thisThis->iExitHow=ELeave;
		thisThis->iExitCode=leaveErr;
		return leaveErr;
		}
	else
		{
		thisThis->iExitHow=EReturn;
		thisThis->iExitCode=returnErr;
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
			return KErrGeneral; 
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

void CTLowLevel::ClosePanicDialogs()
	{
	RWsSession session;
	TInt err = session.Connect();
	TEST(err == KErrNone);
	TInt idFocus = session.GetFocusWindowGroup();
	TWsEvent event;
	event.SetType(EEventKey); //EEventKeyDown
	TKeyEvent* keyEvent = event.Key();
	keyEvent->iCode = EKeyEscape;
	keyEvent->iScanCode = EStdKeyEscape;
	keyEvent->iModifiers = 0;
	TInt theLimit = 50;
	while(idFocus != NULL && (theLimit-- > 0))
		{
		session.SendEventToAllWindowGroups(event);
		TInt idNewFocus = session.GetFocusWindowGroup();
		idFocus=idNewFocus;
		}
	session.Flush();
	session.Close();
	}


/**
@SYMTestCaseID GRAPHICS-SCREENDRIVER-0003
@SYMTestCaseDesc Test that FastBlendBitmapMasked masking works correctly
@SYMDEF INC120146 PDEF120693 PDEF120680 INC120742 PDEF121725
@SYMTestPriority High
@SYMTestActions Test actions are:
	Create pseudorandom initial contents, source and mask lines.
	Set a line of the screen to the initial contents.
	Use FastBlendBitmapMasked, with and without inverted bitmask, to write
		source to target.
	Read line contents back.
	Check each pixel is either the initial value, or the source or a blend,
		depending on corresponding mask pixel.
	Repeat this whole sequence with supported source display modes.
	NOTE - wrapped in memory check for edge case defect INC120742
@SYMTestExpectedResults The correct pixel values are set, based on the mask.
*/
void CTLowLevel::TestFastBlendBitmapMasked(const TInt aRetry)
	{
	const TInt KRetryAmount = 10;

	TAny* interface = NULL;
	TInt err = iDrawDevice->GetInterface(KFastBlendInterfaceID, interface);
	if(err == KErrNone)
		{
		if (aRetry == 0)
			{
			INFO_PRINTF1(_L("START ---->TestFastBlendBitmapMasked"));
			}

		MFastBlend* fastBlend = reinterpret_cast<MFastBlend*>(interface);
		TInt yPos = iSize.iHeight / 2;
		TPoint pt(0,yPos);
		TSize lineSize(iSize.iWidth, 1);
		TRect lineRect(lineSize);

		TInt destStride = iDrawDevice->ScanLineBytes();
 		TInt maskStride = (iSize.iWidth + 7) / 8;
		TInt maskPages = (maskStride/4096+1);
		maskPages+=20;
		// Allocate large enough buffers for all modes
		TAny* source = User::Alloc(iSize.iWidth * 4);
		TAny* expected = User::Alloc(destStride);
		CFbsBitmap testBitmap;
		testBitmap.Create(TSize(1024,maskPages),EColor16MA);
		TUint32* mask = testBitmap.DataAddress()+1024-(maskStride%4096)/4;
		TUint32* slb = iDrawDevice->ScanLineBuffer();

		Check(source != NULL && expected != NULL && mask != NULL);

		TUint32* bitmapData=testBitmap.DataAddress();
		SEpocBitmapHeader header1 = testBitmap.Header();
		TUint32* pastBitmapData=bitmapData+((header1.iBitmapSize-header1.iStructSize)>>2);
		mask = pastBitmapData- (maskStride+3)/4;
	
		TBool canGenerateDefectScenario = EFalse;
		TTestMemThread memThread(pastBitmapData-2,pastBitmapData+2);
		TInt failAt=memThread.FailOffset();
		if (failAt<0)
			{
			INFO_PRINTF2(_L("Error generated by test thread! %i - BAD"),failAt);
			}
		else if (failAt<=1)
			{
			INFO_PRINTF1(_L("Bitmap memory is inaccessable - BAD"));
			}
		else if (failAt>2)
			{
			INFO_PRINTF1(_L("Memory after bitmap is accessable - BAD"));
			}
		else
			{
			INFO_PRINTF1(_L("Memory after bitmap is inaccessable - GOOD"));
			canGenerateDefectScenario=ETrue;
			}
		ClosePanicDialogs();

		if (!canGenerateDefectScenario)
			{
			//if this doesn't work out then the memory allocator is not like it was when the defect was raised!
			//so the test is no longer any good.
			if (aRetry == KRetryAmount)
				{
				INFO_PRINTF2(_L("Failed %d times - Overall test failure"),KRetryAmount);
				Check(EFalse);
				}
			else
				{
				INFO_PRINTF2(_L("RETRYING - attempt %d"),aRetry+2);
				TestFastBlendBitmapMasked(aRetry+1);
				}
			}
		else
			{
			INFO_PRINTF1(_L("Performing test"));
			for (TInt sourceModeIndex = 0; sourceModeIndex < KNumberDisplayModes1; sourceModeIndex++)
			 	{
			 	for (TInt invert = 0; invert < 2; invert++)
			 		{
			 		TDisplayMode sourceMode = TestDisplayMode1[sourceModeIndex];
			 		TInt sourceStride = BytesForPixels(iSize.iWidth, sourceMode);
			 		TBool blended = (sourceMode == EColor16MA || sourceMode == EColor16MAP);
			 		
			 		// Initialise (randomise) the buffers for this iteration
			 		FillBuffer((TUint8*)source, sourceStride, sourceMode, ETrue);
			 		FillBuffer((TUint8*)expected, destStride, iDispMode, ETrue);
			 		FillBuffer((TUint8*)mask, maskStride, EGray2, ETrue);
			 		
			 		// Write out the initial contents
			 		iDrawDevice->WriteLine(0, yPos, iSize.iWidth, (TUint32*)expected,
			 				CGraphicsContext::EDrawModeWriteAlpha);
		
			 		// Fast, masked overwrite, with or without mask inversion
			 		TInt result = fastBlend->FastBlendBitmapMasked(pt, 
			 				(TUint32*)source, sourceStride, lineSize, lineRect,
			 				sourceMode, mask, maskStride, EGray2, lineSize,
			 				TPoint(), invert, CGraphicsContext::EDrawModePEN, 
			 				CFbsDrawDevice::ENoShadow);

			 		if (result == KErrNotSupported)
			 			{
			 			// Unsupported combination of parameters, move on.
			 			continue;
			 			}
		
			 		// Use mask to blend source with 'expected' to get the expected
			 		// output
			 		MaskedBlendBuffer(source, sourceMode, expected, iDispMode, 
			 				mask, iSize.iWidth, invert);

			 		// Read back actual output and compare with expected.
			 		iDrawDevice->ReadLine(0, yPos, iSize.iWidth, slb, iDispMode);

			 		if (!CompareBlendMaskResults(slb, expected, iSize.iWidth,
			 				iDispMode, blended, sourceMode))
			 			{
			 			INFO_PRINTF3(_L("Output mismatch: source mode %S, invert=%d"), 
			 					&DisplayModeNames1[sourceModeIndex], invert);
			 			// Report other details, plus fail the overall test.
			 			Check(EFalse);
			 			}
			 		iIteration++;
			 		}

			 	}
			TTestMemThread memThreadAfter(pastBitmapData-2,pastBitmapData+2);
			failAt=memThreadAfter.FailOffset();
			if (failAt != 2)
				{
				INFO_PRINTF1(_L("Memory after bitmap is accessable - BAD"));
				if (aRetry >= KRetryAmount)
					{
					INFO_PRINTF2(_L("Failed %d times - Overall test failure"),KRetryAmount);
					Check(EFalse);
					}
				else
					{
					INFO_PRINTF2(_L("RETRYING - attempt %d"),aRetry+2);
					TestFastBlendBitmapMasked(aRetry+1);
					}
				}
			else
				{
				INFO_PRINTF1(_L("Memory after bitmap is inaccessable - GOOD"));
				}
			ClosePanicDialogs();
			}
		if (aRetry == 0)
			{
			INFO_PRINTF1(_L("END ---->TestFastBlendBitmapMasked"));
			Report();
			}

		User::Free(source);
		User::Free(expected);
		testBitmap.Reset();
		}
	}

void CTLowLevel::TestWriteRGBAlpha()
	{
	TUint8* writeBuffer = new TUint8[iSize.iWidth * 4];
	TUint8* writeBuffer2 = new TUint8[iSize.iWidth * 4];
	TUint8* maskBuffer = new TUint8[iSize.iWidth];
	Check(writeBuffer != NULL);
	Check(maskBuffer != NULL);

	TInt nRect;

	//special test for EColor16MAP
	if (iDispMode == EColor16MAP)
		{
		//mask fill has vaues 0x00, 0xff, and 0x3A.  The mask is used for blending
		for (TInt maskFillCount=0; maskFillCount<KMaskFill;maskFillCount++)
			{
			for (TInt userDispMode=0; userDispMode< KUserDispModes; userDispMode++)
				{
				for (TInt shadowMode = 0; shadowMode < KNumShadowModes; shadowMode++)
					{
					for (nRect = 0; nRect < KNumTestRects; nRect++)
						{
						for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
							{
							for (TInt nColor = 0; nColor < KNumTestColors; nColor++)
								{
								iDrawDevice->SetUserDisplayMode(UserDisplayModes[userDispMode]);
								iUserDispMode = UserDisplayModes[userDispMode];
								if (iUserDispMode == EColor16MA)
									i16MAUserDispMode = ETrue;
								else
									i16MAUserDispMode = EFalse;

								TRgb col = TestColor[nColor];
								TUint32 internal = col.Internal();
								TUint32 *writeBuf32 = (TUint32*)writeBuffer;
								TInt cnt;
								//fill the buffer with colour
								for (cnt=0;cnt<iSize.iWidth;cnt++)
									{
									*writeBuf32=internal;
									writeBuf32++;
									}

								Mem::Fill(maskBuffer,iSize.iWidth,MaskFill[maskFillCount]);

								TRgb bakCol = TestBackground[nBackColor];
								Clear(bakCol);
								TRect rect = TestRect[nRect];

								iDrawDevice->CFbsDrawDevice::SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
								iDrawDevice->CFbsDrawDevice::SetFadingParameters(100,200);
								iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));

								TInt yy;
								for (yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
									{
									iDrawDevice->WriteRgbAlphaLine(rect.iTl.iX,yy,rect.Width(),writeBuffer,maskBuffer,CGraphicsContext::EDrawModePEN);
									iIteration++;
									}

								//ensure the colour has the 0xff mask value
								TRgb checkColor;
								//use a combined alpha for the check colour
								TUint32 combinedAlpha = MaskFill[maskFillCount]*((internal&0xff000000)>>24);
								combinedAlpha = ((combinedAlpha <<16) & 0xff000000);
								checkColor.SetInternal((internal&0x00ffffff)|combinedAlpha);
								//check colour is not a PMA colour, but has alpha
								CheckRgb(rect,checkColor,bakCol,CGraphicsContext::EDrawModePEN,CFbsDrawDevice::TShadowMode(shadowMode));
								CheckBackground(rect,bakCol);

								//the other WriteRgbAlpha line uses the other shadow mode
								TUint32 *writeBuf2 = (TUint32*)writeBuffer2;
								TUint32 buf2val= NonPMA2PMAPixel(bakCol.Internal());
								//fill the buffer with colour
								for (cnt=0;cnt<iSize.iWidth;cnt++)
									{
									*writeBuf2=buf2val;
									writeBuf2++;
									}

								iDrawDevice->CFbsDrawDevice::SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
								iDrawDevice->CFbsDrawDevice::SetFadingParameters(100,200);
								iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));

								for (yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
									{
									iDrawDevice->WriteRgbAlphaLine(rect.iTl.iX,yy,rect.Width(),
										writeBuffer,
										writeBuffer2,
										maskBuffer,
										CGraphicsContext::EDrawModeWriteAlpha);
									iIteration++;
									}
								//require to Shadow After the checkColor, no shadow with a zero mask
								TBool shadowModeChanged = EFalse;
								if (MaskFill[maskFillCount])
									{
									iPostBlendShadow = (TPostShadowMode) shadowMode;
									shadowMode = 0;
									shadowModeChanged = ETrue;
									}
								CheckRgb(rect,checkColor,bakCol,CGraphicsContext::EDrawModePEN,shadowMode);
								if(shadowModeChanged) shadowMode = iPostBlendShadow;
								iPostBlendShadow = ENoPostShadow;
								CheckBackground(rect,bakCol);

								Clear(bakCol);

								iDrawDevice->CFbsDrawDevice::SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
								iDrawDevice->CFbsDrawDevice::SetFadingParameters(100,200);
								iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));

								for (TInt yy2 = rect.iTl.iY; yy2 < rect.iBr.iY; yy2++)
									{
									iDrawDevice->WriteRgbAlphaMulti(rect.iTl.iX,yy2,rect.Width(),col,maskBuffer);
									iIteration++;
									}

								CheckRgb(rect,checkColor,bakCol,CGraphicsContext::EDrawModePEN,CFbsDrawDevice::TShadowMode(shadowMode));
								CheckBackground(rect,bakCol);
								}
							}
						}
					}
				}
			}
		}

	Report();
	iDrawDevice->SetUserDisplayMode(iDispMode);
	i16MAUserDispMode = EFalse;
	iUserDispMode = ENone;
	Mem::Fill(writeBuffer,iSize.iWidth * 4,0xff);
	Mem::Fill(maskBuffer,iSize.iWidth,0xff);

	for (nRect = 0; nRect < KNumTestRects; nRect++)
		{
		for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
			{
			if ((nBackColor>=KMaxNon16BackColours) && (iDispMode!= EColor16MAP))
				continue;

			TRgb bakCol = TestBackground[nBackColor];
			Clear(bakCol);
			TRect rect = TestRect[nRect];

			for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
				{
				iDrawDevice->WriteRgbAlphaLine(rect.iTl.iX,yy,rect.Width(),writeBuffer,maskBuffer,CGraphicsContext::EDrawModePEN);
				iIteration++;
				}

			CheckRgb(rect,KRgbWhite,bakCol,CGraphicsContext::EDrawModePEN,0);
			CheckBackground(rect,bakCol);

			Clear(bakCol);

			for (TInt yy2 = rect.iTl.iY; yy2 < rect.iBr.iY; yy2++)
				{
				iDrawDevice->WriteRgbAlphaMulti(rect.iTl.iX,yy2,rect.Width(),KRgbWhite,maskBuffer);
				iIteration++;
				}

			CheckRgb(rect,KRgbWhite,bakCol,CGraphicsContext::EDrawModePEN,0);
			CheckBackground(rect,bakCol);

			}
		}
	Report();

	Mem::FillZ(writeBuffer,iSize.iWidth * 3);

	for (nRect = 0; nRect < KNumTestRects; nRect++)
		{
		for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
			{
			if ((nBackColor>=KMaxNon16BackColours) && (iDispMode!= EColor16MAP))
				continue;

			//need to fill with 0xff alpha, so blending takes place
			if (iDispMode== EColor16MAP)
				{
				const TUint32 black = 0xff000000;
				TUint32 *writeBuf32 = (TUint32*) writeBuffer;
				for (TInt cnt=0;cnt<iSize.iWidth;cnt++)
					{
					*writeBuf32=black;
					writeBuf32++;
					}
				}

			TRgb bakCol = TestBackground[nBackColor];
			Clear(bakCol);
			TRect rect = TestRect[nRect];

			for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
				{
				iDrawDevice->WriteRgbAlphaLine(rect.iTl.iX,yy,rect.Width(),writeBuffer,maskBuffer,CGraphicsContext::EDrawModePEN);
				iIteration++;
				}

			CheckRgb(rect,KRgbBlack,bakCol,CGraphicsContext::EDrawModePEN,0);
			CheckBackground(rect,bakCol);

			Clear(bakCol);

			for (TInt yy2 = rect.iTl.iY; yy2 < rect.iBr.iY; yy2++)
				{
				iDrawDevice->WriteRgbAlphaMulti(rect.iTl.iX,yy2,rect.Width(),KRgbBlack,maskBuffer);
				iIteration++;
				}

			CheckRgb(rect,KRgbBlack,bakCol,CGraphicsContext::EDrawModePEN,0);
			CheckBackground(rect,bakCol);
			}
		}
	Report();

	Mem::FillZ(maskBuffer,iSize.iWidth);

	for (nRect = 0; nRect < KNumTestRects; nRect++)
		{
		for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
			{
			if ((nBackColor>=KMaxNon16BackColours) && (iDispMode!= EColor16MAP))
				continue;

			TRgb bakCol = TestBackground[nBackColor];
			Clear(bakCol);
			TRect rect = TestRect[nRect];

			for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
				{
				iDrawDevice->WriteRgbAlphaLine(rect.iTl.iX,yy,rect.Width(),writeBuffer,maskBuffer,CGraphicsContext::EDrawModePEN);
				iIteration++;
				}

			TRgb checkColor2=bakCol;
			if (iDispMode == EColor16MAP)
				checkColor2.SetInternal (0);

			CheckRgb(rect,checkColor2,bakCol,CGraphicsContext::EDrawModePEN,0);
			CheckBackground(rect,bakCol);

			Clear(bakCol);

			for (TInt yy2 = rect.iTl.iY; yy2 < rect.iBr.iY; yy2++)
				{
				iDrawDevice->WriteRgbAlphaMulti(rect.iTl.iX,yy2,rect.Width(),KRgbBlack,maskBuffer);
				iIteration++;
				}

			CheckRgb(rect,checkColor2,bakCol,CGraphicsContext::EDrawModePEN,0);
			CheckBackground(rect,bakCol);

			}
		}
	Report();

	Mem::Fill(writeBuffer,iSize.iWidth * 3,0xff);

	for (nRect = 0; nRect < KNumTestRects; nRect++)
		{
		for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
			{
			if ((nBackColor>=KMaxNon16BackColours) && (iDispMode!= EColor16MAP))
				continue;

			TRgb bakCol = TestBackground[nBackColor];
			Clear(bakCol);
			TRect rect = TestRect[nRect];

			for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
				{
				iDrawDevice->WriteRgbAlphaLine(rect.iTl.iX,yy,rect.Width(),writeBuffer,maskBuffer,CGraphicsContext::EDrawModePEN);
				iIteration++;
				}
			TRgb checkColor3=bakCol;
			if (iDispMode == EColor16MAP)
				checkColor3.SetInternal (0xffffff);

			CheckRgb(rect,checkColor3,bakCol,CGraphicsContext::EDrawModePEN,0);
			CheckBackground(rect,bakCol);

			Clear(bakCol);

			for (TInt yy2 = rect.iTl.iY; yy2 < rect.iBr.iY; yy2++)
				{
				iDrawDevice->WriteRgbAlphaMulti(rect.iTl.iX,yy2,rect.Width(),KRgbWhite,maskBuffer);
				iIteration++;
				}

			CheckRgb(rect,checkColor3,bakCol,CGraphicsContext::EDrawModePEN,0);
			CheckBackground(rect,bakCol);
			}
		}
	Report();

	//Extra test for DEF082251
	if (iDispMode==EColor16MU)
		{
		Mem::Fill(maskBuffer,iSize.iWidth,0x7f);

		for (nRect = 0; nRect < KNumTestRects; nRect++)
			{
			for (TInt nBackColor = 0; nBackColor < KNumTestBackgrounds; nBackColor++)
				{
				TRgb bakCol = TestBackground[nBackColor];
				Clear(bakCol);
				TRect rect=TestRect[nRect];

				const TInt red = 50;
				const TInt green = 60;
				const TInt blue = 70;
				const TInt alpha = 80;

				for (TInt yy2 = rect.iTl.iY; yy2 < rect.iBr.iY; yy2++)
					{
					iDrawDevice->WriteRgbAlphaMulti(rect.iTl.iX,yy2,rect.Width(),
					TRgb(red,green,blue,alpha),maskBuffer);
					iIteration++;
					}

				//work out the color - based on OptimizedBlend32 in the
				//screendriver bmdraw32.cpp
				TInt combinedAlpha = (alpha * 0x7f)>>8;

				const TUint32 alphaValue = (combinedAlpha << 8) + combinedAlpha;
 				TUint32 secondary= bakCol.Value();

 				const TInt r2 = secondary & 0x000000ff;
 				const TInt g2 = (secondary & 0x0000ff00) >> 8;
 				const TInt b2 = (secondary & 0x00ff0000) >> 16;

 				const TInt r3 = ((alphaValue * (red   - r2)) >> 16) + r2;
				const TInt g3 = ((alphaValue * (green - g2)) >> 16) + g2;
				const TInt b3 = ((alphaValue * (blue  - b2)) >> 16) + b2;

				TInt result= (b3 & 0xFF) | ((g3<<8) & 0xFF00) | ((r3<<16) & 0xFF0000) | 0xFF000000;
				TRgb resultColor = TRgb(result,0);

				CheckRgb(rect,resultColor,bakCol,CGraphicsContext::EDrawModePEN,0);
				CheckBackground(rect,bakCol);
				}
			}
			Report();
		}
	delete [] writeBuffer;
	delete [] maskBuffer;
	delete [] writeBuffer2;
	}

void CTLowLevel::TestShadow()
	{
	for (TInt shadowMode = 0; shadowMode < KNumShadowModes; shadowMode++)
		{
		for (TInt nRect = 0; nRect < KNumTestRects; nRect++)
			{
			for (TInt nColor = 0; nColor < KNumTestColors; nColor++)
				{
				if ((nColor>=KMaxNon16Colours) && (iDispMode!= EColor16MAP))
					continue;

				TRgb col = TestColor[nColor];
				Clear(col);

				TRect rect = TestRect[nRect];

				iDrawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(shadowMode));
				iDrawDevice->ShadowArea(rect);

				CheckShadowRgb(rect,col,shadowMode);

				TRgb checkColor=col;
				if (iDispMode == EColor16MAP)
					{
					checkColor.SetInternal (0); //only want contribution from one colour
					}

				TRect outside(0,0,iSize.iWidth,rect.iTl.iY);
				if (!outside.IsEmpty())
					CheckRgb(outside,checkColor,col,CGraphicsContext::EDrawModePEN,0);
				outside.SetRect(0,rect.iBr.iY,iSize.iWidth,iSize.iHeight);
				if (!outside.IsEmpty())
					CheckRgb(outside,checkColor,col,CGraphicsContext::EDrawModePEN,0);
				outside.SetRect(0,rect.iTl.iY,rect.iTl.iX,rect.iBr.iY);
				if (!outside.IsEmpty())
					CheckRgb(outside,checkColor,col,CGraphicsContext::EDrawModePEN,0);
				outside.SetRect(rect.iBr.iX,rect.iTl.iY,iSize.iWidth,rect.iBr.iY);
				if (!outside.IsEmpty())
					CheckRgb(outside,checkColor,col,CGraphicsContext::EDrawModePEN,0);
				iIteration++;
				}
			}
		Report();
		}
	}

/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0002

	@SYMPREQ PREQ1543

	@SYMTestCaseDesc This test code tests WriteRgbOutlineAndShadow() which blends the four colours.
	Colours used for blending are outline, shadow, foreground and background.

	@SYMTestPriority High

	@SYMTestStatus Implemented

	@SYMTestActions It compares the colour written by WriteRgbOutlineAndShadow() with colour
	calculated using lookup table provided by Monotype.
	API Calls:
	CDrawBitmap::GetInterface()
	MOutlineAndShadowBlend::WriteRgbOutlineAndShadow()
	CDrawBitmap::ReadLine()

	@SYMTestExpectedResults Test should pass and colour written by WriteRgbOutlineAndShadow should
	match with the colour calculated through lookup table.
*/
void CTLowLevel::TestWriteRgbOutlineAndShadow()
	{
	MOutlineAndShadowBlend* outlineAndShadow = NULL;
	TInt byteSize = ByteSize();
	TUint8* writeBuffer = new TUint8[iSize.iWidth * sizeof(TRgb)];
	TUint8* readBuffer = new TUint8[iSize.iWidth * sizeof(TRgb)];
	TColorConvertor& colorConvertor = ColorConvertor(iDispMode);
	Check(writeBuffer != NULL);
	Check(readBuffer != NULL);
	TInt err = iDrawDevice->GetInterface(KOutlineAndShadowInterfaceID, reinterpret_cast <TAny*&> (outlineAndShadow));
	Check(err == KErrNone);

	TRect rect = TestRect[0];
	for (TInt nBlendingColors = 0; nBlendingColors < KNumBlendingColors; nBlendingColors++)
		{
		// Select different combinations of colours for testing from the array
		TRgb outlinePenColor = TestOSFBColorsForBlending[nBlendingColors][0];
		TRgb shadowColor = TestOSFBColorsForBlending[nBlendingColors][1];
		TRgb fillColor = TestOSFBColorsForBlending[nBlendingColors][2];
		TRgb backgroundColor = colorConvertor.Color(colorConvertor.Index(TestOSFBColorsForBlending[nBlendingColors][3]));
		Clear(backgroundColor);
		for (TInt yy = rect.iTl.iY; yy < rect.iBr.iY; yy++)
			{
			// Run the test for values from 0 to 255 so that it will cover all the entries
			// of lookup table provided by Monotype
			for (TInt index = 0; index < 256; index++)
				{
				// In case alpha is supported and if alpha value is less than 255 then the colour drawn will be different
				// than the colour specified as it blends with the existing background pixel colour.
				TRgb backgroundColorDrawn= iDrawDevice->ReadPixel(rect.iTl.iX, yy);

				// Fill zeroes in the readBuffer so that we can make sure that there is no garbage value.
				Mem::FillZ(readBuffer, iSize.iWidth * 3);

				// We are writing index in writeBuffer to simulate the buffer provided by rasterizer
				// This index should be a value between 0-255, It would be corresponding the entries of lookup table
				Mem::Fill(writeBuffer, iSize.iWidth * 3, index);

				// Below function blends outline, shadow, foreground and background colours, and writes aLength pixels with new colour
				// starting from aX, aY.
				err = outlineAndShadow->WriteRgbOutlineAndShadow(rect.iTl.iX, yy , rect.Width(), outlinePenColor.Internal(),
																					shadowColor.Internal(),fillColor.Internal(),writeBuffer);
				Check(err == KErrNone);

				// Read the whole line which has been written by WriteRgbOutlineAndShadow()
				iDrawDevice->ReadLine(rect.iTl.iX, yy, rect.Width(), (TUint32*)readBuffer, iDispMode);

				// Check colour of each pixel, it should be same as the colour which is calulated manually in CheckBlendedOutlineAndShadow()
				TBool result = CheckBlendedOutlineAndShadow(outlinePenColor, shadowColor, fillColor, backgroundColorDrawn, index, rect.Width(), readBuffer);
				Check(result);
				if (!result)
					{
					Report();
					delete [] writeBuffer;
					delete [] readBuffer;
					return;
					}

				iIteration++;
				}
			}
		Report();
		}
	delete [] writeBuffer;
	delete [] readBuffer;
	}

inline TInt CTLowLevel::ByteSize()
	{
	return ::ByteSize(iDispMode,iSize.iWidth);
	}

TInt CTLowLevel::LongWidth(TInt aWidth,TDisplayMode aDispMode)
	{
	switch (aDispMode)
		{
	case EGray2:
		return (aWidth + 31) & ~31;
	case EGray4:
		return (aWidth + 15) & ~15;
	case EGray16:
	case EColor16:
		return (aWidth + 7) & ~7;
	case EGray256:
	case EColor256:
		return (aWidth + 3) & ~3;
	case EColor4K:
	case EColor64K:
		return (aWidth + 1) & ~1;
	case EColor16M:
		return (((aWidth * 3) + 11) / 12) * 4;
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		return aWidth;
	default:
		break;
		};
	return 0;
	}

void CTLowLevel::Clear(TRgb aColor)
	{
	iDrawDevice->SetShadowMode(CFbsDrawDevice::ENoShadow);
	iDrawDevice->WriteRgbMulti(0,0,iSize.iWidth,iSize.iHeight,aColor,CGraphicsContext::EDrawModeWriteAlpha);
	}

/*
Fill aBuffer with aByteSize random bytes in such a way that the result is a
valid scan line buffer for a driver of which the display mode is aDispMode.
If aDispMode is EColor16MU, the alpha bytes will be 0xFF if aNoAlpha16MU is ETrue,
or random bytes if aNoAlpha16MU is EFalse.
*/
void CTLowLevel::FillBuffer(TUint8* aBuffer, TInt aByteSize, TDisplayMode aDispMode, TBool aNoAlpha16MU)
	{
	TUint8* bufferLimit = aBuffer + aByteSize;
	TUint8* buffer=aBuffer;

	TInt64 seed = TInt64(TInt(aBuffer) * aByteSize * aDispMode * User::TickCount());

	if (aDispMode != EColor16MU || !aNoAlpha16MU)
		{
		while (aBuffer < bufferLimit)
			{
			*aBuffer++ = (TUint8)Math::Rand(seed);
			}
		}
	else
		{
		while (aBuffer < bufferLimit)
			{
			if (TInt(aBuffer) & 3 == 3)
				*aBuffer++ = 0xFF;
			else
				*aBuffer++ = (TUint8)Math::Rand(seed);
			}
		}
	if (aDispMode == EColor16MU && !aNoAlpha16MU || aDispMode == EColor16MAP)
		{
		//need to do the premultiply alpha to ensure that all the colours are valid
		//in the colour space
		for (;buffer < (bufferLimit-3);buffer+=4)
			{
			TUint alpha=*(buffer+3);
			*(buffer)=((*(buffer))* alpha)/255; //do a pre multiply alpha operation
			*(buffer+1)=((*(buffer+1))* alpha)/255;
			*(buffer+2)=((*(buffer+2))* alpha)/255;
			}
		}

	}

void CTLowLevel::CheckBuffer(TUint8* aWriteBuffer,TDisplayMode aWriteDispMode,TUint8* aReadBuffer,TDisplayMode aReadDispMode,TInt aPixelLength)
	{
	switch (aWriteDispMode)
		{
	case EGray2:
		CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray2);
		break;
	case EGray4:
		if (aReadDispMode == EGray2)
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray2);
		else
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray4);
		break;
	case EGray16:
		if (aReadDispMode == EGray2)
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray2);
		else if (aReadDispMode == EGray4)
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray4);
		else if (aReadDispMode == EColor16)
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor16);
		else
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray16);
		break;
	case EGray256:
		switch (aReadDispMode)
			{
		case EGray2:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray2);
			break;
		case EGray4:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray4);
			break;
		case EGray16:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray16);
			break;
		case EGray256:
		case EColor16M:
		case ERgb:
		case EColor16MU:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray256);
			break;
		case EColor16:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor16);
			break;
		case EColor256:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor256);
			break;
		case EColor4K:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor4K);
			break;
		case EColor64K:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor64K);
			break;
		default:
			break;
			}
		break;
	case EColor16:
		switch (aReadDispMode)
			{
		case EGray2:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray2);
			break;
		case EGray4:
		case EGray16:
		case EGray256:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray4);
			break;
		case EColor16:
		case EColor256:
		case EColor4K:
		case EColor64K:
		case EColor16M:
		case ERgb:
		case EColor16MU:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor16);
			break;
		default:
			break;
			}
		break;
	case EColor256:
		switch (aReadDispMode)
			{
		case EGray2:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray2);
			break;
		case EGray4:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray4);
			break;
		case EGray16:
		case EGray256:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray16);
			break;
		case EColor16:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor16);
			break;
		case EColor256:
		case EColor4K:
		case EColor64K:
		case EColor16M:
		case ERgb:
		case EColor16MU:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor256);
			break;
		default:
			break;
			}
		break;
	case EColor4K:
		switch (aReadDispMode)
			{
		case EGray2:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray2);
			break;
		case EGray4:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray4);
			break;
		case EGray16:
		case EGray256:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray16);
			break;
		case EColor16:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor16);
			break;
		case EColor256:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor256);
			break;
		case EColor4K:
		case EColor64K:
		case EColor16M:
		case ERgb:
		case EColor16MU:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor4K);
			break;
		default:
			break;
			}
		break;
	case EColor64K:
		switch (aReadDispMode)
			{
		case EGray2:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray2);
			break;
		case EGray4:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray4);
			break;
		case EGray16:
		case EGray256:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EGray16);
			break;
		case EColor16:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor16);
			break;
		case EColor256:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor256);
			break;
		case EColor4K:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor4K);
			break;
		case EColor64K:
		case EColor16M:
		case ERgb:
		case EColor16MU:
			CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,EColor64K);
			break;
		default:
			break;
			}
		break;
	case EColor16M:
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		CheckPixel(aWriteBuffer,aWriteDispMode,aReadBuffer,aReadDispMode,aPixelLength,aReadDispMode);
		break;
	default:
		break;
		};
	}

void CTLowLevel::CheckPixel(TUint8* aWriteBuffer,TDisplayMode aWriteDispMode,TUint8* aReadBuffer,TDisplayMode aReadDispMode,TInt aPixelLength,TDisplayMode aCompareDispMode)
	{
	TColorConvertor& colorConvertor = ColorConvertor(aCompareDispMode);

	for (TInt count = 0; count < aPixelLength; count++)
		{
		TRgb writeValue = ExtractRgbValue(count,aWriteBuffer,aWriteDispMode);
		TRgb readValue = ExtractRgbValue(count,aReadBuffer,aReadDispMode);
		CheckMatch(colorConvertor.Index(writeValue),colorConvertor.Index(readValue));
		}
	}

void CTLowLevel::CheckRgb(const TPoint& aPoint,TRgb aColor,TRgb aBackgroundColor,CGraphicsContext::TDrawMode aDrawMode,TInt aShadowMode)
	{
	TRect rect(aPoint,TSize(1,1));
	iDrawDevice->CFbsDrawDevice::SetDitherOrigin(aPoint);
	CheckRgb(rect,aColor,aBackgroundColor,aDrawMode,aShadowMode);
	}

void CTLowLevel::CheckRgb(const TRect& aRect,TRgb aColor,TRgb aBackgroundColor,CGraphicsContext::TDrawMode aDrawMode,TInt aShadowMode)
	{
	Shadow(aColor,aShadowMode);

	if (aDrawMode == CGraphicsContext::EDrawModeNOTPEN)
		aColor = ~aColor;

	TRgb foreColor[4];
	Normalize(aColor,foreColor);

	TRgb backColor[4];
	Normalize(aBackgroundColor,backColor);

	TRgb fore11 = foreColor[0];
	TRgb fore21 = foreColor[1];
	TRgb back11 = backColor[0];
	TRgb back21 = backColor[1];
	TRgb fore12 = foreColor[2];
	TRgb fore22 = foreColor[3];
	TRgb back12 = backColor[2];
	TRgb back22 = backColor[3];

	TInt startY = aRect.iTl.iY;
	TInt endY = aRect.iBr.iY - 1;

	if (startY & 1)
		{
		CheckScanline(aRect.iTl.iX,startY,aRect.Width(),fore12,fore22,back12,back22,aDrawMode);
		startY++;
		}

	for (TInt yy = startY; yy < endY; yy += 2)
		{
		CheckScanline(aRect.iTl.iX,yy,aRect.Width(),fore11,fore21,back11,back21,aDrawMode);
		CheckScanline(aRect.iTl.iX,yy + 1,aRect.Width(),fore12,fore22,back12,back22,aDrawMode);
		}

	if (aRect.iBr.iY & 1)
		{
		CheckScanline(aRect.iTl.iX,endY,aRect.Width(),fore11,fore21,back11,back21,aDrawMode);
		}
	}

void CTLowLevel::CheckShadowRgb(const TRect& aRect,TRgb aColor,TInt aShadowMode)
	{
	TRgb foreColor[4];
	Normalize(aColor,foreColor);

	TRgb fore11 = foreColor[0];
	TRgb fore21 = foreColor[1];
	TRgb fore12 = foreColor[2];
	TRgb fore22 = foreColor[3];

	Shadow(fore11,aShadowMode & 2);
	Shadow(fore21,aShadowMode & 2);
	Shadow(fore12,aShadowMode & 2);
	Shadow(fore22,aShadowMode & 2);

	Normalize(fore11);
	Normalize(fore21);
	Normalize(fore12);
	Normalize(fore22);

	Shadow(fore11,aShadowMode & 1);
	Shadow(fore21,aShadowMode & 1);
	Shadow(fore12,aShadowMode & 1);
	Shadow(fore22,aShadowMode & 1);

	Normalize(fore11);
	Normalize(fore21);
	Normalize(fore12);
	Normalize(fore22);

	TInt startY = aRect.iTl.iY;
	TInt endY = aRect.iBr.iY - 1;

	iDrawDevice->CFbsDrawDevice::ShadowArea(aRect);

	if (iDispMode== EColor16MAP)
		{
		//No dithering, no blending, just checking for a solid colour
		fore12.SetAlpha(0);//do not want any blending to take place
		fore22.SetAlpha(0);//do not want any blending to take place
		for (TInt yy = startY; yy < endY; yy ++)
			{
			CheckScanline(aRect.iTl.iX,yy,aRect.Width(),fore11,fore11,fore22,fore22,CGraphicsContext::EDrawModePEN);
			}
		return;
		}

	if (startY & 1)
		{
		CheckScanline(aRect.iTl.iX,startY,aRect.Width(),fore12,fore22,fore12,fore22,CGraphicsContext::EDrawModePEN);
		startY++;
		}

	for (TInt yy = startY; yy < endY; yy += 2)
		{
		CheckScanline(aRect.iTl.iX,yy,aRect.Width(),fore11,fore21,fore11,fore21,CGraphicsContext::EDrawModePEN);
		CheckScanline(aRect.iTl.iX,yy + 1,aRect.Width(),fore12,fore22,fore12,fore22,CGraphicsContext::EDrawModePEN);
		}

	if (aRect.iBr.iY & 1)
		{
		CheckScanline(aRect.iTl.iX,endY,aRect.Width(),fore11,fore21,fore11,fore21,CGraphicsContext::EDrawModePEN);
		}
	}

void CTLowLevel::CheckScanline(TInt aX,TInt aY,TInt aLength,TRgb aFore1,TRgb aFore2,TRgb aBack1,TRgb aBack2,CGraphicsContext::TDrawMode aDrawMode)
	{
	iDrawDevice->ReadLine(aX,aY,aLength,iDrawDevice->ScanLineBuffer(),iDispMode);

	TUint32 binaryPixel1 = BinaryValue(aFore1,aBack1,aDrawMode);
	TUint32 binaryPixel2 = BinaryValue(aFore2,aBack2,aDrawMode);
	if (aX & 1)
		{
		TUint32 spare = binaryPixel1;
		binaryPixel1 = binaryPixel2;
		binaryPixel2 = spare;
		}

	TInt extra = aLength & 1;
	aLength &= ~1;
	TInt x = 0;

	if (iPostBlendShadow)
		{
		PostBlendShadow(binaryPixel1);
		PostBlendShadow(binaryPixel2);
		}
	if (i16MAUserDispMode)
		{
		binaryPixel1 = PMA2NonPMAPixel(binaryPixel1,PtrTo16BitNormalisationTable());
		binaryPixel2 = PMA2NonPMAPixel(binaryPixel1,PtrTo16BitNormalisationTable());
		}
	while (x < aLength)
		{
		TUint32 binaryValue = ExtractBinaryValue(x,iDrawDevice->ScanLineBuffer(),iDispMode);
		CheckMatch(binaryPixel1,binaryValue);
		x++;

		binaryValue = ExtractBinaryValue(x,iDrawDevice->ScanLineBuffer(),iDispMode);
		CheckMatch(binaryPixel2,binaryValue);
		x++;
		}
	if (extra)
		{
		TUint32 binaryValue = ExtractBinaryValue(x,iDrawDevice->ScanLineBuffer(),iDispMode);
		CheckMatch(binaryPixel1,binaryValue);
		}
	}

void CTLowLevel::CheckLine(TUint8* aWriteBuffer,TUint8* aReadBuffer,TUint8* aBackBuffer,TInt aPixelLength,CGraphicsContext::TDrawMode aDrawMode,TDisplayMode aDispMode)
	{
	TInt wholeBytes = 0;
	TInt extraBits = 0;

	switch (aDispMode)
		{
	case EGray2:
		wholeBytes = aPixelLength / 8;
		extraBits = aPixelLength & 7;
		break;
	case EGray4:
		wholeBytes = aPixelLength / 4;
		extraBits = (aPixelLength & 3) * 2;
		break;
	case EGray16:
	case EColor16:
		wholeBytes = aPixelLength / 2;
		extraBits = (aPixelLength & 1) * 4;
		break;
	case EGray256:
	case EColor256:
		wholeBytes = aPixelLength;
		break;
	case EColor4K:
	case EColor64K:
		wholeBytes = aPixelLength * 2;
		break;
	case EColor16M:
		wholeBytes = aPixelLength * 3;
		break;
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		wholeBytes = aPixelLength * 4;
		break;
	default:
		break;
		};

	TUint8* readLimit = aReadBuffer + wholeBytes;
	TUint8 mask = TUint8(0xff >> (8 - extraBits));
	TInt byteCounter = 0;

	switch (aDrawMode)
		{
	case CGraphicsContext::EDrawModeAND:
		if (iDispMode==EColor16MAP)
			break; //logical opeations not supported for premultiplied alpha
		for (; aReadBuffer < readLimit; aReadBuffer++, aWriteBuffer++, aBackBuffer++)
			{
			if(!((aDispMode == EColor16MU || aDispMode == EColor16MA || aDispMode == EColor16MAP)
				  && ++byteCounter % 4 == 0))
				Check(*aReadBuffer == (*aWriteBuffer & *aBackBuffer));
			}
		if (extraBits > 0)
			Check((*aReadBuffer & mask) == (*aWriteBuffer & *aBackBuffer & mask));
		break;
	case CGraphicsContext::EDrawModePEN:
		if(aDispMode == EColor16MU || aDispMode == EColor16MA || aDispMode == EColor16MAP)
			{
			for (; aReadBuffer<readLimit; aReadBuffer+=4, aWriteBuffer+=4)
				{
				TBool fail=EFalse;
				Blend(aWriteBuffer,aBackBuffer,aDispMode);
				if (!iFuzzyMatch)
					{
					fail|=Check(AbsDiff(*aReadBuffer,*aWriteBuffer)<2);
					fail|=Check(AbsDiff(*(aReadBuffer+1),*(aWriteBuffer+1))<2);
					fail|=Check(AbsDiff(*(aReadBuffer+2),*(aWriteBuffer+2))<2);
					fail|=Check(AbsDiff(*(aReadBuffer+3),*(aWriteBuffer+3))<2);
					}
				else
					{
					fail|=Check(AbsDiff(*aReadBuffer,*aWriteBuffer)<KInaccuracyLimit);
					fail|=Check(AbsDiff(*(aReadBuffer+1),*(aWriteBuffer+1))<KInaccuracyLimit);
					fail|=Check(AbsDiff(*(aReadBuffer+2),*(aWriteBuffer+2))<KInaccuracyLimit);
					fail|=Check(AbsDiff(*(aReadBuffer+3),*(aWriteBuffer+3))<KInaccuracyLimit);
					}
				if (fail)
					{
					_LIT(KLog,"The values 0x%x and 0x%x don't match, fuzzyMatch=%d (limit=%d), memory 0x%x 0x%x");
					INFO_PRINTF7(KLog,*reinterpret_cast<TUint*>(aReadBuffer),*reinterpret_cast<TUint*>(aWriteBuffer),iFuzzyMatch,KInaccuracyLimit,aReadBuffer,aWriteBuffer);
					}
				}
			}
		else
			{
			for (; aReadBuffer < readLimit; aReadBuffer++, aWriteBuffer++)
				{
				if (Check(*aReadBuffer == *aWriteBuffer))
					{
					_LIT(KLog,"The values 0x%x and 0x%x don't match, memory 0x%x 0x%x");
					INFO_PRINTF5(KLog,*aReadBuffer,*aWriteBuffer,aReadBuffer,aWriteBuffer);
					}
				}
			if (extraBits > 0)
				Check((*aReadBuffer & mask) == (*aWriteBuffer & mask));
			}
		break;
	case CGraphicsContext::EDrawModeNOTPEN:
		if (iDispMode==EColor16MAP)
			break; //logical opeations not supported for premultiplied alpha
		if(aDispMode == EColor16MU || aDispMode == EColor16MA )
			{
			for (; aReadBuffer < readLimit; aReadBuffer +=4, aWriteBuffer+=4)
				{
				*aWriteBuffer ^= 0xff;
				*(aWriteBuffer+1) ^= 0xff;
				*(aWriteBuffer+2) ^= 0xff;

				Blend(aWriteBuffer,aBackBuffer,aDispMode);

				if (!iFuzzyMatch)
					{
					Check(AbsDiff(*aReadBuffer, *aWriteBuffer) < 2);
					Check(AbsDiff(*(aReadBuffer+1), *(aWriteBuffer+1)) < 2);
					Check(AbsDiff(*(aReadBuffer+2), *(aWriteBuffer+2)) < 2);
					Check(AbsDiff(*(aReadBuffer+3), *(aWriteBuffer+3)) < 2);
					}
				else
					{
					Check(AbsDiff(*aReadBuffer, *aWriteBuffer) < KInaccuracyLimit);
					Check(AbsDiff(*(aReadBuffer+1), *(aWriteBuffer+1)) < KInaccuracyLimit);
					Check(AbsDiff(*(aReadBuffer+2), *(aWriteBuffer+2)) < KInaccuracyLimit);
					Check(AbsDiff(*(aReadBuffer+3), *(aWriteBuffer+3)) < KInaccuracyLimit);
					}
				}
			}
		else
			{

			for (; aReadBuffer < readLimit; aReadBuffer++, aWriteBuffer++)
				{
				if(!((aDispMode == EColor16MU || aDispMode == EColor16MA || aDispMode == EColor16MAP) && ++byteCounter % 4 == 0))
					Check(*aReadBuffer == (*aWriteBuffer ^ 0xff));
				}
			if (extraBits > 0)
				Check((*aReadBuffer & mask) == ((*aWriteBuffer ^ 0xff) & mask));
			}
		break;
	case CGraphicsContext::EDrawModeXOR:
		if (iDispMode==EColor16MAP)
			break;//logical opeations not supported for premultiplied alpha
		for (; aReadBuffer < readLimit; aReadBuffer++, aWriteBuffer++, aBackBuffer++)
			{
			if(!((aDispMode == EColor16MU || aDispMode == EColor16MA || aDispMode == EColor16MAP) && ++byteCounter % 4 == 0))
				Check(*aReadBuffer == (*aWriteBuffer ^ *aBackBuffer));
			}
		if (extraBits > 0)
			Check((*aReadBuffer & mask) == ((*aWriteBuffer ^ *aBackBuffer) & mask));
		break;
	case CGraphicsContext::EDrawModeOR:
		if (iDispMode==EColor16MAP)
			break;//logical opeations not supported for premultiplied alpha
		for (; aReadBuffer < readLimit; aReadBuffer++, aWriteBuffer++, aBackBuffer++)
			{
			if(!((aDispMode == EColor16MU || aDispMode == EColor16MA || aDispMode == EColor16MAP) && ++byteCounter % 4 == 0))
				Check(*aReadBuffer == (*aWriteBuffer | *aBackBuffer));
			}
		if (extraBits > 0)
			Check((*aReadBuffer & mask) == ((*aWriteBuffer | *aBackBuffer) & mask));
		break;
	case CGraphicsContext::EDrawModeNOTSCREEN:
		if (iDispMode==EColor16MAP)
			break;//logical opeations not supported for premultiplied alpha
		if (aDispMode != EColor4K)
			{
			for (; aReadBuffer < readLimit; aReadBuffer++, aBackBuffer++)
				{
				if(!((aDispMode == EColor16MU || aDispMode == EColor16MA || aDispMode == EColor16MAP) && ++byteCounter % 4 == 0))
					{
					if (iFuzzyMatch==EFalse)
						Check(*aReadBuffer == (*aBackBuffer ^ 0xff));
					else
						{
						TUint8 vals[3];
						vals[0]=*aReadBuffer;
						//put in some tolerance values, try with +/- 1, to begin with
						if (vals[0]<255)
							vals[1]=vals[0]+1;
						else
							vals[1]=vals[0];
						if (vals[0]>0)
							vals[2]=vals[0]-1;
						else
							vals[2]=vals[0];
						Check((vals[0] == (*aBackBuffer ^ 0xff))||
							  (vals[1] == (*aBackBuffer ^ 0xff))||
							  (vals[2] == (*aBackBuffer ^ 0xff)));
						}
					}
				}
			if (extraBits > 0)
				Check((*aReadBuffer & mask) == ((*aBackBuffer ^ 0xff) & mask));
			}
		else
			{
			while (aReadBuffer < readLimit)
				{
				if (TInt(aReadBuffer) & 1)
					Check(*aReadBuffer++ == (*aBackBuffer++ ^ 0x0f));
				else
					Check(*aReadBuffer++ == (*aBackBuffer++ ^ 0xff));
				}
			}
		break;
	default:
		break;
		};
	}

void CTLowLevel::CheckBinary(const TRect& aRect,TUint32* aBuffer,TRgb aForeColor,TRgb aBackColor,CGraphicsContext::TDrawMode aDrawMode,TInt aShadowMode,TBool aWrapDataWords,TBool aUp)
	{
	Shadow(aForeColor,aShadowMode);

	if (aDrawMode == CGraphicsContext::EDrawModeNOTPEN)
		aForeColor = ~aForeColor;

	TRgb foreColor[4];
	Normalize(aForeColor,foreColor);

	TRgb backColor[4];
	Normalize(aBackColor,backColor);

	foreColor[0] = RgbValue(foreColor[0],backColor[0],aDrawMode);
	foreColor[1] = RgbValue(foreColor[1],backColor[1],aDrawMode);
	foreColor[2] = RgbValue(foreColor[2],backColor[2],aDrawMode);
	foreColor[3] = RgbValue(foreColor[3],backColor[3],aDrawMode);


	if (iDispMode==EColor16MAP)
		{
		//pre-multiply and unpremultiply
		TInt count;
		for (count=0;count<4;count++)
			{
			TUint32 tempInt;
			tempInt = backColor[count].Color16MAP(); //Now premultiplied
			backColor[count] = TRgb::Color16MAP(tempInt);
			}
		}

	TUint32 data = *aBuffer++;
	TUint32 mask = 1;

	TInt yy = (aUp) ? aRect.iBr.iY - 1  : aRect.iTl.iY;
	TInt ylimit = (aUp) ? aRect.iTl.iY - 1 : aRect.iBr.iY;
	TInt yinc = (aUp) ? -1 : 1;

	TRgb pixelBuffer[KCheckBinaryPixelBufferSize];
	__ASSERT_ALWAYS(aRect.Width() <= KCheckBinaryPixelBufferSize,User::Panic(_L("CheckBinary buffer"),KErrOverflow));

	for (; yy != ylimit; yy += yinc)
		{
		TInt yoffset = 2 * (yy & 1);

		iDrawDevice->ReadLine(aRect.iTl.iX,yy,aRect.Width(),pixelBuffer,ERgb);
		TRgb* color = pixelBuffer;

		for (TInt xx = aRect.iTl.iX; xx < aRect.iBr.iX; xx++)
			{
			if (!mask)
				{
				mask = 1;
				data = *aBuffer++;
				}

			if (data & mask)
				CheckMatch((*color).Internal(), foreColor[(xx & 1) + yoffset].Internal());
			else
				CheckMatch((*color).Internal(), backColor[(xx & 1) + yoffset].Internal());

			color++;
			mask <<= 1;
			}

		if (aWrapDataWords)
			mask = 0;
		}
	}

void CTLowLevel::CheckBackground(const TRect& aRect,TRgb aBackgroundColor)
	{
	iBlendTestColors= EFalse;
	if (aRect.iTl.iX > 0)
		CheckRgb(TRect(aRect.iTl.iX - 1,aRect.iTl.iY,aRect.iTl.iX,aRect.iBr.iY),aBackgroundColor,aBackgroundColor,CGraphicsContext::EDrawModePEN,0);
	if (aRect.iTl.iY > 0)
		CheckRgb(TRect(aRect.iTl.iX,aRect.iTl.iY - 1,aRect.iBr.iX,aRect.iTl.iY),aBackgroundColor,aBackgroundColor,CGraphicsContext::EDrawModePEN,0);
	if (aRect.iBr.iX < iSize.iWidth - 1)
		CheckRgb(TRect(aRect.iBr.iX,aRect.iTl.iY,aRect.iBr.iX + 1,aRect.iBr.iY),aBackgroundColor,aBackgroundColor,CGraphicsContext::EDrawModePEN,0);
	if (aRect.iBr.iY < iSize.iHeight - 1)
		CheckRgb(TRect(aRect.iTl.iX,aRect.iBr.iY,aRect.iBr.iX,aRect.iBr.iY + 1),aBackgroundColor,aBackgroundColor,CGraphicsContext::EDrawModePEN,0);
	iBlendTestColors= ETrue;
	}
/**
This function is copied as it is from BMDRAW32A.CPP which is used to test WriteRgbOutlineAndShadow for EColor16MA.
It is used in CTLowLevel::CheckBlendedOutlineAndShadow.
@param aBeneath The background pixel colour value
@param aSrcColor The source pixel colour value
@param aAlpha The alpha value
*/
FORCEINLINE TUint32 OptimizedBlend32A(TUint32 aBeneath,TUint32 aSrcColor,TUint8 aAlpha)
 	{
	if(aAlpha)
		{
		if(aAlpha == 0xff) // opaque, so unchanged
			{
			//Still need to convert source to destination from non-multiplied to pre-multiplied
			//But this code resolves to a copy. The ARM optimiser prefers shifts over big constants.
			return (aSrcColor|(aAlpha<<24));
			}
		else
			{
			//0, 1, 2, 3
			//b, g, rect, alpha

			const TUint32 srcMult = aAlpha;
			TUint32 destMult = ((255 - aAlpha) * ((aBeneath >> 24)));
			//This gives a slightly more accurate result than ((aBeneath >> 24)+1)
			destMult=destMult+(destMult>>8);
			destMult+= 0x0080;
			destMult >>= 8;

			TUint32 rb =(((aSrcColor&0x00ff00ff)*srcMult)) + (((aBeneath&0x00ff00ff)*destMult));
			rb = rb+((rb>>8)&0x00ff00ff);
			rb+=0x00800080;
			rb>>=8;
			TUint32 ag = (((aSrcColor&0x0000ff00)*srcMult)) + (((aBeneath&0x0000ff00)*destMult));
			ag>>=8;	 //Note that if alpha is processed here, this shift must be performed before the multiplies
			ag = ag+((ag>>8)&0x00ff00ff);
			ag+=0x00800080;
			TUint32 aa = srcMult+destMult;
			return (rb&0x00ff00ff) | (ag&0x0000ff00) | (aa << 24);

			}
		}
 	else // completely transparent
		{
		return aBeneath;
 		}

 	}

/**
Helper function for TestWriteRgbOutlineAndShadow(). Creates the final colour, blending the outline, shadow,
fill and background colour using lookup table provided by Monotype and compares with the
pixel colour drawn using WriteRgbOutlineAndShadow.

@param aOutlinePenColor Colour used for drawing outline of font
@param aShadowColor Colour used for drawing shadow of font
@param aFillColor Colour used for filling of font
@param aBackgroundColor Background colour of the pixel
@param aLookupIndex Index of the lookup table to be used for generating final colour
@param aLength Number of pixels to compare the pixel colour with the generated colour
@param aReadBuffer Buffer containing the colours drawn using WriteRgbOutlineAndShadow.This will be used for comparing
@return EFalse if pixel colour doesnt match with the calculated colour, otherwise ETrue on success.
*/
TBool CTLowLevel::CheckBlendedOutlineAndShadow(TRgb aOutlinePenColor, TRgb aShadowColor, TRgb aFillColor,
												TRgb aBackgroundColor, TInt aLookupIndex, TInt aLength, TUint8* aReadBuffer)
	{
	TRgb finalColor;
	TInt alpha = aOutlinePenColor.Internal() >> 24;

	if (255 == FourColorBlendLookup[aLookupIndex][3])
		{
		//background colour
		finalColor.SetInternal(aBackgroundColor.Internal());

		/*Reset the alpha with background colour alpha as in product code in case of 255 == FourColorBlendLookup[aLookupIndex][3]
		it doesnt draw and leaves the background colour as it is. So the alpha to be checked should be of background colour alpha*/
		alpha = aBackgroundColor.Alpha();
		}
	else if (255 == FourColorBlendLookup[aLookupIndex][2])
		{
		//fill colour
		finalColor.SetInternal(aFillColor.Internal());
		}
	else if (255 == FourColorBlendLookup[aLookupIndex][1])
		{
		//Shadow colour
		finalColor.SetInternal(aShadowColor.Internal());
		}
	else if (255 == FourColorBlendLookup[aLookupIndex][0])
		{
		//Outline colour
		finalColor.SetInternal(aOutlinePenColor.Internal());
		}
	else
		{
		TInt blendedRedColor = (aOutlinePenColor.Red() * FourColorBlendLookup[aLookupIndex][0] +
				  				aShadowColor.Red() * FourColorBlendLookup[aLookupIndex][1] +
				  				aFillColor.Red() * FourColorBlendLookup[aLookupIndex][2] +
				  				aBackgroundColor.Red() * FourColorBlendLookup[aLookupIndex][3]) >> 8;

		TInt blendedGreenColor = (aOutlinePenColor.Green() * FourColorBlendLookup[aLookupIndex][0] +
							 	aShadowColor.Green() * FourColorBlendLookup[aLookupIndex][1] +
							 	aFillColor.Green() * FourColorBlendLookup[aLookupIndex][2] +
							 	aBackgroundColor.Green() * FourColorBlendLookup[aLookupIndex][3]) >> 8;

		TInt blendedBlueColor = (aOutlinePenColor.Blue() * FourColorBlendLookup[aLookupIndex][0] +
								aShadowColor.Blue() * FourColorBlendLookup[aLookupIndex][1] +
								aFillColor.Blue() * FourColorBlendLookup[aLookupIndex][2] +
								aBackgroundColor.Blue() * FourColorBlendLookup[aLookupIndex][3]) >> 8;

		finalColor = TRgb(blendedRedColor, blendedGreenColor, blendedBlueColor);
		}

	//Set the alpha in the final colour
	finalColor.SetAlpha(alpha);

	//Alpha blending is not supported for display modes below EColor64K.
	switch(iDispMode)
		{
	case EColor64K:
	case EColor16M:
	case EColor16MU:
		if (alpha != 0xff)
			{
			finalColor = AlphaBlend(finalColor.Red(), finalColor.Green(), finalColor.Blue(), aBackgroundColor.Red(), aBackgroundColor.Green(), aBackgroundColor.Blue(), alpha);
			}
		break;
	case EColor16MA:
		//Just use the background color to draw in case 255 == FourColorBlendLookup[aLookupIndex][3]
		if (255 != FourColorBlendLookup[aLookupIndex][3])
			{
			finalColor.SetInternal(OptimizedBlend32A(aBackgroundColor.Internal(), finalColor.Internal(), alpha));
			}
		break;
	case EColor16MAP:
		//Just use the background color to draw in case 255 == FourColorBlendLookup[aLookupIndex][3]
		if (255 != FourColorBlendLookup[aLookupIndex][3])
			{
			TUint32 color16MAP = finalColor.Internal();
			Convert2PMA(color16MAP);
			finalColor.SetInternal(PMAPixelBlend(aBackgroundColor.Internal(), color16MAP));
			}
		break;
		};

	Normalize(finalColor);
	TColorConvertor& colorConvertor = ColorConvertor(iDispMode);
	// Check each pixel of the line, it should match with the calculated blended color.
	for (TInt count = 0; count < aLength; count++)
		{
		TRgb readValue = ExtractRgbValue(count, aReadBuffer, iDispMode);
		if (colorConvertor.Index(finalColor) != colorConvertor.Index(readValue))
			{
			return EFalse;
			}
		}
	return ETrue;
	}

TRgb CTLowLevel::RgbValue(TRgb aFore,TRgb aBack,CGraphicsContext::TDrawMode aDrawMode)
	{
	TUint32 value = BinaryValue(aFore,aBack,aDrawMode);

	switch (iDispMode)
		{
	case EGray2:
		return TRgb::Gray2(value);
	case EGray4:
		return TRgb::Gray4(value);
	case EGray16:
		return TRgb::Gray16(value);
	case EGray256:
		return TRgb::Gray256(value);
	case EColor16:
		return TRgb::Color16(value);
	case EColor256:
		return TRgb::Color256(value);
	case EColor4K:
		return TRgb::Color4K(value);
	case EColor64K:
		return TRgb::Color64K(value);
	case EColor16M:
		return TRgb::Color16M(value);
	case EColor16MU:
		return TRgb::Color16MU(value);
	case EColor16MA:
		return TRgb::Color16MA(value);
	case EColor16MAP:
		return TRgb::Color16MAP(value);
	default:
		break;
		};
	return KRgbBlack;
	}

TUint32 CTLowLevel::BinaryValue(TRgb aFore,TRgb aBack,CGraphicsContext::TDrawMode aDrawMode)
	{
	TUint32 f = 0;
	TUint32 b = 0;
	TUint32 notVal = 0;

	switch (iDispMode)
		{
	case EGray2:
		f = aFore.Gray2();
		b = aBack.Gray2();
		notVal = 1;
		break;
	case EGray4:
		f = aFore.Gray4();
		b = aBack.Gray4();
		notVal = 3;
		break;
	case EGray16:
		f = aFore.Gray16();
		b = aBack.Gray16();
		notVal = 0xf;
		break;
	case EGray256:
		f = aFore.Gray256();
		b = aBack.Gray256();
		notVal = 0xff;
		break;
	case EColor16:
		f = aFore.Color16();
		b = aBack.Color16();
		notVal = 0xf;
		break;
	case EColor256:
		f = aFore.Color256();
		b = aBack.Color256();
		notVal = 0xff;
		break;
	case EColor4K:
		f = aFore.Color4K();
		b = aBack.Color4K();
		notVal = 0xfff;
		break;
	case EColor64K:
		f = aFore.Color64K();
		b = aBack.Color64K();
		notVal = 0xffff;
		break;
	case EColor16M:
		f = aFore.Color16M();
		b = aBack.Color16M();
		notVal = 0xffffff;
		break;
	case EColor16MU:
		f = aFore.Color16MU();
		b = aBack.Color16MU();
		notVal = 0x00ffffff;
		break;
	case EColor16MA:
		f = aFore.Color16MA();
		b = aBack.Color16MA();
		notVal = 0xffffffff;
		break;
	case EColor16MAP:
		f = aFore.Color16MAP();
		b = aBack.Color16MAP();

		//do not want to blend backgound colours for testing
		if (iBlendTestColors && (aDrawMode&CGraphicsContext::EDrawModePEN))
			{
			Blend((TUint8*)(&f),(TUint8*)(&b),iDispMode);
			}
		notVal = 0xffffffff;
		break;
	default:
		break;
		};

	switch (aDrawMode)
		{
	case CGraphicsContext::EDrawModeAND:		return f & b;
	case CGraphicsContext::EDrawModePEN:		return f;
	case CGraphicsContext::EDrawModeWriteAlpha:	return f;
	case CGraphicsContext::EDrawModeXOR:		return f ^ b;
	case CGraphicsContext::EDrawModeOR:			return f | b;
	case CGraphicsContext::EDrawModeNOTSCREEN:	return b ^ notVal;
	case CGraphicsContext::EDrawModeNOTPEN:		return f;
	default:
		break;
		};
	return 0;
	}

TRgb CTLowLevel::ExtractRgbValue(TInt aX,TUint8* aBuffer,TDisplayMode aDispMode)
	{
	TUint32 value = ExtractBinaryValue(aX,(TUint32*)aBuffer,aDispMode);

	switch (aDispMode)
		{
	case EGray2:
		return TRgb::Gray2(value);
	case EGray4:
		return TRgb::Gray4(value);
	case EGray16:
		return TRgb::Gray16(value);
	case EGray256:
		return TRgb::Gray256(value);
	case EColor16:
		return TRgb::Color16(value);
	case EColor256:
		return TRgb::Color256(value);
	case EColor4K:
		return TRgb::Color4K(value);
	case EColor64K:
		return TRgb::Color64K(value);
	case EColor16M:
		return TRgb::Color16M(value);
	case ERgb:
		return TRgb(value, value>>24);
	case EColor16MU:
		return TRgb::Color16MU(value);
	case EColor16MA:
		return TRgb::Color16MA(value);
	case EColor16MAP:
		return TRgb::Color16MAP(value);
	default:
		break;
		};
	return KRgbBlack;
	}

TUint32 CTLowLevel::ExtractBinaryValue(TInt aX,TUint32* aBuffer,TDisplayMode aDispMode)
	{
	switch (aDispMode)
		{
	case EGray2:
		return ((*(aBuffer + (aX >> 5))) >> (aX & 0x1f)) & 1;
	case EGray4:
		return ((*(aBuffer + (aX >> 4))) >> ((aX & 0xf) * 2)) & 3;
	case EGray16:
	case EColor16:
		return ((*(aBuffer + (aX >> 3))) >> ((aX & 7) * 4)) & 0xf;
	case EGray256:
	case EColor256:
		return ((*(aBuffer + (aX >> 2))) >> ((aX & 3) * 8)) & 0xff;
	case EColor4K:
		return ((*(aBuffer + (aX >> 1))) >> ((aX & 1) * 16)) & 0xfff;
	case EColor64K:
		return ((*(aBuffer + (aX >> 1))) >> ((aX & 1) * 16)) & 0xffff;
	case EColor16M:
		{
		TUint8* buffer = ((TUint8*)aBuffer) + (aX * 3);
		return *buffer | (*(buffer + 1) << 8) | (*(buffer + 2) << 16);
		}
	case ERgb:
		return *(aBuffer + aX);
	case EColor16MU:
		return *(aBuffer + aX) & 0x00FFFFFF;
	case EColor16MA:
	case EColor16MAP:
		return *(aBuffer + aX);
	default:
		break;
		};
	return 0;
	}

void CTLowLevel::Normalize(TRgb& aColor)
	{
	return(Normalize(aColor,iDispMode));
	}

void CTLowLevel::Normalize(TRgb& aColor, TDisplayMode aDispMode)
	{
	switch (aDispMode)
		{
	case EGray2:
		aColor = TRgb::Gray2(aColor.Gray2());
		break;
	case EGray4:
		aColor = TRgb::Gray4(aColor.Gray4());
		break;
	case EGray16:
		aColor = TRgb::Gray16(aColor.Gray16());
		break;
	case EGray256:
		aColor = TRgb::Gray256(aColor.Gray256());
		break;
	case EColor16:
		aColor = TRgb::Color16(aColor.Color16());
		break;
	case EColor256:
		aColor = TRgb::Color256(aColor.Color256());
		break;
	case EColor4K:
		aColor = TRgb::Color4K(aColor.Color4K());
		break;
	case EColor64K:
		aColor = TRgb::Color64K(aColor.Color64K());
		break;
	case EColor16M:
		aColor = TRgb::Color16M(aColor.Color16M());
		break;
	case EColor16MU:
		aColor = TRgb::Color16MU(aColor.Color16MU());
		break;
	case EColor16MA:
		aColor = TRgb::Color16MA(aColor.Color16MA());
		break;
	case EColor16MAP:
		//do nothing, because TRGb is already unpremultiplied
		break;
	default:
		break;
		};
	}

void CTLowLevel::Normalize(TRgb& aColor,TRgb aDitherColors[4])
	{
	switch (iDispMode)
		{
	case EGray2:
		FillArray(TRgb::Gray2(aColor.Gray2()),aDitherColors);
		break;
	case EGray4:
		if (iUserDispMode == EGray2)
			FillArray(TRgb::Gray2(aColor.Gray2()),aDitherColors);
		else
			{
			TInt gray16 = aColor.Gray16();
			aDitherColors[0] = TRgb::Gray4(ditherlutab[gray16][0]);
			aDitherColors[1] = TRgb::Gray4(ditherlutab[gray16][1]);
			aDitherColors[2] = TRgb::Gray4(ditherlutab[gray16][2]);
			aDitherColors[3] = TRgb::Gray4(ditherlutab[gray16][3]);
			}
		break;
	case EGray16:
		if (iUserDispMode == EGray2)
			FillArray(TRgb::Gray2(aColor.Gray2()),aDitherColors);
		else if (iUserDispMode == EGray4)
			{
			TInt gray16 = aColor.Gray16();
			aDitherColors[0] = TRgb::Gray4(ditherlutab[gray16][0]);
			aDitherColors[1] = TRgb::Gray4(ditherlutab[gray16][1]);
			aDitherColors[2] = TRgb::Gray4(ditherlutab[gray16][2]);
			aDitherColors[3] = TRgb::Gray4(ditherlutab[gray16][3]);
			}
		else
			FillArray(TRgb::Gray16(aColor.Gray16()),aDitherColors);
		break;
	case EGray256:
		FillArray(TRgb::Gray256(aColor.Gray256()),aDitherColors);
		break;
	case EColor16:
		FillArray(TRgb::Color16(aColor.Color16()),aDitherColors);
		break;
	case EColor256:
		FillArray(TRgb::Color256(aColor.Color256()),aDitherColors);
		break;
	case EColor4K:
		FillArray(TRgb::Color4K(aColor.Color4K()),aDitherColors);
		break;
	case EColor64K:
		FillArray(TRgb::Color64K(aColor.Color64K()),aDitherColors);
		break;
	case EColor16M:
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		FillArray(aColor,aDitherColors);
		break;
	default:
		break;
		};
	}

void CTLowLevel::FillArray(TRgb aColor,TRgb aArray[4])
	{
	aArray[0] = aColor;
	aArray[1] = aColor;
	aArray[2] = aColor;
	aArray[3] = aColor;
	}


void CTLowLevel::PostBlendShadow(TUint32 &aPmaColor)
	{
	//this function should only be called for PMA colours
	const TInt alpha = aPmaColor >> 24;
	TUint32 value = aPmaColor & 0x00ffffff;

	if (iPostBlendShadow & CFbsDrawDevice::EFade)
		{
#if defined(SYMBIAN_USE_FAST_FADING)
		TUint32 fast_fade_offset = ((SYMBIAN_USE_FAST_FADING & 0xff) * alpha) >>8;
		fast_fade_offset = fast_fade_offset | (fast_fade_offset << 8) | (fast_fade_offset <<16);
		value = ((value >> 1) & ~0x00808080) + (fast_fade_offset);
		value = value | (((TUint32)alpha)<<24);
#else
	/*
	here blackmap = 200,
		 whitemap = 100
	iFadeMapFactor = aWhiteMap - aBlackMap + 1;
	iFadeMapOffset = aBlackMap;
	*/

		const TInt fadeMapOffset = ((alpha * 0x7f) >> 8) & 0xff;
		const TInt wordFadeMapOffset = ((fadeMapOffset) << 16) | (fadeMapOffset);
		const TInt rb = ((((value & 0x00ff00ff) * 0x7f) >> 8) + wordFadeMapOffset) & 0x00ff00ff;
	  	const TInt g = ((((value & 0x0000ff00) * 0x7f) >> 16) + fadeMapOffset) << 8;
		value = rb | g | (((TUint32)alpha)<<24);
#endif
		}

	if (iPostBlendShadow & CFbsDrawDevice::EShadow)
		{
		TInt alpha = (aPmaColor>>24);
		TInt red = (value>>16)&0xff;
		TInt green = (value>>8)&0xff;
		TInt blue = value &0xff;

		TInt shadow= (alpha*0x40)>>8;
		red = Max(0,red-shadow);
		green = Max (0,green-shadow);
		blue = Max (0,blue-shadow);
		value = (((TUint32)alpha)<<24)|(((TUint32)red)<<16)|(((TUint32)green)<<8)|((TUint32)blue);
		}
	aPmaColor = value;
	}

void CTLowLevel::Shadow(TRgb& aColor,TInt aShadowMode)
	{
	if (aShadowMode & 2)
		{
		switch (iDrawDevice->DisplayMode())
			{
		case EGray2:
			aColor = TRgb::Gray256(FadeGray(aColor.Gray2() * 255));
			break;
		case EGray4:
		case EGray16:
			aColor = TRgb::Gray256(FadeGray(aColor.Gray16() * 17));
			break;
		case EGray256:
			aColor = TRgb::Gray256(FadeGray(aColor.Gray256()));
			break;
		case EColor16:
			aColor = FadeRgb(TRgb::Color16(aColor.Color16()));
			break;
		case EColor256:
			aColor = FadeRgb(TRgb::Color256(aColor.Color256()));
			break;
		case EColor4K:
			aColor = FadeRgb(TRgb::Color4K(aColor.Color4K()));
			break;
		case EColor64K:
			aColor = FadeRgb(TRgb::Color64K(aColor.Color64K()),KFastFading && iUseFastFade);
			break;
		case EColor16M:
			aColor = FadeRgb(TRgb::Color16M(aColor.Color16M()));
			break;
		case EColor16MU:
			aColor = FadeRgb(TRgb::Color16MU(aColor.Color16MU()),KFastFading && iUseFastFade);
			break;
		case EColor16MA:
			aColor = FadeRgb(TRgb::Color16MA(aColor.Color16MA()),KFastFading && iUseFastFade);
			break;
		case EColor16MAP:
			aColor = FadeRgb(TRgb::Color16MAP(aColor.Color16MAP()),KFastFading && iUseFastFade);
			break;
		default:
			break;
			};
		}

	if (aShadowMode & 1)
		{
		switch (iDrawDevice->DisplayMode())
			{
		case EGray2:
			aColor = KRgbBlack;
			break;
		case EGray4:
		case EGray16:
			aColor = TRgb::Gray16(Max(0,aColor.Gray16()-5));
			break;
		case EGray256:
			aColor = TRgb::Gray256(Max(0,aColor.Gray256()-85));
			break;
		case EColor16:
			{
			TInt color = aColor.Color16();
			if (color == 15) color--;
			else if (color == 14) color = 1;
			else if (color > 7) color += 3;
			else if (color > 4) color -= 3;
			else color = 0;
			aColor = TRgb::Color16(color);
			}
			break;
		case EColor256:
			{
			aColor = TRgb::Color256(aColor.Color256());
			TInt red = aColor.Red();
			TInt green = aColor.Green();
			TInt blue = aColor.Blue();
			red = Max(0,red-0x33);
			green = Max(0,green-0x33);
			blue = Max(0,blue-0x33);
			aColor = TRgb(red,green,blue);
			}
			break;
		case EColor4K:
			{
			TInt color = aColor.Color4K();
			TInt red = (color & 0xf00) >> 8;
			TInt green = (color & 0x0f0) >> 4;
			TInt blue = color & 0x00f;

			red = Max(0,red-5);
			green = Max(0,green-5);
			blue = Max(0,blue-5);

			aColor = TRgb::Color4K((red << 8) | (green << 4) | blue);
			}
			break;
		case EColor64K:
			{
			TInt color = aColor.Color64K();
			TInt red = (color & 0xf800) >> 11;
			TInt green = (color & 0x07e0) >> 5;
			TInt blue = color & 0x001f;

			red = Max(0,red-8);
			green = Max(0,green-16);
			blue = Max(0,blue-8);

			aColor = TRgb::Color64K((red << 11) | (green << 5) | blue);
			}
			break;
		case EColor16M:
		case EColor16MU:
		case EColor16MA:
		case EColor16MAP:
			{
			TInt red = aColor.Red();
			TInt green = aColor.Green();
			TInt blue = aColor.Blue();
			red = Max(0,red-0x40);
			green = Max(0,green-0x40);
			blue = Max(0,blue-0x40);
			aColor = TRgb(red,green,blue,aColor.Alpha());
			}
			break;
		default:
			break;
			};
		}
	}

void CTLowLevel::Blend(TUint8* aBuffer,TUint8* aBufferDest, TDisplayMode aDispMode)
	{
	TUint32* buffer = reinterpret_cast<TUint32*> (aBuffer);
	TUint32* bufferDest = reinterpret_cast<TUint32*> (aBufferDest);
	TInt mask = (*buffer & 0xff000000) >> 24;
	TRgb rgbDest;

	switch(aDispMode)
		{
	case EColor16MU:
		{
		// src + ((255 - mask) * dest) / 255
		if(mask!=255)
			{
			rgbDest=TRgb::Color16MU(*bufferDest);
			if(mask)
				{
				TRgb rgbSrc=TRgb::Color16MU(*buffer);
				rgbDest.SetRed(rgbSrc.Red() + ((255 - mask) * rgbDest.Red()) / 255);
				rgbDest.SetGreen(rgbSrc.Green() + ((255 - mask) * rgbDest.Green()) / 255);
				rgbDest.SetBlue(rgbSrc.Blue() + ((255 - mask) * rgbDest.Blue()) / 255);
				}
			*buffer=rgbDest.Internal();
			}
		}
		break;
	case EColor16MA:
		{
		// (mask * src + (255 - mask) * dest) / 255
		if(mask!=255)
			{
			rgbDest=TRgb::Color16MA(*bufferDest);
			if(mask)
				{
				TRgb rgbSrc=TRgb::Color16MA(*buffer);
				rgbDest.SetRed((mask * rgbSrc.Red() + (255 - mask) * rgbDest.Red()) / 255);
				rgbDest.SetGreen((mask * rgbSrc.Green() + (255 - mask) * rgbDest.Green()) / 255);
				rgbDest.SetBlue((mask * rgbSrc.Blue() + (255 - mask) * rgbDest.Blue()) / 255);
				}
			*buffer=rgbDest.Internal();
			}
		}
		break;
	case EColor16MAP:
		{
		/*
		* Blend function uses the Porter Duff composition equation
		* This blends two pixels with alphas:
		* Ar  = As  + Ad * (1 - As) (Blended Alpha)
		* Cr = Cs + Cd(1 - As) (Blended Colour)
		* Cr = Cs + Cd(255-As)/255 : for alpha 0-255
		* where Ar = alpha result
		* where Cr = colour result
		* where Cs = source colour
		* where Cd = destination colour
		* The function assumes that the mask buffer is the alpha value of the source pixel.
		*/
		if(mask!=255)
			{
			rgbDest=TRgb::Color16MA(*bufferDest);
			if(mask)
				{
				TInt destAlpha = (*bufferDest & 0xff000000) >> 24;
				TInt sourceAlpha = (*buffer & 0xff000000) >> 24;
				TRgb rgbSrc;
				rgbSrc.SetInternal(*buffer);
				rgbDest.SetInternal(*bufferDest);
				TInt resultAlpha = sourceAlpha +((255-sourceAlpha)*destAlpha)/255;
				rgbDest.SetRed(rgbSrc.Red() + ((255 - sourceAlpha) * rgbDest.Red()) / 255);
				rgbDest.SetGreen(rgbSrc.Green() + ((255 - sourceAlpha) * rgbDest.Green()) / 255);
				rgbDest.SetBlue(rgbSrc.Blue() + ((255 - sourceAlpha) * rgbDest.Blue())/ 255);
				rgbDest.SetAlpha(resultAlpha);
				}
			*buffer=rgbDest.Internal();
			}
		}
		break;
	default: break;
		}
	}

void CTLowLevel::Shadow(TUint8* aBuffer,TInt aByteLength,TInt aShadowMode)
	{
	TUint8* buffer = aBuffer;
	const TUint8* bufferLimit = aBuffer + aByteLength;

	if (aShadowMode & 2)
		{
		switch (iDrawDevice->DisplayMode())
			{
		case EGray2:
			while (buffer < bufferLimit)
				*buffer++ = 0xff;
			break;
		case EGray4:
			while (buffer < bufferLimit)
				{
				TInt first = FadeGray((buffer[0] & 0x03) * 85) >> 6;
				TInt second = FadeGray(((buffer[0] >> 2) & 0x03) * 85) >> 6;
				TInt third = FadeGray(((buffer[0] >> 4) & 0x03) * 85) >> 6;
				TInt fourth = FadeGray(((buffer[0] >> 6) & 0x03) * 85) >> 6;
				*buffer++ = TUint8(first | (second << 2) | (third << 4) | (fourth << 6));
				}
			break;
		case EGray16:
			while (buffer < bufferLimit)
				{
				TInt low = FadeGray((buffer[0] & 0x0f) * 17) >> 4;
				TInt high = FadeGray((buffer[0] >> 4) * 17) >> 4;
				*buffer++ = TUint8((high << 4) | low);
				}
			break;
		case EGray256:
			while (buffer < bufferLimit)
				*buffer++ = FadeGray(*buffer);
			break;
		case EColor16:
			while (buffer < bufferLimit)
				{
				TInt low = FadeRgb(TRgb::Color16(buffer[0] & 0x0f)).Color16();
				TInt high = FadeRgb(TRgb::Color16(buffer[0] >> 4)).Color16();
				*buffer++ = TUint8((high << 4) | low);
				}
			break;
		case EColor256:
			while (buffer < bufferLimit)
				*buffer++ = TUint8(FadeRgb(TRgb::Color256(buffer[0])).Color256());
			break;
		case EColor4K:
			while (buffer < bufferLimit)
				{
				TInt color4K = FadeRgb(TRgb::Color4K(buffer[0] | (buffer[1] << 8))).Color4K();
				buffer[0] = TUint8(color4K);
				buffer[1] = TUint8(color4K >> 8);
				buffer += 2;
				}
			break;
		case EColor64K:
			while (buffer < bufferLimit)
				{
				TInt color64K = FadeRgb(TRgb::Color64K(buffer[0] | (buffer[1] << 8)),ETrue).Color64K();
				buffer[0] = TUint8(color64K);
				buffer[1] = TUint8(color64K >> 8);
				buffer += 2;
				}
			break;
		case EColor16M:
			{
			while (buffer < bufferLimit)
				*buffer++ = FadeGray(buffer[0]);
			}
			break;
		case EColor16MU:
			{
			TUint32* buffer32 = reinterpret_cast <TUint32*> (buffer);
			TUint32* bufferLimit32 = buffer32 + aByteLength / 4;
			while (buffer32 < bufferLimit32)
				{
				// scanline buffer for 16MU driver is pre-multiplied
				TRgb color = FadeRgb(TRgb::Color16MAP(*buffer32),ETrue);
				// avoid rounding errors with EDrawModeAND etc.
				*buffer32++ = color.Alpha() == 255 ? color.Internal() : color.Color16MAP();
				}
			}
			break;
		case EColor16MA:
			{
			TUint32* buffer32 = reinterpret_cast <TUint32*> (buffer);
			TUint32* bufferLimit32 = buffer32 + aByteLength / 4;
			while (buffer32 < bufferLimit32)
				{
				TRgb color = FadeRgb(TRgb::Color16MA(*buffer32),ETrue);
				*buffer32++ = color.Color16MA();
				}
			}
			break;
		case EColor16MAP:
			{
			TUint32* buffer32 = reinterpret_cast <TUint32*> (buffer);
			TUint32* bufferLimit32 = buffer32 + aByteLength / 4;
			while (buffer32 < bufferLimit32)
				{
				TRgb color = FadeRgb(TRgb::Color16MAP(*buffer32),ETrue);
				*buffer32++ = color.Color16MAP();
				}
			}
			break;
		default:
			break;
			}
		}

	buffer = aBuffer;

	if (aShadowMode & 1)
		{
		switch (iDrawDevice->DisplayMode())
			{
		case EGray2:
			while (buffer < bufferLimit)
				{
				*buffer++ = 0;
				}
			break;
		case EGray4:
			while (buffer < bufferLimit)
				{
				TInt first = buffer[0] & 0x03;
				TInt second = buffer[0] & 0x0c;
				TInt third = buffer[0] & 0x30;
				TInt fourth = buffer[0] & 0xc0;
				first = Max(0,first-1);
				second = Max(0,second-4);
				third = Max(0,third-16);
				fourth = Max(0,fourth-64);
				*buffer++ = TUint8(fourth | third | second | first);
				}
			break;
		case EGray16:
			while (buffer < bufferLimit)
				{
				TInt low = buffer[0] & 0x0f;
				TInt high = buffer[0] >> 4;
				low = Max(0,low-5);
				high = Max(0,high-5);
				*buffer++ = TUint8((high << 4) | low);
				}
			break;
		case EGray256:
			while (buffer < bufferLimit)
				{
				buffer[0] = TUint8(Max(0,buffer[0]-85));
				buffer++;
				}
			break;
		case EColor16:
			while (buffer < bufferLimit)
				{
				TInt low = buffer[0] & 0x0f;
				TInt high = buffer[0] >> 4;
				if (low == 15) low = 14;
				else if (low == 14) low = 1;
				else if (low >= 11) low = 0;
				else if (low >= 8) low += 3;
				else if (low >= 5) low -= 3;
				else low = 0;
				if (high == 15) high = 14;
				else if (high == 14) high = 1;
				else if (high >= 11) high = 0;
				else if (high >= 8) high += 3;
				else if (high >= 5) high -= 3;
				else high = 0;
				*buffer++ = TUint8((high << 4) | low);
				}
			break;
		case EColor256:
			while (buffer < bufferLimit)
				{
				TRgb color(TRgb::Color256(buffer[0]));
				TInt red = color.Red();
				TInt green = color.Green();
				TInt blue = color.Blue();
				red = Max(0,red-0x33);
				green = Max(0,green-0x33);
				blue = Max(0,blue-0x33);
				*buffer++ = TUint8(TRgb(red,green,blue).Color256());
				}
			break;
		case EColor4K:
			while (buffer < bufferLimit)
				{
				TInt data = (buffer[1] << 8) | buffer[0];
				TInt red = (data & 0xf00) >> 8;
				TInt green = (data & 0x0f0) >> 4;
				TInt blue = data & 0x00f;
				red = Max(0,red-5);
				green = Max(0,green-5);
				blue = Max(0,blue-5);
				data = (red << 8) | (green << 4) | blue;
				buffer[0] = TUint8(data);
				buffer[1] = TUint8(data >> 8);
				buffer += 2;
				}
			break;
		case EColor64K:
			while (buffer < bufferLimit)
				{
				TInt data = (buffer[1] << 8) | buffer[0];
				TInt red = (data & 0xf800) >> 11;
				TInt green = (data & 0x07e0) >> 5;
				TInt blue = data & 0x001f;
				red = Max(0,red-8);
				green = Max(0,green-16);
				blue = Max(0,blue-8);
				data = (red << 11) | (green << 5) | blue;
				buffer[0] = TUint8(data);
				buffer[1] = TUint8(data >> 8);
				buffer += 2;
				}
			break;
		case EColor16M:
			while (buffer < bufferLimit)
				{
				buffer[0] = TUint8(Max(0,buffer[0]-0x40));
				buffer++;
				}
			break;
		case EColor16MU:
			{
			TUint32* buffer32 = reinterpret_cast <TUint32*> (buffer);
			TUint32* bufferLimit32 = buffer32 + aByteLength / 4;
			while (buffer32 < bufferLimit32)
				{
				// scanline buffer for 16MU driver is pre-multiplied
				TRgb color = TRgb::Color16MAP(*buffer32);
				color = TRgb(Max(0,color.Red()-0x40),Max(0,color.Green()-0x40),Max(0,color.Blue()-0x40), color.Alpha());
				// avoid rounding errors with EDrawModeAND etc.
				*buffer32++ = color.Alpha() == 255 ? color.Internal() : color.Color16MAP();
				}
			}
			break;
		case EColor16MA:
			{
			TUint32* buffer32 = reinterpret_cast <TUint32*> (buffer);
			TUint32* bufferLimit32 = buffer32 + aByteLength / 4;
			while (buffer32 < bufferLimit32)
				{
				TRgb color = TRgb::Color16MA(*buffer32);
				color = TRgb(Max(0,color.Red()-0x40),Max(0,color.Green()-0x40),Max(0,color.Blue()-0x40), color.Alpha());
				*buffer32++ = color.Color16MA();
				}
			}
			break;
		case EColor16MAP:
			{
			TUint32* buffer32 = reinterpret_cast <TUint32*> (buffer);
			TUint32* bufferLimit32 = buffer32 + aByteLength / 4;
			while (buffer32 < bufferLimit32)
				{
				TRgb color = TRgb::Color16MAP(*buffer32);
				color = TRgb(Max(0,color.Red()-0x40),Max(0,color.Green()-0x40),Max(0,color.Blue()-0x40), color.Alpha());
				*buffer32++ = color.Color16MAP();
				}
			}
			break;
		default:
			break;
			}
		}
	}

TUint8 CTLowLevel::FadeGray(TInt aGray256)
	{
	return TUint8((aGray256 >> 1) + 128);
	}

/**
A test code function for Fading colour values.
@param aColor Colour value for which faded colour is needed.
@param aFastFade Used to check whether Fast Fading method is required or not.
		aFastFade flag should be true only when there is a corresponding Fast Fading
		implementation in product code and the fading method uses the Fast Fading method.
@return TRgb Faded colour value.
*/
TRgb CTLowLevel::FadeRgb(TRgb aColor, TBool aFastFade/*=EFalse*/)
	{
	if(aFastFade)
		{
#if defined(SYMBIAN_USE_FAST_FADING)
		TUint32 value = ((aColor.Internal() >> 1) & ~0x00808080) + SYMBIAN_USE_FAST_FADING;
		TInt alpha = aColor.Alpha();
		return TRgb(value, alpha);
#endif
		}
	TInt red = (aColor.Red() >> 1) + 128;
	TInt green = (aColor.Green() >> 1) + 128;
	TInt blue = (aColor.Blue() >> 1) + 128;
	TInt alpha = aColor.Alpha();
	return TRgb(red,green,blue,alpha);
	}

TColorConvertor& CTLowLevel::ColorConvertor(TDisplayMode aDisplayMode)
	{
	return *iColorConvertor[aDisplayMode];
	}

void CTLowLevel::Report()
	{
	INFO_PRINTF4(_L("Test %d: %d/%d"),iTestNo,iReportIteration,iTotalReportIterations);
	if (iReportIteration < iTotalReportIterations)
		iReportIteration++;
	}

inline TBool CTLowLevel::Check(TBool aValue)
	{
	if (iLastFailTestNo!=iIteration)
		{
		if (!aValue)
			{
			_LIT(KLog,"Test %d, iteration %d failed  iDispMode %d  iUserDispMode %d  iOrientation %d");
			INFO_PRINTF6(KLog,iTestNo,iIteration,iDispMode,iUserDispMode,iOrientation);
			iLastFailTestNo=iIteration;
			}
		TEST(aValue);
		}
	return !aValue;
	}

//-----------
CTLowLevel1::CTLowLevel1(CTestStep* aStep):
	CTLowLevel(aStep)
	{
	iOrientation = CFbsDrawDevice::EOrientationRotated180;
	iOrientationEnd = CFbsDrawDevice::EOrientationRotated270;
	}

void CTLowLevel1::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	if(iOrientation <= iOrientationEnd)
		{
		INFO_PRINTF2(_L("Screen device : %S"), &DisplayModeNames1[iCurScreenDeviceModeIndex]);
		TDisplayMode display = TestDisplayMode1[iCurScreenDeviceModeIndex++];
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0018
*/
		((CTLowLevelStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0018"));
		TestScreenDrawL(display);
		if(iCurScreenDeviceModeIndex >= KNumberDisplayModes1)
			{
			iCurScreenDeviceModeIndex = 0;
			iOrientation ++;
			}
		((CTLowLevelStep*)iStep)->RecordTestResultL();
		}
	else
		{
		((CTLowLevelStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		}
	}

//--------------
__CONSTRUCT_STEP__(LowLevel)

__CONSTRUCT_STEP__(LowLevel1)
