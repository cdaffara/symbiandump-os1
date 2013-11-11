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
// Header file: Video Recorder Tests.
// 
//

#ifndef __TESTVIDEORECORDER_H__
#define __TESTVIDEORECORDER_H__

#include "tsi_mmf_vclntavi_stepbase.h"
#include "u32hal.h"


/**
 * Load and initialise a video file for recording.
 *
 * RTestVclntVideoFile
 *
 */
 
class CTestVclntAviRecordTimer;

class RTestVclntRecordAviFile : public RTestVclntAviRecorderStep
    {
public:
    RTestVclntRecordAviFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, const TBool aRec);
    static RTestVclntRecordAviFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aRec);
    static RTestVclntRecordAviFile* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aRec);
    virtual TVerdict DoTestStepL();
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();    
protected:
    const TBool iRec;                    
    TTimeIntervalMicroSeconds iDuration;// Stores duration of video
    CTestVclntAviRecordTimer* 	iRecordTimer;
    TVideoAspectRatio  iAspectRatio;
    TVideoAspectRatio  iNewAspectRatio;
    RArray<TVideoAspectRatio> iAspectRatios;
    TUint iAudioChannel;
    TUint iNewAudioChannel;
    RArray<TUint> iAudioChannels;
    TUint iAudioSampleRate;
    TUint iNewAudioSampleRate;
    RArray<TUint> iAudioSampleRates;       
    };

/**
 * Load and initialise an video descriptor for recording.
 *
 * RTestVclntRecordAviDes
 *
 */
class RTestVclntRecordAviDes : public RTestVclntAviRecorderStep
    {
public:
    RTestVclntRecordAviDes(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec);
    static RTestVclntRecordAviDes* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec);
    static RTestVclntRecordAviDes* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec);
    virtual TVerdict DoTestStepL();
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
private:
    HBufC8* iVideo;                        // Buffer for holding video descriptor data
    TPtr8 iBuffer;                        // Stores the descriptor data
    TTimeIntervalMicroSeconds iDuration;// Stores duration of video
    const TBool iRec;                    
    };

/**
 * Load and initialise a video URL for recording.
 *
 * RTestVclntVideoUrl
 *
 */
 
class RTestVclntRecordAviUrl : public RTestVclntAviRecorderStep
    {
public:
    RTestVclntRecordAviUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec);
    static RTestVclntRecordAviUrl* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec);
    static RTestVclntRecordAviUrl* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aRec);
    virtual TVerdict DoTestStepL();
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
protected:
    const TBool iRec;                    
    TTimeIntervalMicroSeconds iDuration;// Stores duration of video
    };

/**
 * Get the Configure the recorded file and record.
 *
 * RTestVclntRecordConfig
 *
 */
class RTestVclntRecordConfig : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordConfig* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
    RTestVclntRecordConfig(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Open file-based clip. Close the clip and then open the clip again.
 *
 * RTestVclntRecordCloseOpen
 *
 */
class RTestVclntRecordCloseOpen : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordCloseOpen* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
private:
    RTestVclntRecordCloseOpen(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);

private:
    TInt iFrameRate;
    TInt iOpenCount;
    };


/**
 * Start recording then stop on request.
 *
 * RTestVclntRecordPause
 *
 */
 

class RTestVclntRecordPause : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordPause* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
    RTestVclntRecordPause(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
protected:
    TInt 						iCountNumber;
    TVclntTestRecordEvents 		iCurrentEvent;
    };


/**
 * Check duration.
 *
 * RTestVclntRecordDuration
 *
 */
class RTestVclntRecordDuration : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordDuration* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aDuration);
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
    RTestVclntRecordDuration(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aDuration);
    };


/**
 * Query record time
 *
 * RTestVclntRecordTime
 *
 */
class RTestVclntRecordTime : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordTime* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aTime);
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
    RTestVclntRecordTime(const TDesC& aTestName, const TDesC& SectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aTime);

private:
    TTimeIntervalMicroSeconds iTime;
    };


/**
 * Set gain to maximum and enquire
 *
 * RTestVclntRecordGain
 *
 */
class RTestVclntRecordGain : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordGain* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aGain);
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
    RTestVclntRecordGain(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aGain);

private:
    TInt iGain;
    };

/**
 * Set meta-information.
 *
 * RTestVclntRecordMeta
 *
 */
class RTestVclntRecordMeta : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordMeta* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
    RTestVclntRecordMeta(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };


