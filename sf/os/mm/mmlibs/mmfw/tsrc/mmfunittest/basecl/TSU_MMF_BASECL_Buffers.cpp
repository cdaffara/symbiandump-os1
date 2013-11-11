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

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_BASECL_Buffers.h"
#include "TSU_MMF_BASECLSuite.h"

//tests 0001 - 0121

CTestStep_MMF_BASECL_U_0001::CTestStep_MMF_BASECL_U_0001()
/** Constructor
 */
 {
 iTestStepName = _L("MM-MMF-BASECL-U-0001-HP");
 }
 

TVerdict CTestStep_MMF_BASECL_U_0001::DoTestStepL( void )
/** Verify CMMFDescriptorBuffer creation and deletion using default buffer size
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
	 {
	 TVerdict verdict = EPass;
 
	 CMMFDescriptorBuffer* descriptorBuffer = NULL;
	 CMMFDataBuffer* dataBuffer = NULL;

	 descriptorBuffer = CMMFDescriptorBuffer::NewL(); //create descriptor buffer
	 dataBuffer = CMMFDataBuffer::NewL(); //create descriptor buffer from databuffer NewL
	 //check buffer really is a CMMFDescriptorBuffer
	 if (descriptorBuffer->Type() != KUidMmfDescriptorBuffer) verdict = EFail;
	 if (dataBuffer->Type() != KUidMmfDescriptorBuffer) verdict = EFail;

	 //check default length is correct
	 if (descriptorBuffer->Data().MaxLength() != KMMFDataBufferDefaultBufferSize) verdict = EFail;
	 if (dataBuffer->Data().MaxLength() != KMMFDataBufferDefaultBufferSize) verdict = EFail;

	 //check default length is 0
	 if (descriptorBuffer->Data().Length() != 0) verdict = EFail; //test using descriptor API
	 if (dataBuffer->Data().Length() != 0) verdict = EFail;
	 if (descriptorBuffer->BufferSize() != 0) verdict = EFail; //test using CMMFBuffer API
	 if (dataBuffer->BufferSize() != 0) verdict = EFail;

	 delete descriptorBuffer;
	 delete dataBuffer;
 
	 return verdict ;
	 }


CTestStep_MMF_BASECL_U_0002::CTestStep_MMF_BASECL_U_0002()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0002-HP");
	 }
 

TVerdict CTestStep_MMF_BASECL_U_0002::DoTestStepL( void )
/** Verify CMMFDescriptorBuffer creation and deletion using a supplied buffer size
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
	 {
	 TVerdict verdict = EPass;
	 CMMFDescriptorBuffer* descriptorBuffer;
	 CMMFDataBuffer* dataBuffer;

	 descriptorBuffer = CMMFDescriptorBuffer::NewL(KMMFTestBufferSize); //create descriptor buffer
	 dataBuffer = CMMFDataBuffer::NewL(KMMFTestBufferSize); //create descriptor buffer from databuffer NewL

	 //check buffer really is a CMMFDescriptorBuffer
	 if (descriptorBuffer->Type() != KUidMmfDescriptorBuffer) verdict = EFail;
	 if (dataBuffer->Type() != KUidMmfDescriptorBuffer) verdict = EFail;

	 //check max length is correct
	 if (descriptorBuffer->Data().MaxLength() != (TInt)KMMFTestBufferSize) verdict = EFail;
	 if (dataBuffer->Data().MaxLength() != (TInt)KMMFTestBufferSize) verdict = EFail;

	 //check actual length is 0
	 if (descriptorBuffer->Data().Length() != 0) verdict = EFail; //test using descriptor API
	 if (dataBuffer->Data().Length() != 0) verdict = EFail;
	 if (descriptorBuffer->BufferSize() != 0) verdict = EFail; //test using CMMFBuffer API
	 if (dataBuffer->BufferSize() != 0) verdict = EFail;

	 delete descriptorBuffer;
	 delete dataBuffer;
	 return verdict ;
	 }


CTestStep_MMF_BASECL_U_0003::CTestStep_MMF_BASECL_U_0003()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0003-HP");
	 }
 
/** Verify CMMFDescriptorBuffer creation and deletion using awkward buffer sizes
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0003::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFDescriptorBuffer* zeroSizeBuffer;

	 zeroSizeBuffer = CMMFDescriptorBuffer::NewL(0); //create descriptor buffer

	 //check default length is correct
	 if (!zeroSizeBuffer) verdict = EFail; //should still create 0 size buffer - can be realloced later
	 else 
		 {
		 if (zeroSizeBuffer->Data().MaxLength() != 0) verdict = EFail;
		 delete zeroSizeBuffer;
		 }
 
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0004::CTestStep_MMF_BASECL_U_0004()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0004-HP");
	 }
 
/** Verify CMMFDescriptorBuffer buffer size reallocation
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0004::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 CMMFDescriptorBuffer* descriptorBuffer = CMMFDescriptorBuffer::NewL(); //create descriptor buffer

	 descriptorBuffer->ReAllocBufferL(KMMFTestBufferSize);

	 if (descriptorBuffer->Data().MaxLength() != (TInt)KMMFTestBufferSize) verdict = EFail;
	 if (descriptorBuffer->BufferSize() != 0) verdict = EFail; //test using CMMFBuffer API - note actual length still 0

	 delete descriptorBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0005::CTestStep_MMF_BASECL_U_0005()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0005-HP");
	 }
 
/** Verify CMMFDataBuffer buffer length setting
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0005::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 CMMFDataBuffer* descriptorBuffer = CMMFDataBuffer::NewL(KMMFTestBufferSize); //create descriptor buffer

	 descriptorBuffer->Data().SetLength(KMMFTestBufferLengthSize);
	 if (descriptorBuffer->BufferSize() != KMMFTestBufferLengthSize) verdict = EFail;

	 delete descriptorBuffer;
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0006::CTestStep_MMF_BASECL_U_0006()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0006-HP");
	 }
 
/** Verify CMMFBuffer buffer stats
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0006::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFDataBuffer* descriptorBuffer = CMMFDataBuffer::NewL(); //create descriptor buffer

	 //check that the buffer is initially available
	 if (descriptorBuffer->Status() != EAvailable) verdict = EFail;

	 //check we can write to the status - onlt try one option assume if one does the others do -since this
	 //is an enumeration this is a reasonable assumption
	 descriptorBuffer->SetStatus(EFull);
	 if (descriptorBuffer->Status() != EFull) verdict = EFail;

	 delete descriptorBuffer;
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0007::CTestStep_MMF_BASECL_U_0007()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0007-HP");
	 }
 
/** Verify CMMFBuffer timestamping
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0007::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 const TInt KMMFTestTime = 1234567890;
	 CMMFDataBuffer* descriptorBuffer = CMMFDataBuffer::NewL(); //create descriptor buffer

	 //check that the initial time to play is 0
	 if (descriptorBuffer->TimeToPlay() != TTimeIntervalMicroSeconds(0)) verdict = EFail;

	 //assign a timestamp and check it is set correctly
	 descriptorBuffer->SetTimeToPlay(TTimeIntervalMicroSeconds(KMMFTestTime));
	 if (descriptorBuffer->TimeToPlay() != TTimeIntervalMicroSeconds(KMMFTestTime)) verdict = EFail;
 
	 delete descriptorBuffer;
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0008::CTestStep_MMF_BASECL_U_0008()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0008-HP");
	 }
 
/** Verify CMMFBuffer buffer framenumbers
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0008::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 CMMFDataBuffer* descriptorBuffer = CMMFDataBuffer::NewL(); //create descriptor buffer

	 //check initial frame number is 0
	 if (descriptorBuffer->FrameNumber() != 0) verdict = EFail;
 
	 //check setting a framenumber
	 descriptorBuffer->SetFrameNumber(KMMFTestFrameNumber);
	 if (descriptorBuffer->FrameNumber() != KMMFTestFrameNumber) verdict = EFail;

	 //check incrementing the framenumber
	 descriptorBuffer->NextFrame();
	 if (descriptorBuffer->FrameNumber() != (KMMFTestFrameNumber+1)) verdict = EFail;

	 delete descriptorBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0009::CTestStep_MMF_BASECL_U_0009()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0009-HP");
	 }
 
/** Verify CMMFBuffer & CMMFDescriptorBuffer positioning
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0009::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 CMMFDataBuffer* descriptorBuffer = CMMFDataBuffer::NewL(KMMFTestBufferSize); //create descriptor buffer

	 //check initial frame number is 0
	 if (descriptorBuffer->Position() != 0) verdict = EFail;
 
	 //check setting a framenumber with no data size - This should keep the position to 0
	 //because there is no buffer size as yet!
	 descriptorBuffer->SetPosition(KMMFTestPosition);
	 if (descriptorBuffer->Position() != 0) verdict = EFail;

	 //check set position 
	 descriptorBuffer->Data().SetLength((TInt)KMMFTestBufferLengthSize);
	 descriptorBuffer->SetPosition(KMMFTestPosition);
	 if (descriptorBuffer->Position() != KMMFTestPosition) verdict = EFail;

	 //try positioning beyond the datalength - should set position to the data length ie buffer size
	 descriptorBuffer->SetPosition(KMMFTestBufferLengthSize*2);
	 if (descriptorBuffer->Position() != descriptorBuffer->BufferSize()) verdict = EFail;

	 delete descriptorBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0010::CTestStep_MMF_BASECL_U_0010()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0010-HP");
	 }
 
/** Verify CMMFBuffer & CMMFDescriptorBuffer RequestSize/SetRequestSize
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0010::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 CMMFDataBuffer* descriptorBuffer = CMMFDataBuffer::NewL(KMMFTestBufferSize); //create descriptor buffer

	 //check initial request size is 0
	 if (descriptorBuffer->RequestSize() != 0) verdict = EFail;
 
	 //set the request size
	 TRAPD(err,descriptorBuffer->SetRequestSizeL(KMMFTestRequestSize));
	 if (err) verdict = EFail;
	 if (descriptorBuffer->RequestSize() != (TInt)KMMFTestRequestSize) verdict = EFail;

	 //set request size < 0 
	 TRAP(err,descriptorBuffer->SetRequestSizeL(-1));
	 if (err != KErrUnderflow) verdict = EFail;
	 if (descriptorBuffer->RequestSize() != (TInt)KMMFTestRequestSize) verdict = EFail;
 
	 //set request size greater to the max avail buffer size
	 TRAP(err,descriptorBuffer->SetRequestSizeL(KMMFTestBufferSize));
	 if (err) verdict = EFail;
	 if (descriptorBuffer->RequestSize() != (TInt)KMMFTestBufferSize) verdict = EFail;

	 //set request size greater than max avail buffer size
	 TRAP(err,descriptorBuffer->SetRequestSizeL(KMMFTestBufferSize+1));
	 if (err != KErrOverflow) verdict = EFail;
	 if (descriptorBuffer->RequestSize() != (TInt)KMMFTestBufferSize) verdict = EFail;

	 delete descriptorBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0011::CTestStep_MMF_BASECL_U_0011()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0011-HP");
	 }
 
/** Verify CMMFDescriptorBuffer OOM
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0011::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 __MM_HEAP_MARK;
 
	 CMMFDataBuffer* dataBuffer = NULL;

	 TInt KMaxFailRate = 100000;
	 TBool complete = EFalse;
	 TBool nullAlloc = EFalse;
	 TInt failCount;

	 for (failCount=1; !complete && failCount<KMaxFailRate; ++failCount)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAPD(err, dataBuffer = CMMFDataBuffer::NewL());
		if (err == KErrNone)
			{//need to check if next alloc fails
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				nullAlloc = ETrue;
				}
			else
				{
				User::Free(testAlloc);
				complete = ETrue;
				}
			//check a databuffer has been created
			if (!dataBuffer) 
				{
				verdict = EFail;
				complete = ETrue;
				}
			else 
				{
				delete dataBuffer;
				dataBuffer = NULL;
				}
			}
		else if (err != KErrNoMemory)
			{
			verdict = EFail; // bad error code
			complete = ETrue;
			}
		}

	 if ((failCount >= KMaxFailRate-1) || (!nullAlloc))verdict = EFail;

	 __UHEAP_RESET;

	 delete dataBuffer;

	 __MM_HEAP_MARKEND;

	 return verdict ;
	 }


CTestStep_MMF_BASECL_U_0012::CTestStep_MMF_BASECL_U_0012()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0012-HP");
	 }
 
/** Verify CMMFDescriptorBuffer ReAllocation OOM
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0012::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 __MM_HEAP_MARK;
 
	 CMMFDescriptorBuffer* dataBuffer = NULL;
	 dataBuffer = CMMFDescriptorBuffer::NewL();

	 TInt KMaxFailRate = 100000;
	 TBool complete = EFalse;
	 TBool nullAlloc = EFalse;
	 TInt failCount;

	 for (failCount=1; !complete && failCount<KMaxFailRate; ++failCount)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAPD(err, dataBuffer->ReAllocBufferL(KMMFTestBufferSize));
		if (err == KErrNone)
			{//need to ckeck if next alloc fails
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				nullAlloc = ETrue;
				}
			else
				{
				User::Free(testAlloc);
				complete = ETrue;
				}
			//check a databuffer has been created and it has the realloced size
			if ((!dataBuffer) || 
			   (dataBuffer->Data().MaxLength() != (TInt)KMMFTestBufferSize))
				{
				verdict = EFail;
				complete = ETrue;
				}
			}
		else if (err != KErrNoMemory)
			{
			verdict = EFail; // bad error code
			complete = ETrue;
			}
		}

	 if ((failCount >= KMaxFailRate-1) || (!nullAlloc)) verdict = EFail;

	 __UHEAP_RESET;
 
	 delete dataBuffer;

	 __MM_HEAP_MARKEND;

	 return verdict ;
	 }




//Tests 20-99 Reserved for future non 0 copy buffer types eg Bitmap, YUV, Audio
//Transfer buffer tests moved to a seperate file




//CMMFPtrBuffer Tests

CTestStep_MMF_BASECL_PtrBuffer::CTestStep_MMF_BASECL_PtrBuffer() : iTestData(0,0,0)
	{
	}


CTestStep_MMF_BASECL_PtrBuffer::~CTestStep_MMF_BASECL_PtrBuffer()
	{
	}

/**
 * Pre-amble for ptr buffer test step
 * Sets __MM_HEAP_MARK for memory leak detection
 */
