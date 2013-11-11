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

#ifndef __TESTAPPS_H
#define __TESTAPPS_H

#include <e32cmn.h>

_LIT(KTestAppGood, "shmatestappgood.exe");
_LIT(KTestAppSlow, "testappslow.exe");
_LIT(KTestAppNoRv, "testappnorv.exe");
_LIT(KTestAppRvAfterRetry, "testapprvafterretry.exe");
_LIT(KTestProcGood, "shmatestprocgood.exe");
_LIT(KTestAppRvError, "testapprverror.exe");

//Semaphore for signalling the start of the process
_LIT(KStartShmaTestProcGood, "startshmatestprocgood");

//Semaphore for signalling the start of the application
_LIT(KStartShmaTestAppGood, "startshmatestappgood");

//For Publish & Subscribe conditions
const TUid KCancelMonitorProperty = {0x567};
const TUint KCancelMonitorKey = 0x1;

const TInt KTestAppGoodUid = 0x10285A5C;
const TInt KTestAppSlowUid = 0x1028350E;
const TInt KTestAppNoRvUid = 0xA0002E9B;
const TInt KTestAppRvAfterRetryUid = 0xA0002E9C;
const TInt KTestProcGoodUid = 0x10285A5B;
const TInt KTestAppRvErrorUid = 0x10283506;

#endif // __TESTAPPS_H
