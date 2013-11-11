
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
// TestDevVideoRecord.cpp
// 
//

#include "TestDevVideoRecord.h"
#include "TestDevVideoPlayTestData.h"
#include "TestDevVideoPlugins/encoder.h"
#include "TestDevVideoPlugins/preproc.h"
#include <e32math.h>


void CTestDevVideoRecordRecordingPosition::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordRecordingPosition::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordRecordingPosition::CTestDevVideoRecordRecordingPosition(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordRecordingPosition* CTestDevVideoRecordRecordingPosition::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordRecordingPosition* self = new(ELeave) CTestDevVideoRecordRecordingPosition(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordRecordingPosition::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  RecordingPosition"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
//	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	// get recording position
	TTimeIntervalMicroSeconds recordPos = aDevVideoRecord.RecordingPosition();

	// check against test data
	TTimeIntervalMicroSeconds testTime(KTestRecordPosition);
	if (recordPos != testTime)
		{
		ERR_PRINTF3(_L("RecordingPosition() incorrect - gave %d (expected %d)"),
			I64LOW(recordPos.Int64()), KTestRecordPosition);
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("RecordingPosition() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordGetOutputBufferStatus::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordGetOutputBufferStatus::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordGetOutputBufferStatus::CTestDevVideoRecordGetOutputBufferStatus(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordGetOutputBufferStatus* CTestDevVideoRecordGetOutputBufferStatus::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordGetOutputBufferStatus* self = new(ELeave) CTestDevVideoRecordGetOutputBufferStatus(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordGetOutputBufferStatus::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  GetOutputBufferStatus"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
//	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	TUint buffers = 0;
	TUint bytes = 0;

	// get output buffer status
	aDevVideoRecord.GetOutputBufferStatus(buffers, bytes);

	// check against test data
	if ((buffers != KTestFreeBuffers) || (bytes != KTestFreeBytes))
		{
		ERR_PRINTF5(_L("GetOutputBufferStatus() incorrect - gave %d, %d (expected %d, %d)"),
			buffers, bytes, KTestFreeBuffers, KTestFreeBytes);
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("GetOutputBufferStatus() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordGetPictureCounters::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordGetPictureCounters::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordGetPictureCounters::CTestDevVideoRecordGetPictureCounters(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordGetPictureCounters* CTestDevVideoRecordGetPictureCounters::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordGetPictureCounters* self = new(ELeave) CTestDevVideoRecordGetPictureCounters(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordGetPictureCounters::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TBool selPre = ETrue;
	TBool selEnc = ETrue;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  Resume"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestEncoderOnly:
			selPre = EFalse;
			break;
		case ETestPreProcOnly:
			selEnc = EFalse;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

	THwDeviceId hwEncoder = 0;
	THwDeviceId hwPreProc = 0;

	// select Encoder
	if (selEnc)
		{
		TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{
		TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
				return EInconclusive;
			}
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	CMMFDevVideoRecord::TPictureCounters picCounts;

	// get picture counters from DevVideoPlay
	aDevVideoRecord.GetPictureCounters(picCounts);

	CMMFDevVideoRecord::TPictureCounters testCounts = GetTestEncPictureCounters();
	if (hwEncoder && hwPreProc)
		{
		// account for both Encoder and pre processor
		testCounts.iPicturesSkippedBufferOverflow *= 2; 
		testCounts.iPicturesSkippedProcPower *= 2; 
		testCounts.iPicturesSkippedRateControl *= 2; 
		}
	else if (!hwEncoder)
		{
		// no processed samples from a pre processor
		testCounts.iPicturesProcessed = 0;
		}
	else if (!hwPreProc)
		{
		// adjust input pictures
		testCounts.iInputPictures = KTestEncInputPictures;
		}

	if (!CompareEncPictureCounters(picCounts, testCounts))
		{
		ERR_PRINTF1(_L("GetPictureCounters() comparison with test data failed!"));
		ret = EFail;
		}
	else 
		{
		INFO_PRINTF1(_L("GetPictureCounters() comparison with test data passed."));
		INFO_PRINTF3(_L("GetPictureCounters(), %d = %d"), err, expErr);
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordGetFrameStabilisation::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordGetFrameStabilisation::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordGetFrameStabilisation::CTestDevVideoRecordGetFrameStabilisation(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordGetFrameStabilisation* CTestDevVideoRecordGetFrameStabilisation::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordGetFrameStabilisation* self = new(ELeave) CTestDevVideoRecordGetFrameStabilisation(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordGetFrameStabilisation::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  GetFrameStabilisation"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	THwDeviceId hwEncoder = 0;
	THwDeviceId hwPreProc = 0;

	// select Encoder
	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	// get frame stabilisation output on encoder
	TRect testRect1(0, 0, 0, 0);
	aDevVideoRecord.GetFrameStabilisationOutput(hwEncoder, testRect1);
	
	// get frame stabilisation output on pre-processor
	TRect testRect2(0, 0, 0, 0);
	aDevVideoRecord.GetFrameStabilisationOutput(hwPreProc, testRect2);

	TBool success1 = EFalse;
	TBool success2 = EFalse;

	if (!(testRect1 == TRect(KTestFrameStableX1, KTestFrameStableY1, KTestFrameStableX2, KTestFrameStableY2)))
		{
		ERR_PRINTF1(_L("GetFrameStabilisationOutput() on Encoder didn't match test data!"));
		}
	else 
		{
		success1 = ETrue;
		}

	if (!(testRect2 == TRect(KTestFrameStableY1, KTestFrameStableX1, KTestFrameStableY2, KTestFrameStableX2)))
		{
		ERR_PRINTF1(_L("GetFrameStabilisationOutput() on PreProcessor didn't match test data!"));
		}
	else
		{
		success2 = ETrue;
		}

	if (success1 && success2)
		{
		INFO_PRINTF1(_L("GetFrameStabilisationOutput() called successfully!"));
		ret = EPass;
		}
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordNumDataBuffers::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordNumDataBuffers::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordNumDataBuffers::CTestDevVideoRecordNumDataBuffers(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordNumDataBuffers* CTestDevVideoRecordNumDataBuffers::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordNumDataBuffers* self = new(ELeave) CTestDevVideoRecordNumDataBuffers(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordNumDataBuffers::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  NumDataBuffers"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
//	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	// tell plugin to start creating buffers
	TRAP(err, aDevVideoRecord.SetImplementationSpecificEncoderOptionsL(KTestISEncBuffers));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't start them buffers a rollin!"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	// start buffer transfer
	aDevVideoRecord.Start();
	TUint numBuffers = 0;

	// get number of buffers
	numBuffers = aDevVideoRecord.NumDataBuffers();

	// should have 1 buffer only due to the way Start() works
	// i.e. when Start() is called on the fake Encoder a new buffer is
	// sent to DevVideoRecord through a callback.
	if (numBuffers != 1)
		{
		ERR_PRINTF3(_L("Number of data buffers incorrect!  Expected %d; Got %d!"), 1, numBuffers);
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("NumDataBuffers() called successfully"));
		}
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordNextBuffer::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordNextBuffer::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordNextBuffer::CTestDevVideoRecordNextBuffer(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordNextBuffer* CTestDevVideoRecordNextBuffer::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordNextBuffer* self = new(ELeave) CTestDevVideoRecordNextBuffer(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordNextBuffer::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  NextBuffer"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
//	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	// tell plugin to start creating buffers
	TRAP(err, aDevVideoRecord.SetImplementationSpecificEncoderOptionsL(KTestISEncBuffers));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't start them buffers a rollin!"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	iDevVideoRecord = &aDevVideoRecord;
	aDevVideoRecord.Start();//Tell plugin to start creating buffers.
	// We should now get a series of NewBuffer() callbacks.

	if (iError == KErrNone)
		{
		ret = EPass;
		}
	else
		{
		ERR_PRINTF2(_L("Error - buffer processing failed with error %d"), iError);
		}
	
	return ret;
	}

void CTestDevVideoRecordNextBuffer::MdvroNewBuffers()
	{
	TRAP(iError, DoNewBuffersL());
	}

void CTestDevVideoRecordNextBuffer::CheckNewBufferInfoL(TUint aNumBuffers)
	{
	TUint numBuffers = 0;
	numBuffers = iDevVideoRecord->NumDataBuffers();

	if (numBuffers != aNumBuffers)
		{
		User::Leave(KErrGeneral);
		}
	}

void CTestDevVideoRecordNextBuffer::DoNewBuffersL()
	{
	User::LeaveIfError(iError);//Don't go any further if we have an error
	switch (iCurrentBufferNumber)
		{
		case 0:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buf = iDevVideoRecord->NextBufferL();
			CheckNewBufferInfoL(0);
			iDevVideoRecord->ReturnBuffer(buf);
			break;
			}
		case 1:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buf = iDevVideoRecord->NextBufferL();
			CheckNewBufferInfoL(0);
			iDevVideoRecord->ReturnBuffer(buf);
			break;
			}
		case 2:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buf = iDevVideoRecord->NextBufferL();
			CheckNewBufferInfoL(0);
			iDevVideoRecord->ReturnBuffer(buf);
			break;
			}
		case 3:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buf = iDevVideoRecord->NextBufferL();
			CheckNewBufferInfoL(0);
			iDevVideoRecord->ReturnBuffer(buf);
			break;
			}
		case 4:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buf = iDevVideoRecord->NextBufferL();
			CheckNewBufferInfoL(0);
			iDevVideoRecord->ReturnBuffer(buf);
			break;
			}
		case 5:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			TVideoOutputBuffer* buf = iDevVideoRecord->NextBufferL();
			CheckNewBufferInfoL(0);
			iDevVideoRecord->ReturnBuffer(buf);
			break;
			}
		case 6:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(1);
			break;
			}
		case 7:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(2);
			break;
			}
		case 8:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(3);
			break;
			}
		case 9:
			{
			iCurrentBufferNumber++;
			CheckNewBufferInfoL(4);
			TVideoOutputBuffer* buf = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buf);
			CheckNewBufferInfoL(3);
			buf = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buf);
			CheckNewBufferInfoL(2);
			buf = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buf);
			CheckNewBufferInfoL(1);
			buf = iDevVideoRecord->NextBufferL();
			iDevVideoRecord->ReturnBuffer(buf);
			CheckNewBufferInfoL(0);
			break;
			}
		default:
			User::Leave(KErrGeneral);
			break;
		};
	}

