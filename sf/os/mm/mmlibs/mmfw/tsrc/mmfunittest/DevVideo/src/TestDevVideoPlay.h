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
// Header file: DevVideoPlay tests.
// 
//

/**
 @file TestDevVideoPlay.h
*/

#ifndef __TESTDEVVIDEOPLAY_H__
#define __TESTDEVVIDEOPLAY_H__

#include "TSU_MMF_DevVideo.h"

/**
 * DevVideoPlay test base class
 *
 * @class CTestDevVideoPlayStep
 *
 * Req. under test REQ1931.3
 */

class CTestDevVideoPlayStep : public CTestDevVideoStep, public MMMFDevVideoPlayObserver
	{
public:
	CTestDevVideoPlayStep(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayStep();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay) = 0;

	// MMMFDevVideoPlayObserver
    virtual void MdvpoNewBuffers();
    virtual void MdvpoReturnPicture(TVideoPicture* aPicture);
    virtual void MdvpoSupplementalInformation(const TDesC8& aData, 
											  const TTimeIntervalMicroSeconds& aTimestamp, 
											  const TPictureId& aPictureId);
	virtual void MdvpoPictureLoss();
	virtual void MdvpoPictureLoss(const TArray<TPictureId>& aPictures);
	virtual void MdvpoSliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture);
	virtual void MdvpoReferencePictureSelection(const TDesC8& aSelectionData);
    virtual void MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& aPresentationTimestamp, 
											const TPictureId& aPictureId);
    virtual void MdvpoNewPictures();
    virtual void MdvpoFatalError(TInt aError);
    virtual void MdvpoInitComplete(TInt aError);
    virtual void MdvpoStreamEnd();

protected:
	TInt iError;
	TTestType iTestType;					// Type of test
	CMMFDevVideoPlay* iDevVideoPlay;
	};


//

class CTestDevVideoPlayCommonFormat : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayCommonFormat* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayCommonFormat(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayCommonFormat() {};
	};

//

class CTestDevVideoPlayFindDecoders : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayFindDecoders* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayFindDecoders(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayFindDecoders() {};
	TInt CheckDecoders(const RArray<TUid>& aFoundDecoders, const RArray<TUid>& aExpectedDecoders, const RArray<TUid>& aUnExpectedDecoders);
	};


//

class CTestDevVideoPlayFindPostProc : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayFindPostProc* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayFindPostProc(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayFindPostProc() {};
	TInt CheckPostProx(const RArray<TUid>& aFoundPostProx, const RArray<TUid>& aExpectedPostProx, const RArray<TUid>& aUnExpectedPostProx);
	};


//

class CTestDevVideoPlayListDecoders : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayListDecoders* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayListDecoders(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayListDecoders() {};
	};


//

class CTestDevVideoPlayListPostProc : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayListPostProc* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayListPostProc(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayListPostProc() {};
	};


//

class CTestDevVideoPlayGetDecoderInfo : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetDecoderInfo* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayGetDecoderInfo(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetDecoderInfo() {};
	TBool ValidDecoderInfo(CVideoDecoderInfo* aInfo);
	};


//

class CTestDevVideoPlayGetPostProcInfo : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetPostProcInfo* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayGetPostProcInfo(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetPostProcInfo() {};
	TBool ValidPostProcInfo(CPostProcessorInfo* aInfo);
	};


//

class CTestDevVideoPlaySelectDecoder : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySelectDecoder* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySelectDecoder(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySelectDecoder() {};
	};

//

class CTestDevVideoPlaySelectPostProc : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySelectPostProc* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySelectPostProc(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySelectPostProc() {};
	};

//

class CTestDevVideoPlayGetHeaderInfo : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetHeaderInfo* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayGetHeaderInfo(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetHeaderInfo() {};
	TBool ValidPictureHeader(const TVideoPictureHeader* aPictureHeader);
	};

//

class CTestDevVideoPlayReturnHeader : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayReturnHeader* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayReturnHeader(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayReturnHeader() {};
	};

//

