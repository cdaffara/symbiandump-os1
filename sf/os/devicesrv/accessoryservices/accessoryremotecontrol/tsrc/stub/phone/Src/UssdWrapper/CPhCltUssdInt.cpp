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
#include    "CPhCltUssdInt.h"
#include "CPhCltUssdImp.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltUssdInt::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltUssdInt* CPhCltUssdInt::NewL( TBool aShowNotes )
    {
    return CPhCltUssdImp::NewL( aShowNotes );
    }


// -----------------------------------------------------------------------------
// CPhCltUssdInt::CPhCltUssdInt
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltUssdInt::CPhCltUssdInt( )    
    {
    }
    



//  End of File  
