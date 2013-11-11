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
#include <mmf/common/mmfpaniccodes.h>

// Test system includes
#include "TSU_MMF_DeviceSuite.h"
#include "TSU_MMF_Oom.h"

//[ actual codec includes ]
#include "MmfPcm16toAlawhwDevice.h"
#include "MmfALawToPcm16HwDevice.h"
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
class TOomTestParams
	{	
public:
	const TText* iTestName;       // name of the test
	};

// constant table of parameters for tests
const TOomTestParams KOomParameters[] =
	{	
		{_S("MM-MMF-SWCODECDEVICES-U-0041-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0042-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0043-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0044-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0045-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0046-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0047-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0048-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0049-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0050-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0051-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0052-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0053-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0054-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0055-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0056-HP")}
	};

/**
*
* CTestStepOomTest
*
*/
template <class Codec>
CTestStepOomTest<Codec>::CTestStepOomTest( TUint aTestIndex )
	{
	// store the name of this test case
	// this is the name that is used by the script file
//	__ASSERT_DEBUG( (aTestIndex >= 0) && ( aTestIndex < sizeof(KOomParameters)/sizeof(TOomTestParams)),Panic(EBadInvariant));
	__ASSERT_DEBUG( ( aTestIndex < sizeof(KOomParameters)/sizeof(TOomTestParams)),Panic(EBadInvariant));	// EABI warning removal
	iTestStepName = (&KOomParameters[aTestIndex])->iTestName;
	}

/**
*
* ~CTestStepOomTest
*
*/
template <class Codec>
CTestStepOomTest<Codec>::~CTestStepOomTest()
	{
	}

/**
*
* DoTestStepL
*
**/
template <class Codec>
TVerdict CTestStepOomTest<Codec>::DoTestStepL()
	{
	TVerdict result = EPass;

	TBool testOK = ETrue;
	TUint failCount = 1;
	TBool testComplete = EFalse;

	while (!testComplete)
		{
		__UHEAP_FAILNEXT(failCount);
		__MM_HEAP_MARK;
		TRAPD(err, Oom());
		switch (err)
			{
			case KErrNoMemory :
				break;
			case KErrNone :
				{
				//attempt to allocate another cell. If this fails, we have tested all 
				//of the memory allocs carried out by ProcessL.
				TAny *testAlloc = User::Alloc(1);
				if (testAlloc==NULL)
					{
					testComplete = ETrue;
					failCount --;
					}
				else
					User::Free(testAlloc);
				break;
				}
			default:
				{
				testOK = EFalse;
				testComplete = ETrue;
				break;
				}
			}
		
		__MM_HEAP_MARKEND;
		__UHEAP_RESET;
		failCount++;
		}

	if(!testOK)
		{
		result = EFail;
		}

	return result;
	}

/**
*
* Oom performs the oom test on the codec in question
* @param srcBuffer
* @param dstBuffer 
* @return TInt error status
*
**/
template <class Codec>
TInt CTestStepOomTest<Codec>::Oom()
	{
	//instantiate codec via a factory function which allows 
	//two phase construction if needed as in the case of gsm610
	// by derivation.
	Codec* theCodec = GetCodecL(); 
	CleanupStack::PushL(theCodec);
	
	//create buffers
	TInt srcBufferSize  = theCodec->SourceBufferSize();
	TInt sinkBufferSize = theCodec->SinkBufferSize();
	CMMFDataBuffer *srcBuffer = CMMFDataBuffer::NewL(srcBufferSize);
	srcBuffer->Data().SetLength( srcBufferSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer *dstBuffer = CMMFDataBuffer::NewL(sinkBufferSize);
	dstBuffer->Data().SetLength( sinkBufferSize );
	CleanupStack::PushL(dstBuffer);
    
	//[perform the coding operation]
	theCodec->ProcessL(*srcBuffer, *dstBuffer);

	CleanupStack::PopAndDestroy(3,theCodec); //theCodec,dstBuffer, srcBuffer
	return KErrNone;
	}

/**
*
* GetCodec()
* @return fully contrsucted codec pointer
* note the client of this function is responsible for deallocation
*
**/
template <class Codec>
Codec* CTestStepOomTest<Codec>::GetCodecL()
	{
	return new(ELeave) Codec();
	}


/**
*
* DoTestStepPreambleL
*
**/
template <class Codec>
TVerdict CTestStepOomTest<Codec>::DoTestStepPreambleL(void)
	{
	return EPass;
	}

/**
*
* DoTestStepPostambleL
*
*/
template <class Codec>
TVerdict CTestStepOomTest<Codec>::DoTestStepPostambleL(void)
	{
	return EPass;
	}

/**
*
* GetCodecL()
* @return CMMFGsm610ToPcm16Codec*
*
**/
CMMFGsm610ToPcm16Codec* CTestStepGsm610ToPcm16OomTest::GetCodecL()
		{
		CMMFGsm610ToPcm16Codec* theCodec = new (ELeave) CMMFGsm610ToPcm16Codec;
		CleanupStack::PushL(theCodec);
		theCodec->ConstructL();
		CleanupStack::Pop(1); // theCodec
		return theCodec;
		}
/**
*
* GetCodecL
* @return CMMFGsm610ToPcm16Codec*
*
**/
CMMFGsm610ToPcm16Codec* CTestStepPcm16ToGsm610OomTest::GetCodecL()
	{
	CMMFGsm610ToPcm16Codec* theCodec = new (ELeave) CMMFGsm610ToPcm16Codec();
	CleanupStack::PushL(theCodec);
	theCodec->ConstructL();
	CleanupStack::Pop(1); // theCodec
	return theCodec;	
	}
 
/**
*
* This is used for template instantiation.
*
* Normally templated classes are defined in header files..
* Here we use them in *.cpp which requires an explicit instantiation.
* We know that any other reference to CTestStepOomTest<class> within
* this dll will instantiate the template and we do not want the compiler
* to remind us about that.
*
**/

#ifdef __VC32__
#pragma warning( disable : 4660 )	
#endif

template class CTestStepOomTest<CMMFPcm16ToALawCodec>;
template class CTestStepOomTest<CMMFAlawToPcm16Codec>;
template class CTestStepOomTest<CMMFGsm610ToPcm16Codec>;
template class CTestStepOomTest<CMMFImaAdpcmToPcm16Codec>;
template class CTestStepOomTest<CMMFMulawToPcm16Codec>;
template class CTestStepOomTest<CMMFPcm16SwapEndianCodec>;
template class CTestStepOomTest<CMMFPcm16ToImaAdpcmCodec>;
template class CTestStepOomTest<CMMFPcm16ToMuLawCodec>;
template class CTestStepOomTest<CMMFPcm16ToPcmU16BECodec>;
template class CTestStepOomTest<CMMFPcm16ToPcmU8Codec>;
template class CTestStepOomTest<CMMFPcmS16ToPcmS8Codec>;
template class CTestStepOomTest<CMMFPcm16ToPcmU16CodecCodec>;
template class CTestStepOomTest<CMMFPcm8ToPcm16Codec>;
template class CTestStepOomTest<CMMFPcmU16BeToPcmS16Codec>;

#ifdef __VC32__
#pragma warning( default : 4660 )
#endif
