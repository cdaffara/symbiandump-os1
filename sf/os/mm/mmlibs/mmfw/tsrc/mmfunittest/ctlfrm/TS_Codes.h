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

const TInt KDescriptorSource = 9;
const TInt KDescriptorSink = 10;
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
const TInt KTestSetSubtitleAvailable = 11;
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
const TInt KFuncThreadStackSize=12;
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

//------------------------------//
// 

enum TMMFTestCustomCommands 
	{
	EMMFTestCustomCommandSetAudioQuality = 1,
	EMMFTestCustomCommandGetAudioQuality,
	EMMFTestCustomCommandGeneralSynchronous,
	EMMFTestCustomCommandGeneralSynchronousWithReturn,
	EMMFTestCustomCommandGeneralAsynchronous,
	EMMFTestCustomCommandGeneralAsynchronousWithReturn,
	EMMFTestCustomCommandRequestGeneralEvent,
	EMMFTestCustomCommandSimulateReloading,
	EMMFTestCustomCommandCheckUrl,
	EMMFTestCustomCommandCheckLongUrl,
	EMMFTestCustomCommandPanic
	};

_LIT8(KFirstParameter,"first");
_LIT8(KSecondParameter,"second");
_LIT8(KReturnParameter,"third");

//URL Test
_LIT(KTestURL, "http://www.symbian.com/test.pav");
_LIT(KTestLongURL, "http://www.symbian.com/stream/clip/urls/aaaaaaaaa50-aaaaaaa60-aaaaaaa70-aaaaaaa80-aaaaaaa90-aaaaaa99/aaaaaaa110-aaaaaa120-aaaaaa130-aaaaaa140-aaaaaa150-aaaaaa160-aaaaaa170-aaaaaa180-aaaaaa190-aaaaa199/aaaaaaa210-aaaaaa220-aaaaaa230-aaaaaa240-aaaaaa250-aaaaaa260-aaaaaa270-aaaaaa280-aaaaaa290-aaaaa299/aaaaaaa310-aaaaaa320-aaaaaa330-aaaaaa340-aaaaaa350-aaaaaa360-aaaaaa370-aaaaaa380-aaaaaa390-aaaaa399/aaaaaaa410-aaaaaa420-aaaaaa430-aaaaaa440-aaaaaa450-aaaaaa460-aaaaaa470-aaaaaa480-aaaaaa490-aaaaa499/test.pav");
const TInt KTestIap = 5;

const TInt KTestCustomCommandSuccess = -4444;
const TInt KTestCustomCommandFailure = -5555;
const TInt KTestGeneralError = -7777;

const TInt KPlayStartTime = 1000000;
const TInt KPlayEndTime = 2000000;
const TInt KTestFrameRate = 25;

const TInt KUidTestController = 0x101F88D8;

 #endif
