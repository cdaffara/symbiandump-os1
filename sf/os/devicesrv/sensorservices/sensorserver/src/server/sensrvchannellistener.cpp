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
* Description:  Sensor server channel listener implementation
*
*/


#include <centralrepository.h>
#include "sensrvdefines.h"
#include "senserverchannel.h"
#include "sensrvchannelbuffer.h"
#include "sensrvpluginproxy.h"
#include "sensrvsession.h"
#include "senserverchannellistener.h"
#include "sensrvclientserver.h"
#include "sensrvtimer.h"
#include "sensrvssymediator.h"
#include "sensrvpropertyqueue.h"
#include "sensrvconditionevaluator.h"
#include "sensrvchannelconditionsetimpl.h"
#include "sensrvprivatecrkeys.h"

// ======== LOCAL FUNCTIONS ========

static TInt SuccessIndicatorToError( TSetPropertySuccessIndicator aInd )
    {
    TInt ret( KErrNotFound );
    switch ( aInd )
        {
        case ESetPropertyIndicationAvailable:
            {
            ret = KSensrvErrPropertySetAvailable;
            break;
            }
        case ESetPropertyIndicationPossible:
            {
            ret = KSensrvErrPropertySetPossible;
            break;
            }
        case ESetPropertyIndicationUnavailable:
            {
            ret = KSensrvErrPropertySetUnavailable;
            break;
            }
        default:
            {
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelListener* CSensrvChannelListener::NewL(CSensrvSession& aSession,
                                                     CSensrvChannel& aChannel,
                                                     CSensrvPluginProxy& aProxy)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::NewL(aSession: 0x%x, aChannel: 0x%x, aProxy: 0x%x)" ), &aSession, &aChannel, aProxy.ImplementationUid().iUid ) );

    CSensrvChannelListener* self = new( ELeave ) CSensrvChannelListener(aSession, aChannel, aProxy);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelListener::CSensrvChannelListener(CSensrvSession& aSession,
                                               CSensrvChannel& aChannel,
                                               CSensrvPluginProxy& aProxy)
    : iSession(aSession),
      iChannel(aChannel),
      iProxy(aProxy),
      iState(EChannelListenerStateInactive),
      iAddedConditionSetList(_FOFF(CSensrvChannelConditionSetImpl,iLink)),
      iAddedConditionSetIter(iAddedConditionSetList),
      iMetConditionSetList(_FOFF(CSensrvChannelConditionSetImpl,iLink)),
      iMetConditionSetIter(iMetConditionSetList),
      iPriority( KErrUnknown )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CSensrvChannelListener()" ) ) );

    // Nothing to do

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CSensrvChannelListener - return" ) ) );
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvChannelListener::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::ConstructL()" ) ) );

    // Create buffering period timer
    iBufferingPeriodTimer = CSensrvTimer::NewL(*this, KSensrvMaximumBufferingPeriod*KMillisToMicros, CSensrvTimer::ETimerIdBufferPeriodTimer);

    iUnnotifiedProperties = CSensrvPropertyQueue::NewL();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvChannelListener::~CSensrvChannelListener()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::~CSensrvChannelListener()" ) ) );

    if(iClientDataMessage)
        {
        // Indicate that channel has been closed with error message
        iClientDataMessage->Complete(KErrNotFound);
        }

    if(iClientPropertyMessage)
        {
        // Indicate that channel has been closed with error message
        iClientPropertyMessage->Complete(KErrNotFound);
        }

    if(iClientConditionMessage)
        {
        // Indicate that channel has been closed with error message
        iClientConditionMessage->Complete(KErrNotFound);
        }

    if (iChannel.Buffer())
        {
        iChannel.Buffer()->RemoveListener(this);
        }

    ClearConditionQueues();

    iTailItem = NULL;

    delete iUnnotifiedProperties;
    delete iBufferingPeriodTimer;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::~CSensrvChannelListener - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Starts listening for either data or conditions, which in reality
// both listen for data; however, in condition listening it is not
// passed to client unless specified condition occurs in the datastream.
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelListener::StartListening(TInt aDesiredBufferingCount,
                                            TInt aMaximumBufferingCount,
                                            TInt aBufferingPeriod,
                                            TSensrvChannelListenerState aState)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::StartListening(aDesiredBufferingCount: %d, aMaximumBufferingCount: %d, aBufferingPeriod: %d, aState: %d)" ), aDesiredBufferingCount, aMaximumBufferingCount, aBufferingPeriod, aState ) );

    TInt err(KErrNone);
    TBool alreadyListening(EFalse);

    if ( iState & ( EChannelListenerStateConditionListening | EChannelListenerStateDataListening ) )
        {
        alreadyListening = ETrue;
        }

    // Add listener to buffer if not already listening
    if (!alreadyListening)
        {
        // Buffer must exist
        err = iChannel.Buffer()->AddListener(this, iTailItem, aState);
        }
    else
        {
        // Otherwise just update correct tail item
        if (aState == EChannelListenerStateDataListening)
            {
            iTailItem->iDataTailIndex = KSensrvChannelBufferUninitializedTail;
            }
        else if (aState == EChannelListenerStateConditionListening)
            {
            iTailItem->iConditionTailIndex = KSensrvChannelBufferUninitializedTail;
            }
        }

    if (err == KErrNone)
        {
        // Enforce maximum and minimum values on parameters
        // If already listening, use smaller values if they differ, except for max count.

        if ( !alreadyListening || aDesiredBufferingCount < iDesiredBufferingCount )
            {
            if (   aDesiredBufferingCount <= 0
                || aDesiredBufferingCount > iChannel.MaxBufferingCount())
                {
                iDesiredBufferingCount = iChannel.MaxBufferingCount();

                // A very small buffer may result in zero maximum count with some constant values
                // so enforce minumum.
                if (iDesiredBufferingCount < KSensrvMinimumBufferingCount)
                    {
                    iDesiredBufferingCount = KSensrvMinimumBufferingCount;
                    }
                }
            else
                {
                iDesiredBufferingCount = aDesiredBufferingCount;
                }
            }

        // Maximum buffering count is only interesting in data listening, so
        // always use data listening maximum count if it is active.
        if ( !alreadyListening || aState == EChannelListenerStateDataListening )
            {
            if (   aMaximumBufferingCount < iDesiredBufferingCount
                || aMaximumBufferingCount > iChannel.MaxBufferingCount())
                {
                iMaximumBufferingCount = iChannel.MaxBufferingCount();
                }
            else
                {
                iMaximumBufferingCount = aMaximumBufferingCount;
                }
            }

        if ( !alreadyListening || aBufferingPeriod*KMillisToMicros < iBufferingPeriod.Int() )
            {
            if (aBufferingPeriod > KSensrvMaximumBufferingPeriod)
                {
                iBufferingPeriod = KSensrvMaximumBufferingPeriod*KMillisToMicros;
                }
            else if (aBufferingPeriod <= 0)
                {
                iBufferingPeriod = 0;
                }
            else if (aBufferingPeriod < KSensrvMinimumBufferingPeriod)
                {
                // Enforce minimum period to avoid excessive load.
                iBufferingPeriod = KSensrvMinimumBufferingPeriod*KMillisToMicros;
                }
            else
                {
                // Buffering period ok, do nothing
                iBufferingPeriod = aBufferingPeriod*KMillisToMicros;
                }
            }

        // Reset buffering period timer if not yet active
        if (iBufferingPeriod.Int() && !iBufferingPeriodTimer->IsActive())
            {
            iBufferingPeriodTimer->Set(iBufferingPeriod);
            iBufferingPeriodExpired = EFalse;
            }

        // Force initial channel data send from NewDataAvailable() when listening to state channel
        if ( iChannel.ChannelGroup() == TSensrvResourceChannelInfo::ESensrvChannelGroupState &&
             !( iState & EChannelListenerStateDataListening ) &&
              ( aState & EChannelListenerStateDataListening ) )
            {
            iInitialDataSent = EFalse;
            }
        else
            {
            iInitialDataSent = ETrue;
            }

        // Set state
        iState |= aState;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::StartListening - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Stops listening for either data or conditions. Actual data listening
// is not ceased, if there is other type of listening still going on.
// ---------------------------------------------------------------------------
//
void CSensrvChannelListener::StopListening(TSensrvChannelListenerState aState,
                                           TBool aDeleteConditions)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::StopListening(aState: %d, aDeleteConditions: %d)" ), aState, aDeleteConditions ) );

    // Clear state
    iState &= ~aState;

    // Remove listener from buffer if no longer needed
    if (!(    iState & EChannelListenerStateConditionListening
           || iState & EChannelListenerStateDataListening))
        {
        if (iChannel.Buffer())
            {
            iChannel.Buffer()->RemoveListener(this);
            }

        iTailItem = NULL;

        iBufferingPeriodTimer->Cancel();
        }
    else
        {
        // Just clean the stopped state(s)
        if (!(iState & EChannelListenerStateConditionListening))
            {
            iTailItem->iConditionTailIndex = KSensrvChannelBufferUnusedTail;
            iTailItem->iConditionDataAvailable = 0;
            }

        if (!(iState & EChannelListenerStateDataListening))
            {
            iTailItem->iDataTailIndex = KSensrvChannelBufferUnusedTail;
            iTailItem->iDataAvailable = 0;
            }
        }

    // Clear condition queues if stopping condition listening
    if (aState & EChannelListenerStateConditionListening
        && aDeleteConditions)
        {
        ClearConditionQueues();
        }

    // Cancel data listening message
    if(!(iState & EChannelListenerStateDataListening) && iClientDataMessage)
        {
        // Indicate that listening has been stopped with error message
        iClientDataMessage->Complete(KErrCancel);
        iClientDataMessage = NULL;
        }

    // Cancel condition listening message
    if(!(iState & EChannelListenerStateConditionListening) && iClientConditionMessage)
        {
        // Indicate that listening has been stopped with error message
        iClientConditionMessage->Complete(KErrCancel);
        iClientConditionMessage = NULL;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::StopListening - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Checks buffer if there is enough data to send to client and/or if condition
// has occurred, depending on state.
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelListener::NewDataAvailable()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::NewDataAvailable()" ) ) );

    TInt err(KErrNone);

    // Condition evaluation
    if (  ( iState & EChannelListenerStateConditionListening )
        && !iAddedConditionSetList.IsEmpty()
       )
        {
        if (iTailItem->iConditionDataAvailable)
            {
            EvaluateConditionsForNewData();
            }
        else
            {
            // Need to check for newly added conditions anyway to clear IsJustAdded flags
            CSensrvChannelConditionSetImpl* ptr = NULL;
            iAddedConditionSetIter.SetToFirst();
            while ( (ptr = iAddedConditionSetIter++) != NULL )
                {
                if (ptr->IsJustAdded())
                    {
                    ptr->ClearJustAdded();
                    }
                }
            }
        }

    // If there is no message waiting, there is no point in checking for new data.
    if (   iClientDataMessage
        && (iState & EChannelListenerStateDataListening))
        {
        // Check if there is enough data
        if ( !iInitialDataSent
            || iBufferingPeriodExpired
            || iTailItem->iDataAvailable >= iDesiredBufferingCount )
            {
            TChannelDataMessage data;
            iChannel.Buffer()->GetMessageData(iTailItem, data, iMaximumBufferingCount);

            // Get the initial data if there is no actual data at the moment
            if ( !iInitialDataSent && data.IsEmpty() )
                {
                iChannel.Buffer()->GetInitialMessageData( data );
                }

            // Do send only if there is data or forced by timer
            if ( !data.IsEmpty() || iBufferingPeriodExpired )
                {
                // Set data to client data transaction and complete transaction
                err = SetMessageData(data);
                iClientDataMessage->Complete(err);
                iClientDataMessage = NULL;

                // If data was sent, initial data is not needed anymore
                if ( !data.IsEmpty() )
                    {
                    iInitialDataSent = ETrue;
                    }

                // Reset buffering period timer
                if (iBufferingPeriod.Int())
                    {
                    iBufferingPeriodTimer->Set(iBufferingPeriod);
                    iBufferingPeriodExpired = EFalse;
                    }
                }
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::NewDataAvailable - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Store changed property to property queue
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelListener::StoreChangedProperty(const TSensrvProperty& aProperty)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::StoreChangedProperty(Property ID: %d)" ), aProperty.GetPropertyId() ) );

    TInt err( KErrNone );

    if (iState & EChannelListenerStatePropertyListening)
        {
        iUnnotifiedProperties->Append(aProperty);
        }
    else
        {
        //This client is not listening property changes
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::StoreChangedProperty - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Complete property changed notification towards client.
// Do nothing if there is no new property available
// ---------------------------------------------------------------------------
//
void CSensrvChannelListener::HandleNextProperty()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::HandleNextProperty()" ) ) );

    CompleteSetPropertySuccessInd();

    if( iClientPropertyMessage )
        {
        TSensrvProperty* property = iUnnotifiedProperties->First();

        if (property)
            {
            TPropertyPckgBuf propertyPckgBuf( *property );

            TInt err(iClientPropertyMessage->Write( KSensrvAsyncPropertyDataPropertySlot, propertyPckgBuf ));

            iClientPropertyMessage->Complete( err );
            iClientPropertyMessage = NULL;

            iUnnotifiedProperties->RemoveFirst();

            if (err != KErrNone)
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::HandleNextProperty - ERROR: Failed to write changed property: %d, change is lost." ), err ) );
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::HandleNextProperty - No property to handle" ) ) );
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::HandleNextProperty - no client property message, postponing notification until message available" ) ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::HandleNextProperty - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelListener::Priority
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelListener::Priority()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::Priority()" ) ) );

    if( KErrUnknown == iPriority )
        {
        iPriority = ReadPriority();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::Priority - return %d" ), iPriority ) );

    return iPriority;
    }

// ---------------------------------------------------------------------------
// CSensrvChannelListener::ReadPriority
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelListener::ReadPriority()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::ReadPriority()" ) ) );
    TInt priority( 0 );

    TInt err(KErrNone);

    // Open priority repository and get necessary values
    CRepository* repository = NULL;
    TRAP( err, repository = CRepository::NewL( KCRUidSensorServerPolicy ) );

    if ( err == KErrNone )
        {
        err = repository->Get( iSession.SecureId(), priority );
        if ( err == KErrNone )
            {
            //OK
            }
        else
            {
            //If the SID is not found from cenrep it is interpreted as zero priority (lowest)
            priority = 0;
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::ReadPriority - Priority repository opening failed: %d" ), err) );
        }

    // Cleanup repository
    delete repository;
    repository = NULL;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::ReadPriority - return %d" ),priority ) );
    return priority;
    }

// -----------------------------------------------------------------------------
// Handles message according to message type
// -----------------------------------------------------------------------------
//
void CSensrvChannelListener::DispatchMessage( CSensrvMessage& aMessage )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage(aMessage.Function(): %d)" ), aMessage.Function() ) );

    TInt err(KErrNone);

    // Check command code and call appropriate function
    switch ( aMessage.Function() )
        {
        ///////////////////////////////////////////////////////////////////////
        case ESensrvSrvReqAsyncChannelData:
            {
            if (iState & EChannelListenerStateDataListening )
                {
                // If iClientDataMessage is still something else than NULL, something has failed.
                // No need to do anything about it though, as session manages message lifecycle,
                // and has already completed duplicate message.
                if (iClientDataMessage)
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - old client data message still exists)" ) ) );
                    }

                iClientDataMessage = &aMessage;

                // Check if there is new data immediately available, and
                // write data to message if it is.
                // Error is irrelevant, because NewDataAvailable() already
                // completes message, if there is an error.
                NewDataAvailable(); // Ignore error
                }
            else
                {
                err = KErrNotFound;
                }
            break;
            }

        ///////////////////////////////////////////////////////////////////////
        case ESensrvSrvReqAsyncPropertyData:
            {
            // If iClientPropertyMessage is still something else than NULL, something has failed.
            // No need to do anything about it though, as session manages message lifecycle,
            // and has already completed duplicate message.
            if ( iClientPropertyMessage )
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - old client property message still exists)" ) ) );
                }

            iClientPropertyMessage = &aMessage;

            iState |= EChannelListenerStatePropertyListening;

            // Check if there is new change immediately available
            HandleNextProperty();
            }
            break;

        ///////////////////////////////////////////////////////////////////////
        case ESensrvSrvReqStopPropertyListening:
            {
            if ( iClientPropertyMessage )
                {
                iClientPropertyMessage->Complete( KErrCancel );
                iClientPropertyMessage = NULL;
                }
            aMessage.Complete( KErrNone );

            iState &= ~EChannelListenerStatePropertyListening;

            // Remove all changes that are not yet notified.
            iUnnotifiedProperties->RemoveAll();

            // Cancel pending property set success notification
            iSetPropertySuccessIndPending = EFalse;
            }
            break;

        ///////////////////////////////////////////////////////////////////////
        case ESensrvSrvReqAddConditionSet:
            {
            // Get evaluator plugin pointer
            if (!iConditionEvaluator)
                {
                TRAP(err, iConditionEvaluator = iChannel.ConditionEvaluatorL());
                }

            if (err == KErrNone)
                {
                CSensrvChannelConditionSetImpl* conditionSet = NULL;
                TRAP(err, conditionSet =
                            static_cast<CSensrvChannelConditionSetImpl*>
                            (CSensrvChannelConditionSet::NewL(ESensrvOrConditionSet)));

                if (err == KErrNone)
                    {
                    TInt serializedLength(aMessage.GetDesLength(KSensrvAddConditionSetDataSlot));
                    HBufC8* serializedSet = HBufC8::New(serializedLength);

                    if (serializedSet)
                        {
                        // Extract set
                        TPtr8 serializedSetPtr(serializedSet->Des());
                        err = aMessage.Read( KSensrvAddConditionSetDataSlot, serializedSetPtr );

                        if (err == KErrNone)
                            {
                            // Deserialize extracted set
                            err = conditionSet->Deserialize(*serializedSet);

                            if (err == KErrNone)
                                {
                                // Check condition set for internal consistency
                                if ( conditionSet->IsComplete() )
                                    {
                                    // Check that list doesn't already contain this set.
                                    TBool notFound(ETrue);
                                    CSensrvChannelConditionSetImpl* ptr = NULL;
                                    iAddedConditionSetIter.SetToFirst();

                                    while ( (ptr = iAddedConditionSetIter++) != NULL && notFound )
                                        {
                                        if (ptr->ConditionSetId() == conditionSet->ConditionSetId())
                                            {
                                            notFound = EFalse;
                                            }
                                        };

                                    if (notFound)
                                        {
                                        // Check condition set validity at plugin
                                        TBool valid(ETrue);
                                        const RSensrvChannelConditionList* conditionList = &(conditionSet->AllConditions());
                                        
                                        for ( TInt i = 0; i < conditionList->Count() && valid; i++)
                                            {
                                            CSensrvChannelCondition* condition1 = (*conditionList)[i];
                                            CSensrvChannelCondition* condition2 = NULL;

                                            if (   (*conditionList)[i]->ConditionType() ==
                                                        ESensrvRangeConditionLowerLimit )
                                                {
                                                // Since set is complete, there must exist upper
                                                // range limit after the lower range limit
                                                condition2 = (*conditionList)[++i];
                                                }

                                            TRAP( err, valid = iConditionEvaluator->CheckConditionValidityL(
                                                condition1,
                                                condition2) );

                                            if( err != KErrNone )
                                                {
                                                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - ERROR: CheckConditionValidityL err %d" ), err ) );
                                                valid = EFalse;
                                                }
                                            }
                                        
                                        if (valid)
                                            {
                                            if (!(iState & EChannelListenerStateConditionListening))
                                                {
                                                // If not yet condition listening, indicate that next data
                                                // should not be ignored
                                                conditionSet->ClearJustAdded();
                                                }
                                            else
                                                {
                                                // If condition listening, force buffer fill
                                                iChannel.ForceBufferFilled();
                                                }

                                            // Add condition set to list
                                            iAddedConditionSetList.AddLast(*conditionSet);

                                            aMessage.Complete(KErrNone);
                                            }
                                        else if( err == KErrNone )
                                            {
                                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - ERROR: Condition set is not valid" ) ) );
                                            err = KErrArgument;
                                            }
                                        }
                                    else
                                        {
                                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - ERROR: Condition set already added" ) ) );
                                        err = KErrAlreadyExists;
                                        }
                                    }
                                else
                                    {
                                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - ERROR: Condition set not complete" ) ) );
                                    err = KErrArgument;
                                    }
                                }
                            else
                                {
                                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - ERROR: Unable to deserialize condition set: %d" ), err ) );
                                }
                            }
                        else
                            {
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - ERROR: Unable to read serialized set from message: %d" ), err ) );
                            }

                        delete serializedSet;
                        serializedSet = NULL;
                        }
                    else
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - ERROR: Unable to create serialized set buffer" ) ) );
                        err = KErrNoMemory;
                        }
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - ERROR: Unable to create condition set: %d" ), err ) );
                    }

                // If there was an error, the new set has not been added to list,
                // and must therefore be deleted here.
                if (err != KErrNone && conditionSet)
                    {
                    delete conditionSet;
                    conditionSet = NULL;
                    }
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - ERROR: Unable to create condition evaluator: %d" ), err ) );
                }

            break;
            }

        ///////////////////////////////////////////////////////////////////////
        case ESensrvSrvReqRemoveConditionSet:
            {
            // Get id from message
            TInt setId(0);
            aMessage.ReadInt(KSensrvRemoveConditionSetIdSlot, setId);

            TBool found(EFalse);
            CSensrvChannelConditionSetImpl* ptr = NULL;

            // Remove condition set from the list
            iAddedConditionSetIter.SetToFirst();
            while ( (ptr = iAddedConditionSetIter++) != NULL && !found )
                {
                if (ptr->ConditionSetId() == setId)
                    {
                    found = ETrue;
                    iAddedConditionSetList.Remove(*ptr);
                    delete ptr;
                    }
                };

            // Remove any queued notifications for removed set.
            // Note that it is theoretically possible to have multiple
            // pending notifications for the same set (although different instance),
            // so always go through whole list.
            iMetConditionSetIter.SetToFirst();
            while ( (ptr = iMetConditionSetIter++) != NULL )
                {
                if (ptr->ConditionSetId() == setId)
                    {
                    iMetConditionSetList.Remove(*ptr);
                    delete ptr;
                    }
                };

            aMessage.Complete(KErrNone);

            break;
            }

        ///////////////////////////////////////////////////////////////////////
        case ESensrvSrvReqAsyncConditionMet:
            {
            if (iState & EChannelListenerStateConditionListening )
                {
                // If iClientConditionMessage is still something else than NULL, something has failed.
                // No need to do anything about it though, as session manages message lifecycle,
                // and has already completed duplicate message.
                if (iClientConditionMessage)
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - old client condition message still exists)" ) ) );
                    }

                iClientConditionMessage = &aMessage;

                // Check if there are met conditions immediately available
                if (!iMetConditionSetList.IsEmpty())
                    {
                    // Conditions available, so complete message immediately
                    CompleteConditionMessageD(*(iMetConditionSetList.First()));
                    }
                }
            else
                {
                err = KErrNotFound;
                }
            break;
            }

        ///////////////////////////////////////////////////////////////////////
        // Cannot identify the message
        default:
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - ERROR: unknown command" )) );
            err = KErrArgument;
            break;
            }
        }

    if (err != KErrNone)
        {
        aMessage.Complete(err);
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::DispatchMessage - return" )) );
    }

// ---------------------------------------------------------------------------
// Check how much data is available for this client in buffer and mod
// it with requested buffering count. If the result is positive, return it.
// If timer has fired, or remainder is zero, return full buffering count.
// Mod is taken instead of simple subtraction because we need to prepare always
// for the last async client data request instead of just the next one.
//
// Note: If client is lagging behind so much that data loss is occurring,
// the remainder will not be correct, so it is recommended that
// a reasonable buffering period is always used if it is not acceptable
// that some data might not be notified even if new channel buffered data
// + SSY bufferred data exceeds iDesiredBufferingCount.
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelListener::RemainingBufferingCount()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::RemainingBufferingCount()" ) ) );

    TInt remainingCount(iDesiredBufferingCount);

    // If there is dataloss, no sense to fine tune buffering count, instead always
    // use desired count, to reduce overhead and hopefully help with system load.
    //
    // Also if buffering period has expired, return full buffering count as remaining
    // count only if data available is less than full buffering count.
    // Otherwise there could be left an unmatched remainder for quite a while, especially
    // if buffering period caused buffer fill is rare for this client.
    //
    // Condition listening always uses full desired count, as conditions are checked
    // wherever new data becomes available.
    if (   !iTailItem->iDataLostCount
        && !(iBufferingPeriodExpired && iTailItem->iDataAvailable <= iMaximumBufferingCount)
        && iState == EChannelListenerStateDataListening)
        {
        if (iTailItem->iDataAvailable >= iMaximumBufferingCount )
            {
            // If data available is more than or equal to maximum count, deduct it from data available
            // for remainder determining purposes, as that is what is going to be
            // left after next client message handling.
            remainingCount -= (iTailItem->iDataAvailable - iMaximumBufferingCount) % iDesiredBufferingCount;
            }
        else if (iTailItem->iDataAvailable >= iDesiredBufferingCount )
            {
            // If available is more than desired count but less than maximum count,
            // use diff between max count and available count.
            TInt maxDiff = iMaximumBufferingCount - iTailItem->iDataAvailable;
            if (maxDiff < iDesiredBufferingCount)
                {
                remainingCount = maxDiff;
                }
            else
                {
                // iDesiredBufferingCount is ok as remainder
                }

            }
        else
            {
            // Otherwise just use difference
            remainingCount -= iTailItem->iDataAvailable;
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::RemainingBufferingCount - return %d" ), remainingCount ) );

    return remainingCount;
    }

// ---------------------------------------------------------------------------
// Forces buffer filled from Ssy.
// ---------------------------------------------------------------------------
//
void CSensrvChannelListener::TimerFired(TInt /*aTimerId*/)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::TimerFired()" ) ) );

    iProxy.Mutex().Wait();

    iBufferingPeriodExpired = ETrue;

    iChannel.ForceBufferFilled();

    iProxy.Mutex().Signal();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::TimerFired - return" ) ) );
    }


// ---------------------------------------------------------
// Sets data to be returned to client.
// ---------------------------------------------------------
//
TInt CSensrvChannelListener::SetMessageData(TChannelDataMessage& aData)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::SetMessageData(aData.iDataItemCount: %d)" ), aData.iDataItemCount ) );
    TInt err(KErrNone);

    // Dump data and counts to message if there is space
    if (iClientDataMessage)
        {
        TInt space = iClientDataMessage->GetDesMaxLength( KSensrvAsyncChannelDataBufferSlot );
        TInt lenFirst = aData.iFirstDataPart.Length();
        TInt lenSecond = aData.iSecondDataPart.Length();
        if ( space >=  lenFirst + lenSecond )
            {
            // It is possible that first data part is zero lenght, if tail pointer was at end
            // of valid buffer when data was fetched.
            if (lenFirst)
                {
                err = iClientDataMessage->Write( KSensrvAsyncChannelDataBufferSlot, aData.iFirstDataPart, 0 );
                }

            if (err == KErrNone)
                {
                if (lenSecond)
                    {
                    err = iClientDataMessage->Write( KSensrvAsyncChannelDataBufferSlot, aData.iSecondDataPart, aData.iFirstDataPart.Length() );

                    if (err != KErrNone)
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::SetMessageData - ERROR: 2nd part write to KSensrvAsyncChannelDataBufferSlot failed: %d" ), err ) );
                        }
                    }
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::SetMessageData - ERROR: Write to KSensrvAsyncChannelDataBufferSlot failed: %d" ), err ) );
                }

            if (err == KErrNone)
                {
                TSensrvAsyncChannelDataCountsRetval countData(aData.iDataItemCount, aData.iDataLostCount);
                TSensrvAsyncChannelDataCountsRetvalPckgBuf pckg(countData);

                space = iClientDataMessage->GetDesMaxLength( KSensrvAsyncChannelDataCountSlot );

                if ( space >= pckg.Length() )
                    {
                    err = iClientDataMessage->Write( KSensrvAsyncChannelDataCountSlot, pckg );

                    if (err != KErrNone)
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::SetMessageData - ERROR: Write to KSensrvAsyncChannelDataCountSlot failed: %d" ), err ) );
                        }
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::SetMessageData - ERROR: Not enough space to write to KSensrvAsyncChannelDataCountSlot" ) ) );
                    err = KErrOverflow;
                    }
                }
            }
        else    // Not enough space to write
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::SetMessageData - ERROR: Not enough space to write to KSensrvAsyncChannelDataBufferSlot" ) ) );
            err = KErrOverflow;
            }

        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::SetMessageData - ERROR: No message defined" ) ) );
        err = KErrBadHandle;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::SetMessageData - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------
