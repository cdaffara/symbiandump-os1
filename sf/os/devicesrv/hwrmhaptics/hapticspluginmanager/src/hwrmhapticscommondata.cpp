/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics common data class definition
*
*/


#include "hwrmhapticstrace.h"
#include "hwrmhapticscommondata.h"


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsCommonData* CHWRMHapticsCommonData::NewL()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsCommonData::NewL()" ) ) );

    CHWRMHapticsCommonData* self = new( ELeave ) CHWRMHapticsCommonData();
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsCommonData::NewL - return 0x%x" ), self ) );
    
    return self;
    }
   
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsCommonData::~CHWRMHapticsCommonData()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsCommonData::~CHWRMHapticsCommonData()" ) ) );

    iClientArray.ResetAndDestroy();
    iClientArray.Close();

    iActuatorEvents.Close();
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsCommonData::~CHWRMHapticsCommonData - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Adds a new session to client array.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::AddSessionL( const CSession2* aSession )
    {
    // create new client status data
    TClientStatus* clientStatus = 
        new (ELeave) TClientStatus( aSession, iGeneralStatus );
    CleanupStack::PushL( clientStatus );
    
    iClientArray.AppendL( clientStatus );
    
    CleanupStack::Pop( clientStatus );
    }

// ---------------------------------------------------------------------------
// Removes a session from the client array.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::RemoveSession( const CSession2* aSession )
    {
    // find the index of the client
    TInt index = FindClient( aSession );
    
    // remove status related data of the session/client
    if ( index != KErrNotFound )
        {
        // complete the observer, if it has been set
        if ( iClientArray[index]->iStatusObserver.Handle() )
            {
            // with completion code KErrCancel the client should stop 
            // requesting notifications
            iClientArray[index]->iStatusObserver.Complete( KErrCancel );
            }

        // delete and remove session/client from the array
        delete iClientArray[index];
        iClientArray.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
// Adds the given message as a haptics status observer.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::AddStatusObserver( 
                                        const RMessage2& aStatusObserverMsg )
    {
    // find the index of the client
    TInt index = FindClient( aStatusObserverMsg.Session() );
    
    if ( index != KErrNotFound )
        {
        // set new observer message
        iClientArray[index]->iStatusObserver = aStatusObserverMsg;
        
        // read and set request types value
        THWRMHapticsStatusTypes reqTypes = 
            static_cast<THWRMHapticsStatusTypes>( aStatusObserverMsg.Int0() );

        iClientArray[index]->iStatusTypes = reqTypes;
        
        // send notification of the current status, if it is needed
        // for this client
        if ( iClientArray[index]->iNotificationRequired )
            {
            NotifyStatus( iClientArray[index]->iStatus, 
                          aStatusObserverMsg.Session() );

            iClientArray[index]->iNotificationRequired = EFalse;
            }
        else if ( iClientArray[index]->iRequiredActuators.Count() )
            {
            // there are unsent actuator event notifications, 
            // send immediately
            THWRMLogicalActuators actuator = 
                iClientArray[index]->iRequiredActuators[0];
            
            // search for the event
            TInt eventIndex = FindActuatorEvent( actuator );
            
            // send notification, if event found
            if ( eventIndex != KErrNotFound )
                {
                NotifyActuatorEventToClient( 
                                index, 
                                iActuatorEvents[eventIndex].iActuatorEvent, 
                                iActuatorEvents[eventIndex].iActuator );
                }
            
            // remove the actuator from required event notification array
            iClientArray[index]->iRequiredActuators.Remove( 0 );
            }
        }
    }

// ---------------------------------------------------------------------------
// Notifies the given haptics status to the observer identified
// with the given session, if the observer message exists.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::NotifyStatus( 
                            MHWRMHapticsObserver::THWRMHapticsStatus aStatus,
                            const CSession2* aSession )
    {
    // get the index of the observer to be notified
    TInt index = FindClient( aSession );
    
    // notify observer and store status
    NotifyStatusToClient( index, aStatus );
    }

