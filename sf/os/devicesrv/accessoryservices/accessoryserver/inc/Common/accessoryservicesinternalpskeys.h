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
* Description:  Internal Publish&Subscribe definitions of the
*                Accessory Services
*
*/


#ifndef ACCESSORYSERVICESINTERNALPSKEYS_H
#define ACCESSORYSERVICESINTERNALPSKEYS_H

#include <e32cmn.h>

static const TUid KPSUidAccessoryServices = { 0x10205030 };

// =============================================================================
// Accessory Services Wired Connection API
// =============================================================================
// Use TUid KPSUidAccessoryServices = { 0x10205030 } 

/**
* Notified when a wired accessory is physically connected or disconnected.  
*/
const TUint32 KAccSrvcsWiredConnection = 0x00000001;
enum TAccSrvcsWiredConnection
    {
    EAccSrvcsWiredConnUninitialized = 0,
    EAccSrvcsWiredConnConnected,
    EAccSrvcsWiredConnDisconnected
    };


#endif ACCESSORYSERVICESINTERNALPSKEYS_H
