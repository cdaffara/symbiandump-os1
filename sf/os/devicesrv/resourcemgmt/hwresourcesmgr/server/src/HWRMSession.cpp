// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include "HWRMSession.h"
#include "HWRMServer.h"
#include "HWRMClient.h"  // THWRMResourceType
#include "HWRMClientServer.h"
#include "HWRMService.h"
#include "HWRMVibraService.h"
#include "HWRMVibraCommonService.h"
#include "HWRMLightService.h"
#include "HWRMPowerService.h"
#include "HWRMFmTxService.h"
#include "HWRMtrace.h"


// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMSession::CHWRMSession
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMSession::CHWRMSession(CHWRMServer& aServer)
    : iHWRMServer(aServer),
    iService(NULL),
    iCompletionNeeded(EFalse)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMSession::CHWRMSession()" ) );
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMSession::CHWRMSession - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMSession* CHWRMSession::NewL(CHWRMServer& aServer)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMSession::NewL()" ) );

    CHWRMSession* self = new( ELeave ) CHWRMSession(aServer);
    
    // No ConstructL as nothing to do there

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMSession::NewL - return 0x%x" ), self );

    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMSession::~CHWRMSession()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMSession::~CHWRMSession()" ) );

    // iHWRMServer not owned and therefore not cleaned

	delete iService;
	iService = NULL;

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMSession::~CHWRMSession - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMSession::ServiceL
// Calls request handling functions. Also traps any leaves and signals client if
// error occurs.
// -----------------------------------------------------------------------------
//
void CHWRMSession::ServiceL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMSession::ServiceL(0x%x)" ), aMessage.Handle() );

#if defined(COMPONENT_TRACE_FLAG) && defined(_DEBUG)
    // check memory usage
    TInt biggestBlock;
    COMPONENT_TRACE2(_L( "HWRM Server - #### Memory Available in Heap: %6d ####" ), User::Heap().Available(biggestBlock) );
    COMPONENT_TRACE2(_L( "HWRM Server - #### Biggest block:            %6d ####" ), biggestBlock);
#endif

#ifdef HWRM_TEST_IGNORE_CLIENTS
    // Just complete calls that come from other parts of system than module test application
    TSecureId* sid = new (ELeave) TSecureId(0x33442255);
    if ( aMessage.SecureId().iId != sid->iId )
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMSession::ServiceL - Ignoring call from 0x%x" ), aMessage.SecureId().iId);
        aMessage.Complete(KErrNone);
        delete sid;
        return;
        }
    delete sid;
