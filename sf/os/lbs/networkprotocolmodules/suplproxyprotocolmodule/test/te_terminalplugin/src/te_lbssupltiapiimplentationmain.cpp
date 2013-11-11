/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "te_lbssupltiapiimplementation.h"
   
// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    
   	#ifdef __EABI__ 
		IMPLEMENTATION_PROXY_ENTRY(0x102871F2, CLbsSuplTiApiImplementation::NewL) 
	#else
        { { 0x102871F2 },  CLbsSuplTiApiImplementation::NewL}
	#endif        
        
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
