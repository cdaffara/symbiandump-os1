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
* Description:  Command Handler Base class.
*
*/



// INCLUDE FILES
#include    "CPhCltCommandHandler.h"
#include    "CPhCltComHandImplementation.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltCommandHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltCommandHandler* CPhCltCommandHandler::NewL()
    {
    return CPhCltCommandHandlerImplementation::NewL();
    }

// -----------------------------------------------------------------------------
// CPhCltCommandHandler::CPhCltCommandHandler
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltCommandHandler::CPhCltCommandHandler( )
    {
    }
    



//  End of File  
