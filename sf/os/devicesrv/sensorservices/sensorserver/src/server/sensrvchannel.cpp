/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor server channel implementation
*
*/



#include <centralrepository.h>
#include "sensrvdefines.h"
#include "senserverchannel.h"
#include "sensrvchannelbuffer.h"
#include "sensrvtransaction.h"
#include "sensrvclientserver.h"
#include "sensrvpluginproxy.h"
#include "sensrvtransactionqueue.h"
#include "sensrvssymediator.h"
#include "sensrvsession.h"
#include "sensrvchannellistener.h"
#include "sensrvservermediator.h"
#include "sensrvtransactionmonitor.h"
#include "sensrvproxymanager.h"
#include "sensrvpropertyqueue.h"
#include "sensrvconditionevaluator.h"
#include "sensrvprivatecrkeys.h"

// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvChannel* CSensrvChannel::NewL( const TSensrvResourceChannelInfo& aInfo,
									  CSensrvPluginProxy& aProxy )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::NewL(aInfo.iChannelId: %d, aProxy: 0x%x)" ), aInfo.iChannelId, aProxy.ImplementationUid().iUid ) );

    CSensrvChannel* self = new( ELeave ) CSensrvChannel( aInfo, aProxy );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvChannel::CSensrvChannel( const TSensrvChannelInfo& aInfo,
                                CSensrvPluginProxy& aProxy )
    : iId( aInfo.iChannelId ),
      iState( EChannelStateClosed ),
      iProxy( aProxy ),
      iChannelInfo( aInfo ),
      iHighestListenerPriority( KMinTInt )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CSensrvChannel()" ) ) );

    // Nothing to do

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CSensrvChannel - return" ) ) );
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvChannel::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ConstructL()" ) ) );

		// coverity[SIZECHECK]
    iChannelQueue = CSensrvTransactionQueue::NewL( ETrue );

    iChannelInfo.iChannelGroup = TSensrvResourceChannelInfo::ESensrvChannelGroupNotSolved;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvChannel::~CSensrvChannel()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::~CSensrvChannel()" ) ) );

    TInt count( iListenerList.Count() );
    for ( TInt i=0; i < count; i++ )
        {
        delete iListenerList[ i ];
        }

    iListenerList.Reset();

    // Remove the first transactions in channel queue from mediator queues and
    // transaction monitor, too. Rest of the queued transactions cannot be
    // queued anywhere else, so no need to check them.
    CSensrvTransaction* firstTransaction = NULL;
    if ( iChannelQueue )
        {
        firstTransaction = iChannelQueue->First();
        }

    if ( iProxy.ServerMediator() )
        {
        iProxy.ServerMediator()->RemoveSingleTransaction( firstTransaction );
        }
    if ( iProxy.SsyMediator() )
        {
        iProxy.SsyMediator()->RemoveSingleTransaction( firstTransaction );
        }
    if ( iProxy.TransactionMonitor() )
        {
        iProxy.TransactionMonitor()->RemoveTransaction( firstTransaction );
        }

    delete iChannelQueue;
    delete iChannelBuffer;
    delete iDataAvailableTransaction;
    delete iForceBufferFilledTransaction;
    delete iForceChannelCloseTransaction;
    delete iPropertyChangedTransaction;
    delete iConditionEvaluator;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::~CSensrvChannel - return" ) ) );
    }


// -----------------------------------------------------------------------------
// Handles message according to message type
// -----------------------------------------------------------------------------
//
void CSensrvChannel::DispatchMessage( CSensrvMessage& aMessage )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::DispatchMessage(aMessage.Function(): %d)" ), aMessage.Function() ) );

    TInt err( KErrNone );
    CSensrvTransaction* transaction = NULL;
    CSensrvTransaction::TSensrvTransactionType transType( CSensrvTransaction::ETransTypeNone );
    
    // Check command code and call appropriate function
    switch ( aMessage.Function() )
        {
        case ESensrvSrvReqOpenChannel:
            {
            transType = CSensrvTransaction::ETransTypeOpenChannel ;
            break;
            }

        case ESensrvSrvReqCloseChannel:
            {
            transType = CSensrvTransaction::ETransTypeCloseChannel ;
            break;
            }

        case ESensrvSrvReqStartListening:
            {
            transType = CSensrvTransaction::ETransTypeStartListening ;
            break;
            }

        case ESensrvSrvReqStopListening:
            {
            transType = CSensrvTransaction::ETransTypeStopListening ;
            break;
            }

        case ESensrvSrvReqAsyncChannelData:
        case ESensrvSrvReqAsyncPropertyData:
        case ESensrvSrvReqStopPropertyListening:
        case ESensrvSrvReqAddConditionSet:
        case ESensrvSrvReqRemoveConditionSet:
        case ESensrvSrvReqAsyncConditionMet:
            {
            // Get correct listener and pass message there
            CSensrvChannelListener* listener = GetListener( aMessage.Session() );

            if ( listener )
                {
                listener->DispatchMessage( aMessage );
                }
            else
                {
                err = KErrNotFound;
                aMessage.Complete( err );
                }

            break;
            }

        case ESensrvSrvReqGetProperty:
            {
            transType = CSensrvTransaction::ETransTypeGetProperty ;
            break;
            }

        case ESensrvSrvReqSetProperty:
            {
            transType = CSensrvTransaction::ETransTypeSetProperty ;
            break;
            }

        case ESensrvSrvReqGetAllProperties:
            {
            transType = CSensrvTransaction::ETransTypeGetAllProperties ;
            break;
            }
            
        case ESensrvSrvReqStartConditionListening:
            {
            transType = CSensrvTransaction::ETransTypeStartConditionListening ;
            break;
            }
            
        case ESensrvSrvReqStopConditionListening:
            {
            transType = CSensrvTransaction::ETransTypeStopConditionListening ;
            break;
            }

        // Cannot identify the message
        default:
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::DispatchMessage - ERROR: unknown command" )) );
            aMessage.Complete( KErrArgument );
            break;
            }
        }
    //Construct Transaction
    if( transType != CSensrvTransaction::ETransTypeNone )
        {
        TRAP( err, transaction = CSensrvTransaction::NewL(
                    &aMessage,
                    &iProxy,
                    this,
                    transType) );
        
        // Queue transaction
        if ( transaction && err == KErrNone )
            {
            transaction->SetState( CSensrvTransaction::ETransStateQueued );
            err = iChannelQueue->Add( transaction );
            }
        }
    // Handle error
    if ( err != KErrNone )
        {
        if ( transaction )
            {
            transaction->SetErrorCode( err );
            transaction->Complete();
            delete transaction;
            transaction = NULL;
            }
        else
            {
            // Do nothing, message gets deleted and completed with error
            // on transaction cleanup in cases where transaction creation failed.
            }
        }
    else
        {
        HandleNextTransaction();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::DispatchMessage - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Channel opened by SSY, check that providers are correctly set and
// complete the transaction.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::ChannelOpened(TInt aErrorCode,
                                   MSsyChannelDataProvider* aDataProvider,
                                   MSsyPropertyProvider* aSensorPropertyProvider)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ChannelOpened(aErrorCode: %d)" ), aErrorCode ) );

    TInt err( aErrorCode );
    TInt propErr( KErrNone );

    // If no error, set state to open, if not supported, need to solve dependent channels later
    if ( err == KErrNone || err == KErrNotSupported )
        {
        __ASSERT_ALWAYS( aDataProvider && aSensorPropertyProvider,
                         User::Panic( KSensrvPanicCategory, ESensrvPanicNullProvider ) );
        iDataProvider = aDataProvider;
        iPropertyProvider = aSensorPropertyProvider;

        // Get data rate for determining maximum buffering count
        TSensrvProperty property( KSensrvPropIdDataRate, KSensrvItemIndexNone, 0 );
        TRAP(propErr, iPropertyProvider->GetPropertyL( iId, property ) );

        if ( propErr == KErrNone )
            {
            TInt maxRate( 0 );

            if ( ESensrvArrayPropertyInfo == property.GetArrayIndex() )
                {
                //Array property
                TInt maxValueIndex( 0 );
                property.GetMaxValue( maxValueIndex );
                property.SetItemIndex( KSensrvItemIndexNone ); //Initialize
                property.SetArrayIndex( maxValueIndex ); //Set array index for filter criteria

                TRAP( propErr, iPropertyProvider->GetPropertyL( iId, property ) );
                property.GetValue( maxRate );
                }
            else
                {
                property.GetMaxValue( maxRate );
                }

            if ( propErr == KErrNone )
                {
                iMaxBufferingCount = iProxy.ProxyManager().BufferSizeMultiplier()
                                     * maxRate
                                     / KSensrvDataBufferLengthMultiplier;

                // Very small buffers still need buffering count of at least one
                if ( iMaxBufferingCount < KSensrvMinimumBufferingCount )
                    {
                    iMaxBufferingCount = KSensrvMinimumBufferingCount;
                    }
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::ChannelOpened - ERROR: Unable to get maximum data rate from array, using default buffer max size. Error: %d" ), propErr ) );
                iMaxBufferingCount = KSensrvDefaultMaxBufferingCount;
                }
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::ChannelOpened - ERROR: Unable to get maximum data rate, using default buffer max size. Error: %d" ), propErr ) );
            iMaxBufferingCount = KSensrvDefaultMaxBufferingCount;
            }

        iState = EChannelStateOpen;
        }

    CSensrvTransaction* transaction = iChannelQueue->First();

    // Unexpected callbacks from SSY mean that executing transaction has been cleaned,
    // so we should not set the error code or notify server mediator
    if (   transaction
       && transaction->Type() == CSensrvTransaction::ETransTypeOpenChannel
       && transaction->State() == CSensrvTransaction::ETransStateHandledAtSsy )
        {
        if ( err != KErrNone && err != KErrNotSupported )
            {
            transaction->SetErrorCode( err );
            }

        // Notify server thread that transaction is complete
        // Error can be ignored, since we are at SSY thread
        iProxy.ServerMediator()->Notify( transaction );
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ChannelOpened - Transaction cleaned already" ) ) );
        }


    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ChannelOpened - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Channel closed by SSY. Cleanup channel and complete the transaction.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::ChannelClosed()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ChannelClosed()" ) ) );

    iState = EChannelStateClosed;

    CSensrvTransaction* transaction = iChannelQueue->First();

    // Unexpected callbacks from SSY mean transaction has been cleaned, so we should not
    // notify server mediator
    if (  transaction
       && transaction->Type()  == CSensrvTransaction::ETransTypeCloseChannel
       && transaction->State() == CSensrvTransaction::ETransStateHandledAtSsy )
        {
        // Notify server thread that transaction is complete
        // Error can be ignored, since we are at SSY thread
        iProxy.ServerMediator()->Notify( transaction );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ChannelClosed - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Availability has changed. Handle it.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::HandleAvailabilityChange( const TSensrvProperty& aProperty )
	{
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleAvailabilityChange()" ) ) );

	TInt propValue;

	aProperty.GetValue( propValue );

	HandleAvailabilityChange( static_cast<TSensrvAvailability>( propValue ) );

	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleAvailabilityChange - return" ) ) );
	}

// ---------------------------------------------------------------------------
// Availability has changed. Handle it.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::HandleAvailabilityChange( const TSensrvAvailability& value )
	{
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleAvailabilityChange()" ) ) );

	switch( value )
		{
		// Channel level transactions handled here
		case ESensrvAvailabilityFalse:
			COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleAvailabilityChange - Availability changed to ESensrvAvailabilityFalse" ) ) );
			SetChannelUnavailable();
			break;

		case ESensrvAvailabilityTrue:
			COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleAvailabilityChange - Availability changed to ESensrvAvailabilityTrue" ) ) );
			SetChannelAvailable();
			break;
		
		default:
		    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleAvailabilityChange - Incorrect value received for Availability, Value ignored" ) ) );
		    break;
		}

	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleAvailabilityChange - return" ) ) );
	}

// ---------------------------------------------------------------------------
// Availability has changed. Handle it.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::SetChannelAvailable( )
	{
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelAvailable()" ) ) );
	//start channel data
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelAvailable - return" ) ) );
	}


// ---------------------------------------------------------------------------
// Availability has changed. Handle it.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::SetChannelUnavailable( )
	{
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelUnavailable()" ) ) );
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelUnavailable - return" ) ) );
	}

// ---------------------------------------------------------------------------
// Property has changed. Store changed property to property queue.
// ---------------------------------------------------------------------------
//
TBool CSensrvChannel::StoreChangedProperty( const TSensrvProperty& aProperty,
                                            const CSensrvSession* aIgnoreSession )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::StoreChangedProperty(aProperty.GetPropertyId() 0x%x, aIgnoreSession: 0x%x)" ), aProperty.GetPropertyId(), aIgnoreSession ) );

	TBool retval( EFalse );
	TInt propertyId = aProperty.GetPropertyId();

	if ( ( propertyId == KSensrvPropIdAvailability ) )
		{
		COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::StoreChangedProperty - server property changed: %d" ), propertyId ) );

		HandleAvailabilityChange( aProperty );
		}

	TInt i( iListenerList.Count() );
	TInt err( KErrNone );

	for (i--; i >= 0; i--)
		{
		if ( &( iListenerList[ i ]->Session()) != aIgnoreSession )
			{
			err = iListenerList[ i ]->StoreChangedProperty( aProperty );

			if ( err != KErrNone )
				{
				// Probably OOM. Not critical to lose property change notification.
				ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::StoreChangedProperty - ERROR: Error while adding property change to listener queue: %d" ), err ) );
				}
			else
				{
				retval = ETrue;
				}
			}
		}

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::StoreChangedProperty - return: %d" ), retval ) );

    return retval;
    }

