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
* Description:  Haptics packetizer plugin factory class implementation.
*
*/

#include <ecom/ecom.h>
#include <centralrepository.h>
#include <hwrmhapticspacketizer.h>

#include "hwrmhapticsinternalcrkeys.h"

// ---------------------------------------------------------------------------
// Factory method for creating a packetizer plugin.
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsPacketizer* CHWRMHapticsPacketizer::NewL( 
    THWRMLogicalActuators aLogicalActuator )
    {
    // map actuator type to cenrep id
    TInt cenrepId( 0 );
    switch( aLogicalActuator )
        {
        case EHWRMLogicalActuatorAny:
            {
            cenrepId = KHWRMHapticsPluginAnyPacketizer;
            break;
            }
        case EHWRMLogicalActuatorDevice:
            {
            cenrepId = KHWRMHapticsPluginDevicePacketizer;
            break;
            }
        case EHWRMLogicalActuatorPrimaryDisplay:
            {
            cenrepId = KHWRMHapticsPluginPrimaryDisplayPacketizer;
            break;
            }
        case EHWRMLogicalActuatorSecondaryDisplay:
            {
            cenrepId = KHWRMHapticsPluginSecondaryDisplayPacketizer;
            break;
            }
        case EHWRMLogicalActuatorGame:
            {
            cenrepId = KHWRMHapticsPluginGamePacketizer;
            break;
            }
        case EHWRMLogicalActuatorGameLeft:
            {
            cenrepId = KHWRMHapticsPluginGameLeftPacketizer;
            break;
            }
        case EHWRMLogicalActuatorGameRight:
            {
            cenrepId = KHWRMHapticsPluginGameRightPacketizer;
            break;
            }
        case EHWRMLogicalActuatorExternalVibra:
            {
            cenrepId = KHWRMHapticsPluginExternalVibraPacketizer;
            break;
            }
        default:
            cenrepId = KHWRMHapticsPluginAnyPacketizer;
            break;
       }

    // create cenrep instance
    CRepository* repository = CRepository::NewLC( KCRUidHWRMHapticsPlugins );

    // get the packetizer plugin UID
    TInt pluginUid( 0 );
    User::LeaveIfError( repository->Get( cenrepId, pluginUid ) );

    // delete cenrep instance
    CleanupStack::PopAndDestroy( repository );

    TInt err = KErrNone;

    // create plugin, if found uid
    TAny* interface = NULL;
    if ( pluginUid != 0 )
        {
        TRAP( err, interface = REComSession::CreateImplementationL(
            TUid::Uid( pluginUid ), 
            _FOFF( CHWRMHapticsPacketizer, iDtor_ID_Key ) ) );
        
        if ( err != KErrNone || !interface )
            {
            err = KErrNotSupported;
            }
        }
    else
        {
        err = KErrNotSupported;
        }
    
    User::LeaveIfError( err );

    return static_cast<CHWRMHapticsPacketizer*>( interface );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsPacketizer::~CHWRMHapticsPacketizer()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// End of file

