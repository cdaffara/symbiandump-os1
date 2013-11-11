
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

void CTestDevVideoPlayGetNewPictureInfo::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetNewPictureInfo::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayGetNewPictureInfo::CTestDevVideoPlayGetNewPictureInfo(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetNewPictureInfo* CTestDevVideoPlayGetNewPictureInfo::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetNewPictureInfo* self = new(ELeave) CTestDevVideoPlayGetNewPictureInfo(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetNewPictureInfo::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetNewPictureInfo"));
	
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

	//Tell plugin to start creating pictures.
	TRAP(iError, aDevVideoPlay.SetVideoDestScreenL(EFalse));
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't SetVideoDestScreen"));
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

	aDevVideoPlay.Start();

	TUint numPictures = 0;
	TTimeIntervalMicroSeconds earliestTimeStamp(0);
	TTimeIntervalMicroSeconds latestTimeStamp(0);


	aDevVideoPlay.GetNewPictureInfo(numPictures, earliestTimeStamp, latestTimeStamp);

	if ((numPictures == 1)
		&& (earliestTimeStamp == TTimeIntervalMicroSeconds(0))
		&& (latestTimeStamp == TTimeIntervalMicroSeconds(0)))
		{
		ret = EPass;
		}
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayNextPicture::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayNextPicture::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayNextPicture::CTestDevVideoPlayNextPicture(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayNextPicture* CTestDevVideoPlayNextPicture::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayNextPicture* self = new(ELeave) CTestDevVideoPlayNextPicture(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayNextPicture::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  NextPicture"));
	
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

	//Tell plugin to start creating pictures.
	TRAP(iError, aDevVideoPlay.SetVideoDestScreenL(EFalse));
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't SetVideoDestScreen"));
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

	iDevVideoPlay = &aDevVideoPlay;
	aDevVideoPlay.Start();//Tell plugin to start creating pictures.
	// We should now get a series of NewPicture() callbacks.

	if (iError == KErrNone)
		{
		ret = EPass;
		}
	else
		{
		ERR_PRINTF2(_L("Error - picture processing failed with error %d"), iError);
		}
	
	return ret;
	}

void CTestDevVideoPlayNextPicture::MdvpoNewPictures()
	{
	TRAP(iError, DoNewPicturesL());
	}

void CTestDevVideoPlayNextPicture::CheckNewPictureInfoL(TUint aNumPictures, const TTimeIntervalMicroSeconds& aFirst, const TTimeIntervalMicroSeconds& aLast)
	{
	TUint numPictures = 0;
	TTimeIntervalMicroSeconds first(0);
	TTimeIntervalMicroSeconds last(0);
	iDevVideoPlay->GetNewPictureInfo(numPictures, first, last);

	if ((numPictures!=aNumPictures) || (first!=aFirst) || (last!=aLast))
		{
		User::Leave(KErrGeneral);
		}
	}

void CTestDevVideoPlayNextPicture::DoNewPicturesL()
	{
	User::LeaveIfError(iError);//Don't go any further if we have an error
	switch (iCurrentPictureNumber)
		{
		case 0:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			CheckNewPictureInfoL(0, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 1:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(1), TTimeIntervalMicroSeconds(1));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			CheckNewPictureInfoL(0, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 2:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(2), TTimeIntervalMicroSeconds(2));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			CheckNewPictureInfoL(0, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 3:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(3), TTimeIntervalMicroSeconds(3));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 4:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(4), TTimeIntervalMicroSeconds(4));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 5:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(5), TTimeIntervalMicroSeconds(5));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			break;
			}
		case 6:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(6), TTimeIntervalMicroSeconds(6));
			break;
			}
		case 7:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(2, TTimeIntervalMicroSeconds(6), TTimeIntervalMicroSeconds(7));
			break;
			}
		case 8:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(3, TTimeIntervalMicroSeconds(6), TTimeIntervalMicroSeconds(8));
			break;
			}
		case 9:
			{
			iCurrentPictureNumber++;
			CheckNewPictureInfoL(4, TTimeIntervalMicroSeconds(6), TTimeIntervalMicroSeconds(9));
			TVideoPicture* picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			CheckNewPictureInfoL(3, TTimeIntervalMicroSeconds(7), TTimeIntervalMicroSeconds(9));
			picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			CheckNewPictureInfoL(2, TTimeIntervalMicroSeconds(8), TTimeIntervalMicroSeconds(9));
			picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			CheckNewPictureInfoL(1, TTimeIntervalMicroSeconds(9), TTimeIntervalMicroSeconds(9));
			picture = iDevVideoPlay->NextPictureL();
			iDevVideoPlay->ReturnPicture(picture);
			CheckNewPictureInfoL(0, TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(0));
			break;
			}
		default:
			User::Leave(KErrGeneral);
			break;
		};
	}