class CTestDevVideoPlayConfigureDecoder: public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayConfigureDecoder* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayConfigureDecoder(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayConfigureDecoder() {};
	};

//

class CTestDevVideoPlaySetInputFormatUC : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetInputFormatUC* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetInputFormatUC(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetInputFormatUC() {};
	};

//

class CTestDevVideoPlaySetInputFormat : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetInputFormat* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetInputFormat(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetInputFormat() {};
	};

//

class CTestDevVideoPlayGetOutputFormatList : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetOutputFormatList* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayGetOutputFormatList(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetOutputFormatList() {};
	};

//

class CTestDevVideoPlaySetOutputFormat : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetOutputFormat* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetOutputFormat(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetOutputFormat() {};
	};

//

class CTestDevVideoPlaySetPostProcTypes : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetPostProcTypes* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetPostProcTypes(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetPostProcTypes() {};
	};

//

class CTestDevVideoPlaySetInputCrop : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetInputCrop* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetInputCrop(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetInputCrop() {};
	};

//

class CTestDevVideoPlaySetYuvToRgbOptionsFormat : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetYuvToRgbOptionsFormat* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetYuvToRgbOptionsFormat(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetYuvToRgbOptionsFormat() {};
	};

//

class CTestDevVideoPlaySetYuvToRgbOptions : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetYuvToRgbOptions* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetYuvToRgbOptions(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetYuvToRgbOptions() {};
	};

//

class CTestDevVideoPlaySetRotate : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetRotate* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetRotate(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetRotate() {};
	};

//

class CTestDevVideoPlaySetScale : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetScale* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetScale(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetScale() {};
	};

//

class CTestDevVideoPlaySetCrop : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetCrop* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetCrop(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetCrop() {};
	};

//

class CTestDevVideoPlaySetPostProcOptions : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetPostProcOptions* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetPostProcOptions(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetPostProcOptions() {};
	};

//

class CTestDevVideoPlaySetClockSource : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetClockSource* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetClockSource(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetClockSource() {};
	};

//

class CTestDevVideoPlaySetVideoOutput : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetVideoOutput* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetVideoOutput(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetVideoOutput() {};
	};

//

class CTestDevVideoPlaySynchronizeDecoding : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySynchronizeDecoding* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySynchronizeDecoding(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySynchronizeDecoding() {};
	};

//

class CTestDevVideoPlaySetBufferOptions : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetBufferOptions* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetBufferOptions(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetBufferOptions() {};
	};

//

class CTestDevVideoPlayGetBufferOptions : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetBufferOptions* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayGetBufferOptions(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetBufferOptions() {};
	};

//

class CTestDevVideoPlaySetHrdVbv : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetHrdVbv* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlaySetHrdVbv(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetHrdVbv() {};
	};

//

class CTestDevVideoPlayInit : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayInit* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayInit(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayInit() {};
	};

//

class CTestDevVideoPlayStartDSA : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayStartDSA* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayStartDSA(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayStartDSA() {};
	};

//

class CTestDevVideoPlaySetClipRegion : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetClipRegion* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlaySetClipRegion(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetClipRegion() {};
	};

//

class CTestDevVideoPlaySetPauseClipFail : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetPauseClipFail* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlaySetPauseClipFail(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetPauseClipFail() {};
	};

//

class CTestDevVideoPlayAbortDSA : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayAbortDSA* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayAbortDSA(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayAbortDSA() {};
	};

//

class CTestDevVideoPlayIsPlaying : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayIsPlaying* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayIsPlaying(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayIsPlaying() {};
	};

//

class CTestDevVideoPlayRedraw : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayRedraw* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayRedraw(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayRedraw() {};
	};

//

class CTestDevVideoPlayStart : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayStart* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayStart(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayStart() {};
	};

//

class CTestDevVideoPlayStop : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayStop* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayStop(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayStop() {};
	};

//

class CTestDevVideoPlayPause : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayPause* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayPause(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayPause() {};
	};

//

class CTestDevVideoPlayResume : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayResume* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayResume(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayResume() {};
	};

//

