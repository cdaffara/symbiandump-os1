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

#ifndef __LOADSYSMON_H__
#define __LOADSYSMON_H__

#include <e32std.h>
#include <ssm/ssmloadsysmon.h>

class CStartupProperties;
class CSsmStartupProperties;

/**Client API for monitoring the applications and processes using RSysMonSession.

@see RSysMonSession

@internalTechnology
@released
*/
NONSHARABLE_CLASS(RLoadSysMonSession) : public MSsmLoadSysMon
	{
public:	
	IMPORT_C static RLoadSysMonSession* CreateL();

	//from MSsmLoadSysMon class
	void OpenL();
	void Close();
	void MonitorL(const CStartupProperties& aStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure=EFalse); //lint !e1735 Virtual function has default parameter - Must use the same default as declared by MSsmLoadSysMon
	void MonitorSelfL(const CStartupProperties& aStartupProperties);
	void CancelMonitorSelfL();
	void MonitorL(const CSsmStartupProperties& aSsmStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure=EFalse); //lint !e1735 Virtual function has default parameter - Must use the same default as declared by MSsmLoadSysMon
	void MonitorSelfL(const CSsmStartupProperties& aSsmStartupProperties);

private:
	RLoadSysMonSession();

private:
	RSysMonSession iSysMonSession;
	};

#endif

