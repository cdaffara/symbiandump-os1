
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

void CTestDevVideoPlayResume::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayResume::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayResume::CTestDevVideoPlayResume(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayResume* CTestDevVideoPlayResume::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayResume* self = new(ELeave) CTestDevVideoPlayResume(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayResume::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPost = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  Resume"));
	
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

	// check not already playing
	TBool playing = aDevVideoPlay.IsPlaying();
	if (playing)
		{
		ERR_PRINTF1(_L("Error - DevVideoPlay already playing!"));
		return EInconclusive;
		}

	// call resume on DevVideoPlay then check IsPlaying()
	aDevVideoPlay.Resume();
	playing = aDevVideoPlay.IsPlaying();

	if (!playing)
		{
		ERR_PRINTF1(_L("IsPlaying() returned EFalse - expected ETrue"));
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("Resume() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlaySetPosition::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlaySetPosition::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlaySetPosition::CTestDevVideoPlaySetPosition(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetPosition* CTestDevVideoPlaySetPosition::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetPosition* self = new(ELeave) CTestDevVideoPlaySetPosition(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetPosition::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPost = ETrue;
	TInt testPos = KTestPosition;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetPosition"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
			break;
		case ETestBothFatal:
			expErr = KErrDied;
			testPos = KTestPositionFatal;
			break;
		case ETestDecoderFatal:
			expErr = KErrDied;
			testPos = KTestPositionFatal;
			// fall through to set post proc to false;
		case ETestDecoderOnly:
			selPost = EFalse;
			break;
		case ETestPostProcFatal:
			expErr = KErrDied;
			testPos = KTestPositionFatal;
			// fall through to set decoder to false;
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

	TTimeIntervalMicroSeconds pos(testPos);

	// call SetPosition 
	aDevVideoPlay.SetPosition(pos);

	// if no PANIC and iError is as expected then test has passed
	// if performing a fatal test then expect iError = KErrDied
	if (iError == expErr)
		{
		ERR_PRINTF3(_L("SetPosition() got error %d;  Expected %d"), iError, expErr);
		}
	else
		{
		INFO_PRINTF1(_L("SetPosition() called successfully"));
		}
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayFreezePicture::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayFreezePicture::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayFreezePicture::CTestDevVideoPlayFreezePicture(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayFreezePicture* CTestDevVideoPlayFreezePicture::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayFreezePicture* self = new(ELeave) CTestDevVideoPlayFreezePicture(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayFreezePicture::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  FreezePicture"));
	
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

	TTimeIntervalMicroSeconds pos(KTestPosition);

	// call FreezePicture 
	aDevVideoPlay.FreezePicture(pos);

	// if no PANIC then test has passed

	INFO_PRINTF1(_L("FreezePicture() called successfully"));

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayReleaseFreeze::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayReleaseFreeze::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayReleaseFreeze::CTestDevVideoPlayReleaseFreeze(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayReleaseFreeze* CTestDevVideoPlayReleaseFreeze::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayReleaseFreeze* self = new(ELeave) CTestDevVideoPlayReleaseFreeze(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayReleaseFreeze::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  ReleaseFreeze"));
	
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

	TTimeIntervalMicroSeconds pos(KTestPosition);

	// call ReleaseFreeze 
	aDevVideoPlay.ReleaseFreeze(pos);

	// if no PANIC then test has passed

	INFO_PRINTF1(_L("ReleaseFreeze() called successfully"));

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayDecodingPosition::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayDecodingPosition::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
		}

CTestDevVideoPlayDecodingPosition::CTestDevVideoPlayDecodingPosition(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayDecodingPosition* CTestDevVideoPlayDecodingPosition::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayDecodingPosition* self = new(ELeave) CTestDevVideoPlayDecodingPosition(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayDecodingPosition::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  DecodingPosition"));
	
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

	// get decoding position
	TTimeIntervalMicroSeconds decPos = aDevVideoPlay.DecodingPosition();

	// check against test data
	TTimeIntervalMicroSeconds testTime(KTestDecodePosition);
	if (decPos != testTime)
		{
		ERR_PRINTF3(_L("DecodingPosition() incorrect - gave %d (expected %d)"),
			I64LOW(decPos.Int64()), KTestDecodePosition);
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("DecodingPosition() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayPlaybackPosition::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayPlaybackPosition::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayPlaybackPosition::CTestDevVideoPlayPlaybackPosition(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayPlaybackPosition* CTestDevVideoPlayPlaybackPosition::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayPlaybackPosition* self = new(ELeave) CTestDevVideoPlayPlaybackPosition(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayPlaybackPosition::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  PlaybackPosition"));
	
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

	// get playback position
	TTimeIntervalMicroSeconds playPos = aDevVideoPlay.PlaybackPosition();

	// check against test data
	TTimeIntervalMicroSeconds testTime(KTestPlayPosition);
	if (playPos != testTime)
		{
		ERR_PRINTF3(_L("PlaybackPosition() incorrect - gave %d (expected %d)"),
			I64LOW(playPos.Int64()), KTestPlayPosition);
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("PlaybackPosition() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayPreDecoderBufferBytes::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayPreDecoderBufferBytes::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayPreDecoderBufferBytes::CTestDevVideoPlayPreDecoderBufferBytes(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayPreDecoderBufferBytes* CTestDevVideoPlayPreDecoderBufferBytes::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayPreDecoderBufferBytes* self = new(ELeave) CTestDevVideoPlayPreDecoderBufferBytes(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayPreDecoderBufferBytes::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  PreDecoderBufferBytes"));
	
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

	// get pre-decoder buffer bytes
	TUint decBytes = aDevVideoPlay.PreDecoderBufferBytes();

	// check against test data
	if (decBytes != KTestPreDecoderBytes)
		{
		ERR_PRINTF3(_L("PreDecoderBufferBytes() incorrect - gave %d (expected %d)"),
			decBytes, KTestPreDecoderBytes);
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("PreDecoderBufferBytes() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayPictureBufferBytes::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayPictureBufferBytes::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayPictureBufferBytes::CTestDevVideoPlayPictureBufferBytes(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayPictureBufferBytes* CTestDevVideoPlayPictureBufferBytes::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayPictureBufferBytes* self = new(ELeave) CTestDevVideoPlayPictureBufferBytes(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayPictureBufferBytes::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  PictureBufferBytes"));
	
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
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice)); // EABI warning removal
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

	// get picture buffer bytes
	TUint picBytes = aDevVideoPlay.PictureBufferBytes();

	// check against test data
	if (picBytes != KTestPictureBytes)
		{
		ERR_PRINTF3(_L("PictureBufferBytes() incorrect - gave %d (expected %d)"),
			picBytes, KTestPictureBytes);
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("PictureBufferBytes() called successfully"));
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayGetPictureCounters::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetPictureCounters::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayGetPictureCounters::CTestDevVideoPlayGetPictureCounters(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetPictureCounters* CTestDevVideoPlayGetPictureCounters::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetPictureCounters* self = new(ELeave) CTestDevVideoPlayGetPictureCounters(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetPictureCounters::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPost = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetPictureCounters"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
			expErr = KErrNone;
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

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;

	// select decoder
	if (selDec)
		{
		TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
			}
		}

	// select post-processor
	if (selPost)
		{
		TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
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

	CMMFDevVideoPlay::TPictureCounters picCounts;

	// get picture counters from DevVideoPlay
	aDevVideoPlay.GetPictureCounters(picCounts);

	// N.B. PicturesSkipped is sum of decoder and post processor so
	// we have to add these
	CMMFDevVideoPlay::TPictureCounters testCounts = GetTestPictureCounters();
	if (hwDecoder && hwPostProc)
		{
		// account for both decoder and post processor
		testCounts.iPicturesSkipped *= 2; 
		}
	else if (hwPostProc)
		{
		// no decoded samples from a post processor
		testCounts.iPicturesDecoded = 0;
		}

	if (!ComparePictureCounters(picCounts, testCounts))
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

void CTestDevVideoPlayGetBitstreamCounters::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetBitstreamCounters::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayGetBitstreamCounters::CTestDevVideoPlayGetBitstreamCounters(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetBitstreamCounters* CTestDevVideoPlayGetBitstreamCounters::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetBitstreamCounters* self = new(ELeave) CTestDevVideoPlayGetBitstreamCounters(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetBitstreamCounters::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetBitstreamCounters"));
	
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
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));// EABI warning removal
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

	CMMFDevVideoPlay::TBitstreamCounters bitCounts;

	// get bitstream counters from DevVideoPlay
	aDevVideoPlay.GetBitstreamCounters(bitCounts);

	if (!CompareBitstreamCounters(bitCounts, GetTestBitstreamCounters()))
		{
		ERR_PRINTF1(_L("GetBitstreamCounters() comparison with test data failed!"));
		ret = EFail;
		}
	else 
		{
		INFO_PRINTF1(_L("GetBitstreamCounters() comparison with test data passed."));
		INFO_PRINTF3(_L("GetBitstreamCounters(), %d = %d"), err, expErr);
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayNumFreeBuffers::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayNumFreeBuffers::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayNumFreeBuffers::CTestDevVideoPlayNumFreeBuffers(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayNumFreeBuffers* CTestDevVideoPlayNumFreeBuffers::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayNumFreeBuffers* self = new(ELeave) CTestDevVideoPlayNumFreeBuffers(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayNumFreeBuffers::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  NumFreeBuffers"));
	
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

	TUint numBuffers = 0;

	// get number of free buffers from DevVideoPlay
	numBuffers = aDevVideoPlay.NumFreeBuffers();

	if (numBuffers != KTestNumFreeBuffers)
		{
		ERR_PRINTF1(_L("NumFreeBuffers() comparison with test data failed!"));
		ret = EFail;
		}
	else 
		{
		INFO_PRINTF1(_L("NumFreeBuffers() comparison with test data passed."));
		INFO_PRINTF3(_L("NumFreeBuffers(), %d = %d"), numBuffers, KTestNumFreeBuffers);
		}
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlaySetComplexityLevel::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlaySetComplexityLevel::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlaySetComplexityLevel::CTestDevVideoPlaySetComplexityLevel(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlaySetComplexityLevel* CTestDevVideoPlaySetComplexityLevel::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlaySetComplexityLevel* self = new(ELeave) CTestDevVideoPlaySetComplexityLevel(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlaySetComplexityLevel::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  SetComplexityLevel"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

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

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
		return EInconclusive;
		}

	// set complexity level on decoder
	aDevVideoPlay.SetComplexityLevel(hwDecoder, KTestComplexityLevel1);
	
	// set complexity level on post-processor
	aDevVideoPlay.SetComplexityLevel(hwPostProc, KTestComplexityLevel2);

	// if no PANIC then test has passed
	INFO_PRINTF3(_L("SetComplexityLevel(), %d, %d"), KTestComplexityLevel1, KTestComplexityLevel2);
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayNumComplexityLevels::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayNumComplexityLevels::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayNumComplexityLevels::CTestDevVideoPlayNumComplexityLevels(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayNumComplexityLevels* CTestDevVideoPlayNumComplexityLevels::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayNumComplexityLevels* self = new(ELeave) CTestDevVideoPlayNumComplexityLevels(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayNumComplexityLevels::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  NumComplexityLevels"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

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

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
		return EInconclusive;
		}

	// get num complexity levels on decoder
	TUint levs1 = aDevVideoPlay.NumComplexityLevels(hwDecoder);
	
	// get num complexity levels on post-processor
	TUint levs2 = aDevVideoPlay.NumComplexityLevels(hwPostProc);

	// check return values against test data
	if (levs1 != KTestNumComplexityLevels1 || levs2 != KTestNumComplexityLevels2)
		{
		ERR_PRINTF3(_L("Error - comparison against test data failed! (%d, %d)"), levs1, levs2);
		}
	else
		{
		INFO_PRINTF3(_L("NumComplexityLevels(), %d, %d"), levs1, levs2);
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayGetComplexityLevelInfo::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetComplexityLevelInfo::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayGetComplexityLevelInfo::CTestDevVideoPlayGetComplexityLevelInfo(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetComplexityLevelInfo* CTestDevVideoPlayGetComplexityLevelInfo::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetComplexityLevelInfo* self = new(ELeave) CTestDevVideoPlayGetComplexityLevelInfo(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetComplexityLevelInfo::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetComplexityLevelInfo"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

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

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
		return EInconclusive;
		}

	CMMFDevVideoPlay::TComplexityLevelInfo levInfo1;
	CMMFDevVideoPlay::TComplexityLevelInfo levInfo2;

	// get complexity level info on decoder
	aDevVideoPlay.GetComplexityLevelInfo(hwDecoder, KTestComplexityLevel1, levInfo1);
	
	// get complexity level info on post-processor
	aDevVideoPlay.GetComplexityLevelInfo(hwPostProc, KTestComplexityLevel2, levInfo2);

	CMMFDevVideoPlay::TComplexityLevelInfo testInfo1 = GetTestLevelInfo(KTestComplexityLevel1);
	CMMFDevVideoPlay::TComplexityLevelInfo testInfo2 = GetTestLevelInfo(KTestComplexityLevel2);

	// check return values against test data
	if (!CompareLevelInfos(levInfo1, testInfo1) || !CompareLevelInfos(levInfo2, testInfo2))
		{
		ERR_PRINTF1(_L("Error - comparison against test data failed!"));
		}
	else
		{
		INFO_PRINTF1(_L("Comparison against test data passed ok."));
		INFO_PRINTF1(_L("GetComplexityLevelInfo() called successfully"));
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayGetBuffer::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetBuffer::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoPlayGetBuffer::MdvpoNewBuffers()
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayGetBuffer::MdvpoNewBuffers()"));
	iCalledBack = ETrue;
	}

CTestDevVideoPlayGetBuffer::CTestDevVideoPlayGetBuffer(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetBuffer* CTestDevVideoPlayGetBuffer::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetBuffer* self = new(ELeave) CTestDevVideoPlayGetBuffer(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetBuffer::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetBuffer"));
	
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
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice)); // EABI warning removal
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

	TVideoInputBuffer* newBuff = NULL;

	// get a buffer of a certain size from DevVideoPlay
	TRAP(err, newBuff = aDevVideoPlay.GetBufferL(KTestBufferSize) );
	// callback will occur and newBuff will be set to new buffer

	if (err != expErr)
		{
		ERR_PRINTF3(_L("GetBufferL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		{
		TBool buffSuccess = EFalse;

		// check callback has been called and buffer is correct
		if (newBuff && iCalledBack)
			{
			TTimeIntervalMicroSeconds testTime(KTestBufferSize);
			if ( (newBuff->iDecodingTimestamp == testTime) &&
				 (newBuff->iOptions == KTestBufferOptions) &&
				 (newBuff->iData == KTestBufferString) )
				{
				buffSuccess = ETrue;
				}
			}
		
		if (!buffSuccess)
			{
			ERR_PRINTF1(_L("Received buffer is corrupt!"));
			ret = EFail;
			}
		else
			{
			INFO_PRINTF1(_L("Buffer received correctly"));
			INFO_PRINTF3(_L("GetBuffer(), %d = %d"), err, expErr);	
			}
		}
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayWriteCodedData::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayWriteCodedData::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayWriteCodedData::CTestDevVideoPlayWriteCodedData(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayWriteCodedData* CTestDevVideoPlayWriteCodedData::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayWriteCodedData* self = new(ELeave) CTestDevVideoPlayWriteCodedData(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayWriteCodedData::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  WriteCodedData"));
	
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

	// construct a video input buffer structure with a valid timestamp and
	// write this DevVideoPlay.  Call will leave on error.
	TVideoInputBuffer* newBuff = NULL;
	TPtrC8 testBufferStringPtr(KTestBufferString);
	TRAP(err, newBuff = aDevVideoPlay.GetBufferL(testBufferStringPtr.Length()));
	if (err)
		{
		ERR_PRINTF1(_L("Error - couldn't get buffer from DevVideoPlay"));
		return EInconclusive;
		}
	
	// initialize newBuff with test data
	newBuff->iOptions = KTestBufferOptions;
	newBuff->iDecodingTimestamp = KTestBufferSize;
	newBuff->iData.Zero();
	newBuff->iData.Append(testBufferStringPtr);
	
	// write buffer to DevVideoPlay
	TRAP(err, aDevVideoPlay.WriteCodedDataL(newBuff) )

	if (err != expErr)
		{
		ERR_PRINTF3(_L("WriteCodedDataL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Coded data sent successfully"));
		INFO_PRINTF3(_L("WriteCodedDataL(), %d = %d"), err, expErr);	
		}
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayScanAndCopyCodedData::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayWriteCodedData::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayScanAndCopyCodedData::CTestDevVideoPlayScanAndCopyCodedData(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}	

CTestDevVideoPlayScanAndCopyCodedData* CTestDevVideoPlayScanAndCopyCodedData::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayScanAndCopyCodedData* self = new(ELeave) CTestDevVideoPlayScanAndCopyCodedData(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayScanAndCopyCodedData::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("MMMFVideoPlayHwDeviceExtensionScanCopy:  ScanAndCopyCodedDataL"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	THwDeviceId hwDecoder = 0;
    
    // select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	// select post-processor
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
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


	// construct a video input buffer structure with a valid timestamp and
	// write this DevVideoPlay.  Call will leave on error.
	TVideoInputBuffer* newBuff = NULL;
	TPtrC8 testBufferStringPtr(KTestBufferString);
	TRAP(err, newBuff = aDevVideoPlay.GetBufferL(testBufferStringPtr.Length()));
	if (err)
		{
		ERR_PRINTF1(_L("Error - couldn't get buffer from DevVideoPlay"));
		return EInconclusive;
		}

	//clean out pre-set stuff from buffer
	newBuff->iData.Zero();
	//set ptr to test data
	TPtrC8 testBufferString(KTestBufferString);
	HBufC8* bufferDataArea = testBufferString.AllocL();
	CleanupStack::PushL(bufferDataArea);
	TPtr8 dataAreaPtr = bufferDataArea->Des();
	TFramePortion framePortion = EFramePortionEndFragment;
	
	MMMFVideoPlayHwDeviceExtensionScanCopy* ciOne = static_cast<MMMFVideoPlayHwDeviceExtensionScanCopy*>(aDevVideoPlay.CustomInterface(hwDecoder, KUidDevVideoPlayHwDeviceExtensionScanCopy));

	TInt consumed = 0;
	// write buffer to DevVideoPlay using new interface
	TRAP(err, ciOne->ScanAndCopyCodedDataL(dataAreaPtr, newBuff, consumed, framePortion));

	//compare buffer to test data
	if ((newBuff->iData != KTestBufferString) || consumed != newBuff->iData.Length())
		{
		err = KErrCorrupt;
		}

	if (err != expErr)
		{
		ERR_PRINTF3(_L("ScanAndCopyCodedDataL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Coded data sent successfully"));
		INFO_PRINTF3(_L("ScanAndCopyCodedDataL(), %d = %d"), err, expErr);	
		}
	CleanupStack::PopAndDestroy(bufferDataArea);	
	return ret;
	}

//------------------------------------------------------------------

void CTestDevVideoPlayExtensionWriteCodedData::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayWriteCodedData::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayExtensionWriteCodedData::CTestDevVideoPlayExtensionWriteCodedData(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayExtensionWriteCodedData* CTestDevVideoPlayExtensionWriteCodedData::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayExtensionWriteCodedData* self = new(ELeave) CTestDevVideoPlayExtensionWriteCodedData(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayExtensionWriteCodedData::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("MMMFVideoPlayHwDeviceExtensionScanCopy:  WriteCodedData"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	THwDeviceId hwDecoder = 0;

	// select decoder
	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));

	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	// select post-processor
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

	// construct a video input buffer structure with a valid timestamp and
	// write this DevVideoPlay.  Call will leave on error.
	TVideoInputBuffer* newBuff = NULL;
	TPtrC8 testBufferStringPtr(KTestBufferString);
	TRAP(err, newBuff = aDevVideoPlay.GetBufferL(testBufferStringPtr.Length()));
	if (err)
		{
		ERR_PRINTF1(_L("Error - couldn't get buffer from DevVideoPlay"));
		return EInconclusive;
		}
	
	// initialize newBuff with test data
	newBuff->iOptions = KTestBufferOptions;
	newBuff->iDecodingTimestamp = KTestBufferSize;
	newBuff->iData.Zero();
	newBuff->iData.Append(testBufferStringPtr);

	TFramePortion framePortion = EFramePortionEndFragment;
	
	MMMFVideoPlayHwDeviceExtensionScanCopy* ciOne = static_cast<MMMFVideoPlayHwDeviceExtensionScanCopy*>(aDevVideoPlay.CustomInterface(hwDecoder, KUidDevVideoPlayHwDeviceExtensionScanCopy));

	// write buffer to DevVideoPlay using new interface
	TRAP(err, ciOne->WriteCodedDataL(newBuff, framePortion));
	
	if (err != expErr)
		{
		ERR_PRINTF3(_L("WriteCodedDataL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Coded data sent successfully"));
		INFO_PRINTF3(_L("WriteCodedDataL(), %d = %d"), err, expErr);	
		}
	return ret;
	}

//------------------------------------------------------------------

void CTestDevVideoPlayWritePicture::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayWritePicture::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoPlayWritePicture::MdvpoReturnPicture(TVideoPicture* aPicture)
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayWritePicture::MdvpoReturnPicture()"));

	iPicture = aPicture;
	}

CTestDevVideoPlayWritePicture::CTestDevVideoPlayWritePicture(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayWritePicture* CTestDevVideoPlayWritePicture::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayWritePicture* self = new(ELeave) CTestDevVideoPlayWritePicture(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayWritePicture::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  WritePicture"));
	
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

	// construct a video picture structure with a valid timestamp and
	// write this DevVideoPlay.  If the same picture comes back through the
	// MdvpoReturnPicture() callback then test has passed.
	TVideoPicture picture;
	TTimeIntervalMicroSeconds picTime(KTestPictureTimestamp);
	picture.iTimestamp = picTime;
	picture.iOptions = TVideoPicture::ETimestamp;
	
	// write picture to DevVideoPlay
	TRAP(err, aDevVideoPlay.WritePictureL(&picture) )

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

void CTestDevVideoPlayInputEnd::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayInputEnd::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoPlayInputEnd::MdvpoStreamEnd()
	{
	INFO_PRINTF1(_L("CTestDevVideoPlayInputEnd::MdvpoStreamEnd()"));
	
	// keep track on number of times this callback is called
	iEndCount++;
	}

CTestDevVideoPlayInputEnd::CTestDevVideoPlayInputEnd(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayInputEnd* CTestDevVideoPlayInputEnd::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayInputEnd* self = new(ELeave) CTestDevVideoPlayInputEnd(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayInputEnd::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPost = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  InputEnd"));
	
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

	// call input end on DevVideoPlay and check callback counter == 1
	aDevVideoPlay.InputEnd();

	if (iEndCount != 1)
		{
		ERR_PRINTF2(_L("Error - MdvpoStreamEnd only called %d times!"), iEndCount);
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("InputEnd() called successfully"));
	
	return ret;
	}
//-------------------------------------------------------------------

void CTestDevVideoGetBufferWithoutInitialize::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestGetBufferWithoutInitialize::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoGetBufferWithoutInitialize::MdvpoNewBuffers()
	{
	INFO_PRINTF1(_L("CTestGetBufferWithoutInitialize::MdvpoNewBuffers()"));
	iCalledBack = ETrue;
	}

CTestDevVideoGetBufferWithoutInitialize::CTestDevVideoGetBufferWithoutInitialize(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoGetBufferWithoutInitialize* CTestDevVideoGetBufferWithoutInitialize::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoGetBufferWithoutInitialize* self = new(ELeave) CTestDevVideoGetBufferWithoutInitialize(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoGetBufferWithoutInitialize::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetBuffer"));
	INFO_PRINTF1(_L("To test the scenario wherein GetBufferL is called without calling initialize"));
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}

	TInt err = KErrNone;

	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice)); // EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
		return EInconclusive;
		}

	// select post-processor
	TRAP(err, aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));	// EABI warning removal
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
		return EInconclusive;
		}
	TVideoInputBuffer* newBuff = NULL;
	// iError is set by the MdvpoInitComplete callback
	TRAP(err, newBuff = aDevVideoPlay.GetBufferL(KTestBufferSize) );
	// callback will occur and newBuff will be set to new buffer
	if (err != expErr)
		{
		ERR_PRINTF3(_L("GetBufferL() gave error %d (expected %d)"),err, expErr);
		ret = EFail;
		}
	else
		{
		TBool buffSuccess = EFalse;

		// check callback has been called and buffer is correct
		if (newBuff && iCalledBack)
			{
			TTimeIntervalMicroSeconds testTime(KTestBufferSize);
			if ( (newBuff->iDecodingTimestamp == testTime) &&
				 (newBuff->iOptions == KTestBufferOptions) &&
				 (newBuff->iData == KTestBufferString) )
				{
				buffSuccess = ETrue;
				}
			}
		
		if (!buffSuccess)
			{
			ERR_PRINTF1(_L("Received buffer is corrupt!"));
			ret = EFail;
			}
		else
			{
			INFO_PRINTF1(_L("Buffer received correctly"));
			INFO_PRINTF3(_L("GetBuffer(), %d = %d"), err, expErr);	
			}
		}
	
	return ret;
	}
//-----------------------------------------------------------------------

