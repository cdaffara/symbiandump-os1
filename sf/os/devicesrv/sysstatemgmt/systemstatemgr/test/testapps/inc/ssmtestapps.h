// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __SSMTESTAPPS_H
#define __SSMTESTAPPS_H

#include <e32cmn.h>

_LIT(KTestAppGood, "ssmtestappgood.exe");
_LIT(KTestAppSlow, "ssmtestappslow.exe");
_LIT(KTestAppNoRv, "ssmtestappnorv.exe");
_LIT(KTestAppRvAfterRetry, "ssmtestapprvafterretry.exe");
_LIT(KTestProcGood, "ssmtestprocgood.exe");
_LIT(KTestAppRvError, "ssmtestapprverror.exe");
_LIT(KTestAppPanic, "ssmtestapppanic.exe");
_LIT(KTestAppPanicZero, "ssmtestapppaniczero.exe");
_LIT(KTestAppForFAndF, "ssmtestappsucessfulafterretry.exe");
_LIT(KTestProcWriteTime, "ssmtestprocwritetime.exe");
_LIT(KTestAppWriteTime, "ssmtestappwritetime.exe");
_LIT(KSsmCleTestProc, "ssmcletestproc.exe");
_LIT(KSsmCleTestApplication, "ssmcletestapplication.exe");
_LIT(KSsmCleTestNotExist, "ssmcletestnotexist.exe");

_LIT(KStartProcSignalSemaphore, "startprocsignalsemaphore");
_LIT(KStartAppSignalSemaphore, "startappsignalsemaphore");

const TInt KTestAppFailure = 12345; // Just a random number used to indicate that one of following 4 test apps has failed
_LIT(KTestProcTranFromNormalWoCap, "ssmstatetranfromnormalwocap.exe");

_LIT(KTestAppLogFileLoc, "C:\\logs\\ssmtest\\");
_LIT(KFileForFAndF, "c:\\countforfnf.dat"); //This file is used by KTestAppForFAndF

const TInt KTestCommandLineMaxLength = 200;

const TInt KTestAppGoodUid = 0x20010322;
const TInt KTestAppSlowUid = 0x2000E669;
const TInt KTestAppNoRvUid = 0x20010321;
const TInt KTestAppRvAfterRetryUid = 0x20010320;
const TInt KTestProcGoodUid = 0x2000E668;
const TInt KTestAppRvErrorUid = 0x2000E66A;
const TInt KTestAppPanicUid = 0x2000D75A;
const TInt KTestAppPanicZeroUid = 0x2000E660;
const TInt KTestAppForFAndFUid = 0x20010332;
const TInt KTestProcWriteTimeUid = 0x20010323;
const TInt KSsmCleTestApplicationUid = 0x10285BAD;
const TInt KSsmCleTestProcUid = 0x10285BAE;
const TInt KTestProcTranFromNormalWoCapUid = 0xA0009DF6;

const TInt KErrBadRendezvousTest = -111;

#endif // __SSMTESTAPPS_H