// ---------------------------------------------------------------------------
// Handles transaction finalization and completes transaction on channels's queue.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::CompleteTransaction( CSensrvTransaction* aTransaction )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CompleteTransaction(aTransaction: 0x%x)" ), aTransaction ) );

    __ASSERT_ALWAYS( aTransaction, User::Panic( KSensrvPanicCategory, ESensrvPanicNullTransaction ) );

    TInt err = aTransaction->ErrorCode();

    switch( aTransaction->Type() )
        {
        // Channel level transactions handled here
        case CSensrvTransaction::ETransTypeOpenChannel:
            {
            // If opening failed at SSY, no need to create buffer
            if ( iState != EChannelStateClosed )
                {
                if ( !iChannelBuffer )
                    {
                    TRAP( err, iChannelBuffer =
                        CSensrvChannelBuffer::NewL( iChannelInfo.iDataItemSize,
                                                   iMaxBufferingCount * KSensrvDataBufferLengthMultiplier ) )

                    if ( err == KErrNone)
                        {
                        // Write maximum buffering count and data item size to message
                        CSensrvTransaction::TChannelOpenMessageData data( iMaxBufferingCount, iChannelInfo.iDataItemSize );
                        err = aTransaction->SetMessageData( &data );
                        }
                    else
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::CompleteTransaction - ERROR: Buffer creation failed: %d" ), err ) );
                        }
                    }
                }

            if ( err == KErrNone )
                {
                // Update indicator values on open and notify changes to clients

                HandleDependencyOnOpen();

                HandleSetPropertySuccessIndOnCreateOrDelete();
                }
            else
                {
                // If there was error, delete the listener
                aTransaction->SetErrorCode(err);
                DeleteListener( aTransaction->Session() );
                CloseIfNeeded();
                }
            }
            break;

        case CSensrvTransaction::ETransTypeCloseChannel:
            {
            // If an error occurred, channel was left open, so only delete buffer
            // and condition evaluator if channel state is closed.
            if ( iState == EChannelStateClosed )
                {
                delete iChannelBuffer;
                iChannelBuffer = NULL;

                delete iConditionEvaluator;
                iConditionEvaluator = NULL;
                }

            // Client listener is already deleted successfully, so report ok to client
            // even if there was an error.
            aTransaction->SetErrorCode( KErrNone );
            }
            break;

        case CSensrvTransaction::ETransTypeStartListening:
        case CSensrvTransaction::ETransTypeStartConditionListening:
            {
            if ( err != KErrNone )
                {
                CSensrvChannelListener* listener = GetListener(aTransaction->Session());

                // If error at SSY, stop listening
                if ( listener )
                    {
                    CSensrvChannelListener::TSensrvChannelListenerState stopType =
                        aTransaction->Type() == CSensrvTransaction::ETransTypeStartListening ?
                             CSensrvChannelListener::EChannelListenerStateDataListening :
                             CSensrvChannelListener::EChannelListenerStateConditionListening;

                    // Stop listening that was started, but do not delete any
                    // added conditions, as client will expect them to be added if he retries.
                    listener->StopListening(stopType, EFalse);
                    }
                else
                    {
                    // No listener found, we may have to close
                    CloseIfNeeded();
                    }
                }
            else
                {
                iState = EChannelStateListening;
                }
            }
            break;

        case CSensrvTransaction::ETransTypeStopListening:
        case CSensrvTransaction::ETransTypeStopConditionListening:
            {
            if ( err != KErrNone )
                {
                // If an error occurred, assume channel was left active still.
                // However, client listener state has already been set to not-active,
                // so report ok to client.
                aTransaction->SetErrorCode( KErrNone );
                }
            else
                {
                iState = EChannelStateOpen;
                }
            }
            break;

        case CSensrvTransaction::ETransTypeMediatorNewDataAvailable:
            {
            // Remove force transaction from monitor
            if ( iForcingBufferFilled )
                {
                iForcingBufferFilled = EFalse;
                iProxy.TransactionMonitor()->RemoveTransaction( iForceBufferFilledTransaction );
                }

            // Notify each listener that new data is available
            TInt i( iListenerList.Count() );
            for ( i--; i >= 0; i-- )
                {
                err = iListenerList[ i ]->NewDataAvailable();

                // Delete any listeners who cannot handle new data, as most likely the message
                // descriptors for writing are invalid.
                if (err != KErrNone)
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::CompleteTransaction - ERROR: Error while handling new data in listener, deleting... (%d)" ), err ) );
                    delete iListenerList[ i ];
                    iListenerList.Remove( i );
                    CloseIfNeeded();
                    }
                }
            }
            break;

        case CSensrvTransaction::ETransTypeMediatorForceChannelClose:
            {
            // If channel is already closed, no need for this
            if ( iState != EChannelStateClosed )
                {
                // First transaction requires special handling, if it is already executing.
                CSensrvTransaction* first = iChannelQueue->First();
                TBool closeNeeded( ETrue );

                if (   first
                    && first->State() != CSensrvTransaction::ETransStateQueued
                    && first->Type() != CSensrvTransaction::ETransTypeCloseChannel )
                    {
                    // There is a non-close transaction executing, so
                    // remove the transaction from mediators
                    if ( iProxy.ServerMediator() )
                        {
                        iProxy.ServerMediator()->RemoveSingleTransaction( first );
                        }

                    if ( iProxy.SsyMediator() )
                        {
                        iProxy.SsyMediator()->RemoveSingleTransaction( first );
                        }
                    }
                else if (   first
                         && first->State() != CSensrvTransaction::ETransStateQueued
                         && first->Type() == CSensrvTransaction::ETransTypeCloseChannel )
                    {
                    // The first executing transaction is already a close transaction,
                    // so it is not necessary to force close anymore.
                    closeNeeded = EFalse;
                    }
                else
                    {
                    // No executing transaction, proceed with close
                    }

                // Delete listeners and their transactions if close needed
                if (closeNeeded)
                    {
                    TInt i(iListenerList.Count());
                    for ( i--; i >= 0; i-- )
                        {
                        iChannelQueue->Remove( &( iListenerList[ i ]->Session() ) );

                        delete iListenerList[ i ];
                        iListenerList.Remove( i );
                        }

                    // Insert new channel close to beginning of the queue
                    // (Queue might still have open channel transactions in it)
                    CSensrvTransaction* closeTransaction = NULL;
                    TRAP( err, closeTransaction = CSensrvTransaction::NewL(
                          NULL,
                          &iProxy,
                          this,
                          CSensrvTransaction::ETransTypeCloseChannel ) );

                    if ( (err==KErrNone) && closeTransaction )
                        {
                        closeTransaction->SetState( CSensrvTransaction::ETransStateQueued );
                        err = iChannelQueue->Add( closeTransaction, EFalse );

                        if ( err != KErrNone )
                            {
                            delete closeTransaction;
                            closeTransaction = NULL;
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::CompleteTransaction - ERROR: Unable to queue internal close transaction" ) ) );
                            }
                        else
                            {
                            HandleNextTransaction();
                            }
                        }
                    else
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::CompleteTransaction - ERROR: Unable to create internal close transaction" ) ) );
                        }
                    }
                }
            else
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CompleteTransaction - Channel already closed when handling ETransTypeMediatorForceChannelClose." ) ) );
                }
            }
            break;

        case CSensrvTransaction::ETransTypeGetProperty:
            {
            //Data is already written in SSY mediator
            }
            break;

        case CSensrvTransaction::ETransTypeGetAllProperties:
            {
            //Data is already written in SSY mediator
            }
            break;

        case CSensrvTransaction::ETransTypeSetProperty:
            {
            // Since SSY won't be sending the property change notification to this channel,
            // send notification to other clients than setter.
            }
            break;

        case CSensrvTransaction::ETransTypeMediatorPropertyChanged:
            {
            // Update indicator values on property change and notify changes to clients
            HandleSetPropertySuccessIndOnPropertyChange();

            // Notify each listener that new data is available
            TInt i( iListenerList.Count() );
            for ( i--; i >= 0; i-- )
                {
                iListenerList[i]->HandleNextProperty();
                }
            }
            break;

        default:
            // Unexpected transaction type, so we do not know what to do with it
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::CompleteTransaction - ERROR: Invalid transaction type" ) ) );
            User::Panic(KSensrvPanicCategory, ESensrvPanicUnknownTransactionType);
            break;
        }

    // Mediator transactions do not cause further handling
    if ( !aTransaction->IsMediatorType() )
        {
        // Complete the transaction
        iProxy.TransactionMonitor()->RemoveTransaction(aTransaction);
        iChannelQueue->Remove(aTransaction, CSensrvTransactionQueue::ERemovalTypeComplete);

        // Execute next transaction in queue, if any
        HandleNextTransaction();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CompleteTransaction - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Cleans up everything related to terminated session.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::SessionTerminated( CSensrvSession* aSession )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SessionTerminated(aSession: 0x%x)"),aSession ) );

    // Cleanup any waiting transactions for this session
    iChannelQueue->Remove(aSession);

    // Delete listener for session if it exists
    DeleteListener(aSession);

    // Close channel if channel state, transactions and listeners are not correctly
    // matched.
    CloseIfNeeded();

    // NOTE: Session termination for a session that has currently executing synchronous
    // client/server message is _extremely_ unlikely and will probably never happen in real devices,
    // Only plausable case that comes to mind is that client thread is paniced by some other server
    // handling some asynchronous message for that client, while the client is waiting on
    // synchronous SendReceive call to sensor server.

    // However, just in case, handle the next transaction. If next transaction and removed transaction
    // were both open channel or close channel transactions, the handling of next transaction might
    // cause error at SSY, depending on timing and how SSY handles simultaneous open/close to the same
    // channel. Preventing this from occuring in server code seems to be quite complex,
    // so for a case this rare it is not cost effective.
    HandleNextTransaction();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SessionTerminated - return" )) );
    }