// Completes asynchronous condition message
// and removes the set from list if successful.
// ---------------------------------------------------------
//
void CSensrvChannelListener::CompleteConditionMessageD(CSensrvChannelConditionSetImpl& aSet)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CompleteConditionMessageD(aSet.ConditionSetId(): %d)" ), aSet.ConditionSetId() ) );
    TInt err(KErrNone);

    __ASSERT_DEBUG(aSet.MetData(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullMetData));
    __ASSERT_DEBUG(iClientConditionMessage, User::Panic(KSensrvPanicCategory, ESensrvPanicNullMessage));

    TInt space = iClientConditionMessage->GetDesMaxLength( KSensrvAsyncConditionMetReturnDataSlot );
    if ( space >= aSet.MetData()->Size() )
        {
        err = iClientConditionMessage->Write( KSensrvAsyncConditionMetReturnDataSlot, *(aSet.MetData()) );

        if (err == KErrNone)
            {
            TSensrvTIntPckgBuf setIdPckgBuf( aSet.ConditionSetId() );

            space = iClientConditionMessage->GetDesMaxLength( KSensrvAsyncConditionMetReturnIdSlot );

            if ( space >= setIdPckgBuf.Length() )
                {
                err = iClientConditionMessage->Write( KSensrvAsyncConditionMetReturnIdSlot, setIdPckgBuf );

                if (err != KErrNone)
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CompleteConditionMessageD - ERROR: Write to KSensrvAsyncConditionMetReturnIdSlot failed: %d" ), err ) );
                    }
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CompleteConditionMessageD - ERROR: Not enough space to write to KSensrvAsyncConditionMetReturnIdSlot" ) ) );
                err = KErrOverflow;
                }
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CompleteConditionMessageD - ERROR: Write to KSensrvAsyncConditionMetReturnDataSlot failed: %d" ), err ) );
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CompleteConditionMessageD - ERROR: Not enough space to write to KSensrvAsyncConditionMetReturnDataSlot" ) ) );
        err = KErrOverflow;
        }

    // Remove set from met sets list if successful
    if (err == KErrNone)
        {
        iMetConditionSetList.Remove(aSet);
        delete &aSet;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CompleteConditionMessageD - Completing" ), err ) );

    // Complete message even if there is error. This will cause minor error to client and
    // hopefully it will provide a message that will not cause failures.
    iClientConditionMessage->Complete(err);
    iClientConditionMessage = NULL;
    }

