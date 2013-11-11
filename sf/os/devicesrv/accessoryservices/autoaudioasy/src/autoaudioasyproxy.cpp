/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementations for autoaudioasy
*
*/

#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "cautoaudioasymainservicebase.h"

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    {{0x2002F9A4}, (TProxyNewLPtr) CAutoAudioAsyMainServiceBase::NewL}
    };

//
// ---------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy( TInt&)
// ---------------------------------------------------------
//  
// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
