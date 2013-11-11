/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Implementation of CSsmLangSelCmd class.
*
*/

#include "ssmlangselcmd.h"
#include "ssmmapperutility.h"
#include "ssmalternativelanguages.h"
#include "ssmlanguageloader.h"
#include "syslangutilprivatecrkeys.h"
#include "trace.h"
#include "ssmmapperutilityinternalpskeys.h"

#include <syslangutil.h>
#include <CommonEngineDomainCRKeys.h>
#include <centralrepository.h>


/** Default granularity for list of installed languages. */
const TInt KLanguageListGranularity( 4 );

/** Any of the code set to zero means auto selection is enabled. */
const TInt KSettingAutomatic = 0;

/** The maximum number of regions to be appended to the hash set. */
const TInt KMaxNumberOfRegions = 56;

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// HashLangRegionMappingFunction
// ---------------------------------------------------------------------------
//
inline TUint32 HashLangRegionMappingFunction(const TLanguageRegion& aMapping)
    {
    return aMapping.iLanguage;
    }

// ---------------------------------------------------------------------------
// LangRegionMappingIdentityRelation
// ---------------------------------------------------------------------------
//
inline TBool LangRegionMappingIdentityRelation(const TLanguageRegion& aMapping1,
    const TLanguageRegion& aMapping2)
    {
    return (aMapping1.iLanguage == aMapping2.iLanguage);
    }

// ---------------------------------------------------------------------------
// RegionLangMappingIdentityRelation
// ---------------------------------------------------------------------------
//
inline TBool RegionLangMappingIdentityRelation(const TLanguageRegion& aMapping1,
    const TLanguageRegion& aMapping2)
    {
    return (aMapping1.iRegion == aMapping2.iRegion);
    }