// ---------------------------------------------------------------------------
// Notifies the last event of the given actuator to the observer
// identified with the given session.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::NotifyActuatorEvent( 
                                            THWRMLogicalActuators aActuator,
                                            const CSession2* aSession )
    {
    // get the index of the observer to be notified
    TInt clientIndex = FindClient( aSession );
    
    // try to find the event data
    TInt eventIndex = FindActuatorEvent( aActuator );

    // if client and event were found, notify client
    if ( clientIndex != KErrNotFound && eventIndex != KErrNotFound )
        {
        NotifyActuatorEventToClient( 
                        clientIndex, 
                        iActuatorEvents[eventIndex].iActuatorEvent, 
                        iActuatorEvents[eventIndex].iActuator );
        }
    }

// ---------------------------------------------------------------------------
// Notifies all observers except the one identified with the given
// session using the given haptics status.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::BroadcastStatus( 
                            MHWRMHapticsObserver::THWRMHapticsStatus aStatus,
                            const CSession2* aSession )
    {
    // notify all clients, except the one with given session
    for ( TInt i = 0; i < iClientArray.Count(); ++i )
        {
        if ( iClientArray[i]->iSession != aSession )
            {
            NotifyStatusToClient( i, aStatus );
            }
        }

    iGeneralStatus = aStatus;
    }

// ---------------------------------------------------------------------------
// Notifies all observers listening to actuator events using the 
// given event and actuator type values.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::BroadcastActuatorEvent( 
                MHWRMHapticsActuatorObserver::THWRMActuatorEvents aEvent,
                THWRMLogicalActuators aActuator )
    {
    // notify all clients listening to actuator events
    for ( TInt i = 0; i < iClientArray.Count(); ++i )
        {
        if ( iClientArray[i]->iStatusTypes == EHWRMHapticsActuatorStatus ||
             iClientArray[i]->iStatusTypes == EHWRMHapticsBothStatus )
            {
            NotifyActuatorEventToClient( i, aEvent, aActuator );
            }
        }
    
    // store actuator event, so that it can be sent, if notification did not
    // succeed above
    StoreActuatorEvent( aEvent, aActuator );
    }

// ---------------------------------------------------------------------------
// Returns the current status of the given session.
// ---------------------------------------------------------------------------
//
MHWRMHapticsObserver::THWRMHapticsStatus
CHWRMHapticsCommonData::CurrentStatus( const CSession2* aSession ) const
    {
    // get the index of the client
    TInt index = FindClient( aSession );

    return iClientArray[index]->iStatus;
    }

