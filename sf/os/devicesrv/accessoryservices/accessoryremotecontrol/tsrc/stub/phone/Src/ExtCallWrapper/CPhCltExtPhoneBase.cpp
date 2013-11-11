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
* Description:  Ext Phone Base, dialing base class.
*
*/



// INCLUDE FILES
#include    "CPhCltExtPhoneBase.h"
#include    "CPhCltExtPhone.h"
#include    "PhCltTypes.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltExtPhoneBase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltExtPhoneBase* CPhCltExtPhoneBase::NewL( 
    MPhCltExtPhoneObserver* aObserver )
    {
    return CPhCltExtPhone::NewL( aObserver );
    }


// -----------------------------------------------------------------------------
// CPhCltExtPhoneBase::CPhCltExtPhoneBase
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltExtPhoneBase::CPhCltExtPhoneBase( MPhCltExtPhoneObserver* aObserver )
    : CActive( EPriorityStandard ), iObserver( aObserver )
    {
    }
    



//  End of File  
