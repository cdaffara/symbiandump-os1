/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CaudioclientstubDocument implementation
*
*/


// INCLUDE FILES
#include "audioclientstubAppUi.h"
#include "audioclientstubDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CaudioclientstubDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CaudioclientstubDocument* CaudioclientstubDocument::NewL(CEikApplication& aApp)
    {
    RDebug::Print( _L("CaudioclientstubDocument::NewL") ) ;
    CaudioclientstubDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CaudioclientstubDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CaudioclientstubDocument* CaudioclientstubDocument::NewLC(
        CEikApplication& aApp)
    {
    RDebug::Print( _L("CaudioclientstubDocument::NewLC") ) ;
    CaudioclientstubDocument* self = new ( ELeave ) CaudioclientstubDocument( aApp );

    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CaudioclientstubDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CaudioclientstubDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CaudioclientstubDocument::CaudioclientstubDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CaudioclientstubDocument::CaudioclientstubDocument(CEikApplication& aApp) :
    CAknDocument(aApp)
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CaudioclientstubDocument::~CaudioclientstubDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CaudioclientstubDocument::~CaudioclientstubDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CaudioclientstubDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CaudioclientstubDocument::CreateAppUiL()
    {
    RDebug::Print( _L("CaudioclientstubDocument::CreateAppUiL") ) ;
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return new ( ELeave )CaudioclientstubAppUi;
    }

// End of File