TVerdict CTestStep_MMF_BASECL_PtrBuffer::DoTestStepPreambleL()

 {
	 __MM_HEAP_MARK;
	 TVerdict verdict = EPass;

	 return verdict;
	 }
 

/**
 * Post-amble for ptr buffer test step
 * Sets __MM_HEAP_MARKEND for memory leak detection
 */
TVerdict CTestStep_MMF_BASECL_PtrBuffer::DoTestStepPostambleL()

	{
	TVerdict verdict = EPass;

	__MM_HEAP_MARKEND;
	return verdict;
	}



CTestStep_MMF_BASECL_U_0150::CTestStep_MMF_BASECL_U_0150()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0150-LP");
	 }
 
/** Basic sanity check to verify ptr buffer creation 
 * Use case: N/A
 * @test Req. under test PREQ 10
 */
TVerdict CTestStep_MMF_BASECL_U_0150::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	 CMMFPtrBuffer* ptrBuffer = NULL;


	 ptrBuffer = CMMFPtrBuffer::NewL(); //create descriptor buffer

	 //check buffer really is a CMMFPtrBuffer
	 if (ptrBuffer->Type() != KUidMmfPtrBuffer) verdict = EFail;

	 //check default length is 0
	 if (ptrBuffer->Data().Length() != 0) verdict = EFail; //test using descriptor API

	 delete ptrBuffer;
 
	 return verdict ;
	 }