// ---------------------------------------------------------------------------
// CompareLanguageMapping
// ---------------------------------------------------------------------------
//
static TInt CompareLanguageMapping(TLanguageRegion const &aLanguage1, TLanguageRegion const &aLanguage2)
    {
    if( aLanguage1.iLanguage == aLanguage2.iLanguage )
        {
        return 0;
        }
    return ( aLanguage1.iLanguage < aLanguage2.iLanguage ) ? -1 : 1;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::NewL
// ---------------------------------------------------------------------------
//
CSsmLangSelCmd* CSsmLangSelCmd::NewL()
	{
    FUNC_LOG;
    CSsmLangSelCmd* self = new ( ELeave ) CSsmLangSelCmd();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	return self;	
	}

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::CSsmLangSelCmd
// ---------------------------------------------------------------------------
//
CSsmLangSelCmd::CSsmLangSelCmd() : CActive( EPriorityNormal ),
    iState( EQueryListNone ),
    iLangRegionMappingHashSet( &::HashLangRegionMappingFunction, &::LangRegionMappingIdentityRelation )
    {
    FUNC_LOG;

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::~CSsmLangSelCmd
// ---------------------------------------------------------------------------
//
CSsmLangSelCmd::~CSsmLangSelCmd()
    {
    FUNC_LOG;
    Cancel();
    delete iLangList;
    delete iMapperUtility;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::ConstructL
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::ConstructL()
    {
    FUNC_LOG;
    iMapperUtility = CSsmMapperUtility::NewL();
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::Initialize
// ---------------------------------------------------------------------------
//
 TInt CSsmLangSelCmd::Initialize( CSsmCustomCommandEnv* aCmdEnv )
     {
     FUNC_LOG;

     iEnv = aCmdEnv;
     TRAPD( errorCode, InitializeL() );
     return errorCode;
     }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::InitializeL
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::InitializeL()
    {
    FUNC_LOG;
    ASSERT_TRACE( iEnv );

    iValidLanguages = new ( ELeave ) CArrayFixFlat<TInt>( KLanguageListGranularity );
    
    RFs* fsSession = &( const_cast<RFs&>( iEnv->Rfs() ) );
    
    //Get the list of installed languages
    TInt errorCode = SysLangUtil::GetInstalledLanguages( iValidLanguages,
                                                         fsSession );

    ERROR( errorCode, "Failed to get installed languages" );
    User::LeaveIfError( errorCode );
    
    //Initialize the Language and Region mapping hash set
    InitializeRegionMappingL();

    //Get the number of installed languages
    const TInt validLangCount = iValidLanguages->Count();
    TLinearOrder<TLanguageRegion> order(&CompareLanguageMapping);
    
    //Get the mapped regions for the installed languages
    for(TInt index = 0; index < validLangCount; ++index) 
        {
        TLanguageRegion region = iLangRegionMappingHashSet.FindL(TLanguageRegion(iValidLanguages->At(index)));
        INFO_2( "Found Region code = %d for language %d", region.iRegion, region.iLanguage );
        iRegionsArray.InsertInOrderL(region, order);
        }

    //Connect to Misc adaptation
    errorCode = iAdaptation.Connect();
    ERROR( errorCode, "Failed to connect to RSsmMiscAdaptation" );
    User::LeaveIfError( errorCode );
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::InitializeRegionMappingL
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::InitializeRegionMappingL()
    {
    //Reserve the memory for the number of mappings to be appended
    iLangRegionMappingHashSet.ReserveL( KMaxNumberOfRegions );

    //Insert the Language - Region mapping
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangEnglish,             ERegGBR ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangFrench,              ERegFRA ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangGerman,              ERegDEU ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangSpanish,             ERegESP ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangItalian,             ERegITA ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangSwedish,             ERegSWE ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangDanish,              ERegDNK ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangNorwegian,           ERegNOR ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangFinnish,             ERegFIN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangAmerican,            ERegUSA ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangPortuguese,          ERegPRT ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangTurkish,             ERegTUR ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangIcelandic,           ERegISL ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangRussian,             ERegRUS ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangHungarian,           ERegHUN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangDutch,               ERegNLD ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangBelgianFlemish,      ERegBEL ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangCzech,               ERegCZE ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangSlovak,              ERegSVK ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangPolish,              ERegPOL ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangSlovenian,           ERegSVN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangTaiwanChinese,       ERegTWN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangHongKongChinese,     ERegHKG ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangPrcChinese,          ERegCHN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangJapanese,            ERegJPN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangThai,                ERegTHA ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangArabic,              ERegARE ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangTagalog,             ERegPHL ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangBulgarian,           ERegBGR ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangCatalan,             ERegESP ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangCroatian,            ERegHRV ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangEstonian,            ERegEST ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangFarsi,               ERegIRN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangCanadianFrench,      ERegCAN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangGreek,               ERegGRC ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangHebrew,              ERegISR ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangHindi,               ERegIND ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangIndonesian,          ERegIDN ) );
	iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangKorean,          	  ERegKOR ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangLatvian,             ERegLVA ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangLithuanian,          ERegLTU ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangMalay,               ERegMYS ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangBrazilianPortuguese, ERegBRA ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangRomanian,            ERegROU ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangSerbian,             ERegSCG ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangLatinAmericanSpanish, ERegMEX ));
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangUkrainian,           ERegUKR ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangUrdu,                ERegPAK ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangVietnamese,          ERegVNM ) );
#ifdef __E32LANG_H__
    // 5.0
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangBasque,              ERegESP ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangGalician,            ERegESP ) );
#endif //__E32LANG_H__
#if !defined(__SERIES60_31__)
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangEnglish_Apac,        ERegGBR ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangEnglish_Taiwan,      ERegTWN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangEnglish_HongKong,    ERegHKG ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangEnglish_Prc,         ERegCHN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangEnglish_Japan,       ERegJPN ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangEnglish_Thailand,    ERegTHA ) );
    iLangRegionMappingHashSet.InsertL( TLanguageRegion( ELangMalay_Apac,          ERegMYS ) );
