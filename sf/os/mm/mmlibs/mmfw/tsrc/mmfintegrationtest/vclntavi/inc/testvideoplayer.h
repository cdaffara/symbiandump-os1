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
// Header file: Video Player Tests.
// 
//

#ifndef __TESTVIDEOPLAYER_H__
#define __TESTVIDEOPLAYER_H__

#include "tsi_mmf_vclntavi_stepbase.h"
#include "u32hal.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfvideoenums.h>
#endif


/**
 * Load and initialise a video file.
 *
 * RTestVclntPlayAviFile
 *
 */
class RTestVclntPlayAviFile : public RTestVclntAviPlayerStep
    {
public:
    RTestVclntPlayAviFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aPlay);
    static RTestVclntPlayAviFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aPlay);
    static RTestVclntPlayAviFile* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aPlay);
    virtual TVerdict DoTestStepL();
    virtual TVerdict DoTestL(CVideoPlayerUtility* aVideo);
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
    virtual TVerdict SetCacheSize();
protected:
    const TBool iPlay;
    TTimeIntervalMicroSeconds iDuration;
    };


/**
 * Load and initialise an video descriptor.
 *
 * RTestVclntPlayAviDes
 *
 */
class RTestVclntPlayAviDes : public RTestVclntAviPlayerStep
    {
public:
    RTestVclntPlayAviDes(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aPlay) ;
    static RTestVclntPlayAviDes* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aPlay);
    static RTestVclntPlayAviDes* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError,const TBool aPlay);
    virtual TVerdict DoTestStepL();
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
protected:
    HBufC8* iVideo;                        // Buffer for holding video descriptor data
    TTimeIntervalMicroSeconds iDuration;// Stores duration of video
    const TBool iPlay;                    // Does this test require playing the video
    } ;

/**
 * Open video from a URL and play.
 *
 * RTestVclntPlayAviUrl
 *
 */
 
class RTestVclntPlayAviUrl : public RTestVclntAviPlayerStep
    {
public:
    RTestVclntPlayAviUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aPlay);
    static RTestVclntPlayAviUrl* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aPlay);
    static RTestVclntPlayAviUrl* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TBool aPlay);
    virtual TVerdict DoTestStepL();
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
protected:
    TTimeIntervalMicroSeconds iDuration;// Stores duration of video
    const TBool iPlay;                    // Does this test require playing the video
    };

/**
 * Set and enquire framerate
 *
 * RTestVclntEnqFrameRate
 *
 */
class RTestVclntEnqFrameRate : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntEnqFrameRate* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aFrameRate);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);

private:
    RTestVclntEnqFrameRate(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aFrameRate);

private:
    TReal32 iFrameRate;
    };


/**
 * set position and play form this position.
 * 
 * RTestVclntPosition
 *
 */
class RTestVclntPosition : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntPosition* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aPosition);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntPosition(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aPosition);

private:
    TTimeIntervalMicroSeconds iPosition;
    };

/**
 * Video priority
 * 
 * RTestVclntPriority
 *
 */
class RTestVclntPriority : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntPriority* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aPriority);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntPriority(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aPriority);

private:
    TInt iPriority;
    };


/**
 * Video duration
 * 
 * RTestVclntDuration
 *
 */
class RTestVclntDuration : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntDuration* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aDuration);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
    virtual TVerdict SetCacheSize();
private:
    RTestVclntDuration(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aDuration);
    SVMCacheInfo iCurrentCacheSize;
    TVerdict CheckCacheError(TInt aError);
    };


/**
 * Video Volume
 * 
 * RTestVclntVolume
 *
 */
class RTestVclntVolume : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntVolume* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aVolume);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntVolume(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aVolume);

private:
    TInt iVolume;
    };


/**
 * Video open, close and then open again.
 * 
 * RTestVclntCloseOpen
 *
 */
class RTestVclntCloseOpen : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntCloseOpen* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
private:
    RTestVclntCloseOpen(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    TInt iOpenCount;
    };



/**
 * Video pause.
 * 
 * RTestVclntPause
 *
 */
class RTestVclntPause : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntPause* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
private:
    RTestVclntPause(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    TUint iPlayerDuration;
    TUint iActualDuration;
    };


/**
 * Video pause.
 * 
 * RTestVclntBalance
 *
 */
class RTestVclntBalance : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntBalance* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aBalance);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntBalance(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aBalance);

private:
    TInt iBalance;
    };



/**
 * Video set play window and delete play window.
 * 
 * RTestVclntPlayWindow
 *
 */
class RTestVclntPlayWindow : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntPlayWindow* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aStart, const TTimeIntervalMicroSeconds aEnd);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntPlayWindow(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TTimeIntervalMicroSeconds aStart, const TTimeIntervalMicroSeconds aEnd);

private:
    TTimeIntervalMicroSeconds iStart;
    TTimeIntervalMicroSeconds iEnd;
    };


/**
 * Video get meta data.
 * 
 * RTestVclntMeta
 *
 */
class RTestVclntMeta : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntMeta* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntMeta(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);