CTestStep_MMF_BASECL_U_0151::CTestStep_MMF_BASECL_U_0151()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0151-HP");
	 }
 
/** Verify CMMFPtrBuffer creation and deletion using a supplied empty TPtr8
 * Use case: N/A
 * @test Req. under test PREQ 10
 */
TVerdict CTestStep_MMF_BASECL_U_0151::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFPtrBuffer* ptrBuffer;

	 const TPtr8 testTPtr(0,0,0);

	 ptrBuffer = CMMFPtrBuffer::NewL(testTPtr); //create descriptor buffer

	 //check buffer really is a CMMFPtrBuffer
	 if (ptrBuffer->Type() != KUidMmfPtrBuffer) verdict = EFail;

	 //check actual length is 0
	 if (ptrBuffer->Data().Length() != 0) verdict = EFail; //test using descriptor API

	 if (ptrBuffer->BufferSize() != 0) verdict = EFail; //test using CMMFBuffer API


	 delete ptrBuffer;

	 return verdict ;
	 }


CTestStep_MMF_BASECL_U_0152::CTestStep_MMF_BASECL_U_0152()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0152-HP");
	 }
 
/** Verify CMMFPtrBuffer creation and deletion using a TPtr8 with Length and MaxLength
 * Use case: N/A
 * @test Req. under test PREQ 10
 */
