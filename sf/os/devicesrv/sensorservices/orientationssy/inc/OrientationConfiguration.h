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
* Description:  Channel class of Orientation SSY
*
*/


#ifndef ORIENTATIONCONFIGURATION_H
#define ORIENTATIONCONFIGURATION_H


// INCLUDES
#include <e32base.h>
#include <sensrvchanneldatatypes.h>
#include <sensrvorientationsensor.h>

// FORWARD DECLARATIONS
class TAngleAndTimerConfig;

// CONSTANTS

// CLASS DECLARATION

/**
*  TOrientationConfiguration class
*
*  Orientation configurations for different orientation states.
*/
class TOrientationConfiguration
    {

public:
    
    /**
    * Struct defining angle and timer for a state transition
    */
    class TAngleAndTimerConfig
        {
    public:
        TInt iAngle;
        TInt iTimerValueInMilSeconds;
        };
    
    /**
    * Default C++ constructor
    */
    TOrientationConfiguration();
    
    /**
    * Method for getting the timer for an orientation
    */
    TInt GetTimerToOrientation( 
        TSensrvOrientationData::TSensrvDeviceOrientation aOrientation );
    
    /**
    * The identifier for current state
    */
    TSensrvOrientationData::TSensrvDeviceOrientation iOrientationState;
    
    /**
    * The used angle and timer to state display up
    */
    TAngleAndTimerConfig iOrientationDisplayUp;
    
    /**
    * The used angle and timer to state display down
    */
    TAngleAndTimerConfig iOrientationDisplayDown;
    
    /**
    * The used angle and timer to state display left up
    */
    TAngleAndTimerConfig iOrientationDisplayLeftUp;
    
    /**
    * The used angle and timer to state display right up
    */
    TAngleAndTimerConfig iOrientationDisplayRightUp;
    
    /**
    * The used angle and timer to state display upwards
    */
    TAngleAndTimerConfig iOrientationDisplayUpwards;
    
    /**
    * The used angle and timer to state display downwards
    */
    TAngleAndTimerConfig iOrientationDisplayDownwards;
    
    };
        
#endif // ORIENTATIONCONFIGURATION_H

// End of File
