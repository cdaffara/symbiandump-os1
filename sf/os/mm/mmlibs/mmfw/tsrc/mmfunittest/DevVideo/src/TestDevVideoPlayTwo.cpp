
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
// TestDevVideoPlay.cpp
// 
//

#include "TestDevVideoPlay.h"
#include "TestDevVideoPlayTestData.h"
#include "TestDevVideoPlugins/decoder.h"
#include "TestDevVideoPlugins/postproc.h"
#include <e32math.h>

CTestDevVideoPlaySetInputCrop::CTestDevVideoPlaySetInputCrop(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetInputCrop* CTestDevVideoPlaySetInputCrop::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetInputCrop* self = new(ELeave) CTestDevVideoPlaySetInputCrop(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetInputCrop::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetInputCrop"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// set input crop on decoder
	TRect testRect1(KTestInputCropRectA, KTestInputCropRectB, KTestInputCropRectC, KTestInputCropRectD);
	TRAP(err, aDevVideoPlay.SetInputCropOptionsL(hwDecoder, testRect1) )
	
	// set input crop on post-processor
	TRect testRect2(KTestInputCropRectD, KTestInputCropRectC, KTestInputCropRectB, KTestInputCropRectA);
	TRAP(err2, aDevVideoPlay.SetInputCropOptionsL(hwPostProc, testRect2) )


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetInputCropOptionsL() on Decoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetInputCropOptionsL() on Post-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetInputCropOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetYuvToRgbOptionsFormat::CTestDevVideoPlaySetYuvToRgbOptionsFormat(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetYuvToRgbOptionsFormat* CTestDevVideoPlaySetYuvToRgbOptionsFormat::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetYuvToRgbOptionsFormat* self = new(ELeave) CTestDevVideoPlaySetYuvToRgbOptionsFormat(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetYuvToRgbOptionsFormat::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetYuvToRgbOptionsFormat"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// set YUV to RGB options with formats on decoder
	TRAP(err, aDevVideoPlay.SetYuvToRgbOptionsL(hwDecoder, KTestYuvToRgb1, KTestYuvFormat1, KTestRgbFormat1) );
	
	// set YUV to RGB options with formats on post-processor
	TRAP(err2, aDevVideoPlay.SetYuvToRgbOptionsL(hwPostProc, KTestYuvToRgb2, KTestYuvFormat2, KTestRgbFormat2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetYuvToRgbOptionsL() on Decoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetYuvToRgbOptionsL() on Post-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetYuvToRgbOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetYuvToRgbOptions::CTestDevVideoPlaySetYuvToRgbOptions(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetYuvToRgbOptions* CTestDevVideoPlaySetYuvToRgbOptions::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetYuvToRgbOptions* self = new(ELeave) CTestDevVideoPlaySetYuvToRgbOptions(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetYuvToRgbOptions::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetYuvToRgbOptions"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// set YUV to RGB options with formats on decoder
	TRAP(err, aDevVideoPlay.SetYuvToRgbOptionsL(hwDecoder, KTestYuvToRgb1) );
	
	// set YUV to RGB options with formats on post-processor
	TRAP(err2, aDevVideoPlay.SetYuvToRgbOptionsL(hwPostProc, KTestYuvToRgb2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetYuvToRgbOptionsL() on Decoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetYuvToRgbOptionsL() on Post-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetYuvToRgbOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetRotate::CTestDevVideoPlaySetRotate(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetRotate* CTestDevVideoPlaySetRotate::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetRotate* self = new(ELeave) CTestDevVideoPlaySetRotate(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetRotate::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetRotate"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// set rotate options on decoder
	TRAP(err, aDevVideoPlay.SetRotateOptionsL(hwDecoder, KTestRotate1) );
	
	// set rotate options on post-processor
	TRAP(err2, aDevVideoPlay.SetRotateOptionsL(hwPostProc, KTestRotate2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetRotateOptionsL() on Decoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetRotateOptionsL() on Post-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetRotateOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetScale::CTestDevVideoPlaySetScale(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetScale* CTestDevVideoPlaySetScale::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetScale* self = new(ELeave) CTestDevVideoPlaySetScale(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetScale::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetScale"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// set scale options on decoder
	TSize testScale1(KTestScaleX, KTestScaleY);
	TRAP(err, aDevVideoPlay.SetScaleOptionsL(hwDecoder, testScale1, ETrue) );
	
	TSize testScale2(KTestScaleY, KTestScaleX);
	// set scale options on post-processor
	TRAP(err2, aDevVideoPlay.SetScaleOptionsL(hwPostProc, testScale2, ETrue) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetScaleOptionsL() on Decoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetScaleOptionsL() on Post-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetScaleOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetCrop::CTestDevVideoPlaySetCrop(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetCrop* CTestDevVideoPlaySetCrop::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetCrop* self = new(ELeave) CTestDevVideoPlaySetCrop(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetCrop::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetCrop"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// set output crop options on decoder
	TRect testRect1(KTestOutputCropRectA, KTestOutputCropRectB, KTestOutputCropRectC, KTestOutputCropRectD);
	TRAP(err, aDevVideoPlay.SetOutputCropOptionsL(hwDecoder, testRect1) );
	
	// set output crop options on post-processor
	TRect testRect2(KTestOutputCropRectD, KTestOutputCropRectC, KTestOutputCropRectB, KTestOutputCropRectA);
	TRAP(err2, aDevVideoPlay.SetOutputCropOptionsL(hwPostProc, testRect2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetOutputCropOptionsL() on Decoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetOutputCropOptionsL() on Post-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetOutputCropOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetPostProcOptions::CTestDevVideoPlaySetPostProcOptions(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetPostProcOptions* CTestDevVideoPlaySetPostProcOptions::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetPostProcOptions* self = new(ELeave) CTestDevVideoPlaySetPostProcOptions(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetPostProcOptions::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetPostProcOptions"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;
	TInt err2 = KErrNone;

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// set post-processor options on decoder
	TRAP(err, aDevVideoPlay.SetPostProcSpecificOptionsL(hwDecoder, KTestPostProcOptions1) );
	
	// set post-processor options on post-processor
	TRAP(err2, aDevVideoPlay.SetPostProcSpecificOptionsL(hwPostProc, KTestPostProcOptions2) );


	if ((err != expErr) || (err2 != expErr))
		{
		ERR_PRINTF3(_L("SetPostProcSpecificOptionsL() on Decoder gave error %d (expected %d)"),err, expErr);
		ERR_PRINTF3(_L("SetPostProcSpecificOptionsL() on Post-Processor gave error %d (expected %d)"),err2, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF4(_L("SetPostProcSpecificOptionsL(), %d, %d= %d"), err, err2, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetClockSource::CTestDevVideoPlaySetClockSource(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetClockSource* CTestDevVideoPlaySetClockSource::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetClockSource* self = new(ELeave) CTestDevVideoPlaySetClockSource(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetClockSource::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPost = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetClockSource"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;	// EABI warning removal
			break;
		case ETestDecoderOnly:
			selPost = EFalse;
			break;
		case ETestPostProcOnly:
			selDec = EFalse;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
	if (selDec)
		{
//		TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
		TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
				return EInconclusive;
			}
		}

	// select post-processor
	if (selPost)
		{
		TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
//		TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
				return EInconclusive;
			}
		}

	CSystemClockSource *time1 = NULL;

	// no parameters so no need to have a separate 'test clock source'
	TRAP(err, time1 = CSystemClockSource::NewL());
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't construct system clock class"));
		
		return EInconclusive;
		}
	
	// set clock source 
	aDevVideoPlay.SetClockSource(time1);
	
	// if there is no panic then test has passed

	INFO_PRINTF1(_L("SetClockSourceL() called correctly on HW Devices."));
	
	// delete clock source
	delete time1; time1 = NULL;

	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetVideoOutput::CTestDevVideoPlaySetVideoOutput(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetVideoOutput* CTestDevVideoPlaySetVideoOutput::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetVideoOutput* self = new(ELeave) CTestDevVideoPlaySetVideoOutput(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetVideoOutput::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetVideoOutput"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	expErr = KErrHardwareNotAvailable;//Post processor plugin will leave with this if it gets
									  //the method call correctly

	TRAP(err, aDevVideoPlay.SetVideoDestScreenL(ETrue) )

	if (err != expErr)
		{
		ERR_PRINTF3(_L("SetVideoDestScreenL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetVideoDestScreenL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySynchronizeDecoding::CTestDevVideoPlaySynchronizeDecoding(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySynchronizeDecoding* CTestDevVideoPlaySynchronizeDecoding::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySynchronizeDecoding* self = new(ELeave) CTestDevVideoPlaySynchronizeDecoding(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySynchronizeDecoding::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SynchronizeDecoding"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// eABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// eABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// call synchronize decoding on Decoder
	aDevVideoPlay.SynchronizeDecoding(ETrue);
	// if this doesn't panic then test passes

	INFO_PRINTF1(_L("SynchronizeDecoding() called successfully."));
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetBufferOptions::CTestDevVideoPlaySetBufferOptions(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetBufferOptions* CTestDevVideoPlaySetBufferOptions::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetBufferOptions* self = new(ELeave) CTestDevVideoPlaySetBufferOptions(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetBufferOptions::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetBufferOptions"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// initialize buffer options to the test data
	CMMFDevVideoPlay::TBufferOptions buffOptions = GetTestBufferOptions();

	// set buffer options on decoder [will leave on error]
	TRAP(err, aDevVideoPlay.SetBufferOptionsL(buffOptions) );

	if (err != expErr)
		{
		ERR_PRINTF3(_L("SetBufferOptionsL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("SetBufferOptions(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlayGetBufferOptions::CTestDevVideoPlayGetBufferOptions(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetBufferOptions* CTestDevVideoPlayGetBufferOptions::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetBufferOptions* self = new(ELeave) CTestDevVideoPlayGetBufferOptions(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetBufferOptions::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetBufferOptions"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// get buffer options from the decoder
	CMMFDevVideoPlay::TBufferOptions buffOptions;
	aDevVideoPlay.GetBufferOptions(buffOptions);

	// compare this to the test data
	if ( !CompareBufferOptions(buffOptions, GetTestBufferOptions()) )
		{
		ERR_PRINTF1(_L("GetBufferOptions() comparison with test data failed!"));
		ret = EFail;
		}
	else 
		{
		INFO_PRINTF1(_L("GetBufferOptions() comparison with test data passed."));
		INFO_PRINTF3(_L("GetBufferOptions(), %d = %d"), err, expErr);
		}
	
	return ret;
	}
//------------------------------------------------------------------

CTestDevVideoPlaySetHrdVbv::CTestDevVideoPlaySetHrdVbv(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetHrdVbv* CTestDevVideoPlaySetHrdVbv::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetHrdVbv* self = new(ELeave) CTestDevVideoPlaySetHrdVbv(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetHrdVbv::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetHrdVbv"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	HBufC8* hrdVbvParams = NULL; 
	TRAP(err, hrdVbvParams = KTestHrdVbvParams().AllocL());
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize hrd/vbv params"));
			return EInconclusive;
		}

	// set hrd/vbv spec on decoder
	aDevVideoPlay.SetHrdVbvSpec(KTestHrdVbvSpec, *hrdVbvParams);
	// if this doesn't panic then test has passed

	// delete copy
	delete hrdVbvParams; hrdVbvParams = NULL;

	INFO_PRINTF1(_L("SetHrdVbvSpec() called successfully."));
	
	return ret;
	}

//------------------------------------------------------------------

void CTestDevVideoPlayInit::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayInit::CTestDevVideoPlayInit(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayInit* CTestDevVideoPlayInit::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayInit* self = new(ELeave) CTestDevVideoPlayInit(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayInit::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  Init"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// initialize CDevVideoPlay and wait for response in iError
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();

	if (iError != expErr)
		{
		ERR_PRINTF3(_L("Initialize() gave error %d (expected %d)"),iError, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("Initialize(), %d = %d"), iError, expErr);
	
	return ret;
	}

//------------------------------------------------------------------

void CTestDevVideoPlayStartDSA::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayStartDSA::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayStartDSA::CTestDevVideoPlayStartDSA(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayStartDSA* CTestDevVideoPlayStartDSA::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayStartDSA* self = new(ELeave) CTestDevVideoPlayStartDSA(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayStartDSA::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  StartDSA"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
			return EInconclusive;
		}

	// screen device is independant of DevVideo so we can construct one here
	// and just check that it is non NULL at the plugin
	CFbsScreenDevice *screenDev = NULL;
	TRAP(err, screenDev = CFbsScreenDevice::NewL(_L("scdv"), EColor16));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize screen device"));
		//return EInconclusive;
		}

	TRect dsaRect(KTestDSARectA, KTestDSARectB, KTestDSARectC, KTestDSARectD);
	TRegionFix<1> reg1(dsaRect);
	
	// start direct screen access
	TRAP(err, aDevVideoPlay.StartDirectScreenAccessL(dsaRect, *screenDev, reg1) );
	
	// delete screen device and rect
	delete screenDev;  
	screenDev = NULL;

	reg1.Clear();

	if (err != expErr)
		{
		ERR_PRINTF3(_L("StartDirectScreenAccessL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF3(_L("StartDirectScreenAccessL(), %d = %d"), err, expErr);
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlaySetClipRegion::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlaySetClipRegion::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlaySetClipRegion::CTestDevVideoPlaySetClipRegion(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetClipRegion* CTestDevVideoPlaySetClipRegion::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetClipRegion* self = new(ELeave) CTestDevVideoPlaySetClipRegion(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetClipRegion::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetClipRegion"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// eABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
			return EInconclusive;
		}

	TRect dsaRect(KTestDSARectA, KTestDSARectB, KTestDSARectC, KTestDSARectD);
	TRegionFix<1> reg1(dsaRect);
	
	// set clip region 
	aDevVideoPlay.SetScreenClipRegion(reg1);
	
	// clear region
	reg1.Clear();

	// if no PANIC then test has passed
	INFO_PRINTF1(_L("SetScreenClipRegion() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlaySetPauseClipFail::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlaySetPauseClipFail::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlaySetPauseClipFail::CTestDevVideoPlaySetPauseClipFail(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetPauseClipFail* CTestDevVideoPlaySetPauseClipFail::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetPauseClipFail* self = new(ELeave) CTestDevVideoPlaySetPauseClipFail(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetPauseClipFail::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetPauseClipFail"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
			return EInconclusive;
		}

	// set pause on clip fail to ETrue
	aDevVideoPlay.SetPauseOnClipFail(ETrue);

	// if no PANIC then test has passed
	INFO_PRINTF1(_L("SetPauseClipFail() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayAbortDSA::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayAbortDSA::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayAbortDSA::CTestDevVideoPlayAbortDSA(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayAbortDSA* CTestDevVideoPlayAbortDSA::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayAbortDSA* self = new(ELeave) CTestDevVideoPlayAbortDSA(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayAbortDSA::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  AbortDSA"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// eABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
			return EInconclusive;
		}

	// abort direct screen access [is there a way to check this?]
	aDevVideoPlay.AbortDirectScreenAccess();

	INFO_PRINTF1(_L("AbortDirectScreenAccess() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayIsPlaying::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayIsPlaying::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayIsPlaying::CTestDevVideoPlayIsPlaying(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayIsPlaying* CTestDevVideoPlayIsPlaying::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayIsPlaying* self = new(ELeave) CTestDevVideoPlayIsPlaying(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayIsPlaying::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  IsPlaying"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
			return EInconclusive;
		}

	TBool playing = ETrue;

	// check DevVideoPlay is not playing
	playing = aDevVideoPlay.IsPlaying();

	if (playing)
		{
		ERR_PRINTF1(_L("IsPlaying() reported ETrue - expected EFalse!"));
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("IsPlaying() returned EFalse as expected"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayRedraw::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayRedraw::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayRedraw::CTestDevVideoPlayRedraw(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayRedraw* CTestDevVideoPlayRedraw::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayRedraw* self = new(ELeave) CTestDevVideoPlayRedraw(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayRedraw::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  Redraw"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
//	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
			return EInconclusive;
		}

	// call redraw [is there a way to check this?]
	aDevVideoPlay.Redraw();

	INFO_PRINTF1(_L("Redraw() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayStart::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayStart::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayStart::CTestDevVideoPlayStart(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayStart* CTestDevVideoPlayStart::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayStart* self = new(ELeave) CTestDevVideoPlayStart(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayStart::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPost = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  Start"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;	// EABI warning removal
			break;
		case ETestDecoderOnly:
			selPost = EFalse;
			break;
		case ETestPostProcOnly:
			selDec = EFalse;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
	if (selDec)
		{
//		TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
		TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
				return EInconclusive;
			}
		}

	// select post-processor
	if (selPost)
		{
//		TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
		TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABIO warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
				return EInconclusive;
			}
		}

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
			return EInconclusive;
		}

	// check not already playing
	TBool playing = aDevVideoPlay.IsPlaying();
	if (playing)
		{
		ERR_PRINTF1(_L("Error - DevVideoPlay already playing!"));
			return EInconclusive;
		}

	// call start on DevVideoPlay then check IsPlaying()
	aDevVideoPlay.Start();
	playing = aDevVideoPlay.IsPlaying();

	if (!playing)
		{
		ERR_PRINTF1(_L("IsPlaying() returned EFalse - expected ETrue"));
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("Start() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayStop::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayStop::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayStop::CTestDevVideoPlayStop(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayStop* CTestDevVideoPlayStop::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayStop* self = new(ELeave) CTestDevVideoPlayStop(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayStop::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPost = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  Stop"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;	// EABI warning removal
			break;
		case ETestDecoderOnly:
			selPost = EFalse;
			break;
		case ETestPostProcOnly:
			selDec = EFalse;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
	if (selDec)
		{
//		TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
		TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
				return EInconclusive;
			}
		}

	// select post-processor
	if (selPost)
		{
//		TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
		TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
				return EInconclusive;
			}
		}
	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
			return EInconclusive;
		}

	// check DevVideoPlay has started playing then call stop
	aDevVideoPlay.Start();
	TBool playing = aDevVideoPlay.IsPlaying();

	if (!playing)
		{
		ERR_PRINTF1(_L("Error - couldn't start playback!"));
			return EInconclusive;
		}

	// call stop
	aDevVideoPlay.Stop();
	playing = aDevVideoPlay.IsPlaying();

	if (playing)
		{
		ERR_PRINTF1(_L("IsPlaying() returned ETrue - expected EFalse"));
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("Stop() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayPause::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayPause::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayPause::CTestDevVideoPlayPause(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayPause* CTestDevVideoPlayPause::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayPause* self = new(ELeave) CTestDevVideoPlayPause(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayPause::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPost = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  Pause"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;	// EABI warning removal
			break;
		case ETestDecoderOnly:
			selPost = EFalse;
			break;
		case ETestPostProcOnly:
			selDec = EFalse;
			break;
		default:
			ERR_PRINTF1(_L("Error - invalid test step type"));
			return EInconclusive;
		}

	TInt err = KErrNone;

//	THwDeviceId hwDecoder = 0;
//	THwDeviceId hwPostProc = 0;

	// select decoder
	if (selDec)
		{
//		TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
		TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
				return EInconclusive;
			}
		}

	// select post-processor
	if (selPost)
		{
//		TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
		TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
				return EInconclusive;
			}
		}

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
			return EInconclusive;
		}

	// check DevVideoPlay has started playing then call pause
	aDevVideoPlay.Start();
	TBool playing = aDevVideoPlay.IsPlaying();

	if (!playing)
		{
		ERR_PRINTF1(_L("Error - couldn't start playback!"));
			return EInconclusive;
		}

	// call pause
	aDevVideoPlay.Pause();
	playing = aDevVideoPlay.IsPlaying();

	if (playing)
		{
		ERR_PRINTF1(_L("IsPlaying() returned ETrue - expected EFalse"));
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("Pause() called successfully"));

	return ret;
	}
//------------------------------------------------------------------