TVerdict CTestStep_MMF_BASECL_U_0152::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFPtrBuffer* ptrBuffer;


	 TUint8 emptyBuf;
	 const TPtr8 testTPtr(&emptyBuf,KMMFTestBufferLengthSize,KMMFTestBufferSize);

	 ptrBuffer = CMMFPtrBuffer::NewL(testTPtr); //create ptr buffer

	 //check buffer really is a CMMFPtrBuffer
	 if (ptrBuffer->Type() != KUidMmfPtrBuffer) verdict = EFail;


	 //check max length is correct
	 if (ptrBuffer->Data().MaxLength() != static_cast<TInt>(KMMFTestBufferSize)) verdict = EFail;

	 //check actual length is KMMFTestBufferLengthSize
	 if (ptrBuffer->Data().Length() != static_cast<TInt>(KMMFTestBufferLengthSize)) verdict = EFail; //test using descriptor API

	 if (ptrBuffer->BufferSize() != KMMFTestBufferLengthSize) verdict = EFail; //test using CMMFBuffer API


	 delete ptrBuffer;

	 return verdict ;
	 }


CTestStep_MMF_BASECL_U_0153::CTestStep_MMF_BASECL_U_0153()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0153-HP");
	 }
 
/** Verify CMMFPtrBuffer creation and deletion using a default constructor TPtr8 with a TPtr8 set via SetPtr. Length and MaxLength verified after SetPtr
 * Use case: N/A
 * @test Req. under test PREQ 10
 */
