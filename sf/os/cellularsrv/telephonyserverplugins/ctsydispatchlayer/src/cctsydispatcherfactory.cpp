// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cctsydispatcherfactory.h"
#include "cmessagerouter.h"

#include <ctsy/serviceapi/ctsysatmessagingbase.h>

#include <ctsy/ltsy/ltsylogger.h>
#include "testsattsyfactory.h"

#include <ctsy/ltsy/mltsydispatchfactory.h>


CCtsyDispatcherFactoryV1* CCtsyDispatcherFactoryV1::NewL()
	{
	TSYLOGENTRYEXIT;
	CCtsyDispatcherFactoryV1* self = new (ELeave) CCtsyDispatcherFactoryV1;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CCtsyDispatcherFactoryV1::CCtsyDispatcherFactoryV1()
	{
	}
	
void CCtsyDispatcherFactoryV1::ConstructL()
	{
	TSYLOGENTRYEXIT;
	}

CCtsyDispatcherFactoryV1::~CCtsyDispatcherFactoryV1()
	{
	iloadedLib.Close();
	delete iMessageRouter;
	}

MLtsyFactoryBase::TCtsyInterfaceVersion CCtsyDispatcherFactoryV1::Version()
    {
    TSYLOGENTRYEXIT;
    return MLtsyFactoryBase::EIfVersion1;
    }
	
void CCtsyDispatcherFactoryV1::Release()
	{
	TSYLOGENTRYEXIT;
	delete this;
	}

MMessageRouter* CCtsyDispatcherFactoryV1::GetMessageRouter(MmMessageManagerCallback& aMessageManager)
	{
	TSYLOGENTRYEXIT;
	if (!iMessageRouter)
		{
		TInt err = KErrNone;
		TRAP(err, iMessageRouter = CMessageRouter::NewL(aMessageManager));
		}
	return iMessageRouter;
	}
	
CTsySatMessagingBase* CCtsyDispatcherFactoryV1::GetSatMessagingService(MCtsySatService& aSatMessagingService)
	{
	TSYLOGENTRYEXIT;
	//create message router to pass to STK CTSY
	CTsySatMessagingBase* satbase = NULL;
	TRAP_IGNORE(satbase = GetSatMessagingServiceL(aSatMessagingService));
	return satbase;
	}

CTsySatMessagingBase* CCtsyDispatcherFactoryV1::GetSatMessagingServiceL(MCtsySatService& aSatMessagingService)
	{
		
	// When used by a licensee the TEST_CODE macro should be undefined and
	// the licensee must provide an implementation for the GetSatMessagingService
	// method in their dispatcher factory implementation
	
#define TEST_CODE
#ifndef TEST_CODE
	
	__ASSERT_DEBUG(iMessageRouter, CtsyDispatcherPanic(EInvalidNullPtr));
	return iMessageRouter->iLtsyFactoryV1->GetSatMessagingService(aSatMessagingService);
	
#else
	
	// Our test SAT TSY component is loaded only when the dispatcher is used in our 
	// test environment.
	_LIT(KSatDllName, "cstktsy.dll");
	User::LeaveIfError(iloadedLib.Load(KSatDllName));
	typedef CSatTsyFactory* (*CSatTsyNewLC)();
	CSatTsyNewLC libEntry = (CSatTsyNewLC)iloadedLib.Lookup(1);
	
	CSatTsyFactory* satfactory = (*libEntry)();	// libEntry may leave.
	CSatTsyFactory::TSatTsyPtrContainer satptr = satfactory->CreateSatTsyL(*iMessageRouter,aSatMessagingService);
	
	CleanupStack::PopAndDestroy(satfactory);
	iMessageRouter->SetSatMessageManager(*satptr.iSatTsyMessageManagerCallback);
	MStkTsyCallOrigin* origin = satptr.iSatTsy;
	iMessageRouter->SetSatCallOriginInterface(*origin);
	return satptr.iSatTsy;	
#endif		
	}

EXPORT_C MLtsyFactoryBase* CtsyDispatcherFactoryL()
	{
	TSYLOGENTRYEXIT;
	CCtsyDispatcherFactoryV1* factory = CCtsyDispatcherFactoryV1::NewL();
    return factory;
	}
