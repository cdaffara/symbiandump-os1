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
// TestAudioRecOgg.h
// This file contains integration tests of the MMF Audio Client.
// The enabling of agent's user interface and disabling automatic intent 
// features are tested.
// CMdaAudioRecorderUtility
// 
//

#ifndef TEST_OGGRECORDER_H
#define TEST_OGGRECORDER_H

#include "TestRecorder.h"
#include "TSI_MMFACLNT.h"

const int KMaxWriteLength = 4000;
// Audio recorder events

enum TMmfAudioOggRecEvents
    {
    EOggIdleAudioRecorder = 0,
    EOggMoscoStateChange,
    };
   
/**
*
* Base class
*
* @lib "TSI_MMFACLNT.lib"
*
* @class CTestMmfOggAudioRecorderBase
*
*/
class CTestMmfOggAudioRecorderBase : public CTestMmfAclntStep,
                                     public MMdaObjectStateChangeObserver
    {
public:
    static CTestMmfOggAudioRecorderBase* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    CTestMmfOggAudioRecorderBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    void FsmL(TMmfAudioOggRecEvents aEventCode, CMdaAudioClipUtility::TState aRecorderState, TInt aError = KErrNone);
    
    virtual TVerdict DoTestStepL();
    
    // from CTestMmfAclntStep
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();

    // from MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode); 
    void PrepareState(TMmfAudioOggRecEvents aExpectedEvent, TInt aExpectedError = KErrNone);
    
protected:
	
    void StateInit();
    void StateChange(TMmfAudioOggRecEvents aExpectedEvent, CMdaAudioClipUtility::TState aExpectedRecorderState, TInt aExpectedError = KErrNone);
    
    TFileName iFileName;

    TInt iError;
    TInt iExpectedError;
    TInt iState;
    
	CBase* iObject;
    TInt iPreviousState;
	TInt iCurrentState;
	
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
		
    TMmfAudioOggRecEvents iExpectedRecEvent;
    CMdaAudioClipUtility::TState iExpectedRecState;

    CMdaAudioRecorderUtility* iAudioRecorder;
    };



#endif //__TEST_AUDIORECORDERDRM_H__