TVerdict CTestStep_MMF_BASECL_U_0153::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFPtrBuffer* ptrBuffer;

	 TUint8 emptyBuf;
	 const TPtr8 testTPtr(&emptyBuf,KMMFTestBufferLengthSize,KMMFTestBufferSize);

	 ptrBuffer = CMMFPtrBuffer::NewL(); //create ptr buffer

	 ptrBuffer->SetPtr(testTPtr);

	 //check buffer really is a CMMFPtrBuffer
	 if (ptrBuffer->Type() != KUidMmfPtrBuffer) verdict = EFail;


	 //check max length is correct
	 if (ptrBuffer->Data().MaxLength() != static_cast<TInt>(KMMFTestBufferSize)) verdict = EFail;

	 //check actual length is KMMFTestBufferLengthSize
	 if (ptrBuffer->Data().Length() != static_cast<TInt>(KMMFTestBufferLengthSize)) verdict = EFail; //test using descriptor API

	 if (ptrBuffer->BufferSize() != KMMFTestBufferLengthSize) verdict = EFail; //test using CMMFBuffer API


	 delete ptrBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0154::CTestStep_MMF_BASECL_U_0154()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0154-HP");
	 }
 
/** Verify CMMFPtrBuffer OOM
 * Use case: N/A
 * @test Req. under test PREQ 10
 */
