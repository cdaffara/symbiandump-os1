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
* Description:  This class deliveres PoC related operations from accessories 
*				 to client that listens to them. It also provides methods for 
*				 client to respond to PoC operations.
*
*/


// INCLUDE FILES

#include "RemConExtensionApi.h"
#include "RemConPocTarget.h"
#include "RemConPocTargetObserver.h"
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
// CRemConPocTarget::CRemConPocTarget
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRemConPocTarget::CRemConPocTarget(CRemConInterfaceSelector& aInterfaceSelector, 
								   MRemConPocTargetObserver& aObserver)
:	CRemConInterfaceBase(TUid::Uid(KRemConExtPocApiUid), 
						 KMaxName,
						 aInterfaceSelector,
						 ERemConClientTypeTarget),
	iObserver(aObserver)
	{
	}

// -----------------------------------------------------------------------------
// CRemConPocTarget::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
// CRemConPocTarget::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRemConPocTarget* CRemConPocTarget::NewL(CRemConInterfaceSelector& aInterfaceSelector, 
												  MRemConPocTargetObserver& aObserver)
	{
	COM_TRACE_( "[REMCONEXTAPI] CRemConPocTarget::NewL() start" );
		
	CRemConPocTarget* self = new(ELeave) CRemConPocTarget(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	
	COM_TRACE_( "[REMCONEXTAPI] CRemConPocTarget::NewL() end" );
	return self;
	}
    
// Destructor
EXPORT_C CRemConPocTarget::~CRemConPocTarget()
	{
	COM_TRACE_( "[REMCONEXTAPI] ~CRemConPocTarget()" );
	}

// -----------------------------------------------------------------------------
// CRemConPocTarget::SendResponse
// Sends responses to accessories.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConPocTarget::SendResponse( 
							TRequestStatus& aStatus,
							TInt aOperationId,
							TInt aError )
	{
	COM_TRACE_2( "[REMCONEXTAPI] CRemConPocTarget::SendResponse() opId=0x%x, error=0x%x", aOperationId, aError );
	
	iNumRemotes = 0;
	TUint8* parameter = reinterpret_cast<TUint8*>( &aError );
	iOutData.Copy( parameter );
	
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConExtPocApiUid),
		(TUint)aOperationId, 
		iNumRemotes,
		ERemConResponse,
		iOutData);
	
	COM_TRACE_1( "[REMCONEXTAPI] CRemConPocTarget::SendResponse() iNumRemotes=0x%x", iNumRemotes );
	}

// -----------------------------------------------------------------------------
// CRemConPocTarget::GetInterfaceIf
// Called by Remote Control intermediate client when an operation is sent to 
// this API. Intermediate client uses this method to figure out version of 
// interface it should call to inform this API about incoming operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CRemConPocTarget::GetInterfaceIf( TUid aUid )
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
// CRemConPocTarget::MrcibNewMessage
// Called by Remote Control intermediate client when accessory has sent 
// PoC related operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConPocTarget::MrcibNewMessage(TUint aOperationId, const TDesC8& /*aData*/, TRemConMessageSubType /*aMessageSubType*/, const TRemConAddress& aRemoteAddr)
	{
	
	iSourceUID = aRemoteAddr.BearerUid().iUid;
	
	switch ( aOperationId )
		{
		case ERemConExtAPOCButtonUp:
			{
			COM_TRACE_( "[REMCONEXTAPI] APOCButtonUp()" );
		 	iObserver.APOCButtonUp();
			break;
			}
			
		case ERemConExtAPOCButtonDown:
			{
			COM_TRACE_( "[REMCONEXTAPI] APOCButtonDown()" );
		 	iObserver.APOCButtonDown();
			break;
			}
					
		default:
			COM_TRACE_( "[REMCONEXTAPI] CRemConPocTarget::MrcibNewMessage() Operation not identified" );
			break;
		}
	}
	
// -----------------------------------------------------------------------------
// CRemConPocTarget::AnswerCallResponse
// Called by client to send a response to accessory's operation after
// client has processed operation. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRemConPocTarget::APOCButtonUpResponse( 
											TRequestStatus& aStatus, 
											TInt aError)
	{
	SendResponse( aStatus, ERemConExtAPOCButtonUp, aError );
	}	

// -----------------------------------------------------------------------------
// CRemConPocTarget::EndCallResponse
// Called by client to send a response to accessory's operation after
// client has processed operation. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRemConPocTarget::APOCButtonDownResponse( 
											TRequestStatus& aStatus, 
											TInt aError )	
	{
	SendResponse( aStatus, ERemConExtAPOCButtonDown, aError );
	}

// -----------------------------------------------------------------------------
// CRemConPocTarget::GetCommandSourceInfo
// Method to get the bearer source detail for the last command received
// -----------------------------------------------------------------------------
//
EXPORT_C void CRemConPocTarget::GetCommandSourceInfo( TRemConExtCmdSource& aSource )
  {
  CRemConCallHandlingTarget::GetUIDSource( iSourceUID, aSource );
  }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
