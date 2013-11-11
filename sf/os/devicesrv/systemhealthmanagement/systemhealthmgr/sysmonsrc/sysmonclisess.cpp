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
#include <ssm/ssmstartupproperties.h>

#include "sysmonclisess.h"
#include "sysmoncliserv.h"
#include "shmadebug.h"

/**
Default constructor.
Creates an instance of the RSysMonSession class
@publishedPartner
@released
*/
EXPORT_C RSysMonSession::RSysMonSession() : RSessionBase()
	{
	}

/**
Opens connection with SysMonServer
@return returns KErrNone or an error code.
@publishedPartner
@released
*/
EXPORT_C void RSysMonSession::OpenL()
	{
	DEBUGPRINT1(_L("SysMonCli: Opening session"));
	User::LeaveIfError(CreateSession(KSysMonServerName, TVersion(0,0,0), 1));
	}

/**
Closes connection with SysMonServer
@publishedPartner
@released
*/
EXPORT_C void RSysMonSession::Close()
	{
	DEBUGPRINT1(_L("SysMonCli: Closing session"));
	RSessionBase::Close();
	}

/**
Used to initiate the monitoring of a started component using CStartupProperties.
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
		start-method @c EWaitForStart or @c EFireAndForget. 
		Start-method @c EDeferredWaitForStart can not be used for monitoring.
@param aProcess The running instance of the component to be monitored.
@param aExecuteRecoveryMethodOnFailure Gives the option to initiate monitoring on an already 
		dead process. If the process is already dead, the monitor will be scheduled to recover 
		the process. The default behaviour is to leave and not allow monitoring to be setup if 
		the process is already dead.
@capability ECapabilityProtServ
@capability ECapabilityPowerMgmt for ERestartOSWithMode
@publishedPartner
@deprecated Instead of this use RSysMonSession::MonitorL(const CSsmStartupProperties& aSsmStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure)
@leave KErrDied if @c aProcess is no longer running and @c aExecuteRecoveryMethodOnFailure is false
@leave KErrArgument if @c aStartupProperties contains invalid argument or if @c aProcess contain NULL handle
@leave KErrPermissionDenied if client does not have the requied PlatSec capability
@leave KErrAlreadyExists if the process is already registered for monitoring
*/
EXPORT_C void RSysMonSession::MonitorL(const CStartupProperties& aStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure)
	{
	DEBUGPRINT1(_L("SysMonCli: Monitor process using StartupProperties"));
	DoMonitorL(aStartupProperties, &aProcess, aExecuteRecoveryMethodOnFailure);
	}

/**
Used by a started component to initiate self-monitoring using CStartupProperties.

@param aStartupProperties Properties for itself.
		Number of retries and timeout in @c aStartupProperties should only be used with 
		start-method @c EWaitForStart or @c EFireAndForget.
		Start-method @c EDeferredWaitForStart can not be used for monitoring.		
@capability ECapabilityProtServ for ERestartOS ,ERestartOSWithMode and ECriticalNoRetries
@capability ECapabilityPowerMgmt for ERestartOSWithMode
@publishedPartner
@deprecated Instead of this use RSysMonSession::MonitorSelfL(const CSsmStartupProperties& aSsmStartupProperties)
@leave KErrArgument if @c aStartupProperties contains invalid argument or if @c aProcess contain NULL handle
@leave KErrPermissionDenied if client does not have the requied PlatSec capability
@leave KErrAlreadyExists if the process is already registered for monitoring
*/
EXPORT_C void RSysMonSession::MonitorSelfL(const CStartupProperties& aStartupProperties)
	{
	DEBUGPRINT1(_L("SysMonCli: Monitor self StartupProperties"));
	DoMonitorL(aStartupProperties, NULL, EFalse);
	}

/**
Used to initiate the monitoring of a started component using CSsmStartupProperties
Example use:
@code
	CSsmStartupProperties* prop = CSsmStartupProperties::NewLC(KFilename, KNullDesC);
	prop->SetCommandTypeL(ESsmCmdStartApp);
	prop->SetExecutionBehaviour(ESsmWaitForSignal);	
	prop->SetMonitorInfo(ESsmIgnoreOnFailure, 0, 1000, 1);
	iMySysMonSession.MonitorL(*prop, process);
	CleanupStack::PopAndDestroy(prop);
@endcode

@param aSsmStartupProperties Properties for the component to be monitored.
		Number of retries and timeout in @c aSsmStartupProperties should only be used with 
		start-method @c ESsmWaitForSignal or @c ESsmFireAndForget. 
		Start-method @c ESsmDeferredWaitForSignal can not be used for monitoring.
@param aProcess The running instance of the component to be monitored.
@param aExecuteRecoveryMethodOnFailure Gives the option to initiate monitoring on an already 
		dead process. If the process is already dead, the monitor will be scheduled to recover 
		the process. The default behaviour is to leave and not allow monitoring to be setup if 
		the process is already dead.
@capability ECapabilityProtServ
@capability ECapabilityPowerMgmt for ERestartOSWithMode
@publishedPartner
@released
@leave KErrDied if @c aProcess is no longer running and @c aExecuteRecoveryMethodOnFailure is false
@leave KErrArgument if @c aSsmStartupProperties contains invalid argument or if @c aProcess contain NULL handle
@leave KErrPermissionDenied if client does not have the requied PlatSec capability
@leave KErrAlreadyExists if the process is already registered for monitoring
*/
EXPORT_C void RSysMonSession::MonitorL(const CSsmStartupProperties& aSsmStartupProperties, const RProcess& aProcess, TBool aExecuteRecoveryMethodOnFailure)
	{
	DEBUGPRINT1(_L("SysMonCli: Monitor process using SsmStartUpProperties"));
	// Create an old style startupproperties instance using the SsmStartupProperties
	CStartupProperties* startupProperties = ChangeToOldStylePropertiesL(aSsmStartupProperties);
	CleanupStack::PushL(startupProperties);
	DoMonitorL(*startupProperties, &aProcess, aExecuteRecoveryMethodOnFailure);
	CleanupStack::PopAndDestroy(startupProperties);
	}

