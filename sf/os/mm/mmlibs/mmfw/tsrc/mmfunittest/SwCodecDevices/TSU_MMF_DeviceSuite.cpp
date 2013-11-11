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
// TSU_MMF_SwCodecDevicesSuite.cpp
// This main DLL entry point for the TSU_MMF_SWCODECDEVICES.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_DeviceSuite.h"
#include "TSU_MMF_SWCODECDEVICES.h"
#include "tsu_mmf_CodecTests.h"
#include "TSU_MMF_SignConversionCodecs.h"
#include "TSU_MMF_Loop.h"
#include "TSU_MMF_Oom.h"
//[ actual codec includes ]
#include "MmfPcm16toAlawhwDevice.h"
#include "MmfALawToPcm16HwDevice.h"
#include "GSM610.H"
#include "MmfImaAdpcmtopcm16hwdevice.h"
#include "MmfMuLawToPcm16hwDevice.h"
#include "mmfpcm16SwapEndianhwdevice.h"
#include "mmfpcm16ToImaAdpcm.h"
#include "mmfpcm16toMulawhwdevice.h"
#include "mmfpcm16topcmU16BEHwDevice.h"
#include "MMFpcm16ToPcmU8HwDevice.h"
#include "MMFpcm16ToPcmU8HwDevice.h"
#include "mmfpcmS16PcmS8HwDevice.h"
#include "mmfpcmS16topcmU16Codec.h"
#include "mmfpcmS8ToPcmS16HwDevice.h"
#include "mmfpcmU16BeToPcmS16HwDevice.h"
#include "mmfpcmU16TopcmS16HwDevice.h"
#include "MMFpcmU8ToPcm16HwDevice.h"


/** 
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite_MMF_SwCodecDevices* NewTestSuiteL() 
    { 
	CTestSuite_MMF_SwCodecDevices* result = new (ELeave) CTestSuite_MMF_SwCodecDevices;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(1, result); // result
	return result;
    }

//destructor
CTestSuite_MMF_SwCodecDevices::~CTestSuite_MMF_SwCodecDevices()
	{
	}

//Get Test Suite version
_LIT(KTxtVersion,"0.1");
TPtrC CTestSuite_MMF_SwCodecDevices::GetVersion( void )
	{
	return KTxtVersion();
	}

/** 
 *
 * Add a test step into the suite
 * @param	"CTestStep_MMF_SwCodecDevices* aPtrTestStep" 
 *			Test step pointer
 */
