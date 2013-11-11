
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


void CTestDevVideoRecordSetChannelBitErrorRate::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSetChannelBitErrorRate::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSetChannelBitErrorRate::CTestDevVideoRecordSetChannelBitErrorRate(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetChannelBitErrorRate* CTestDevVideoRecordSetChannelBitErrorRate::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetChannelBitErrorRate* self = new(ELeave) CTestDevVideoRecordSetChannelBitErrorRate(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetChannelBitErrorRate::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetChannelBitErrorRate"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	aDevVideoRecord.SetChannelBitErrorRate( KTestLevel, KTestErrorRate, KTestStdDeviation );

	// if no PANIC then test has passed
	INFO_PRINTF3(_L("SetChannelBitErrorRate(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSetSegTargetSize::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSetSegTargetSize::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSetSegTargetSize::CTestDevVideoRecordSetSegTargetSize(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetSegTargetSize* CTestDevVideoRecordSetSegTargetSize::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetSegTargetSize* self = new(ELeave) CTestDevVideoRecordSetSegTargetSize(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetSegTargetSize::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetSegTargetSize"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	aDevVideoRecord.SetSegmentTargetSize( KTestLayer, KTestSizeBytes, KTestSizeMacroblocks );

	// if no PANIC then test has passed
	INFO_PRINTF3(_L("SetSegmentTargetSize(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSetRateControl::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSetRateControl::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSetRateControl::CTestDevVideoRecordSetRateControl(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetRateControl* CTestDevVideoRecordSetRateControl::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetRateControl* self = new(ELeave) CTestDevVideoRecordSetRateControl(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetRateControl::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetRateControl"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	TRateControlOptions testOptions = GetTestRateControlOptions();
	aDevVideoRecord.SetRateControlOptions( KTestLayer, testOptions );

	// if no PANIC then test has passed
	INFO_PRINTF3(_L("SetRateControlOptions(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSetInLayerScalability::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSetInLayerScalability::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSetInLayerScalability::CTestDevVideoRecordSetInLayerScalability(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetInLayerScalability* CTestDevVideoRecordSetInLayerScalability::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetInLayerScalability* self = new(ELeave) CTestDevVideoRecordSetInLayerScalability(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetInLayerScalability::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetInLayerScalability"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	RArray<TUint> bitShare;
	RArray<TUint> picShare;

	CleanupClosePushL(bitShare);
	CleanupClosePushL(picShare);

	User::LeaveIfError(bitShare.Append(KTestBitrateShare1));
	User::LeaveIfError(bitShare.Append(KTestBitrateShare2));
	User::LeaveIfError(picShare.Append(KTestPictureShare1));
	User::LeaveIfError(picShare.Append(KTestPictureShare2));

	// this will leave if an error occurs
	TRAP(err, aDevVideoRecord.SetInLayerScalabilityL( KTestLayer, KTestNumSteps, KTestLayerScaleType, bitShare.Array(), picShare.Array()));

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetInLayerScalabilityL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetInLayerScalabilityL(), %d = %d"), err, expErr);
	
	// pop both Arrays off the stack
	CleanupStack::PopAndDestroy(2, &bitShare);
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSetLayerPromPointPeriod::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSetLayerPromPointPeriod::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSetLayerPromPointPeriod::CTestDevVideoRecordSetLayerPromPointPeriod(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetLayerPromPointPeriod* CTestDevVideoRecordSetLayerPromPointPeriod::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetLayerPromPointPeriod* self = new(ELeave) CTestDevVideoRecordSetLayerPromPointPeriod(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetLayerPromPointPeriod::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetLayerPromPointPeriod"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	// this will PANIC if an error occurs
	aDevVideoRecord.SetLayerPromotionPointPeriod( KTestLayer, KTestPointPeriod );

	// if no PANIC then test has passed
	INFO_PRINTF3(_L("SetLayerPromotionPointPeriod(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordCSSettingsOutput::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordCSSettingsOutput::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordCSSettingsOutput::CTestDevVideoRecordCSSettingsOutput(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordCSSettingsOutput* CTestDevVideoRecordCSSettingsOutput::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordCSSettingsOutput* self = new(ELeave) CTestDevVideoRecordCSSettingsOutput(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordCSSettingsOutput::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  CSSettingsOutput"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	// get the CS specific output
	HBufC8* csOutput = NULL;
	TRAP(err, 
			csOutput = aDevVideoRecord.CodingStandardSpecificSettingsOutputLC();
			CleanupStack::Pop(csOutput); );

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("CodingStandardSpecificSettingsOutputLC() on Encoder gave error %d (expected %d)"),err, expErr);
		}
	else if (!csOutput)
		{
		ERR_PRINTF1(_L("Received null CS output!")); 
		}
	else if ( !(*csOutput == KTestCSSettingsOutput) )
		{
		ERR_PRINTF2(_L("Received incorrect CS output:  %S"), csOutput); 
		}
	else
		{
		INFO_PRINTF1(_L("Correctly compared CS output to test data"));
		INFO_PRINTF3(_L("CodingStandardSpecificSettingsOutputLC(), %d = %d"), err, expErr);
		ret = EPass;
		}
			
	delete csOutput;
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordISSettingsOutput::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordISSettingsOutput::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordISSettingsOutput::CTestDevVideoRecordISSettingsOutput(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordISSettingsOutput* CTestDevVideoRecordISSettingsOutput::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordISSettingsOutput* self = new(ELeave) CTestDevVideoRecordISSettingsOutput(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordISSettingsOutput::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  ISSettingsOutput"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	// get the IS specific output
	HBufC8* isOutput = NULL;
	TRAP(err, 
			isOutput = aDevVideoRecord.ImplementationSpecificSettingsOutputLC(); 
			CleanupStack::Pop(isOutput); );

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("ImplementationSpecificSettingsOutputLC() on Encoder gave error %d (expected %d)"),err, expErr);
		}
	else if (!isOutput)
		{
		ERR_PRINTF1(_L("Received null IS output!")); 
		}
	else if ( !(*isOutput == KTestISSettingsOutput) )
		{
		ERR_PRINTF2(_L("Received incorrect IS output:  %S"), isOutput); 
		}
	else
		{
		INFO_PRINTF1(_L("Correctly compared IS output to test data"));
		INFO_PRINTF3(_L("ImplementationSpecificSettingsOutputLC(), %d = %d"), err, expErr);
		ret = EPass;
		}
	
	delete isOutput;
	isOutput = NULL;
		
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordWritePicture::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordWritePicture::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoRecordWritePicture::MdvroReturnPicture(TVideoPicture* aPicture)
	{
	INFO_PRINTF1(_L("CTestDevVideoRecordWritePicture::MdvroReturnPicture()"));

	iPicture = aPicture;
	}

CTestDevVideoRecordWritePicture::CTestDevVideoRecordWritePicture(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordWritePicture* CTestDevVideoRecordWritePicture::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordWritePicture* self = new(ELeave) CTestDevVideoRecordWritePicture(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordWritePicture::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TBool selEnc = ETrue;
	TBool selPre = ETrue;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  WritePicture"));
	
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

	// select Encoder
	if (selEnc)
		{
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	// construct a video picture structure with a valid timestamp and
	// write this DevVideoRecord.  If expected picture comes back through the
	// MdvroReturnPicture() callback then test has passed.
	TVideoPicture picture;
	TTimeIntervalMicroSeconds picTime(KTestPictureTimestamp);
	picture.iTimestamp = picTime;
	picture.iOptions = TVideoPicture::ETimestamp;
	
	// write picture to DevVideoRecord
	TRAP(err, aDevVideoRecord.WritePictureL(&picture) )

	if (err != expErr)
		{
		ERR_PRINTF3(_L("WritePicture() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		{
		TBool picSuccess = EFalse;

		// check callback has been called and picture is correct
		if (iPicture)
			{
			// if only using a Encoder then the timestamp will
			// have been changed
			if (!selPre)
				picTime = KTestEncPictureTimestamp;

			if ( (iPicture->iTimestamp == picTime) &&
				 (iPicture->iOptions == TVideoPicture::ETimestamp) )
				{
				picSuccess = ETrue;
				}
			}
		
		if (!picSuccess)
			{
			ERR_PRINTF1(_L("Received picture is corrupt!"));
			ret = EFail;
			}
		else
			{
			INFO_PRINTF1(_L("Original picture received correctly"));
			INFO_PRINTF3(_L("WritePicture(), %d = %d"), err, expErr);	
			}
		}
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSendSupplementalInfo::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSendSupplementalInfo::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoRecordSendSupplementalInfo::MdvroSupplementalInfoSent()
	{
	INFO_PRINTF1(_L("CTestDevVideoRecordSendSupplementalInfo::MdvpoSupplementalInfoSent()"));
	iSuppInfoSent = ETrue;
	}

CTestDevVideoRecordSendSupplementalInfo::CTestDevVideoRecordSendSupplementalInfo(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSendSupplementalInfo* CTestDevVideoRecordSendSupplementalInfo::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSendSupplementalInfo* self = new(ELeave) CTestDevVideoRecordSendSupplementalInfo(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSendSupplementalInfo::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SendSupplementalInfo"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	iSuppInfoSent = EFalse;

	// this will leave if an error occurs
	HBufC8* info = KTestSuppInfo().AllocL();
	TRAP(err, aDevVideoRecord.SendSupplementalInfoL( *info ));

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SendSupplementalInfoL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else if (!iSuppInfoSent)
		{
		ERR_PRINTF1(_L("Error - MdvroSupplementalInfoSent() callback didn't occur!"));
		}
	else
		INFO_PRINTF3(_L("SendSupplementalInfoL(), %d = %d"), err, expErr);
	
	// delete info
	delete info;
	info = NULL;

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSendSupplementalInfoTime::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSendSupplementalInfoTime::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoRecordSendSupplementalInfoTime::MdvroSupplementalInfoSent()
	{
	INFO_PRINTF1(_L("CTestDevVideoRecordSendSupplementalInfo::MdvpoSupplementalInfoSent()"));
	iSuppInfoSent = ETrue;
	}

CTestDevVideoRecordSendSupplementalInfoTime::CTestDevVideoRecordSendSupplementalInfoTime(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSendSupplementalInfoTime* CTestDevVideoRecordSendSupplementalInfoTime::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSendSupplementalInfoTime* self = new(ELeave) CTestDevVideoRecordSendSupplementalInfoTime(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSendSupplementalInfoTime::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SendSupplementalInfoTime"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	iSuppInfoSent = EFalse;

	// this will leave if an error occurs
	HBufC8* info = KTestSuppInfo().AllocL();
	TTimeIntervalMicroSeconds suppTime( KTestSuppTime );
	TRAP(err, aDevVideoRecord.SendSupplementalInfoL( *info, suppTime ));

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SendSupplementalInfoL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else if (!iSuppInfoSent)
		{
		ERR_PRINTF1(_L("Error - MdvroSupplementalInfoSent() callback didn't occur!"));
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SendSupplementalInfoL(), %d = %d"), err, expErr);
	
	// delete info
	delete info;
	info = NULL;

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordCancelSupplementalInfo::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordCancelSupplementalInfo::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoRecordCancelSupplementalInfo::MdvroSupplementalInfoSent()
	{
	INFO_PRINTF1(_L("CTestDevVideoRecordSendSupplementalInfo::MdvpoSupplementalInfoSent()"));
	iSuppInfoCount++;
	}

CTestDevVideoRecordCancelSupplementalInfo::CTestDevVideoRecordCancelSupplementalInfo(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordCancelSupplementalInfo* CTestDevVideoRecordCancelSupplementalInfo::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordCancelSupplementalInfo* self = new(ELeave) CTestDevVideoRecordCancelSupplementalInfo(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordCancelSupplementalInfo::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EFail;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  CancelSupplementalInfo"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
			return EInconclusive;
		}

	// select pre-processor
	TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	iSuppInfoCount = 0;

	// this will leave if an error occurs
	HBufC8* info = KTestSuppInfo().AllocL();
	TTimeIntervalMicroSeconds suppTime( KTestSuppTimeCancel );
	TRAP(err, aDevVideoRecord.SendSupplementalInfoL( *info, suppTime ));

	// delete info - in a real plugin we can't do this until callback has occurred
	// but this is a fake plugin so this is ok.
	delete info;
	info = NULL;

	// if error occurred then test is inconclusive since CancelSupplementalInfo() doesn't leave
	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SendSupplementalInfoL() on Encoder gave error %d (expected %d)"),err, expErr);	
		return EInconclusive;
		}

	// check that supp info callback has not occurred
	if (iSuppInfoCount == 0)
		{
		// cancel send request
		aDevVideoRecord.CancelSupplementalInfo();
		
		// check callback has now been called
		if (iSuppInfoCount == 1)
			{
			INFO_PRINTF1(_L("Request cancelled successfully!"));
			ret = EPass;
			}
		else 
			ERR_PRINTF1(_L("Error - MdvroSupplementalInfoSent() callback didn't occur!"));
		}
	else
		{				
		ERR_PRINTF1(_L("Error - MdvroSupplementalInfoSent() callback already occurred!"));
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordInputEnd::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordInputEnd::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoRecordInputEnd::MdvroStreamEnd()
	{
	INFO_PRINTF1(_L("CTestDevVideoRecordInputEnd::MdvpoStreamEnd()"));
	
	// keep track on number of times this callback is called
	iEndCount++;
	}

CTestDevVideoRecordInputEnd::CTestDevVideoRecordInputEnd(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordInputEnd* CTestDevVideoRecordInputEnd::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordInputEnd* self = new(ELeave) CTestDevVideoRecordInputEnd(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordInputEnd::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selPre = ETrue;
	TBool selEnc = ETrue;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  InputEnd"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;		
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

	// select Encoder
	if (selEnc)
		{
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	
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

	// call input end on DevVideoRecord and check callback counter == 1
	aDevVideoRecord.InputEnd();

	if (iEndCount != 1)
		{
		ERR_PRINTF2(_L("Error - MdvpoStreamEnd only called %d times!"), iEndCount);
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("InputEnd() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordStart::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordStart::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordStart::CTestDevVideoRecordStart(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordStart* CTestDevVideoRecordStart::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordStart* self = new(ELeave) CTestDevVideoRecordStart(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordStart::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selPre = ETrue;
	TBool selEnc = ETrue;
	TInt expPos = 0;

	INFO_PRINTF1(_L("CMMFDevVideoRecord:  Start"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;				
			expPos = KTestRecTimeStartPre;
			break;
		case ETestEncoderOnly:
			selPre = EFalse;
			expPos = KTestRecTimeStartEnc;
			break;
		case ETestPreProcOnly:
			selEnc = EFalse;
			expPos = KTestRecTimeStartPre;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	if (selEnc)
		{
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
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

	// call start
	aDevVideoRecord.Start();

	// check position
	TTimeIntervalMicroSeconds time(0);
	TTimeIntervalMicroSeconds expTime(expPos);
	time = aDevVideoRecord.RecordingPosition();

	if (!(time == expTime))
		{
		ERR_PRINTF3(_L("Error - recording position incorrect:  Expected %u, Got %u!"),
			I64LOW(expTime.Int64()), I64LOW(time.Int64()));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Start() called successfully"));
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordStop::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordStop::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordStop::CTestDevVideoRecordStop(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordStop* CTestDevVideoRecordStop::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordStop* self = new(ELeave) CTestDevVideoRecordStop(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordStop::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selPre = ETrue;
	TBool selEnc = ETrue;
	TInt expPos = 0;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  Stop"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;			
			expPos = KTestRecTimeStop;
			break;
		case ETestEncoderOnly:
			selPre = EFalse;
			expPos = KTestRecTimeStop;
			break;
		case ETestPreProcOnly:
			selEnc = EFalse;
			expPos = KTestRecTimeStop;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	if (selEnc)
		{
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{

		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
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

	// call Stop
	aDevVideoRecord.Stop();

	// check position
	TTimeIntervalMicroSeconds time(0);
	TTimeIntervalMicroSeconds expTime(expPos);
	time = aDevVideoRecord.RecordingPosition();

	if (!(time == expTime))
		{
		ERR_PRINTF3(_L("Error - recording position incorrect:  Expected %u, Got %u!"),
			I64LOW(expTime.Int64()), I64LOW(time.Int64()));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Stop() called successfully"));
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordPause::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordPause::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordPause::CTestDevVideoRecordPause(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordPause* CTestDevVideoRecordPause::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordPause* self = new(ELeave) CTestDevVideoRecordPause(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordPause::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selPre = ETrue;
	TBool selEnc = ETrue;
	TInt expPos = 0;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  Pause"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;				
			expPos = KTestRecTimePausePre;
			break;
		case ETestEncoderOnly:
			selPre = EFalse;
			expPos = KTestRecTimePauseEnc;
			break;
		case ETestPreProcOnly:
			selEnc = EFalse;
			expPos = KTestRecTimePausePre;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	if (selEnc)
		{
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
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

	// call Pause
	aDevVideoRecord.Pause();

	// check position
	TTimeIntervalMicroSeconds time(0);
	TTimeIntervalMicroSeconds expTime(expPos);
	time = aDevVideoRecord.RecordingPosition();

	if (!(time == expTime))
		{
		ERR_PRINTF3(_L("Error - recording position incorrect:  Expected %u, Got %u!"),
			I64LOW(expTime.Int64()), I64LOW(time.Int64()));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Pause() called successfully"));
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordResume::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordResume::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordResume::CTestDevVideoRecordResume(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordResume* CTestDevVideoRecordResume::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordResume* self = new(ELeave) CTestDevVideoRecordResume(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordResume::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selPre = ETrue;
	TBool selEnc = ETrue;
	TInt expPos = 0;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  Resume"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;				
			expPos = KTestRecTimeResumePre;
			break;
		case ETestEncoderOnly:
			selPre = EFalse;
			expPos = KTestRecTimeResumeEnc;
			break;
		case ETestPreProcOnly:
			selEnc = EFalse;
			expPos = KTestRecTimeResumePre;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

	// select Encoder
	if (selEnc)
		{
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
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

	// call Resume
	aDevVideoRecord.Resume();

	// check position
	TTimeIntervalMicroSeconds time(0);
	TTimeIntervalMicroSeconds expTime(expPos);
	time = aDevVideoRecord.RecordingPosition();

	if (!(time == expTime))
		{
		ERR_PRINTF3(_L("Error - recording position incorrect:  Expected %u, Got %u!"),
			I64LOW(expTime.Int64()), I64LOW(time.Int64()));
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Resume() called successfully"));
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordFreezePicture::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordFreezePicture::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordFreezePicture::CTestDevVideoRecordFreezePicture(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordFreezePicture* CTestDevVideoRecordFreezePicture::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordFreezePicture* self = new(ELeave) CTestDevVideoRecordFreezePicture(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordFreezePicture::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selPre = ETrue;
	TBool selEnc = ETrue;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  FreezePicture"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;	
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

	// select Encoder
	if (selEnc)
		{
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
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

	// call Freeze
	aDevVideoRecord.Freeze();

	// if there is no PANIC then test has passed.

	INFO_PRINTF1(_L("Freeze() called successfully"));

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordReleaseFreeze::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordReleaseFreeze::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordReleaseFreeze::CTestDevVideoRecordReleaseFreeze(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordReleaseFreeze* CTestDevVideoRecordReleaseFreeze::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordReleaseFreeze* self = new(ELeave) CTestDevVideoRecordReleaseFreeze(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordReleaseFreeze::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selPre = ETrue;
	TBool selEnc = ETrue;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  ReleaseFreeze"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;	
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

	// select Encoder
	if (selEnc)
		{
		TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize Encoder"));
				return EInconclusive;
			}
		}

	// select pre-processor
	if (selPre)
		{
		TRAP(err, aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));
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

	// call ReleaseFreeze
	aDevVideoRecord.ReleaseFreeze();

	INFO_PRINTF1(_L("ReleaseFreeze() called successfully"));

	return ret;
	}
//------------------------------------------------------------------

