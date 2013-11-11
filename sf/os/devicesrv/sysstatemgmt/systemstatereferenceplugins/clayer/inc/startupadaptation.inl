/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface   : Domain, Startup Adaptation API
* Implementation for inline functions of CStartupAdaptation
* class.
*
*/



#include <ecom/ecom.h>

/*
* @file
* @publishedPartner
* @released
*/

/**
* Interface UID for StartupAdaptation interface.
*/
static const TUid KStartupAdaptationIfUid = {0x101F8768};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStartupAdaptation::NewL
//
// -----------------------------------------------------------------------------
//
inline CStartupAdaptation* CStartupAdaptation::NewL(
    MStartupAdaptationObserver& aObserver )
    {
    // Set resolve parameters to use in listing available implementations.
    TEComResolverParams resolverParams;
    resolverParams.SetGenericMatch( ETrue );

    // List available implementations.
    RImplInfoPtrArray implInfoArray;
    REComSession::ListImplementationsL(
        KStartupAdaptationIfUid,
        resolverParams,
        KRomOnlyResolverUid,
        implInfoArray );

    // There must be one and only one Startup Adaptation plug-in.
    if ( implInfoArray.Count() < 1 )
        {
        User::Leave( KErrNotFound );
        }
    if ( implInfoArray.Count() > 1 )
        {
        User::Leave( KErrGeneral );
        }

    // Create instance using ECom
    TUid dtorIDKey;
    TAny* ptr = REComSession::CreateImplementationL(
        implInfoArray[ 0 ]->ImplementationUid(),
        dtorIDKey,
        &aObserver );

    implInfoArray.ResetAndDestroy();
    implInfoArray.Close();

    CStartupAdaptation* plugin = reinterpret_cast<CStartupAdaptation*>( ptr );
    plugin->iDtorIDKey = dtorIDKey;

    return plugin;
    }


// Destructor
inline CStartupAdaptation::~CStartupAdaptation()
    {
    REComSession::DestroyedImplementation( iDtorIDKey );
    }