// ---------------------------------------------------------------------------
// Handles SSY notification failure.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::HandleSsyNotifyFailure()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleSsyNotifyFailure()" ) ) );

    // Check queued transactions to determine if they are handled at SSY
    CSensrvTransaction* transaction = iChannelQueue->First();

    if ( transaction && transaction->State() == CSensrvTransaction::ETransStateNotifyFailed )
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleSsyNotifyFailure - Notify failed transaction found" ) ) );
        transaction->SetState( CSensrvTransaction::ETransStateHandledAtSsy );
        CompleteTransaction( transaction );
        }

    // Also check new data available transaction
    if ( iDataAvailableTransaction && iDataAvailableTransaction->State() == CSensrvTransaction::ETransStateNotifyFailed )
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleSsyNotifyFailure - Notify failed for iDataAvailableTransaction" ) ) );
        iDataAvailableTransaction->SetState( CSensrvTransaction::ETransStateHandledAtSsy );
        CompleteTransaction( iDataAvailableTransaction );
        }

    // Also check force channel close transaction
    if ( iForceChannelCloseTransaction && iForceChannelCloseTransaction->State() == CSensrvTransaction::ETransStateNotifyFailed )
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleSsyNotifyFailure - Notify failed for iForceChannelCloseTransaction" ) ) );
        iForceChannelCloseTransaction->SetState( CSensrvTransaction::ETransStateHandledAtSsy );
        CompleteTransaction( iForceChannelCloseTransaction );
        }

    // Also check property change transaction
    if ( iPropertyChangedTransaction && iPropertyChangedTransaction->State() == CSensrvTransaction::ETransStateNotifyFailed )
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleSsyNotifyFailure - Notify failed for iPropertyChangedTransaction" ) ) );
        iPropertyChangedTransaction->SetState( CSensrvTransaction::ETransStateHandledAtSsy );
        CompleteTransaction( iPropertyChangedTransaction );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleSsyNotifyFailure - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Forces buffer filled at SSY.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::ForceBufferFilled()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ForceBufferFilled()" ) ) );

    // Notify SSY thread that buffer fill must be forced.
    if (iForceBufferFilledTransaction)
        {
        if (iForcingBufferFilled)
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ForceBufferFilled - Already forcing bufferfilled" ) ) );
            }
        else
            {
            TInt err = iProxy.SsyMediator()->Notify(iForceBufferFilledTransaction);

            if (err == KErrNone)
                {
                iForcingBufferFilled = ETrue;
                iProxy.TransactionMonitor()->AddTransaction(iForceBufferFilledTransaction);
                }
            else
                {
                // Just trace, it is not critical if notifying SSY mediator fails in this case.
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::ForceBufferFilled - ERROR: SSY mediator notify failed: %d" ), err ) );
                }
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::ForceBufferFilled - ERROR: Buffer filled transaction NULL" ) ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ForceBufferFilled - return. iForcingBufferFilled: %d" ), iForcingBufferFilled ) );
    }

