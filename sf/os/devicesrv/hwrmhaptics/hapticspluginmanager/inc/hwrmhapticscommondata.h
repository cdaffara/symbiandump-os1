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
* Description:  Haptics common data class declaration.
*
*/


#ifndef C_HWRMHAPTICSCOMMONDATA_H
#define C_HWRMHAPTICSCOMMONDATA_H

#include <e32base.h>
#include <hwrmhapticsobserver.h>
#include <hwrmlogicalactuators.h>
#include <hwrmhapticsactuatorobserver.h>
#include "hwrmhapticsclientserver.h"

/**
 * Container for haptics data that is common for all sessions.
 *
 * @lib hwrmhapticspluginmanager.lib
 * @since S60 5.1
 */
class CHWRMHapticsCommonData : public CBase
    {
public:
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CHWRMHapticsCommonData* NewL();
        
    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsCommonData();
        
public:
    
    /**
     * Adds a new session to client array. This method is used, when
     * a session to haptics server is created.
     *
     * @param aSession Session pointer used to identify the client to 
     * be added.
     * Function may leave with Symbian OS error code
     */
    void AddSessionL( const CSession2* aSession );
    
    /**
     * Removes a session from the client array. This method is used,
     * when a session to haptics server is closed.
     *
     * @param aSession Session pointer used to identify the client to 
     * be removed.
     */
    void RemoveSession( const CSession2* aSession );
    
    /**
     * Adds the given message as a haptics status observer for a client
     * identified using the session of the message.
     *
     * @param aStatusObserverMsg New observer to be added.
     */
    void AddStatusObserver( const RMessage2& aStatusObserverMsg );

    /**
     * Notifies the given haptics status to the observer identified
     * with the given session.
     *
     * @param aStatus Status value to be notified with.
     * @param aSession Session pointer used to identify the client to 
     * be notified.
     */
    void NotifyStatus( MHWRMHapticsObserver::THWRMHapticsStatus aStatus,
                       const CSession2* aSession );

    /**
     * Notifies the last event of the given actuator to the observer
     * identified with the given session.
     *
     * @param aActuator The type of the actuator, whose last event should
     * be notified.
     * @param aSession Session pointer used to identify the client to 
     * be notified.
     */
    void NotifyActuatorEvent( THWRMLogicalActuators aActuator,
                              const CSession2* aSession );

    /**
     * Notifies all observers except the one identified with the given
     * session using the given haptics status.
     * 
     * @param aStatus Haptics status value to be notified with.
     * @param aSession Session pointer of the client, which should not 
     * be notified.This can be used e.g. when a client makes an active 
     * reservation; In that case it is unnecessary to notify that client, 
     * but all the others; The client making the reservation knows that 
     * the reservation succeeded from the return code of the command, 
     * and that it did not receive suspended -status.
     */
    void BroadcastStatus( MHWRMHapticsObserver::THWRMHapticsStatus aStatus,
                          const CSession2* aSession );

    /**
     * Notifies all observers listening to actuator events using the 
     * given event and actuator type values.
     *
     * @param aEvent Actuator event value to be notified with.
     * @param aActuator Logical actuator type to which this event is 
     * related to.
     */
    void BroadcastActuatorEvent( 
            MHWRMHapticsActuatorObserver::THWRMActuatorEvents aEvent,
            THWRMLogicalActuators aActuator );

    /**
     * Returns the current haptics status of the given session.
     * 
     * @param aSession Session pointer used to identify the client, whose
     * status is fetched.
     * @return Current status value of the client.
     */
    MHWRMHapticsObserver::THWRMHapticsStatus 
        CurrentStatus( const CSession2* aSession ) const;

private:

    /**
     * Internal typedef for haptics status enumeration.
     */
    typedef MHWRMHapticsObserver::THWRMHapticsStatus THapticsStatus;

    /**
     * Internal typedef for actuator event enumeration.
     */
    typedef MHWRMHapticsActuatorObserver::THWRMActuatorEvents TActuatorStatus;

private:
    
    /**
     * C++ default constructor.
     */
    CHWRMHapticsCommonData();

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * Returns the index of the observer with given session.
     *
     * @param aSession Session pointer, which identifies the client.
     * @return Index of the observer in the internal array, if found.
     * KErrNotFound otherwise.
     */
    TInt FindClient( const CSession2* aSession ) const;

    /**
     * Returns the index of the actuator event in the event array.
     *
     * @param aActuator Logical actuator type to be found.
     * @return Index of the observer in the internal array, if found.
     * KErrNotFound otherwise.
     */
    TInt FindActuatorEvent( THWRMLogicalActuators aActuator ) const;

    /**
     * Updates given actuators status, or if not found, inserts new
     * data for the actuator.
     *
     * @param aStatus Event value for the actuator.
     * @param aActuator Logical actuator type.
     */
    void StoreActuatorEvent( TActuatorStatus aStatus, 
                             THWRMLogicalActuators aActuator );
    
    /**
     * Implements the actual haptics status notification to the client.
     *
     * @param aIndex Index of the client in the internal array.
     * @param aStatus Haptics status value to be sent to client.
     */
    void NotifyStatusToClient( TInt aIndex, THapticsStatus aStatus );

    /**
     * Implements the actual actuator event notification to the client.
     *
     * @param aIndex Index of the client in the internal array.
     * @param aStatus Actuator event value to be sent to client.
     * @param aActuator Logical actuator type to which this event is
     * related to (sent to client also).
     */
    void NotifyActuatorEventToClient( TInt aIndex, TActuatorStatus aStatus,
                                      THWRMLogicalActuators aActuator );

private:

    /**
     * Data storage for a single client's status handling. Same application
     * may have several haptics client instances, in which case each session
     * generates a new client status data.
     */
    struct TClientStatus
        {
        /**
         * C++ constructor. By default client observes only haptics status,
         * so that earlier actuator events are not sent to client, when
         * it starts observation (creation of client).
         * 
         * @param aSession Session object identifying the client session.
         * @param aStatus Initial haptics status.
         */
        TClientStatus( const CSession2* aSession, THapticsStatus aStatus )
            : iSession( aSession ), iStatus( aStatus ), 
              iStatusTypes( EHWRMHapticsSessionStatus ), 
              iNotificationRequired( ETrue )
            {
            }
        
        /**
         * Destructor.
         */
        ~TClientStatus()
            {
            iRequiredActuators.Close();
            }
        
        /**
         * Status observation message received from the client.
         * The handle of the message is used to check whether or not
         * client has requested status notification.
         */
        RMessage2 iStatusObserver;

        /**
         * Pointer to the observing client's session, which is used to 
         * identify the client instance.
         */
        const CSession2* iSession;

        /**
         * Current status of this client. Needs to be stored here, since
         * it can be queried by the client without requested notification.
         */
        THapticsStatus iStatus;

        /**
         * What kind of status types has the client requested to be 
         * notified to it in general.
         */
        THWRMHapticsStatusTypes iStatusTypes;

        /**
         * Whether or not a notification should be sent to the client
         * immediately when it requests notification. The value is true,
         * when the client's status has changed, but it has not been sent
         * to the client, since there is no registered observation message
         * to use for notification. Basically, this flag has two different
         * cases, where it is needed:
         * 1. Client registeres an observation message for the first time,
         *    in which case the current status needs to be notified to client.
         * 2. Status changes in between last notification and the registration
         *    of the next observing message. In this case this flag is set
         *    true --> notification is sent, when the client requests
         *    notification for the next time.
         */
        TBool iNotificationRequired;

        /**
         * List of actuators, which have events to notify to client, 
         * but have not been notified yet. Used in similar matter as
         * iNotificationRequired, but for actuator events.
         */
        RArray<THWRMLogicalActuators> iRequiredActuators;
        };

    /**
     * Actuator event data for an actuator. Used to store last 
     * actuator specific events.
     */
    struct TActuatorEvent
        {
        /**
         * C++ constructor.
         */
        TActuatorEvent( TActuatorStatus aStatus,
                        THWRMLogicalActuators aActuator )
            : iActuatorEvent( aStatus ), iActuator( aActuator )
            {
            }
        
        /**
         * Actuator event.
         */
        TActuatorStatus iActuatorEvent;

        /**
         * Logical actuator type.
         */
        THWRMLogicalActuators iActuator;
        };

private:
    
    /**
     * Last haptics status value, which has been broadcasted. This value
     * is set to client data, when client's status data is created (when its
     * session is created).
     */
    THapticsStatus iGeneralStatus;

    /**
     * Stores last received actuator events for each actuator type.
     */
    RArray<TActuatorEvent> iActuatorEvents;
    
    /**
     * Array containing all observers listening to haptics status.
     * Pointers are not owned by this component.
     */
    RPointerArray<TClientStatus> iClientArray;
    };

#endif  // C_HWRMHAPTICSCOMMONDATA_H
            
// End of File