// ---------------------------------------------------------------------------
// Evaluates conditions for new data.
// ---------------------------------------------------------------------------
//
void CSensrvChannelListener::EvaluateConditionsForNewData()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::EvaluateConditionsForNewData()" ) ) );

    // Condition evaluator must exist if there are conditions added to list.
    __ASSERT_DEBUG(iConditionEvaluator, User::Panic(KSensrvPanicCategory, ESensrvPanicNullEvaluator));

    // Get condition data from buffer
    TChannelDataParts data;
    iChannel.Buffer()->GetConditionData(iTailItem, data);

    // Reset buffering period timer if not also data listening
    if (   iBufferingPeriod.Int()
        && !(iState & EChannelListenerStateDataListening))
        {
        iBufferingPeriodTimer->Set(iBufferingPeriod);
        iBufferingPeriodExpired = EFalse;
        }

    TInt firstPartCount(data.iFirstDataPart.Size()/iChannel.DataItemSize());
    TInt secondPartCount(data.iSecondDataPart.Size()/iChannel.DataItemSize());

    // Check for set completions, ignore just added sets
    CSensrvChannelConditionSetImpl* ptr = NULL;
    iAddedConditionSetIter.SetToFirst();

    // Loop through all added sets
    while ( (ptr = iAddedConditionSetIter++) != NULL )
        {
        TBool singleMatch(ptr->ConditionSetType() == ESensrvOrConditionSet);
        TBool matchFound(EFalse);

        if (ptr->IsJustAdded())
            {
            // Do not process sets that have just been added
            ptr->ClearJustAdded();
            }
        else
            {
            const RSensrvChannelConditionList* conditionList = &(ptr->AllConditions());
            TPtrC8 activePtr(data.iFirstDataPart.Ptr(), iChannel.DataItemSize());

            TInt passCounter(KSensrvDataPartCount);
            TInt activePartCount(firstPartCount);

            // Loop once for each datapart with length.
            // If first part has no length, then second one is always empty, too.
            while (activePtr.Length() && passCounter && !matchFound)
                {
                // Loop through data items in data part
                for ( TInt i = 0; i < activePartCount && !matchFound; i++)
                    {
                    TInt matchCount(0);
                    TInt checkCount(0);

                    // Check each condition in set against data item.
                    // For OR-sets, finding single match is reason to exit loop.
                    // For AND-sets, finding single mismatch is reason to exit loop.
                    for ( TInt j = 0;
                          j < conditionList->Count()
                          && ( (   singleMatch && !matchCount)
                                || !singleMatch && matchCount == checkCount );
                          j++)
                        {
                        CSensrvChannelCondition* condition1 = (*conditionList)[j];
                        CSensrvChannelCondition* condition2 = NULL;

                        if ( (*conditionList)[j]->ConditionType() ==
                                    ESensrvRangeConditionLowerLimit )
                            {
                            // Since set is complete, there must exist upper
                            // range limit after the lower range limit
                            condition2 = (*conditionList)[++j];
                            }

                        checkCount++;

                        TBool isMatch( EFalse );

                        TRAPD( err, isMatch = iConditionEvaluator->EvaluateConditionL(
                                condition1,
                                condition2,
                                activePtr) );

                        if ( err == KErrNone && isMatch )
                            {
                            matchCount++;
                            }
                        else if( err )
                            {
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::EvaluateConditionsForNewData - ERROR: EvaluateConditionL err %d" ), err ) );
                            }
                        else
                            {
                            // Ok, no match, do nothing
                            }
                        }

                    // Check if required amount of matches was found
                    if (   (singleMatch && matchCount)
                        || (!singleMatch && matchCount == checkCount) )
                        {
                        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::EvaluateConditionsForNewData - Match found" ) ) );
                        matchFound = ETrue;

                        // Set met data
                        TInt setMetErr(ptr->SetMetData(activePtr));

                        if (setMetErr == KErrNone)
                            {
                            // Remove matched set from added list and add it to met list
                            iAddedConditionSetList.Remove(*ptr);
                            iMetConditionSetList.AddLast(*ptr);
                            }
                        else
                            {
                            // OOM situation, can't do much about this, so ignore the match this time
                            matchFound = EFalse;
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::EvaluateConditionsForNewData - ERROR: Setting met data to set failed; ignore found match..." ) ) );
                            }
                        }

                    // Set active pointer to next data item
                    activePtr.Set(activePtr.Ptr() + iChannel.DataItemSize(), iChannel.DataItemSize());
                    }

                activePtr.Set(data.iSecondDataPart);
                activePartCount = secondPartCount;
                passCounter--;
                }// while (passCounter)
            }
        } // while (added sets iterator)


    // If async message is waiting and some conditions were met, notify first item from queue immediately
    if (!iMetConditionSetList.IsEmpty() && iClientConditionMessage)
        {
        CompleteConditionMessageD(*(iMetConditionSetList.First()));
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::EvaluateConditionsForNewData - return " )) );
    }

