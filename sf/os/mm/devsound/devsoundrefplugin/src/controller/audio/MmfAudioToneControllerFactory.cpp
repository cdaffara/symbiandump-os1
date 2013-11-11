/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#include "MmfAudioToneController.hrh"
#include "MmfAudioToneController.h"


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
// move to another module to allow custom audio controller for testing

/**
Table of implementations to be returned to th ECom system
@internalAll 
*/
const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidControllerAudioTone,	CMMFAudioToneController::NewL)
	};

/**
Used by the ECom system to obtain a list of implementations available in the library

@internalAll
@param aTableCount will be initialized to the number of entries
@return TImplementationProxy* a pointer to a list of available implementations
*/

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

