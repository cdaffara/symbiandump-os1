// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <f32file.h>
#include <startupproperties.h>

#include "appstarter2.h"
#include <startup.hrh>
#include "startuputilprovider.h"

#include "SysStartDebug.h"
#include "sysstartpanic.h"


//
// Standard Symbian factory functions
//

CAppStarter* CAppStarter::NewL(CStartupProperties* aAppInfo, CStartupUtilProvider& aProvider)
	{
	CAppStarter* self = CAppStarter::NewLC(aAppInfo, aProvider);
	CleanupStack::Pop(self);
	return self;
	}

CAppStarter* CAppStarter::NewLC(CStartupProperties* aAppInfo, CStartupUtilProvider& aProvider)
	{
	__ASSERT_ALWAYS(aAppInfo != NULL, PanicNow(KPanicAppStart, ENULLPropertiesPassed));

	CAppStarter* self = new (ELeave) CAppStarter(aAppInfo, aProvider);
	CleanupStack::PushL(self);
	return self;
	}
	
CAppStarter::~CAppStarter()
	{
	delete iAppInfo;
	iProcess.Close();
	}
	
CAppStarter::CAppStarter(CStartupProperties* aAppInfo, CStartupUtilProvider& aProvider) 
	:iProvider(aProvider), iAppInfo(aAppInfo) 
	{
	}
	 	
//
// Public member functions
//

/** Implementation of MStartupCommand interface.

@see MStartupCommand.
*/
void CAppStarter::Execute(TRequestStatus& aCallerStatus)
	{
	aCallerStatus = KRequestPending;

	TPtrC appFileName = iAppInfo->FileName();
	TPtrC args = iAppInfo->Args();
	TParsePtrC parser(iAppInfo->FileName());

	DEBUGPRINT3(_L("SysStart: starting %S %S"), &appFileName, &args);

	TInt err = KErrNone;		

	TInt tried=0;
		
	if (iAppInfo->StartMethod() == EDeferredWaitForStart)
		{
		// Start process asynchronously, iCommandStatus will be checked at MultipleWait
		// and monitoring will commence afterwards if required
		TRAP(err, iProvider.StartSafe().StartL(*iAppInfo, iProcess, iCommandStatus));
		DEBUGPRINT2(_L("SysStart: Attempted %S with StartMethod()==EDeferredWaitForStart"), &appFileName);
		}		
	else
		{
		// Start process synchronously for EWaitForStart and EFireAndForget case
		TRAP(err, iProvider.StartSafe().StartL(*iAppInfo, iProcess, tried));
		DEBUGPRINT4(_L("SysStart: Attempted %S %d times, result: %d"), &appFileName, (tried + 1), err);
		
		// Start monitor if required
		if (KErrNone == err && iAppInfo->Monitored())
			{
			const TBool KEvenIfAlreadyDead = ETrue;
			TRAP(err, iProvider.SysMonSessionL().MonitorL(*iAppInfo, iProcess, KEvenIfAlreadyDead));			
			DEBUGPRINT3(_L("SysStart: Attempted to monitor %S, result: %d "), &appFileName, err);
			}
		}		
 	
	TRequestStatus* callerStatus = &aCallerStatus;
	User::RequestComplete(callerStatus, err);
	}

CStartupProperties& CAppStarter::AppInfo()
	{
	return *iAppInfo;
	}

RProcess& CAppStarter::Process()
	{
	return iProcess;
	}

TRequestStatus& CAppStarter::CommandStatus()
	{
	return iCommandStatus;
	}

void CAppStarter::Release() 
	{
	delete this;
	}
