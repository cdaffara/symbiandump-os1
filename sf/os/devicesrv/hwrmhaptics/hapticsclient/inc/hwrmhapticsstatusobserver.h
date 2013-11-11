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
* Description:  Haptics status observer header file.
*
*/


#ifndef HWRMHAPTICSSTATUSOBSERVER_H
#define HWRMHAPTICSSTATUSOBSERVER_H

#include <e32base.h>
#include <hwrmhapticsobserver.h>
#include "hwrmhapticsclientserver.h"

class RHWRMHapticsGenericClient;
class MHWRMHapticsActuatorObserver;

/**
 * Active object for observing haptics status changes in the 
 * haptics server.
 *
 * @lib hwrmhapticsclient.lib
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CHWRMHapticsStatusObserver ): public CActive
    {
public:

    /**
     * Two-phased constructor. A newly created instance starts 
     * immediately listening to status changes.
     *
     * @param aHapticsCallback Pointer to a haptics status callback instance. 
     * If NULL, status values are not notified to the user of haptics client.
     * @param aActuatorCallback Pointer to a actuator status callback instance. 
     * If NULL, status values are not notified to the user of haptics client.
     * @param aClient Open session to haptics server for status value
     * observation.
     * @return A pointer to a newly created instance.
     */
    static CHWRMHapticsStatusObserver* NewL( 
                            MHWRMHapticsObserver* aHapticsCallback,
                            MHWRMHapticsActuatorObserver* aActuatorCallback,
                            RHWRMHapticsSession* aClient );

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsStatusObserver();

    /**
     * Retrieves the current haptics status. 
     *
     * @return THWRMHapticsStatus indicating the current haptics status.
     */
    MHWRMHapticsObserver::THWRMHapticsStatus CurrentStatus() const;

protected:

    /**
     * @see CActive
     */
    virtual void RunL();

    /**
     * @see CActive
     */
    virtual TInt RunError(TInt aError);

    /**
     * @see CActive
     */
    virtual void DoCancel();

private:

    /**
     * Constructor.
     *
     * @param aHapticsCallback Pointer to haptics status callback instance.
     * @param aActuatorCallback Pointer to actuator status callback instance.
     * @param aClient Open session to haptics server for status value
     * observation.
     */
    CHWRMHapticsStatusObserver( MHWRMHapticsObserver* aHapticsCallback,
                                MHWRMHapticsActuatorObserver* aActuatorCallback,
                                RHWRMHapticsSession* aClient );
                                               
    /**
     * By default Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Activates this object and starts observation.
     */
    void OrderNotification();

    /**
     * Based on available callback instances, returns the type
     * of status notification, which should be used in notification
     * requests.
     *
     * @return Status type to be used in notification requests.
     */
    THWRMHapticsStatusTypes NotificationStatusType() const;
    
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
     * Haptics status callback instance. Not owned.
     */
    MHWRMHapticsObserver* iHapticsCallback;
    
    /** 
     * Actuator status callback instance. Not owned.
     */
    MHWRMHapticsActuatorObserver* iActuatorCallback;
    
    /** 
     * Connection to haptics server. Not owned.
     */
    RHWRMHapticsSession* iClient;
    
    /**
     * Last haptics status received.
     */
    THapticsStatus iHapticsStatus;

    /**
     * Last status value received from server. This value can be of 
     * either THWRMHapticsStatus or THWRMActuatorEvents.
     */
    TInt iLastStatus;
    
    /**
     * Indication of which type of status was received the last time.
     * Also used when sending notification request to indicate what type
     * of statuses should be notified.
     */
    THWRMHapticsStatusTypes iStatusType;
    
    /**
     * Actuator type to which received actuator event is related to.
     */
    THWRMLogicalActuators iActuator;

    /**
     * Status package for async calls.
     */
    TPckg<TInt> iStatusPckg;

    /**
     * Status type package for async calls.
     */
    TPckg<THWRMHapticsStatusTypes> iStatusTypePckg;

    /**
     * Actuator type package for async calls.
     */
    TPckg<THWRMLogicalActuators> iActuatorPckg;
    };

#endif // HWRMHAPTICSSTATUSOBSERVER_H

// End of File