// ---------------------------------------------------------------------------
// Recalculates buffering count to match lowest need of all buffers.
//
// Note: Even for one client it is necessary to recalculate buffering count,
// as buffering period caused buffer fills combined with a client that
// occasionally doesn't get data in timely manner will need recalculation
// after such events.
// Trying to optimize this to skip unnecessary recalculation is quite complex,
// and probably not that much faster than just recalculating always
// without extra checks.
// ---------------------------------------------------------------------------
//
TInt CSensrvChannel::RecalculateBufferingCount()
    {
    TInt count(iListenerList.Count());

    TInt previousLow(iMaxBufferingCount);

    // Determine lowest remaining need of each listener.
    TInt remainingCount(0);

    if (count)
        {
        for (TInt i=0; previousLow > KSensrvMinimumBufferingCount && i < count; i++)
            {
            if (   iListenerList[i]->State() & CSensrvChannelListener::EChannelListenerStateConditionListening
                || iListenerList[i]->State() & CSensrvChannelListener::EChannelListenerStateDataListening )
                {
                remainingCount = iListenerList[i]->RemainingBufferingCount();

                if (remainingCount < previousLow)
                    {
                    previousLow = remainingCount;
                    }
                }
            }
        }
    else
        {
        // No listeners, so return zero count.
        previousLow = 0;
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::RecalculateBufferingCount - No listeners" ) ) );
        }

    BUFFER_TRACE( (_L( "#### Recalculated buffering count: %d" ), previousLow) );

    return previousLow;
    }



// ---------------------------------------------------------------------------
// Creates a new condition evaluator or returns an existing one.
// Once the evaluator is created, it will not be destroyed until channel
// is closed.
// ---------------------------------------------------------------------------
//
CSensrvConditionEvaluator* CSensrvChannel::ConditionEvaluatorL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ConditionEvaluatorL()" ) ) );

    if (!iConditionEvaluator)
        {
        iConditionEvaluator = CSensrvConditionEvaluator::NewL( iChannelInfo );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ConditionEvaluatorL - return 0x%x" ), iConditionEvaluator ) );

    return iConditionEvaluator;
    }

// ---------------------------------------------------------------------------
// Handles next transaction
// ---------------------------------------------------------------------------
//
void CSensrvChannel::HandleNextTransaction()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction()" ) ) );

    CSensrvTransaction* transaction = iChannelQueue->First();

    // Handle transactions until a transaction is left in executing state
    while (transaction)
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - Handling transaction of type: %d" ), transaction->Type() ) );

        if (transaction->State() != CSensrvTransaction::ETransStateQueued)
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - Previous transaction still executing" ) ) );
            transaction = NULL;
            break;
            }
        
        TInt err(KErrNone);

        transaction->SetState(CSensrvTransaction::ETransStateExecuting);

        TBool ssyTransaction(EFalse);

        // Check channel closed state, only open and close have special operations
        if ( iState == EChannelStateClosed
            && ( transaction->Type() != CSensrvTransaction::ETransTypeOpenChannel
                 && transaction->Type() != CSensrvTransaction::ETransTypeCloseChannel ) )
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - ERROR: Channel closed." ) ) );
            err = KErrNotFound;

            // Since channel is closed, also delete listener, if it exists
            DeleteListener(transaction->Session());
            }
        else
            {
            switch(transaction->Type())
                {
                ///////////////////////////////////////////////
                case CSensrvTransaction::ETransTypeOpenChannel:
                    {
                    // If session already has a listener, channel is already open
                    // for this client, which is an error
                    CSensrvChannelListener* listener = GetListener(transaction->Session());

                    if (listener)
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - ERROR: ETransTypeOpenChannel, listener already exists." ) ) );
                        err = KErrAlreadyExists;
                        }
                    else
                        {
                        // Add new listener for the session
                        TRAP(err, listener = CSensrvChannelListener::NewL(*transaction->Session(), *this, iProxy));

                        if (err == KErrNone)
                            {
                            err = iListenerList.Append(listener);
                            if (err == KErrNone )
                                {
                                // Quick update is allowed only when the highest has been searched earlier.
                                if ( iHighestListenerPriorityMultiples > 0 )
                                    {
                                    // Update cached highest listener priority tracking
                                    TInt priority( listener->Priority() );
                                    if ( priority > iHighestListenerPriority )
                                        {
                                        iHighestListenerPriority = priority;
                                        iHighestListenerPriorityMultiples = 1;
                                        }
                                    else if ( priority == iHighestListenerPriority )
                                        {
                                        iHighestListenerPriorityMultiples++;
                                        }
                                    }
                                }
                            else
                                {
                                delete listener;
                                listener = NULL;
                                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - ERROR: ETransTypeOpenChannel, listener append failed: %d." ), err ) );
                                }
                            }
                        else
                            {
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - ERROR: ETransTypeOpenChannel, new listener creation failed: %d" ), err ) );
                            }
                        }

                    // If channel state is closed, SSY needs to handle transaction to open channel.
                    // Also stop unloading timer if it is active.
                    if ( err == KErrNone && iState == EChannelStateClosed )
                        {
                        // Stop unload timer if it happens to be active
                        iProxy.StopUnloadTimer();
                       
                        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - Opening channel to SSY" ) ) );
                        ssyTransaction = ETrue;
                        }

                    // Write maximum buffering count and data item size to message, if buffer exists.
                    if ( err == KErrNone && iChannelBuffer )
                        {
                        CSensrvTransaction::TChannelOpenMessageData data( iMaxBufferingCount, iChannelInfo.iDataItemSize );
                        err = transaction->SetMessageData( &data );
                        }

                    // Create mediator transactions to be used in notifying property changes.
                    // These transactions are never deleted and are reused.
                    if ( err == KErrNone && !iPropertyChangedTransaction )
                        {
                        TRAP( err, iPropertyChangedTransaction = CSensrvTransaction::NewL(
                                   NULL,
                                   &iProxy,
                                   this,
                                   CSensrvTransaction::ETransTypeMediatorPropertyChanged ) );

                        if ( err != KErrNone )
                            {
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - ERROR: ETransTypeMediatorPropertyChanged transaction creation failed: %d" ), err ) );
                            }
                        }

                    if ( err != KErrNone)
                        {
                        DeleteListener(transaction->Session());
                        }
                    // If SSY transaction required, handle the indication when the SSY transaction completes
                    if ( err == KErrNone && !ssyTransaction )
                        {
                        HandleDependencyOnOpen();

                        // Update indicator values on open and notify changes to clients
                        HandleSetPropertySuccessIndOnCreateOrDelete();
                        }
                    }
                    break;

                ///////////////////////////////////////////////
                case CSensrvTransaction::ETransTypeCloseChannel:
                    {
                    //get the listener
                    CSensrvChannelListener* listener = FindListener( transaction->Session() );

                    TInt priority( KMinTInt );

                    if ( listener )
                        {
                        priority = listener->Priority();
                        }

                    DeleteListener( transaction->Session() );

                    HandleDependencyOnClose( priority ); //listener is gone

                    // If there are more listeners remaining, or channel already closed, do not close channel
                    if ( iState == EChannelStateClosed )
                        {
                        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - Channel already closed, not closing channel to SSY" ) ) );
                        }
                    else if ( iListenerList.Count() > 0 )
                        {
                        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - More clients remaining, not closing channel to SSY" ) ) );
                        }
                    else
                        {
                        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - Closing channel to SSY" ) ) );
                        ssyTransaction = ETrue;
                        }
                    // Update indicator values on close and notify changes to clients

                    HandleSetPropertySuccessIndOnCreateOrDelete();
                    }
                    break;

                ///////////////////////////////////////////////
                case CSensrvTransaction::ETransTypeStartListening:
                case CSensrvTransaction::ETransTypeStartConditionListening:
                    {
                    err = HandleStartListeningTransaction(*transaction);
                    ssyTransaction = ETrue;
                    }
                    break;

                ///////////////////////////////////////////////
                case CSensrvTransaction::ETransTypeStopListening:
                case CSensrvTransaction::ETransTypeStopConditionListening:
                    {
                    HandleStopListeningTransaction(*transaction, ssyTransaction);
                    }
                    break;

                ///////////////////////////////////////////////
                case CSensrvTransaction::ETransTypeGetProperty:
                case CSensrvTransaction::ETransTypeGetAllProperties:
                case CSensrvTransaction::ETransTypeSetProperty:
                    {
                    ssyTransaction = ETrue;
                    CSensrvChannelListener* listener = GetListener(transaction->Session());
                    if ( !listener )
                        {
                        //Listener doesn't exist anymore (for some reason).
                        err = KErrNotFound;
                        }
                    }
                    break;

                ///////////////////////////////////////////////
                default:
                    // Unexpected transaction type, so we do not know what to do with it
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - ERROR: Invalid transaction type" ) ) );
                    User::Panic(KSensrvPanicCategory, ESensrvPanicUnknownTransactionType);
                    break;
                }
            }

        if (err == KErrNone && ssyTransaction)
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - Notifying SSY..." ) ) );
            // If SSY mediator is not yet initialized, it is an error
            __ASSERT_ALWAYS(iProxy.SsyMediator(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullSsyMediator));

            // Notify SSY mediator that transaction is ready to be handled.
            err = iProxy.SsyMediator()->Notify(transaction);
            }

        // If there was error, transaction needs to be removed from queue (which also completes it)
        // Transaction also needs to be completed if SSY transaction is not needed.
        // If transaction is completed, next one in queue is handled.
        if (err != KErrNone)
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - ERROR: Failed to handle next transaction: %d, type: %d" ), err, transaction->Type() ) );
            transaction->SetErrorCode(err);
            iChannelQueue->Remove(transaction, CSensrvTransactionQueue::ERemovalTypeComplete);

            // Prepare to handle next transaction
            transaction = iChannelQueue->First();
            }
        else if (!ssyTransaction)
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - SSY transaction not needed, completing transaction" ) ) );
            iChannelQueue->Remove(transaction, CSensrvTransactionQueue::ERemovalTypeComplete);

            // Prepare to handle next transaction
            transaction = iChannelQueue->First();
            }
        else
            {
            // SSY transaction executing, no need for further transaction handling now
            iProxy.TransactionMonitor()->AddTransaction(transaction);
            transaction = NULL;
            }
        } // End while(transaction)

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Gets listener pointer from list or NULL if no listener found.
// ---------------------------------------------------------------------------
//
CSensrvChannelListener* CSensrvChannel::GetListener(CSensrvSession* aSession)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetListener(aSession: 0x%x)" ), aSession ) );

    CSensrvChannelListener* listener = NULL;
    TInt count(iListenerList.Count());

    for (TInt i=0; i < count; i++)
        {
        if (&iListenerList[i]->Session() == aSession)
            {
            listener = iListenerList[i];
            break;
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetListener - return 0x%x" ), listener ) );

    return listener;
    }

// ---------------------------------------------------------------------------
// Deletes listener if it exists
// ---------------------------------------------------------------------------
//
void CSensrvChannel::DeleteListener(CSensrvSession* aSession)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::DeleteListener(aSession: 0x%x)" ), aSession ) );

    TInt count(iListenerList.Count());
    TBool deleted( EFalse );

    for ( TInt i=0; !deleted && i < count; i++ )
        {
        if ( &iListenerList[i]->Session() == aSession )
            {
            deleted = ETrue;
            CSensrvChannelListener* ptr = iListenerList[i];
            if ( ptr == iCurrentSetPropertyControl )
                {
                // Reset the property control
                iCurrentSetPropertyControl = NULL;
                }
            if ( iHighestListenerPriorityMultiples > 0 &&
                 ptr->Priority() == iHighestListenerPriority )
                {
                // Update cached highest listener priority tracking
                if ( !( --iHighestListenerPriorityMultiples ) )
                    {
                    // Reset the highest priority when there are no multiples.
                    // Next GetHighestListenerPriority() call triggers complete priority check.
                    iHighestListenerPriority = KMinTInt;
                    }
                }
            delete ptr;
            iListenerList.Remove(i);
            }
        }

    // If listener was not found, it is normally not an error, but in some cases it is interesting.
    // -> Trace only regular COMPONENT_TRACE
    if ( !deleted )
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::DeleteListener - Listener not found" ) ) );
        }

    // Check if unload timer needs to be started, since there are no more clients
    if (!iListenerList.Count())
        {
        iProxy.StartUnloadTimerIfNeeded();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::DeleteListener - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Finds a listener based on session
// ---------------------------------------------------------------------------
//
CSensrvChannelListener* CSensrvChannel::FindListener( CSensrvSession* aSession )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::FindListener(aSession: 0x%x)" ), aSession ) );

    TInt count( iListenerList.Count() );
    CSensrvChannelListener* ptr( NULL );

    if ( aSession )
		{
		for ( TInt i=0; i < count; i++ )
			{
			if ( &iListenerList[i]->Session() == aSession )
				{
				ptr = iListenerList[ i ];
				break;
				}
			}
		}

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::FindListener - return" ) ) );

	return ptr;
    }

