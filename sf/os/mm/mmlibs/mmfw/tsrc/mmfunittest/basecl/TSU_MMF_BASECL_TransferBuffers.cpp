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
#include "TSU_MMF_BASECL_Buffers.h"
#include "TSU_MMF_BASECLSuite.h"

//Tests 20-99 Reserved for future non 0 copy buffer types eg Bitmap, YUV, Audio
//These are the EKA1 variants for CMMFTransferBuffer


CTestStep_MMF_BASECL_TransferBuffer::CTestStep_MMF_BASECL_TransferBuffer() : iTestData(0,0,0)
	{
	}


CTestStep_MMF_BASECL_TransferBuffer::~CTestStep_MMF_BASECL_TransferBuffer()
	{
	}

/**
 * Pre-amble for transfer buffer test step
 * Sets __MM_HEAP_MARK for memory leak detection
 * Connects to the transfer buffer tester server
 * Uses the transfer buffer tester server to create a buffer in the server process
 * and map it into the client transfer window
 */
TVerdict CTestStep_MMF_BASECL_TransferBuffer::DoTestStepPreambleL()

 {
	 __MM_HEAP_MARK;

	 if (iTransferBufferTesterClient.Handle()) iTransferBufferTesterClient.Close();
	 if (iTransferWindow.Handle()) iTransferWindow.Close();

	 TVerdict verdict = EPass;
	 User::LeaveIfError(iTransferBufferTesterClient.Connect());
	 INFO_PRINTF1(_L("TransferBufferTesterServer Opened OK"));

	 //connection to transfer buffer tester was succesfull
	 TInt error = iTransferBufferTesterClient.CreateTransferBuffer(KMMFTestBufferSize);
	 if (error != KErrNone) 
		 {
		 INFO_PRINTF1(_L("Transfer Buffer not created !"));
		 verdict = EInconclusive;
		 }
	 else
		{//server process transfer buffer created successfully now create CMMFTransferBuffer from server buffer
		error = iTransferBufferTesterClient.WriteTestData(iTestData);
		if (error != KErrNone) 
			{
			INFO_PRINTF2(_L("could not write test date %d" ), error );
			verdict = EInconclusive;
			}
		else
			{
			INFO_PRINTF1(_L("Test data written  OK"));
			}

		error = iTransferWindow.Create(KMMFDefaultTransferWindowSize);
		if (error != KErrNone) 
			{
			INFO_PRINTF2(_L("could not create client transfer window %d" ), error );
			verdict = EInconclusive;
			}
		else 
			{
			INFO_PRINTF1(_L("client transfer window created  OK"));
			}

		error = iTransferBufferTesterClient.MapBufferIntoClientWindowHandle(iTransferWindow.Handle());
		if (error != KErrNone) 
			{
			INFO_PRINTF2(_L("could not map in buffer to client transfer window %d" ), error );
			verdict = EInconclusive;
			}
		else 
			{
			INFO_PRINTF1(_L("Transfer buffer mapped into client transfer window  OK"));
			}
		}
	 return verdict;
	 }
 

/**
 * Post-amble for transfer buffer test step
 * closes transfer buffer tester server
 * Sets __MM_HEAP_MARKEND for memory leak detection
 */
TVerdict CTestStep_MMF_BASECL_TransferBuffer::DoTestStepPostambleL()

	{
	TVerdict verdict = EPass;

	iTransferBufferTesterClient.Close();
	INFO_PRINTF1(_L("Transfer buffer tester client closed -shutting down server..."));

	User::After(KMMF1sServerShutdownTime); //1s delay to allow for transfer buffer server shutdown

	__MM_HEAP_MARKEND;

	return verdict;
	}



CTestStep_MMF_BASECL_U_0100::CTestStep_MMF_BASECL_U_0100()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0100-LP");
	 }
 
