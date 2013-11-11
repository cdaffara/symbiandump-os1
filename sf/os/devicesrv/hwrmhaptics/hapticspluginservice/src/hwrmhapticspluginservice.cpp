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
* Description:  Plugin base API methods implementation.
*
*/


#include <hwrmhapticspluginservice.h>

// -----------------------------------------------------------------------------
// CHWRMHapticsPluginService::~CHWRMHapticsPluginService
// Destructor informs ECom that implementation has been destroyed.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsPluginService::~CHWRMHapticsPluginService()
    {
    REComSession::DestroyedImplementation( iDestructorIDKey );
    }

// -----------------------------------------------------------------------------
// CHWRMHapticsPluginService::NewL
// Factory method to create implementation. Implementation is found via ECom.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsPluginService* CHWRMHapticsPluginService::NewL( 
    TUid aImplementationUid, 
    MHWRMHapticsPluginCallback* aResponseCallback )
    {
    TAny* interface = REComSession::CreateImplementationL( 
        aImplementationUid, 
        _FOFF( CHWRMHapticsPluginService, iDestructorIDKey ), 
        aResponseCallback );
    
    CHWRMHapticsPluginService* service =
         reinterpret_cast<CHWRMHapticsPluginService*>( interface );
    
    return service;
    }

// End of File
