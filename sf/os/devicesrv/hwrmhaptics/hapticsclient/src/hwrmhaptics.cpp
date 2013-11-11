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
* Description:  Implementation of haptics client.
*
*/


#include <e32svr.h>
#include <hwrmhaptics.h>
#include "hwrmhapticsimpl.h"

// ---------------------------------------------------------------------------
// Two-phase constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHaptics* CHWRMHaptics::NewL(
    MHWRMHapticsObserver* aHapticsCallback,
    MHWRMHapticsActuatorObserver* aActuatorCallback )
    {        
    CHWRMHapticsImpl* impl = new ( ELeave ) CHWRMHapticsImpl();
    
    CleanupStack::PushL( impl );
    
    impl->ConstructL( aHapticsCallback, aActuatorCallback );
    
    CleanupStack::Pop( impl );

    return impl;      
    }
    
// ---------------------------------------------------------------------------
// Two-phase asynchronous constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHaptics* CHWRMHaptics::NewL( 
    MHWRMHapticsObserver* aHapticsCallback,
    MHWRMHapticsActuatorObserver* aActuatorCallback,
    TRequestStatus& aStatus )
    {
    CHWRMHapticsImpl* impl = new ( ELeave ) CHWRMHapticsImpl();
    
    CleanupStack::PushL( impl );
    
    impl->ConstructL( aHapticsCallback, aActuatorCallback, aStatus );
    
    CleanupStack::Pop( impl );

    return impl;
    }

//  End of File  