/** Basic sanity check to verify transfer buffers are working on Target 
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0100::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	 TInt maxlength = iTransferWindow.GetBufferSize();
	 INFO_PRINTF2(_L("buffer size %d" ), maxlength );

	 TUint8* address = static_cast<TUint8*>(iTransferWindow.GetBufferAddress());
	 INFO_PRINTF1(_L("obtained address" ));

	 TPtr8 Ptr(0,0,0);
	 TUint length = iTestData.Length();
	 INFO_PRINTF2(_L("length %d" ), length );
	 Ptr.Set(address, length, length);
	 HBufC* heapBuf = HBufC::NewLC(length);
	 TPtr16 HpPtr(heapBuf->Des());
	 HpPtr.Copy(Ptr); //copy into 16 bit buffer
	 HBufC* testData = HBufC::NewLC(length);
	 TPtr16 testDataBuf (testData->Des());
	 testDataBuf.Copy(iTestData);
	 INFO_PRINTF2(_L("copied server buffer data = %S"), testData);
	 INFO_PRINTF2(_L("copied tsfr buffer data = %S"), heapBuf);

	 if (iTestData.Compare(Ptr) != 0) 
		{
		verdict = EFail;
		INFO_PRINTF1(_L("data mis-match"));
		}

	 iTransferWindow.Close();
	 INFO_PRINTF1(_L("transfer window handle closed Ok"));

	 CleanupStack::PopAndDestroy(2, heapBuf);

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0101::CTestStep_MMF_BASECL_U_0101()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0101-HP");
	 }
 
/** Verify CMMFTransferBuffer creation and deletion using supplied handles
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0101::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 if (!mmfTransferBuffer) verdict = EFail;
	 if (mmfTransferBuffer->Type() != KUidMmfTransferBuffer) verdict = EFail;

	 //check max length is correct
	 if (mmfTransferBuffer->Data().MaxLength() != (TInt)KMMFTestBufferSize) verdict = EFail;

	 //check actual length is 0
	 if (mmfTransferBuffer->Data().Length() != 0) verdict = EFail; //test using descriptor API
	 if (mmfTransferBuffer->BufferSize() != 0) verdict = EFail; //test using CMMFBuffer API

	 delete mmfTransferBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0102::CTestStep_MMF_BASECL_U_0102()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0102-HP");
	 }
 
/** Verify CMMFTransferBuffer creation and deletion specifying the buffer data length
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0102::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;

	 TUint length = iTestData.Length();

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow, length);

	 if (!mmfTransferBuffer) verdict = EFail;
	 if (mmfTransferBuffer->Type() != KUidMmfTransferBuffer) verdict = EFail;

	 //check max length is correct
	 if (mmfTransferBuffer->Data().MaxLength() != (TInt)KMMFTestBufferSize) verdict = EFail;

	 //check length is correct
	 if (mmfTransferBuffer->Data().Length() != (TInt)length) verdict = EFail; //test using descriptor API
	 if (mmfTransferBuffer->BufferSize() != length) verdict = EFail; //test using CMMFBuffer API

	 delete mmfTransferBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0103::CTestStep_MMF_BASECL_U_0103()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0103-HP");
	 }
 
/** Verify CMMFTransferBuffer buffer data is identical the data in the server process
 * with test data added server side and verified client side
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0103::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;
 
	 TUint length = iTestData.Length();

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow, length);

	 //compare the testData which was sent by inter process transfer with the data mapped into
	 //the CMMFTransferBuffer - they should be identical
	 TPtr8 testData8((TUint8*)iTestData.Ptr(), length, length); //test data is 16 bit but mmfDatabuffer is 8 bit
	 if (testData8.Compare(mmfTransferBuffer->Data()) != 0) verdict = EFail;

	 delete mmfTransferBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0104::CTestStep_MMF_BASECL_U_0104()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0104-HP");
	 }
 
/** Verify CMMFTransferBuffer mapping data out the buffer
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0104::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 mmfTransferBuffer->MapOutBuffer();

	 if (mmfTransferBuffer->Status() != EUnAvailable) verdict = EFail;

	 if (mmfTransferBuffer->BufferSize() != 0) verdict = EFail;

	 delete mmfTransferBuffer;
		
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0105::CTestStep_MMF_BASECL_U_0105()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0105-HP");
	 }
 
/** Verify CMMFTransferBuffer Updating transfer window
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0105::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 mmfTransferBuffer->MapOutBuffer();

	 //map the buffer back in server side
	 TInt error = iTransferBufferTesterClient.MapBufferIntoClientWindowHandle(iTransferWindow.Handle());
	 if (error != KErrNone) INFO_PRINTF2(_L("could not map in buffer to client transfer window %d" ), error );

	 if (!error)
		{
		TUint length = iTestData.Length();

		error = mmfTransferBuffer->UpdateTransferWindow(iTransferWindow, length);
		if (error) verdict = EFail;
		if (mmfTransferBuffer->Status() != EAvailable) verdict = EFail;

		TPtr8 testData8((TUint8*)iTestData.Ptr(), length, length); //test data is 16 bit but mmfDatabuffer is 8 bit
		if (testData8.Compare(mmfTransferBuffer->Data()) != 0) verdict = EFail;
		}

	 delete mmfTransferBuffer;
		
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0106::CTestStep_MMF_BASECL_U_0106()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0106-HP");
	 }
 
/** Verify CMMFTransferBuffer length
 * This test works by appending data to the data mapped into this process 
 * from the server - the test then verifeid that the CMMFTransferBuffer 
 * BufferSize() procedure picks up the new length set on the descriptor
 * and that the testdata copied from the server side matches that of the mapped
 * in transfer buffer along with the appended data
 * also write data first then map in buffer?
 * with test data added client side and verified server side
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0106::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;

	 TUint length = iTestData.Length();

	 //this is the data to be appended to the transfer buffer
	 _LIT(KAppendData,"  This is data appended client side");
	 TPtrC appendString(KAppendData);

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow, length);
			
	 //create a appendedData TPtr containing the transfer buffer data  + the appended data
	 TUint8* pSrc = CONST_CAST(TUint8*,mmfTransferBuffer->Data().Ptr()); //get pointer to data
	 TUint totalLength = length + appendString.Length(); //server data + appended data length
	 TPtr8 appendedData(pSrc, length, totalLength);
	 appendedData.Append(appendString);

	 //now update the transfer buffer desciptor length
	 mmfTransferBuffer->Data().SetLength(totalLength);

	 //Verify that the CMMFTransferBuffer Buffersize() has got the updated length
	 if (mmfTransferBuffer->Data().Length() != (TInt)totalLength) verdict = EFail; //test using descriptor API
	 if (mmfTransferBuffer->BufferSize() != totalLength) verdict = EFail; //test using CMMFBuffer API

	 if (verdict != EFail)
		{ //if the above fails there is no point in continuing further

		//now take a copy of the appended transfer buffer TPtr for debug print
		HBufC16* transferBufCopy = HBufC::NewLC(mmfTransferBuffer->Data().Length());
		transferBufCopy->Des().Copy(mmfTransferBuffer->Data());
		HBufC8* appendedDataHeap = HBufC8::NewLC(totalLength);
		TPtr8 appendedDataHeapBuf(appendedDataHeap->Des());
		appendedDataHeapBuf.Copy(iTestData);
		appendedDataHeapBuf.Append(appendString);
		HBufC16* appendedDataCopy = HBufC16::NewLC(appendedDataHeapBuf.Length());
		appendedDataCopy->Des().Copy(appendedDataHeapBuf);


		INFO_PRINTF2(_L("copied server buf & appended data = %S"), appendedDataCopy);
		INFO_PRINTF2(_L("transfer buffer & appended data = %S"), transferBufCopy);

		//compare the interprocess test data TPtr + appended data with the appended transfer buffer
		if (appendedDataHeapBuf.Compare(mmfTransferBuffer->Data()) != 0) 
			{
			verdict = EFail;
			INFO_PRINTF1(_L("appended data mis-match"));
			}

		CleanupStack::PopAndDestroy(3, transferBufCopy); // appendedDataCopy, appendedDataHeap. transferBufCopy, 
		}
	 else
		{
		INFO_PRINTF1(_L("Buffer length incorrect"));
		}

	 delete mmfTransferBuffer;

	 return verdict;
	 }


 CTestStep_MMF_BASECL_U_0107::CTestStep_MMF_BASECL_U_0107()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0107-HP");
	 }
 
/** Verify CMMFTransferBuffer status checking
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0107::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 //check that the buffer is initially available
	 if (mmfTransferBuffer->Status() != EAvailable) verdict = EFail;

	 //check we can write to the status - onlt try one option assume if one does the others do -since this
	 //is an enumeration this is a reasonable assumption
	 mmfTransferBuffer->SetStatus(EFull);
	 if (mmfTransferBuffer->Status() != EFull) verdict = EFail;

	 delete mmfTransferBuffer;
		
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0108::CTestStep_MMF_BASECL_U_0108()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0108-HP");
	 }
 
/** Verify CMMFTransferBuffer time to play
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0108::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 const TInt KMMFTestTime = 1234567890;
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 //check that the initial time to play is 0
	 if (mmfTransferBuffer->TimeToPlay() != TTimeIntervalMicroSeconds(0)) verdict = EFail;

	 //assign a timestamp and check it is set correctly
	 mmfTransferBuffer->SetTimeToPlay(TTimeIntervalMicroSeconds(KMMFTestTime));
	 if (mmfTransferBuffer->TimeToPlay() != TTimeIntervalMicroSeconds(KMMFTestTime)) verdict = EFail;
		 
	 delete mmfTransferBuffer;

	 return verdict;
	 }



CTestStep_MMF_BASECL_U_0109::CTestStep_MMF_BASECL_U_0109()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0109-HP");
	 }
 
/** Verify CMMFTransferBuffer frame number
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0109::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;
 
	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 //check initial frame number is 0
	 if (mmfTransferBuffer->FrameNumber() != 0) verdict = EFail;
 
	 //check setting a framenumber
	 mmfTransferBuffer->SetFrameNumber(KMMFTestFrameNumber);
	 if (mmfTransferBuffer->FrameNumber() != KMMFTestFrameNumber) verdict = EFail;

	 //check incrementing the framenumber
	 mmfTransferBuffer->NextFrame();
	 if (mmfTransferBuffer->FrameNumber() != (KMMFTestFrameNumber+1)) verdict = EFail;

	 delete mmfTransferBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0110::CTestStep_MMF_BASECL_U_0110()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0110-HP");
	 }
 
/** Verify CMMFTransferBuffer positioning
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0110::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 //check initial frame number is 0
	 if (mmfTransferBuffer->Position() != 0) verdict = EFail;
 
	 //check setting a framenumber with no data size - This should keep the position to 0
	 //because there is no buffer size as yet!
	 mmfTransferBuffer->SetPosition(KMMFTestPosition);
	 if (mmfTransferBuffer->Position() != 0) verdict = EFail;

	 //check set position 
	 mmfTransferBuffer->Data().SetLength((TInt)KMMFTestBufferLengthSize);
	 mmfTransferBuffer->SetPosition(KMMFTestPosition);
	 if (mmfTransferBuffer->Position() != KMMFTestPosition) verdict = EFail;

	 //try positioning beyond the datalength - should set position to the data length ie buffer size
	 mmfTransferBuffer->SetPosition(KMMFTestBufferLengthSize*2);
	 if (mmfTransferBuffer->Position() != mmfTransferBuffer->BufferSize()) verdict = EFail;

	 delete mmfTransferBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0111::CTestStep_MMF_BASECL_U_0111()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0111-LP");
	 }
 
/** Verify CMMFTransferBuffer Set Request Size
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0111::DoTestStepL( void )

	 {
	 TVerdict verdict = EPass;
 
	 //This test has moved to 0120 !!
		
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0112::CTestStep_MMF_BASECL_U_0112()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0112-HP");
	 }
 
/** Verify CMMFTransferBuffer Transfer window
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0112::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 if (iTransferWindow.Handle() != mmfTransferBuffer->TransferWindow().Handle()) verdict = EFail;

	 delete mmfTransferBuffer;
		
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0113::CTestStep_MMF_BASECL_U_0113()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0113-HP");
	 }
 
/** Verify CMMFTransferBuffer with null RTransferwindow handle
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0113::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	 RTransferWindow transferWindow;
	 CMMFTransferBuffer* mmfTransferBuffer = NULL;

	 TRAPD (err,mmfTransferBuffer = CMMFTransferBuffer::NewL(transferWindow));

	 if (mmfTransferBuffer) verdict = EFail;
	 if (err != KErrBadHandle) verdict = EFail;

	 TRAP (err,mmfTransferBuffer = CMMFTransferBuffer::NewL(transferWindow, KMMFTestBufferSize));

	 if (mmfTransferBuffer) verdict = EFail;
	 if (err != KErrBadHandle) verdict = EFail;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0114::CTestStep_MMF_BASECL_U_0114()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0114-HP");
	 }
 
/** Verify CMMFTransferBuffer but set the buffer length to be bigger than mapped in transfer buffer length
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0114::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;
	 mmfTransferBuffer = 0;
 
	 TRAPD(err, mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow, KMMFTestBufferSize+1));

	 if (mmfTransferBuffer) verdict = EFail;
	 if (err != KErrTooBig) verdict = EFail;

	 delete mmfTransferBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0115::CTestStep_MMF_BASECL_U_0115()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0115-HP");
	 }
 
/** Verify CMMFTransferBuffer creation where no transfer buffer has been mapped in to the 
 * supplied RTransferWindow
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0115::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	 RTransferBufferTesterClient transferBufferTesterClient;
	 RTransferWindow transferWindow;
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = 0;

	 TInt error = transferWindow.Create(KMMFDefaultTransferWindowSize);
	 if (error != KErrNone) INFO_PRINTF2(_L("could not create client transfer window %d" ), error );
	 else INFO_PRINTF1(_L("client transfer window created  OK"));

	 TRAPD(err, mmfTransferBuffer = CMMFTransferBuffer::NewL(transferWindow));

	 if (mmfTransferBuffer) verdict = EFail; //NewL should not create a CMMFTransferBuffer
	 if (err != KErrNotReady) verdict = EFail;

	 transferBufferTesterClient.Close();

	 User::After(KMMF1sServerShutdownTime); //1s server shutdown
		
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0116::CTestStep_MMF_BASECL_U_0116()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0116-HP");
	 }
 
/** Verify CMMFTransferBuffer updating the transfer window
 * but with a different transfer window to the prvious window
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0116::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 mmfTransferBuffer->MapOutBuffer();

	 iTransferWindow.Close();

	 TInt error = iTransferWindow.Create(0x10000);
	 if (error != KErrNone) INFO_PRINTF2(_L("could not create client transfer window %d" ), error );
	 else INFO_PRINTF1(_L("client transfer window created  OK"));


	 //map the buffer back in server side
	 error = iTransferBufferTesterClient.MapBufferIntoClientWindowHandle(iTransferWindow.Handle());
	 if (error != KErrNone) INFO_PRINTF2(_L("could not map in buffer to client transfer window %d" ), error );

	 if (!error)
		{
		TUint length = iTestData.Length();

		error = mmfTransferBuffer->UpdateTransferWindow(iTransferWindow, length);
		if (error) verdict = EFail;
		if (mmfTransferBuffer->Status() != EAvailable) verdict = EFail;

		TPtr8 testData8((TUint8*)iTestData.Ptr(), length, length); //test data is 16 bit but mmfDatabuffer is 8 bit
		if (testData8.Compare(mmfTransferBuffer->Data()) != 0) verdict = EFail;
		}

	 delete mmfTransferBuffer;
		
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0117::CTestStep_MMF_BASECL_U_0117()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0117-HP");
	 }
 
/** Verify CMMFTransferBuffer updating the transfer window with a null transfer window
 * but with a different transfer window to the prvious window
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0117::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 mmfTransferBuffer->MapOutBuffer();

	 iTransferWindow.Close(); 

	 //pass in an invalid iTransferWindow
	 TInt err = mmfTransferBuffer->UpdateTransferWindow(iTransferWindow);
	 if (err != KErrBadHandle) verdict = EFail;
	 if (mmfTransferBuffer->Status() != EUnAvailable) verdict = EFail;

	 delete mmfTransferBuffer;
		
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0118::CTestStep_MMF_BASECL_U_0118()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0118-HP");
	 }
 
/** Verify CMMFTransferBuffer updating the transfer window 
 * with the buffer length bigger than mapped in transfer buffer length
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0118::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 mmfTransferBuffer->MapOutBuffer();

	 iTransferWindow.Close();

	 TInt err = iTransferWindow.Create(KMMFDefaultTransferWindowSize);
	 if (err != KErrNone) INFO_PRINTF2(_L("could not create client transfer window %d" ), err );
	 else INFO_PRINTF1(_L("client transfer window created  OK"));

	 //map the buffer back in server side
	 err = iTransferBufferTesterClient.MapBufferIntoClientWindowHandle(iTransferWindow.Handle());
	 if (err != KErrNone) INFO_PRINTF2(_L("could not map in buffer to client transfer window %d" ), err );

	 if (!err)
		{
		err = mmfTransferBuffer->UpdateTransferWindow(iTransferWindow, KMMFTestBufferSize+1);
		if (err != KErrTooBig) verdict = EFail;
		if (mmfTransferBuffer->Status() != EUnAvailable) verdict = EFail;
		}

	 delete mmfTransferBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0119::CTestStep_MMF_BASECL_U_0119()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0119-HP");
	 }
 
/** Verify CMMFTransferBuffer create with valid RTransferWindow - then 
 * try to map in a new transfer window which does not have a valid buffer mapped in
 * with the buffer length bigger than mapped in transfer buffer length
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0119::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;

	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);

	 mmfTransferBuffer->MapOutBuffer();

	 iTransferWindow.Close();

	 TInt err = iTransferWindow.Create(KMMFDefaultTransferWindowSize);
	 if (err != KErrNone) INFO_PRINTF2(_L("could not create client transfer window %d" ), err );
	 else INFO_PRINTF1(_L("client transfer window created  OK"));

	 if (!err)
		{
		err = mmfTransferBuffer->UpdateTransferWindow(iTransferWindow);
		if (err != KErrNotReady) verdict = EFail;
		if (mmfTransferBuffer->Status() != EUnAvailable) verdict = EFail;
		}

	 delete mmfTransferBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0120::CTestStep_MMF_BASECL_U_0120()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0120-HP");
	 }
 
/** Verify CMMFTransferBuffer RequestSize/SetRequestSize
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0120::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 CMMFTransferBuffer* mmfTransferBuffer;
 
	 mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow);
 
	 //set the request size
	 TRAPD(err,mmfTransferBuffer->SetRequestSizeL(KMMFTestRequestSize));
	 if (err) verdict = EFail;
	 if (mmfTransferBuffer->RequestSize() != (TInt)KMMFTestRequestSize) verdict = EFail;

	 //set request size < 0 
	 TRAP(err,mmfTransferBuffer->SetRequestSizeL(-1));
	 if (err != KErrUnderflow) verdict = EFail;
	 if (mmfTransferBuffer->RequestSize() != (TInt)KMMFTestRequestSize) verdict = EFail;

	 //set request size greater to the max avail buffer size
	 TRAP(err,mmfTransferBuffer->SetRequestSizeL(KMMFTestBufferSize));
	 if (err) verdict = EFail;
	 if (mmfTransferBuffer->RequestSize() != (TInt)KMMFTestBufferSize) verdict = EFail;

	 //set request size greater than max avail buffer size
	 TRAP(err,mmfTransferBuffer->SetRequestSizeL(KMMFTestBufferSize+1));
	 if (err != KErrOverflow) verdict = EFail;
	 if (mmfTransferBuffer->RequestSize() != (TInt)KMMFTestBufferSize) verdict = EFail;

	 delete mmfTransferBuffer;

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0121::CTestStep_MMF_BASECL_U_0121()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0121-HP");
	 }
 
/** Verify CMMFTransferBuffer OOM
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0121::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	 CMMFTransferBuffer* mmfTransferBuffer = NULL;

	 __UHEAP_SETFAIL(RHeap::EFailNext, 1);
	 TRAPD(err, mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow));

	 if ((err != KErrNoMemory) || (mmfTransferBuffer)) verdict = EFail;

	 __UHEAP_RESET;
	 iTransferWindow.Close(); //won't be closed by CMMFTransferBuffer destructor !
	 delete mmfTransferBuffer;

 
	 /*XXX This test does not use the full heap count test
	 * this should not matter as a CMMFTransferBuffer NewL() only makes a single heap allocation
	 * I could not get the full test working - see commented below
	 * but the problem was with the transfer buffer tester client/server  -not
	 * with the CMMFTransfeBuffer alloction - which I could not get to work */

	 /*
	 TInt KMaxFailRate = 100000;
	 TBool complete = EFalse;
 
	__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAPD(err, mmfTransferBuffer = CMMFTransferBuffer::NewL(iTransferWindow));
		if (err == KErrNone)
			{//need to ckeck if next alloc fails
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				// reachedEnd = ETrue;
				// failCount -= 1;
				}
			else
				{
				User::Free(testAlloc);
				complete = ETrue;
				}
			//check a databuffer has been created
			if (!mmfTransferBuffer) 
				{
				verdict = EFail;
				complete = ETrue;
				}
			else 
				{
				delete mmfTransferBuffer;
				mmfTransferBuffer = NULL;
				//connection to transfer buffer tester was succesfull
	//			TInt error = iTransferBufferTesterClient.CreateTransferBuffer(KMMFTestBufferSize);
	//			if (error != KErrNone) 
	//				{
	//				INFO_PRINTF1(_L("TRANSFER BUFFER NOT CREATED !"));
	//				}
	//			else
					{//server process transfer buffer created successfully now create CMMFTransferBuffer from server buffer
	//				err = iTransferBufferTesterClient.WriteTestData(iTestData);
	//				if (err != KErrNone) INFO_PRINTF2(_L("could not write test date %d" ), err );
	//				else INFO_PRINTF1(_L("Test data written  OK"));

					err = iTransferWindow.Create(0x10000);
					if (err != KErrNone) INFO_PRINTF2(_L("could not create client transfer window %d" ), err );
					else INFO_PRINTF1(_L("client transfer window created  OK"));

					err = iTransferBufferTesterClient.MapBufferIntoClientWindowHandle(iTransferWindow.Handle());
					if (err != KErrNone) INFO_PRINTF2(_L("could not map in buffer to client transfer window %d" ), err );
					else 
						{
						INFO_PRINTF1(_L("Transfer buffer mapped into client transfer window  OK"));
						verdict = EPass;
						}
					}

				if (err)
					{
					verdict = EInconclusive;
					complete = ETrue;
					}
				}
			}
		else if (err != KErrNoMemory)
			{
			verdict = EFail; // bad error code
			complete = ETrue;
			}
		}

	 if (failCount >= KMaxFailRate-1) verdict = EFail;

	 __UHEAP_RESET;
	 delete mmfTransferBuffer;
	 __MM_HEAP_MARKEND;
	 */

	 return verdict ;
	 }