// ---------------------------------------------------------------------------
// Returns the index of the observer with given session.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsCommonData::FindClient( const CSession2* aSession ) const
    {
    TInt ret = KErrNotFound;
    
    // find the observer with the given session
    for ( TInt i = 0; i < iClientArray.Count() && ret == KErrNotFound; ++i )
        {
        if ( iClientArray[i]->iSession == aSession )
            {
            ret = i;
            }
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// Returns the index of the actuator event in the event array.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsCommonData::FindActuatorEvent( 
                                    THWRMLogicalActuators aActuator ) const
    {
    TInt ret = KErrNotFound;
    
    // find the event with the given actuator
    for ( TInt i = 0; i < iActuatorEvents.Count() && ret == KErrNotFound; ++i )
        {
        if ( iActuatorEvents[i].iActuator == aActuator )
            {
            ret = i;
            }
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// Updates given actuators status, or if not found, inserts new
// data for the actuator.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::StoreActuatorEvent( 
                                        TActuatorStatus aStatus, 
                                        THWRMLogicalActuators aActuator )
    {
    // try to find the event data
    TInt index = FindActuatorEvent( aActuator );

    if ( index != KErrNotFound )
        {
        // data was found, update it
        iActuatorEvents[index].iActuatorEvent = aStatus;
        iActuatorEvents[index].iActuator = aActuator;
        }
    else
        {
        // data was not found, create new data
        TActuatorEvent event( aStatus, aActuator );
        iActuatorEvents.Append( event );
        }
    }

// ---------------------------------------------------------------------------
// Implements the actual notification to the client and stores the status
// for the indexed client data.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::NotifyStatusToClient( 
                        TInt aIndex,
                        MHWRMHapticsObserver::THWRMHapticsStatus aStatus )
    {
    // notify only if the client has an observer and client has requested
    // haptics status notification
    if ( iClientArray[aIndex]->iStatusObserver.Handle() && 
         ( iClientArray[aIndex]->iStatusTypes == EHWRMHapticsSessionStatus ||
           iClientArray[aIndex]->iStatusTypes == EHWRMHapticsBothStatus ) )
        {
        // create status package
        TPckg<THWRMHapticsStatusTypes> statusTypePckg( EHWRMHapticsSessionStatus );
        TPckg<TInt> statusPckg( aStatus );
        
        // write type and status to message and complete it. Completing the 
        // message sets the handle in the message to zero, which is used to 
        // notice that the message has already been completed.
        TInt err = iClientArray[aIndex]->iStatusObserver.Write( 1, statusTypePckg, 0 );
        err = iClientArray[aIndex]->iStatusObserver.Write( 2, statusPckg, 0 );
        iClientArray[aIndex]->iStatusObserver.Complete( err );
        }
    else
        {
        // status was not sent to client, so it needs to be sent, when
        // the client requests status notification for the next time
        iClientArray[aIndex]->iNotificationRequired = ETrue;
        }
    
    // store the status value for the client even if it was not sent to
    // an observer so that it can be fetched by a direct get status -command
    iClientArray[aIndex]->iStatus = aStatus;
    }

// ---------------------------------------------------------------------------
// Implements the actual actuator event notification to the client.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::NotifyActuatorEventToClient( 
                                            TInt aIndex, 
                                            TActuatorStatus aStatus,
                                            THWRMLogicalActuators aActuator )
    {
    // notify or store data for client only if client has requested actuator
    // event notification
    if ( iClientArray[aIndex]->iStatusTypes == EHWRMHapticsActuatorStatus ||
         iClientArray[aIndex]->iStatusTypes == EHWRMHapticsBothStatus )
        {
        // notify only if the client has an observer
        if ( iClientArray[aIndex]->iStatusObserver.Handle() )
            {
            // create type, status and actuator packages
            TPckg<THWRMHapticsStatusTypes> statusTypePckg( EHWRMHapticsActuatorStatus );
            TPckg<TInt> statusPckg( aStatus );
            TPckg<THWRMLogicalActuators> actuatorPckg( aActuator );
            
            // write type, status and actuator to message and complete it. 
            // Completing the message sets the handle in the message to zero, 
            // which is used to notice that the message has already been 
            // completed.
            TInt err = iClientArray[aIndex]->iStatusObserver.Write( 1, statusTypePckg, 0 );
            err = iClientArray[aIndex]->iStatusObserver.Write( 2, statusPckg, 0 );
            err = iClientArray[aIndex]->iStatusObserver.Write( 3, actuatorPckg, 0 );
            iClientArray[aIndex]->iStatusObserver.Complete( err );
            }
        else
            {
            // event was not sent to client, so it needs to be sent, when
            // the client requests notification for the next time
            TInt index = iClientArray[aIndex]->iRequiredActuators.Find( aActuator );
            if ( index == KErrNotFound )
                {
                // actuator was not in the list already --> needs to be added
                iClientArray[aIndex]->iRequiredActuators.Append( aActuator );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsCommonData::CHWRMHapticsCommonData()
    : iGeneralStatus( MHWRMHapticsObserver::EHWRMHapticsStatusAvailable )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsCommonData::CHWRMHapticsCommonData()" ) ) );
    COMPONENT_TRACE( ( _L( "CHWRMHapticsCommonData::CHWRMHapticsCommonData - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsCommonData::ConstructL()
    {    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsCommonData::ConstructL()" ) ) );
    COMPONENT_TRACE( ( _L( "CHWRMHapticsCommonData::ConstructL - return" ) ) );
    }

//  End of File  
