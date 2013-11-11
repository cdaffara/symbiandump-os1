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

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_DeviceSuite.h"
#include "TSU_MMF_Loop.h"

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

//[ Codec Unit tests  structure 
// The unit tests shall use text files
// for small portions of test data 
// which should be stored are stored in a simple format
// containing the relevant parameters for the test
//]
class TLoopTestParams
	{	
public:
	const TText* iTestName;       // name of the test
	};

// constant table of parameters for tests
const TLoopTestParams KLoopParameters[] =
	{	
		{_S("MM-MMF-SWCODECDEVICES-U-0061-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0062-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0063-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0064-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0065-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0066-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0067-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0068-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0069-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0070-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0071-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0072-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0073-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0074-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0075-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0076-HP")}
	};

/**
*
* CTestStepLoopTest
*
*/
template <class Codec>
CTestStepLoopTest<Codec>::CTestStepLoopTest( TUint aTestIndex )
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = (&KLoopParameters[aTestIndex])->iTestName;
	}

/**
*
* ~CTestStepLoopTest
*
*/
template <class Codec>
CTestStepLoopTest<Codec>::~CTestStepLoopTest()
	{
	}

/**
*
* DoTestStepL
* This method tests codec src & destination buffer size violations
* and position violations.
*
**/
template <class Codec>
TVerdict CTestStepLoopTest<Codec>::DoTestStepL()
	{
	__MM_HEAP_MARK;	
	TVerdict result = EPass;

	//[ Buffer size violation on the src ]
	const TInt KSrcDataBufferSize  = iCodecUnderTest->SourceBufferSize();
	const TInt KSinkDataBufferSize = iCodecUnderTest->SinkBufferSize();

	//[ create the data buffers and do not worry about filling them since
	// this is simply a precondition test ]
	TInt srcDataBufferSize  = KSrcDataBufferSize*2;
	TInt sinkDataBufferSize = KSinkDataBufferSize;
	if( CodecPreconditionTestL( srcDataBufferSize, sinkDataBufferSize ) == KErrNone )
		{
        //[ info print here ]
		INFO_PRINTF1(_L("Src Data Buffer precondition violation"));
		result = EFail;
		}

	//[ Buffer size violation on the Destination ]
	srcDataBufferSize  = KSrcDataBufferSize;
	sinkDataBufferSize = KSinkDataBufferSize/4;
	if( CodecPreconditionTestL( srcDataBufferSize, sinkDataBufferSize ) == KErrNone )
		{
       	INFO_PRINTF1(_L("Sink Data Buffer precondition violation"));
		result = EFail;
		}

	//[ Position violation on the src ]
	srcDataBufferSize  = KSrcDataBufferSize;
	sinkDataBufferSize = KSinkDataBufferSize;
	if( CodecPreconditionTestL( srcDataBufferSize, sinkDataBufferSize, srcDataBufferSize/2, 0 ) == KErrNone )
		{
        INFO_PRINTF1(_L("Src Data Buffer position precondition violation"));
		result = EFail;
		}

	//[ Position violation on the Destination ]
	srcDataBufferSize  = KSrcDataBufferSize;
	sinkDataBufferSize = KSinkDataBufferSize;
	if( CodecPreconditionTestL( srcDataBufferSize, sinkDataBufferSize,0, sinkDataBufferSize/2 ) == KErrNone )
		{
        INFO_PRINTF1(_L("Sink Data Buffer position precondition violation"));
		result = EFail;
		}

	delete iSourceData;
	delete iCodedData ;
	iSourceData = NULL;
	iCodedData   = NULL;

	__MM_HEAP_MARKEND;
	return result;
	}

/**
*
* CodecPreconditionTest
* @precondition 0<= aSrcPosition  < aSrcDataBufferSize
* @precondition 0<= aSinkPosition < aSinkDataBufferSize
* @return TInt error code 
**/
template <class Codec>
TInt CTestStepLoopTest<Codec>::CodecPreconditionTestL( TInt aSrcDataBufferSize, TInt aSinkDataBufferSize, TInt aSrcPosition, TInt aSinkPosition )
	{
	TInt result = KErrNone;
	//[ assert preconditions ]
	ASSERT( (0 <= aSrcPosition ) && ( aSrcPosition < aSrcDataBufferSize ));
	ASSERT( (0 <= aSinkPosition ) && ( aSinkPosition < aSinkDataBufferSize ));

	delete iSourceData;
	delete iCodedData ;
	iSourceData = NULL;
	iCodedData   = NULL;
	iSourceData = CMMFDescriptorBuffer::NewL(aSrcDataBufferSize * sizeof(TUint8));
	iCodedData   = CMMFDescriptorBuffer::NewL(aSinkDataBufferSize * sizeof(TUint8));
    //set the lengths of the buffers
	iSourceData->Data().SetLength(aSrcDataBufferSize);
	iCodedData->Data().SetLength(aSinkDataBufferSize);
	
	//set the positions on the buffers	
	iSourceData->SetPosition(aSrcPosition);
	iCodedData->SetPosition(aSinkPosition);

	TRAP(result,iCodecUnderTest->ProcessL(*iSourceData, *iCodedData));

	return result;
	}

/**
*
* DoTestStepPreambleL
*
**/
template <class Codec>
TVerdict CTestStepLoopTest<Codec>::DoTestStepPreambleL(void)
	{
	iCodecUnderTest = new (ELeave) Codec;
	return EPass;
	}

/**
*
* DoTestStepPostambleL
*
*/
template <class Codec>
TVerdict CTestStepLoopTest<Codec>::DoTestStepPostambleL(void)
	{
	delete iCodecUnderTest;
	iCodecUnderTest = NULL;
	delete 	iSourceData;
	iSourceData = NULL;
	delete  iCodedData;
	iCodedData = NULL;
	return EPass;
	}

/**
*
* This is used for template instantiation.
*
**/

template class CTestStepLoopTest<CMMFPcm16ToALawCodec>;
template class CTestStepLoopTest<CMMFAlawToPcm16Codec>;
template class CTestStepLoopTest<CMMFGsm610ToPcm16Codec>;
template class CTestStepLoopTest<CMMFImaAdpcmToPcm16Codec>;
template class CTestStepLoopTest<CMMFMulawToPcm16Codec>;
template class CTestStepLoopTest<CMMFPcm16SwapEndianCodec>;
template class CTestStepLoopTest<CMMFPcm16ToImaAdpcmCodec>;
template class CTestStepLoopTest<CMMFPcm16ToMuLawCodec>;
template class CTestStepLoopTest<CMMFPcm16ToPcmU16BECodec>;
template class CTestStepLoopTest<CMMFPcm16ToPcmU8Codec>;
template class CTestStepLoopTest<CMMFPcmS16ToPcmS8Codec>;
template class CTestStepLoopTest<CMMFPcm16ToPcmU16CodecCodec>;
template class CTestStepLoopTest<CMMFPcm8ToPcm16Codec>;
template class CTestStepLoopTest<CMMFPcmU16BeToPcmS16Codec>;
template class CTestStepLoopTest<CMMFPcm16ToGsm610Codec>;

