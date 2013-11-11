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
* Description:  Image Handler Base class.
*
*/



// INCLUDE FILES
#include    "CPhCltImageHandler.h"
#include    "CPhCltImageHandlerImplementation.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltMessenger::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltImageHandler* CPhCltImageHandler::NewL()
    {
    return CPhCltImageHandlerImplementation::NewL();
    }

// -----------------------------------------------------------------------------
// CPhCltMessenger::CPhCltMessenger
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltImageHandler::CPhCltImageHandler( )
    {
    }
    
//  End of File  
