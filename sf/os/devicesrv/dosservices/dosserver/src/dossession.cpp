/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*    Implementation for the CDosSessionclass
*
*/


#include <e32svr.h>
#include "dossession.h"
#include "dosfactorybase.h"
#include "dosextensionbase.h"
#include "doseventrcvservice.h"
#include "doseventsndservice.h"
#include "doseventmanager.h"
#include "dosshareddatabase.h"

#include "dos_debug.h"

//
// ---------------------------------------------------------
// CDosSession Constructor
// ---------------------------------------------------------
//  

CDosSession::CDosSession()
{
	iServiceCon = NULL;
	iServices = NULL;
}

//
// ---------------------------------------------------------
// CDosSession Destructor
// ---------------------------------------------------------
//  

CDosSession::~CDosSession()
{
	COM_TRACE_( "CDosSession::~CDosSession()" );

	if(iServices != NULL)
	{
		COM_TRACE_( "[DOSSERVER]\t Deleting services..." );
		COM_TRACE_1( "[DOSSERVER]\t iServices : %d",iServices);
		COM_TRACE_1( "[DOSSERVER]\t iServices count: %d", iServices->Count() );
		delete iServices;
	}

	COM_TRACE_( "[DOSSERVER]\t Callback dropsession" );

	Server().DropSession();

	if(iServiceCon != NULL)
	{
		COM_TRACE_( "[DOSSERVER]\t iServiceCon - remove callback" );
		COM_TRACE_1( "[DOSSERVER]\t iServiceCon : %d", iServiceCon );
		COM_TRACE_1( "[DOSSERVER]\t iServiceCon count: %d", iServiceCon->Count() );
		Server().ContainerIx()->Remove(iServiceCon);
	}

	COM_TRACE_( "CDosSession::~CDosSession() - end" );
}


//
// ---------------------------------------------------------
// CDosSession::CreateL
//
// 2nd phase construct for sessions - called by the CServer 
// framework
// ---------------------------------------------------------
//  
 
void CDosSession::CreateL()
{
	iServiceCon=Server().ContainerIx()->CreateL();
	iServices = CObjectIx::NewL();
	Server().AddSession();
}

//
// ---------------------------------------------------------
// CDosSession::ServiceL
//
// Handle a client request.
// Leaving is handled by CServer::RunError() which reports 
// the error code to the client.
// ---------------------------------------------------------
//
void CDosSession::ServiceL(const RMessage2& aMessage)
{
	COM_TRACE_( "[DOSSERVER]\t CDosSession::ServiceL()" );
	TInt err(KErrNone);
	TInt retVal(KErrNone);

	if((aMessage.Int2() & KAutoComplete) ||
	 aMessage.Function() == EDosCloseSubSession ||
	 aMessage.Function() == EDosEventFiring )
	{
		// Trap possible DSY-plugin leaves here and complete the message with
		// the leave code.
		TRAP(err, retVal = DispatchMessageL(aMessage));
		if (err == KErrNone)
		{
			aMessage.Complete(retVal);
		}
		else
		{
			COM_TRACE_1( "[DOSSERVER]\t ERROR: Leave in DSY impl. (autocomplete set) (%d)", err);
			aMessage.Complete(err);
		}
	}
	else
	{
		//Requests with AutoComplete flag unset will be completed by themselves
		COM_TRACE_( "[DOSSERVER]\t AutoComplete flag unset" );

		// Trap possible DSY-plugin leaves here and complete the message with 
		// the leave code.
		// Since this call is asynchronous only leaves for the call to DSY 
		// callback-function can be trapped here.
		// After that callback function returns the possible leaves will be 
		// trapped by active scheduler and error cases get eventually handled
		// at CDosServer::RunError(). 
		TRAP(err, DispatchMessageL(aMessage));		
		if (err != KErrNone)
		{
			COM_TRACE_1( "[DOSSERVER]\t ERROR: Leave in DSY impl. (%d)", err );
			aMessage.Complete(err);
		}
	}
}

