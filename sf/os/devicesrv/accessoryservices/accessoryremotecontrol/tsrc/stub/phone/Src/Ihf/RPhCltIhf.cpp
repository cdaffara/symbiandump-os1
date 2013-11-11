/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
// Connect to the  server - default number of message slots = 1
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltIhf::Open( RPhCltServer& aServer, TInt /*aPriority*/ )
    {
    __ASSERT_ALWAYS( aServer.Handle() , 
        User::Panic( KPhClientPanicCategory , KErrBadHandle ) );

    return CreateSubSession( 
        aServer, 
        EPhoneServerIhfSubSessionOpen, 
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltIhf::Close
//
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltIhf::Close()
    {    
    CloseSubSession( EPhoneServerIhfSubSessionClose );
    }


// -----------------------------------------------------------------------------
// RPhCltIhf::SetIhfMode
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltIhf::SetIhfMode( TBool aMode, TBool aShowNote )
    {
    __ASSERT_ALWAYS( SubSessionHandle() , 
        User::Panic( KPhClientPanicCategory , KErrBadHandle ) );

    return SendReceive( 
        EPhoneServerIhfSubSessionSetMode, 
        TIpcArgs( 
            aMode,
            aShowNote ) );
    }


// -----------------------------------------------------------------------------
// RPhCltIhf::SetIhfModeFromPhone
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltIhf::SetIhfModeFromPhone( TBool aMode, TBool aShowNote )
    {
    __ASSERT_ALWAYS( SubSessionHandle() , 
        User::Panic( KPhClientPanicCategory , KErrBadHandle ) );
    
    return SendReceive( 
        EPhoneServerIhfSubSessionSetModeFromPhone, 
        TIpcArgs( 
            aMode,
            aShowNote ) );
    }


// End of File
