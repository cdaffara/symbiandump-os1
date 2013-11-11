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

#ifndef HWRMCLIENTSERVER_H
#define HWRMCLIENTSERVER_H

//  INCLUDES
#include <e32base.h>


// CONSTANTS
// None

// MACROS
// None

// DATA TYPES

// Opcodes used in message passing between client and server
enum THWRMServRequest
    {
	/**
	* Create vibra service
	*
	* parameters:
	* None
	*/
	EHWRMCreateVibraService = 0,
	
	/**
	* Create light service
	*
	* parameters:
	* None
	*/
    EHWRMCreateLightService,
    
    /**
 	* Create power service
 	*
 	* parameters:
 	* None
 	*/
    EHWRMCreatePowerService,

	/**
	* Create FM Tx service
	*
	* parameters:
	* None
	*/
    EHWRMCreateFmTxService,
    
    EHWRMEndOfCreateServiceOpCodes
	
    // The service specific commands are in separate headers.
    // Light commands start at 1000
    // Vibra commands start at 2000
    // Power commands start at 3000
    // FM Tx commands start at 4000
    };

/**
* The following enumaration defines the HWRM server panic codes.
*/
enum THWRMPanic
{
    // Illegal function called via client-server interface
    EPanicIllegalFunction=0,
    
    // Bad handle in client-server interface
	EPanicBadHandle,
	
	// Bad descriptor in client-srever interface
	EPanicBadDescriptor,
	
	// Unable to restart profile observer
	EPanicProfileObserverPanic
};


// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// None

#endif  // HWRMCLIENTSERVER_H
            
// End of File
