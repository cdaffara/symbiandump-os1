/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Implementation of SsmLanguageLoader class.
*
*/

#include <e32std.h>
#include <hal.h>

#include "ssmlanguageloader.h"
#include "ssmcommonlocale.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// SsmLanguageLoader::StoreLanguageToHal
//
// -----------------------------------------------------------------------------
//
TInt SsmLanguageLoader::StoreLanguageToHal( const TInt aLanguage )
    {
    FUNC_LOG;

    TInt errorCode = HAL::Set( HAL::ELanguageIndex, aLanguage );
    return errorCode;
    }


// ---------------------------------------------------------------------------
// SsmLanguageLoader::LoadLocale
//
// ---------------------------------------------------------------------------
//
TInt SsmLanguageLoader::LoadLocale( const TInt aLanguage, const TInt aRegion, const TInt aCollation)
    {
    FUNC_LOG;

    TInt errorCode = StoreLanguageToHal( aLanguage );
    ERROR( errorCode, "Failed to store language code to HAL" );

    //Format the language dll to be loaded
    //Dot plus four to six digit locale
    TBuf<KMaxDllExtensionLength> extension; 
    extension.Format( KDllExtensionFormat, aLanguage );

    //Padd with zero to make the file extension length to be minimum of four
    for( ; extension.Length() < KMinDllExtensionLength ;) 
        {
        extension.Insert( KDllExtensionPaddingPosition, KDllExtensionPadding );
        }

    TBuf<KMaxDllNameLength> languageDllName( KLanguageDllNameBase );
    languageDllName.Append( extension );
    INFO_1( "Loading language DLL named '%S'", &languageDllName );

    //Format the region dll to be loaded
    //Dot plus four to six digit locale
    extension.Zero();
    extension.Format( KDllExtensionFormat, aRegion );

    //Padd with zero to make the file extension length to be minimum of four
    for( ; extension.Length() < KMinDllExtensionLength ;) 
        {
        extension.Insert( KDllExtensionPaddingPosition, KDllExtensionPadding );
        }

    TBuf<KMaxDllNameLength> regionDllName( KRegionDllNameBase );
    regionDllName.Append( extension );
    INFO_1( "Loading Region DLL named '%S'", &regionDllName );


    //Format the collation file to be loaded
    //Dot plus four to six digit locale
    extension.Zero();
    extension.Format( KDllExtensionFormat, aCollation );

    //Padd with zero to make the file extension length to be minimum of four
    for( ; extension.Length() < KMinDllExtensionLength ;) 
        {
        extension.Insert( KDllExtensionPaddingPosition, KDllExtensionPadding );
        }

    TBuf<KMaxDllNameLength> collationDllName( KCollationDllNameBase );
    collationDllName.Append( extension );
    INFO_1( "Loading Collation DLL named '%S'", &collationDllName );
    
    //Load the given Language, Region and Collation dlls
    errorCode = ChangeLocale( languageDllName, regionDllName, collationDllName );
    if ( errorCode == KErrNone )
        {
        TLocale().Set();
        }
    return errorCode;
    }

// ---------------------------------------------------------------------------
// SsmLanguageLoader::ChangeLocale
//
// ---------------------------------------------------------------------------
//
TInt SsmLanguageLoader::ChangeLocale( const TDesC& aLanguageDllName,  const TDesC& aRegionDllName, const TDesC& aCollationDllName )
    {
    FUNC_LOG;
    TExtendedLocale extLocale;

    INFO_3( "Loading Language dll '%S', Region dll '%S' and Collation dll '%S'",
            &aLanguageDllName, &aRegionDllName, &aCollationDllName );
    TInt errorCode = extLocale.LoadLocale( aLanguageDllName, aRegionDllName, aCollationDllName);
    if ( errorCode == KErrNone )
        {
        //Save the loaded locale settings
        errorCode = extLocale.SaveSystemSettings();
        ERROR( errorCode, "Failed to save locale settings" );
        }
    else
        {
        INFO_3( "Failed to load Language dll '%S', Region dll '%S' and Collation dll '%S'",
                    &aLanguageDllName, &aRegionDllName, &aCollationDllName );
        }

    return errorCode;
    }
