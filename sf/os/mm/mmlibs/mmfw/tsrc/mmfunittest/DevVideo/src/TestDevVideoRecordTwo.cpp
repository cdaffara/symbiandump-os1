
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


CTestDevVideoRecordSetRgbToYuvOptions::CTestDevVideoRecordSetRgbToYuvOptions(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetRgbToYuvOptions* CTestDevVideoRecordSetRgbToYuvOptions::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetRgbToYuvOptions* self = new(ELeave) CTestDevVideoRecordSetRgbToYuvOptions(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetRgbToYuvOptions::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetRgbToYuvOptions"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

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

	// set RGB to YUV options with formats on encoder
	TRAP(err, aDevVideoRecord.SetRgbToYuvOptionsL(hwEncoder, KTestRgbRange1, KTestYuvFormat1) );
	
	// set RGB to YUV options with formats on pre-processor
	TRAP(err2, aDevVideoRecord.SetRgbToYuvOptionsL(hwPreProc, KTestRgbRange2, KTestYuvFormat2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetRgbToYuvOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetRgbToYuvOptionsL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetRgbToYuvOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetYuvToYuvOptions::CTestDevVideoRecordSetYuvToYuvOptions(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetYuvToYuvOptions* CTestDevVideoRecordSetYuvToYuvOptions::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetYuvToYuvOptions* self = new(ELeave) CTestDevVideoRecordSetYuvToYuvOptions(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetYuvToYuvOptions::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetYuvToYuvOptions"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

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

	// set YUV to YUV options with formats on Encoder
	TRAP(err, aDevVideoRecord.SetYuvToYuvOptionsL(hwEncoder, KTestYuvFormat1, KTestYuvFormat2) );
	
	// set YUV to YUV options with formats on pre-processor
	TRAP(err2, aDevVideoRecord.SetYuvToYuvOptionsL(hwPreProc, KTestYuvFormat2, KTestYuvFormat1) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetYuvToYuvOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetYuvToYuvOptionsL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetYuvToYuvOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetRotate::CTestDevVideoRecordSetRotate(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetRotate* CTestDevVideoRecordSetRotate::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetRotate* self = new(ELeave) CTestDevVideoRecordSetRotate(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetRotate::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetRotate"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

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

	// set rotate options on encoder
	TRAP(err, aDevVideoRecord.SetRotateOptionsL(hwEncoder, KTestRotate1) );
	
	// set rotate options on pre-processor
	TRAP(err2, aDevVideoRecord.SetRotateOptionsL(hwPreProc, KTestRotate2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetRotateOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetRotateOptionsL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetRotateOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetScale::CTestDevVideoRecordSetScale(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetScale* CTestDevVideoRecordSetScale::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetScale* self = new(ELeave) CTestDevVideoRecordSetScale(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetScale::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetScale"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

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

	// set scale options on encoder
	TSize testScale1(KTestScaleX, KTestScaleY);
	TRAP(err, aDevVideoRecord.SetScaleOptionsL(hwEncoder, testScale1, ETrue) );
	
	TSize testScale2(KTestScaleY, KTestScaleX);
	// set scale options on pre-processor
	TRAP(err2, aDevVideoRecord.SetScaleOptionsL(hwPreProc, testScale2, ETrue) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetScaleOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetScaleOptionsL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetScaleOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetInputCrop::CTestDevVideoRecordSetInputCrop(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetInputCrop* CTestDevVideoRecordSetInputCrop::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetInputCrop* self = new(ELeave) CTestDevVideoRecordSetInputCrop(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetInputCrop::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetInputCrop"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

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

	// set input crop on Encoder
	TRect testRect1(KTestInputCropRectA, KTestInputCropRectB, KTestInputCropRectC, KTestInputCropRectD);
	TRAP(err, aDevVideoRecord.SetInputCropOptionsL(hwEncoder, testRect1) )
	
	// set input crop on pre-processor
	TRect testRect2(KTestInputCropRectD, KTestInputCropRectC, KTestInputCropRectB, KTestInputCropRectA);
	TRAP(err2, aDevVideoRecord.SetInputCropOptionsL(hwPreProc, testRect2) )


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetInputCropOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetInputCropOptionsL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetInputCropOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetOutputCrop::CTestDevVideoRecordSetOutputCrop(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetOutputCrop* CTestDevVideoRecordSetOutputCrop::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetOutputCrop* self = new(ELeave) CTestDevVideoRecordSetOutputCrop(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetOutputCrop::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetOutputCrop"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

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

	// set output crop options on encoder
	TRect testRect1(KTestOutputCropRectA, KTestOutputCropRectB, KTestOutputCropRectC, KTestOutputCropRectD);
	TRAP(err, aDevVideoRecord.SetOutputCropOptionsL(hwEncoder, testRect1) );
	
	// set output crop options on pre-processor
	TRect testRect2(KTestOutputCropRectD, KTestOutputCropRectC, KTestOutputCropRectB, KTestOutputCropRectA);
	TRAP(err2, aDevVideoRecord.SetOutputCropOptionsL(hwPreProc, testRect2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetOutputCropOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetOutputCropOptionsL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetOutputCropOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetOutputPad::CTestDevVideoRecordSetOutputPad(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetOutputPad* CTestDevVideoRecordSetOutputPad::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetOutputPad* self = new(ELeave) CTestDevVideoRecordSetOutputPad(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetOutputPad::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetOutputPad"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

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

	TSize testPad1(KTestPadX, KTestPadY);
	TSize testPad2(KTestPadY, KTestPadX);
	
	TPoint testPoint1(KTestPadPointX, KTestPadPointY);
	TPoint testPoint2(KTestPadPointY, KTestPadPointX);
	
	// set output pad options on encoder
	TRAP(err, aDevVideoRecord.SetOutputPadOptionsL(hwEncoder, testPad1, testPoint1) );
	
	// set output pad options on pre-processor
	TRAP(err2, aDevVideoRecord.SetOutputPadOptionsL(hwPreProc, testPad2, testPoint2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetOutputPadOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetOutputPadOptionsL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetOutputPadOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetColorEnhance::CTestDevVideoRecordSetColorEnhance(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetColorEnhance* CTestDevVideoRecordSetColorEnhance::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetColorEnhance* self = new(ELeave) CTestDevVideoRecordSetColorEnhance(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetColorEnhance::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetColorEnhance"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

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

	// set color enhancement options on encoder
	TColorEnhancementOptions testCol1 = KTestColorEnhance1;
	TRAP(err, aDevVideoRecord.SetColorEnhancementOptionsL(hwEncoder, testCol1) );
	
	// set color enhancement options on pre-processor
	TColorEnhancementOptions testCol2 = KTestColorEnhance2;
	TRAP(err2, aDevVideoRecord.SetColorEnhancementOptionsL(hwPreProc, testCol2) );

	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetColorEnhancementOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetColorEnhancementOptionsL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetColorEnhancementOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetFrameStabilisation::CTestDevVideoRecordSetFrameStabilisation(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetFrameStabilisation* CTestDevVideoRecordSetFrameStabilisation::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetFrameStabilisation* self = new(ELeave) CTestDevVideoRecordSetFrameStabilisation(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetFrameStabilisation::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetFrameStabilisation"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

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

	// set frame stabilisation options on encoder
	TSize testSize1(KTestScaleX, KTestScaleY);
	TRAP(err, aDevVideoRecord.SetFrameStabilisationOptionsL(hwEncoder, testSize1, ETrue) );
	
	// set frame stabilisation options on pre-processor
	TSize testSize2(KTestScaleY, KTestScaleX);
	TRAP(err2, aDevVideoRecord.SetFrameStabilisationOptionsL(hwPreProc, testSize2, ETrue) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetFrameStabilisationOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetFrameStabilisationOptionsL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetFrameStabilisationOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetCustomISPreProc::CTestDevVideoRecordSetCustomISPreProc(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetCustomISPreProc* CTestDevVideoRecordSetCustomISPreProc::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetCustomISPreProc* self = new(ELeave) CTestDevVideoRecordSetCustomISPreProc(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetCustomISPreProc::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetCustomISPreProc"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

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

	// set custom pre proc options on encoder
	TRAP(err, aDevVideoRecord.SetCustomPreProcessOptionsL(hwEncoder, KTestCustomPreProc1) );
	
	// set custom pre proc options on pre-processor
	TRAP(err2, aDevVideoRecord.SetCustomPreProcessOptionsL(hwPreProc, KTestCustomPreProc2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetCustomPreProcessOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetCustomPreProcessOptionsL() on Pre-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetCustomPreProcessOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetErrors::CTestDevVideoRecordSetErrors(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetErrors* CTestDevVideoRecordSetErrors::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetErrors* self = new(ELeave) CTestDevVideoRecordSetErrors(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetErrors::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetErrors"));
	
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

	// this will PANIC if an error occurs
	aDevVideoRecord.SetErrorsExpected( ETrue, EFalse );

	// if there is no panic then test has passed
	INFO_PRINTF1(_L("SetErrorsExpected() called correctly on Encoder."));
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetMinRandomAccess::CTestDevVideoRecordSetMinRandomAccess(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetMinRandomAccess* CTestDevVideoRecordSetMinRandomAccess::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetMinRandomAccess* self = new(ELeave) CTestDevVideoRecordSetMinRandomAccess(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetMinRandomAccess::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetMinRandomAccess"));
	
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

	// this will PANIC if an error occurs
	aDevVideoRecord.SetMinRandomAccessRate( KTestRandomAccessRate );

	// if there is no panic then test has passed
	INFO_PRINTF1(_L("SetMinRandomAccessRate() called correctly on Encoder."));
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetNumBitrateLayers::CTestDevVideoRecordSetNumBitrateLayers(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetNumBitrateLayers* CTestDevVideoRecordSetNumBitrateLayers::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetNumBitrateLayers* self = new(ELeave) CTestDevVideoRecordSetNumBitrateLayers(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetNumBitrateLayers::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetNumBitrateLayers"));
	
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

	// this will leave if an error occurs
	TRAP(err, aDevVideoRecord.SetNumBitrateLayersL( KTestNumBitrateLayers ));

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetNumBitrateLayersL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetNumBitrateLayersL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetScalabilityType::CTestDevVideoRecordSetScalabilityType(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetScalabilityType* CTestDevVideoRecordSetScalabilityType::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetScalabilityType* self = new(ELeave) CTestDevVideoRecordSetScalabilityType(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetScalabilityType::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetScalabilityType"));
	
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

	// this will leave if an error occurs
	TRAP(err, aDevVideoRecord.SetScalabilityLayerTypeL( KTestLayer, KTestScaleType ));

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetScalabilityLayerTypeL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetScalabilityLayerTypeL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetGlobalReferencePic::CTestDevVideoRecordSetGlobalReferencePic(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetGlobalReferencePic* CTestDevVideoRecordSetGlobalReferencePic::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetGlobalReferencePic* self = new(ELeave) CTestDevVideoRecordSetGlobalReferencePic(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetGlobalReferencePic::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetGlobalReferencePic"));
	
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
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// eABI warning removal
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

	// this will PANIC if an error occurs
	aDevVideoRecord.SetGlobalReferenceOptions( KTestMaxRefPics, KTestMaxPicDelay );

	// if there is no panic then test has passed
	INFO_PRINTF1(_L("SetGlobalReferenceOptions() called correctly on Encoder."));
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetLayerReference::CTestDevVideoRecordSetLayerReference(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetLayerReference* CTestDevVideoRecordSetLayerReference::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetLayerReference* self = new(ELeave) CTestDevVideoRecordSetLayerReference(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetLayerReference::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetLayerReference"));
	
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

	// this will PANIC if an error occurs
	aDevVideoRecord.SetLayerReferenceOptions( KTestLayer, KTestMaxRefPics, KTestMaxPicDelay );

	// if there is no panic then test has passed
	INFO_PRINTF1(_L("SetLayerReferenceOptions() called correctly on Encoder."));
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetBufferOptions::CTestDevVideoRecordSetBufferOptions(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetBufferOptions* CTestDevVideoRecordSetBufferOptions::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetBufferOptions* self = new(ELeave) CTestDevVideoRecordSetBufferOptions(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetBufferOptions::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetBufferOptions"));
	
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

	// this will leave if an error occurs
	TEncoderBufferOptions testOptions = GetTestEncBufferOptions( );
	TRAP(err, aDevVideoRecord.SetBufferOptionsL( testOptions ));

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetBufferOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetBufferOptionsL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetCSEncoderOptions::CTestDevVideoRecordSetCSEncoderOptions(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetCSEncoderOptions* CTestDevVideoRecordSetCSEncoderOptions::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetCSEncoderOptions* self = new(ELeave) CTestDevVideoRecordSetCSEncoderOptions(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetCSEncoderOptions::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetCSEncoderOptions"));
	
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

	// this will leave if an error occurs
	TPtrC8 testOptions(KTestCSEncoderOptions);
	TRAP(err, aDevVideoRecord.SetCodingStandardSpecificOptionsL( testOptions ));

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetCodingStandardSpecificOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetCodingStandardSpecificOptionsL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoRecordSetISEncoderOptions::CTestDevVideoRecordSetISEncoderOptions(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetISEncoderOptions* CTestDevVideoRecordSetISEncoderOptions::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetISEncoderOptions* self = new(ELeave) CTestDevVideoRecordSetISEncoderOptions(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetISEncoderOptions::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetISEncoderOptions"));
	
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

	// this will leave if an error occurs
	TPtrC8 testOptions(KTestISEncoderOptions);
	TRAP(err, aDevVideoRecord.SetImplementationSpecificEncoderOptionsL( testOptions ));

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetImplementationSpecificEncoderOptionsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetImplementationSpecificEncoderOptionsL(), %d = %d"), err, expErr);
	
	return ret;
	}

//------------------------------------------------------------------

void CTestDevVideoRecordInit::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordInit::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordInit::CTestDevVideoRecordInit(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordInit* CTestDevVideoRecordInit::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordInit* self = new(ELeave) CTestDevVideoRecordInit(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordInit::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  Init"));
	
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
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// eABI warning removal
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

	if (iError != expErr)
		{
		ERR_PRINTF3(_L("Initialize()() gave error %d (expected %d)"),iError, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("Initialize()(), %d = %d"), iError, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordGetCSInitOutput::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordGetCSInitOutput::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordGetCSInitOutput::CTestDevVideoRecordGetCSInitOutput(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordGetCSInitOutput* CTestDevVideoRecordGetCSInitOutput::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordGetCSInitOutput* self = new(ELeave) CTestDevVideoRecordGetCSInitOutput(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordGetCSInitOutput::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EFail;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  GetCSInitOutput"));
	
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

	// get the CS specific output
	HBufC8* csOutput = NULL;
	TRAP(err, 
			csOutput = aDevVideoRecord.CodingStandardSpecificInitOutputLC();
			CleanupStack::Pop(csOutput); );

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("CodingStandardSpecificInitOutputLC() on Encoder gave error %d (expected %d)"),err, expErr);
		}
	else if (!csOutput)
		{
		ERR_PRINTF1(_L("Received null CS output!")); 
		}
	else if ( !(*csOutput == KTestCSInitOutput) )
		{
		ERR_PRINTF2(_L("Received incorrect CS output:  %S"), csOutput); 
		}
	else
		{
		INFO_PRINTF1(_L("Correctly compared CS output to test data"));
		INFO_PRINTF3(_L("CodingStandardSpecificInitOutputLC(), %d = %d"), err, expErr);
		ret = EPass;
		}
	
	delete csOutput;
	csOutput = NULL;

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordGetISInitOutput::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordGetISInitOutput::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordGetISInitOutput::CTestDevVideoRecordGetISInitOutput(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordGetISInitOutput* CTestDevVideoRecordGetISInitOutput::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordGetISInitOutput* self = new(ELeave) CTestDevVideoRecordGetISInitOutput(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordGetISInitOutput::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EFail;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  GetISInitOutput"));
	
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

	// get the IS specific output
	HBufC8* isOutput = NULL;
	TRAP(err, 
			isOutput = aDevVideoRecord.ImplementationSpecificInitOutputLC(); 
			CleanupStack::Pop(isOutput); );

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("ImplementationSpecificInitOutputLC() on Encoder gave error %d (expected %d)"),err, expErr);
		}
	else if (!isOutput)
		{
		ERR_PRINTF1(_L("Received null IS output!")); 
		}
	else if ( !(*isOutput == KTestISInitOutput) )
		{
		ERR_PRINTF2(_L("Received incorrect IS output:  %S"), isOutput); 
		}
	else
		{
		INFO_PRINTF1(_L("Correctly compared IS output to test data"));
		INFO_PRINTF3(_L("ImplementationSpecificInitOutputLC(), %d = %d"), err, expErr);
		ret = EPass;
		}
	
	delete isOutput;
	isOutput = NULL;
		
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSetErrorProtectionLevels::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSetErrorProtectionLevels::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSetErrorProtectionLevels::CTestDevVideoRecordSetErrorProtectionLevels(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetErrorProtectionLevels* CTestDevVideoRecordSetErrorProtectionLevels::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetErrorProtectionLevels* self = new(ELeave) CTestDevVideoRecordSetErrorProtectionLevels(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetErrorProtectionLevels::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetErrorProtectionLevels"));
	
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

	// this will leave if an error occurs
	TRAP(err, aDevVideoRecord.SetErrorProtectionLevelsL( KTestProtectLevels, ETrue ));

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetErrorProtectionLevelsL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetErrorProtectionLevelsL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSetupErrorProtection::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSetupErrorProtection::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSetupErrorProtection::CTestDevVideoRecordSetupErrorProtection(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetupErrorProtection* CTestDevVideoRecordSetupErrorProtection::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetupErrorProtection* self = new(ELeave) CTestDevVideoRecordSetupErrorProtection(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetupErrorProtection::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetupErrorProtection"));
	
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
	TRAP(err, aDevVideoRecord.SelectEncoderL(KUidDevVideoTestEncodeHwDevice));	// eABI warning removal
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

	// this will leave if an error occurs
	TRAP(err, aDevVideoRecord.SetErrorProtectionLevelL( KTestLevel, KTestBitrate, KTestStrength ));

	if ( err != expErr )
		{
		ERR_PRINTF3(_L("SetErrorProtectionLevelL() on Encoder gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetErrorProtectionLevelL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoRecordSetChannelBufferLossRate::MdvroInitializeComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoRecordSetChannelBufferLossRate::MdvroInitializeComplete();  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoRecordSetChannelBufferLossRate::CTestDevVideoRecordSetChannelBufferLossRate(const TDesC& aTestName, const TTestType aTestType)
	:CTestDevVideoRecordStep(aTestName, aTestType)
	{
	}

CTestDevVideoRecordSetChannelBufferLossRate* CTestDevVideoRecordSetChannelBufferLossRate::NewL(const TDesC& aTestName, const TTestType aTestType)
	{
	CTestDevVideoRecordSetChannelBufferLossRate* self = new(ELeave) CTestDevVideoRecordSetChannelBufferLossRate(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoRecordSetChannelBufferLossRate::DoTestL(CMMFDevVideoRecord& aDevVideoRecord)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoRecord:  SetChannelBufferLossRate"));
	
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
//	TRAP(err, hwPreProc = aDevVideoRecord.SelectPreProcessorL(KUidDevVideoTestPreProcHwDevice));	// EABI warning removal
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
	
	TTimeIntervalMicroSeconds32 testBurst(KTestLossBurstLength);
	aDevVideoRecord.SetChannelPacketLossRate( KTestLevel, KTestLossRate, testBurst );

	// if no PANIC then test has passed
	INFO_PRINTF3(_L("SetChannelBufferLossRate(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

