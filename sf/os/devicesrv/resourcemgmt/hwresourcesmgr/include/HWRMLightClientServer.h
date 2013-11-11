/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMLIGHTCLIENTSERVER_H
#define HWRMLIGHTCLIENTSERVER_H

// INCLUDES
#include <hwrm/hwrmextendedlight.h> // for the target enum

// CONSTANTS

// Maximum number of possible lights targets
// Note: If this is increased beyond 16, light status
//       passing to clients needs to be modified.
const TInt KHWRMLightMaxTargets(31); 

// First target value in targets enumeration
const TInt KHWRMLightFirstTarget(CHWRMExtendedLight::EPrimaryDisplay);  

// Value for all defined separate targets combined.
// Not included in Light API target enumeration because of possible
// future compatibility issues if more targets are added.
const TInt KHWRMAllTargets(CHWRMExtendedLight::EPrimaryDisplay |
                           CHWRMExtendedLight::EPrimaryKeyboard |
                           CHWRMExtendedLight::ESecondaryDisplay |
                           CHWRMExtendedLight::ESecondaryKeyboard |
                           CHWRMExtendedLight::ECustomTarget1 |
                           CHWRMExtendedLight::ECustomTarget2 |
                           CHWRMExtendedLight::ECustomTarget3 |
                           CHWRMExtendedLight::ECustomTarget4 |
                           CHWRMExtendedLight::ECustomTarget5 |
                           CHWRMExtendedLight::ECustomTarget6 |
                           CHWRMExtendedLight::ECustomTarget7 |
                           CHWRMExtendedLight::ECustomTarget8 |
                           CHWRMExtendedLight::ECustomTarget9 |
                           CHWRMExtendedLight::ECustomTarget10 |
                           CHWRMExtendedLight::ECustomTarget11 |
                           CHWRMExtendedLight::ECustomTarget12 |
                           CHWRMExtendedLight::ECustomTarget13 |
                           CHWRMExtendedLight::ECustomTarget14 |
                           CHWRMExtendedLight::ECustomTarget15 |
                           CHWRMExtendedLight::ECustomTarget16 |
                           CHWRMExtendedLight::ECustomTarget17 |
                           CHWRMExtendedLight::ECustomTarget18 |
                           CHWRMExtendedLight::ECustomTarget19 |
                           CHWRMExtendedLight::ECustomTarget20 |
                           CHWRMExtendedLight::ECustomTarget21 |
                           CHWRMExtendedLight::ECustomTarget22 |
                           CHWRMExtendedLight::ECustomTarget23 |
                           CHWRMExtendedLight::ECustomTarget24 |
                           CHWRMExtendedLight::ECustomTarget25 |
                           CHWRMExtendedLight::ECustomTarget26 |
                           CHWRMExtendedLight::ECustomTarget27 
                           );  
                           
// Value for all defined non-custom targets.
const TInt KHWRMNonCustomTargets(CHWRMExtendedLight::EPrimaryDisplay |
                                 CHWRMExtendedLight::EPrimaryKeyboard |
                                 CHWRMExtendedLight::ESecondaryDisplay |
                                 CHWRMExtendedLight::ESecondaryKeyboard );  

const TUint8 KHWRMRedShift             = 16;
const TUint8 KHWRMGreenShift           = 8;
const TUint8 KHWRMSetDefaultColorShift = 24;
                                         
const TUint KHWRMSetColorMask      = 0x1000000;
const TUint KHWRMInvalidColorMask  = 0xFFFFFFFF;


// MACROS
// None

// DATA TYPES

// Note: cannot use same packages as plugins as same header
// includes plugin API and therefore ecom.

// Note: Both plugin API HWRMLightCommand::TLightTarget and
// Light Client API HWRMLight::TLightTarget enumerations
// are expected to have exact same values defined for
// same targets. There is no foreseeable reason why
// they would ever differ, and are only separately defined
// to avoid need of #including client header in plugin definition.
// This means that targets passed in light client-server commands
// can be directly cast as targets of plugin calls.

/**
* Data structure used to pass data with ELightsOnCmdId.
*/ 
struct THWRMLightsOnData
    {
    // Light target(s). Target can be used as bitmask.
    // Command only affects selected targets.
    TInt iTarget;

    // Intensity of light as percentage (1-100).
    // If intensity is 0, default intensity is used.
    TInt iIntensity;
    
    // Duration of the call in milliseconds
    TInt iDuration;
    
    // Is fade-in used or not?
    TBool iFadeIn;

    // Light color 
    //| MSB |     |       | LSB  |
    //| |   | RED | GREEN | BLUE |
    //  |
    //  |
    //  |-> 00 = Set Default Color
    //      01 = Set Color specified in RGB.
    TUint iColor;

    };

