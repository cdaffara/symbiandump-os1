// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>
#include <e32def.h>

#include <elements/cftransport.h>
#include <elements/nm_common.h>
#include <elements/nm_node.h>
#include <elements/nm_log.h>
#include <elements/nm_address.h>
#include <elements/nm_signatures.h>
#include <elements/nm_messages_base.h>
#include <elements/nm_interfaces.h>

#include <elements/mm_node.h>
#include <elements/mm_activities.h>

#include <MMExample1states.h>

class TMyWorkerThreadRegister; 
LOCAL_D CConsoleBase* console; 

_LIT8(KLoggingTag, "MMExample1");

_LIT(KJunctionPanicCat, "Junction");

const TInt KPanicUnknownMessage = 1;

/**
   This classes records information about a worker thread, which is useful
   when nodemessages is running across multiple threads. In this example,
   nodemessages is running in a single thread.

   Most users of node messages will never need to use this as they will be
   using node messages from within serverden which does all this setup for
   you.
 */
class TMyWorkerThreadRegister : public CommsFW::MWorkerThreadRegister
	{
public:
	CommsFW::TWorkerId SelfWorkerId() const
		{ return 1; }
	CommsFW::TWorkerId UpperBoundWorkerId() const
		{ return 1; }

	RAllocator& WorkerHeap(CommsFW::TWorkerId /*aWorkerId*/) const
		{
		return User::Allocator();
		}

	void PanicWorker(CommsFW::TWorkerId /*aWorkerId*/, const TDesC& aCategory, TInt aReason) const
		{
		User::Panic(aCategory, aReason);
		}
	};

_LIT(KNorth, "North");
_LIT(KSouth, "South");
_LIT(KEast, "East");
_LIT(KWest, "West");

LOCAL_C void MainL ()
	{
	/**
	   Setup the environment for passing messages.
	*/
	TMyWorkerThreadRegister workerregister;
	Messages::CGlobals* globals = &Messages::TlsGlobals::InstallGlobalsL();

	CommsFW::CCommsTransport* transport = CommsFW::CCommsTransport::NewL(workerregister, globals->VirtualCtor(), NULL);
	globals->SetTransportSender(&transport->GetSender());
	globals->SetTransportReceiver(&transport->GetReceiver());
	CleanupStack::PushL(transport);

	CClock* clock = new(ELeave)CClock();
	CleanupStack::PushL(clock);

	CController* controller = new(ELeave)CController();
	CleanupStack::PushL(controller);

	CTrafficLight* north = new(ELeave)CTrafficLight(console, KNorth());
	CleanupStack::PushL(north);
	north->AddClientL(clock->Id(), Messages::TClientType(EClock));
	clock->AddClientL(north->Id(), Messages::TClientType(ETrafficLight));
	north->AddClientL(controller->Id(), Messages::TClientType(EController));
	controller->AddClientL(north->Id(), Messages::TClientType(ETrafficLight, ENorth));

	CTrafficLight* south = new(ELeave)CTrafficLight(console, KSouth());
	CleanupStack::PushL(south);
	south->AddClientL(clock->Id(), Messages::TClientType(EClock));
	clock->AddClientL(south->Id(), Messages::TClientType(ETrafficLight));
	south->AddClientL(controller->Id(), Messages::TClientType(EController));
	controller->AddClientL(south->Id(), Messages::TClientType(ETrafficLight, ESouth));

	CTrafficLight* east = new(ELeave)CTrafficLight(console, KEast());
	CleanupStack::PushL(east);
	east->AddClientL(clock->Id(), Messages::TClientType(EClock));
	clock->AddClientL(east->Id(), Messages::TClientType(ETrafficLight));
	east->AddClientL(controller->Id(), Messages::TClientType(EController));
	controller->AddClientL(east->Id(), Messages::TClientType(ETrafficLight, EEast));

	CTrafficLight* west = new(ELeave)CTrafficLight(console, KWest());
	CleanupStack::PushL(west);
	west->AddClientL(clock->Id(), Messages::TClientType(EClock));
	clock->AddClientL(west->Id(), Messages::TClientType(ETrafficLight));
	west->AddClientL(controller->Id(), Messages::TClientType(EController));
	controller->AddClientL(west->Id(), Messages::TClientType(ETrafficLight, EWest));

	Messages::RNodeInterface::OpenPostMessageClose(controller->Id(), controller->Id(), TGo().CRef());
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(7, transport);
	
	Messages::TlsGlobals::UnInstallGlobals();
	}

LOCAL_C void DoStartL ()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL (scheduler);
	CActiveScheduler::Install (scheduler);

	MainL ();

	CleanupStack::PopAndDestroy (scheduler);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(createError, console = Console::NewL(_L("Mesh Machine Junction Example"), TSize(KConsFullScreen,KConsFullScreen)));
	if (createError)
		{
		return createError;
		}

	TRAPD(mainError, DoStartL());
	if (mainError)
		{
		console->Printf(_L(" failed, leave code = %d"), mainError);
		}
	console->Printf(_L(" [press any key]\n"));
	console->Getch();

	delete console;
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

