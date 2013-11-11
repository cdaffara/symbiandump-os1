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
* Description:  Ussd Interface, 
*
*/



// INCLUDE FILES
#include    "CPhCltUssd.h"
#include "CPhCltUssdImp.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltUssd::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltUssd* CPhCltUssd::NewL( TBool aShowNotes )
    {
    return CPhCltUssdImp::NewL( aShowNotes );
    }


// -----------------------------------------------------------------------------
// CPhCltUssd::CPhCltUssd
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltUssd::CPhCltUssd( )    
    {
    }
    



//  End of File  
