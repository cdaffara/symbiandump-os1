/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of SysLangUtil class.
*
*/


#include <s32file.h>
#include <centralrepository.h>

#include "syslangutil.h"
#include "syslangutiltrace.h"
#include "ssmmapperutilitystatic.h"
#include "syslangutilprivatecrkeys.h"
#include "ssmcommonlocale.h"
#include <CommonEngineDomainCRKeys.h>

const TInt KReadBufSize = 10;
const TInt KLangArraySize = 10;
//Maximum number of retries for starting cenrep transaction.
const TInt KMaxCountForReiteration = 5;

// R&D support: Use language file in Starters internal directory for module
// testing purposes.
#ifdef __STARTER_MODULE_TEST_SUPPORT__
  _LIT( KLanguagesIni, "C:\\private\\100059C9\\languages.txt" );
  _LIT( KRegionsIni, "C:\\private\\100059C9\\regions.txt" );
  _LIT( KCollationsIni, "C:\\private\\100059C9\\collations.txt" );
  
#else // __STARTER_MODULE_TEST_SUPPORT__
  _LIT( KLanguagesIni, "z:\\resource\\bootdata\\languages.txt" );
  _LIT( KRegionsIni, "z:\\resource\\bootdata\\regions.txt" );
  _LIT( KCollationsIni, "z:\\resource\\bootdata\\collations.txt" );
#endif // __STARTER_MODULE_TEST_SUPPORT__

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SysLangUtil::IsValidLanguage
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool SysLangUtil::IsValidLanguage(
    const TInt& aLanguage,
    RFs* aFileServerSession )
    {
    API_FUNC_ENTRY_TRACE;

    TBool hadFS;
    TInt err;
    aFileServerSession = CheckFS( aFileServerSession, hadFS, err );

    if ( err )
        {
        return EFalse;
        }

    RFile file;
    err = file.Open(
        *aFileServerSession,
        KLanguagesIni,
        EFileStream | EFileRead | EFileShareReadersOnly );

    if ( !err )
        {
        // Prepare Reader
        TFileText reader;
        reader.Set(file);
        if ( reader.Seek(ESeekStart) )
            {
            file.Close();
            if ( !hadFS )
                {
                aFileServerSession->Close();
                delete aFileServerSession;
                aFileServerSession = NULL;
                }
            return EFalse;
            }

        TBuf<KReadBufSize> readBuf;
        err = EFalse; // This solution is not very elegant but saves ROM :)

        while ( !reader.Read( readBuf ) && readBuf.Length() )
            {
            if ( readBuf.Length() > 1 )
                {
                TLex lex( readBuf );
                lex.SkipSpace();
                TInt language;
                err = lex.Val( language );

                if ( language == aLanguage )
                    {
                     err = ETrue;
                     break;
                    }
                }
            }
        file.Close();
        }
    else
        {
        err = EFalse;
        ERROR_TRACE_1( "ERROR: file '%S' is missing", &KLanguagesIni );
        }

    if ( !hadFS )
        {
        aFileServerSession->Close();
        delete aFileServerSession;
        aFileServerSession = NULL;
        }

    FUNC_EXIT_RET_TRACE( err );
    return err;
    }


// -----------------------------------------------------------------------------
// SysLangUtil::GetDefaultLanguage
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::GetDefaultLanguage(
    TInt& aLanguage,
    RFs* aFileServerSession )
    {
    API_FUNC_ENTRY_TRACE;

    TBool hadFS;
    TInt err;
    aFileServerSession = CheckFS( aFileServerSession, hadFS, err );

    if ( err )
        {
        return err;
        }

    RFile file;
    err = file.Open(
        *aFileServerSession,
        KLanguagesIni,
        EFileStream | EFileRead | EFileShareReadersOnly );

    if ( !err )
        {
        // Prepare Reader
        TFileText reader;
        reader.Set(file);
        err = reader.Seek( ESeekStart );
        if ( !err )
            {
            TBuf<KReadBufSize> readBuf;

            while ( !reader.Read( readBuf ) && readBuf.Length() )
                {
                if ( readBuf.Locate('d') != KErrNotFound )
                   {
                        TLex lex( readBuf );
                        lex.SkipSpace();
                        err = lex.Val( aLanguage );
                        break;
                   }

                readBuf.Zero();
                }

            if ( !readBuf.Length() )
                {
                ERROR_TRACE_1( "ERROR: file '%S' is missing", &KLanguagesIni );
                file.Close();
                return KErrNotFound;
                }
            }
        file.Close();
        }

    if ( !hadFS )
        {
        aFileServerSession->Close();
        delete aFileServerSession;
        aFileServerSession = NULL;
        }

    FUNC_EXIT_RET_TRACE( err );
    return err;
    }


