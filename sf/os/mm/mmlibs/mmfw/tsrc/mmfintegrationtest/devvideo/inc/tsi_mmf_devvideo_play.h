// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declaration of test step classes for DevVideoPlay.
// 
//


#ifndef TSI_MMF_DEVVIDEOPLAY_H
#define TSI_MMF_DEVVIDEOPLAY_H

#include <mmf/devvideo/devvideoplay.h>
#include <mmf/devvideo/videoplayhwdevice.h>

#include "tsi_mmf_devvideo_stepbase.h"

/**
 *
 * RTestStepDevVideoPlayBase
 * - base class for all DevVideoPlay test steps.
 *
 */
class RTestStepDevVideoPlayBase : public RTestStepDevVideoBase, public MMMFDevVideoPlayObserver
    {
public:
    RTestStepDevVideoPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // Executes events of DevVideoPlay 
    void Fsm(TMmfDevVideoPlayEvents aDevVideoPlayEvent);
    // from RTestStep, implementation to be used by all tests
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    // Set the Input and output video formats for Decode
    TInt SetInputOutputFormatsL();
    // check the file header information
    void CheckHeaderInfoL();
    
    // from MMMFDevVideoPlayObserver
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
    CMMFDevVideoPlay* iMMFDevVideoPlay;
    THwDeviceId iDecoderHwDeviceId;
    TUid iDecoderUid;
    TInt iTotalBytesRead;
    TBool iNewPictures;
    TBool iInputEnd;
    TBool iStreamEnd;
    };


// Positive Tests

/**
 *
 * RTestDevVideoPlaySimple
 * - Play a video file, encoded using XviD codec.
 *
 */
class RTestDevVideoPlaySimple : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoPlaySimple* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoPlaySimple(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    };

/**
 *
 * RTestDevVideoListVideoDecoders
 * - Get a list of available video decoders.
 *
 */
class RTestDevVideoListVideoDecoders : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoListVideoDecoders* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoListVideoDecoders(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    };

/**
 *
 * RTestDevVideoListVideoPostProcessors
 * - Get a list of available video post-processors.
 *
 */
class RTestDevVideoListVideoPostProcessors : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoListVideoPostProcessors* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoListVideoPostProcessors(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    };

/**
 *
 * RTestDevVideoPlayConfHwDev
 * - Configure and reconfigure the video HwDevice and play a video file, encoded using XviD codec.
 *
 */
class RTestDevVideoPlayConfHwDev : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoPlayConfHwDev* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // Executes events of DevVideoPlay
    void Fsm(TMmfDevVideoPlayEvents aDevVideoPlayEvent);

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMMFDevVideoPlayObserver
    virtual void MdvpoFatalError(TInt aError);
    virtual void MdvpoInitComplete(TInt aError);
    virtual void MdvpoStreamEnd();

private:
    RTestDevVideoPlayConfHwDev(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    };

/**
 *
 * RTestDevVideoPlayAlloc
 * - Alloc test to load a video HwDevice to play a video file, encoded using XviD codec.
 *
 */
class RTestDevVideoPlayAlloc : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoPlayAlloc* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    void VideoPlayAllocL();
    void DoTestL();
    
    // from RTestStep
    virtual TVerdict DoTestStepL();
    
    // from MMMFDevVideoPlayObserver
    virtual void MdvpoNewBuffers();
    virtual void MdvpoNewPictures();
    virtual void MdvpoFatalError(TInt aError);
    virtual void MdvpoInitComplete(TInt aError);
    virtual void MdvpoStreamEnd();

private:
    RTestDevVideoPlayAlloc(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    TBool iSchedulerStarted;
    };

/**
 *
 * RTestDevVideoPlayStop
 * - Play/stop playing a video file, encoded using XviD codec.
 *
 */
class RTestDevVideoPlayStop : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoPlayStop* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // Executes events of DevVideoPlay
    void Fsm(TMmfDevVideoPlayEvents aDevVideoPlayEvent);

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMMFDevVideoPlayObserver
    virtual void MdvpoNewBuffers();
    virtual void MdvpoNewPictures();

    // timer handling
    static TInt Check(TAny* aObject);
    void DoCheck();
private:
    RTestDevVideoPlayStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    
protected:
    CPeriodic* iTimer;    
    TInt iCheckCounter;
    TBool iNewBuffers;    
    TBool iIsActive;
    };

/**
 *
 * RTestDevVideoPlayPause
 * - Play/pause the play of a video file, encoded using XviD codec.
 *
 */
class RTestDevVideoPlayPause : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoPlayPause* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // Executes events of DevVideoPlay
    void Fsm(TMmfDevVideoPlayEvents aDevVideoPlayEvent);

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMMFDevVideoPlayObserver
    virtual void MdvpoNewBuffers();
    virtual void MdvpoNewPictures();

    // timer handling
    static TInt Check(TAny* aObject);
    void DoCheck();
private:
    RTestDevVideoPlayPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    
protected:
    CPeriodic* iTimer;    
    TInt iCheckCounter;
    TBool iNewBuffers;    
    TBool iIsActive;
    };

/**
 *
 * RTestDevVideoPlayDecodePlaybackPos
 * - Set and get the decoding and playback position and play the video file.
 *
 */
class RTestDevVideoPlayDecodePlaybackPos : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoPlayDecodePlaybackPos* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    // from RTestStep
    virtual TVerdict DoTestStepL();
    // to handle the callback and get the positions
    virtual void MdvpoNewPictures();
    virtual void MdvpoStreamEnd();
private:
    RTestDevVideoPlayDecodePlaybackPos(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    TTimeIntervalMicroSeconds iInitDecodingPos;
    TTimeIntervalMicroSeconds iInitPlaybackPos;
    TTimeIntervalMicroSeconds iFinalDecodingPos;
    TTimeIntervalMicroSeconds iFinalPlaybackPos;
    };

/**
 *
 * RTestDevVideoPlayDecoderInfo
 * - Get information about a specified decoder.
 *
 */
class RTestDevVideoPlayDecoderInfo : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoPlayDecoderInfo* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoPlayDecoderInfo(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    TBool ValidDecoderInfo(CVideoDecoderInfo* aInfo);
    };

// Negative Tests

/**
 *
 * RTestDevVideoPlayUidHwDev
 * - Play a video file, encoded using XviD codec, specifying invalid Uid of video HwDevice.
 *
 */
class RTestDevVideoPlayUidHwDev : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoPlayUidHwDev* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoPlayUidHwDev(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    };

/**
 *
 * RTestDevVideoPlayCorruptFile
 * - Play a corrupted video file, encoded using XviD codec.
 *
 */
class RTestDevVideoPlayCorruptFile : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoPlayCorruptFile* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoPlayCorruptFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    };

/**
 *
 * RTestDevVideoPlayWithoutVideoType
 * - Open and play video file without specifying input / output video  types.
 *
 */
class RTestDevVideoPlayWithoutVideoType : public RTestStepDevVideoPlayBase
    {
public:
    static RTestDevVideoPlayWithoutVideoType* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoPlayWithoutVideoType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aDecoderUid);
    };

#endif    // TSI_MMF_DEVVIDEOPLAY_H