// ---------------------------------------------------------------------------
// Clears condition queues.
// ---------------------------------------------------------------------------
//
void CSensrvChannelListener::ClearConditionQueues()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::ClearConditionQueues()" ) ) );

    CSensrvChannelConditionSetImpl* ptr = NULL;
    iAddedConditionSetIter.SetToFirst();
    while ( (ptr = iAddedConditionSetIter++) != NULL )
        {
        delete ptr;
        };

    iMetConditionSetIter.SetToFirst();
    while ( (ptr = iMetConditionSetIter++) != NULL )
        {
        delete ptr;
        };

    iAddedConditionSetList.Reset();
    iMetConditionSetList.Reset();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::ClearConditionQueues - return " )) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelListener::CompleteSetPropertySuccessInd()
// ---------------------------------------------------------------------------
//
void CSensrvChannelListener::CompleteSetPropertySuccessInd()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CompleteSetPropertySuccessInd()" ) ) );

    if( iSetPropertySuccessIndPending && iClientPropertyMessage )
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CompleteSetPropertySuccessInd - Complete %d" ),
            iSetPropertySuccessInd ) );

        TInt err( SuccessIndicatorToError( iSetPropertySuccessInd ) );
        iClientPropertyMessage->Complete( err );
        iClientPropertyMessage = NULL;
        iSetPropertySuccessIndPending = EFalse;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::CompleteSetPropertySuccessInd - return" )) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelListener::HandleSetPropertySuccessInd()