//------------------------------------------------------------------

void CTestDevVideoPlayGetSnapshot::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetSnapshot::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayGetSnapshot::CTestDevVideoPlayGetSnapshot(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetSnapshot* CTestDevVideoPlayGetSnapshot::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetSnapshot* self = new(ELeave) CTestDevVideoPlayGetSnapshot(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetSnapshot::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EFail;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetSnapshot"));
	
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

	TBool result = EFalse;
	TPictureData pictureData;

	TUncompressedVideoFormat format;
	format.iDataFormat = EYuvRawData;
	format.iYuvFormat = KTestYuvFormat1;

	TRAP(err, result = aDevVideoPlay.GetSnapshotL(pictureData, format));

	if (err == KErrNone)
		{
		if (result)
			{
			if ((pictureData.iDataFormat == EYuvRawData) &&
				(pictureData.iDataSize == TSize(KTestPictureDataSizeX, KTestPictureDataSizeY)) && 
				(pictureData.iRawData == NULL))
				{
				ret = EPass;
				}
			}
		}
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayGetTimedSnapshotTimestamp::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetTimedSnapshotTimestamp::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoPlayGetTimedSnapshotTimestamp::MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& aPresentationTimestamp, 
											const TPictureId& /*aPictureId*/)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetTimedSnapshotTimestamp::MdvpoTimedSnapshotComplete():  Error = %d"), aError);
	
	iError = aError;
	iTimestamp = aPresentationTimestamp;
	iPictureData = aPictureData;
	}

CTestDevVideoPlayGetTimedSnapshotTimestamp::CTestDevVideoPlayGetTimedSnapshotTimestamp(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetTimedSnapshotTimestamp* CTestDevVideoPlayGetTimedSnapshotTimestamp::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetTimedSnapshotTimestamp* self = new(ELeave) CTestDevVideoPlayGetTimedSnapshotTimestamp(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetTimedSnapshotTimestamp::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EFail;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetTimedSnapshotTimestamp"));
	
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

	TPictureData pictureData;

	TUncompressedVideoFormat format;
	format.iDataFormat = EYuvRawData;
	format.iYuvFormat = KTestYuvFormat1;
	TTimeIntervalMicroSeconds snapTime(KTestSnapshotTimestamp);

	// get a timed snapshot from the post processor
	TRAP(err, aDevVideoPlay.GetTimedSnapshotL(&pictureData, format, snapTime));

	// check picture and timestamp from callback
	if ((err == KErrNone) && (iError == KErrNone))
		{
		if ((iPictureData) && (iTimestamp == TTimeIntervalMicroSeconds(KTestSnapshotTimestamp)))
			{
			if ((iPictureData->iDataFormat == EYuvRawData) &&
				(iPictureData->iDataSize == TSize(KTestPictureDataSizeX, KTestPictureDataSizeY)) && 
				(iPictureData->iRawData == NULL))
				{
				ret = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("Error - PictureData is corrupt!"));
				}
			}
		}
	else
		{
		ERR_PRINTF3(_L("GetTimedSnapshotL() gave error %d (expected %d)"),err, expErr);
		}
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayGetTimedSnapshotID::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetTimedSnapshotID::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoPlayGetTimedSnapshotID::MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/, 
											const TPictureId& aPictureId)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetTimedSnapshotID::MdvpoTimedSnapshotComplete():  Error = %d"), aError);
	
	iError = aError;
	iPictureId = aPictureId;
	iPictureData = aPictureData;
	}

