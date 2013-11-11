/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class deliveres synchronization related operations from 
*				 accessories to client that listens to them. It also provides 
*				 methods for client to respond to synchronization operations.
*
*/


// INCLUDE FILES

#include "RemConExtensionApi.h"
#include "RemConSynchronizationTarget.h"
#include "RemConSynchronizationTargetObserver.h"
#include "RemConDebug.h"
#include <e32base.h>
#include <remconinterfaceselector.h>
#include <RemConCallHandlingTarget.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRemConSynchronizationTarget::CRemConSynchronizationTarget
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRemConSynchronizationTarget::CRemConSynchronizationTarget(
								CRemConInterfaceSelector& aInterfaceSelector, 
								MRemConSynchronizationTargetObserver& aObserver)
:	CRemConInterfaceBase(TUid::Uid(KRemConExtSynchronizationApiUid), 
						 KMaxName,
						 aInterfaceSelector,
						 ERemConClientTypeTarget),
	iObserver(aObserver)
	{
	}

// -----------------------------------------------------------------------------
// CRemConSynchronizationTarget::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
// CRemConSynchronizationTarget::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRemConSynchronizationTarget* CRemConSynchronizationTarget::NewL(
								CRemConInterfaceSelector& aInterfaceSelector, 
								MRemConSynchronizationTargetObserver& aObserver)
	{
	COM_TRACE_( "[REMCONEXTAPI] CRemConSynchronizationTarget::NewL() start" );
	
	CRemConSynchronizationTarget* self = new(ELeave) 
					CRemConSynchronizationTarget(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	
	COM_TRACE_( "[REMCONEXTAPI] CRemConSynchronizationTarget::NewL() end" );
	return self;
	}
    
// Destructor
EXPORT_C CRemConSynchronizationTarget::~CRemConSynchronizationTarget()
	{
	COM_TRACE_( "[REMCONEXTAPI] ~CRemConSynchronizationTarget()" );
	}

// -----------------------------------------------------------------------------
// CRemConSynchronizationTarget::GetInterfaceIf
// Called by Remote Control intermediate client when an operation is sent to 
// this API. Intermediate client uses this method to figure out version of 
// interface it should call to inform this API about incoming operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CRemConSynchronizationTarget::GetInterfaceIf( TUid aUid )
    {
    COM_TRACE_2( "[REMCONEXTAPI] CRemConPocTarget::GetInterfaceIf() this=0x%x, aUid=0x%x", this, aUid );

	TAny* ret = NULL;
	
	// Asked interface version implemented by this API ?
	if ( aUid == TUid::Uid( KRemConInterfaceIf3 ) )
		{
		// Return this interface version
		ret = reinterpret_cast< TAny* >( 
				static_cast< MRemConInterfaceIf3* >( this ) );
		}

	COM_TRACE_1( "[REMCONEXTAPI] CRemConPocTarget::GetInterfaceIf() this=0x%x", ret );
	return ret;
    }

// -----------------------------------------------------------------------------
// CRemConSynchronizationTarget::MrcibNewMessage
// Called by Remote Control intermediate client when accessory has sent 
// PoC related operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConSynchronizationTarget::MrcibNewMessage(TUint aOperationId, const TDesC8& /*aData*/, TRemConMessageSubType /*aMessageSubType*/, const TRemConAddress& aRemoteAddr)
	{
	
	iSourceUID = aRemoteAddr.BearerUid().iUid;
	
	switch ( aOperationId )
		{
		case ERemConExtSynchronization:
			{
			COM_TRACE_( "[REMCONEXTAPI] Synchronize()" );
		 	iObserver.Synchronize();
			break;
			}
							
		default:
			COM_TRACE_( "[REMCONEXTAPI] CRemConSynchronizationTarget::MrcibNewMessage() Operation not identified" );
			break;
		}
	}

// -----------------------------------------------------------------------------
// CRemConSynchronizationTarget::SynchronizeResponse
// Called by client to send a response to accessory's operation after
// client has processed operation. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRemConSynchronizationTarget::SynchronizeResponse( 
											TRequestStatus& aStatus, 
											TInt aError)
	{
	COM_TRACE_1( "[REMCONEXTAPI] CRemConSynchronizationTarget::SynchronizeResponse() error=0x%x", aError );
	
	iNumRemotes = 0;
	TUint8* parameter = reinterpret_cast< TUint8*> ( &aError );
	iOutData.Copy( parameter );
		
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConExtSynchronizationApiUid),
		(TUint)ERemConExtSynchronization, 
		iNumRemotes,
		ERemConResponse,
		iOutData);
	
	COM_TRACE_1( "[REMCONEXTENSIOAPI] CRemConSynchronizationTarget::SynchronizeResponse() iNumRemotes=0x%x", iNumRemotes );
	}	

// -----------------------------------------------------------------------------
// CRemConSynchronizationTarget::GetCommandSourceInfo
// Method to get the bearer source detail for the last command received
// -----------------------------------------------------------------------------
//
EXPORT_C void CRemConSynchronizationTarget::GetCommandSourceInfo( TRemConExtCmdSource& aSource )
  {
  CRemConCallHandlingTarget::GetUIDSource( iSourceUID, aSource );
  }
  
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
