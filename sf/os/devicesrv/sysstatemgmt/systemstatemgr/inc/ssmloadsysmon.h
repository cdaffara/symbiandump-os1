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

#ifndef __SSMLOADSYSMON_H__
#define __SSMLOADSYSMON_H__

#include <e32std.h>

class CStartupProperties;
class CSsmStartupProperties;

/**
Shim dll used to remove the dependency of startsafe.dll which is common symbian component 
on the sysmoncli.dll which is optional replaceable. loadsysmon.dll will load sysmoncli.dll.

@internalTechnology
@released
*/
class MSsmLoadSysMon
	{
public:
	virtual void OpenL()=0;
	virtual void Close()=0;
	virtual void MonitorL(const CStartupProperties& aStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure=EFalse)=0;
	virtual void MonitorSelfL(const CStartupProperties& aStartupProperties)=0;
	virtual void CancelMonitorSelfL()=0;
	virtual void MonitorL(const CSsmStartupProperties& aSsmStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure=EFalse)=0;
	virtual void MonitorSelfL(const CSsmStartupProperties& aSsmStartupProperties)=0;
	};

#endif