// -----------------------------------------------------------------------------
// SysLangUtil::GetInstalledLanguages
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::GetInstalledLanguages(
    CArrayFixFlat<TInt>*& aLanguages,
    RFs* aFileServerSession )
    {
    API_FUNC_ENTRY_TRACE;

    if ( aLanguages )
        {
        delete aLanguages;
        }

    aLanguages = new CArrayFixFlat<TInt>( KLangArraySize );

    if ( !aLanguages )
        {
        return KErrNoMemory;
        }


    TBool hadFS;
    TInt err;
    aFileServerSession = CheckFS( aFileServerSession, hadFS, err );

    if ( err )
        {
        return err;
        }

    RFile file;
    err = file.Open(
        *aFileServerSession,
        KLanguagesIni,
        EFileStream | EFileRead | EFileShareReadersOnly );
    if ( !err )
        {
        // Prepare Reader
        TFileText reader;
        reader.Set( file );
        err = reader.Seek( ESeekStart );
        if ( !err )
            {
            TBuf<KReadBufSize> readBuf;

            for ( TInt i = 0; !reader.Read( readBuf ) && readBuf.Length(); i++ )
                {
                if ( readBuf.Length() > 0 )
                    {
                    TLex lex( readBuf );
                    lex.SkipSpace();
                    TInt language;
                    err = lex.Val( language );
                    if ( err ) break;
                    // Will not leave if array size (KLangArraySize) not exceeded
                    TRAP( err, aLanguages->AppendL( language ) );
                    err = KErrNone; // Ignore.
                    }
                readBuf.Zero();
                }
            }
        file.Close();
        }
    else
        {
        ERROR_TRACE_1( "ERROR: file '%S' is missing", &KLanguagesIni );
        }

    if ( !hadFS )
        {
        aFileServerSession->Close();
        delete aFileServerSession;
        aFileServerSession = NULL;
        }

    FUNC_EXIT_RET_TRACE( err );
    return err;
    }


// -----------------------------------------------------------------------------
// SysLangUtil::RestoreSIMLanguage
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::RestoreSIMLanguage(
    TInt &aLanguage,
    RFs* /*aFileServerSession*/ )
    {
    API_FUNC_ENTRY_TRACE;
    
    TUid mappedUid = SsmMapperUtility::CrUid( KCRUidSysLangUtil );

    CRepository* repository = NULL;
    TRAPD( errorCode, repository = CRepository::NewL( mappedUid ) );

    if ( errorCode == KErrNone )
        {
        errorCode = repository->Get( KSysLangUtilSimLanguage, aLanguage );

        if ( errorCode != KErrNone )
            {
            ERROR_TRACE_1( "Failed to get value from CentRep, error code %d.", errorCode );
            }
        }
    else
        {
        ERROR_TRACE_1( "Failed to open repository, error code %d.", errorCode );
        }

    delete repository;

    FUNC_EXIT_RET_TRACE( errorCode );
    return errorCode;
    }


// -----------------------------------------------------------------------------
// SysLangUtil::CheckFS
//
// -----------------------------------------------------------------------------
//
RFs* SysLangUtil::CheckFS( RFs* aRFs, TBool& aExist, TInt& aErr )
    {
    FUNC_ENTRY_TRACE;

    if ( !aRFs )
        {
        aExist = EFalse;
        aRFs = new RFs;
        if ( aRFs )
            {
            aErr = aRFs->Connect();
            if ( aErr != KErrNone)
                {
                delete aRFs;
                return NULL;
                }
            }
        else
            {
            aErr = KErrNoMemory;
            return NULL;
            }
        }
    else
        {
        // File server session given as argument MUST be already connected
        ASSERT_TRACE( aRFs->Handle() );

        aExist = ETrue;
        }

    aErr = KErrNone;

    FUNC_EXIT_TRACE;
    return aRFs;
    }

// -----------------------------------------------------------------------------
// SysLangUtil::ChangeLanguage
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::ChangeLanguage(const TInt  aLanguageCode)
	{
    API_FUNC_ENTRY_TRACE;    
    TRAPD( errorCode, LoadDllL( KGSDisplayTxtLang,aLanguageCode, KLanguageDllNameBase ) );      
    FUNC_EXIT_RET_TRACE( errorCode );
    return errorCode;	 
	}


// -----------------------------------------------------------------------------
// SysLangUtil::ChangeRegion
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::ChangeRegion(const TInt aRegionCode)
	{
    API_FUNC_ENTRY_TRACE;
    TRAPD( errorCode, LoadDllL( KGSRegion, aRegionCode, KRegionDllNameBase ) );     
    FUNC_EXIT_RET_TRACE( errorCode );
    return errorCode;
	}

// -----------------------------------------------------------------------------
// SysLangUtil::ChangeCollation
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::ChangeCollation(const TInt aCollationCode)
	{
    API_FUNC_ENTRY_TRACE; 
    TRAPD( errorCode, LoadDllL( KGSCollation, aCollationCode, KCollationDllNameBase ) );      
    FUNC_EXIT_RET_TRACE( errorCode );
    return errorCode;
	}
	