// ---------------------------------------------------------------------------
//
void CSensrvChannelListener::HandleSetPropertySuccessInd(
        const TPropertySetSuccessEvaluationData& aEvaluationData )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::HandleSetPropertySuccessInd()" ) ) );

    TSetPropertySuccessIndicator newInd( EvaluatePriorities( aEvaluationData ) );
    if ( newInd != iSetPropertySuccessInd )
        {
        iSetPropertySuccessInd = newInd;

        // Inform change to client if property listening is enabled
        if ( iState & EChannelListenerStatePropertyListening )
            {
            iSetPropertySuccessIndPending = ETrue;
            CompleteSetPropertySuccessInd();
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::HandleSetPropertySuccessInd - listener: 0x%x priority: %d ind: %d return" ),
        this, iPriority, iSetPropertySuccessInd ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelListener::EvaluatePriorities()
// ---------------------------------------------------------------------------
//
TSetPropertySuccessIndicator CSensrvChannelListener::EvaluatePriorities(
        const TPropertySetSuccessEvaluationData& aEvaluationData )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelListener::EvaluatePriorities()" ) ) );

    TInt priority( Priority() );
    if ( priority == aEvaluationData.iHighestPriorityOfAffectedChannels &&
         aEvaluationData.iMultiplesOnAffectedChannels == 1 )
        {
        // Setting of a property is certain to succeed because client has the highest priority
        // of all clients that have open channels on that sensor device
        return ESetPropertyIndicationAvailable;
        }
    if ( priority < aEvaluationData.iHighestPriorityOfChannel )
        {
        // Setting of a property will not succeed because:
        // 1. There are clients on the same channel with higher priority
        return ESetPropertyIndicationUnavailable;
        }
    CSensrvChannelListener* controlListener = iChannel.TakenPropertySetControl();
    if ( controlListener && controlListener != this )
        {
        // Setting of a property will not succeed because:
        // 2. A client of the same priority is already setting properties on channel.

        // Note that if affected channel got the control with the same priority,
        // it may be possible that some property sets are allowed.
        return ESetPropertyIndicationUnavailable;
        }
    // Setting of a property may not succeed because:
    // 1. Client has the highest priority on its own channel however there are multiple clients
    // with the same priority and control is granted on a first come first served basis.
    // 2. Client has highest priority on its own channel however there are higher priority clients
    // on other channels of the device that may be affected by the setting, in which case it will
    // not be allowed.
    // 3. Client has highest priority on its channel however there are same priority
    // clients on other channels of the device that may be affected by the setting, in which
    // case it will not be allowed
    return ESetPropertyIndicationPossible;
    }