/**
Used by a started component to initiate self-monitoring using CSsmStartupProperties

@param aSsmStartupProperties Properties for itself.
		Number of retries and timeout in @c aSsmStartupProperties should only be used with 
		start-method @c ESsmWaitForSignal or @c ESsmFireAndForget. 
		Start-method @c ESsmDeferredWaitForSignal can not be used for monitoring.		
@capability ECapabilityProtServ for ERestartOS ,ERestartOSWithMode and ECriticalNoRetries
@capability ECapabilityPowerMgmt for ERestartOSWithMode
@publishedPartner
@released
@leave KErrArgument if @c aSsmStartupProperties contains invalid argument or if @c aProcess contain NULL handle
@leave KErrPermissionDenied if client does not have the requied PlatSec capability
@leave KErrAlreadyExists if the process is already registered for monitoring
*/
EXPORT_C void RSysMonSession::MonitorSelfL(const CSsmStartupProperties& aSsmStartupProperties)
	{
	DEBUGPRINT1(_L("SysMonCli: Monitor self using SsmStartUpProperties"));
	// Create an old style startupproperties instance using the SsmStartupProperties
	CStartupProperties* startupProperties = ChangeToOldStylePropertiesL(aSsmStartupProperties);
	CleanupStack::PushL(startupProperties);
	DoMonitorL(*startupProperties, NULL, EFalse);
	CleanupStack::PopAndDestroy(startupProperties);
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
@publishedPartner
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
	if ((aProp.FileName().Length() > 0) && (aProp.Monitored()) && (aProp.StartupType() == EStartProcess || aProp.StartupType() == EStartApp))
		{
		if((aProp.StartMethod() == EFireAndForget) || (aProp.StartMethod() == EWaitForStart))
			{
			if(((aProp.NoOfRetries() >= 0) && (aProp.Timeout() >= 0)) && 
				(aProp.RecoveryMethod() >= EIgnoreOnFailure && aProp.RecoveryMethod() <= ECriticalNoRetries))
				{
				return KErrNone;
				}
			}
		}
	return KErrArgument;
	}

/**
Used to cancel monitoring of all the components during device shutdown/fail

This function is to be called only by the ssm server. By calling this function early during device shutdown/fail,
System Monitor will cancel all the outstanding monitor requests for all the components so that it will 
not restart them after the application\process finally exit during the device shutdown.
@internalTechnology
@released
*/
EXPORT_C TInt RSysMonSession::CancelAllMonitors()
    {
    DEBUGPRINT1(_L("SysMonCli: Cancelling all the monitors"));
    return SendReceive(ECancelAllMonitors);
    }

/**
Creates old style startup properties from the given CSsmStartupProperties.
@param aSsmStartupProperties Ssm Startup properties.
@return Pointer to newly created CStartupProperties object
*/
CStartupProperties* RSysMonSession::ChangeToOldStylePropertiesL(const CSsmStartupProperties& aSsmStartupProperties) const
	{
	
	CStartupProperties* startupProperties = CStartupProperties::NewL(aSsmStartupProperties.FileName(), aSsmStartupProperties.Args());
	CleanupStack::PushL(startupProperties);
	const TSsmCommandType cmdType = aSsmStartupProperties.CommandType();
	switch (cmdType)
		{
		case ESsmCmdStartApp:
			startupProperties->SetStartupType(EStartApp);
			//also copy the app specific info
			startupProperties->SetViewless(aSsmStartupProperties.Viewless());
			startupProperties->SetStartInBackground(aSsmStartupProperties.StartInBackground());
			break;
		case ESsmCmdStartProcess:
			startupProperties->SetStartupType(EStartProcess);
			break;
		default:
			User::Leave(KErrArgument);
		}
	startupProperties->SetStartMethod(static_cast<TStartMethod>(aSsmStartupProperties.ExecutionBehaviour()));
	startupProperties->SetTimeout(aSsmStartupProperties.Timeout());
	startupProperties->SetNoOfRetries(aSsmStartupProperties.Retries());
	startupProperties->SetMonitored(aSsmStartupProperties.IsMonitoringRequired());
	if (aSsmStartupProperties.IsMonitoringRequired())
		{
		TSsmMonitorInfo monitorInfo = aSsmStartupProperties.MonitorInfo();
		startupProperties->SetNoOfRetries(monitorInfo.iRetries);
		startupProperties->SetRecoveryParams(static_cast<TRecoveryMethod>(monitorInfo.iRestartPolicy), monitorInfo.iRestartMode);
		startupProperties->SetTimeout(monitorInfo.iTimeout);
		}

	CleanupStack::Pop(startupProperties);
	return startupProperties;
	}