// -----------------------------------------------------------------------------
// SysLangUtil::GetInstalledRegions
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::GetInstalledRegions(CArrayFixFlat<TInt>*& aRegions,  RFs* aFileServerSession )
	{
	API_FUNC_ENTRY_TRACE;	
	TRAPD(err, ReadFileL(aRegions, KRegionsIni, aFileServerSession ));
	FUNC_EXIT_RET_TRACE( err );
	return err;
	}

// -----------------------------------------------------------------------------
// SysLangUtil::GetInstalledCollations
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SysLangUtil::GetInstalledCollations(CArrayFixFlat<TInt>*& aCollations,  RFs* aFileServerSession )
	{
	API_FUNC_ENTRY_TRACE;
	TRAPD(err, ReadFileL(aCollations,  KCollationsIni, aFileServerSession ));
	FUNC_EXIT_RET_TRACE( err );  
	return err;	
	}

void SysLangUtil::LoadDllL( const TUint32 aCategory,const TInt aCode, const TDesC& aDllNameBase )
    {
    CRepository* cenrep = NULL; 
    TUint32 keyInfo;
    cenrep = CRepository::NewLC( KCRUidCommonEngineKeys );
    
    // Re-iterate through StartTransaction, if errorCode returned is KErrLocked
    TInt errorCode = KErrLocked;
	//Counter to restrict re-iteration. 
	TInt counter = KMaxCountForReiteration;
    while ( KErrLocked == errorCode && counter > 0 )
        {
        errorCode = cenrep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
		--counter;
        }
    User::LeaveIfError(errorCode);
	cenrep->CleanupCancelTransactionPushL();
    User::LeaveIfError(cenrep->Set( aCategory, aCode)); 
           
    TBuf<KMaxDllExtensionLength> extension;       
    extension.Format( KDllExtensionFormat, aCode );
    
    // Padd ".1" to ".001" for compatibility.   
    for( ; extension.Length() < KMinDllExtensionLength ;) 
        {
        extension.Insert( KDllExtensionPaddingPosition,
                          KDllExtensionPadding );
        }

    TBuf<KMaxDllNameLength> dllName( aDllNameBase );
    dllName.Append( extension );
    
    //Loading DLL
    TExtendedLocale extLocale;
    extLocale.LoadSystemSettings();
    errorCode = extLocale.LoadLocaleAspect( dllName );
    ERROR_TRACE_2( "LoadLocaleAspect returned error %d while loading dll %S" ,errorCode, &dllName );
    User::LeaveIfError(errorCode);
	
    User::LeaveIfError(extLocale.SaveSystemSettings());
	errorCode = cenrep->CommitTransaction(keyInfo);
	if ( KErrNone != errorCode )
        {
        ERROR_TRACE_2( "CommitTransaction() error: Key Info %d for setting Category %d", keyInfo, aCategory  );
        User::Leave(errorCode);
        }
	CleanupStack::PopAndDestroy();// for CleanupCancelTransactionPushL()
	CleanupStack::PopAndDestroy(cenrep);
    User::LeaveIfError(TLocale().Set());    
    }

void SysLangUtil::ReadFileL(CArrayFixFlat<TInt>*& aArray, const TDesC& aFilePath, RFs* aFileServerSession )
    {
    TBool hadFS = EFalse;
    TInt err = KErrNone;
    if ( aArray )
        {
        delete aArray;
        aArray = NULL;
        }
    aArray = new(ELeave) CArrayFixFlat<TInt>( KLangArraySize );    
    
    aFileServerSession = CheckFS( aFileServerSession, hadFS, err );    
    User::LeaveIfError(err);
    RFile file;
    CleanupClosePushL(file);
    err = file.Open(*aFileServerSession, aFilePath, EFileStream | EFileRead | EFileShareReadersOnly );
    ERROR_TRACE_2( "SysLangUtil::ReadFileL() : Opening the file : %S failed with error : %d", &aFilePath, err) ; 
	User::LeaveIfError(err);
	
    // Prepare Reader
    TFileText reader;
    reader.Set( file );
    err = reader.Seek( ESeekStart );
    if ( !err )
        {
        TBuf<KReadBufSize> readBuf;
        for ( TInt i = 0; !reader.Read( readBuf ) && readBuf.Length(); ++i )
            {
            TLex lex( readBuf );
            lex.SkipSpace();
            TInt value;
            err = lex.Val( value );
            if ( err ) 
                {
                break;                
                }
            // IGNORE the Leave
            TRAP_IGNORE( aArray->AppendL( value ));                          
            readBuf.Zero();
            }
        }
    CleanupStack::PopAndDestroy(&file);    
    if ( !hadFS )
        {
        aFileServerSession->Close();
        delete aFileServerSession;
        aFileServerSession = NULL;
        }  
    }

