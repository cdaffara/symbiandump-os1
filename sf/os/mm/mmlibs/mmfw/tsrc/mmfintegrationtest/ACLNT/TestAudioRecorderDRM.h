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
// This file contains integration tests of the MMF Audio Client.
// The enabling of agent's user interface and disabling automatic intent 
// features are tested.
// CMdaAudioRecorderUtility
// 
//

#ifndef TEST_AUDIORECORDERDRM_H
#define TEST_AUDIORECORDERDRM_H

#include "TSI_MMFACLNT.h"

// Audio recorder events
enum TMmfAudioRecEvents
    {
    EIdleAudioRecorder = 0,
    EMoscoStateChange,
    };


/**
*
* Base class
*
* @lib "TSI_MMFACLNT.lib"
*
* @class CTestMmfDRMAudioRecorderBase
*
*/
class CTestMmfDRMAudioRecorderBase : public CTestMmfAclntStep,
                                     public MMdaObjectStateChangeObserver
    {
public:
    void FsmL(TMmfAudioRecEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError = KErrNone);

    // from CTestMmfAclntStep
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();

    // from MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, 
        TInt aCurrentState, TInt aErrorCode);

protected:
	CTestMmfDRMAudioRecorderBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId);
    void StateInit();
    void StateChange(TMmfAudioRecEvents aExpectedEvent, CMdaAudioClipUtility::TState aExpectedRecorderState, TInt aExpectedError = KErrNone);
    virtual void AfterOpenL();

    TFileName iFileName;

    TInt iError;
    TInt iExpectedError;
    TInt iState;

	CBase* iObject;
    TInt iPreviousState;
	TInt iCurrentState;
	
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TBuf<KIDBufSize> iUniqueId;					// Unique ID for DRM protected content
	
    TMmfAudioRecEvents iExpectedRecEvent;
    CMdaAudioClipUtility::TState iExpectedRecState;

    CMdaAudioRecorderUtility* iAudioRecorder;
    };


// Positive Tests

/**
*
* Enable agent's user interface for errors and confirmation requests property. 
* Open and play a protected audio clip from a file for PLAY intent.
*
* @lib "TSI_MMFACLNT.lib"
*
* @class CTestMmfAudioRecPlayDRMEnableAgentUI
*
*/
class CTestMmfAudioRecPlayDRMEnableAgentUI : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfAudioRecPlayDRMEnableAgentUI* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId);

    // from RTestStep
    virtual TVerdict DoTestStepL();
protected:
	virtual void AfterOpenL();
private:
    CTestMmfAudioRecPlayDRMEnableAgentUI(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId);
    };


/**
*
* Open a protected audio file and disable automatic intent. Play.
*
* @lib "TSI_MMFACLNT.lib"
*
* @class CTestMmfAudioRecPlayDRMDisableAutoIntent
*
*/
class CTestMmfAudioRecPlayDRMDisableAutoIntent : public CTestMmfDRMAudioRecorderBase
    {
public:
    static CTestMmfAudioRecPlayDRMDisableAutoIntent* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId);
    // from RTestStep
    virtual TVerdict DoTestStepL();
protected:
	virtual void AfterOpenL();
private:
    CTestMmfAudioRecPlayDRMDisableAutoIntent(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId);
    };

#endif //TEST_AUDIORECORDERDRM_H
