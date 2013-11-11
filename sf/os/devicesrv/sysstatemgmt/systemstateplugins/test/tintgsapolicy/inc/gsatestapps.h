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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __GSATESTAPPS_H
#define __GSATESTAPPS_H

#include <e32cmn.h>

_LIT(KProcSemaphore, "ssmstatetransemaphore");
const TInt KTestAppFailure = 12345; // Just a random number used to indicate that one of following 4 test apps has failed
_LIT(KTestProcTranFromShutdown, "tgsastatetranfromshutdown.exe");
_LIT(KTestProcTranToShutdown, "tgsastatetrantoshutdown.exe");
_LIT(KTestProcTranFromNormal, "tgsastatetranfromnormal.exe");
_LIT(KTestProcTranFromStartup, "tgsastatetranfromstartup.exe");

const TInt KTestProcTranFromShutdownUid = 0xA0009DF3;
const TInt KTestProcTranToShutdownUid = 0xA0009DF4;
const TInt KTestProcTranFromNormalUid = 0xA0009DF5;
const TInt KTestProcTranFromStartupUid = 0xA000A180;

_LIT(KGsaTestStateTranResultFilePath, "c:\\sspluginstest\\gsatest\\");
_LIT(KGsaTestStateTranFromShutdownResult, "c:\\sspluginstest\\gsatest\\statetranfromshutdownresult.txt");
_LIT(KGsaTestStateTranToShutdownResult, "c:\\sspluginstest\\gsatest\\statetrantoshutdownresult.txt");
_LIT(KGsaTestStateTranFromNormalResult, "c:\\sspluginstest\\gsatest\\statetranfromnormalresult.txt");
_LIT(KGsaTestStateTranFromStartupResult, "c:\\sspluginstest\\gsatest\\statetranfromstartupresult.txt");

#endif // __SSMTESTAPPS_H
