/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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


// INCLUDE FILES
#include    "CPhCltEmergencyCall.h"
#include    "CPhCltEmergencyCallImpl.h"
#include    "PhCltTypes.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltEmergencyCall::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltEmergencyCall* CPhCltEmergencyCall::NewL( 
    MPhCltEmergencyCallObserver* aObserver )
    {
    return CPhCltEmergencyCallImpl::NewL( aObserver );
    }


// -----------------------------------------------------------------------------
// CPhCltEmergencyCall::CPhCltEmergencyCall
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltEmergencyCall::CPhCltEmergencyCall( MPhCltEmergencyCallObserver* aObserver )
    : CActive( EPriorityStandard ), iObserver( aObserver )
    {
    }
    



//  End of File  
