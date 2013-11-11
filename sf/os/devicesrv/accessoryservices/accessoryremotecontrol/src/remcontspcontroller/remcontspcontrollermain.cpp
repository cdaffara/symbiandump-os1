/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ECom defined way to provide information about 
*				 RemConTspController plug-in for Ecom framework.
*
*/


// INCLUDE FILES
#include "RemConTspController.h"
#include <ecom/ImplementationProxy.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// Define the private interface UIDs
const TImplementationProxy ImplementationTable[] =
	{										
	IMPLEMENTATION_PROXY_ENTRY( 0x10205045, CRemConTspController::NewL )
	};

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ImplementationGroupProxy Returns the number of interface implementations 
// for ECom framework.
//
// Returns: TImplementationProxy*: Pointer contains UID and function pointer.
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

//
// End of file
