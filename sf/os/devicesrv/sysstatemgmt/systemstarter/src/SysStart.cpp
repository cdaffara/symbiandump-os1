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

#include "SysStart.h"

#include <e32uid.h>
#include <e32property.h>
#include <domainmanager.h>

#include "StartupState.h"
#include "StartupStateInfo.h"
#include "SysStartDebug.h"

#include "sysstartpanic.h"
#include "resourcefilereader2.h"
 	
//
// Standard Symbian factory functions/destructor
//

CSystemStarter* CSystemStarter::NewL()
	{
	CSystemStarter* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CSystemStarter* CSystemStarter::NewLC()
	{
	CSystemStarter* self = new (ELeave) CSystemStarter;
	CleanupStack::PushL(self);
	return self;
	}

CSystemStarter::~CSystemStarter()
	{
	Cancel();
	}

//
// Public member functions
//

void CSystemStarter::Start()
	{
	// We need an active request so that we can actually start the active
	// scheduler, hence this roundabout way of kicking things off...
	RequestNotify();
	}

//
// Member function overrides (CActive)
//

void CSystemStarter::RunL()
	{
	TInt bootMode = 0;
	TInt err = RProperty::Get(KUidSystemCategory, KSystemStartupModeKey, bootMode);
	DEBUGPRINT4(_L("SysStart: key=%d bootMode=%d error = %d"), KSystemStartupModeKey, bootMode, err);
	if(KErrNone != err)
		{
        PanicNow(KPanicSysStart, EPropertyRetrieveError);
        }
        
    // Domain manager has already been started by the boot starter...
    err = RDmDomainManager::AddDomainHierarchy(KDmHierarchyIdStartup);
	if(KErrNone != err && KErrAlreadyExists !=err)
		{
        PanicNow(KPanicSysStart, EAddDomainHierarchyError);
        }
    
	RDmDomainManager::WaitForInitialization();

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CResourceFileReader* resourceReader = CResourceFileReader::NewLC(bootMode, fs); // bootMode is stored for later chekcing.
	const MStartupStateInfo* info = resourceReader->GetStateInfoL();

	__UHEAP_MARK;
	while (info)
		{
		CStartupState* state = CStartupState::NewLC(*info);
		state->Start();
		CleanupStack::PopAndDestroy(state);
		delete info;
		info = resourceReader->GetStateInfoL();
		}
	__UHEAP_MARKEND;
	
	CleanupStack::PopAndDestroy(resourceReader);
	CleanupStack::PopAndDestroy(); // fs
	
	CActiveScheduler::Stop();
	}

	
TInt CSystemStarter::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

void CSystemStarter::DoCancel()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrCancel);
	}

//
// Private member functions
//

CSystemStarter::CSystemStarter() :
	CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

void CSystemStarter::RequestNotify()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}
