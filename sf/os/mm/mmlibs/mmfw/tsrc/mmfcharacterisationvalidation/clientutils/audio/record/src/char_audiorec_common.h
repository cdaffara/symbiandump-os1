// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef AUDIORECCOMMON_H
#define AUDIORECCOMMON_H

#include <e32base.h>
#include <mdaaudiosampleeditor.h>
#include <testframework.h>
#include <e32std.h>

const TInt KExpectedDeviation= 1500000; // 1.5s

const TInt KOneSec = 1000000; // 1s
const TInt KTwoSec = 2000000; // 2s
const TInt KThreeSec = 3000000; // 3s
const TInt KFourSec = 4000000; // 4s

#define KWavController  270489058

const TUint32 KMMFFourCCCodeVORB = 0x42524F56;  // ('V', 'O', 'R', 'B')

//enum TARState
enum TAudioRecorderState
	{
	EARStateIdle = 0,
	EARStateBeforeOpen,
	EARStateOpened,
	EARStateRecording,
	EARStateRecordInRecording,
	EARStateRecordStopped,
	EARStatePlaying,
	EARStatePlayWhilePlaying,
	EARStatePlayStopped,
	EARStateError
	};
    
// Array to display which state change event occurred.
const TText* const KAudioRecorderState[] =
    {
    _S("EARStateIdle"),
    _S("EARStateBeforeOpen"),
    _S("EARStateOpened"),
    _S("EARStateRecording"),
    _S("EARStateRecordInRecording"),
    _S("EARStateRecordStopped"),
    _S("EARStatePlaying"),
    _S("EARStatePlayWhilePlaying"),
    _S("EARStatePlayStopped"),
    _S("EARStateError")
    };    
    
TAny GetDriveName(TDes& aFileName);

class CMMFMdaAudioRecorderUtility;

/**************************************************
 * Base class to pass the events to the test step class using the RAsyncTestStep
 * 
 * @class RAudioRecTest
 *
 **************************************************/
    
    
class RAudioRecTest : public RAsyncTestStep,
					  public MMdaObjectStateChangeObserver
	{
public:
	RAudioRecTest(const TDesC& aStepName, const TDesC& aSectName);

	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void DoKickoffTestL() = 0;  //-- for the derived class to implement
	void CloseTest();
	
	//Audio Recorder CallBack
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

	//State Machine Functions
	virtual void Fsm(TAudioRecorderState aEventCode) = 0;
	TBool ValidateState();
	void SetExpectedEventAndError(TAudioRecorderState aState,TInt aError);
		
protected:
	TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDivation);
	const CTestSuite* iTestSuite;	
public:
	static TInt TimerCallback(TAny*);
	virtual void DoTimerCallback();
	TCallBack iCallBack;
	CPeriodic* iTimer;
	
	TFileName iSectName;
	TFileName iFileName;
	TFileName iFileName2;
	TFileName iFileName3;
	CMdaAudioRecorderUtility* iRecorder;
	
	TAudioRecorderState iExpectedState;
	TAudioRecorderState iState;
	TInt iExpectedError;
	TInt iError;
	TInt iTestExpectedError;
	RFs iFs;
	};
	


#endif // AUDIORECCOMMON_H
