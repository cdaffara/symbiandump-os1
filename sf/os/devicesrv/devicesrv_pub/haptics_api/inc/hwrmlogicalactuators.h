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
* Description:  Enumerations of logical actuators and actuator events.
*
*/


#ifndef T_HWRMLOGICALACTUATORS_H
#define T_HWRMLOGICALACTUATORS_H

#include <e32std.h>

 /**
 * @enum THWRMLogicalActuators
 * 
 * Logical actuators types, that can be used for Haptics client objects.
 * Logical actuator is mapped to physical actuator according to
 * system configuration.
 */
enum THWRMLogicalActuators
    {
    /**    
    * Client wants any kind of haptic feedback.
    */    
    EHWRMLogicalActuatorAny = 1, 
    
    /**
    * Client wants strong haptic feedback, which shakes the whole device.
    */ 
    EHWRMLogicalActuatorDevice = 2, 
    
    /**
    * Clients wants haptic feedback to primary display.
    */
    EHWRMLogicalActuatorPrimaryDisplay = 4, 
    
    /**
    * Clients wants haptic feedback to secondary display.
    */
    EHWRMLogicalActuatorSecondaryDisplay = 8, 
    
    /**
    * Clients wants haptic feedback for gaming.
    */
    EHWRMLogicalActuatorGame = 16, 
    
    /**
    * Clients wants haptic feedback for gaming on the left side of device.
    */
    EHWRMLogicalActuatorGameLeft = 32, 
    
    /**
    * Clients wants haptic feedback for gaming on the right side of device.
    */
    EHWRMLogicalActuatorGameRight = 64, 
    
    /** 
    * Clients wants haptic feedback for external actuator, e.g. USB actuator.
    */
    EHWRMLogicalActuatorExternalVibra = 128,  
    
    /** 
    * For sanity checks only e.g. when enumerating logical actuators.
    */
    EHWRMLogicalActuatorLast = 256
    };

#endif // T_HWRMLOGICALACTUATORS_H
