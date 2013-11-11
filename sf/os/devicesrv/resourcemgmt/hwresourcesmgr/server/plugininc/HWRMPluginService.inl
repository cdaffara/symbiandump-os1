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
// CHWRMPluginService::~CHWRMPluginService
// Destructor informs ECom that implementation has been destroyed.
// -----------------------------------------------------------------------------
//
inline CHWRMPluginService::~CHWRMPluginService()
    {
    REComSession::DestroyedImplementation(iDestructorIDKey);
    }

// -----------------------------------------------------------------------------
// CHWRMPluginService::NewL
// Factory method to create implementation. Implementation is found via ECom.
// -----------------------------------------------------------------------------
//
inline CHWRMPluginService* CHWRMPluginService::NewL(const TDesC8& aMatch, MHWRMPluginCallback* aResponseCallback)
    {
    // Set resolve parameters to find correct implementation
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatch);
    resolverParams.SetWildcardMatch(EFalse);

    // Create instance using ECom
    TAny* interface = REComSession::CreateImplementationL(KCHWRMPluginServiceInterfaceUid, 
                                        _FOFF(CHWRMPluginService, iDestructorIDKey),
                                        resolverParams,
                                        KRomOnlyResolverUid);
    
    CHWRMPluginService* service = reinterpret_cast<CHWRMPluginService*>(interface);
    
    // Set response callback pointer
    service->iResponseCallback = aResponseCallback;
    
    return service;
    }
            
// End of File