class CTestDevVideoPlaySetPosition : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetPosition* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlaySetPosition(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetPosition() {};
	};

//

class CTestDevVideoPlayFreezePicture : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayFreezePicture* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayFreezePicture(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayFreezePicture() {};
	};

//

class CTestDevVideoPlayReleaseFreeze : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayReleaseFreeze* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayReleaseFreeze(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayReleaseFreeze() {};
	};

//

class CTestDevVideoPlayDecodingPosition : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayDecodingPosition* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayDecodingPosition(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayDecodingPosition() {};
	};

//

class CTestDevVideoPlayPlaybackPosition : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayPlaybackPosition* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayPlaybackPosition(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayPlaybackPosition() {};
	};

//

class CTestDevVideoPlayPreDecoderBufferBytes : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayPreDecoderBufferBytes* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayPreDecoderBufferBytes(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayPreDecoderBufferBytes() {};
	};

//

class CTestDevVideoPlayPictureBufferBytes : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayPictureBufferBytes* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayPictureBufferBytes(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayPictureBufferBytes() {};
	};

//

class CTestDevVideoPlayGetPictureCounters : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetPictureCounters* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayGetPictureCounters(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetPictureCounters() {};
	};

//

class CTestDevVideoPlayGetBitstreamCounters : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetBitstreamCounters* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayGetBitstreamCounters(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetBitstreamCounters() {};
	};

//

class CTestDevVideoPlayNumFreeBuffers : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayNumFreeBuffers* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayNumFreeBuffers(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayNumFreeBuffers() {};
	};

//

class CTestDevVideoPlaySetComplexityLevel : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlaySetComplexityLevel* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlaySetComplexityLevel(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlaySetComplexityLevel() {};
	};

//

class CTestDevVideoPlayNumComplexityLevels : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayNumComplexityLevels* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayNumComplexityLevels(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayNumComplexityLevels() {};
	};

//

class CTestDevVideoPlayGetComplexityLevelInfo : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetComplexityLevelInfo* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayGetComplexityLevelInfo(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetComplexityLevelInfo() {};
	};

//

class CTestDevVideoPlayGetBuffer : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetBuffer* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
	virtual void MdvpoNewBuffers();
private:
	CTestDevVideoPlayGetBuffer(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetBuffer() {};
	TBool iCalledBack;
	};

//

class CTestDevVideoPlayWriteCodedData : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayWriteCodedData* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayWriteCodedData(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayWriteCodedData() {};
	};

//

class CTestDevVideoPlayScanAndCopyCodedData : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayScanAndCopyCodedData* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayScanAndCopyCodedData(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayScanAndCopyCodedData() {};
	};

//

class CTestDevVideoPlayExtensionWriteCodedData : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayExtensionWriteCodedData* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayExtensionWriteCodedData(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayExtensionWriteCodedData() {};
	};

//

class CTestDevVideoPlayWritePicture : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayWritePicture* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
	virtual void MdvpoReturnPicture(TVideoPicture* aPicture);
private:
	CTestDevVideoPlayWritePicture(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayWritePicture() {};
	TVideoPicture *iPicture;
	};

//

class CTestDevVideoPlayInputEnd : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayInputEnd* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
	virtual void MdvpoStreamEnd();
private:
	CTestDevVideoPlayInputEnd(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayInputEnd() {};
	TUint iEndCount;
	};

//

class CTestDevVideoPlayGetNewPictureInfo : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetNewPictureInfo* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayGetNewPictureInfo(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetNewPictureInfo() {};
	};

//

class CTestDevVideoPlayNextPicture : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayNextPicture* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
    virtual void MdvpoNewPictures();
	void CheckNewPictureInfoL(TUint aNumPictures, const TTimeIntervalMicroSeconds& aFirst, const TTimeIntervalMicroSeconds& aLast);
	void DoNewPicturesL();
private:
	CTestDevVideoPlayNextPicture(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayNextPicture() {};
private:
	TInt iCurrentPictureNumber;
	CMMFDevVideoPlay* iDevVideoPlay;
	};


//

