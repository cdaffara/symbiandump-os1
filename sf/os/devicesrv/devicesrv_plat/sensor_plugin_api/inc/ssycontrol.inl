/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SSY Control ECom interface inline methods
*
*/


#include <ecom/ecom.h>

// -----------------------------------------------------------------------------
// Destructor informs ECom that implementation has been destroyed.
// -----------------------------------------------------------------------------
//
inline CSsyControl::~CSsyControl()
    {
    REComSession::DestroyedImplementation( iDestructorIdKey );
    }

// -----------------------------------------------------------------------------
// Factory method to create implementation. Implementation is found via ECom.
// -----------------------------------------------------------------------------
//
inline CSsyControl* CSsyControl::NewL( MSsyCallback& aSsyCallback,
                                       TUid aImplementationUid )
    {
    // Create instance using ECom
    TAny* interface = REComSession::CreateImplementationL( 
                                aImplementationUid, 
                                _FOFF( CSsyControl, iDestructorIdKey ),
                                &aSsyCallback );

    CSsyControl* service = reinterpret_cast<CSsyControl*>( interface );
    
    return service;
    }
            
// End of File


