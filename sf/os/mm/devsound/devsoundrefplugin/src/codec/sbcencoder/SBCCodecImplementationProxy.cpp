/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <mmf/plugin/mmfsbccodecimplementationuids.hrh>

#include "SBCEncoder.h"

/**
Exported proxy for instantiation method resolution
Define the interface UIDs
*/
const TImplementationProxy implementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidCodecPCM16ToSBC, CSBCEncoder::NewL)
	};

/**
Used by the ECom system to obtain a list of implementations available in the library

@internalAll
@param aTableCount will be initialized to the number of entries
@return TImplementationProxy* a pointer to a list of available implementations
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(implementationTable) / sizeof(TImplementationProxy);
	return implementationTable;
	}