//------------------------------------------------------------------

void CTestDevVideoRecordPictureLoss::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordPictureLoss::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordPictureLoss::CTestDevVideoRecordPictureLoss(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordPictureLoss* CTestDevVideoRecordPictureLoss::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordPictureLoss* self = new(ELeave) CTestDevVideoRecordPictureLoss(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordPictureLoss::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  PictureLoss"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
//	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// eABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	// call PictureLoss() on Encoder
	aDevVideoRecord.PictureLoss();

	INFO_PRINTF1(_L("PictureLoss() called successfully"));

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordPictureLossSpec::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordPictureLossSpec::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordPictureLossSpec::CTestDevVideoRecordPictureLossSpec(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordPictureLossSpec* CTestDevVideoRecordPictureLossSpec::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordPictureLossSpec* self = new(ELeave) CTestDevVideoRecordPictureLossSpec(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordPictureLossSpec::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  PictureLossSpec"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
//	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	RArray<TPictureId> testIds;

	User::LeaveIfError(testIds.Append(KTestPictureId1));
	User::LeaveIfError(testIds.Append(KTestPictureId2));
	User::LeaveIfError(testIds.Append(KTestPictureId3));

	// call multiple PictureLoss() on Encoder
	aDevVideoRecord.PictureLoss(testIds.Array());
	
	testIds.Close();
	
	INFO_PRINTF1(_L("PictureLoss() called successfully"));

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSliceLoss::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSliceLoss::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSliceLoss::CTestDevVideoRecordSliceLoss(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSliceLoss* CTestDevVideoRecordSliceLoss::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSliceLoss* self = new(ELeave) CTestDevVideoRecordSliceLoss(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSliceLoss::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SliceLoss"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
//	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	// call SliceLoss() on Encoder
	aDevVideoRecord.SliceLoss(KTestFirstMacroblock, KTestNumMacroblocks, KTestPictureId1);

	INFO_PRINTF1(_L("SliceLoss() called successfully"));

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSendRefPictureSelection::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSendRefPictureSelection::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSendRefPictureSelection::CTestDevVideoRecordSendRefPictureSelection(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSendRefPictureSelection* CTestDevVideoRecordSendRefPictureSelection::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSendRefPictureSelection* self = new(ELeave) CTestDevVideoRecordSendRefPictureSelection(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSendRefPictureSelection::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SendRefPictureSelection"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
//	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	// call ReferencePictureSelection() on Encoder
	aDevVideoRecord.ReferencePictureSelection(KTestRefPictureInfo);

	INFO_PRINTF1(_L("ReferencePictureSelection() called successfully"));

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordNumComplexityLevels::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordNumComplexityLevels::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordNumComplexityLevels::CTestDevVideoRecordNumComplexityLevels(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordNumComplexityLevels* CTestDevVideoRecordNumComplexityLevels::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordNumComplexityLevels* self = new(ELeave) CTestDevVideoRecordNumComplexityLevels(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordNumComplexityLevels::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  NumComplexityLevels"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	THwDeviceId hwEncoder = 0;
	THwDeviceId hwPreProc = 0;

	// select Encoder
	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	// get num complexity levels on Encoder
	TUint levs1 = aDevVideoRecord.NumComplexityLevels(hwEncoder);
	
	// get num complexity levels on pre-processor
	TUint levs2 = aDevVideoRecord.NumComplexityLevels(hwPreProc);

	// check return values against test data
	if (levs1 != KTestNumComplexityLevels1 || levs2 != KTestNumComplexityLevels2)
		{
		ERR_PRINTF3(_L("Error - comparison against test data failed! (%d, %d)"), levs1, levs2);
		ret = EFail;
		}
	else
		{
		INFO_PRINTF3(_L("NumComplexityLevels(), %d, %d"), levs1, levs2);
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSetComplexityLevel::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSetComplexityLevel::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSetComplexityLevel::CTestDevVideoRecordSetComplexityLevel(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetComplexityLevel* CTestDevVideoRecordSetComplexityLevel::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetComplexityLevel* self = new(ELeave) CTestDevVideoRecordSetComplexityLevel(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetComplexityLevel::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetComplexityLevel"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	THwDeviceId hwEncoder = 0;
	THwDeviceId hwPreProc = 0;

	// select Encoder
	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitializeComplete callback
	aDevVideoRecord.Initialize();

	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	// set complexity level on encoder
	aDevVideoRecord.SetComplexityLevel(hwEncoder, KTestComplexityLevel1);
	
	// set complexity level on pre-processor
	aDevVideoRecord.SetComplexityLevel(hwPreProc, KTestComplexityLevel2);

	// if no PANIC then test has passed
	INFO_PRINTF3(_L("SetComplexityLevel(), %d, %d"), KTestComplexityLevel1, KTestComplexityLevel2);
	
	return ret;
	}


//------------------------------------------------------------------

void CTestDevVideoRecordCommit::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordCommit::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordCommit::CTestDevVideoRecordCommit(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordCommit* CTestDevVideoRecordCommit::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoRecordCommit* self = new(ELeave) CTestDevVideoRecordCommit(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordCommit::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPre = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoRecord:  Commit"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestEncoderOnly:
			selPre = EFalse;
			break;
		case ETestPreProcOnly:
			selDec = EFalse;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
	if (selDec)
		{
//		TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select Pre-processor
	if (selPre)
		{
//		TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Pre processor"));
				return EInconclusive;
			}
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoRecord.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	// call CommitL on DevVideoRecord 
	TRAP(iError, aDevVideoRecord.CommitL());

	if (iError != expErr)
		{
		ERR_PRINTF3(_L("CommitL() left with error %d; Expected %d!"), iError, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("CommitL() called successfully"));
	
	return ret;
	}

//------------------------------------------------------------------

void CTestDevVideoRecordRevert::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordRevert::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordRevert::CTestDevVideoRecordRevert(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordRevert* CTestDevVideoRecordRevert::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoRecordRevert* self = new(ELeave) CTestDevVideoRecordRevert(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordRevert::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPre = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoRecord:  Revert"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;	// EABI warning removal
			break;
		case ETestEncoderOnly:
			selPre = EFalse;
			break;
		case ETestPreProcOnly:
			selDec = EFalse;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwEncoder = 0;
//	THwDeviceId hwPreProc = 0;

	// select Encoder
	if (selDec)
		{
//		TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select Pre-processor
	if (selPre)
		{
//		TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Pre processor"));
				return EInconclusive;
			}
		}

	iError = KErrNotReady;
	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoRecord.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
			return EInconclusive;
		}

	// call Revert on DevVideoRecord
	aDevVideoRecord.Revert();

	// if no PANIC then test has passed...

	INFO_PRINTF1(_L("Revert() called successfully"));
	
	return ret;
	}


//------------------------------------------------------------------

CTestDevVideoRecordCustomInterface::CTestDevVideoRecordCustomInterface(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordCustomInterface* CTestDevVideoRecordCustomInterface::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoRecordCustomInterface* self = new(ELeave) CTestDevVideoRecordCustomInterface(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordCustomInterface::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  CustomInterface"));

	THwDeviceId hwEncoder = 0;
	THwDeviceId hwPreProc = 0;
	TInt err = KErrNone;

	TRAP(err, hwEncoder = aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize encoder"));
			return EInconclusive;
		}
	
	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize pre processor"));
			return EInconclusive;
		}

	// First try test uninitialized

	TAny* ciOne = NULL;
	TAny* ciTwo = NULL;
	TAny* ciThree = NULL;
	TAny* ciFour = NULL;
	
	ciOne = aDevVideoRecord.CustomInterface(hwEncoder, KUidCustomInterfaceThree);
	ciTwo = aDevVideoRecord.CustomInterface(hwEncoder, KUidCustomInterfaceFour);
	ciThree = aDevVideoRecord.CustomInterface(hwPreProc, KUidCustomInterfaceFour);
	ciFour = aDevVideoRecord.CustomInterface(hwPreProc, KUidCustomInterfaceThree);

	if (ciOne==NULL || ciTwo!=NULL || ciThree==NULL || ciFour!=NULL)
		{
		ERR_PRINTF1(_L("Test Failed - wrong interfaces returned"));
		return EFail;
		}

	// Next, re-perform test initialized.

	// initialize CDevVideoRecord and wait for response
	// iError is set by the MdvroInitComplete callback
	aDevVideoRecord.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoRecord"));
		return EInconclusive;
		}

	ciOne = NULL;
	ciTwo = NULL;
	ciThree = NULL;
	ciFour = NULL;
	
	ciOne = aDevVideoRecord.CustomInterface(hwEncoder, KUidCustomInterfaceThree);
	ciTwo = aDevVideoRecord.CustomInterface(hwEncoder, KUidCustomInterfaceFour);
	ciThree = aDevVideoRecord.CustomInterface(hwPreProc, KUidCustomInterfaceFour);
	ciFour = aDevVideoRecord.CustomInterface(hwPreProc, KUidCustomInterfaceThree);

	if (ciOne==NULL || ciTwo!=NULL || ciThree==NULL || ciFour!=NULL)
		{
		ERR_PRINTF1(_L("Test Failed - wrong interfaces returned"));
		return EFail;
		}


	return EPass;
	}

/*
//------------------------------------------------------------------

CTestDevVideoRecordXXX::CTestDevVideoRecordXXX(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordXXX* CTestDevVideoRecordXXX::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoRecordXXX* self = new(ELeave) CTestDevVideoRecordXXX(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordXXX::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  XXX"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	// XXXX

	TInt err = KErrNone;
	TRAP(err, aDevVideoRecord.API_FUNCTION_HERE() )

	if (err != expErr)
		{
		ERR_PRINTF3(_L("XXX() gave error %d (expected %d)"),iError, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("XXX(), %d = %d"), iError, expErr);
	
	return ret;
	}

*/

