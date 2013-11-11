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
* Description:  Main application class
*
*/


// INCLUDE FILES
#include "audioclientstub.hrh"
#include "audioclientstubDocument.h"
#include "audioclientstubApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CaudioclientstubApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CaudioclientstubApplication::CreateDocumentL()
    {
    // Create an audioclientstub document, and return a pointer to it
    RDebug::Print( _L(" CaudioclientstubApplication::CreateDocumentL") ) ;
    return CaudioclientstubDocument::NewL(*this);
    }

// -----------------------------------------------------------------------------
// CaudioclientstubApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CaudioclientstubApplication::AppDllUid() const
    {
    // Return the UID for the audioclientstub application
    RDebug::Print( _L(" CaudioclientstubApplication::AppDllUid") ) ;
    return KUidaudioclientstubApp;
    }

// End of File
