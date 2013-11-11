/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base class for PSM plug-ins
*
*/


#include <ecom/ecom.h>
#include <psmsettingsprovider.h>

// -----------------------------------------------------------------------------
// Contructor
// -----------------------------------------------------------------------------
//
inline CPsmPluginBase::TPsmPluginCTorParams::TPsmPluginCTorParams( 
                                    MPsmSettingsProvider& aSettingsProvider ) :
    iSettingsProvider( aSettingsProvider )
    {
    }
    
// -----------------------------------------------------------------------------
// CPsmPluginBase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
inline CPsmPluginBase* CPsmPluginBase::NewL( const TUid& aImplementationUid,
                                             MPsmSettingsProvider& aSettingsProvider )
    {
    CPsmPluginBase* service = CPsmPluginBase::NewLC( aImplementationUid,
                                                     aSettingsProvider );
    CleanupStack::Pop( service );
    return service;
    }

// -----------------------------------------------------------------------------
// CPsmPluginBase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
inline CPsmPluginBase* CPsmPluginBase::NewLC( const TUid& aImplementationUid,
                                              MPsmSettingsProvider& aSettingsProvider )
    {
    // Create instance using ECom
    TPsmPluginCTorParams initParams( aSettingsProvider );
    
    TAny* interface = REComSession::CreateImplementationL( 
                                aImplementationUid, 
                                _FOFF( CPsmPluginBase, iDestructorIdKey ),
                                &initParams );

    CPsmPluginBase* service = reinterpret_cast<CPsmPluginBase*>( interface );
    CleanupStack::PushL( service );

    return service;
    }

// -----------------------------------------------------------------------------
// Base class constructor
// -----------------------------------------------------------------------------
//
inline CPsmPluginBase::CPsmPluginBase( TPsmPluginCTorParams& aInitParams ) :
    iSettingsProvider( aInitParams.iSettingsProvider )
    {
    }

// -----------------------------------------------------------------------------
// Destructor informs ECom that implementation has been destroyed.
// -----------------------------------------------------------------------------
//
inline CPsmPluginBase::~CPsmPluginBase()
    {
    REComSession::DestroyedImplementation( iDestructorIdKey );
    }

// End of file
