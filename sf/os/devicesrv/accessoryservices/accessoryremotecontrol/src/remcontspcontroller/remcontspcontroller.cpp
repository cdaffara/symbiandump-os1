/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements Target Selector Plugin used by Symbian's Remote 
*				 Control Framework. TSP controls sending of operations between 
*				 Series 60 components and remote targets (e.g. headset).
*
*				 This version doesn't really self specify receiver for 
*				 a message but accepts all receivers suggested by RemCon server.
*
*
*/


// INCLUDE FILES
#include "RemConDebug.h"
#include "remcontspcontroller.h"
#include <remcontargetselectorpluginobserver.h>
#include <remconaddress.h>
#include <clientinfo.h>
#include <bearersecurity.h>
#include <remconextensionapi.h>
#include <ctsydomainpskeys.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
#ifdef __ACCESSORY_FW

// UID3 of process (Accessory Server) which loads PhoneCmdHandler.dll
const TUint32 KUid3 = 0x10205030;

#else

// UID3 of process (Phone) which loads PhoneCmdHandler.dll
const TUint32 KUid3 = 0x100058B3;

#endif

const TUint32 KUid3MusicPlayer = 0x102072C3;


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRemConTspController::CRemConTspController
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CRemConTspController::CRemConTspController( 
								MRemConTargetSelectorPluginObserver& aObserver )
:	CRemConTargetSelectorPlugin( aObserver )
	{
	}
    
// -----------------------------------------------------------------------------
// CRemConTspController::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRemConTspController* CRemConTspController::NewL( 
								MRemConTargetSelectorPluginObserver& aObserver )
	{
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::NewL()" );
	
	CRemConTspController* self = new(ELeave) CRemConTspController( aObserver );
	return self;
	}

// Destructor
CRemConTspController::~CRemConTspController()
	{
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::~CRemConTspController()" );
	}

// -----------------------------------------------------------------------------
// CRemConTspController::GetInterface
// Provides implementation of this (TSP) interface. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CRemConTspController::GetInterface( TUid aUid )
	{
	COM_TRACE_2( "[REMCONTSPCONTROLLER] CRemConTspController::GetInterface() this=%d, aUid=%d", this, aUid );

	TAny* ret = NULL;
	
	// TSP interface is asked ?
	if ( aUid == TUid::Uid( KRemConTargetSelectorInterface1 ) )
		{
		// convert this object to TAny
		ret = reinterpret_cast< TAny* >( 
				static_cast< MRemConTargetSelectorPluginInterface*>(this) );
		}
	else if ( aUid == TUid::Uid(KRemConTargetSelectorInterface2) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MRemConTargetSelectorPluginInterfaceV2*>(this)
			);
		}

	COM_TRACE_1( "[REMCONTSPCONTROLLER] CRemConTspController::GetInterface() this=%d", ret );
	return ret;
	}

// -----------------------------------------------------------------------------
// CRemConTspController::AddressOutgoingCommand
// Defines bearers (represents remote target) to which command will be sent. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConTspController::AddressOutgoingCommand(
		TUid aInterfaceUid,
		TUint aOperationId, 
		const TClientInfo& aSender,
		TSglQue<TRemConAddress>& aConnections,
		TSglQue<TBearerSecurity>& aBearerSecurity)
	{
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::AddressOutgoingCommand() starts" );
	
	// Avoid warnings for unused parameters
	(void)aInterfaceUid;
	(void)aOperationId;
	(void)aSender;
		
	TInt err = KErrNotFound;
	TSglQueIter<TBearerSecurity> iter( aBearerSecurity );
	TBearerSecurity* sec = iter;
	
	// Go through all bearers
	while( sec )
		{
		// No security check in this version.
		// Could maybe be smth like "if ( sec->SecurityPolicy().CheckPolicy( aSender.Message() ) )"
				
		// Address command to all given bearers
		TRemConAddress* conn = NULL;
		conn = new TRemConAddress;
		if ( conn )
			{
			COM_TRACE_1( "[REMCONTSPCONTROLLER] Sender's secure id = %d", aSender.SecureId().iId );
			COM_TRACE_1( "[REMCONTSPCONTROLLER] Command sent to bearer %d", sec->BearerUid().iUid );
						
			conn->BearerUid().iUid = sec->BearerUid().iUid; 
			// No connection-specific data for this bearer.
			// Ownership of conn is passed back to RemCon.
			aConnections.AddLast( *conn );
			err = KErrNone;
			}
		else
			{
			COM_TRACE_1( "[REMCONTSPCONTROLLER] Memory allocation failed for bearer %d", sec->BearerUid().iUid );
			
			err = KErrNoMemory;
			break;	// Get out from while() and return KErrNoMemory
			}
		iter++;
		sec = iter;
		}

	COM_TRACE_1( "[REMCONTSPCONTROLLER] CRemConTspController::AddressOutgoingCommand() err=%d", err );
	Observer().OutgoingCommandAddressed( err );
	
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::AddressOutgoingCommand() ends" );
	}

// -----------------------------------------------------------------------------
// CRemConTspController::PermitOutgoingCommand
// Defines whether connection-oriented controller can send a command to bearer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConTspController::PermitOutgoingCommand(
		TUid aInterfaceUid,
		TUint aOperationId, 
		const TClientInfo& aSender,
		const TRemConAddress& aConnection )
	{
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::PermitOutgoingCommand()" );
	
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::PermitOutgoingCommand() start" );
	COM_TRACE_2( "[REMCONTSPCONTROLLER] aInterfaceUid = %d, aOperationId = %d", aInterfaceUid.iUid, aOperationId );
	COM_TRACE_1( "[REMCONTSPCONTROLLER] sender's secure id =%d", aSender.SecureId().iId );
				
	// Avoid warnings for unused parameters
	(void)aInterfaceUid;
	(void)aOperationId;
	(void)aSender;
	(void)aConnection;
	
	// Permit all connection oriented controllers to send commands 
	Observer().OutgoingCommandPermitted( ETrue );
	
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::PermitOutgoingCommand() ends" );
	}

