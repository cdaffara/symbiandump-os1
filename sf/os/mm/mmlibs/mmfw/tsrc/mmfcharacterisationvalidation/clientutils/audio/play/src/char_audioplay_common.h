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

#ifndef AUDIOPLAYCOMMON_H
#define AUDIOPLAYCOMMON_H

#include <testframework.h>
#include <mdaaudiosampleplayer.h>

/**
 * Global Data used within tests
 *
 */
const TInt KNameBufSize			= 25;
const TInt KSizeBuf				= 256;
const TInt KDelay				= 1000000; // 1 second
const TInt KLongDelay			= 5000000; // 5 seconds
const TInt KExpectedDeviation	= 1500000; // 1.5 second tolerance for timing tests

enum TAudioPlayState
	{
	EAPIdle = 0,
	EAPOpening,
	EAPOpenComplete,
	EAPPlaying,
	EAPPlayComplete,
	EAPPlayingAgain,
	EAPPResourceAvailable,
	};

// Array to display which state change event occurred.
const TText* const KAudioPlayState[] =
	{
	_S("EAPIdle"),
	_S("EAPOpening"),
	_S("EAPOpenComplete"),
	_S("EAPPlaying"),
	_S("EAPPlayComplete"),
	_S("EAPPlayingAgain"),
	_S("EAPPResourceAvailable")
	};


class RTestBaseAudioPlay : public RAsyncTestStep,
						   public MMdaAudioPlayerCallback,
						   public MMMFAudioResourceNotificationCallback
	{
public:
	// Callback function from MMdaAudioPlayerCallback
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MapcPlayComplete(TInt aError);
	
	// Callback function from MMMFAudioResourceNotificationCallback
	void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8 &aNotificationData);
	
protected:
	// Constructor
	RTestBaseAudioPlay(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Standard functions from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();
	virtual void DoKickoffTestL() = 0; //-- for the derived class to implement
		
	// State machine function
	virtual void Fsm() = 0;
	
	// Function to set the data member iFileName
	void InitialiseFileName();

	// Function to compare duration with expected time with an allowed deviation
	TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation);

	// Function to set the expected state and error
	void SetExpectedState(TAudioPlayState aExpState, TInt aExpError = KErrNone);

	// Function to end test when the wrong state is encountered
	void WrongState(TInt aError = KErrNone);
	
public:
	static TInt TimerCallback(TAny*);
	virtual void DoTimerCallback();
	
	// Members to hold the ini file sections and the file name for each test.
	TBuf<KNameBufSize> iSectName;	// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;	// Key name for retrieving filename
	TFileName iFileName;
	CMdaAudioPlayerUtility* iUtility;
	TCallBack iCallBack;
	CPeriodic* iTimer;
	TAudioPlayState iState;
	TAudioPlayState iExpState;
	TInt iExpError;
	};

#endif // AUDIOPLAYCOMMON_H
