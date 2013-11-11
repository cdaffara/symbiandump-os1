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
 
#ifndef __APPFWK_SYSSTART2_TESTSTEP_H__
#define __APPFWK_SYSSTART2_TESTSTEP_H__

#include <e32std.h>
#include <test/testexecutestepbase.h>
#ifndef SYMBIAN_SYSTEM_STATE_MANAGEMENT
#include "../../../startsafesrc/startsafe.h"
#else
#include <startsafe.h>
#endif

// Default throttle time of SysMon
const TInt KThrottleTime = 15000000; // 15 seconds
const TInt KSysStart2TestKillCode = 666;


class CAppFwkSysStart2TestStep : public CTestStep
	{
public:
	TVerdict doTestStepPostambleL();
	TBool ProcessExists( const TDesC& aProcessName, TFullName& aName );
	void LogonAndCancelMonL();
	};

#endif
