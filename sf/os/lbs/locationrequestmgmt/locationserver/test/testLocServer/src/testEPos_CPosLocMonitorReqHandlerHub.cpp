/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  	Receives the Last Known Position, Last Known Position Area requests and Empty
* 				Position Store requests from Subsessions and Sessions in the Location Server and 
*				forwards them to the respective active objects*
*/

#include <e32base.h>
#include <e32def.h>
#include "testEPos_CPosLocMonitorReqHandlerHub.h"
#include <lbs/epos_lastknownlocationpskeys.h>


// Defines SID of Location Server
const TInt KLocationServerSID=0x102869E5; //TODO - value to be changed to the actual test location server

// CONSTANTS
#ifdef _DEBUG
_LIT(KTraceFileName, "testEPos_CPosLocMonitorReqHandlerHub.cpp");
#endif


/**
 * NewL of the Two-phased constructor.
 *
 */
CPosLocMonitorReqHandlerHub* CPosLocMonitorReqHandlerHub::NewL()
	{

	CPosLocMonitorReqHandlerHub* self = new( ELeave ) CPosLocMonitorReqHandlerHub();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

/**
 * C++ default constructor
 * 
 * @param aDumpInterval 
 */
CPosLocMonitorReqHandlerHub::CPosLocMonitorReqHandlerHub() 
    {

    }

/**
 * Symbian 2nd phase constructor
 */
void CPosLocMonitorReqHandlerHub::ConstructL()
	{
	
	// Establish a session with the Location Monitor - The subsessions are
	// established in the "active objects" owned by the CPosLocMonitorReqHandlerHub
	User::LeaveIfError(iLocMonSession.Connect()); 
	
	// Establish the subsession with the location monitor - As SetPositionInfoL()
	// is likely to be called whenever we have an update from the PSYs, the subsession
	// with the location monitor is created as a member variable instead of a local variable.
	
	// As this subsession handle is used for internal request from SetPositionInfoL() 
	// and not for any requests from the client we do not leave if it fails
	TInt err = iLocMonSubSession.OpenL(iLocMonSession);
	
	}


CPosLocMonitorReqHandlerHub::~CPosLocMonitorReqHandlerHub()
	{
	
    delete iLastKnownPosHandler;	
    delete iLastKnownPosAreaHandler;
    delete iEmptyLastKnownPosStoreHandler;
	
	// Close the session with the Location Monitor
	if ( iLocMonSubSession.SubSessionHandle() )
		{
		iLocMonSubSession.Close();
		}
	
	// Close the session with the Location Monitor
	if ( iLocMonSession.Handle() )
		{
		iLocMonSession.Close();
		}
		
	}

/** 
 * Set Position Information (SetPositionInfoL)
 * 		>> Whenever there is a position update from any of the PSYs update the 
 * 		>> location monitor of this latest position.
 *
 * @param aPositionInfo The last known position.
 */
void CPosLocMonitorReqHandlerHub::SetPositionInfoL( const TPositionInfo& aPositionInfo )
	{
	RLbsLocMonitorAreaPositioner locMonAreaPositioner;
	CleanupClosePushL(locMonAreaPositioner);
	User::LeaveIfError(locMonAreaPositioner.OpenL(iLocMonSession));

	// If the subsession is not opened yet, open a new one 
	TInt locMonSubSessionHandle = iLocMonSubSession.SubSessionHandle();
	
	if ( !locMonSubSessionHandle )
		{
			TInt errOpen = iLocMonSubSession.OpenL(iLocMonSession);
			if (errOpen==KErrNone)
				{
				// Call the function to send the latest location update to the Location monitor
				//	TODO  Can Leave ??? ******
				TInt errSetPos = iLocMonSubSession.SetLastKnownPosition(aPositionInfo);
				}
		}
	else
		{
		//	TODO  Can Leave ??? ******
		// Call the function to send the latest location update to the Location monitor
		TInt errSetPos = iLocMonSubSession.SetLastKnownPosition(aPositionInfo);
		}
	}

/** 
 * Get Last Known Position Request
 * 		>> Called by the subsession to request the last known position.
 *
 * @param aMessage The message containing info about the request from subsession
 */
void CPosLocMonitorReqHandlerHub::GetLastKnownPosReqL(const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosLocMonitorReqHandlerHub::GetLastKnownPosReqL", __LINE__)
	
	if (!iLastKnownPosHandler)
		{
		// The very first request with the location monitor
		iLastKnownPosHandler = CPosLastKnownPosHandler::NewL();
		}
	
	// The timeout value for this request is hardcoded in the constant 
	// KLastKnownPosTimeOut [in CPosLastKnownPosHandler]
	iLastKnownPosHandler->GetLastKnownPosL(iLocMonSession, aMessage);
	}


/** Cancel Get Last Known Position Request 
 * 		>> Called by the subsession to cancel a last knwon position request
 * @param aMessage The message containing info about the request from subsession
 */
void CPosLocMonitorReqHandlerHub::CancelGetLastKnownPosReqL(const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosLocMonitorReqHandlerHub::CancelGetLastKnownPosReqL", __LINE__)

	if (!iLastKnownPosHandler)
		{
		// The client never sent any get last known position request earlier
		if (!aMessage.IsNull())
			{
			aMessage.Complete(KErrNotFound);
			}
		}
	else
		{
		iLastKnownPosHandler->CancelGetLastKnownPosL(aMessage);
		}
	}


/** 
 * Get Last Known Position Area Request
 * 		>> Called by the subsession to request the last known position area.
 *
 * @param aMessage The message containing info about the request from subsession
 */
void CPosLocMonitorReqHandlerHub::GetLastKnownPosAreaReqL(const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosLocMonitorReqHandlerHub::GetLastKnownPosAreaReqL", __LINE__)

	if (!iLastKnownPosAreaHandler)
		{
		// The very first request with the location monitor
		iLastKnownPosAreaHandler = CPosLastKnownPosAreaHandler::NewL();
		}
	iLastKnownPosAreaHandler->GetLastKnownPosAreaL(iLocMonSession, aMessage);

	}


/** Cancel Get Last Known Position Area Request 
 * 		>> Called by the subsession to cancel a LKPosArea request
 * @param aMessage The message containing info about the request from subsession
 */
void CPosLocMonitorReqHandlerHub::CancelGetLastKnownPosAreaReqL(const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosLocMonitorReqHandlerHub::CancelGetLastKnownPosAreaReqL", __LINE__)

	if (!iLastKnownPosAreaHandler)
		{
		// The client never sent any get last known position area request earlier
		if (!aMessage.IsNull())
			{
			aMessage.Complete(KErrNotFound);
			}
		}
	else
		{
		iLastKnownPosAreaHandler->CancelGetLastKnownPosAreaL(aMessage);
		}
	}

/** 
 * Empty Last Known Position Store Request (EmptyPositionStoreReq)
 *		>> Called by the subsession to Empty the last known position store
 *
 *  @param aMessage The message containing info about the request from subsession
 */
void CPosLocMonitorReqHandlerHub::EmptyLastKnownPosStoreReqL(const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosLocMonitorReqHandlerHub::EmptyLastKnownPosStoreL", __LINE__)
	
	if (!iEmptyLastKnownPosStoreHandler)
		{
		// The very first request with the location monitor
		iEmptyLastKnownPosStoreHandler = CPosEmptyLastKnownPosStoreHandler::NewL();
		}
	
	iEmptyLastKnownPosStoreHandler->EmptyLastKnownPosStoreL(iLocMonSession, aMessage);

	// Cancel outstanding last known position or last known position area requests
	if (iLastKnownPosAreaHandler)
		{
		iLastKnownPosAreaHandler->NotifyOnEmptyLastKnownPosStoreReq();
		}
	
	if (iLastKnownPosHandler)
		{
		iLastKnownPosHandler->NotifyOnEmptyLastKnownPosStoreReq();
		}
	
	}


/** Cancel Empty Last Known Position Store Request 
 * 		>> Called by the subsession to cancel an earlier EmptyLastKnownPosStoreReq
 * @param aMessage The message containing info about the request from subsession
 */
void CPosLocMonitorReqHandlerHub::CancelEmptyLastKnownPosStoreReqL(const RMessage2& aMessage)
	{
	DEBUG_TRACE("CPosLocMonitorReqHandlerHub::CancelGetLastKnownPosAreaReqL", __LINE__)
	
	iEmptyLastKnownPosStoreHandler->CancelEmptyLastKnownPosStoreL(aMessage);
	}


/** 
 * NotifyServerShutDown
 * 		>> Notification from the Session that the server is going to shutdown
 * 
 */
void CPosLocMonitorReqHandlerHub::NotifyServerShutDown()
	{
	DEBUG_TRACE("CPosLocMonitorReqHandlerHub::NotifyServerShutDown", __LINE__)
	
	if (iLastKnownPosAreaHandler)
		{
		iLastKnownPosAreaHandler->NotifyServerShutDown();
		}
	
	if (iLastKnownPosHandler)
		{
		iLastKnownPosHandler->NotifyServerShutDown();
		}
	
	if (iEmptyLastKnownPosStoreHandler)
		{
		iEmptyLastKnownPosStoreHandler->NotifyServerShutDown();
		}
	}

/** 
 * NotifySubSessionClosed
 * 		>> Notification from the Session that the subsessions are to be closed
 * 
 */
void CPosLocMonitorReqHandlerHub::NotifySubSessionClosed(const RMessage2& aMessage)
	{

	if (iLastKnownPosHandler)
		{
		iLastKnownPosHandler->NotifySubSessionClosed(aMessage);
		}
	
	if (iLastKnownPosAreaHandler)
		{
		iLastKnownPosAreaHandler->NotifySubSessionClosed(aMessage);
		}
	// No need to notify the emptylastknownpositionstore request as it is issued on a 
	// session and not a subsession
	}

/** 
 * NotifySessionClosed
 * 		>> Notification from the Session that it is going to be closed 
 *		NOTE : Not guaranteed that the requests would be completed with EPositoinRequestsNotCancelled
 * 			   - This method is used to atleast cleanup the request queue
 * @param aSessionPtr The Session pointer
 */
void CPosLocMonitorReqHandlerHub::NotifySessionClosed(const CSession2* aSessionPtr)
	{
	DEBUG_TRACE("CPosLocMonitorReqHandlerHub::NotifySessionClosed", __LINE__)
	
	if (iLastKnownPosAreaHandler)
		{
		iLastKnownPosAreaHandler->NotifySessionClosed(aSessionPtr);
		}
	
	if (iLastKnownPosHandler)
		{
		iLastKnownPosHandler->NotifySessionClosed(aSessionPtr);
		}
	
	if (iEmptyLastKnownPosStoreHandler)
		{
		iEmptyLastKnownPosStoreHandler->NotifySessionClosed(aSessionPtr);
		}
	
	}



