// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Common header file for all A3F DevSound Integration test steps to share macros, static functions,
// constants and enumerations.
// 
//

#ifndef TSI_A3F_DEVSOUND_TESTSTEP_H
#define TSI_A3F_DEVSOUND_TESTSTEP_H

// System includes
#include <e32base.h>
#include <testframework.h>
#include <mmf/server/sounddevice.h>

// STR_SWITCH... STR_CASE... STR_SWITCH_END macro can be used as switch case that would take string.
#define STR_SWITCH(str)  {\
		TBuf<100> __ps(str);\
		while(1) {

#define STR_SWITCH_END()  break; } }

#define STR_CASE(str) if(__ps == str)

#define DEFAULT_CASE()

// Panic function
static void Panic(const TDesC &aCategory, TInt aReason)
	{
	User::Panic(aCategory, aReason);
	}

// Constants declarations
const TInt KNameSize = 25;
const TInt KBufferLen = 256;
const TInt KMicroSecsInOneSec = 1000000;
const TInt KMicroSecsInTenSecs = 10000000;
const TInt KMicroSecsInOneMinute = 60000000;
const TInt KFrequency1 = 100;
const TInt KFrequency2 = 200;
const TInt KNULLVolume = 0;
const TInt KNULLSamples = 0;

// Enumerations
enum TDevSoundPlayState
	{
	EStatePlayStart = 0,
	EStatePlayInitializing,
	EStatePlaying,
	EStatePlayPaused,
	EStatePlayStopped
	};

enum TDevSoundPlayEvents
	{
	EEventPlayStart = 0,
	EEventInitCompletePlay,
	EEventBTBF,
	EEventPlayTimerComplete,
	EEventPlayPaused,
	EEventPlayStopped
	};

enum TPanicCodes
	{
	EFsmPausedTwice = 0,
	EFsmIncorrectErrorPassed,
	EInvalidCallbackCall
	};

#endif // TSI_A3F_DEVSOUND_TESTSTEP_H