// ---------------------------------------------------------------------------
// Closes the channel to SSY if there are no more listeners.
// ---------------------------------------------------------------------------
//
void CSensrvChannel::CloseIfNeeded()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CloseIfNeeded()" ) ) );

    // Check listener count and state
    if (iState == EChannelStateClosed)
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CloseIfNeeded - Close NOT needed, channel already closed" ) ) );
        }
    else
        {
        TInt err(KErrNone);
        TInt count(iListenerList.Count());
        CSensrvTransaction* transaction = NULL;

        if (!count)
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CloseIfNeeded - Close needed, no clients" ) ) );
            // Create internal close transaction and queue it
            TRAP(err, transaction = CSensrvTransaction::NewL(
                NULL,
                &iProxy,
                this,
                CSensrvTransaction::ETransTypeCloseChannel));
            }
        else if (iState == EChannelStateListening)
            {
            // Check if any of other clients are actually listening. If not,
            // queue stop listening transaction
            TBool activeFound(EFalse);

            for (TInt i=0; !activeFound && i < count; i++)
                {
                if (   iListenerList[i]->State() & CSensrvChannelListener::EChannelListenerStateConditionListening
                    || iListenerList[i]->State() & CSensrvChannelListener::EChannelListenerStateDataListening)
                    {
                    activeFound = ETrue;
                    }
                }

            if (!activeFound)
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CloseIfNeeded - Stop needed, no active clients" ) ) );
                TRAP(err, transaction = CSensrvTransaction::NewL(
                    NULL,
                    &iProxy,
                    this,
                    CSensrvTransaction::ETransTypeStopListening));
                }
            else
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CloseIfNeeded - Stop NOT needed, active client found" ) ) );
                }
            }
        else
            {
            // State open and some other clients, no need to do anything
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CloseIfNeeded - Close NOT needed, channel open and other clients found " ) ) );
            }

        if ( (err == KErrNone) && (transaction) )
            {
            // Queue transaction. If there is some open transaction queued before close,
            // that is alright, close/stop will be ignored in transaction handling if state at
            // that point is not suitable.
            transaction->SetState(CSensrvTransaction::ETransStateQueued);
            err = iChannelQueue->Add(transaction);

            if (err != KErrNone)
                {
                delete transaction;
                transaction = NULL;
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::CloseIfNeeded - ERROR: Unable to queue close/stop transaction" ) ) );
                }
            }
        else
            {
            transaction = NULL;
            }
        }

    // Note that if there is an error creating/queueing transaction, channel will stay in incorrect state,
    // but can't do anything about it.

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::CloseIfNeeded - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Handles start data and condition listening transactions
// ---------------------------------------------------------------------------
//
TInt CSensrvChannel::HandleStartListeningTransaction(CSensrvTransaction& aTransaction)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction()" ) ) );

    TInt err(KErrNone);

    CSensrvChannelListener::TSensrvChannelListenerState targetState
        (CSensrvChannelListener::EChannelListenerStateDataListening);

    if (aTransaction.Type() == CSensrvTransaction::ETransTypeStartConditionListening)
        {
        // Condition evaluator must exist
        TRAP( err, ConditionEvaluatorL() );

        targetState = CSensrvChannelListener::EChannelListenerStateConditionListening;
        }
    else // ETransTypeStartListening
        {
        // Target state already data listening
        }

    if (err == KErrNone)
        {
        // Check that there is listener for this client session
        CSensrvChannelListener* listener = GetListener(aTransaction.Session());

        if (!listener)
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction - ERROR: Listener doesn't exist." ) ) );
            err = KErrNotFound;
            }
        else if (listener->State() & targetState)
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction - ERROR: Listener already listening." ) ) );
            err = KErrAlreadyExists;
            }
        else
            {
            // Create mediator transactions to be used in notifying data writes to buffer.
            // These transactions are never deleted and are reused.
            if (!iDataAvailableTransaction)
                {
                TRAP(err, iDataAvailableTransaction = CSensrvTransaction::NewL(
                    NULL,
                    &iProxy,
                    this,
                    CSensrvTransaction::ETransTypeMediatorNewDataAvailable));

                if (err != KErrNone)
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction - ERROR: ETransTypeMediatorNewDataAvailable transaction creation failed: %d" ), err ) );
                    }
                }

            if (err == KErrNone && !iForceBufferFilledTransaction)
                {
                TRAP(err, iForceBufferFilledTransaction = CSensrvTransaction::NewL(
                    NULL,
                    &iProxy,
                    this,
                    CSensrvTransaction::ETransTypeMediatorForceBufferFilled));

                if (err != KErrNone)
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction - ERROR: ETransTypeMediatorForceBufferFilled transaction creation failed: %d" ), err ) );
                    }
                }

            if (err == KErrNone && !iForceChannelCloseTransaction)
                {
                TRAP(err, iForceChannelCloseTransaction = CSensrvTransaction::NewL(
                    NULL,
                    &iProxy,
                    this,
                    CSensrvTransaction::ETransTypeMediatorForceChannelClose));

                if (err != KErrNone)
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction - ERROR: ETransTypeMediatorForceChannelClose transaction creation failed: %d" ), err ) );
                    }
                }

            if ( err == KErrNone )
                {
                // Get buffering count and period from message
                TSensrvStartListeningParametersPckgBuf pckg;
                err = aTransaction.Message()->Read( KSensrvStartListeningParametersSlot, pckg );

                if ( err == KErrNone )
                    {
                    TSensrvStartListeningParameters startParams = pckg();

					TSensrvResourceChannelInfo::TSensrvChannelGroup group = ChannelGroup();

					if (   group == TSensrvResourceChannelInfo::ESensrvChannelGroupEvent
						|| group == TSensrvResourceChannelInfo::ESensrvChannelGroupState )
						{
						startParams.iBufferingPeriod = 0;
						}

                    err = listener->StartListening( startParams.iDesiredBufferingCount,
                                                    startParams.iMaximumBufferingCount,
                                                    startParams.iBufferingPeriod,
                                                    targetState );
                    if (err != KErrNone)
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction - ERROR: Listener adding failed: %d" ), err ) );
                        }
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction - ERROR: Start parameters reading failed: %d" ), err ) );
                    }
                }
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction - ERROR: Condition evaluator plugin loading failed: %d" ), err ) );
        }


    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Handles stop data and condition listening transactions
