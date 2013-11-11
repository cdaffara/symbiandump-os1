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
// CMdaAudioRecorderUtility
// 
//

#ifndef __TESTSTEP_AUDIORECORDERDRM_H__
#define __TESTSTEP_AUDIORECORDERDRM_H__

#include <mdaaudiosampleeditor.h>
#include "TestStepUnitMMFAudClient.h"


// Audio recorder events
enum TMmfAudioRecorderEvents
    {
    EIdleRecorder = 0,
    EMoscoStateChange,
    };


/**
*
* Base class
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioRecorderBase
*
*/
class CTestMmfDRMAudioRecorderBase : public CTestStepUnitMMFAudClient,
                                     public MMdaObjectStateChangeObserver
    {
public:
    void FsmL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError = KErrNone);
    void FsmHandleL(TMmfAudioRecorderEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError = KErrNone);

    // from CTestStepUnitMMFAudClient
	virtual enum TVerdict DoTestStepPreambleL();
	virtual enum TVerdict DoTestStepPostambleL();

    // from MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, 
    TInt aCurrentState, TInt aErrorCode);

protected:
	CTestMmfDRMAudioRecorderBase();
    void StateInit();
    void StateChange(TMmfAudioRecorderEvents aExpectedEvent, CMdaAudioClipUtility::TState aExpectedRecorderState, TInt aExpectedError = KErrNone);
	void EnableLogs(TBool aEnable);
	
    TInt iError;
    TInt iExpectedError;
    TInt iState;

    CBase* iObject;
    TInt iPreviousState;
    TInt iCurrentState;

    TMmfAudioRecorderEvents iExpectedRecEvent;
    CMdaAudioClipUtility::TState iExpectedRecState;

    CMdaAudioRecorderUtility* iAudioRecorder;
    TBool iEnableLogs;
    };


// Positive Tests

/**
*
* Open and play a protected audio clip from a file for PLAY intent. (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioRecPlayFileSource
*
*/
class CTestMmfDRMAudioRecPlayFileSource : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfDRMAudioRecPlayFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    CTestMmfDRMAudioRecPlayFileSource();
    };


/**
*
* Open and play a protected audio clip from a file for PLAY intent. 
* (TMMFileHandleSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioRecPlayFileHandleSource
*
*/
class CTestMmfDRMAudioRecPlayFileHandleSource : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfDRMAudioRecPlayFileHandleSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, 
        TInt aCurrentState, TInt aErrorCode);

private:
    CTestMmfDRMAudioRecPlayFileHandleSource();
    };


/**
*
* Open, play and stop the play of the protected audio clip 
* from a file for PLAY/STOP intents. (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioRecPlayStopFileSource
*
*/
class CTestMmfDRMAudioRecPlayStopFileSource : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfDRMAudioRecPlayStopFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    CTestMmfDRMAudioRecPlayStopFileSource();
    };


/**
*
* Alloc test to open and play a protected audio clip from a file for PLAY intent. 
* (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioRecPlayAllocFileSource
*
*/
class CTestMmfDRMAudioRecPlayAllocFileSource : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfDRMAudioRecPlayAllocFileSource* NewL();

    void AudioRecPlayAllocL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    CTestMmfDRMAudioRecPlayAllocFileSource();
    };


/**
*
* Open and play a protected audio clip from a file for PLAY intent with Unique Id.
* (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioRecPlayUniqueIdFileSource
*
*/
class CTestMmfDRMAudioRecPlayUniqueIdFileSource : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfDRMAudioRecPlayUniqueIdFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

private:
    CTestMmfDRMAudioRecPlayUniqueIdFileSource();
    };


/**
*
* Alloc test to open and play a protected audio clip from a file for PLAY intent.
* (TMMFileHandleSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioRecPlayAllocFileHandleSource
*
*/
class CTestMmfDRMAudioRecPlayAllocFileHandleSource : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfDRMAudioRecPlayAllocFileHandleSource* NewL();

    void AudioRecPlayAllocL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, 
        TInt aCurrentState, TInt aErrorCode);

private:
    CTestMmfDRMAudioRecPlayAllocFileHandleSource();
    };


// Negative Tests

/**
*
* Open a protected audio clip from a file with rights for UNKNOWN intent. (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioRecOpenUnknownFileSource
*
*/
class CTestMmfDRMAudioRecOpenUnknownFileSource : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfDRMAudioRecOpenUnknownFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, 
        TInt aCurrentState, TInt aErrorCode);
    
private:
    CTestMmfDRMAudioRecOpenUnknownFileSource();
    };


/**
*
* Open a protected audio clip from a file for PLAY intent with NO rights. (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioRecPlayNoRightsFileSource
*
*/
class CTestMmfDRMAudioRecPlayNoRightsFileSource : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfDRMAudioRecPlayNoRightsFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, 
        TInt aCurrentState, TInt aErrorCode);

private:
    CTestMmfDRMAudioRecPlayNoRightsFileSource();
    };


/**
*
* Open a protected audio clip from a file for PLAY intent with INVALID Unique Id. 
* (TMMFileSource).
*
* @lib "TSU_MMF_ACLNT_01.lib"
*
* @class CTestMmfDRMAudioRecPlayInvalidIdFileSource
*
*/
class CTestMmfDRMAudioRecPlayInvalidIdFileSource : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfDRMAudioRecPlayInvalidIdFileSource* NewL();

    // from RTestStep
    virtual TVerdict DoTestStepL();

    // from MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, 
        TInt aCurrentState, TInt aErrorCode);

private:
    CTestMmfDRMAudioRecPlayInvalidIdFileSource();
    };


#endif //__TESTSTEP_AUDIORECORDERDRM_H__
