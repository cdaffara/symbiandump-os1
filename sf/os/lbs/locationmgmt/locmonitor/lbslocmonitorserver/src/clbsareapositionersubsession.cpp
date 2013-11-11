/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include <lbsposition.h>
#include "clbsareapositionersubsession.h"
#include "lbslocmonitorserverdata.h"
#include "clbslocmonitorutils.h"
#include "lbsdevloggermacros.h"


//-------------------------------------------------------------------------------
// Methods called by the Server Framework for subsession creation and destruction
// ------------------------------------------------------------------------------

//
// Called by the Server Framework when the subsession is
// closed by the client or otherwise destroyed (i.e. if the
// owning session is closed).
// 
void CLbsAreaPositionerSubsession::VirtualRelease()
	{
	LBSLOG(ELogP1,"CLbsAreaPositionerSubsession::VirtualRelease()");
	delete this;
	}

// Called by the server framework after creating this object
// in CSubSessionBase::GetImplL
void CLbsAreaPositionerSubsession::CreateSubSessionL(const RMessage2& aMessage, const CSecureServerBase* aServer)
	{
	LBSLOG(ELogP1,"CLbsAreaPositionerSubsession::CreateSubSessionL()");
	iLocMonitorServer = reinterpret_cast<const CLbsLocMonitorServer*>(aServer);
	
	// set ourselves as an observer of server destruction
	CLbsLocMonitorServer* server = const_cast<CLbsLocMonitorServer*>(iLocMonitorServer);
	server->SetLocMonServerDestructObserverL(*this);
	
	iSubsessionId.iSessionPtr = static_cast<CLbsLocMonitorSession*>(aMessage.Session());
	
	TInt subsessionId;
	TPckg<TInt> subsessionIdBuf(subsessionId);
	User::LeaveIfError(aMessage.Read(3, subsessionIdBuf));
	
	iSubsessionId.iSubsessionId = subsessionId;
	}

// Called by the Server Framework on sub-session closure
void CLbsAreaPositionerSubsession::CloseSubSession()
	{
	LBSLOG(ELogP1,"CLbsAreaPositionerSubsession::CloseSubSession()");
	
	// The server should not have been destroyed at this point
	__ASSERT_DEBUG(iLocMonitorServer, User::Invariant());
	// Release anything allocated in CreateSubSessionL.
    // remove ourselves as an observer of server destruction
    CLbsLocMonitorServer* server = const_cast<CLbsLocMonitorServer*>(iLocMonitorServer);    
    server->UnsetLocMonServerDestructObserver(*this); 
	}	


void CLbsAreaPositionerSubsession::DispatchL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"CLbsAreaPositionerSubsession::DispatchL()");
	switch(aMessage.Function())
		{
		case ESetLastKnownPosition:
			SetLastKnownPositionL(aMessage);
			break;
		case EGetLastKnownPosition:
		case EGetLastKnownPositionArea:
			GetPositionL(aMessage);
			break;
		case ECancelGetLastKnownPosition:
		case ECancelGetLastKnownPositionArea:
			CancelGetPositionL(aMessage);
			break;
		default:
			__ASSERT_DEBUG(EFalse, User::Invariant());
			break;
		}
	}
	
void CLbsAreaPositionerSubsession::DispatchError(const RMessage2& /*aMessage*/, TInt /*aError*/)
	{
	LBSLOG(ELogP1,"CLbsAreaPositionerSubsession::DispatchError()");
	// errors from DispatchL end up in here
	}

// -----------------------------------------------------------------------------
// Construction/Destruction methods not directly called by the Server framework
//------------------------------------------------------------------------------

CLbsAreaPositionerSubsession::CLbsAreaPositionerSubsession()
	{
	LBSLOG(ELogP1,"CLbsAreaPositionerSubsession::CLbsAreaPositionerSubsession()");
	}

CLbsAreaPositionerSubsession::~CLbsAreaPositionerSubsession()
	{
	LBSLOG(ELogP1,"->CLbsAreaPositionerSubsession::~CLbsAreaPositionerSubsession");		

	if(iLocMonitorServer)
	    {
        // Tell the DB reader that this subsession is being deleted
        // so that if there are any outanding requests from this session
        // they are terminated with KErrServerTerminated.
        iLocMonitorServer->ReadRequestHandler().ClientTerminated(iSubsessionId);
        // remove ourselves as an observer of server destruction
        CLbsLocMonitorServer* server = const_cast<CLbsLocMonitorServer*>(iLocMonitorServer);    
        server->UnsetLocMonServerDestructObserver(*this);        
	    }
	
	LBSLOG(ELogP1,"<-CLbsAreaPositionerSubsession::~CLbsAreaPositionerSubsession");
	}

CLbsAreaPositionerSubsession* CLbsAreaPositionerSubsession::NewL()
	{
	LBSLOG(ELogP1,"CLbsAreaPositionerSubsession::NewL()");
	CLbsAreaPositionerSubsession* self = new (ELeave) CLbsAreaPositionerSubsession;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CLbsAreaPositionerSubsession::ConstructL()
	{
	LBSLOG(ELogP1,"CLbsAreaPositionerSubsession::ConstructL()");
	// Nothing to do
	}

void CLbsAreaPositionerSubsession::LocMonServerDestructed()
    {
    iLocMonitorServer = NULL;
    }


//-------------------------------------------------------------------------
// 							Action methods
// (These method just relay the client request to a database writer or 
//  reader where requests of the same type are either serialized or  
// 	merged into a single call to the database)
//-------------------------------------------------------------------------

/*
Called when a a client (e.g. Location Server) has a position that wants to
add to the database.
*/
void CLbsAreaPositionerSubsession::SetLastKnownPositionL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"->CLbsAreaPositionerSubsession::SetLastKnownPositionL");

	// Read the position
	TPosition position;
	TPckg<TPosition> positionPkg(position);
	aMessage.ReadL(0, positionPkg, 0);
	
	// Complete the call 
	aMessage.Complete(KErrNone);
	
	// Pass the message on to the database writer
	iLocMonitorServer->ReadRequestHandler().StoreLastKnownPosition(position);

	LBSLOG(ELogP1,"<-CLbsAreaPositionerSubsession::SetLastKnownPositionL");
	}

void CLbsAreaPositionerSubsession::GetPositionL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"->CLbsAreaPositionerSubsession::GetPositionL");		
	
	iLocMonitorServer->ReadRequestHandler().PositionRequestL(aMessage);
	
	LBSLOG(ELogP1,"<-CLbsAreaPositionerSubsession::GetPositionL");	
	}
	

void CLbsAreaPositionerSubsession::CancelGetPositionL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"->CLbsAreaPositionerSubsession::CancelGetPositionL");		

	iLocMonitorServer->ReadRequestHandler().CancelPositionRequestL(aMessage);

	LBSLOG(ELogP1,"<-CLbsAreaPositionerSubsession::CancelGetPositionL");	
	}