// ---------------------------------------------------------------------------
//
TInt CSensrvChannel::HandleStopListeningTransaction(CSensrvTransaction& aTransaction,
                                                    TBool& aSsyTransactionNeeded)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStopListeningTransaction()" ) ) );

    TInt err(KErrNone);

    CSensrvChannelListener::TSensrvChannelListenerState currentState
        (CSensrvChannelListener::EChannelListenerStateDataListening);

    if (aTransaction.Type() == CSensrvTransaction::ETransTypeStopConditionListening)
        {
        currentState = CSensrvChannelListener::EChannelListenerStateConditionListening;
        }
    else // ETransTypeStartListening
        {
        // Target state already data listening
        }

    // Check that there is listener for this client session
    CSensrvChannelListener* listener = GetListener(aTransaction.Session());

    if (listener && !(listener->State() & currentState))
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - ERROR: Listener not listening." ) ) );
        err = KErrNotFound;
        }
    else
        {
        if (listener)
            {
            listener->StopListening(currentState, ETrue);
            }

        if (iState == EChannelStateOpen)
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleNextTransaction - ERROR: Channel not listening. Listener: 0x%x" ), listener ) );
            err = KErrNotFound;
            }
        else // iState == EChannelStateListening
            {
            // Check if there are any more active listeners
            TInt count(iListenerList.Count());
            TBool activeFound(EFalse);

            for (TInt i=0; !activeFound && i < count; i++)
                {
                if (   iListenerList[i]->State() & CSensrvChannelListener::EChannelListenerStateConditionListening
                    || iListenerList[i]->State() & CSensrvChannelListener::EChannelListenerStateDataListening)
                    {
                    activeFound = ETrue;
                    }
                }

            if (!activeFound)
                {
                // No active listeners remain, stop listening at SSY
                aSsyTransactionNeeded = ETrue;
                }
            else
                {
                // At least one active listener remaining, do not stop listening at SSY
                aSsyTransactionNeeded = EFalse;
                }
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleStartListeningTransaction - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::GetHighestListenerPriority()
// ---------------------------------------------------------------------------
//
TInt CSensrvChannel::GetHighestListenerPriority( TInt& aHighestPriority )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetHighestListenerPriority()" ) ) );

    // Prefer the cached results if possible
    if ( iHighestListenerPriorityMultiples <= 0 )
        {
        // Search the highest priority and its multiples from this channel and cache the results
        iHighestListenerPriorityMultiples = 0;
        iHighestListenerPriority = KMinTInt;
        TInt i( iListenerList.Count() );
        for( i--; i >= 0; i-- )
            {
            CSensrvChannelListener* ptr = iListenerList[ i ];
            TInt priority( ptr->Priority() );
            if ( priority > iHighestListenerPriority )
                {
                iHighestListenerPriority = priority;
                iHighestListenerPriorityMultiples = 1;
                }
            else if ( priority == iHighestListenerPriority )
                {
                iHighestListenerPriorityMultiples++;
                }
            }
        }

    aHighestPriority = iHighestListenerPriority;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetHighestListenerPriority() - channel: %d priority: %d multiples: %d return" ),
        iId, iHighestListenerPriority, iHighestListenerPriorityMultiples ) );

    return iHighestListenerPriorityMultiples;
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::GetLowestChannelPriority()
// ---------------------------------------------------------------------------
//
TInt CSensrvChannel::GetLowestChannelPriority( const RSensrvChannelList* aAffectedChannelsList, TSensrvChannelId& aChannelId )
    {

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetLowestChannelPriority()" ) ) );

    TInt lowestPriority( KMaxTInt );
    TInt newPriority( 0 );
    TInt multiplesOnChannel( 0 );

    if ( aAffectedChannelsList )
        {
        TInt i( aAffectedChannelsList->Count() );
        CSensrvChannel* channel = NULL;
        for ( i--; i >= 0; i-- )
            {
			if ( ( *aAffectedChannelsList )[ i ] != iId )
            	{
				channel = iProxy.GetChannelForId( ( *aAffectedChannelsList )[ i ] );
				if ( channel )
					{
					multiplesOnChannel = channel->GetHighestListenerPriority( newPriority );
					if ( multiplesOnChannel > 0 )
						{
						if ( newPriority < lowestPriority )
							{
							lowestPriority 	= newPriority;
							aChannelId 		= ( *aAffectedChannelsList )[ i ];
							}
						}
					}
				}
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetLowestChannelPriority() - channel %d priority: %d" ), aChannelId, lowestPriority ) );

	return lowestPriority;
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::GetHighestChannelPriority()
// ---------------------------------------------------------------------------
//
TInt CSensrvChannel::GetHighestChannelPriority( const RSensrvChannelList* aAffectedChannelsList, TSensrvChannelId& aChannelId )
    {

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetHighestChannelPriority()" ) ) );

    TInt highestPriority( KMinTInt );
    TInt newPriority( 0 );
    TInt multiplesOnChannel( 0 );

    if ( aAffectedChannelsList )
        {
        TInt i( aAffectedChannelsList->Count() );
        CSensrvChannel* channel = NULL;
        for ( i--; i >= 0; i-- )
            {
			if ( ( *aAffectedChannelsList )[ i ] != iId )
            	{
				channel = iProxy.GetChannelForId( ( *aAffectedChannelsList )[ i ] );
				if ( channel )
					{
					multiplesOnChannel = channel->GetHighestListenerPriority( newPriority );
					if ( multiplesOnChannel > 0 )
						{
						if ( newPriority > highestPriority )
							{
							highestPriority = newPriority;
							aChannelId 		= ( *aAffectedChannelsList )[ i ];
							}
						}
					}
				}
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetHighestChannelPriority() - channel %d priority: %d" ), aChannelId, highestPriority ) );

	return highestPriority;
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::GetHighestListenerPriorityOfAffectedChannels()
// ---------------------------------------------------------------------------
//
TInt CSensrvChannel::GetHighestListenerPriorityOfAffectedChannels( TInt& aHighestPriority )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetHighestListenerPriorityOfAffectedChannels()" ) ) );

    // Search the highest priority and its multiples from all affected channels
    RSensrvChannelList* affectedChannels = GetAffectedChannels();
    aHighestPriority = KMinTInt;
    TInt multiples( 0 );
    if ( affectedChannels )
        {
        TInt i( affectedChannels->Count() );
        TInt highestOfChannel( 0 );
        TInt multiplesOnChannel( 0 );
        CSensrvChannel* channel = NULL;
        for ( i--; i >= 0; i-- )
            {
            channel = iProxy.GetChannelForId( ( *affectedChannels )[ i ] );
            if ( channel )
                {
                multiplesOnChannel = channel->GetHighestListenerPriority( highestOfChannel );
                if ( multiplesOnChannel > 0 )
                    {
                    if ( highestOfChannel > aHighestPriority )
                        {
                        aHighestPriority = highestOfChannel;
                        multiples = multiplesOnChannel;
                        }
                    else if ( highestOfChannel == aHighestPriority )
                        {
                        multiples += multiplesOnChannel;
                        }
                    }
                }
            }

        affectedChannels->Reset();
        delete affectedChannels;
		affectedChannels = NULL;

        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetHighestListenerPriorityOfAffectedChannels() - priority: %d multiples: %d return" ),
         aHighestPriority, multiples ) );

    return multiples;
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::UpdateSetPropertySuccessIndToChannelListeners()
// ---------------------------------------------------------------------------
//
void CSensrvChannel::UpdateSetPropertySuccessIndToChannelListeners(
        CSensrvChannel& aChannel, TInt aHighestOfAffectedChannels, TInt aMultiplesOnAffectedChannels )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::UpdateSetPropertySuccessIndToChannelListeners()" ) ) );

    TInt highestOfChannel( 0 );
    TInt multiplesOnChannel( aChannel.GetHighestListenerPriority( highestOfChannel ) );
    if ( multiplesOnChannel > 0 )
        {
        if ( aChannel.iCurrentSetPropertyControl &&
             aChannel.iCurrentSetPropertyControl->Priority() < highestOfChannel )
            {
            // Reset the property control due to higher priority listener
            aChannel.iCurrentSetPropertyControl = NULL;
            }
        CSensrvChannelListener::TPropertySetSuccessEvaluationData evaluationData(
            aHighestOfAffectedChannels, aMultiplesOnAffectedChannels,
            highestOfChannel, multiplesOnChannel );
        TInt i( aChannel.iListenerList.Count() );
        for( i--; i >= 0; i-- )
            {
            aChannel.iListenerList[ i ]->HandleSetPropertySuccessInd( evaluationData );
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::UpdateSetPropertySuccessIndToChannelListeners() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::HandleSetPropertySuccessIndOnPropertyChange()
// ---------------------------------------------------------------------------
//
void CSensrvChannel::HandleSetPropertySuccessIndOnPropertyChange()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleSetPropertySuccessIndOnPropertyChange()" ) ) );

    TInt highestOfAffectedChannels( 0 );
    TInt multiplesOnAffectedChannels( GetHighestListenerPriorityOfAffectedChannels( highestOfAffectedChannels ) );

    // Send the priority information for listener of this channel to be evaluated
    UpdateSetPropertySuccessIndToChannelListeners(
        *this, highestOfAffectedChannels, multiplesOnAffectedChannels );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleSetPropertySuccessIndOnPropertyChange() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::HandleSetPropertySuccessIndOnCreateOrDelete()
// ---------------------------------------------------------------------------
//
void CSensrvChannel::HandleSetPropertySuccessIndOnCreateOrDelete()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleSetPropertySuccessIndOnCreateOrDelete()" ) ) );

    // Get the priority information
    RSensrvChannelList* affectedChannels = GetAffectedChannels();
    if ( affectedChannels )
        {
        // Get the priority information
        TInt highestOfAffectedChannels( 0 );
        TInt multiplesOnAffectedChannels( GetHighestListenerPriorityOfAffectedChannels(
            highestOfAffectedChannels ) );

        // Send the priority information for listeners of affected channels to be evaluated
        TInt i( affectedChannels->Count() );
        for ( i--; i >= 0; i-- )
            {
            CSensrvChannel* channel = iProxy.GetChannelForId( ( *affectedChannels )[ i ] );
            if ( channel )
                {
                UpdateSetPropertySuccessIndToChannelListeners(
                    *channel, highestOfAffectedChannels, multiplesOnAffectedChannels );
                }
            }

        affectedChannels->Reset();
        delete affectedChannels;
		affectedChannels = NULL;

        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleSetPropertySuccessIndOnCreateOrDelete() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::TakePropertySetControl()
// ---------------------------------------------------------------------------
//
void CSensrvChannel::TakePropertySetControl( CSensrvChannelListener* aListener )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::TakePropertySetControl()" ) ) );

    if ( iCurrentSetPropertyControl != aListener )
        {
        if ( aListener )
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::TakePropertySetControl() channel: %d listener: 0x%x priority: %d" ),
                iId, aListener, aListener->Priority() ) );
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::TakePropertySetControl() channel: %d listener: 0x%x" ), iId, aListener ) );
            }
        iCurrentSetPropertyControl = aListener;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::TakePropertySetControl() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::GetAffectedChannels()
// ---------------------------------------------------------------------------
//
RSensrvChannelList* CSensrvChannel::GetAffectedChannels() const
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetAffectedChannels()" ) ) );

    RSensrvChannelList* ret = new RSensrvChannelList();

    if ( iProxy.SsyMediator() && ret )
        {
        iProxy.SsyMediator()->AffectedChannels( iId, iPropertyProvider, ret );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetAffectedChannels() - return 0x%x" ), ret ) );
    return ret;
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::GetAvailabilityAffectedChannels()
// ---------------------------------------------------------------------------
//
RSensrvChannelList* CSensrvChannel::GetAvailabilityAffectedChannels( TSensrvAvailability aValue ) const
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetAvailabilityAffectedChannels()" ) ) );

    RSensrvChannelList* ret = new RSensrvChannelList();

    if ( iProxy.SsyMediator() && ret )
        {
        iProxy.SsyMediator()->AvailabilityAffectedChannels( iId, iPropertyProvider, ret, aValue );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetAvailabilityAffectedChannels() - return 0x%x" ), ret ) );
    return ret;
    }

// ---------------------------------------------------------------------------
// CSensrvChannel::HandleDependencyOnOpen()
// ---------------------------------------------------------------------------
//
void CSensrvChannel::HandleDependencyOnOpen()
	{
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnOpen() channel %d" ), iId ) );

	TInt propErr( KErrNone );														//for property reading status
	TSensrvProperty property( KSensrvPropIdAvailability, KSensrvItemIndexNone, 0 );	//for property

	TRAP( propErr, iPropertyProvider->GetPropertyL( iId, property ) ); //handle property reading error below

	if ( propErr == KErrNone )
		{//availability property read OK ->
		TInt isAvailable( static_cast<TInt>( ESensrvAvailabilityFalse ) );	//for availability property value

		property.GetValue( isAvailable ); //get iId channel availability property

		if ( ESensrvAvailabilityFalse == ( static_cast<TSensrvAvailability>( isAvailable ) ) )
			{//current channel is not available, check if it needs to be set available ->
			COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnOpen - channel %d unavailable (%d)" ),iId, isAvailable ) );

			RSensrvChannelList* affectedChannels = GetAvailabilityAffectedChannels( ESensrvAvailabilityTrue ); //get the affected channels

			if ( affectedChannels )
				{//affected channels returned a valid pointer ->
				if ( affectedChannels->Count() )
					{//there are affected channels ->
					TInt 				channelPriority( KMinTInt ); //for current channel priority
					TInt 				othersPriority( KMinTInt );	 //for other channel priority
					TSensrvChannelId 	otherChannelId( 0 );		 //for other channel channel id

					(void)GetHighestListenerPriority( channelPriority ); //get current channel highest priority, multiplicity value is not needed, thus cast'd to void

					othersPriority = GetLowestChannelPriority( affectedChannels, otherChannelId );

					if ( channelPriority <= othersPriority )
						{//a higher priority client is blocking this channel ->
						//current channel highest is not high enough!
						COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnOpen - channel %d priority (%d) is not high enough (vs. %d) to be enabled" ),iId, channelPriority, othersPriority ) );
						}//<- a higher priority client is blocking this channel

					else
						{//this channel now has high enough priority user to enable and override another channel ->
						//other has lower highest priority, disable it and enable this
						COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnOpen - channel %d priority (%d) is high enough (vs. %d)" ),iId, channelPriority, othersPriority ) );

						(void)SetChannelAvailability( iId /*<- to enable*/, otherChannelId /* <- to disable */, property );
						}//<- this channel has now high enough priority user to enable and override another channel
					}//<-there are affected channels
				else
					{// no affected channels in list ->
					(void)SetChannelAvailability( iId /*<- to enable*/, property, ESensrvAvailabilityTrue );
					} // <- no affected channels

		        affectedChannels->Reset();
		        delete affectedChannels;
				affectedChannels = NULL;

				}//<- affected channels returned a valid pointer
			}//<- current channel is not available, check if it needs to be set available
		else
			{//Current channel is available ->
			COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnOpen - channel %d already available (%d) or no affected channels" ),iId, isAvailable ) );
			}//<- Current channel is available
		}//<- availability property read OK
	else
		{
		//Error reading property
		ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnOpen - ERROR: Unable to get property  Error: %d" ), propErr ) );
		}
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnOpen - return" ) ) );
	}

// ---------------------------------------------------------------------------
// CSensrvChannel::HandleDependencyOnClose()
// ---------------------------------------------------------------------------
//
void CSensrvChannel::HandleDependencyOnClose( const TInt &aPriority )
	{
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnClose() channel %d" ), iId ) );

	TInt propErr( KErrNone );

	TSensrvProperty property( KSensrvPropIdAvailability, KSensrvItemIndexNone, 0 );

	TRAP( propErr, iPropertyProvider->GetPropertyL( iId, property ) ); //get availability property for current channel

	if ( propErr == KErrNone )
		{//availability property read OK ->

		TInt isAvailable( static_cast<TInt>( ESensrvAvailabilityFalse ) );	//for availability property value

		property.GetValue( isAvailable ); 	//get iId channel availability property

		if ( ESensrvAvailabilityTrue == ( static_cast<TSensrvAvailability>( isAvailable ) ) )
			{//Current channel is available, check if it needs to be set unavailable ->

			RSensrvChannelList* affectedChannels = GetAvailabilityAffectedChannels( ESensrvAvailabilityFalse );

			if ( affectedChannels )
				{//affected channels returned a valid pointer ->
				if ( affectedChannels->Count() )
					{//there are affected channels ->
					TInt 				channelPriority( KMinTInt ); //for current channel priority
					TInt 				othersPriority( KMinTInt );	 //for other channel priority
					TSensrvChannelId 	otherChannelId( 0 );		 //for other channel channel id

					(void)GetHighestListenerPriority( channelPriority ); //get current channel highest priority, multiplicity value is not needed, thus cast'd to void

					if ( channelPriority >= aPriority )
						{//current channel has remaining listener(s) with a higher priority than listener that just left ->
						//do nuthin', only highest priority / channel matters
						COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnClose - current channel has highest or equal priority" ) ) );
						}//<- current channel has remaining listener(s) with a higher priority than listener that just left
					else if ( 0 < ListenerCount() )
						{//highest priority listener of the channel left, other listeners exist ->

						othersPriority = GetHighestChannelPriority( affectedChannels, otherChannelId );

						if ( channelPriority < othersPriority )
							{//an unavailable channel has a higher priority listener than current channel ->
							COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnClose - channel %d priority is high enough (%d) vs. current channel %d (%d)" ),
												otherChannelId, othersPriority, iId, channelPriority ) );

							(void)SetChannelAvailability( otherChannelId /* <- to enable */, iId /* <- to disable */, property );
							}//<- an unavailable channel has a higher priority listener than current channel
							else
							{//an unavailable channel does not have a higher priority listener than current channel ->
							COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnClose - other channel %d priority is not high enough (%d) vs. current channel %d (%d)" ),
												otherChannelId, othersPriority, iId, channelPriority ) );
							}//<- an unavailable channel does not have a higher priority listener than current channel
						}//<- highest priority listener of the channel left, other listeners exist
					else
						{//highest priority of the channel closed, no other listeners exist ->
						othersPriority = GetHighestChannelPriority( affectedChannels, otherChannelId );

						COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnClose - channel %d has no more listeners, enabling channel %d" ),iId, otherChannelId ) );

						(void)SetChannelAvailability( otherChannelId /* <- to enable */, iId /* <- to disable */, property );
						} //<- highest priority of the channel closed, no other listeners exist
					}//<- there are affected channels

				affectedChannels->Reset();
				delete affectedChannels;
				affectedChannels = NULL;

				}//<- affected channels returned a valid pointer
			}//<- Current channel is available, check if it needs to be set unavailable
		else
			{//not available ->
			COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnClose - channel not available (%d) or no affected channels" ), isAvailable ) );
			}//<- not available
		}//<- availability property read OK
	else
		{
		//Error reading property
		ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnClose - ERROR: Unable to get property  Error: %d" ), propErr ) );
		}
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::HandleDependencyOnClose - return" ) ) );
	}

// ---------------------------------------------------------------------------
// CSensrvChannel::SetAvailability()
// ---------------------------------------------------------------------------
//
TInt CSensrvChannel::SetAvailability( TSensrvChannelId aChannelId, TSensrvProperty& aProperty, TSensrvAvailability aValue, const TSecurityInfo& aSecurityInfo )
	{
	TInt propErr( KErrNone );

	aProperty.SetValue( static_cast<TInt>( aValue ) );
	aProperty.SetSecurityInfo( aSecurityInfo );

	TRAP( propErr, iPropertyProvider->SetPropertyL( aChannelId, aProperty ) );

	if ( propErr == KErrNone )
		{
		//property set ok
		COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetAvailability - Channel %d property set to %d OK" ), aChannelId, aValue ) );
		}
	else
		{
		//handle property error
		ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetAvailability - ERROR: Unable to set property for dependent channel - Error: %d" ), propErr ) );
		}

	return propErr;
	}

// ---------------------------------------------------------------------------
// CSensrvChannel::SetChannelAvailability()
// ---------------------------------------------------------------------------
//
TInt CSensrvChannel::SetChannelAvailability( TSensrvChannelId aChannelIdToEnable, TSensrvChannelId aChannelIdToDisable, TSensrvProperty& aProperty )
	{
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelAvailability" ) ) );

	TInt status ( KErrNone );
	TSecurityInfo securityInfo;

	securityInfo.SetToCurrentInfo();

	status = SetAvailability( aChannelIdToDisable, aProperty, ESensrvAvailabilityFalse, securityInfo );

	if ( KErrNone == status )
		{
		status = SetAvailability( aChannelIdToEnable, aProperty, ESensrvAvailabilityTrue, securityInfo );

		if ( KErrNone != status )
			{
			//Handle error
			ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelAvailability - ERROR: Unable to set channel %d to available - Error: %d" ), aChannelIdToEnable, status ) );

			//rewind
			(void)SetAvailability( aChannelIdToDisable, aProperty, ESensrvAvailabilityTrue, securityInfo );
			}
		}
	else
		{
		//Handle error
		ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelAvailability - ERROR: Unable to set channel %d to unavailable - Error: %d" ), aChannelIdToDisable, status ) );
		//nothing to rewind
		}

	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelAvailability() - return %d" ), status ) );

	return status;
	}

// ---------------------------------------------------------------------------
// CSensrvChannel::SetChannelAvailable()
// ---------------------------------------------------------------------------
//
TInt CSensrvChannel::SetChannelAvailability( TSensrvChannelId aChannelId, TSensrvProperty& aProperty, TSensrvAvailability aValue )
	{
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelAvailability" ) ) );

	TInt status ( KErrNone );
	TSecurityInfo securityInfo;

	securityInfo.SetToCurrentInfo();

	status = SetAvailability( aChannelId, aProperty, aValue, securityInfo );
	if ( KErrNone != status )
		{
		//Handle error
		ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelAvailability - ERROR: Unable to set channel %d to %d - Error: %d" ), aChannelId, aValue, status ) );
		}

	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::SetChannelAvailability() - return %d" ), status ) );

	return status;
	}

// ---------------------------------------------------------------------------
// Reads channel group
// ---------------------------------------------------------------------------
//
TSensrvResourceChannelInfo::TSensrvChannelGroup CSensrvChannel::ReadChannelGroup()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ReadChannelGroup()" ) ) );

    // Use not defined as default if not defined or read fails
    TInt channelGroup( TSensrvResourceChannelInfo::ESensrvChannelGroupNotDefined );
    TInt err( KErrNone );

    // Open channel group repository and get necessary values
    CRepository* repository = NULL;
    TRAP( err, repository = CRepository::NewL( KCRUidSensorServerChannelGroups ) );
    if ( err == KErrNone )
        {
        err = repository->Get( iChannelInfo.iChannelType, channelGroup );
        if ( err != KErrNone || channelGroup < TSensrvResourceChannelInfo::ESensrvChannelGroupNotDefined )
            {
            channelGroup = TSensrvResourceChannelInfo::ESensrvChannelGroupNotDefined;
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannel::ReadChannelGroup - Channel groups repository opening failed: %d" ), err ) );
        }

    // Cleanup repository
    delete repository;
    repository = NULL;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ReadChannelGroup() - return %d" ), channelGroup ) );

    return static_cast< TSensrvResourceChannelInfo::TSensrvChannelGroup >( channelGroup );
    }

// ---------------------------------------------------------------------------
// Gets channel group
// ---------------------------------------------------------------------------
//
TSensrvResourceChannelInfo::TSensrvChannelGroup CSensrvChannel::ChannelGroup()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ChannelGroup()" ) ) );

    if ( iChannelInfo.iChannelGroup == TSensrvResourceChannelInfo::ESensrvChannelGroupNotSolved )
        {
        iChannelInfo.iChannelGroup = ReadChannelGroup();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::ChannelGroup() - return %d" ), iChannelInfo.iChannelGroup ) );

    return iChannelInfo.iChannelGroup;
    }
