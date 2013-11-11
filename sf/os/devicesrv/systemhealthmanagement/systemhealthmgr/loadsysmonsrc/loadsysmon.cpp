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

#include "sysmonclisess.h"
#include "loadsysmon.h"

/**
Default constructor.
Creates an instance of the RLoadSysMonSession class
@internalTechnology
@released
*/
RLoadSysMonSession::RLoadSysMonSession()
	{
	}

/**
Used to create an instance of RLoadSysMonSession class

@return An instance of RLoadSysMonSession
@internalTechnology
@released
*/
EXPORT_C RLoadSysMonSession* RLoadSysMonSession::CreateL()
	{
	RLoadSysMonSession* self = new(ELeave) RLoadSysMonSession();
	return self;
	}


/**
Opens connection with SysMonServer

@leave KErrNone if successful, otherwise one of the other system-wide error codes.
@internalTechnology
@released
@see RSysMonSession
*/
void RLoadSysMonSession::OpenL()
	{
	iSysMonSession.OpenL();
	}

/**
Closes connection with SysMonServer
@internalTechnology
@released
@see RSysMonSession
*/
void RLoadSysMonSession::Close()
	{
	iSysMonSession.Close();
	}

/**
Used to initiate the monitoring of a started component using CStartupProperties.
@internalTechnology
@released
@see RSysMonSession
*/
void RLoadSysMonSession::MonitorL(const CStartupProperties& aStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure)
	{
	iSysMonSession.MonitorL(aStartupProperties, aProcess, aExecuteRecoveryMethodOnFailure);
	}

/**
Used by a started component to initiate self-monitoring using CStartupProperties.
@internalTechnology
@released
@see RSysMonSession
*/
void RLoadSysMonSession::MonitorSelfL(const CStartupProperties& aStartupProperties)
	{
	iSysMonSession.MonitorSelfL(aStartupProperties);
	}

/**
Used to initiate the monitoring of a started component using CSsmStartupProperties.
@internalTechnology
@released
@see RSysMonSession
*/
void RLoadSysMonSession::MonitorL(const CSsmStartupProperties& aSsmStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure)
	{
	iSysMonSession.MonitorL(aSsmStartupProperties, aProcess, aExecuteRecoveryMethodOnFailure);
	}

/**
Used by a started component to initiate self-monitoring using CSsmStartupProperties.
@internalTechnology
@released
@see RSysMonSession
*/
void RLoadSysMonSession::MonitorSelfL(const CSsmStartupProperties& aSsmStartupProperties)
	{
	iSysMonSession.MonitorSelfL(aSsmStartupProperties);
	}

/**
Used to cancel self-monitoring of a component.

@internalTechnology
@released
@see RSysMonSession
*/
void RLoadSysMonSession::CancelMonitorSelfL()
	{
	iSysMonSession.CancelMonitorSelfL();
	}

