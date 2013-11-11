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
* Description:  This file contains the header of the 
*                MHWRMHapticsActuatorObserver class.
*
*/


#ifndef M_HWRMHAPTICSACTUATOROBSERVER_H
#define M_HWRMHAPTICSACTUATOROBSERVER_H

#include <hwrmlogicalactuators.h>

/**
* MHWRMHapticsActuatorObserver observer class.
*
* A client should derive from this abstract class to receive
* logical actuator event notifications.
*
* @since S60 5.1
*/
class MHWRMHapticsActuatorObserver
    {
public:
 
    /**
    * @enum THWRMActuatorEvents
    * 
    * Events that can happen to logical actuators.
    */    
    enum THWRMActuatorEvents
        {
        /**
        * This event occurs, when an actuator plugin of a previously
        * unsupported logical actuator type is installed to the device.
        *
        * In practice, this event may occur when the user of the device
        * starts using an external vibra device, which requires the 
        * installation of an actuator plugin currently not present in
        * the device.
        */
        EHWRMActuatorAttached,
        
        /**
        * This event occurs, when an actuator plugin of a previously
        * supported logical actuator type is uninstalled from the device.
        *
        * In practice, this event may occur when the user of the device
        * chooses to stop using an external vibra device, and uninstalls
        * a previously installed actuator plugin.
        */
        EHWRMActuatorDetached,
        
        /**
        * Actuator is currently enabled and it is ready to be used
        * as normal.
        *
        * Enabled/disabled state of an actuator is notified, when the
        * actuator is opened by a client and when the state changes
        * on runtime.
        */
        EHWRMActuatorEnabled,
        
        /**
        * Actuator is currently disabled and it is not able to handle
        * haptics commands. This state of a logical actuator is in most
        * cases relevant only for actuator plugins handling external
        * vibra devices; E.g. connection to the external device may be
        * lost, in which case the plugin is not able to execute incoming
        * commands.
        */
        EHWRMActuatorDisabled, 
        
        /**
        * For sanity checks only e.g. when enumerating actuator events.
        */ 
        EHWRMEventLast    
        };
        
    /**
    * Called when the state of an actuator changes. 
    * 
    * @param aEvent Indicates the type of event.
    * @param aActuator Logical actuator to which the event happened.
    *
    * @since S60 5.1
    */
    virtual void ActuatorEventL( THWRMActuatorEvents aEvent, 
                                 THWRMLogicalActuators aActuator ) = 0;
    };

#endif // M_HWRMHAPTICSACTUATOROBSERVER_H