// -----------------------------------------------------------------------------
// CRemConTspController::CancelOutgoingCommand
// Cancels the current AddressOutgoingCommand or PermitOutgoingCommand command.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConTspController::CancelOutgoingCommand()
	{
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::CancelOutgoingCommand()" );
	// Nothing to do until AddressOutgoingCommand() and/or PermitOutgoingCommand() 
	// will be implemented asynchronously.
	}

// -----------------------------------------------------------------------------
// CRemConTspController::AddressIncomingCommand
// Defines remote targets to which command will be sent.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConTspController::AddressIncomingCommand(
		TUid aInterfaceUid,
		TUint aOperationId, 
		TSglQue<TClientInfo>& aClients )
	{
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::AddressIncomingCommand() start" );
	COM_TRACE_2( "[REMCONTSPCONTROLLER] aInterfaceUid = %d, aOperationId = %d", aInterfaceUid.iUid, aOperationId );
		
	#ifdef _DEBUG
	
	// For debuging purpose write all remote target ids to trace
	TSglQueIter<TClientInfo> iter( aClients );
	TClientInfo* target = iter;
	TInt number(0);
	
	while( target )
		{
		COM_TRACE_2( "[REMCONTSPCONTROLLER] %d. client secureId = %d", ++number, target->SecureId().iId );
		iter++;
		target = iter;
		}

	#endif
		
	TInt err( KErrNone );
	
	// This version sends operation to all existing clients with following 
	// exception: If there's an incoming or ongoing voice call, 
	// RemConExtensionApi's command ERemConExtAnswerEnd is sent only to 
	// PhoneCmdHandler. 
	if( aInterfaceUid.iUid == KRemConExtCallHandlingApiUid && 
		aOperationId == ERemConExtAnswerEnd 
	  )
		{
		TInt callState( EPSCTsyCallStateUninitialized );
    	TInt err( KErrNone );
    	err = iProperty.Get( KPSUidCtsyCallInformation, 
    						 KCTsyCallState, 
    						 callState ); 
    						 
    	if( ( err == KErrNone ) && 
    	    ( callState == EPSCTsyCallStateRinging ||
    	      callState == EPSCTsyCallStateConnected )
    	  )
			{
			TSglQueIter<TClientInfo> iter( aClients );
			TClientInfo* target = iter;
			
			while( target )
				{
				if( target->SecureId().iId == KUid3 )
					{
					COM_TRACE_1( "[REMCONTSPCONTROLLER] Command is sent only to PhoneCmdHandler running in process identified by secure id=%d", target->SecureId().iId );
									
					// Add PhoneCmdHandler as the only remote target to 
					// receive ERemConExtAnswerEnd command. 
					aClients.Reset();
					aClients.AddFirst( *target );
					break;	// get out from while-loop
					}
				iter++;
				target = iter;
				}
			}
		}

	Observer().IncomingCommandAddressed( err );
	
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::AddressIncomingCommand() ends" );
	}



// -----------------------------------------------------------------------------
// CRemConTspController::PermitOutgoingResponse
// Defines which client can send a response to a command to bearer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConTspController::PermitOutgoingResponse(
		TUid aInterfaceUid,
		TUint aOperationId,
		const TClientInfo& aClient,
		TClientInfoConstIter& aClients)
	{
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::PermitOutgoingResponse()" );
	
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::PermitOutgoingResponse() start" );
	COM_TRACE_2( "[REMCONTSPCONTROLLER] aInterfaceUid = %d, aOperationId = %d", aInterfaceUid.iUid, aOperationId );
				
	// Avoid warnings for unused parameters
	(void)aInterfaceUid;
	(void)aOperationId;
	(void)aClient;
	(void)aClients;

	Observer().OutgoingResponsePermitted(ETrue);
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::PermitOutgoingResponse() ends" );
	}


// -----------------------------------------------------------------------------
// CRemConTspController::CancelOutgoingResponse
// Called by RemCon to cancel the current PermitOutgoingResponse request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConTspController::CancelOutgoingResponse()
	{
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::CancelOutgoingCommand()" );
	// Nothing to do until PermitOutgoingResponse() 
	// will be implemented asynchronously.
	}


// -----------------------------------------------------------------------------
// CRemConTspController::AddressIncomingNotify
// Defines remote targets to which notify will be sent.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConTspController::AddressIncomingNotify(
		TUid aInterfaceUid,
		TUint aOperationId,
		TSglQue<TClientInfo>& aClients)
	{
	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::AddressIncomingNotify() start" );
	COM_TRACE_2( "[REMCONTSPCONTROLLER] aInterfaceUid = %d, aOperationId = %d", aInterfaceUid.iUid, aOperationId );
	(void)aInterfaceUid;
	(void)aOperationId;

	TClientInfo* clientInfo = aClients.First();
	TSglQueIter<TClientInfo> iter( aClients );
	TClientInfo* target = iter;
	
	while( target )
		{
		if( target->SecureId().iId == KUid3MusicPlayer )
			{
			COM_TRACE_1( "[REMCONTSPCONTROLLER] Command is sent only to PlaybackUI running in process identified by secure id=%d", target->SecureId().iId );
			break;	// get out from while-loop
			}
		iter++;
		target = iter;
		}

	Observer().IncomingNotifyAddressed(target, KErrNone);

	COM_TRACE_( "[REMCONTSPCONTROLLER] CRemConTspController::AddressIncomingCommand() ends" );

	}



//
// End of file
