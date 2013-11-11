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

#ifndef __GTESTAPPS_H
#define __GTESTAPPS_H

#include <e32cmn.h>

_LIT(KTestAppGood, "gsatestappgood.exe");
_LIT(KTestProcGood, "gsatestprocgood.exe");


const TInt KTestAppFailure = 12345; // Just a random number used to indicate that one of following 4 test apps has failed


_LIT(KTestAppLogFileLoc, "C:\\logs\\gsatest\\");

const TInt KTestCommandLineMaxLength = 200;

const TInt KTestAppGoodUid = 0x2000F5A8;
const TInt KTestProcGoodUid = 0xA000B6B8;

const TInt KErrBadRendezvousTest = -111;

/** If KTestAppGood is launched as process, then it has to be running until the monitoring is setup*/
_LIT(KLaunchProcessAndWait, "-launchprocessandwait");

//Semaphore for signalling the start of the process
_LIT(KStartTestProcGoodSem, "starttestprocgood");

#endif // __GTESTAPPS_H
