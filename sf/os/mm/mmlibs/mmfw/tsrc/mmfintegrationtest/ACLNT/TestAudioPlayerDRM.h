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
// CMdaAudioPlayerUtility
// 
//

#ifndef TEST_AUDIOPLAYERDRM_H
#define TEST_AUDIOPLAYERDRM_H

#include "TSI_MMFACLNT.h"

// Audio events
enum TMmfAudioEvents
    {
    EAudioOpen = 0,
    EAudioPlay,
    EAudioPlayEnd,
    };

/**
*
* Base class
*
* @lib "TSI_MMFACLNT.lib"
*
* @class CTestMmfDRMAudioPlayerBase
*
*/
class CTestMmfDRMAudioPlayerBase : public CTestMmfAclntStep,
                                   public MMdaAudioPlayerCallback
    {
public:
    void FsmL(TMmfAudioEvents aEvent, TInt aError = KErrNone);

    // from CTestMmfAclntStep
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();

    // from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

public:
    TInt iError;
    TFileName iFileName;
    CMdaAudioPlayerUtility* iAudioPlayer;
protected:
	virtual void AfterOpenL();
	CTestMmfDRMAudioPlayerBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId);
	
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TBuf<KIDBufSize> iUniqueId;					// Unique ID for DRM protected content
    };

// Positive Tests

/**
*
* Enable agent's user interface for errors and confirmation requests property. 
* Open and play a protected audio clip from a file for PLAY intent.
*
* @lib "TSI_MMFACLNT.lib"
*
* @class CTestMmfAudioPlayDRMEnableAgentUI
*
*/
class CTestMmfAudioPlayDRMEnableAgentUI : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfAudioPlayDRMEnableAgentUI* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId);

    // from RTestStep
    virtual TVerdict DoTestStepL();
protected:
	void AfterOpenL();

private:
    CTestMmfAudioPlayDRMEnableAgentUI(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId);
    };


/**
*
* Open a protected audio file and disable automatic intent. Play.
*
* @lib "TSI_MMFACLNT.lib"
*
* @class CTestMmfAudioPlayDRMDisableAutoIntent
*
*/
class CTestMmfAudioPlayDRMDisableAutoIntent : public CTestMmfDRMAudioPlayerBase
    {
public:
    static CTestMmfAudioPlayDRMDisableAutoIntent* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId);

    // from RTestStep
    virtual TVerdict DoTestStepL();

protected:
	void AfterOpenL();
private:
    CTestMmfAudioPlayDRMDisableAutoIntent(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId);
    };

#endif //TEST_AUDIOPLAYERDRM_H
