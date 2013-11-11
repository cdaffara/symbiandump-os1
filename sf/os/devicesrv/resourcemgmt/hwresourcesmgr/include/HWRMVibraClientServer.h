/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HWRMVIBRACLIENTSERVER_H
#define HWRMVIBRACLIENTSERVER_H

// INCLUDES
// None

// CONSTANTS
// Maximum number of possible vibra targets
const TInt KHWRMVibraMaxTargets(1);   

// MACROS
// None

// DATA TYPES
// None

// Opcodes used in message passing between client and server
// Vibra opcodes start at 2000
enum THWRMVibraServRequest
    { 
	
	// Services for Vibra
	
	/**
	* Starts vibra with default intensity
	* 
	* parameters:
	* 0: TInt - Duration in milliseconds.
	*/
	EHWRMStartVibraDefaultIntensity = 2000,
	
	/**
	* Starts vibra with specified intensity 
	* 
	* parameters:
	* 0: TInt - Duration in milliseconds.
	* 1: TInt - Intensity (valid values: -100 to 100)
	*/
	EHWRMStartVibra,
	
	/**
	* Stops vibra immediately.
	* 
	* parameters:
	* None
	*/
	EHWRMStopVibra,

    /**
    * Cleans up the vibra service so that session can be closed.
    *
	* parameters:
	* None
    */
  	EHWRMCleanupVibra,
  	
  	/**
  	* Reserves vibra.
  	*
  	* parameters:
  	* 0: TBool - Restore state flag
  	* 1: TBool - ForceNoCCoeEnv flag
  	*/
  	EHWRMReserveVibra,
  	
  	/**
  	* Releases vibra.
    *
	* parameters:
	* None
  	*/
    EHWRMReleaseVibra,
    /**
	* Generates vibra pulse with default intensity and duration
	* 
	* parameters:
	* None
	*/
	EHWRMPulseVibraDefault,
	
	/**
	* Generates vibra pulse with specified intensity and default duration
	* 
	* parameters:
	* 0: TInt - Duration in milliseconds.
	*/
	EHWRMPulseVibraDefaultIntensity,
	
	/**
	* Generates vibra pulse with specified intensity and duration
	* 
	* parameters:
	* 0: TInt - Duration in milliseconds.
	* 1: TInt - Intensity (valid values: -100 to 100)
	*/
    EHWRMPulseVibra,

	EHWRMEndOfVibraOpCodes
    };


// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// None

#endif  // HWRMVIBRACLIENTSERVER_H
            
// End of File
