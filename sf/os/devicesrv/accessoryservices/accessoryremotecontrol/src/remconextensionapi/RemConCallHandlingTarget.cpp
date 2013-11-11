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
* Description:  This class deliveres call handling related operations from 
*				 accessories to client that listens to them. It also provides 
*				 methods for client to respond to call handling operations.
*
*/


// INCLUDE FILES
#include "RemConExtensionApi.h"
#include "RemConCallHandlingTarget.h"
#include "RemConCallHandlingTargetObserver.h"
#include "RemConDebug.h"
#include "RemConSourceHandler.h"
#include <e32base.h>
#include <remconinterfaceselector.h>

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
// CRemConCallHandlingTarget::CRemConCallHandlingTarget
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRemConCallHandlingTarget::CRemConCallHandlingTarget(
						CRemConInterfaceSelector& aInterfaceSelector, 
						MRemConCallHandlingTargetObserver& aObserver)
:	CRemConInterfaceBase(TUid::Uid(KRemConExtCallHandlingApiUid), 
						 KMaxName,
						 aInterfaceSelector,
						 ERemConClientTypeTarget),
	iObserver(aObserver)
	{
	}

// -----------------------------------------------------------------------------
// CRemConCallHandlingTarget::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
// CRemConCallHandlingTarget::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRemConCallHandlingTarget* CRemConCallHandlingTarget::NewL(
					CRemConInterfaceSelector& aInterfaceSelector, 
					MRemConCallHandlingTargetObserver& aObserver)
	{
	COM_TRACE_( "[REMCONEXTAPI] CRemConCallHandlingTarget::NewL() start" );
		
	CRemConCallHandlingTarget* self = 
		new(ELeave) CRemConCallHandlingTarget(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	
	COM_TRACE_( "[REMCONEXTAPI] CRemConCallHandlingTarget::NewL() end" );
	return self;
	}
    
// Destructor
EXPORT_C CRemConCallHandlingTarget::~CRemConCallHandlingTarget()
	{
	COM_TRACE_( "[REMCONEXTAPI] ~CRemConCallHandlingTarget()" );
	}

// -----------------------------------------------------------------------------
// CRemConCallHandlingTarget::SendAnyResponse
// Sends responses to accessories.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConCallHandlingTarget::SendAnyResponse(
									TRequestStatus& aStatus,
									TInt aOperationId,
									TInt aError)
	{
	COM_TRACE_2( "[REMCONEXTAPI] CRemConCallHandlingTarget::SendAnyResponse() opId=0x%x, error=0x%x", aOperationId, aError );
	
	iNumRemotes = 0;
	TUint8* parameter = reinterpret_cast<TUint8*>( &aError );
	iOutData.Copy( parameter );
	
	InterfaceSelector().Send(
		aStatus, 
		TUid::Uid(KRemConExtCallHandlingApiUid),
		(TUint)aOperationId, 
		iNumRemotes,
		ERemConResponse,
		iOutData );
	
	COM_TRACE_1( "[REMCONEXTAPI] CRemConCallHandlingTarget::SendAnyResponse() iNumRemotes=0x%x", iNumRemotes );
	}
	
// -----------------------------------------------------------------------------
// CRemConCallHandlingTarget::GetInterfaceIf
// Called by Remote Control intermediate client when an operation is sent to 
// this API. Intermediate client uses this method to figure out version of 
// interface it should call to inform this API about incoming operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CRemConCallHandlingTarget::GetInterfaceIf( TUid aUid )
    {
    COM_TRACE_2( "[REMCONEXTAPI] CRemConCallHandlingTarget::GetInterfaceIf() this=0x%x, aUid=0x%x", this, aUid );

	TAny* ret = NULL;
	
	// Asked interface version implemented by this API ?
	if ( aUid == TUid::Uid( KRemConInterfaceIf3 ) )
		{
		// Return this interface version
		ret = reinterpret_cast< TAny* >( 
				static_cast< MRemConInterfaceIf3* >( this ) );
		}

	COM_TRACE_1( "[REMCONEXTAPI] CRemConCallHandlingTarget::GetInterfaceIf() this=0x%x", ret );
	return ret;
    }
		