//
// ---------------------------------------------------------
// CDosSession::DispatchMessageL
// ---------------------------------------------------------
//  
TInt CDosSession::DispatchMessageL(const RMessage2& aMessage)
{
	COM_TRACE_( "[DOSSERVER]\t CDosSession::DispatchMessageL()" );

	CDosService* dosService=NULL;

	switch (aMessage.Function())
		{
		case EDosCreateExtensionSubSession:
			COM_TRACE_( "[DOSSERVER]\t EDosCreateExtensionSubSession" );
			dosService = Server().DosFactory()->NewExtensionServiceL();
			break;

		case EDosCreateEventRcvSubSession:
			COM_TRACE_( "[DOSSERVER]\t EDosCreateEventRcvSubSession" );
			dosService = CDosEventRcvService::NewL(Server().EventManager());
			break;

		case EDosCreateEventSndSubSession:
			COM_TRACE_( "[DOSSERVER]\t EDosCreateEventSndSubSession" );
			dosService = CDosEventSndService::NewL(Server().EventManager());
			break;
		
		case EDosCloseSubSession:
			COM_TRACE_( "[DOSSERVER]\t EDosCloseSubSession" );
			CloseSubSession(aMessage);
			return KErrNone;

		case EServerShutdown: 
			COM_TRACE_( "[DOSSERVER]\t EServerShutdown" );
			CActiveScheduler::Stop();
			return KErrNone;
		case EDosCreateSharedDataSubSession: 
			COM_TRACE_( "[DOSSERVER]\t EDosCreateSharedDataSubSession" );
			iSDCounter++;
			COM_TRACE_1( "CDosSession::DispatchMessageL() calling CDosSharedDataBase::NewL(0x%x)", Server().iFileSystemNotifier );
			COM_TRACE_1( "CDosSession::DispatchMessageL() iSDCounter = %d", iSDCounter);
			dosService = CDosSharedDataBase::NewL(Server().iFileSystemNotifier);
			break;
		}

	COM_TRACE_( "[DOSSERVER]\t dosService created, continuing..." );
	
	if(dosService)
	{
		CleanupStack::PushL(dosService);
		SetupSubSessionL(aMessage,dosService);
		CleanupStack::Pop();
		return KErrNone;
	}

	dosService = ServiceFromHandle(aMessage.Int3());
	if (dosService)
	{
		COM_TRACE_( "[DOSSERVER]\t Calling ExecuteMessageL..." );
		return dosService->ExecuteMessageL(aMessage);
	}

	COM_TRACE_( "[DOSSERVER]\t DOSSESSION Planning to panic..." );
	PanicClient(aMessage,EPanicBadHandle);
	return KErrNone;
}

//
// ---------------------------------------------------------
// CDosSession::CloseSubSession
// ---------------------------------------------------------
//  

void CDosSession::CloseSubSession(const RMessage2& aMessage)
{
	COM_TRACE_( "[DOSSERVER]\t CDosSession::CloseSubSession()" );

	iServices->Remove(aMessage.Int3());
}

//
// ---------------------------------------------------------
// CDosSession::SetupSubSession
// ---------------------------------------------------------
//  
void CDosSession::SetupSubSessionL(const RMessage2& aMessage,CDosService* aService)
{
	COM_TRACE_( "[DOSSERVER]\t CDosSession::SetupSubSessionL()" );

	if(aService)
	{
		COM_TRACE_( "[DOSSERVER]\t CDosSession::SetupSubSessionL() - aService valid" );
		iServiceCon->AddL(aService);
		TInt handle=iServices->AddL(aService);
		TPckg<TInt> handlePckg(handle);
		TRAPD(res,aMessage.WriteL(3, handlePckg))
		if (res!=KErrNone)
		{
			COM_TRACE_( "[DOSSERVER]\t CDosSession::SetupSubSessionL() - EPanicBadDescriptor" );

			iServices->Remove(handle);
			PanicClient(aMessage,EPanicBadDescriptor);
		}
		// Every Service has a reference to the Event Manager in case it wants to produce
		// events
		aService->iEventManager = Server().EventManager();
	}
	COM_TRACE_( "[DOSSERVER]\t CDosSession::SetupSubSessionL() completed" );
}