/**
* Data structure used to pass data with ELightsBlinkCmdId
*/ 
struct THWRMLightsBlinkData
    {
    // Light target(s). Target can be used as bitmask.
    // Command only affects selected targets.
    TInt iTarget;

    // Intensity of light as percentage (1-100).
    // If intensity is 0, default intensity is used.
    TInt iIntensity;
    
    // Duration of the call in milliseconds
    TInt iDuration;

    // Light on cycle duration in milliseconds.
    // If value is 0, use default cycle duration.
    TInt iOnCycleDuration;

    // Light off cycle duration in milliseconds.
    // If value is 0, use default cycle duration.
    TInt iOffCycleDuration;

    // Light color 
    //| MSB |     |       | LSB  |
    //| |   | RED | GREEN | BLUE |
    //  |
    //  |
    //  |-> 00 = Set Default Color
    //      01 = Set Color specified in RGB.
    TUint iColor;
    };

/**
* Data structure used to pass data with ELightsOffCmdId.
*/
struct THWRMLightsOffData
    {
    // Light target(s). Target can be used as bitmask.
    // Command only affects selected targets.
    TInt iTarget;

    // Duration of the call in milliseconds
    TInt iDuration;

    // Is fade-out used or not.
    TInt iFadeOut;
    };

struct THWRMSetColorData
    {
    // Light target(s). Target can be used as bitmask.
    // Command only affects selected targets.
    TInt iTarget;

    //| MSB |     |       | LSB  |
    //| |   | RED | GREEN | BLUE |
    //  |
    //  |
    //  |-> 00 = Set Default Color
    //      01 = Set Color specified in RGB.
    TUint iColor;
    };
    



// Data packages for commands
typedef TPckgBuf<THWRMLightsOnData>       THWRMLightsOnCmdPackage;
typedef TPckgBuf<THWRMLightsBlinkData>    THWRMLightsBlinkCmdPackage;
typedef TPckgBuf<THWRMLightsOffData>      THWRMLightsOffCmdPackage;
typedef TPckgBuf<THWRMSetColorData>       THWRMSetColorCmdPackage;


// Opcodes used in message passing between client and server.
// Light opcodes start at 1000
enum THWRMLightServRequest
    {
	
	/**
	* Turn lights on
	* 
	* parameters:
	* 0: HWRMLightCommand::TLightsOnCmdPackage 
	*/
	EHWRMLightsOn = 1000,
	
	/**
	* Turn lights off
	* 
	* parameters:
	* 0: HWRMLightCommand::TLightsOffCmdPackage 
	*/
	EHWRMLightsOff,

	/**
	* Set lights to blink
	* 
	* parameters:
	* 0: HWRMLightCommand::TLightsBlinkCmdPackage 
	*/
	EHWRMLightsBlink,

    /**
    * Cleans up the light service so that session can be closed.
    *
	* parameters:
	* None
    */
  	EHWRMCleanupLights,
  	
  	/**
  	* Reserves lights.
  	*
  	* parameters:
  	* 0: TBool - Target mask
  	* 1: TBool - Restore state flag
  	* 2: TBool - ForceNoCCoeEnv flag
  	*/
  	EHWRMReserveLights,
  	
  	/**
  	* Releases lights.
    *
	* parameters:
	* 0: TInt - Target mask
  	*/
    EHWRMReleaseLights,
    
   	/**
	* Gets supported light targets
	*
	* parameters:
	* 0 - TInt as desc - Returns the supported light target mask.
	*/
    EHWRMGetSupportedTargets,

    /**
    * Sets the color for targets specified in the target mask.
    *
    */
    EHWRMSetLightColor,

    /*
    * Set the color for targets and switches ON the light for the specified duration.
    */
    EHWRMLightsOnWithColor,
    
    /*
    * Set the color for targets and blinks the light targets for the specified duration.
    */
    EHWRMLightsBlinkWithColor,

    EHWRMEndOfLightOpCodes
    };


// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// None

#endif  // HWRMLIGHTCLIENTSERVER_H
            
// End of File
