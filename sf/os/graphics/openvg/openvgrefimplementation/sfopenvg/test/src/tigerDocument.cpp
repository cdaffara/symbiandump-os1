/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* Implementation of CTigerDocument class
*/


// INCLUDE FILES
#include "TigerDocument.h"
#include "TigerAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CTigerDocument::CTigerDocument
//
// Just calls the base class constructor.
// -----------------------------------------------------------------------------
//

CTigerDocument::CTigerDocument(CEikApplication& aApp)
: CAknDocument(aApp)
    {
    }

// -----------------------------------------------------------------------------
// CTigerDocument::~CTigerDocument
//
// Empty destructor.
// -----------------------------------------------------------------------------
//

CTigerDocument::~CTigerDocument()
    {
    }

// -----------------------------------------------------------------------------
// CTigerDocument::ConstructL
//
// Empty 2nd phase constructor.
// -----------------------------------------------------------------------------
//

void CTigerDocument::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CTigerDocument::NewL
//
// -----------------------------------------------------------------------------
//

CTigerDocument* CTigerDocument::NewL(
        CEikApplication& aApp)     // CTigerApp reference
    {
    CTigerDocument* self = new (ELeave) CTigerDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CTigerDocument::CreateAppUiL()
// Constructs and returns a CTigerAppUi object.
// ----------------------------------------------------
//

CEikAppUi* CTigerDocument::CreateAppUiL()
    {
    return new (ELeave) CTigerAppUi;
    }

// End of File
