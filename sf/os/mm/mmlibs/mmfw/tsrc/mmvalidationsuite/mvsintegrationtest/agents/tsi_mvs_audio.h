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

#ifndef TSI_MVS_AUDIO_H
#define TSI_MVS_AUDIO_H

#include "tsi_mvs_agents_suite.h"
#include "tsi_mvs_agents_stepbase.h"


class RTestStepMVSAgent;

// Positive tests //

/**
 * 
 * RTestStepMVSAudPositionPlayRec
 * 
 * 
 */
class RTestStepMVSAudPositionPlayRec : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudPositionPlayRec(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:
    TTimeIntervalMicroSeconds iPlayDuration;
    TTimeIntervalMicroSeconds iRecordDuration;
    };

/**
 * 
 * RTestStepMVSAudRptPlayRec
 * 
 * 
 */
class RTestStepMVSAudRptPlayRec : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudRptPlayRec(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:
    TTimeIntervalMicroSeconds iPlayDuration;
    TTimeIntervalMicroSeconds iRecordDuration;
    };

/**
 * 
 * RTestStepMVSAudOtherPlayRec
 * 
 * 
 */
class RTestStepMVSAudOtherPlayRec : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudOtherPlayRec(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:
    TUint iPlayBitRate;
    TUint iRecordBitRate;
    };


/**
 * 
 * RTestStepMVSAudRecNew
 * 
 * 
 */
class RTestStepMVSAudRecNew : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudRecNew(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid aRecordControllerUid, const TUid aPlayrollerUid);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:
    TUid iRecordControllerUid;
    TUid iPlayControllerUid;
    };

/**
 * 
 * RTestStepMVSAudPlaySettings
 * 
 * 
 */
class RTestStepMVSAudPlaySettings : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudPlaySettings(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:
    };

/**
 * 
 * RTestStepMVSAudRecConfig
 * 
 * 
 */
class RTestStepMVSAudRecConfig : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudRecConfig(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSAudRecCrop
 * 
 * 
 */
class RTestStepMVSAudRecCrop : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudRecCrop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSAudRecMeta
 * 
 * 
 */
class RTestStepMVSAudRecMeta : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudRecMeta(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSAudioAgentBasic
 * 
 * 
 */
class RTestStepMVSAudioAgentBasic : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudioAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType);
    virtual TVerdict DoTestStepL();
    virtual TVerdict PerformTestL();
protected:
    //Preamble and Postamble
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    
    TBuf<10> iTestType;
    TMvsTestAgents iAgentUnderTest;
    };

/**
 * 
 * RTestStepMVSAudioPlayAgentBasic
 * 
 * 
 */
class RTestStepMVSAudioPlayAgentBasic : public RTestStepMVSAudioAgentBasic
    {
public:
    RTestStepMVSAudioPlayAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType);
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSAudioRecordAgentBasic
 * 
 * 
 */
class RTestStepMVSAudioRecordAgentBasic : public RTestStepMVSAudioAgentBasic
    {
public:
    RTestStepMVSAudioRecordAgentBasic(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aTestType, const TUid aRecordControllerUid, const TUid aPlayControllerUid);
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:
    TUid iRecordControllerUid;
    TUid iPlayControllerUid;
    };

// Negative tests //

/**
 * 
 * RTestStepMVSAudOpenWhenOpen
 * 
 * 
 */
class RTestStepMVSAudOpenWhenOpen : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudOpenWhenOpen(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aKeyName2);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:
    TBuf<KIniNameBuf>            iKeyName2;            // Key name for retrieving 2nd filename
    TFileName                    iFilename2;
    };

/**
 * 
 * RTestStepMVSAudOpenUnsupportedFile
 * 
 * 
 */
class RTestStepMVSAudOpenUnsupportedFile : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudOpenUnsupportedFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSAudOpenWhenPlaying
 * 
 * 
 */
class RTestStepMVSAudOpenWhenPlaying : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudOpenWhenPlaying(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSAudRecUnsupportedFile
 * 
 * 
 */
class RTestStepMVSAudRecUnsupportedFile : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudRecUnsupportedFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
    };

/**
 * 
 * RTestStepMVSAudPlayPausePlay
 * Test added as part of CR1566 (TruePause)
 * Test should work in both A3F and non-A3F even when the lower layers implement the Pause-Resume differently
 * 
 */
class RTestStepMVSAudPlayPausePlay : public RTestStepMVSAudioAgent
    {
public:
    RTestStepMVSAudPlayPausePlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    virtual TVerdict PerformTestL();
    virtual void FsmL(TMvsTestAgentEvents aEventCode);
private:
	TTimeIntervalMicroSeconds iPlayDuration;
    };    
#endif// __TSI_MMF_MVS_AUDIO_H__

