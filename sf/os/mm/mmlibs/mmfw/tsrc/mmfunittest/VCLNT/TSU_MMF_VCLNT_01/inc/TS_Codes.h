// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>


#ifndef TS_CODES_H
#define TS_CODES_H


//-----------------------------//
// Test custom command function definitions //
//-----------------------------//
enum TMMFTestCustomCommands 
	{
	EMMFTestCustomCommandSetFrameRate = 1,
	EMMFTestCustomCommandGetFrameRate,
	EMMFTestCustomCommandGeneralSynchronous,
	EMMFTestCustomCommandGeneralSynchronousWithReturn,
	EMMFTestCustomCommandGeneralAsynchronous,
	EMMFTestCustomCommandGeneralAsynchronousWithReturn,
	EMMFTestCustomCommandRequestGeneralEvent,
	EMMFTestCustomCommandSimulateReloading,
	EMMFTestCustomCommandCheckUrl,
	EMMFTestCustomCommandSimulateSurfaceCreated,
	EMMFTestCustomCommandSimulateParaChanged,
	EMMFTestCustomCommandSimulateRemoveSurface,
	EMMFTestCustomCommandSetSubtitleDisplayCheck,
	EMMFTestCustomCommandGetSubtitleCallCount,
	EMMFTestCustomCommandSimulateCrpReadyEvent,
	EMMFTestCustomCommandSetLanguageNotAvail,
	};

_LIT8(KFirstParameter,"first");
_LIT8(KSecondParameter,"second");
_LIT8(KReturnParameter,"third");

//URL Test
_LIT(KTestURL, "http://symbian.com/clock.dummy");
const TInt KTestIap = 5;

// These are Codes which are passed to and from functions via the Controller Framework.

const TInt KGetAudioQuality = 1;
const TInt KSetAudionQuality = 2;
const TInt KDummyFunc1 = 3;
const TInt KDummyFunc1Return = -4;
const TInt KDummyFunc2 = 5;
const TInt KDummyFunc2Return = -6;
const TInt KLogFunction = 7;
const TInt KFuncComplete = 0;
const TInt KModeFunction = 8;
const TInt KLogIntFunction = 9;
const TInt KLogInt2Function = 10;
const TInt KLogRealFunction = 11;
const TInt KLogTimeInterMicroFunction = 12;
const TInt KLogTFourCCFunction = 13;
const TInt KLogFunction2 = 14;
const TInt KTestSetError = 15;
const TInt KTestCustomCommandSuccess = -4444;
const TInt KTestCustomCommandFailure = -5555;
const TInt KTestGeneralError = -7777;

const TInt KPlayStartTime = 1000000;
const TInt KPlayEndTime = 2000000;
const TInt KTestFrameRate = 25;

//-----------------------------//
// Enums for EMMFTestCustomCommandGetSubtitleCallCount //
//-----------------------------//
enum TMMFTestGetSubtitleCountType
	{
	EMMFTestEnableSubtitleCallCount = 1,
	EMMFTestDisableSubtitleCallCount,
	EMMFTestAddSubtitleConfigCallCount,
	EMMFTestRemoveSubtitleConfigCallCount,
	EMMFTestGetCrpParamCallCount,
	EMMFTestCrpReadyEventCount,
	EMMFTestUpdateSubtitleConfigCallCount
	};

//-----------------------------//
// Leave Codes (Failure Tests) //
//-----------------------------//

const TInt KOpenFailure = 201;
const TInt KAddDataSourceFailure = 202;
const TInt KAddDataSinkFailure = 203;
const TInt KRemoveDataSourceFailure = 204;
const TInt KRemoveDataSinkFailure = 205;
const TInt KPrimeFailure = 206;
const TInt KFailureCode1 = 210;
const TInt KFailureCode2 = 211;


#endif
