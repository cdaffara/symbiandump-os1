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

#ifndef __SYSMONCLISESS_H__
#define __SYSMONCLISESS_H__

#include <e32std.h>

class CStartupProperties;

/**
Client API for monitoring the applications and processes using System Monitor 
Client can use this class to register for process/application monitoring. Upon failure 
of the process/application, System Monitor will try to restart the process according to 
the supplied parameters. If the process still fail to start after the retry limit is 
reached, the recovery action will be taken. The retry action can be ignore failure, 
restart the system, or restart the system with a specified startup mode, and different 
PlatSec capability is required for each option. See API description for PlatSec capabiolity 
required.

@publishedAll
@released
*/
NONSHARABLE_CLASS (RSysMonSession) : public RSessionBase
	{
public:
	IMPORT_C RSysMonSession();
	
	IMPORT_C void OpenL();
	IMPORT_C void Close();
	IMPORT_C void MonitorL(const CStartupProperties& aStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure=EFalse);
	IMPORT_C void MonitorSelfL(const CStartupProperties& aStartupProperties);
	IMPORT_C void CancelMonitorSelfL();
		
private:
	void DoMonitorL(const CStartupProperties& aStartupProperties, const RProcess* aProcess, TBool aExecuteRecoveryMethodOnFailure);
	static TInt Validate(const CStartupProperties& aStartupProperties);
	};

#endif
