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
* Description:  ECom interface for effect data getter plugin.
*
*/

#include <hwrmhapticseffectdatagetter.h>
#include "hwrmhapticstrace.h"

const TUid KHapticsEffectDataGetterIFUid = {0x20021202};
const TInt KHapticsAllowedAmountOfPlugins = 1;

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsEffectDataGetter* CHWRMHapticsEffectDataGetter::NewL()
    {
    RImplInfoPtrArray implInfoArray;
    CleanupClosePushL( implInfoArray );

    // list implementations
    REComSession::ListImplementationsL( KHapticsEffectDataGetterIFUid, 
                                        implInfoArray );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsEffectDataGetter::NewL() - Found %d plugins" ), implInfoArray.Count() ) );

    // create plugin, if found
    CHWRMHapticsEffectDataGetter* effectDataGetter = NULL;
    if ( implInfoArray.Count() == KHapticsAllowedAmountOfPlugins )
        {
        TUid pluginUid = implInfoArray[0]->ImplementationUid();
        
        // delete implementation info list contents
        implInfoArray.ResetAndDestroy();
        
        effectDataGetter = static_cast<CHWRMHapticsEffectDataGetter*>(
            REComSession::CreateImplementationL( pluginUid, 
                _FOFF( CHWRMHapticsEffectDataGetter, iDtor_ID_Key ) ) );
        }
    else
        {
        // there are none or unsupported amount of plugins found
        User::Leave( KErrNotSupported );
        }

    CleanupStack::PopAndDestroy( &implInfoArray );
    
    return effectDataGetter;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
// 
EXPORT_C CHWRMHapticsEffectDataGetter::~CHWRMHapticsEffectDataGetter()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// End of file.
