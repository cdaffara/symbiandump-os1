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
// Declaration of test step classes for DevVideoRecord.
// 
//


#ifndef TSI_MMF_DEVVIDEORECORD_H
#define TSI_MMF_DEVVIDEORECORD_H

#include <mmf/devvideo/devvideorecord.h>

#include "tsi_mmf_devvideo_stepbase.h"


/**
 *
 * RTestStepDevVideoRecordBase
 * - base class for all DevVideoRecord test steps.
 *
 */
class RTestStepDevVideoRecordBase : public RTestStepDevVideoBase, public MMMFDevVideoRecordObserver 
    {
public:
    RTestStepDevVideoRecordBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    // Executes events of DevSound
    void Fsm(TMmfDevVideoRecordEvents aDevVideoRecordEvent);
    // from RTestStep, implementation to be used by all tests
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    // Set the Input and output video formats for Encode
    TInt SetInputOutputFormatsL();
    // from MMMFDevVideoRecordObserver
    virtual void MdvroReturnPicture(TVideoPicture* aPicture);
    virtual void MdvroSupplementalInfoSent();
    virtual void MdvroNewBuffers();
    virtual void MdvroFatalError(TInt aError);
    virtual void MdvroInitializeComplete(TInt aError);
    virtual void MdvroStreamEnd();

protected:
    CMMFDevVideoRecord* iMMFDevVideoRecord;
    THwDeviceId iEncoderHwDeviceId;
    TUid iEncoderUid;
    CFbsBitmap* iBitmap;
    HBufC8* iDataBuf;
    TVideoOutputBuffer* iOutputBuf;
    TInt iCallback1;
    TInt iCallback2;
    };

// Positive Tests

/**
 *
 * RTestDevVideoRecordSimple
 * - Record a video file, encoded using XviD codec.
 *
 */
class RTestDevVideoRecordSimple : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoRecordSimple* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    // from RTestStep
    virtual TVerdict DoTestStepL();
    // from MMMFDevVideoRecordObserver
    virtual void MdvroNewBuffers();

private:
    RTestDevVideoRecordSimple(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    };

/**
 *
 * RTestDevVideoRecordUidHwDev
 * - Record a video file, encoded using XviD codec, specifying Uid of video HwDevice.
 * - Record a video file, encoded using XviD codec, specifying invalid Uid of video HwDevice.
 *
 */
class RTestDevVideoRecordUidHwDev : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoRecordUidHwDev* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TTestType aTestType, const TUid& aEncoderUid);
    void Fsm(TMmfDevVideoRecordEvents aDevVideoRecordEvent);
    // from RTestStep
    virtual TVerdict DoTestStepL();
    // from MMMFDevVideoRecordObserver
    virtual void MdvroNewBuffers();
    
private:
    RTestDevVideoRecordUidHwDev(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TTestType aTestType, const TUid& aEncoderUid);
    };

/**
 *
 * RTestDevVideoListVideoEncoders 
 * - Get a list of available video encoders.
 *
 */
class RTestDevVideoListVideoEncoders : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoListVideoEncoders* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoListVideoEncoders(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    };

/**
 *
 * RTestDevVideoListVideoPreProcessors
 * - Get a list of available video pre-processors.
 *
 */
class RTestDevVideoListVideoPreProcessors : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoListVideoPreProcessors* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoListVideoPreProcessors(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    };

/**
 *
 * RTestDevVideoRecordConfHwDev
 * - Configure and reconfigure the video HwDevice and record a video file, encoded using XviD codec.
 *
 */
class RTestDevVideoRecordConfHwDev : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoRecordConfHwDev* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    // from RTestStep
    virtual TVerdict DoTestStepL();
    // from MMMFDevVideoRecordObserver
    virtual void MdvroNewBuffers();
    
private:
    RTestDevVideoRecordConfHwDev(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    };

/**
 *
 * RTestDevVideoRecordAlloc
 * - Alloc test to load a video HwDevice to record a video file, encoded using XviD codec.
 *
 */
class RTestDevVideoRecordAlloc : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoRecordAlloc* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    void VideoRecordAllocL();
    void DoTestL();
    
    // from RTestStep
    virtual TVerdict DoTestStepL();
    
    // from MMMFDevVideoRecordObserver
    virtual void MdvroNewBuffers();
    virtual void MdvroFatalError(TInt aError);
    
private:
    RTestDevVideoRecordAlloc(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    TBool iSchedulerStarted;
    };

/**
 *
 * RTestDevVideoRecordWithAudioVideoTypes
 * - Open and record video file specifying video and audio types.
 *
 */
class RTestDevVideoRecordWithAudioVideoTypes : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoRecordWithAudioVideoTypes* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoRecordWithAudioVideoTypes(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    };

/**
 *
 * RTestDevVideoRecordWithoutVideoType
 * - Open and record video file without specifying video type.
 *
 */
class RTestDevVideoRecordWithoutVideoType : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoRecordWithoutVideoType* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    void Fsm(TMmfDevVideoRecordEvents aDevVideoRecordEvent);
    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoRecordWithoutVideoType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    };


/**
 *
 * RTestDevVideoRecordStop
 * - Record/stop recording a video file, encoded using XviD codec.
 *
 */
class RTestDevVideoRecordStop : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoRecordStop* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    // Executes events of DevSound
    void Fsm(TMmfDevVideoRecordEvents aDevVideoRecordEvent);
    // from RTestStep
    virtual TVerdict DoTestStepL();
    // from MMMFDevVideoRecordObserver
    virtual void MdvroFatalError(TInt aError);
    virtual void MdvroInitializeComplete(TInt aError);
    virtual void MdvroStreamEnd();

private:
    RTestDevVideoRecordStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    };

/**
 *
 * RTestDevVideoRecordPause
 * - Record/pause recording of a video file, encoded using XviD codec.
 *
 */
class RTestDevVideoRecordPause : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoRecordPause* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    // Executes events of DevSound
    void Fsm(TMmfDevVideoRecordEvents aDevVideoRecordEvent);
    // from RTestStep
    virtual TVerdict DoTestStepL();
    // from MMMFDevVideoRecordObserver
    virtual void MdvroFatalError(TInt aError);
    virtual void MdvroInitializeComplete(TInt aError);
    virtual void MdvroStreamEnd();

private:
    RTestDevVideoRecordPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    };

/**
 *
 * RTestDevVideoRecordRecordingPos
 * - Get current recording position of the video file.
 *
 */
class RTestDevVideoRecordRecordingPos : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoRecordRecordingPos* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    // from RTestStep
    virtual TVerdict DoTestStepL();
    // from MMMFDevVideoRecordObserver
    virtual void MdvroNewBuffers();
    
private:
    RTestDevVideoRecordRecordingPos(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    };

/**
 *
 * RTestDevVideoRecordEncoderInfo
 * - Get information about a specified encoder.
 *
 */
class RTestDevVideoRecordEncoderInfo : public RTestStepDevVideoRecordBase
    {
public:
    static RTestDevVideoRecordEncoderInfo* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    RTestDevVideoRecordEncoderInfo(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid& aEncoderUid);
    TBool ValidEncoderInfoL(CVideoEncoderInfo* aInfo);
    };

#endif    // TSI_MMF_DEVVIDEORECORD_H

