// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains testing steps for the unit test of the MMF Audio Client.
// The client is opened with DRM intent and played.
// CMdaAudioPlayerUtility
// 
//

#ifndef __TESTSTEP_AUDIOPLAYERDRM_H__
#define __TESTSTEP_AUDIOPLAYERDRM_H__

#include <mdaaudiosampleplayer.h>
#include "TestStepUnitMMFAudClient.h"


// Audio events
enum TMmfAudioPlayerEvents
    {
    EAudioOpen = 0,
    EAudioPlay,
    EAudioPlayEnd,
    };


/**
*
* Base class
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPlayerBase
*
*/
class CTestMmfDRMAudioPlayerBase : public CTestStepUnitMMFAudClient,
                                   public MMdaAudioPlayerCallback
    {
public:
    void FsmL(TMmfAudioPlayerEvents aEvent);
    void FsmHandleL(TMmfAudioPlayerEvents aEvent);
    void EnableLogs(TBool aEnable);
    
    // from CTestStepUnitMMFAudClient
	virtual enum TVerdict DoTestStepPreambleL();
	virtual enum TVerdict DoTestStepPostambleL();

    // from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);
protected:
	CTestMmfDRMAudioPlayerBase();
public:
    TInt iError;
    CMdaAudioPlayerUtility* iAudioPlayer;
    TBool iEnableLogs;
    };


// Positive Tests

/**
*
* Open and play a protected audio clip from a file for PLAY intent. (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPlayFileSource
*
*/
class CTestMmfDRMAudioPlayFileSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioPlayFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    CTestMmfDRMAudioPlayFileSource();
    };


/**
*
* Open and play a protected audio clip from a file for PLAY intent. 
* (TMMFileHandleSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPlayFileHandleSource
*
*/
class CTestMmfDRMAudioPlayFileHandleSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioPlayFileHandleSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
    CTestMmfDRMAudioPlayFileHandleSource();
    };


/**
*
* Open, play, pause, continue and stop the play of the protected audio clip 
* from a file for PLAY/PAUSE/CONTINUE/STOP intents. (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPPCSFileSource
*
*/
class CTestMmfDRMAudioPPCSFileSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioPPCSFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    CTestMmfDRMAudioPPCSFileSource();
    };


/**
*
* Alloc test to open and play a protected audio clip from a file for PLAY intent.
* (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPlayAllocFileSource
*
*/
class CTestMmfDRMAudioPlayAllocFileSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioPlayAllocFileSource* NewL();

    void AudioPlayAllocL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    CTestMmfDRMAudioPlayAllocFileSource();
    };


/**
*
* Open and play a protected audio clip from a file for PLAY intent with Unique Id.
* (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPlayUniqueIdFileSource
*
*/
class CTestMmfDRMAudioPlayUniqueIdFileSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioPlayUniqueIdFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    CTestMmfDRMAudioPlayUniqueIdFileSource();
    };


/**
*
* Alloc test to open and play a protected audio clip from a file for PLAY intent.
* (TMMFileHandleSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPlayAllocFileHandleSource
*
*/
class CTestMmfDRMAudioPlayAllocFileHandleSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioPlayAllocFileHandleSource* NewL();

    void AudioPlayAllocL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
    CTestMmfDRMAudioPlayAllocFileHandleSource();
    };


// Negative Tests

/**
*
* Open a protected audio clip from a file with rights for UNKNOWN intent. (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioOpenUnknownFileSource
*
*/
class CTestMmfDRMAudioOpenUnknownFileSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioOpenUnknownFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
    CTestMmfDRMAudioOpenUnknownFileSource();
    };


/**
*
* Open a protected audio clip from a file for PLAY intent with NO rights. (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPlayNoRightsFileSource
*
*/
class CTestMmfDRMAudioPlayNoRightsFileSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioPlayNoRightsFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
    CTestMmfDRMAudioPlayNoRightsFileSource();
    };


/**
*
* Open and play a protected audio clip from a file for PLAY intent 
* with rights count set to 1. (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPlayRightsCountFileSource
*
*/
class CTestMmfDRMAudioPlayRightsCountFileSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioPlayRightsCountFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    CTestMmfDRMAudioPlayRightsCountFileSource();
    };


/**
*
* Open a protected audio clip from a file for PLAY intent with INVALID Unique Id. 
* (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPlayInvalidIdFileSource
*
*/
class CTestMmfDRMAudioPlayInvalidIdFileSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioPlayInvalidIdFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
    CTestMmfDRMAudioPlayInvalidIdFileSource();
    };

/**
*
* Opens a protected audio clip from a file for PLAY and STOP intent.This 
* TestCase checks whether the Audio controller correctly calls execute 
* intent on stopping.
* (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioPlayAndStopFileSource
*
*/
class CTestMmfDRMAudioPlayAndStopFileSource : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfDRMAudioPlayAndStopFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    CTestMmfDRMAudioPlayAndStopFileSource();
    };

    /**
     *
     * Open a protected audio clip, switch the state between play and pause
     * very quickly (0.05 second each) for PLAY/PAUSE/CONTINUE/STOP intents 
     * to test PDEF117405. (TMMFileSource).
     *
     * @lib "TSU_MMF_ACLNT_01.lib"
     *
     * @class CTestMmfDRMAudioPPCFFileSource
     *
     */
   class CTestMmfDRMAudioPPCFFileSource : public CTestMmfDRMAudioPlayerBase
      {
  public:
      static CTestMmfDRMAudioPPCFFileSource* NewL();

      // from RTestStep
      virtual TVerdict DoTestStepL();

  private:
      CTestMmfDRMAudioPPCFFileSource();
      };
           
    
#endif //__TESTSTEP_AUDIOPLAYERDRM_H__
