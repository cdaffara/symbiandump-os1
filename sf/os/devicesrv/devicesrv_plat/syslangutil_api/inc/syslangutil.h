/*
* Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of SysLangUtil class.
*
*/


#ifndef SYSLANGUTIL_H
#define SYSLANGUTIL_H

#include <e32base.h>

class RFs;

/**
*  Language selection -related utility methods.
*
*  ?more_complete_description
*
*  @lib SysLangUtil.lib
*  @since S60 3.0
*/
class SysLangUtil
    {

public:

    /**
    * Checks if language given as an argument is valid i.e. installed
    * language.
    * @param aLanguage returned Symbian language code matching SIM one.
    * @param aFileServerSession Optional file server session handle. If
    *           omitted, new one will be created.
    * @return ETrue if language is installed, otherwise returns EFalse.
    */
    IMPORT_C static TBool IsValidLanguage(
        const TInt& aLanguage,
        RFs* aFileServerSession = NULL );

    /**
    * Checks and returns the default language in a param given.
    * @param aLanguage  Symbian language as a number
    * @param aFileServerSession Optional file server session handle. If
    *           omitted, new one will be created.
    * @return returns KErrNone if everything went OK, otherwise returns
    *           the errorcode.
    */
    IMPORT_C static TInt GetDefaultLanguage(
        TInt& aLanguage,
        RFs* aFileServerSession = NULL );

    /**
    * Checks and returns installed languages in a param given.
    * @param aLanguages  Symbian languages as a arrayfixflat<int>, ownership
    *           given to user, so user must take care of proper cleanup!
    * @param aFileServerSession Optional file server session handle. If
    *           omitted, new one will be created.
    * @return returns KErrNone if everything went OK, otherwise returns
    *           the errorcode.
    */
    IMPORT_C static TInt GetInstalledLanguages(
        CArrayFixFlat<TInt>*& aLanguages,
        RFs* aFileServerSession = NULL );

    /**
    * Returns SIM preferred languages in a param given.
    * @param aLanguage Language that was stored in flash file system
    * @param aFileServerSession Optional file server session handle. If
    *           omitted, new one will be created.
    * @return returns KErrNone if everything went OK, otherwise returns
    *           the errorcode.
    */
    IMPORT_C static TInt RestoreSIMLanguage(
        TInt &aLanguage,
        RFs* aFileServerSession = NULL );
	
	/**
    * Changes the Language to the one which is passed as an argument.
	* @param aLanguageCode  Language to be set .
    * 
    * @return returns KErrNone if everything is OK, otherwise returns
    *           errorcode.
    */
	IMPORT_C static TInt SysLangUtil::ChangeLanguage(const TInt aLanguageCode);
	
	/**
    * Changes the region to the one which is passed as an argument.
	* @param aRegionCode  region code to be set .
    * 
    * @return returns KErrNone if everything is OK, otherwise returns
    *           errorcode.
    */
	IMPORT_C static TInt SysLangUtil::ChangeRegion(const TInt aRegionCode);
	
	/**
    * Changes the collation to the one which is passed as an argument.
    * @param aCollationCode  collation to be set .
    * 
    * @return returns KErrNone if everything is OK, otherwise returns
    *           errorcode.
    */
	
	IMPORT_C static TInt SysLangUtil::ChangeCollation(const TInt aCollationCode);
	
	/**
    * Checks and returns installed regions in a param given.
    * @param aRegions  Symbian regions as a arrayfixflat<int>, ownership
    *           given to user, so user must take care of proper cleanup!
    * @param aFileServerSession Optional file server session handle. If
    *           omitted, new one will be created.
    * @return returns KErrNone if everything went OK, otherwise returns
    *           the errorcode.
    */
	IMPORT_C static TInt SysLangUtil::GetInstalledRegions(CArrayFixFlat<TInt>*& aRegions,
															RFs* aFileServerSession = NULL );
	
	/**
    * Checks and returns installed collations in a param given.
    * @param aRegions  Symbian collations as a arrayfixflat<int>, ownership
    *           given to user, so user must take care of proper cleanup!
    * @param aFileServerSession Optional file server session handle. If
    *           omitted, new one will be created.
    * @return returns KErrNone if everything went OK, otherwise returns
    *           the errorcode.
    */
															
	IMPORT_C static TInt SysLangUtil::GetInstalledCollations(CArrayFixFlat<TInt>*& aCollations,
															RFs* aFileServerSession = NULL );													

private:

    // Utility:

    static RFs* CheckFS( RFs* aRFs, TBool &aExist, TInt &aErr );
	static void LoadDllL( const TUint32 aCategory,const TInt aCode, const TDesC& aDllNameBase );
	static void ReadFileL(CArrayFixFlat<TInt>*& aArray, const TDesC& aFilePath, RFs* aFileServerSession = NULL );
    };

#endif // SYSLANGUTIL_H
