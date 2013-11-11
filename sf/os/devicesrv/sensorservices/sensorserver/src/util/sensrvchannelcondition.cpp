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
* Description:  Channel condition object.
*
*/


#include "sensrvchannelcondition.h"
#include "sensrvchannelconditionimpl.h"

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelCondition::CSensrvChannelCondition()
    {
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CSensrvChannelCondition* CSensrvChannelCondition::NewL
        ( TInt aConditionType, 
          TInt aConditionOperator,
          TInt aItemIndex,
          TDesC8& aValue )
    {
    CSensrvChannelCondition* self = 
        CSensrvChannelCondition::NewLC( aConditionType,
                                        aConditionOperator,
                                        aItemIndex,
                                        aValue );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CSensrvChannelCondition* CSensrvChannelCondition::NewLC
        ( TInt aConditionType, 
          TInt aConditionOperator,
          TInt aItemIndex,
          TDesC8& aValue )
    {
    CSensrvChannelConditionImpl* self = 
        new( ELeave ) CSensrvChannelConditionImpl(aConditionType,
                                                  aConditionOperator,
                                                  aItemIndex);
    CleanupStack::PushL( self );
    self->ConstructL(aValue);
    return self;
    }