/**
 * Set priority.
 *
 * RTestVclntRecordPriority
 *
 */

class RTestVclntRecordPriority : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordPriority* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aPriority);
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
    RTestVclntRecordPriority(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aPriority);

private:
    TInt iPriority;
    };


/**
 * Specify maximum clip size in bytes.
 *
 * RTestVclntRecordSize
 *
 */
class RTestVclntRecordSize : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordSize* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
    RTestVclntRecordSize(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };


/**
 * Set / get audio / video types
 *
 * RTestVclntRecordTypes
 *
 */
class RTestVclntRecordTypes : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordTypes* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestL(CVideoRecorderUtility* aRec);
private:
    RTestVclntRecordTypes(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Record utility - Delete object before record operation has completed.
 *
 * RTestVclntRecordDelete
 *
 */
class RTestVclntRecordDelete : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntRecordDelete* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);

private:
    RTestVclntRecordDelete(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Play Video file
 * 
 * RTestVclntRecordAviRFile
 *
 */
class RTestVclntRecordAviRFile : public RTestVclntAviRecorderStep
    {
public:
    RTestVclntRecordAviRFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    static RTestVclntRecordAviRFile* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    static RTestVclntRecordAviRFile* NewLC(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
protected:
    TTimeIntervalMicroSeconds iDuration;// Stores duration of video
    RFs iFs;
    RFile iFile;
    };

/**
 * Play Video file
 * 
 * RTestVclntRecordAviRFileAllParams
 *
 */
class RTestVclntRecordAviRFileAllParams : public RTestVclntAviRecorderStep
    {
public:
    RTestVclntRecordAviRFileAllParams(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TBool aAlloc, TUid aControllerUid, TUid aVideoFormat, TFourCC aAudioType);
    static RTestVclntRecordAviRFileAllParams* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, const TBool aAlloc, TUid aControllerUid, TUid aVideoFormat, TFourCC aAudioType);
    static RTestVclntRecordAviRFileAllParams* NewLC(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, const TBool aAlloc, TUid aControllerUid, TUid aVideoFormat, TFourCC aAudioType);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual TInt SetCacheSize();
	virtual TInt CheckCacheError(TInt aError);
protected:
    TVerdict PerformTestStepL();
    TBool iAlloc;                    
    TTimeIntervalMicroSeconds iDuration;
    TUid iControllerUid;
    TUid iVideoFormat;
    TFourCC iAudioType;
    RFs iFs;
    RFile iFile;
    };

/**
 * 
 * CTestVclntAviRecordTimer 
 * - Timer for those test classes requiring timed stop
 *
 */
class CTestVclntAviRecordTimer : public CTimer
    {
public:
    static CTestVclntAviRecordTimer* NewL(RTestVclntRecordAviFile* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    void Start();
protected:
    TInt RunError(TInt aError);
    void RunL();
private:
    CTestVclntAviRecordTimer(RTestVclntRecordAviFile* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    RTestVclntRecordAviFile* 		iParent;
    TTimeIntervalMicroSeconds32 iWaitTime;
    };                                              

/**
CTestVclntAviRecordSetVideoEnabled
*/
class CTestVclntAviRecordSetVideoEnabled : public RTestVclntAviRecorderStep
    {
public:
	CTestVclntAviRecordSetVideoEnabled(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    static CTestVclntAviRecordSetVideoEnabled* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
protected:
    TTimeIntervalMicroSeconds iDuration;// Stores duration of video
    RFs iFs;
    RFile iFile;
    };    
    
/**
CTestVclntAviRecordSetVideoQuality
*/
class CTestVclntAviRecordSetVideoQuality : public RTestVclntAviRecorderStep
    {
public:
	CTestVclntAviRecordSetVideoQuality(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    static CTestVclntAviRecordSetVideoQuality* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
protected:
    TTimeIntervalMicroSeconds iDuration;// Stores duration of video
    RFs iFs;
    RFile iFile;
    };      
    
/**
CTestVclntAviRecordSetVideoFrameRateFixed
*/
class CTestVclntAviRecordSetVideoFrameRateFixed : public RTestVclntAviRecorderStep
    {
public:
	CTestVclntAviRecordSetVideoFrameRateFixed(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    static CTestVclntAviRecordSetVideoFrameRateFixed* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
protected:
    TTimeIntervalMicroSeconds iDuration;// Stores duration of video
    RFs iFs;
    RFile iFile;
    };
#endif // __TESTVIDEORECORDER_H__