CTestDevVideoPlayGetTimedSnapshotID::CTestDevVideoPlayGetTimedSnapshotID(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetTimedSnapshotID* CTestDevVideoPlayGetTimedSnapshotID::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetTimedSnapshotID* self = new(ELeave) CTestDevVideoPlayGetTimedSnapshotID(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetTimedSnapshotID::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetTimedSnapshotID"));
	
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

	TPictureData pictureData;

	TUncompressedVideoFormat format;
	format.iDataFormat = EYuvRawData;
	format.iYuvFormat = KTestYuvFormat1;
	TPictureId snapId = KTestSnapshotId;

	// get a timed snapshot from the post processor
	TRAP(err, aDevVideoPlay.GetTimedSnapshotL(&pictureData, format, snapId));

	// check picture and timestamp from callback
	if ((err == KErrNone) && (iError == KErrNone))
		{
		if ( (iPictureData) && (iPictureId.iIdType == KTestSnapshotId.iIdType) && 
			 (iPictureId.iId == KTestSnapshotId.iId) )
			{
			if ((iPictureData->iDataFormat == EYuvRawData) &&
				(iPictureData->iDataSize == TSize(KTestPictureDataSizeX, KTestPictureDataSizeY)) && 
				(iPictureData->iRawData == NULL))
				{
				ret = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("Error - PictureData is corrupt!"));
				}
			}
		}
	else
		{
		ERR_PRINTF3(_L("GetTimedSnapshotL() gave error %d (expected %d)"),err, expErr);
		}
	
	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayCancelTimedSnapshot::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayCancelTimedSnapshot::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

void CTestDevVideoPlayCancelTimedSnapshot::MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* /*aPictureData*/, 
											const TTimeIntervalMicroSeconds& /*aPresentationTimestamp*/, 
											const TPictureId& /*aPictureId*/)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayCancelTimedSnapshot::MdvpoTimedSnapshotComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayCancelTimedSnapshot::CTestDevVideoPlayCancelTimedSnapshot(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayCancelTimedSnapshot* CTestDevVideoPlayCancelTimedSnapshot::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayCancelTimedSnapshot* self = new(ELeave) CTestDevVideoPlayCancelTimedSnapshot(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayCancelTimedSnapshot::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EFail;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  CancelTimedSnapshot"));
	
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
	TRAP(err, aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}

	// select post-processor
//	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
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

	
	TPictureData pictureData;

	TUncompressedVideoFormat format;
	format.iDataFormat = EYuvRawData;
	format.iYuvFormat = KTestYuvFormat1;
	TPictureId snapId = KTestSnapshotId;

	// get a timed snapshot from the post processor
	TRAP(err, aDevVideoPlay.GetTimedSnapshotL(&pictureData, format, snapId));

	// cancel the snapshot
	aDevVideoPlay.CancelTimedSnapshot();

	// check picture has been cancelled
	if (err == KErrNone) 
		{
		if (iError == KErrAbort)
			{
			INFO_PRINTF1(_L("Timed snapshot cancelled successfully"));
			ret = EPass;
			}
		else
			{
			ERR_PRINTF3(_L("MdvpoTimedSnapshotComplete() gave error %d (expected %d)"),iError, KErrAbort);			
			}
		}
	else
		{
		ERR_PRINTF3(_L("CancelTimedSnapshotL() gave error %d (expected %d)"),err, expErr);
		}

	return ret;
	}
//------------------------------------------------------------------

void CTestDevVideoPlayGetSupportedSnapshotFormats::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayGetSupportedSnapshotFormats::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayGetSupportedSnapshotFormats::CTestDevVideoPlayGetSupportedSnapshotFormats(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayGetSupportedSnapshotFormats* CTestDevVideoPlayGetSupportedSnapshotFormats::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayGetSupportedSnapshotFormats* self = new(ELeave) CTestDevVideoPlayGetSupportedSnapshotFormats(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayGetSupportedSnapshotFormats::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EFail;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  GetSupportedSnapshotFormats"));
	
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

	RArray<TUncompressedVideoFormat> vidFormats;

	// get a list of supported formats from the plugin
	TRAP(err, aDevVideoPlay.GetSupportedSnapshotFormatsL(vidFormats) )

	// check picture and timestamp from callback
	if (err == KErrNone)
		{
		if ( vidFormats.Count() == 3)
			{
			if ((vidFormats[0] == KTestVidFormat1) &&
				(vidFormats[1] == KTestVidFormat2) &&
				(vidFormats[2] == KTestVidFormat3) )
				{
				INFO_PRINTF1(_L("Snapshot format list checks ok"));
				ret = EPass;
				}
			else
				{
				ERR_PRINTF1(_L("Error - Snapshot format list is corrupt!"));
				}
			}
		}
	else
		{
		ERR_PRINTF3(_L("GetSupportedSnapshotFormatsL() gave error %d (expected %d)"),err, expErr);
		}

	vidFormats.Reset();
	vidFormats.Close();

	return ret;
	}

//------------------------------------------------------------------

CTestDevVideoPlayClock::CTestDevVideoPlayClock(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	}

void CTestDevVideoPlayClock::ConstructL()
	{
	}

CTestDevVideoPlayClock::~CTestDevVideoPlayClock()
	{
	}

CTestDevVideoPlayClock* CTestDevVideoPlayClock::NewL(const TDesC& aTestName)
	{
	CTestDevVideoPlayClock* self = new(ELeave) CTestDevVideoPlayClock(aTestName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TInt CTestDevVideoPlayClock::StartThread(TAny* aAny)
	{
	CTestDevVideoPlayClock* me = static_cast<CTestDevVideoPlayClock*>(aAny);
	me->RunThread();

	return KErrNone;
	}

void CTestDevVideoPlayClock::RunThread()
	{
	TTimeIntervalMicroSeconds currentTime(0);

	while(!iShutdownSubthread)
		{
		currentTime = iClockSource->Time();
		TUint timeUint = I64LOW(currentTime.Int64());
		}
	}

void CTestDevVideoPlayClock::MmcspuoTick(const TTimeIntervalMicroSeconds& aTime)
	{
	iPeriodicUtilityIteration++;

	TUint currentTime = I64LOW(aTime.Int64());
	TUint predictedTime = iPeriodicUtilityIteration * KTestClock2Seconds;

	if (!TimeComparison(currentTime, predictedTime, KTestClockBigDeviationMS))
		{
		ERR_PRINTF3(_L("Error - Periodic Utility time comparison failed:  Got %u;  Expected %u"), currentTime, predictedTime);
		iPeriodicUtilityTestVerdict = EFail;
		iPeriodicUtility->Stop();
		CActiveScheduler::Stop();
		}
	else
		{
		INFO_PRINTF3(_L("Periodic Utility time comparison passed:  Got %u;  Expected %u"), currentTime, predictedTime);
		}

	if (iPeriodicUtilityIteration >= 5)
		{
		iPeriodicUtility->Stop();
		CActiveScheduler::Stop();
		}
	}

TVerdict CTestDevVideoPlayClock::DoTestStepL()
	{
	TVerdict ret = EPass;
	TTimeIntervalMicroSeconds testTime(0);
	TUint timeUint = 0;

	iClockSource = CSystemClockSource::NewL();

	// Construct a new thread to constantly probe the clock - to test that it'll work from multiple threads
	RThread thread;
	
	TInt error = thread.Create(_L("ClockSourceTestThread"), 
								&CTestDevVideoPlayClock::StartThread,
								KDefaultStackSize, 
								&User::Heap(),
								static_cast<TAny*>(this), 
								EOwnerThread);

	if (error)
		{
		ERR_PRINTF1(_L("Error - Couldn't create periodic utility object"));
		delete iClockSource;
		return EInconclusive;
		}

	TRequestStatus stat;
	thread.Logon(stat);
	if (stat!=KRequestPending)
		{
		delete iClockSource;
		ERR_PRINTF2(_L("Error - Couldn't logon to the thread err=%d"), stat.Int());
		thread.LogonCancel(stat);
		User::WaitForRequest(stat);
		return EInconclusive;
		}
	thread.SetPriority(EPriorityLess);

	// wait for system to calm down
	User::After(KTestClock2Seconds);

	// reset the clock
	iClockSource->Reset();

	thread.Resume();

	// wait for 2 seconds and then check the time == 2 seconds
	User::After(KTestClock2Seconds);
	testTime = iClockSource->Time();
	timeUint = I64LOW(testTime.Int64());

	if (!TimeComparison(timeUint, KTestClock2Seconds, KTestClockDeviationMS))
		{
		ERR_PRINTF3(_L("Error - time comparison failed:  Got %u;  Expected %u"), timeUint, KTestClock2Seconds);
		ret = EFail;
		}
	else 
		{
		INFO_PRINTF3(_L("Time comparison passed:  Got %u;  Expected %u"), timeUint, KTestClock2Seconds);
		}

	// reset the clock back to 2 seconds
	//testTime = KTestClock2Seconds;
	//iClockSource->Reset(testTime);

	// suspend timer for 2 seconds then resume for 2 seconds
	iClockSource->Suspend();
	User::After(KTestClock2Seconds);
	iClockSource->Resume();
	User::After(KTestClock2Seconds);
	testTime = iClockSource->Time();
	timeUint = I64LOW(testTime.Int64());

	if (!TimeComparison(timeUint, KTestClock4Seconds, KTestClockDeviationMS))
		{
		ERR_PRINTF3(_L("Error - Suspend() + Resume() time comparison failed:  Got %u;  Expected %u"), timeUint, KTestClock4Seconds);
		ret = EFail;
		}
	else
		{
		INFO_PRINTF3(_L("Suspend() + Resume() time comparison passed:  Got %u;  Expected %u"), timeUint, KTestClock4Seconds);
		}

	// reset clock to 18 seconds
	testTime = KTestClock18Seconds;
	iClockSource->Reset(testTime);

	// wait for 2 seconds and then check the time == 20 seconds
	User::After(KTestClock2Seconds);
	testTime = iClockSource->Time();
	timeUint = I64LOW(testTime.Int64());

	if (!TimeComparison(timeUint, KTestClock20Seconds, KTestClockDeviationMS))
		{
		ERR_PRINTF3(_L("Error - Reset() time comparison failed:  Got %u;  Expected %u"), timeUint, KTestClock20Seconds);
		ret = EFail;
		}
	else
		{
		INFO_PRINTF3(_L("Reset() time comparison passed:  Got %u;  Expected %u"), timeUint, KTestClock20Seconds);
		}

	// suspend for 2 seconds then resume for 2 seconds
	iClockSource->Suspend();
	User::After(KTestClock2Seconds);
	iClockSource->Resume();
	User::After(KTestClock2Seconds);
	
	testTime = iClockSource->Time();
	timeUint = I64LOW(testTime.Int64());

	if (!TimeComparison(timeUint, KTestClock22Seconds, KTestClockDeviationMS))
		{
		ERR_PRINTF3(_L("Error - Suspend() + Resume() time comparison failed:  Got %u;  Expected %u"), timeUint, KTestClock22Seconds);
		ret = EFail;
		}
	else
		{
		INFO_PRINTF3(_L("Suspend() + Resume() time comparison passed:  Got %u;  Expected %u"), timeUint, KTestClock22Seconds);
		}

	// Now test the periodic utility
	TRAPD(perError, iPeriodicUtility = CMMFClockSourcePeriodicUtility::NewL(*iClockSource, *this));
	if (perError)
		{
		ERR_PRINTF1(_L("Error - Couldn't create periodic utility object"));
		ret = EInconclusive;
		}
	else
		{
		iPeriodicUtilityTestVerdict = EPass;
		TTimeIntervalMicroSeconds32 period = KTestClock2Seconds;
		iClockSource->Reset();
		iPeriodicUtility->Start(period);
		CActiveScheduler::Start();
		ret = iPeriodicUtilityTestVerdict;
		}
	delete iPeriodicUtility;
	iPeriodicUtility = NULL;

	// Clean up the thread
	iShutdownSubthread = ETrue;
	User::WaitForRequest(stat);
	thread.Close();

	delete iClockSource;

	return ret;
	}

//------------------------------------------------------------------

void CTestDevVideoPlayCommit::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayCommit::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayCommit::CTestDevVideoPlayCommit(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayCommit* CTestDevVideoPlayCommit::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayCommit* self = new(ELeave) CTestDevVideoPlayCommit(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayCommit::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPost = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  Commit"));
	
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

	// call CommitL on DevVideoPlay 
	TRAP(iError, aDevVideoPlay.CommitL());

	if (iError != expErr)
		{
		ERR_PRINTF3(_L("Commit left with error %d; Expected %d!"), iError, expErr);
		ret = EFail;
		}
	else
		INFO_PRINTF1(_L("CommitL() called successfully"));
	
	return ret;
	}

//------------------------------------------------------------------

void CTestDevVideoPlayRevert::MdvpoInitComplete(TInt aError)
	{
	INFO_PRINTF2(_L("CTestDevVideoPlayRevert::MdvpoInitComplete():  Error = %d"), aError);
	
	iError = aError;
	}

CTestDevVideoPlayRevert::CTestDevVideoPlayRevert(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayRevert* CTestDevVideoPlayRevert::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayRevert* self = new(ELeave) CTestDevVideoPlayRevert(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayRevert::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
//	TInt expErr = KErrNone;
	TBool selDec = ETrue;
	TBool selPost = ETrue;

	INFO_PRINTF1(_L("CMMFDevVideoPlay:  Revert"));
	
	// expected results
	switch(iTestType)
		{
		case ETestValid:
//			expErr = KErrNone;
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

	// call Revert on DevVideoPlay
	aDevVideoPlay.Revert();

	// if no PANIC then test has passed...

	INFO_PRINTF1(_L("Revert() called successfully"));
	
	return ret;
	}


//------------------------------------------------------------------

CTestDevVideoPlayCustomInterface::CTestDevVideoPlayCustomInterface(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayCustomInterface* CTestDevVideoPlayCustomInterface::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayCustomInterface* self = new(ELeave) CTestDevVideoPlayCustomInterface(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayCustomInterface::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  CustomInterface"));

	THwDeviceId hwDecoder = 0;
	THwDeviceId hwPostProc = 0;
	TInt err = KErrNone;

	TRAP(err, hwDecoder = aDevVideoPlay.SelectDecoderL(KUidDevVideoTestDecodeHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize decoder"));
			return EInconclusive;
		}
	
	TRAP(err, hwPostProc = aDevVideoPlay.SelectPostProcessorL(KUidDevVideoTestPostProcHwDevice));
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize post processor"));
			return EInconclusive;
		}

	// First try test uninitialized

	TAny* ciOne = NULL;
	TAny* ciTwo = NULL;
	TAny* ciThree = NULL;
	TAny* ciFour = NULL;
	
	ciOne = aDevVideoPlay.CustomInterface(hwDecoder, KUidCustomInterfaceOne);
	ciTwo = aDevVideoPlay.CustomInterface(hwDecoder, KUidCustomInterfaceTwo);
	ciThree = aDevVideoPlay.CustomInterface(hwPostProc, KUidCustomInterfaceTwo);
	ciFour = aDevVideoPlay.CustomInterface(hwPostProc, KUidCustomInterfaceOne);

	if (ciOne==NULL || ciTwo!=NULL || ciThree==NULL || ciFour!=NULL)
		{
		ERR_PRINTF1(_L("Test Failed - wrong interfaces returned"));
		return EFail;
		}

	// Next, re-perform test initialized.

	// initialize CDevVideoPlay and wait for response
	// iError is set by the MdvpoInitComplete callback
	aDevVideoPlay.Initialize();
	
	if (iError != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't initialize DevVideoPlay"));
		return EInconclusive;
		}

	ciOne = NULL;
	ciTwo = NULL;
	ciThree = NULL;
	ciFour = NULL;
	
	ciOne = aDevVideoPlay.CustomInterface(hwDecoder, KUidCustomInterfaceOne);
	ciTwo = aDevVideoPlay.CustomInterface(hwDecoder, KUidCustomInterfaceTwo);
	ciThree = aDevVideoPlay.CustomInterface(hwPostProc, KUidCustomInterfaceTwo);
	ciFour = aDevVideoPlay.CustomInterface(hwPostProc, KUidCustomInterfaceOne);

	if (ciOne==NULL || ciTwo!=NULL || ciThree==NULL || ciFour!=NULL)
		{
		ERR_PRINTF1(_L("Test Failed - wrong interfaces returned"));
		return EFail;
		}


	return EPass;
	}



/*
//------------------------------------------------------------------

CTestDevVideoPlayXXX::CTestDevVideoPlayXXX(const TDesC& aTestName, TTestType aTestType)
	:CTestDevVideoPlayStep(aTestName, aTestType)
	{
	}

CTestDevVideoPlayXXX* CTestDevVideoPlayXXX::NewL(const TDesC& aTestName, TTestType aTestType)
	{
	CTestDevVideoPlayXXX* self = new(ELeave) CTestDevVideoPlayXXX(aTestName, aTestType);
	return self;
	}

TVerdict CTestDevVideoPlayXXX::DoTestL(CMMFDevVideoPlay& aDevVideoPlay)
	{
	TVerdict ret = EPass;
	TInt expErr = KErrNone;
	
	INFO_PRINTF1(_L("CMMFDevVideoPlay:  XXX"));
	
	// expected results
	if (iTestType != ETestValid)
		{
		ERR_PRINTF1(_L("Error - invalid test step type"));
		return EInconclusive;
		}
	
	// XXXX

	TInt err = KErrNone;
	// TRAP(err, aDevVideoPlay.API_FUNCTION_HERE() )

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

