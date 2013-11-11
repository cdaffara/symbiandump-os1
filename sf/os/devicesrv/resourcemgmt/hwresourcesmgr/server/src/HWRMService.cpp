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
#include "HWRMService.h"
#include "HWRMPluginHandler.h"
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
// CHWRMService::BaseConstructL
// Symbian 2nd phase constructor can leave.
// This method must be called within NewL of the subclass.
// -----------------------------------------------------------------------------
//
void CHWRMService::BaseConstructL(CHWRMPluginHandler* aPluginHandler,
                                  CHWRMReservationHandler* aReservationHandler)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMService::BaseConstructL(0x%x, 0x%x)" ), aPluginHandler, aReservationHandler ) ;

    if ( !aPluginHandler )
        {
        User::Leave(KErrBadHandle);
        }
        
    iPluginHandler = aPluginHandler;
    
    iTransactionList = CHWRMPluginTransactionList::NewL();
    
    iReservationHandler = aReservationHandler;

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMService::BaseConstructL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMService::BaseDestruct
// Cleans up base class.
// -----------------------------------------------------------------------------
//
void CHWRMService::BaseDestruct()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMService::BaseDestruct()" ) );

    // No need to destroy iPluginHandler or iReservationHandler, not owned
    
    // Complete any pending requests
    while ( iTransactionList->GetFirstItem() )
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMService::BaseDestruct - Deleting request %d" ), iTransactionList->GetFirstItem()->iTransId );
        THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->RemoveFirstItem());

        if (data->iRequestMessage.Handle())
            {
            // Check that this request is not first one of a split request
            if ( !data->iSplitCommand || !CheckForMessage(data->iRequestMessage.Handle()) )
                {
                COMPONENT_TRACE1(_L( "HWRM Server - CHWRMService::BaseDestruct() - Canceling pending message" ) );
                data->iRequestMessage.Complete(KErrCancel);
                }
            else
                {
                COMPONENT_TRACE1(_L( "HWRM Server - CHWRMService::BaseDestruct() - Split request, not canceling message yet" ) );
                }
            }

        TRAPD(err, iPluginHandler->CancelCommandL(data->iTransId));
        
        if ( err != KErrNone )
            {
            COMPONENT_TRACE3(_L( "HWRM Server - CHWRMService::BaseDestruct - Cancelling Command (transid: %d) failed: %d" ), data->iTransId, err );
            }

        delete data;
        }
        
    // Destroy transaction list
    delete iTransactionList;
    
    // NULL things for completeness (and PC-lint)
    iTransactionList    = NULL;
    iPluginHandler      = NULL;
    iReservationHandler = NULL;
        
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMService::BaseDestruct - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMService::CompleteRequest
// Completes request. Subclass calls this from ProcessResponseL
// -----------------------------------------------------------------------------
//
TInt CHWRMService::CompleteRequest(TUint8 aTransId, TInt aCompletionCode)
    {
    return CompleteRequest(aTransId, aCompletionCode, iTransactionList, ETrue);
    }

// -----------------------------------------------------------------------------
// CHWRMService::CompleteRequest
// Completes request. Subclass calls this from ProcessResponseL
// -----------------------------------------------------------------------------
//
TInt CHWRMService::CompleteRequest(TUint8 aTransId, TInt aCompletionCode, 
    CHWRMPluginTransactionList* aTransactionList, TBool aCompleteMsg)
    {
    // Handle message completion
    TInt contextErr(KErrNone);
    
    THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(
                                            aTransactionList->FindTransaction( aTransId, ETrue ) );
    
    if ( data )
        {        
        // Message can be completed, if there is one pending          
        if ( data->iRequestMessage.Handle() )
            {
            // Check that this request is not first one of a split request
            if ( aCompleteMsg &&
                 ( !data->iSplitCommand || !CheckForMessage( data->iRequestMessage.Handle() ) ) )
                {
                COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMService::CompleteRequest - Completing pending message" ) ) );
                data->iRequestMessage.Complete(aCompletionCode);
                }
            else
                {
                COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMService::CompleteRequest - Split request, not completing message yet" ) ) );            
                }
            }
        else
            {
            // No else, message has already been completed. 
            // This happens when command was requested by timer.
            COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMService::CompleteRequest - No message to complete" ) ) );
            }
        
        // Cleanup data
        if(data->iSplitCommandData)
            {
            COMPONENT_TRACE3( ( _L( "HWRM Server - CHWRMService::CompleteRequest Deleting iSplitCommandData(0x%x) for Id(0x%x)" ) ), data->iSplitCommandData, data->iSplitCommandId );
            delete data->iSplitCommandData;
            }
        delete data;
        data = NULL;
        }
    else
        {
        // Unexpected completion, something is amiss.
        COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMService::CompleteRequest - No related transaction data found!" ) ) );
        contextErr = KErrBadHandle;
        }    

    return contextErr;
    }


// -----------------------------------------------------------------------------
// CHWRMService::CheckForMessage
// Checks transaction list if specified message is in any transaction.
// -----------------------------------------------------------------------------
//
TBool CHWRMService::CheckForMessage(TInt aHandle)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMService::CheckForMessage - Checking for message: 0x%x" ), aHandle );
    
    THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->GetFirstItem());
    TBool retval(EFalse);
    
    while ( !retval && data )
        {
        if ( data->iRequestMessage.Handle() == aHandle )
            {
            retval = ETrue;
            }
        data = static_cast<THWRMPluginRequestData*>(data->iNextData);
        }
        
    return retval;
    }
        
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
