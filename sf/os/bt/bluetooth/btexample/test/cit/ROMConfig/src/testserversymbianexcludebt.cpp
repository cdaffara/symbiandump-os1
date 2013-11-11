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

#include "testserversymbianexcludebt.h"
#include <e32std.h>
#include <rsshared.h>

// Put all of the test step header files here...
#include "cteststepbtromconfigrsocketserv001.h"
#include "cteststepbtromconfigrsocketserv002.h"
#include "cteststepbtromconfigrsocketserv003.h"
#include "cteststepbtromconfigrsocket001.h"
#include "cteststepbtromconfigrhostresolver001.h"
#include "cteststepbtromconfigrcommserv001.h"
#include "cteststepbtromconfiguser001.h"
#include "cteststepbtromconfiguser002.h"
#include "cteststepbtromconfiguser003.h"
#include "cteststepbtromconfiguser004.h"
#include "cteststepbtromconfigmanager001.h"
#include "cteststepbtromconfigmanager002.h"
#include "cteststepbtromconfigsdp001.h"
#include "cteststepbtromconfigSdpAgent001.h"
#include "cteststepbtromconfigremcon001.h"
#include "cteststepbtromconfigremcon002.h"
#include "cteststepbtromconfigpan001.h"
#include "cteststepbtromconfigrbtphylnkAdapt001.h"

_LIT(KServerName,"TestServerSymbianExcludeBt");

TInt LoadDrivers()
	{
#ifdef __WINS__
	#define KPDDName _L("ECDRV")
	#define KLDDName _L("ECOMM")
#else
	#define KPDDName _L("EUART1")
	#define KLDDName _L("ECOMM")
#endif
	TInt rerr = KErrNone;

	rerr = StartC32();
	if (rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}

	rerr = User::LoadPhysicalDevice(KPDDName);
	if (rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}

	rerr = User::LoadLogicalDevice(KLDDName);
	if (rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}
	return KErrNone;
	}

/**
Called inside the MainL() function to create and start the test
@return Instance of the test server
*/
CTestServerSymbianExcludeBt* CTestServerSymbianExcludeBt::NewL()
	{
	CTestServerSymbianExcludeBt* server(new (ELeave)CTestServerSymbianExcludeBt);
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	CActiveScheduler* sched(new (ELeave) CActiveScheduler);
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	// this registers the server with the active scheduler and calls SetActive
	CTestServerSymbianExcludeBt* server(CTestServerSymbianExcludeBt::NewL());

	// signal to the client that we are ready by
	// rendevousing process
	RProcess::Rendezvous(KErrNone);

	// run the active scheduler
	sched->Start();

	// clean up
	delete server;
	CleanupStack::PopAndDestroy(sched);
	}

/**
@return Standard Epoc error code on exit
*/
GLDEF_C TInt E32Main()
	{
	const TInt rerr(LoadDrivers());
	if (rerr != KErrNone)
		{
		return rerr;
		}

	__UHEAP_MARK;
	CTrapCleanup* cleanup(CTrapCleanup::New());

	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err, MainL());

	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}

/**
Implementation of CTestServer pure virtual
@return A CTestStep derived instance
*/
CTestStep* CTestServerSymbianExcludeBt::CreateTestStep(const TDesC& aStepName)
	{
	if ( aStepName == KTestNameRSocketServ001 )
		{
		return CTestStepBtRomConfigRSocketServ001::New(*this);
		}
	if ( aStepName == KTestNameRSocketServ002 )
		{
		return CTestStepBtRomConfigRSocketServ002::New(*this);
		}
	if ( aStepName == KTestNameRSocketServ003 )
		{
		return CTestStepBtRomConfigRSocketServ003::New(*this);
		}
	if ( aStepName == KTestNameRSocket001 )
		{
		return CTestStepBtRomConfigRSocket001::New(*this);
		}
	if ( aStepName == KTestNameRHostResolver001 )
		{
		return CTestStepBtRomConfigRHostResolver001::New(*this);
		}
	if ( aStepName == KTestNameRCommServ001 )
		{
		return CTestStepBtRomConfigRCommServ001::New(*this);
		}									
	if ( aStepName == KTestNameUser001 )
		{
		return CTestStepBtRomConfigUser001::New(*this);
		}
	if ( aStepName == KTestNameUser002 )
		{
		return CTestStepBtRomConfigUser002::New(*this);
		}
	if ( aStepName == KTestNameUser003 )
		{
		return CTestStepBtRomConfigUser003::New(*this);
		}
	if ( aStepName == KTestNameUser004 )
		{
		return CTestStepBtRomConfigUser004::New(*this);
		}
	if ( aStepName == KTestNameManager001 )
		{
		return CTestStepBtRomConfigManager001::New(*this);
		}
	if ( aStepName == KTestNameManager002 )
		{
		return CTestStepBtRomConfigManager002::New(*this);
		}
	if ( aStepName == KTestNameSdp001 )
		{
		return CTestStepBtRomConfigSdp001::New(*this);
		}
	if ( aStepName == KTestNameSdpAgent001 )
		{
		return CTestStepBtRomConfigSdpAgent001::New(*this);
		}
	if ( aStepName == KTestNameRemCon001 )
		{
		return CTestStepBtRomConfigRemCon001::New(*this);
		}
	if ( aStepName == KTestNameRemCon002 )
		{
		return CTestStepBtRomConfigRemCon002::New(*this);
		}
	if ( aStepName == KTestNamePan001 )
		{
		return CTestStepBtRomConfigPan001::New(*this);
		}
	if ( aStepName == KTestNameRBtPhyLnkAdapt001 )
		{
		return CTestStepBtRomConfigRBtPhyLnkAdapt001::New(*this);
		}
	return NULL;
	}

// EOF
