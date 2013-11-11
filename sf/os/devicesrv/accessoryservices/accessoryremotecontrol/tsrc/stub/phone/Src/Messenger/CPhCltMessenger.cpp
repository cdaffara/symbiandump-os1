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
* Description:  Messenger Base class.
*
*/



// INCLUDE FILES
#include    "CPhCltMessenger.h"
#include    "CPhCltMessengerImplementation.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltMessenger::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltMessenger* CPhCltMessenger::NewL( const TUid& aCategoryUid,
            const TInt& aDefaultMessageSize )
    {
    return CPhCltMessengerImplementation::NewL( aCategoryUid, aDefaultMessageSize);
    }

// -----------------------------------------------------------------------------
// CPhCltMessenger::CPhCltMessenger
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltMessenger::CPhCltMessenger( )
    {
    }
    
//  End of File  
