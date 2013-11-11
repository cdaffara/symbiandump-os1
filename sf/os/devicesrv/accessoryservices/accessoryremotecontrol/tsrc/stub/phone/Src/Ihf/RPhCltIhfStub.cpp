/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IHF
*
*/



// INCLUDE FILES
#include "RPhCltServer.h"
#include "RPhCltIhf.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPhCltIhf::RPhCltIhf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPhCltIhf::RPhCltIhf( )
    : RSubSessionBase()
    {
    }


// -----------------------------------------------------------------------------
// RPhCltIhf::Open
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltIhf::Open( RPhCltServer& /*aServer*/, TInt /*aPriority*/ )
    {
    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// RPhCltIhf::Close
//
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltIhf::Close()
    {    
    }


// -----------------------------------------------------------------------------
// RPhCltIhf::SetIhfMode
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltIhf::SetIhfMode( TBool /*aMode*/, TBool /*aShowNote*/ )
    {
    return KErrNotSupported;
    }


// -----------------------------------------------------------------------------
// RPhCltIhf::SetIhfModeFromPhone
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltIhf::SetIhfModeFromPhone( 
        TBool /*aMode*/, TBool /*aShowNote*/ )
    {
    return KErrNotSupported;
    }


// End of File
