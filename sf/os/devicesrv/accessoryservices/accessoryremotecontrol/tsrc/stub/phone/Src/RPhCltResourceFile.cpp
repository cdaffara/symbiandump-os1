/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Resource File handler.
*
*/



// INCLUDE FILES
#include    "RPhCltResourceFile.h"
#include    "ConeResLoader.h"
#include    "PhCltClientServer.h"


// CONSTANTS

// Panic category
_LIT( KPhCltResourceFilePanic, "PhCltRes" );




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPhCltResourceFile::RPhCltResourceFile
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPhCltResourceFile::RPhCltResourceFile()
    : iLoader( NULL )
    {
    }


// -----------------------------------------------------------------------------
// RPhCltResourceFile::OpenL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltResourceFile::OpenL( CCoeEnv& aEnv )
    {
    // Must not be called if already loaded.
    __ASSERT_ALWAYS( 
        !iLoader, 
        User::Panic( KPhCltResourceFilePanic, KErrNone ) );

    TFileName* tmpFileName =
        new ( ELeave ) TFileName;
    CleanupStack::PushL( tmpFileName );
    tmpFileName->Append( KPhCltServerZDrive );
    tmpFileName->Append( KDC_RESOURCE_FILES_DIR );
    tmpFileName->Append( KPhCltResourceFileNameAndPath );
    tmpFileName->ZeroTerminate();

    // Load resource file
    RConeResourceLoader* loader = 
        new ( ELeave ) RConeResourceLoader( aEnv );
    CleanupStack::PushL( loader );
    loader->OpenL( *tmpFileName );
    CleanupStack::Pop( loader );

    // Completed, store loader.
    iLoader = loader;

    CleanupStack::PopAndDestroy( tmpFileName );
    }


// -----------------------------------------------------------------------------
// RPhCltResourceFile::Close
//
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltResourceFile::Close()
    {
    if ( iLoader )
        {
        iLoader->Close();
        delete iLoader;
        iLoader = NULL;
        }
    }


//  End of File  