TVerdict CTestStep_MMF_BASECL_U_0154::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 __MM_HEAP_MARK;
 
	 CMMFPtrBuffer* ptrBuffer = NULL;

	 TInt KMaxFailRate = 100000;
	 TBool complete = EFalse;
	 TBool nullAlloc = EFalse;
	 TInt failCount;

	 for (failCount=1; !complete && failCount<KMaxFailRate; ++failCount)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAPD(err, ptrBuffer = CMMFPtrBuffer::NewL());
		if (err == KErrNone)
			{//need to check if next alloc fails
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				nullAlloc = ETrue;
				}
			else
				{
				User::Free(testAlloc);
				complete = ETrue;
				}
			//check a databuffer has been created
			if (!ptrBuffer) 
				{
				verdict = EFail;
				complete = ETrue;
				}
			else 
				{
				delete ptrBuffer;
				ptrBuffer = NULL;
				}
			}
		else if (err != KErrNoMemory)
			{
			verdict = EFail; // bad error code
			complete = ETrue;
			}
		}

	 if ((failCount >= KMaxFailRate-1) || (!nullAlloc))verdict = EFail;

	 __UHEAP_RESET;

	 delete ptrBuffer;

	 __MM_HEAP_MARKEND;

	 return verdict ;
	 }


 CTestStep_MMF_BASECL_U_0157::CTestStep_MMF_BASECL_U_0157()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0157-HP");
	 }
 
/** Verify CMMFPtrBuffer status checking
 * Use case: N/A
 * @test Req. under test PREQ 10
 */
TVerdict CTestStep_MMF_BASECL_U_0157::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 const TInt KMMFTestTime = 1234567890;
	 CMMFPtrBuffer* ptrBuffer = CMMFPtrBuffer::NewL(); //create descriptor buffer

	 //check that the initial time to play is 0
	 if (ptrBuffer->TimeToPlay() != TTimeIntervalMicroSeconds(0)) verdict = EFail;

	 //assign a timestamp and check it is set correctly
	 ptrBuffer->SetTimeToPlay(TTimeIntervalMicroSeconds(KMMFTestTime));
	 if (ptrBuffer->TimeToPlay() != TTimeIntervalMicroSeconds(KMMFTestTime)) verdict = EFail;
 
	 delete ptrBuffer;
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0159::CTestStep_MMF_BASECL_U_0159()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0159-HP");
	 }
 
/** Verify CMMFPtrBuffer frame number
 * Use case: N/A
 * @test Req. under test PREQ 10
 */
TVerdict CTestStep_MMF_BASECL_U_0159::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 CMMFPtrBuffer* mmfPtrBuffer;
 
	 mmfPtrBuffer = CMMFPtrBuffer::NewL();

	 //check initial frame number is 0
	 if (mmfPtrBuffer->FrameNumber() != 0) verdict = EFail;
 
	 //check setting a framenumber
	 mmfPtrBuffer->SetFrameNumber(KMMFTestFrameNumber);
	 if (mmfPtrBuffer->FrameNumber() != KMMFTestFrameNumber) verdict = EFail;

	 //check incrementing the framenumber
	 mmfPtrBuffer->NextFrame();
	 if (mmfPtrBuffer->FrameNumber() != (KMMFTestFrameNumber+1)) verdict = EFail;

	 delete mmfPtrBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0160::CTestStep_MMF_BASECL_U_0160()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0160-HP");
	 }
 
/** Verify CMMFPtrBuffer positioning
 * Use case: N/A
 * @test Req. under test PREQ 10
 */
TVerdict CTestStep_MMF_BASECL_U_0160::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 CMMFPtrBuffer* mmfPtrBuffer;

	 mmfPtrBuffer = CMMFPtrBuffer::NewL();

	 //check initial frame number is 0
	 if (mmfPtrBuffer->Position() != 0) verdict = EFail;
 
	 //check setting a framenumber with no data size - This should keep the position to 0
	 //because there is no buffer size as yet!
	 mmfPtrBuffer->SetPosition(KMMFTestPosition);
	 if (mmfPtrBuffer->Position() != 0) verdict = EFail;

	 //check set position 

	 mmfPtrBuffer->SetPosition(KMMFTestPosition);

	 //this should PASS as we can't set pos. on a mmfPtrBuffer with a default constructor and no SetPtr call.
	 if (mmfPtrBuffer->Position() == KMMFTestPosition) verdict = EFail;

	 delete mmfPtrBuffer;

	 return verdict;
	 }




//tests 161-199 reserved for additional transfer / ptr buffer tests