void CTestSuite_MMF_SwCodecDevices::AddTestStepL( CTestStep_MMF_SwCodecDevices* aPtrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetSuite(this);       
	// add the step using tyhe base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}

/**
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestSuite_MMF_SWCODECDEVICES 
 */
void CTestSuite_MMF_SwCodecDevices::InitialiseL( void )
	{
	// store the name of this test suite 
	InitSystemPath();
	iSuiteName = _L("TSU_MMF_SWCODECDEVICES");

	AddTestStepL( new(ELeave) CTest_MMF_SwCodecDevices_U_0001() );
	AddTestStepL( new(ELeave) CTest_MMF_SwCodecDevices_U_0002() );	
	AddTestStepL( new(ELeave) CTest_MMF_SwCodecDevices_U_0003() );	
	AddTestStepL( new(ELeave) CTestALawCodec_U_0004());
	AddTestStepL( new(ELeave) CTestMuLawCodec_U_0006());

	AddTestStepL( new(ELeave) CTestStepCodecUnitTest<CMMFPcm16SwapEndianCodec,TComparator,0,1,1>(0));
	AddTestStepL( new(ELeave) CTestStepCodecUnitTest<CMMFPcm16ToImaAdpcmCodec,TComparator>(1) );
	AddTestStepL( new(ELeave) CTestStepCodecUnitTest<CMMFImaAdpcmToPcm16Codec,TDbComparator<>,0,0,1>(2));
	AddTestStepL( new(ELeave) CTestStepCodecUnitTest<CMMFPcm16ToPcmU8Codec,TComparator>(3));  
	AddTestStepL( new(ELeave) CTestStepCodecUnitTest<CMMFPcmU8ToPcm16Codec,TDbComparator<>,0,0,1>(4));  
	AddTestStepL( new(ELeave) CTestStepCodecUnitTest<CMMFPcm16ToPcmU8Codec,TComparator>(5));
	
	AddTestStepL( new(ELeave) CTestStepSignConversionTest<CMMFPcm16ToPcmU8Codec,CMMFPcm16ToPcmU8Generator,0>());
    AddTestStepL( new(ELeave) CTestStepSignConversionTest<CMMFPcm8ToPcm16Codec,CMMFPcm8ToPcm16Generator,1>());
    AddTestStepL( new(ELeave) CTestStepSignConversionTest<CMMFPcm16SwapEndianCodec,CMMFPcmU16toPcmU16BGenerator,2>());
    AddTestStepL( new(ELeave) CTestStepSignConversionTest<CMMFPcm16SwapEndianCodec,CMMFPcmU16BtoPcmU16Generator,3>());
	AddTestStepL( new(ELeave) CTestStepSignConversionTest<CMMFPcmU8ToPcm16Codec,CMMFPcmU8ToPcm16Generator,4>());

	AddTestStepL( new(ELeave) CTestIMaadCodec());

	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcm16ToALawCodec>(0));
    AddTestStepL( new(ELeave) CTestStepOomTest<CMMFAlawToPcm16Codec>(1));
	AddTestStepL( new(ELeave) CTestStepGsm610ToPcm16OomTest(2));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFImaAdpcmToPcm16Codec>(3));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFMulawToPcm16Codec>(4));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcm16SwapEndianCodec>(5));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcm16ToImaAdpcmCodec>(6));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcm16ToMuLawCodec>(7));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcm16ToPcmU16BECodec>(8));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcm16ToPcmU8Codec>(9));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcmS16ToPcmS8Codec>(10));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcm16ToPcmU16CodecCodec>(11));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcm8ToPcm16Codec>(12));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcmU16BeToPcmS16Codec>(13));
	AddTestStepL( new(ELeave) CTestStepOomTest<CMMFPcm16ToPcmU16CodecCodec>(14));
	AddTestStepL( new(ELeave) CTestStepPcm16ToGsm610OomTest(15));
	
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcm16ToALawCodec>(0));
    AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFAlawToPcm16Codec>(1));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFGsm610ToPcm16Codec>(2));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFImaAdpcmToPcm16Codec>(3));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFMulawToPcm16Codec>(4));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcm16SwapEndianCodec>(5));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcm16ToImaAdpcmCodec>(6));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcm16ToMuLawCodec>(7));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcm16ToPcmU16BECodec>(8));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcm16ToPcmU8Codec>(9));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcmS16ToPcmS8Codec>(10));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcm16ToPcmU16CodecCodec>(11));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcm8ToPcm16Codec>(12));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcmU16BeToPcmS16Codec>(13));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcm16ToPcmU16CodecCodec>(14));
	AddTestStepL( new(ELeave) CTestStepLoopTest<CMMFPcm16ToGsm610Codec>(15));
	
	}

/**
*
* InitSystemPath
* This sets up the default path
* 
*/
void CTestSuite_MMF_SwCodecDevices::InitSystemPath()
	{
	_LIT(KAudioStremOutSourcePathOnC,"c:\\mm\\mmf\\testfiles\\SwCodecDevices\\");

	RFs fs;
	TUint att;
	TInt ret = fs.Connect();
	__ASSERT_ALWAYS(ret == KErrNone,User::Panic(_L("Sys path not setup"),ret));

	if (fs.Att(KAudioStremOutSourcePathOnC, att) == KErrNone)
		iDefaultPath = KAudioStremOutSourcePathOnC;
	else
		User::Panic(_L("Test files not found"),KErrNotFound);

	fs.Close();
	}

const TDesC& CTestSuite_MMF_SwCodecDevices::DefaultPath() const
	{
	return iDefaultPath;
	}
