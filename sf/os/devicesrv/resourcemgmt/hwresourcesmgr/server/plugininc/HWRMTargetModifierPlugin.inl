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




#include <ecom/ecom.h>

// -----------------------------------------------------------------------------
// CHWRMTargetModifierPlugin::~CHWRMTargetModifierPlugin
// Destructor informs ECom that implementation has been destroyed.
// -----------------------------------------------------------------------------
//
inline CHWRMTargetModifierPlugin::~CHWRMTargetModifierPlugin()
    {
    REComSession::DestroyedImplementation(iDestructorIDKey);
    }

// -----------------------------------------------------------------------------
// CHWRMTargetModifierPlugin::NewL
// Factory method to create implementation. Implementation is found via ECom.
// -----------------------------------------------------------------------------
//
inline CHWRMTargetModifierPlugin* CHWRMTargetModifierPlugin::NewL(const TDesC8& aMatch)
    {
    // Set resolve parameters to find correct implementation
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatch);
    resolverParams.SetWildcardMatch(EFalse);

    TAny* interface = REComSession::CreateImplementationL(KHWRMTargetModifierPluginInterfaceUid, 
                                        _FOFF(CHWRMTargetModifierPlugin, iDestructorIDKey),
                                        resolverParams,
                                        KRomOnlyResolverUid);

    CHWRMTargetModifierPlugin* service = reinterpret_cast<CHWRMTargetModifierPlugin*>(interface);
    
    return service;
    }
            
// End of File