#endif //!defined(__SERIES60_31__)
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::Execute
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::Execute( const TDesC8& /*aParams*/, TRequestStatus& aRequest )
    {
    FUNC_LOG;
    
    ASSERT_TRACE( !iClientStatus );
    ASSERT_TRACE( !IsActive() );
    ASSERT_TRACE( iEnv );

    aRequest = KRequestPending;
    iClientStatus = &aRequest;

    RFs* fsSession = &( const_cast<RFs&>( iEnv->Rfs() ) );
    
    //Get the default language
    TInt errorCode = SysLangUtil::GetDefaultLanguage( iDefaultLanguage, fsSession );
    ERROR( errorCode, "Failed to get default language" );

    //Get the language selected
    TInt lastSelectedLang = GetLastSelectedLang();
    TInt lastSelectedRegion ( KErrNotFound );
    TInt lastSelectedCollation ( KErrNotFound );
    
    //Reset error code to KErrNotFound
    errorCode = KErrNotFound;
    
    if ( KSettingAutomatic == lastSelectedLang )
        {
        //Auto selection is enabled
        SetIndividualSettingsToCentRep( KSettingAutomatic, KSettingAutomatic );
        PrepareNextList();
        }
    else if ( ValidateAndGetSettings( lastSelectedLang, lastSelectedRegion, lastSelectedCollation ) )
        {
        //Validate and get the selected Language, Region and Collation code
        errorCode = UseLocale( lastSelectedLang , lastSelectedRegion, lastSelectedCollation );
        ERROR( errorCode, "Failed to load selected Language, Region and Collation" );
        }

    //Load the default Language, Region and Collation in case of loading selected codes failed 
    //and auto selection is disabled.
    if( KErrNone != errorCode && KSettingAutomatic != lastSelectedLang )
        {
        //Validate and use the default codes 
        errorCode = ValidateAndUseDefaultLanguage();
        ERROR( errorCode, "Failed to load default Language, Region and Collation" );
        }
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::GetLastSelectedLang
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::GetLastSelectedLang()
    {
    FUNC_LOG;

    TInt value( 0 );
    TInt errorCode = iMapperUtility->CrValue( KCRUidCommonEngineKeys, 
                                              KGSDisplayTxtLang,
                                              value );
    ERROR( errorCode, "Failed read KGSDisplayTxtLang CenRep key with error= %d"  );       
    
    INFO_1( "Last selected language is %d", value );
    return value;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::SetIndividualSettingsToCentRep
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::SetIndividualSettingsToCentRep( const TInt aRegion, const TInt aCollation )
    {
    FUNC_LOG;
    CRepository* cenrep = NULL;
    TRAPD( errorCode, cenrep = CRepository::NewL( KCRUidCommonEngineKeys ) );
  
    if ( KErrNone == errorCode )
        {
        //Set the Region CR with the given value
        errorCode = cenrep->Set( KGSRegion, aRegion );
        ERROR( errorCode, "Failed to Set Region code to CentRep, %d"  );                
        }
    else
        {
        INFO_1( "Failed to initialize cen rep %d", errorCode );        
        }

    if ( KErrNone == errorCode )
        {
        //Set the Collation CR with the given value
        errorCode = cenrep->Set( KGSCollation, aCollation );
        ERROR( errorCode, "Failed to Set Collation code to CentRep, %d"  );               
        }
    delete cenrep;
    return errorCode;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::ValidateAndGetSettings
//
// ---------------------------------------------------------------------------
//
TBool CSsmLangSelCmd::ValidateAndGetSettings( const TInt aLastSelectedLang, TInt& aLastSelectedRegion, TInt& aLastSelectedCollation )
    {
    TBool isValid( EFalse );
    //Validate and get the selected settings
	TInt errorCode = KErrNone;
    TRAP( errorCode, isValid = ValidateAndGetSettingsL( aLastSelectedLang, aLastSelectedRegion, aLastSelectedCollation ) );
    ERROR_1( errorCode, "Validation failed for the selected Language, region and Collation with error %d", errorCode );
    return isValid;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::ValidateAndGetSettingsL
//
// ---------------------------------------------------------------------------
//
TBool CSsmLangSelCmd::ValidateAndGetSettingsL( const TInt aLastSelectedLang, TInt& aLastSelectedRegion, TInt& aLastSelectedCollation )
    {
    TBool isValid( EFalse );

    //Get the selected Region and Collation 
    GetIndividualSettingsFromCentRepL( aLastSelectedRegion, aLastSelectedCollation );

    //Check whether the selected Language is valid or not
    if( IsValid( aLastSelectedLang ) )
        {
         if (KSettingAutomatic >= aLastSelectedRegion )
            {
            //Selected Region is Automatic. Get the Region associated 
            //with the Language being loaded
            if( ValidateAndGetMappedRegion( aLastSelectedLang, aLastSelectedRegion ) )
                {
                isValid = ETrue;
                }
            }
         else if( IsRegionValidL( aLastSelectedRegion ) )
                {
                //Selected Region is a valid Region
                isValid = ETrue;
                }
            
         if ( KSettingAutomatic >= aLastSelectedCollation )
             {
             //Collation is not set. Use the language code as the collation code
             aLastSelectedCollation = aLastSelectedLang;
             }
        }
    
    return isValid;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::IsValid
//
// ---------------------------------------------------------------------------
//
TBool CSsmLangSelCmd::IsValid( const TInt aLanguage ) const
    {
    FUNC_LOG;
    ASSERT_TRACE( iValidLanguages );

    TBool valid( EFalse );
    TKeyArrayFix keyProp( 0, ECmpTInt );
    TInt index( KErrNotFound );
    // Returns zero if element is found.
    if ( 0 == iValidLanguages->Find( aLanguage, keyProp, index ) )
        {
        valid = ETrue;
        }
    
    return valid;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::GetLanguageFromCentRep
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::GetLanguageFromCentRep()
    {
    FUNC_LOG;

    TInt lang = iDefaultLanguage;
    TInt errorCode = iMapperUtility->CrValue( KCRUidSysLangUtil,
                                              KSysLangUtilSimLanguage,
                                              lang );
    
    ERROR_1( errorCode, "Failed to read language code from CenRep %d", 
             errorCode );
    return lang;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::IsRegionValidL
//
// ---------------------------------------------------------------------------
//
TBool CSsmLangSelCmd::IsRegionValidL( const TInt aRegion ) const
    {
    FUNC_LOG;
    TInt index = iRegionsArray.FindL(TLanguageRegion(0, aRegion), &::RegionLangMappingIdentityRelation);

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::ValidRegionAndGetMappedLanguage
//
// ---------------------------------------------------------------------------
//
TBool CSsmLangSelCmd::ValidateAndGetMappedRegion( const TInt aLanguage, TInt& aRegion )
    {
    FUNC_LOG;

    TBool valid( EFalse );
    TLanguageRegion lang( aLanguage );

    //Find the Language - Region mapping for the given Language
    TInt index = iRegionsArray.Find( lang, &::LangRegionMappingIdentityRelation );
    if( 0 <= index )
        {
        //Set the Region associated with the given Language
        aRegion = iRegionsArray[index].iRegion;
        INFO_1( "ValidRegionAndGetMappedLanguage:: Found region %d", aRegion);
        valid = ETrue;
        }
    else
        {
        INFO_1( "ValidRegionAndGetMappedLanguage:: Not Found region for lang %d", aLanguage);    
        }

    return valid;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::GetIndividualSettingsFromCentRepL
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::GetIndividualSettingsFromCentRepL( TInt& aRegion, TInt& aCollation )
    {
    FUNC_LOG;
    CRepository* cenrep = CRepository::NewL( KCRUidCommonEngineKeys );
    CleanupStack::PushL( cenrep );
    
    //Get the last selected Region from central repository
    User::LeaveIfError(cenrep->Get( KGSRegion, aRegion));   
        
    //Get the last selected Collation from central repository
    User::LeaveIfError(cenrep->Get( KGSCollation, aCollation));    
    
    INFO_2( "Last selected region %d and collation %d", aRegion, aCollation );
    CleanupStack::PopAndDestroy( cenrep );
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::ValidateAndUseDefaultLanguage
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::ValidateAndUseDefaultLanguage()
    {
    FUNC_LOG;
    TLanguageRegion langRegion;
    
    //Get the Region associated with default Language
    TRAPD( error, langRegion = iLangRegionMappingHashSet.FindL( TLanguageRegion( iDefaultLanguage ) ) );
    if( KErrNone == error )
       {
       //Load the settings for default Language
       error = UseLocale( iDefaultLanguage, langRegion.iRegion, iDefaultLanguage );
       ERROR_1( error, "Failed to load default language setting %d", error );
       }
    else
        {
        INFO_1( "Failed to find the Region associated with default Language %d", error );
        }

    if ( iClientStatus )
        {
        ERROR_1( error, "Language selection custom command completed with error %d", error );
        User::RequestComplete( iClientStatus, error );
        iClientStatus = NULL;
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::UseLocale
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::UseLocale( const TInt aLang, const TInt aRegion, const TInt aCollation )
    {
    FUNC_LOG;
    TInt errorCode(KErrNotFound);

    //Load the given Language, Region and Collation DLLs
    errorCode = SsmLanguageLoader::LoadLocale( aLang, aRegion, aCollation );

    if( KErrNone != errorCode )
        {
        INFO_1( "Loading failed with error  %d", errorCode);
        }
    else if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, errorCode );
        iClientStatus = NULL;
        }
    return errorCode;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::RunL
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::RunL()
    {
    FUNC_LOG;
    TInt errorCode = iStatus.Int();
    
    ERROR( errorCode, "CSsmLangSelCmd::RunL with error" );

    if ( iState == EQueryListSize )
        {
        if ( errorCode == KErrNone )
            {
            TRAP( errorCode, FetchLanguageListL() );
            ERROR( errorCode, "Failed to fetch language list" );
            }
        else
            {
            TryNextList();
            }
        }
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::UseLanguage
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::UseLanguage( const TInt aLang )
    {
    FUNC_LOG;
    TInt errorCode = KErrNotFound;
    TInt region = KErrNotFound;

    //Validate and get associated Region
    if( ValidateAndGetMappedRegion( aLang, region ) )
        {
        errorCode = SsmLanguageLoader::LoadLocale( aLang, region, aLang );
        }
    else
        {
        INFO_1( "Validation failed for the language (auto) %d", aLang );
        }

    if( KErrNone != errorCode )
        {
        INFO_1( "Loading the auto settings failed with error %d", errorCode );
        
        //Use the default setting as the selected one failed
        errorCode = ValidateAndUseDefaultLanguage();
        }
    else if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, errorCode );
        iClientStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::ExecuteCancel()
    {
    FUNC_LOG;

    Cancel();
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::DoCancel
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::DoCancel()
    {
    FUNC_LOG;
    
    //Cancel the pending request with misc adaptation
    if( EQueryListNone != iState )
        {
        iAdaptation.Cancel();
        }
    
    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, KErrCancel );
        iClientStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::Close
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::Close()
    {
    FUNC_LOG;
    delete iValidLanguages;
    iValidLanguages = NULL;
    iRegionsArray.Close();
    iLangRegionMappingHashSet.Close();
    iAdaptation.Close();
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::Release
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::Release()
    {
    FUNC_LOG;

	delete this;
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::PrepareNextList
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::PrepareNextList()
    {
    FUNC_LOG;
    ASSERT_TRACE( iLangListPref >= EPrimaryLanguages &&
                  iLangListPref <= ETertiaryLanguages );
    INFO_1( "Trying preferred language list %d", iLangListPref );

    iState = EQueryListSize;
    iAdaptation.PrepareSimLanguages(
        static_cast<TSsmLanguageListPriority>( iLangListPref++ ),
        iLangListSize, iStatus );
    SetActive();
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::FetchLanguageListL
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::FetchLanguageListL()
    {
    FUNC_LOG;
    INFO_1( "Required language list size = %d", iLangListSize() );

    iState = EQueryListContent;
    delete iLangList;
    iLangList = NULL;
    
    iLangList = iAdaptation.SimLanguagesArrayL( iLangListSize() );    
    
    TInt selectionResult = InterpretResult();

    if ( selectionResult > KErrNone )
        {
        StoreLanguageToCentRep( selectionResult );
        UseLanguage( selectionResult );
        }
    else
        {
        TryNextList();
        }
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::InterpretResult
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::InterpretResult()
    {
    FUNC_LOG;

    TInt selectedLanguage = KErrNotFound;
    TInt langCount( iLangListSize() );
    INFO_1( "Number of languages: %d", langCount );

    for ( TInt i = 0; i < langCount; i++ )
        {
        TInt lang = ( *iLangList )[i];
        INFO_1( "Language: %d", lang );

        TInt nearestMatch = ValidAlternative( lang );
        if ( nearestMatch > 0 ) // Not an error code
            {
            selectedLanguage = nearestMatch;
            break;
            }
        }

    INFO_1( "Selected language %d from list", selectedLanguage );
    return selectedLanguage;
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::ValidAlternative
//
// ---------------------------------------------------------------------------
//
TInt CSsmLangSelCmd::ValidAlternative( const TInt aLanguage ) const
    {
    FUNC_LOG;

    TInt retVal = KErrNotFound;

    if ( IsValid( aLanguage ) )
        {
        retVal = aLanguage;
        }
    else
        {
        for ( TInt i = 0; i < KSsmAlternativeLanguageTableSize; i++ )
            {
            if ( KAlternativeLanguageTable[i].iLang1 == aLanguage )
                {
                INFO_2( "Alternative %d found for language %d",
                    KAlternativeLanguageTable[i].iLang2, aLanguage );

                if ( IsValid( KAlternativeLanguageTable[i].iLang2 ) )
                    {
                    retVal = KAlternativeLanguageTable[i].iLang2;
                    break;
                    }
                }
            else if ( KAlternativeLanguageTable[i].iLang2 == aLanguage )
                {
                INFO_2( "Alternative %d found for language %d",
                    KAlternativeLanguageTable[i].iLang1, aLanguage );

                if ( IsValid( KAlternativeLanguageTable[i].iLang1 ) )
                    {
                    retVal = KAlternativeLanguageTable[i].iLang1;
                    break;
                    }
                }
            }
        }

    return retVal;
    }


// ---------------------------------------------------------------------------
// CSsmLangSelCmd::TryNextList
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::TryNextList()
    {
    FUNC_LOG;

    if ( iLangListPref <= ETertiaryLanguages )
        {
        PrepareNextList();
        }
    else // All lists exhausted
        {
        TInt prevLang = GetLanguageFromCentRep();
        if ( IsValid( prevLang ) )
            {
            UseLanguage( prevLang );
            }
        else
            {
            INFO( "Previous language not valid, reverting to default" );
            UseLanguage( iDefaultLanguage );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSsmLangSelCmd::StoreLanguageToCentRep
//
// ---------------------------------------------------------------------------
//
void CSsmLangSelCmd::StoreLanguageToCentRep( const TInt aLanguage )
    {
    FUNC_LOG;
    TUid mappedUid = iMapperUtility->CrUid( KCRUidSysLangUtil );
    CRepository* cenrep = NULL;
    TRAPD( errorCode, cenrep = CRepository::NewL( mappedUid ) );
    ERROR_1( errorCode, "Failed to initialize cen rep %d", errorCode );
    
    if ( KErrNone == errorCode )
        {
        errorCode = cenrep->Set( KSysLangUtilSimLanguage, aLanguage );
        ERROR_1( errorCode, "Failed to store language code to CentRep, %d", 
                                                                  errorCode );
        delete cenrep;
        }
    }