#endif

    // Reset completion need flag
    iCompletionNeeded = ETrue;
    
    TRAPD( error, DispatchMessageL( aMessage ));

    if ( error != KErrNone )
        {
        COMPONENT_TRACE2(_L("HWRM Server - CHWRMSession::ServiceL(): Error in DispatchMessageL: %d"), error );
        iCompletionNeeded = ETrue;
        }

    if ( iCompletionNeeded )
        {
        aMessage.Complete(error);
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMSession::ServiceL - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMSession::DispatchMessageL
// Calls matching function of CWalletTaskOrganizer for handling the request.
// -----------------------------------------------------------------------------
//
void CHWRMSession::DispatchMessageL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMSession::DispatchMessageL(0x%x), client SID: 0x%x" ), aMessage.Function(), aMessage.SecureId().iId );

    // Check command code and call appropriate function
    switch ( aMessage.Function() )
        {
    	case EHWRMCreateVibraService:
    	    {
    	    if ( iService )
    	        {
    	        User::Leave(KErrAlreadyExists);
    	        }
    	    else
    	        {
    	        CHWRMPluginHandler* pluginHandler = iHWRMServer.GetPluginHandler(HWRMResourceTypeVibra);
    	        CHWRMReservationHandler* reservationHandler = iHWRMServer.GetReservationHandler(HWRMResourceTypeVibra);
    	        CHWRMVibraCommonService* commonService = iHWRMServer.GetVibraCommonService();
    	        CHWRMVibraCommonData* vibraCommonData = iHWRMServer.GetVibraCommonData();
    	        
        	    // if handler or common data is NULL, this resource is not supported by device
        	    if ( !pluginHandler || !reservationHandler || !vibraCommonData )
        	        {
        	        User::Leave(KErrNotSupported);
        	        }
        	    else
        	        {        	        
        	        iService = static_cast<CHWRMService*>
                        (CHWRMVibraService::NewL(pluginHandler, 
                        reservationHandler, *commonService, *vibraCommonData, 
                        vibraCommonData->IsPrivilegedSid(aMessage.SecureId())));
        	        }
    	        }
    		break;
    	    }
    	case EHWRMCreateLightService:
    	    {
    	    if ( iService )
    	        {
    	        User::Leave(KErrAlreadyExists);
    	        }
    	    else
    	        {
    	        CHWRMPluginHandler* pluginHandler = iHWRMServer.GetPluginHandler(HWRMResourceTypeLight);
    	        CHWRMReservationHandler* reservationHandler = iHWRMServer.GetReservationHandler(HWRMResourceTypeLight);
        	    
    	        CHWRMLightCommonData* lightCommonData = iHWRMServer.GetLightCommonData();
        	    
        	    // if handler is NULL, this resource is not supported by device
        	    if ( !pluginHandler|| !reservationHandler || !lightCommonData)
        	        {
        	        User::Leave(KErrNotSupported);
        	        }
        	    else
        	        {
        	        iService = static_cast<CHWRMService*>(CHWRMLightService::NewL(pluginHandler, reservationHandler, *lightCommonData));
        	        }
    	        }
    		break;
    	    }
    	    
    	case EHWRMCreatePowerService:
    	    {
    	    if ( iService )
    	        {
    	        User::Leave(KErrAlreadyExists);
    	        }
    	    else
    	        {
    	        CHWRMPluginHandler* pluginHandler = iHWRMServer.GetPluginHandler(HWRMResourceTypePower);
        	    
    	        CHWRMPowerCommonData* powerCommonData = iHWRMServer.GetPowerCommonData();
        	    
        	    // if handler is NULL, this resource is not supported by device
        	    if ( !pluginHandler || !powerCommonData)
        	        {
        	        User::Leave(KErrNotSupported);
        	        }
        	    else
        	        {
        	        iService = static_cast<CHWRMService*>(CHWRMPowerService::NewL(pluginHandler, *powerCommonData));
        	        }
    	        }
    		break;
    	    }
    	    
    	case EHWRMCreateFmTxService:
    	    {
    	    if ( iService )
    	        {
    	        User::Leave(KErrAlreadyExists);
    	        }
    	    else
    	        {
                CHWRMPluginHandler* pluginHandler = iHWRMServer.GetPluginHandler( 
                                                                    HWRMResourceTypeFmTx );
                CHWRMPluginHandler* watcherPluginHandler = iHWRMServer.GetPluginHandler( 
                                                                    HWRMResourceTypeFmTxWatcher );
      	        CHWRMReservationHandler* reservationHandler = iHWRMServer.GetReservationHandler( 
                                                                    HWRMResourceTypeFmTx);
  	        
    	        CHWRMFmTxCommonData* fmTxCommonData = iHWRMServer.GetFmTxCommonData();
    	      	        
        	    // if handler or common data is NULL, this resource is not supported by device
        	    if ( !pluginHandler|| !watcherPluginHandler || !reservationHandler || !fmTxCommonData)
        	        {
        	        User::Leave(KErrNotSupported);
        	        }
        	    else
        	        {
        	        iService = static_cast<CHWRMService*>(CHWRMFmTxService::NewL(pluginHandler,
        	        															 *watcherPluginHandler,
        	        															 reservationHandler,
        	        															 *fmTxCommonData,
        	        															 *(fmTxCommonData->GetRdsTextConverter())) );
        	        }
    	        }
    		break;
    	    }

        // Cannot identify the message, it must be for service.
        // If no service yet established, panic.
        default:
            {
            if ( iService )
                {
                iCompletionNeeded = iService->ExecuteMessageL(aMessage);
                }
            else
                {
                User::Leave(KErrBadHandle);                
                }
           
            break;
            }
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMSession::DispatchMessageL - return" ));
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
