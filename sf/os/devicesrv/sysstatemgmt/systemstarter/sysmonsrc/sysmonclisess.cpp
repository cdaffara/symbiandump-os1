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

#include <startupproperties.h>

#include "sysmonclisess.h"
#include "sysmoncliserv.h"
#include "../src/SysStartDebug.h"

/**
Default constructor.
Creates an instance of the RSysMonSession class
@publishedAll
@released
*/
EXPORT_C RSysMonSession::RSysMonSession() : RSessionBase()
	{
	}

/**
Opens connection with SysMonServer
@return returns KErrNone or an error code.
@publishedAll
@released
*/
EXPORT_C void RSysMonSession::OpenL()
	{
	DEBUGPRINT1(_L("SysMonCli: Opening session"));
	User::LeaveIfError(CreateSession(KSysMonServerName, TVersion(0,0,0), 1));
	}

/**
Closes connection with SysMonServer
@publishedAll
@released
*/
EXPORT_C void RSysMonSession::Close()
	{
	DEBUGPRINT1(_L("SysMonCli: Closing session"));
	RSessionBase::Close();
	}

/**
Used to initiate the monitoring of a started component. 
Example use:
@code
	CStartupProperties* prop = CStartupProperties::NewLC(KFilename, KNullDesC);
	prop->SetMonitored(ETrue);
	prop->SetStartupType(EStartApp);
	prop->SetStartMethod(EWaitForStart);
	prop->SetNoOfRetries(1);
	prop->SetTimeout(1000);
	prop->SetRecoveryParams(EIgnoreOnFailure, 0);
	iMySysMonSession.MonitorL(*prop, process);
	CleanupStack::PopAndDestroy(prop);
@endcode

@param aStartupProperties Properties for the component to be monitored.
		Number of retries and timeout in @c aStartupProperties should only be used with 
		start-method @c EWaitForStart. A monitored component with start-method @c EFireAndForget 
		will not be restarted on failure, the recovery method will be executed immediately. 
		Start-method @c EDeferredWaitForStart can not be used for monitoring.
@param aProcess The running instance of the component to be monitored.
@param aExecuteRecoveryMethodOnFailure Gives the option to initiate monitoring on an already 
		dead process. If the process is already dead, the monitor will be scheduled to recover 
		the process. The default behaviour is to leave and not allow monitoring to be setup if 
		the process is already dead.
@capability ECapabilityProtServ
@capability ECapabilityPowerMgmt for ERestartOSWithMode
@publishedAll
@deprecated
@leave KErrDied if @c aProcess is no longer running and @c aExecuteRecoveryMethodOnFailure is false
@leave KErrArgument if @c aStartupProperties contains invalid argument or if @c aProcess contain NULL handle
@leave KErrPermissionDenied if client does not have the requied PlatSec capability
@leave KErrAlreadyExists if the process is already registered for monitoring
*/
EXPORT_C void RSysMonSession::MonitorL(const CStartupProperties& aStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure)
	{
	DEBUGPRINT1(_L("SysMonCli: Monitor process"));
	DoMonitorL(aStartupProperties, &aProcess, aExecuteRecoveryMethodOnFailure);
	}

/**
Used by a started component to initiate self-monitoring.

@param aStartupProperties Properties for itself.
		Number of retries and timeout in @c aStartupProperties should only be used with 
		start-method @c EWaitForStart. A monitored component with start-method @c EFireAndForget 
		will not be restarted on failure, the recovery method will be executed immediately. 
		Start-method @c EDeferredWaitForStart can not be used for monitoring.		
@capability ECapabilityProtServ for ERestartOS and ERestartOSWithMode
@capability ECapabilityPowerMgmt for ERestartOSWithMode
@publishedAll
@deprecated
@leave KErrArgument if @c aStartupProperties contains invalid argument or if @c aProcess contain NULL handle
@leave KErrPermissionDenied if client does not have the requied PlatSec capability
@leave KErrAlreadyExists if the process is already registered for monitoring
*/
EXPORT_C void RSysMonSession::MonitorSelfL(const CStartupProperties& aStartupProperties)
	{
	DEBUGPRINT1(_L("SysMonCli: Monitor self"));
	DoMonitorL(aStartupProperties, NULL, EFalse);
	}


void RSysMonSession::DoMonitorL(const CStartupProperties& aStartupProperties, const RProcess* aProcess, TBool aExecuteRecoveryMethodOnFailure)
	{
	User::LeaveIfError(Validate(aStartupProperties));
	
	CBufFlat* const propsBuf = CBufFlat::NewL(aStartupProperties.Size());
    CleanupStack::PushL(propsBuf);
      
    aStartupProperties.ExternalizeL(*propsBuf);
    TPtr8 propsPtr(propsBuf->Ptr(0));

#ifdef _DEBUG
	TPtrC fileName = aStartupProperties.FileName();
	DEBUGPRINT2(_L("SysMonCli: Initiating monitor for %S"), &fileName);
#endif

	if (aProcess)
		{
		if (aProcess->Handle() == KNullHandle)
			{
			User::Leave(KErrArgument);
			}
		
		TPckg<TProcessId> pidPckg(aProcess->Id());
		TIpcArgs ipcArgs(&propsPtr, &pidPckg, aExecuteRecoveryMethodOnFailure);
		User::LeaveIfError(SendReceive(EMonitor, ipcArgs));
		}
	else
		{
		TIpcArgs ipcArgs(&propsPtr);
		User::LeaveIfError(SendReceive(EMonitorSelf, ipcArgs));
		}

	CleanupStack::PopAndDestroy(propsBuf);
	}

/**
Used to cancel self-monitoring of a component.

This function is to be called by the component being monitored either by calling 
MonitorL() or MonitorSelfL() function. By calling this function before exiting, 
System Monitor will cancel the monitor request of the component so that System Monitor 
will not restart it after it exit.
@publishedAll
@released
*/
EXPORT_C void RSysMonSession::CancelMonitorSelfL()
	{
	DEBUGPRINT1(_L("SysMonCli: Cancelling monitor"));
	User::LeaveIfError(SendReceive(ECancelSelf));
	}

/** Validate CStartupProperties parameter
*/
TInt RSysMonSession::Validate(const CStartupProperties& aProp)
	{
	if(aProp.FileName().Length() > 0)
		{
		if(aProp.Monitored())
			{
			if(aProp.StartupType() == EStartProcess || aProp.StartupType() == EStartApp)
				{
				if(((aProp.StartMethod() == EFireAndForget) && (aProp.NoOfRetries() == 0)) ||
					(aProp.StartMethod() == EWaitForStart))
					{
					if(aProp.NoOfRetries() >= 0)
						{
						if(aProp.Timeout() >= 0)
							{
							if(aProp.RecoveryMethod() >= EIgnoreOnFailure && aProp.RecoveryMethod() <= ERestartOSWithMode)
								{
								return KErrNone;
								}
							}
						}
					}
				}
			}
		}
	return KErrArgument;
	}
