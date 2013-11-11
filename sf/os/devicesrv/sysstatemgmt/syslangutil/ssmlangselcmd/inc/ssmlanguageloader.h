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
* Description: Declaration of SsmLanguageLoader class.
*
*/

#ifndef SSMLANGUAGELOADER_H
#define SSMLANGUAGELOADER_H

#include <e32cmn.h>

/**
*  Static function managing operations related to loading locale based on
*  language code.
*
*  @lib None
*  @since S60 3.2
*/
class SsmLanguageLoader
    {

public:

    /**
    * Store language code to HAL and load corresponding Language, Region and Collation DLLs.
    *
    * @param aLanguage Language code identifying which language DLL to load.
    * @param aRegion Region code identifying which region DLL to load.
    * @param aCollation Collation code identifying which collation DLL to load.
    * @return KErrNone if loading was successful, one of Symbian error codes
    * otherwise.
    */
    static TInt LoadLocale( const TInt aLanguage, const TInt aRegion, const TInt aCollation);
private:

    /**
    * Store language code to HAL and load corresponding locale DLL.
    *
    * @since S60 3.2
    * @param aLanguage Language code identifying which locale DLL to load.
    * @return KErrNone if successful, one of Symbian error codes otherwise.
    */
    static TInt StoreLanguageToHal( const TInt aLanguage );

    /**
    * Loads the given Language, Region and Collation DLLs and saves the settings
    *
    * @param aLanguageDllName The name of the Language DLL to load.
    * @param aRegionDllName The name of the Region DLL to load.
    * @param aCollationDllName The name of the Collation DLL to load.
    * @return KErrNone if successful, one of Symbian error codes otherwise.
    */
    static TInt ChangeLocale( const TDesC& aLanguageDllName,  const TDesC& aRegionDllName, const TDesC& aCollationDllName );
    };
#endif // SSMLANGUAGELOADER_H