private:
    TTimeIntervalMicroSeconds iStart;
    TTimeIntervalMicroSeconds iEnd;
    };


/**
 * Video get framesize.
 * 
 * RTestVclntFrameSize
 *
 */
class RTestVclntFrameSize : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntFrameSize* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntFrameSize(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
private:
    TSize iFrameSize;
    };


/**
 * Video get mime type.
 * 
 * RTestVclntMimeType
 *
 */
class RTestVclntMimeType : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntMimeType* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntMimeType(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };


/**
 * Video player scale values test.
 * 
 * RTestVclntScale
 *
 */
class RTestVclntScale : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntScale* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntScale(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };
    
/**
 * Crop test for video
 * 
 * RTestVclntCrop
 *
 */
class RTestVclntCrop : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntCrop* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntCrop(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

#ifdef SYMBIAN_BUILD_GCE

/**
Auto scale test for video
*/

class RTestVclntAutoScale : public RTestVclntPlayAviFile
	{
public:
	static RTestVclntAutoScale* NewL(const TDesC& aTestName);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntAutoScale(const TDesC& aTestName);	
	};

#endif // SYMBIAN_BUILD_GCE

/**
 * Frame test for video.
 * 
 * RTestVclntGetFrame
 *
 */
class RTestVclntGetFrame : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntGetFrame* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TBool aUseIntentAPI);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
protected:
    virtual void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
private:
    RTestVclntGetFrame(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TBool aUseIntentAPI);
    TBool iUseIntentAPI;
    };


/**
 * Video rebuffering.
 * 
 * RTestVclntRebuffering
 *
 */
class RTestVclntRebuffering : public RTestVclntPlayAviFile, public MVideoLoadingObserver
    {
public:
    static RTestVclntRebuffering* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
    void MvloLoadingStarted();
    void MvloLoadingComplete();
private:
    RTestVclntRebuffering(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Video get framesize.
 * 
 * RTestVclntRepeat
 *
 */
class RTestVclntRepeat : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntRepeat* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aRepeat);
    virtual TVerdict DoTestL(CVideoPlayerUtility* aPlayer);
private:
    RTestVclntRepeat(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, const TInt aRepeat);
private:
    TInt iRepeat;
    };



/**
 * Video Delete object while playing
 * 
 * RTestVclntDelete
 *
 */
class RTestVclntDelete : public RTestVclntPlayAviFile
    {
public:
    static RTestVclntDelete* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
private:
    RTestVclntDelete(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Play Video file
 * 
 * RTestVclntPlayAviFileHandle
 *
 */
class RTestVclntPlayAviFileHandle : public RTestVclntAviPlayerStep
    {
public:
    RTestVclntPlayAviFileHandle(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, const TBool aAlloc);
    static RTestVclntPlayAviFileHandle* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, const TBool aAlloc);
    static RTestVclntPlayAviFileHandle* NewLC(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, const TBool aAlloc);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
	virtual TInt SetCacheSize();
	virtual TInt CheckCacheError(TInt aError);

protected:
    TVerdict PerformTestStepL();
    const TBool iAlloc;                        // Is this test an Alloc test
    TTimeIntervalMicroSeconds iDuration;    // Stores duration of video
    RFs iFs;
    RFile iFile;
    TBool iIsOpen;
    };
    
/**

RTestVideoPlayCapabilityVelocity

*/
class RTestVideoPlayCapabilityVelocity: public RTestVclntAviPlayerStep
    {
public:
	RTestVideoPlayCapabilityVelocity(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    static RTestVideoPlayCapabilityVelocity* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
protected:
	TVideoPlayRateCapabilities iVideoPlayRateCapabilities;
	TInt iPlayVelocity;
    RFs iFs;
    RFile iFile;
    };

/**

RTestVideoPlayStepFrame

*/
class RTestVideoPlayStepFrame: public RTestVclntAviPlayerStep
    {
public:
	RTestVideoPlayStepFrame(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    static RTestVideoPlayStepFrame* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
    static TInt TimerCallback(TAny* aPtr);
protected:
	TVideoPlayRateCapabilities iVideoPlayRateCapabilities;
	CPeriodic* iTimer;
	TInt iPlayVelocity;
    RFs iFs;
    RFile iFile;
    };
    
/**

RTestVideoPlayCapabilityVelocity

*/
class RTestVideoPlayAudVidEnable: public RTestVclntAviPlayerStep
    {
public:
	RTestVideoPlayAudVidEnable(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    static RTestVideoPlayAudVidEnable* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
protected:
	TInt iPlayVelocity;
    RFs iFs;
    RFile iFile;
    };

/**

RTestVideoPlayAutoScale

*/
class RTestVideoPlayAutoScale: public RTestVclntAviPlayerStep
    {
public:
	RTestVideoPlayAutoScale(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    static RTestVideoPlayAutoScale* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
protected:
	TInt iPlayVelocity;
    RFs iFs;
    RFile iFile;
    };

#endif //__TESTVIDEOPLAYER_H__
