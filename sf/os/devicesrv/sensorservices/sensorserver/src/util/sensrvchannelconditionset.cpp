/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel condition set object
*
*/


#include "sensrvchannelconditionset.h"
#include "sensrvchannelconditionsetimpl.h"

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelConditionSet::CSensrvChannelConditionSet()
    {
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CSensrvChannelConditionSet* CSensrvChannelConditionSet::NewL
        ( TSensrvConditionSetType aConditionSetType )
    {
    CSensrvChannelConditionSet* self = CSensrvChannelConditionSet::NewLC( aConditionSetType );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CSensrvChannelConditionSet* CSensrvChannelConditionSet::NewLC
        ( TSensrvConditionSetType aConditionSetType )
    {
    CSensrvChannelConditionSetImpl* self = 
        new( ELeave ) CSensrvChannelConditionSetImpl(aConditionSetType);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }



