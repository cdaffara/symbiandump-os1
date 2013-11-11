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
#include <ecom/ecom.h>
#include "HWRMPluginHandler.h"
#include "HWRMClientServer.h"
#include "HWRMService.h"
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
_LIT( KPanicCategory, "HWRMPluginHandler" );

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMPluginHandler::CHWRMPluginHandler
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMPluginHandler::CHWRMPluginHandler(TInt aRequestTimeout)
    : iPlugin(NULL), 
    iTransIdCounter(0),
    iTransactionList(NULL),
    iPluginTimer(NULL),
    iRequestTimeout(aRequestTimeout)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::CHWRMPluginHandler()" ));
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::CHWRMPluginHandler - return " ));
    }

// -----------------------------------------------------------------------------
// CHWRMPluginHandler::ConstructL
// 2nd phase constructor gets plugin instance.
// -----------------------------------------------------------------------------
//
void CHWRMPluginHandler::ConstructL(const TDesC8& aMatch)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::ConstructL(<aMatch>)" ));

    // get plugin instance
    iPlugin = CHWRMPluginService::NewL(aMatch, this);
    iTransactionList = CHWRMPluginTransactionList::NewL();
    iPluginTimer = CHWRMGenericTimer::NewL(*this, iRequestTimeout, 0);

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::ConstructL - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMPluginHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
// 
CHWRMPluginHandler* CHWRMPluginHandler::NewL(const TDesC8& aMatch, TInt aRequestTimeout)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::NewL(<aMatch>)" ) );
    
    CHWRMPluginHandler* self = new( ELeave ) CHWRMPluginHandler(aRequestTimeout);
    
    CleanupStack::PushL( self );
    self->ConstructL(aMatch);
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginHandler::NewL - return 0x%x" ), self );

    return self;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMPluginHandler::~CHWRMPluginHandler()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::~CHWRMPluginHandler()" ) );
    
    delete iPluginTimer;

    if ( iPlugin )
        {
        // Cancel any ongoing requests
        while ( iTransactionList->GetFirstItem() )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginHandler::~CHWRMPluginHandler - Canceling transaction %d" ), iTransactionList->GetFirstItem()->iTransId );
            TRAPD(err, CancelCommandL(iTransactionList->GetFirstItem()->iTransId));
            if ( err != KErrNone )
                {
                COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginHandler::~CHWRMPluginHandler - Canceling transaction %d failed" ), iTransactionList->GetFirstItem()->iTransId );
                }
            }

        delete iPlugin;
        iPlugin = NULL;
        }
        
    // delete transaction list
    delete iTransactionList;

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::~CHWRMPluginHandler - return " ));
    }

// -----------------------------------------------------------------------------
// CHWRMPluginHandler::ProcessCommandL
// Handles plugin requests from sessions. 
// Only one concurrent request is supported.
// -----------------------------------------------------------------------------
//
TUint8 CHWRMPluginHandler::ProcessCommandL( TInt aCommandId,
                                           TDesC8& aData,
                                           CHWRMService* aCompletionCallback  )
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMPluginHandler::ProcessCommandL(0x%x, <aData>, 0x%x)" ), aCommandId, aCompletionCallback );

    __ASSERT_ALWAYS(iPlugin, User::Panic(KPanicCategory, EPanicBadHandle));

    // Generate new transaction ID.
    iTransIdCounter++; 
    if ( iTransIdCounter == 0 )
        {
        // Counter will overflow back to zero when it hits max.
        // However, zero indicates completed transaction as return value, so 
        // increase counter again.
        iTransIdCounter++; 
        }
    
    // If we run out of transIds, return server is busy.
    if ( iTransactionList->FindTransaction(iTransIdCounter, EFalse) )
        {
        User::Leave(KErrServerBusy);
        }

    // Create transaction data before call in case it leaves
    TTime obsoletionTime;
    obsoletionTime.UniversalTime();
    obsoletionTime += iRequestTimeout;
    THWRMTransactionData* data = new(ELeave) THWRMTransactionData(aCompletionCallback, iTransIdCounter, aCommandId, obsoletionTime);

    // Push transaction data to cleanup stack so that it will clean out if ProcessCommandL leaves.
    CleanupStack::PushL( data );
    
    iPlugin->ProcessCommandL( aCommandId, data->iTransId, aData );
    
    TUint8 retval(0);

    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );
    
    // Add data to list
    iTransactionList->AddTransaction( data );

    retval = data->iTransId;

    // Start  timer if it is not already started
    if ( !iPluginTimer->IsActive())
        {
        iPluginTimer->Set(iRequestTimeout);
        }
        
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginHandler::ProcessCommandL - return 0x%x" ), retval );         
    
    return retval;
        
    }
    