class CTestDevVideoPlayGetSnapshot : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetSnapshot* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayGetSnapshot(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetSnapshot() {};
	};

//

class CTestDevVideoPlayGetTimedSnapshotTimestamp : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetTimedSnapshotTimestamp* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
	virtual void MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& aPresentationTimestamp, 
											const TPictureId& aPictureId);
private:
	CTestDevVideoPlayGetTimedSnapshotTimestamp(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetTimedSnapshotTimestamp() {};
	TTimeIntervalMicroSeconds iTimestamp;
	TPictureData* iPictureData;
	};

//

class CTestDevVideoPlayGetTimedSnapshotID : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetTimedSnapshotID* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
	virtual void MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& aPresentationTimestamp, 
											const TPictureId& aPictureId);
private:
	CTestDevVideoPlayGetTimedSnapshotID(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetTimedSnapshotID() {};
	TPictureId iPictureId;
	TPictureData* iPictureData;
	};

//

class CTestDevVideoPlayCancelTimedSnapshot : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayCancelTimedSnapshot* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
	virtual void MdvpoTimedSnapshotComplete(TInt aError, 
											TPictureData* aPictureData, 
											const TTimeIntervalMicroSeconds& aPresentationTimestamp, 
											const TPictureId& aPictureId);
private:
	CTestDevVideoPlayCancelTimedSnapshot(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayCancelTimedSnapshot() {};
	};

//

class CTestDevVideoPlayGetSupportedSnapshotFormats : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetSupportedSnapshotFormats* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayGetSupportedSnapshotFormats(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetSupportedSnapshotFormats() {};
	};

//

class CTestDevVideoPlayClock : public CTestDevVideoStep, public MMMFClockSourcePeriodicUtilityObserver
	{
public:
	static CTestDevVideoPlayClock* NewL(const TDesC& aTestName);
	CTestDevVideoPlayClock(const TDesC& aTestName);
	~CTestDevVideoPlayClock();
	void ConstructL();

	virtual TVerdict DoTestStepL();
	static TInt StartThread(TAny* aAny);
	void RunThread();

	// from MMMFClockSourcePeriodicUtilityObserver
	virtual void MmcspuoTick(const TTimeIntervalMicroSeconds& aTime);
protected:
	TInt iError;
	CSystemClockSource* iClockSource;
	CMMFClockSourcePeriodicUtility* iPeriodicUtility;
	TUint iPeriodicUtilityIteration;
	TBool iShutdownSubthread;
	TVerdict iPeriodicUtilityTestVerdict;
	};

//

class CTestDevVideoPlayCommit : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayCommit* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayCommit(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayCommit() {};
	};

//

class CTestDevVideoPlayRevert : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayRevert* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
private:
	CTestDevVideoPlayRevert(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayRevert() {};
	};

//

class CTestDevVideoPlayCustomInterface : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayCustomInterface* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayCustomInterface(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayCustomInterface() {};
	};
  
//
/*
class CTestDevVideoPlayXXX : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayXXX* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayXXX(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayXXX() {};
	};
*/
//
class CTestDevVideoGetBufferWithoutInitialize : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoGetBufferWithoutInitialize* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
	virtual void MdvpoInitComplete(TInt aError);
	virtual void MdvpoNewBuffers();
private:
	CTestDevVideoGetBufferWithoutInitialize(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoGetBufferWithoutInitialize() {};
	TBool iCalledBack;
	};

//

class CTestDevVideoPlayGetDecoderPostProcInfo : public CTestDevVideoPlayStep
	{
public:
	static CTestDevVideoPlayGetDecoderPostProcInfo* NewL(const TDesC& aTestName, TTestType aTestType);
	virtual TVerdict DoTestL(CMMFDevVideoPlay& aDevVideoPlay);
private:
	CTestDevVideoPlayGetDecoderPostProcInfo(const TDesC& aTestName, TTestType aTestType);
	~CTestDevVideoPlayGetDecoderPostProcInfo() {};
	TBool ValidPostProcInfo(CPostProcessorInfo* aInfo);
	};

#endif	// TESTDEVVIDEOPLAY_H__
