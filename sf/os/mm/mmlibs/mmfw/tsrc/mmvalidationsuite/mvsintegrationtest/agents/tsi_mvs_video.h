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
//

#ifndef TSI_MMF_MVS_VIDEO_H
#define TSI_MMF_MVS_VIDEO_H

#include "tsi_mvs_agents_suite.h"
#include "tsi_mvs_agents_stepbase.h"
#include "u32hal.h"

const TReal32 KScaleWidth1 = 1.5f;
const TReal32 KScaleHeight1 = 1.65f;

// INI File Fields
_LIT(KIniFrameRate, "FrameRate");
_LIT(KIniFrameWidth, "FrameWidth");
_LIT(KIniFrameHeight, "FrameHeight");
_LIT(KIniVideoBitRate, "VideoBitRate");
_LIT(KIniAudioBitRate, "AudioBitRate");
_LIT(KIniVideoType, "VideoType");
_LIT(KIniAudioEnabled, "AudioEnabled");

class CMmfVideoPlayTimer;
class CMvsVideoRecordTimer;
class RTestStepMVSAgent;
class RTestStepMVSVideoAgent;

/**
 * 
 * RTestStepMVSVidAviPlayPos
 * 
 * 
 */
class RTestStepMVSVidAviPlayPos : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidAviPlayPos(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    
    // timer handling
     virtual void Finished();
    
protected:
    CMmfVideoPlayTimer* 		iPlayTimer;
    TBool 						iRestarted;
    TInt 						iPauseCount;
    };

/**
 * 
 * RTestStepMVSVidAviPlayRot
 * 
 * 
 */
class RTestStepMVSVidAviPlayRot : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidAviPlayRot(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSVidAviPlayScl
 * 
 * 
 */
class RTestStepMVSVidAviPlayScl : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidAviPlayScl(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSVidPlaySettings
 * 
 * 
 */
class RTestStepMVSVidPlaySettings : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidPlaySettings(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSVidAviRecPlay
 * 
 * 
 */
class RTestStepMVSVidAviRecPlay : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidAviRecPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
protected:
    //Preamble and Postamble
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
private:
	TInt SetCacheSize();
	TInt CheckCacheError(TInt aError);
    SVMCacheInfo iCurrentCacheSize;
    TReal32		 				iFrameRate;
    TSize 						iFrameSize;
    TInt 						iVideoBitRate;
    TInt 						iAudioBitRate;
    TBuf8<256> 					iVideoType;
    TFourCC 					iAudioType;
    TBool 						iAudioEnabled;
    };

/**
 * 
 * RTestStepMVSVidRecPause
 * 
 * 
 */
class RTestStepMVSVidRecPause : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidRecPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    void Finished();
protected:
	CMvsVideoRecordTimer 		*iRecordTimer;
	CMvsVideoRecordTimer 		*iRecordTimer1;
	CMvsVideoRecordTimer 		*iRecordTimer2;
	TInt 						RecordResume;
	TMVSState 					iCurrentState;
	TInt 						iFlag;	
    };
    
/**
 * 
 * RTestStepMVSVidRecSettings
 * 
 * 
 */
class RTestStepMVSVidRecSettings : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidRecSettings(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSVidRecMeta
 * 
 * 
 */
class RTestStepMVSVidRecMeta : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidRecMeta(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };


/**
 * 
 * RTestStepMVSVideoAgentBasic
 * 
 * 
 */
class RTestStepMVSVideoAgentBasic : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVideoAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType);
    virtual TVerdict DoTestStepL();
    virtual TVerdict PerformTestL();
protected:
    //Preamble and Postamble
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    
    TBuf<16> 					iTestType;
    TMvsTestAgents 				iAgentUnderTest;
    };

/**
 * 
 * RTestStepMVSVideoPlayAgentBasic
 * 
 * 
 */
class RTestStepMVSVideoPlayAgentBasic : public RTestStepMVSVideoAgentBasic
    {
public:
    RTestStepMVSVideoPlayAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType);
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:
	TInt SetCacheSize();
	TInt CheckCacheError(TInt aError);
	TInt RevertCacheSize();
	SVMCacheInfo iCurrentCacheSize;
    };

/**
 * 
 * RTestStepMVSVideoRecordAgentBasic
 * 
 * 
 */
class RTestStepMVSVideoRecordAgentBasic : public RTestStepMVSVideoAgentBasic
    {
public:
    RTestStepMVSVideoRecordAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType);
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:    
    TInt SetCacheSize();
    TInt CheckCacheError(TInt aError);
    TInt RevertCacheSize();
    SVMCacheInfo iCurrentCacheSize;
    };
    
/**
 * 
 * RTestStepMVSVidAviPlayChangeWin
 * 
 * 
 */
class RTestStepMVSVidAviPlayChangeWin : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidAviPlayChangeWin(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    
private:    
    TBool iPlayAgain;
    };

/**
 * 
 * RTestStepMVSVidAviPlayOverlayText
 * 
 * 
 */
class RTestStepMVSVidAviPlayOverlayText : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidAviPlayOverlayText(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    
protected:
    //Preamble and Postamble
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    
private:    
    CWindowGc*  iGc;    
    CFont*      iFont;        
    };

/**
 * 
 * RTestStepMVSVidAviPlayCropRegion
 * 
 * 
 */
class RTestStepMVSVidAviPlayCropRegion : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidAviPlayCropRegion(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };
            
/**
 * 
 * RTestStepMVSVidAviPlayDisplaySetting
 * 
 * 
 */
class RTestStepMVSVidAviPlayDisplaySetting : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidAviPlayDisplaySetting(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    
protected:
    virtual void PlayL(CMVSVideoPlayAgent& aVideoPlayAgent,
                       const TRect& aVideoExtent,
                       const TRect& aWindowClipping);        
    };


// Negative tests //

/**
 * 
 * RTestStepMVSVidPlayUnsupportedFile
 * 
 * 
 */
class RTestStepMVSVidPlayUnsupportedFile : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidPlayUnsupportedFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:
    TInt 						iExpectedError;
    };

/**
 * 
 * RTestStepMVSVidRecUnsupportedFile
 * 
 * 
 */
class RTestStepMVSVidRecUnsupportedFile : public RTestStepMVSVideoAgent
    {
public:
    RTestStepMVSVidRecUnsupportedFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/* 
 * CMmfVideoPlayTimer 
 * - Timer for those test classes requiring timed stop
 *
 */    
class CMmfVideoPlayTimer : public CTimer
    {
public:
    static CMmfVideoPlayTimer* NewL(RTestStepMVSVideoAgent* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    void Start();
protected:
    TInt RunError(TInt aError);
    void RunL();
private:
    CMmfVideoPlayTimer(RTestStepMVSVideoAgent* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    RTestStepMVSVideoAgent* 	iParent;
    TTimeIntervalMicroSeconds32 iWaitTime;
    };

/* 
 * CMvsVideoRecordTimer 
 * - Timer for those test classes requiring timed stop
 *
 */    
class CMvsVideoRecordTimer : public CTimer
    {
public:
    static CMvsVideoRecordTimer* NewL(RTestStepMVSVidRecPause* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    void Start();
protected:
    TInt RunError(TInt aError);
    void RunL();
private:
    CMvsVideoRecordTimer(RTestStepMVSVidRecPause* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    RTestStepMVSVidRecPause* 	iParent;
    TTimeIntervalMicroSeconds32 iWaitTime;
    };
#endif// __TSI_MMF_MVS_VIDEO_H__