// -----------------------------------------------------------------------------
// CHWRMPluginHandler::CancelCommandL
// Cancels the currently executing request
// -----------------------------------------------------------------------------
//
void CHWRMPluginHandler::CancelCommandL( TUint8 aTransId )
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::CancelCommandL()" ));

    __ASSERT_ALWAYS(iPlugin, User::Panic(KPanicCategory, EPanicBadHandle));
    
    // Find correct transaction data and remove it from list
   THWRMTransactionData* data = static_cast<THWRMTransactionData*>(iTransactionList->FindTransaction(aTransId, ETrue));
   
    // cancel request timer if no more transactions open
    if ( iTransactionList->Count() == 0 )
        {
        iPluginTimer->Cancel();
        }

    // If transaction is not open, do nothing
    // Do not cancel if request has no callback (i.e. final destructor state restorings)
    // (these are canceled by timeout if they do not complete successfully)
    if ( data && data->iCompletionCallback )
        {            
        CleanupStack::PushL( data );
        
        iPlugin->CancelCommandL( data->iTransId, data->iCommandId );
        
        // Destroy the transaction data, since transaction is over.
        CleanupStack::PopAndDestroy( data );
        data = NULL;
        }
    else 
    	{
		TTime now;
		now.UniversalTime();    	
    	if ( data && data->iObsoletionTime >= now )
            {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::CancelCommandL - Not cancelled because no callback" ) );
            
            // Push data back to list
            iTransactionList->AddTransaction( data );
            }
       	else
       		{
       		COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::CancelCommandL - Cancelled because command was obsolete" ) );
       		}
        }
        
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::CancelCommandL - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMPluginHandler::ProcessResponseL
// Routes response from plugin to correct service instance
// -----------------------------------------------------------------------------
//
void CHWRMPluginHandler::ProcessResponseL( TInt aCommandId,
                                           TUint8 aTransId,
                                           TDesC8& aData  )
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMPluginHandler::ProcessResponseL(0x%x, 0x%x, <aData> )" ), aCommandId, aTransId );

    // Find and remove correct transaction data and remove it from queue
    THWRMTransactionData* data = static_cast<THWRMTransactionData*>(iTransactionList->FindTransaction(aTransId, ETrue));
    
    // cancel request timer if no more transactions open
    if ( iTransactionList->Count() == 0 )
        {
        iPluginTimer->Cancel();
        }

    // If transaction is not open, response not expected.
    if ( data )
        {
        CleanupStack::PushL( data );

        // Check that command ID is the expected one        
        if ( data->iCommandId != aCommandId )
            {
            COMPONENT_TRACE3(_L( "HWRM Server - CHWRMPluginHandler::ProcessResponseL - Command ID mismatch, expected: 0x%x, got 0x%x" ), data->iCommandId, aCommandId );
            User::Leave(KErrBadHandle);
            }

        // Route data to callback service if one is needed
        if ( data->iCompletionCallback )
            {            
            data->iCompletionCallback->ProcessResponseL(aCommandId, aTransId, aData, EFalse); 
            }
        
        CleanupStack::PopAndDestroy( data );
        data = NULL;
        }
    else
        {
        // There is problem in adaptation, as unexpected transaction was completed.
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::ProcessResponseL - No transaction data found!" ) );
        User::Leave(KErrBadHandle);
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::ProcessResponseL - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMPluginHandler::EventL
// Routes indication from plugin to correct indication handler
// -----------------------------------------------------------------------------
//
void CHWRMPluginHandler::EventL( const TUint32 aIndId, TDesC8& aData )
	{	
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPluginHandler::EventL(), Count() = %d" ), iIndicationCallbacks.Count() );		
				
	for (TUint i=0; i<iIndicationCallbacks.Count(); i++)
		{
		MHWRMIndicationHandler* pHandler = iIndicationCallbacks[i];
				
		COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPluginHandler::EventL(), callback %d" ), i );					
		pHandler->ProcessIndicationL(aIndId, aData);
		}
	}

// -----------------------------------------------------------------------------
// CHWRMPluginHandler::RegisterForIndications
// Registers a handler for receiving HWRM plug-in indications
// -----------------------------------------------------------------------------
//
void CHWRMPluginHandler::RegisterForIndicationsL(MHWRMIndicationHandler* aCallback)
	{
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPluginHandler::RegisterForIndicationsL(), Count() = %d" ), iIndicationCallbacks.Count() );

	iIndicationCallbacks.AppendL(aCallback);
	}

// -----------------------------------------------------------------------------
// CHWRMPluginHandler::DeregisterForIndications
// Deregisters handlers for receiving HWRM plug-in indications
// -----------------------------------------------------------------------------
//
void CHWRMPluginHandler::DeregisterForIndications(MHWRMIndicationHandler* aCallback)	
	{
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPluginHandler::DeregisterForIndications(), Count() = %d" ), iIndicationCallbacks.Count() );

	TInt findErr = iIndicationCallbacks.Find(aCallback);
	COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPluginHandler::DeregisterForIndications(), findErr = %d" ), findErr );
	
	if ( findErr != KErrNotFound )
		{
		iIndicationCallbacks.Remove(findErr);
		COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPluginHandler::DeregisterForIndications(), item removed, Count() = %d" ), iIndicationCallbacks.Count() );
		}
	}

// -----------------------------------------------------------------------------
// CHWRMPluginHandler::GenericTimerFired
// Cancels all obsolete transactions. TimerId is irrelevant as only one timer.
// -----------------------------------------------------------------------------
//
void CHWRMPluginHandler::GenericTimerFired(TInt /*aTimerId*/, TBool /*aCutOff*/)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::GenericTimerFired()" ) );

    __ASSERT_ALWAYS(iPlugin, User::Panic(KPanicCategory, EPanicBadHandle));

    // Since requests are added to end of list always, they are in order of obsolescense.
    // Cancel requests that are obsolete and notify callback service
    TInt err(KErrNone);
    TTime now;
    now.UniversalTime();
    THWRMTransactionData* data = static_cast<THWRMTransactionData*>(iTransactionList->GetFirstItem());
    
    while ( data && ( data->iObsoletionTime < now ) )	
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginHandler::GenericTimerFired - Canceling obsolete transaction 0x%x" ), data->iTransId );

        // Cancel transaction. 
        TRAP(err, iPlugin->CancelCommandL( data->iTransId, data->iCommandId ));
        if ( err != KErrNone )
            {
            // Ignore errors as we cannot do anything about it anyway. Just trace.
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginHandler::GenericTimerFired - Canceling obsolete transaction FAILED (%d)!" ), err );
            }
            
        // Notify service that request was canceled by timeout
        if ( data->iCompletionCallback )
            {
            TBuf8<1> emptyDes;
            TRAP(err, data->iCompletionCallback->ProcessResponseL(data->iCommandId, data->iTransId, emptyDes, ETrue)); 
            if ( err != KErrNone )
                {
                // Ignore errors as we cannot do anything about it anyway. Just trace.
                COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPluginHandler::GenericTimerFired - Notifying obsolete transaction cancel FAILED (%d)!" ), err );
                }
            }
        
        // Destroy the transaction data, since transaction is over.
        iTransactionList->RemoveFirstItem();
        delete data;
        data = NULL;
        
        // get next data to check
        data = static_cast<THWRMTransactionData*>(iTransactionList->GetFirstItem());
        }

    // Restart timer if there is more transactions in list
    if ( data )
        {
        iPluginTimer->Set(iRequestTimeout);
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPluginHandler::GenericTimerFired - return" ) );
    }
    

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
