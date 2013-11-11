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
* Implementation of CTigerApp
*/

// INCLUDE FILES
#include    <eikstart.h>
#include    "TigerApp.h"
#include    "TigerDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTigerApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CTigerApp::AppDllUid() const
    {
    return KUidTiger;
    }


// ---------------------------------------------------------
// CTigerApp::CreateDocumentL()
// Creates CTigerDocument object
// ---------------------------------------------------------
//
CApaDocument* CTigerApp::CreateDocumentL()
    {
    return CTigerDocument::NewL( *this );
    }


// ================= OTHER EXPORTED FUNCTIONS ==============


CApaApplication* NewApplication()
    {
    return new CTigerApp;
    }

TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// End of File

