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




// INCLUDES
#include <ecom/ecom.h>

// CONSTANTS
// Plugin name defined by default_data in resource file
_LIT8( KPluginName, "HWRMPowerState" );


// -----------------------------------------------------------------------------
// CHWRMPowerStatePlugin::~CHWRMPowerStatePlugin
// Destructor informs ECom that implementation has been destroyed.
// -----------------------------------------------------------------------------
//
inline CHWRMPowerStatePlugin::~CHWRMPowerStatePlugin()
    {
    REComSession::DestroyedImplementation(iDestructorIDKey);
    }

// -----------------------------------------------------------------------------
// CHWRMPowerStatePlugin::NewL
// Factory method to create implementation. Implementation is found via ECom.
// -----------------------------------------------------------------------------
//
inline CHWRMPowerStatePlugin* CHWRMPowerStatePlugin::NewL( 
                        MHWRMPowerStatePluginCallback* aNotificationCallback )
    {
    // Set resolve parameters to find correct implementation
    TEComResolverParams resolverParams;
    resolverParams.SetDataType( KPluginName );
    resolverParams.SetGenericMatch( EFalse );

    // Create instance using ECom
    TAny* interface = REComSession::CreateImplementationL( 
                                KCHWRMPowerStatePluginInterfaceUid, 
                                _FOFF(CHWRMPowerStatePlugin, iDestructorIDKey),
                                resolverParams,
                                KRomOnlyResolverUid );

    CHWRMPowerStatePlugin* service = reinterpret_cast<CHWRMPowerStatePlugin*>(interface);
    
    // Set notification callback pointer
    service->iNotificationCallback = aNotificationCallback;
    
    return service;
    }
            
// End of File