// -----------------------------------------------------------------------------
// CRemConCallHandlingTarget::MrcibNewMessage
// Called by Remote Control intermediate client when accessory has sent 
// call handling related operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRemConCallHandlingTarget::MrcibNewMessage(TUint aOperationId, const TDesC8& aData, TRemConMessageSubType /*aMessageSubType*/, const TRemConAddress& aRemoteAddr)
	{
	iSourceUID = aRemoteAddr.BearerUid().iUid;
	
	switch ( aOperationId )
		{
		case ERemConExtAnswerCall:
			{
			COM_TRACE_( "[REMCONEXTAPI] AnswerCall()" );
		 	iObserver.AnswerCall();
			break;
			}
		
		case ERemConExtEndCall:
			{
			COM_TRACE_( "[REMCONEXTAPI] EndCall()" );
		 	iObserver.EndCall();
			break;
			}
			
		case ERemConExtAnswerEnd:
			{
			COM_TRACE_( "[REMCONEXTAPI] AnswerEndCall()" );
		 	iObserver.AnswerEndCall();
			break;
			}
			
		case ERemConExtVoiceDial:
			{
			COM_TRACE_( "[REMCONEXTAPI] VoiceDial()" );
		 	 	
			// Copy active status from aData to TBool
			TUint8* ptr = const_cast< TUint8* >( aData.Ptr() ); 
	        TPtr8 tptr( ptr, aData.Length(), aData.Length() );
	        TLex8 input( tptr );
			TBool active( EFalse );
			input.Val( active );
	        
	        COM_TRACE_1( "[REMCONEXTAPI] VoiceDial() active=%d ", active );
		
	        iObserver.VoiceDial( active );
			break;
			}
			
		case ERemConExtLastNumberRedial:
			{
			COM_TRACE_( "[REMCONEXTAPI] LastNumberRedial()" );
		 	iObserver.LastNumberRedial();
			break;
			}
					
		case ERemConExtDialCall:
			{
			COM_TRACE_( "[REMCONEXTAPI] DialCall()" );
		 	iObserver.DialCall( aData );
			break;
			}
			
		case ERemConExt3WaysCalling:
			{
			COM_TRACE_( "[REMCONEXTAPI] MultipartyCalling()" );
			iObserver.MultipartyCalling( aData );
			break;
			}
			
		case ERemConExtGenerateDTMF:
			{
			
	        COM_TRACE_( "[REMCONEXTAPI] GenerateDTMF()" );
	                            
            // Copy DTMF character from aData to TChar
            TUint8* ptr8 = const_cast< TUint8* >( aData.Ptr() ); 
            
            COM_TRACE_1( "[REMCONEXTAPI] data %S", &aData );

            COM_TRACE_1( "[REMCONEXTAPI] aData.Length %d", aData.Length() );
            for(TInt i = 0; i<aData.Length(); i++)    
                {
                COM_TRACE_1( "i=%d", i );
                COM_TRACE_1( "[REMCONEXTAPI] GenerateDTMF() ASCII numerical from ptr8=%d", *ptr8 );
                TChar ch( *ptr8 );
                COM_TRACE_RAW_1( "[REMCONEXTAPI] GenerateDTMF() character from ch=", ch );
                iObserver.GenerateDTMF( ch );
                ptr8++;
                }
            break;  
			
			}
			
		case ERemConExtSpeedDial:
			{
			COM_TRACE_( "[REMCONEXTAPI] SpeedDial()" );
					
			// Copy TInt from aData to index
			TUint8* ptr = const_cast< TUint8* >( aData.Ptr() ); 
	        TPtr8 tptr( ptr, aData.Length(), aData.Length() );
	        TLex8 input( tptr );
			TInt index( 0 );
			input.Val( index );
			
			COM_TRACE_1( "[REMCONEXTAPI] SpeedDial() index=%d", index );
					
			iObserver.SpeedDial( index );	
			break;
			}
			
		default:
			COM_TRACE_( "[REMCONEXTAPI] CRemConCallHandlingTarget::MrcibNewMessage() Operation not identified" );
			break;
		}
	}

// -----------------------------------------------------------------------------
// CRemConCallHandlingTarget::SendlResponse
// Called by client to send a response to accessory's operation after
// client has processed operation. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRemConCallHandlingTarget::SendResponse( 
											TRequestStatus& aStatus,
											TInt aOperationId, 
											TInt aError )	
	{
	SendAnyResponse( aStatus, aOperationId, aError );
	}									

// -----------------------------------------------------------------------------
// CRemConCallHandlingTarget::GetCommandSourceInfo
// Method to get the bearer source detail for the last command received
// -----------------------------------------------------------------------------
//
EXPORT_C void CRemConCallHandlingTarget::GetCommandSourceInfo( TRemConExtCmdSource& aSource )
  {
  COM_TRACE_( "[REMCONEXTAPI] CRemConCallHandlingTarget::GetCommandSourceInfo()" );
  GetUIDSource( iSourceUID, aSource );
  COM_TRACE_1( "[REMCONEXTAPI] CRemConCallHandlingTarget::GetCommandSourceInfo() - source [%d]", aSource );
  }

void CRemConCallHandlingTarget::GetUIDSource( TUint32 aUid, TRemConExtCmdSource& aSource )
  {
  COM_TRACE_( "[REMCONEXTAPI] CRemConCallHandlingTarget::GetUIDSource()" );
  
  switch ( aUid )
  {
  case KRemConExtBTAVRCPBearerID:
  case KRemConExtBTBearerID:
      {
      aSource = ERemConExtCmdSourceBluetooth;
      }
      break;
      
  case KRemConExtUSBBearerID:
  case KRemConExtWiredBearerID:
      {
      aSource = ERemConExtCmdSourceWired;
      }
      break;
      
  default:
      {
      aSource = ERemConExtCmdSourceUnKnown;
      }
      break;
  }
  
  COM_TRACE_1( "[REMCONEXTAPI] CRemConCallHandlingTarget::GetUIDSource() - return %d", aSource );
  }
  
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
