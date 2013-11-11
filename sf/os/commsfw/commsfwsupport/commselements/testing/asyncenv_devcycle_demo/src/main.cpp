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

#include "testnode.h"
#include <dummyfactory.h>
#include <dummytransportimpl.h>
#include <dummyruntimectxaddressbook.h>
#include <e32test.h>
#include <ecom/ecom.h>

using namespace Messages;
using namespace Dummy;
using namespace DummyTransport;

LOCAL_D RTest test(_L("Asynchronous Environment"));

//
// Global declarations
//

class CMainGlobals : public CBase
	{
public:
	static CMainGlobals* NewL();
	~CMainGlobals();
	void ConstructL();

private:
	CDummyTransportImpl* iTransportImpl;
	CActiveScheduler* iActiveScheduler;
	};
	
CMainGlobals* CMainGlobals::NewL()
	{
	CMainGlobals* self = new (ELeave) CMainGlobals;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CMainGlobals::ConstructL()
	{
	iActiveScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler); // Install active scheduler
	CGlobals& globals = TlsGlobals::InstallGlobalsL();
	iTransportImpl = CDummyTransportImpl::NewL(*globals.VirtualCtor());
	globals.SetTransportSender(iTransportImpl);
	globals.SetTransportReceiver(iTransportImpl);
	CTestFactoryContainer::InstallL();
	}

CMainGlobals::~CMainGlobals()
	{
	CTestFactoryContainer::UnInstall();
	TlsGlobals::UnInstallGlobals();
	delete iTransportImpl;
	delete iActiveScheduler;
	}

LOCAL_D void TestL()
//
//	Test function - does asynchronous services processing
//		creates, installs an Active Scheduler
//		creates an Active Object which issues an asynchronous service request
	{
	test.Start(_L("Utils starting..."));
	CConsoleBase* console = test.Console();

	CTestNode* utils = CTestNode::NewLC();
	utils->StartTest();
	CleanupStack::PopAndDestroy(utils);

	test.End();
	}

GLDEF_C TInt E32Main()
//
//	Main function - checks for memory leaks
//		calls TestL() test function
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack = CTrapCleanup::New();

	TInt err = KErrNoMemory;
	if (cleanupStack)
		{
		CMainGlobals* globals = NULL;
		TRAP(err, globals = CMainGlobals::NewL());
		if (globals)
			{
			TRAP(err,TestL());
			if (err!=KErrNone)
				{
				test.Printf(_L("Test failed with error code: %i"), err);
				}
			delete globals;
			}
		delete cleanupStack;
		}

	test.Printf(_L("\nPress a key to exit...\n"));
 	test.Getch();
	test.Close();
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	return err;
	}

