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
// Observer application used by T_SUPL_async.cpp to check functionality of the
// LBS Host Settings Store observer with multiple applications.
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#include <e32base.h>
#include <e32debug.h>
#include <lbs/lbshostsettings.h>
#include <centralrepository.h>
#include <lbs/lbshostsettingsclasstypes.h>

class CTestAppObserver : public CBase, public MLbsHostSettingsStoreObserver
	{
public:
	IMPORT_C static CTestAppObserver* NewL();
	IMPORT_C virtual void LbsHostSettingsUpdated(TInt aError, TLbsHostStoreId aStoreId, TLbsHostSettingsId aSettingsId) const;
	~CTestAppObserver();
	
private:
	CTestAppObserver();
	};

LOCAL_C void MainL()
	{
	// Create active scheduler
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CleanupStack::PushL(scheduler); 
	CActiveScheduler::Install(scheduler);

	// Create observer
	CTestAppObserver* obs = CTestAppObserver::NewL();
	CleanupStack::PushL(obs);

	// Create store and start notifier
	CLbsHostSettingsStore* store = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId, *obs);
	CleanupStack::PushL(store);

	// Start scheduler
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(store);	
	CleanupStack::PopAndDestroy(obs);	
	CleanupStack::PopAndDestroy(scheduler);	
	}

EXPORT_C CTestAppObserver* CTestAppObserver::NewL()
	{
	CTestAppObserver* self = new(ELeave) CTestAppObserver();
	return self;
	}

EXPORT_C void CTestAppObserver::LbsHostSettingsUpdated(TInt aError, TLbsHostStoreId aStoreId, TLbsHostSettingsId aSettingsId) const
	{
	TInt reason;

	// Only one integer value may be returned to the tester via the
	// rendezvous, so use it to pass the settings ID unless the error code
	// or store ID have unexpected values.
	if (aError != KErrNone)
		{
		reason = aError;
		}
	else if (aStoreId != KLbsHostSettingsSuplStoreId)
		{
		reason = KErrArgument;
		}
	else
		{
		reason = aSettingsId.iUid;
		}

	RProcess::Rendezvous(reason);
	}

CTestAppObserver::CTestAppObserver()
	{
	}

CTestAppObserver::~CTestAppObserver()
	{
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;

	CTrapCleanup* cleanupStack = CTrapCleanup::New(); 	
	TRAPD(ret, MainL());	
	delete cleanupStack;
	
	__UHEAP_MARKEND;
	return(ret);
	}
