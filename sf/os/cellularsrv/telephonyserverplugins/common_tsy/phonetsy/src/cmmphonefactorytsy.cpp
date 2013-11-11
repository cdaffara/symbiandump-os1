// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// INCLUDE FILES
//



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonefactorytsyTraces.h"
#endif
#include <featmgr/featurecontrol.h>
#include <featureuids.h>

#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/ctsysatmessagingbase.h>

#include "cmmmessagemanagerbase.h"
#include "cmmphonefactorytsy.h"
#include "cmmphonetsy.h"
#include "MmTsy_conf.h"

_LIT(KLtsyIniFile, "ltsydata.ini");

#ifdef USING_CTSY_DISPATCHER
_LIT(KDefaultLicenseeTsyDllName, "ctsydispatcher.dll");
const TInt KDefaultLicenseeTsyUID3 =  0x10285C38;
#else
_LIT(KDefaultLicenseeTsyDllName, "licenseetsy.dll");
const TInt KDefaultLicenseeTsyUID3 = 0x2000BEE4;
#endif //USING_CTSY_DISPATCHER

typedef MLtsyFactoryBase*(*TFactoryBaseNewL)();

// ======== MEMBER FUNCTIONS ========

CMmPhoneFactoryTsy::CMmPhoneFactoryTsy()
    {
    // Version numbers defined in ETEL.h
    iVersion = TVersion( KEtelMajorVersionNumber,
        KEtelMinorVersionNumber,
        KEtelBuildVersionNumber );
    }

CMmPhoneFactoryTsy* CMmPhoneFactoryTsy::NewL()
    {
    CMmPhoneFactoryTsy* phoneFactoryTsy = NULL;
    phoneFactoryTsy = new ( ELeave ) CMmPhoneFactoryTsy();
    CleanupStack::PushL( phoneFactoryTsy );
    phoneFactoryTsy->ConstructL();   
    CleanupStack::Pop();
    return phoneFactoryTsy;
    }    

void CMmPhoneFactoryTsy::ConstructL()
    {
    }

CMmPhoneFactoryTsy::~CMmPhoneFactoryTsy()
    {
    iLoadedLib.Close();
    }

// ---------------------------------------------------------------------------
// CMmPhoneFactoryTsy::NewPhoneL
// Creates a new phone object instance
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CPhoneBase* CMmPhoneFactoryTsy::NewPhoneL( const TDesC& aName )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_NEWPHONEL_1, "TSY: CMmPhoneFactoryTsy::NewL - NEW LOG");

    iPhoneTsy = NULL;

    // In case of debug build, print flag info.
#ifdef TF_LOGGING_ENABLED
    
    RFeatureControl featureControl;
    TInt err = featureControl.Open();
    if (err != KErrNone)
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_NEWPHONEL_2, "CMmPhoneFactoryTsy::NewPhoneL - failed to connect to FeatMgr");
        }
    else if (featureControl.FeatureSupported(NFeature::KCsVideoTelephony) == KFeatureSupported) 
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_NEWPHONEL_3, "TSY: __CS_VIDEO_TELEPHONY -flag is on");
        }
    else if (featureControl.FeatureSupported(NFeature::KEmergencyCallsEnabledInOfflineMode) == KFeatureSupported)
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_NEWPHONEL_4, "TSY: __COMMON_TSY__EMERGENCY_CALLS_ENABLED_IN_OFFLINE_MODE -flag is on");
        }

    featureControl.Close();
#endif
    
    // Check if the Phone Name is OK
    if ( KErrNone == aName.CompareF( KMmTsyPhoneName ) )
        {
        iMessageRouter = NULL;
                
        // Dynamic loading of LTSY
        iLtsyFactory = LoadLibraryL();
        }

    return iPhoneTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneFactoryTsy::EnumeratePhones
// Returns the maximum number of supported phone instances
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneFactoryTsy::EnumeratePhones()
    {
    return KMmTsyNumOfSupportedPhones;
    }

// ---------------------------------------------------------------------------
// CMmPhoneFactoryTsy::GetPhoneInfo
// Index of the requested phone is passed to the client
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneFactoryTsy::GetPhoneInfo(
    const TInt aIndex,
    RTelServer::TPhoneInfo& aInfo )
    {
    TInt ret( KErrNotFound );
    
    if( aIndex == 0 )
        {
        aInfo.iName.Copy( TPtrC( KMmTsyPhoneName ) );
        aInfo.iNetworkType = KMmTsyNetworkType;
        aInfo.iNumberOfLines = KMmTsyNumOfSupportedLines;
        //(TAN) Symbian's comment to iExtension parameter:
        //The best solution is not to worry about this iExtensions field too
        //much and to just return one API extension value, and that being the
        //main API supported by the TSY. If the TSY also supports e.g. GPRS
        //and/or SAT then the client can find this out using the
        //RTelServer::IsSupportedByModule method.
        aInfo.iExtensions = KETelExtMultimodeV1;

        // Phone found. Change ret value.
        ret = KErrNone;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneFactoryTsy::IsSupported
// Provides information about the supported functional units of
// the TSY. This method directs the request to all supported protocol
// extensions. If functional unit is supported by any of those protocol
// extension the result will be ETrue. Otherwise the method will 
// return KErrNone
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPhoneFactoryTsy::IsSupported( const TInt aFunctionalUnit )
    {
    TBool ret = EFalse;

    switch ( aFunctionalUnit )
        {
        //Supported functional units of mmtsy
        case KETelFuncMobileIccAccess:
        case KETelFuncMobileNetwork:
        case KETelFuncMobileIdentity:
        case KETelFuncMobilePower:
        case KETelFuncMobileSignal:
        case KETelFuncMobileDTMF:
        case KETelFuncMobileUserNetworkAccess:
        case KETelFuncMobileIdentityService:
        case KETelFuncMobileCallForwarding:
        case KETelFuncMobileCallBarring:
        case KETelFuncMobileCallWaiting:
        case KETelFuncMobileCallCompletion:
        case KETelFuncMobileSecurity:
        case KETelFuncMobileAlternateLineService:
        case KETelFuncMobileFixedDiallingNumbers:
        case KETelFuncMobileDataCall:
        case KETelFuncMobileEmergencyCall:
        case KETelFuncMobileSmsMessaging:
        case KETelFuncMobileBroadcastMessaging:
        case KETelFuncMobileUssdMessaging:
        case KETelFuncMobileConferenceCall:
        case KETelFuncMobilePhonebook:
        case KETelFuncMobileSmsStore:
        case KETelFuncMobileOwnNumberStore:
        case KETelFuncMobileEmergencyNumberStore:
        case KETelFuncMobilePrivacy:
        case KETelExtMultimodeV1:
        case KETelExtMultimodeV2:
        case KETelExtMultimodeV3:
        case KEtelFuncMobileAPNControlList:
        case KEtelExtMultimodeV9:
        case KEtelFuncCellInfo:
            ret = ETrue;
            break;

        //Unsupported functional units
        //case KETelFuncMobileAlternatingCall:
        //case KETelFuncMobileMessageWaiting:
        //case KETelFuncMobileNamStore:
        //case KETelFuncMobileMultimediaCall:
        //case KEtelFuncMobileMultimediaCallSettings:
        //case KETelFuncMobileCost:

        default:
            ret = EFalse;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneFactoryTsy::NewSimAtk
// Creates CTsySatMessaging object and returns it
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTsySatMessagingBase* CMmPhoneFactoryTsy::NewSimAtk( const TDesC& aName )
    {
    CTsySatMessagingBase* satMessaging = NULL;
    
    if ( KErrNone == aName.CompareF( KSatToolkit ) )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_NEWSIMATK_1, "TSY: CMmPhoneFactoryTsy::NewSimAtk - Starting to open SimAtkTSY");
       
        if (iLtsyFactory == NULL)
            {
            TRAP_IGNORE(iLtsyFactory = LoadLibraryL());
            }
        
        if (iLtsyFactory != NULL)
            {
            // Get SIM ATK TSY messaging service from LicenseeTsy
            MLtsyFactoryV1* ptr_v1 = static_cast<MLtsyFactoryV1*>( iLtsyFactory );
     
#ifdef USING_CTSY_DISPATCHER        
            satMessaging = ptr_v1->GetSatMessagingService(iPhoneTsy->SmsService());
#else
            satMessaging = ptr_v1->GetSatMessagingService( );
#endif
            }        
        }

    return satMessaging;
    }

// ---------------------------------------------------------------------------
// extern "C"
// The LibEntry function is implemented in another programming
// language (ASM?). extern "C" is required to port from other language
// ---------------------------------------------------------------------------
//
extern "C"
    {
    IMPORT_C CPhoneFactoryBase* LibEntry(); // Force "Proper Name" export
    }

// ---------------------------------------------------------------------------
// LibEntry
// A Windows Entry Procedure. The LibEntry() procedure in the
// DLL C run-time libraries performs all the necessary initialization
// required by the WEPs (Windows Entry Point) they contain. This code is
// copied from Symbian's example
// Returns: CPhoneFactoryBase*: Pointer to CMmPhoneFactoryTsy class
// ---------------------------------------------------------------------------
//
EXPORT_C CPhoneFactoryBase* LibEntry()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LIBENTRY_1, "TSY: CPhoneFactoryBase::LibEntry()...");
    // return NULL if instantiation fails
    CMmPhoneFactoryTsy* factory(NULL);
    TRAP_IGNORE(factory = CMmPhoneFactoryTsy::NewL()); 
    return factory; 
    }

#ifdef USING_CTSY_DISPATCHER
TBool CMmPhoneFactoryTsy::UsingCtsyDispatcher()
/**
 * Function to determine whether the setup is using the CTSY Dispatcher layer.
 */
    {
    return ETrue; 
    }
#endif //USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmPhoneFactoryTsy::LoadLibraryL
// Dynamically loads a LTSY.
// ---------------------------------------------------------------------------
//
MLtsyFactoryBase* CMmPhoneFactoryTsy::LoadLibraryL()
    {
    TPtrC ltsyDllName(0, NULL);
    TUint ltsyDllUid = 0;
    
#ifdef USING_CTSY_DISPATCHER 
    ltsyDllName.Set(KDefaultLicenseeTsyDllName);
    ltsyDllUid = KDefaultLicenseeTsyUID3; 
#else
    RBuf configData;
    CleanupClosePushL(configData);
    
    TRAPD(ret,
          {
          ReadConfigFileL(&configData);
          ltsyDllName.Set(ReadDllNameFromConfigL(configData));
          ltsyDllUid = ReadDllUidFromConfigL(configData);
          });
    
    if (ret != KErrNone)
        {
        ltsyDllName.Set(KDefaultLicenseeTsyDllName);
        ltsyDllUid = KDefaultLicenseeTsyUID3;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_ERROR, CMMPHONEFACTORYTSY_LOADLIBRARYL_1A, "TSY: Failed to load LTSY dll details from ini (error: %d), using defaults", ret);
        }
#endif
    
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_LOADLIBRARYL_1, "TSY: CMmPhoneFactoryTsy::LoadLibraryL - Loading Dll=%S, UID3=0x%8X", ltsyDllName, ltsyDllUid);
    User::LeaveIfError(iLoadedLib.Load(ltsyDllName));
    
#ifndef USING_CTSY_DISPATCHER
    CleanupStack::PopAndDestroy();  // configData
#endif
    
    // Check the Uid3 is as expected
    if (iLoadedLib.Type()[2] != TUid::Uid(ltsyDllUid))
        {
        User::Leave(KErrBadLibraryEntryPoint);
        }
    
    TFactoryBaseNewL libEntry = reinterpret_cast<TFactoryBaseNewL>(iLoadedLib.Lookup(1));   
    MLtsyFactoryBase* factory = (*libEntry)();    // libEntry may leave.
    User::LeaveIfNull(factory);
    
    InitCtsyL(factory);
    
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_LOADLIBRARYL_2, "TSY: Loaded LTSY");
    return factory;
    }

void CMmPhoneFactoryTsy::InitCtsyL(MLtsyFactoryBase* aLtsyFactory)
/**
 * Initialises iPhoneTsy
 */
    {    
    if (iPhoneTsy != NULL)
        {
        return;
        }
    
    // Create Message Manager (the 'core' of the LTSY Plug-in API)
    CMmMessageManagerBase* messageManager = CMmMessageManagerBase::NewL();
    User::LeaveIfNull(messageManager);
    CleanupStack::PushL( messageManager );

    // get TSY message manager callback object
    MmMessageManagerCallback* callBack = 
        messageManager->GetMessageManagerCallback();

    // Get message router from LTSY. All current interface versions
    // have the same GetMessageRouter defined in version 1.
    MLtsyFactoryV1* ptr_v1 = 
         static_cast<MLtsyFactoryV1*>( aLtsyFactory );
             
    iMessageRouter = ptr_v1->GetMessageRouter( *callBack );
    User::LeaveIfNull( iMessageRouter );
    
    // set the pointer to the message router object
    messageManager->SetMessageRouter( iMessageRouter );

    // Ownership of messageManager to be passed to iPhoneTsy.
    CleanupStack::Pop( messageManager );
    
    // LTSY Plug-in API successfully created
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_INITLTSYL_1, "TSY: CMmPhoneFactoryTsy::NewL - Starting to open CommonTSY");
    // Create Phone Tsy (which creates the whole Common TSY)
    iPhoneTsy = CMmPhoneTsy::NewL(messageManager, this, aLtsyFactory);

    if (iPhoneTsy != NULL)
        {
        // Licencee Tsy successfully created
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_NORMAL, CMMPHONEFACTORYTSY_INITLTSYL_2, "TSY: CMmPhoneFactoryTsy::NewL -  CommonTSY successfully opened");
        messageManager->SetPhoneTsy( iPhoneTsy );
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_WARNING, CMMPHONEFACTORYTSY_INITLTSYL_3, "TSY: CMmPhoneFactoryTsy::NewL - CommonTSY not opened");
        }
    }

TPtrC CMmPhoneFactoryTsy::ReadDllNameFromConfigL(const TDesC& aConfigData)
    {
    _LIT(KLicenseeTsyDllKeyName, "LicenseeTsyDllName");
    TPtrC dllName = GetValueForKeyL(aConfigData, KLicenseeTsyDllKeyName);
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_READDLLNAMEFROMCONFIGL_1, "TSY: Config file: LTSY DLL name = %S", dllName);
    return dllName;
    }

TUint CMmPhoneFactoryTsy::ReadDllUidFromConfigL(const TDesC& aConfigData)
    {
    _LIT(KLicenseeTsyUid3, "LicenseeTsyUid3");

    const TDesC& rawUid = GetValueForKeyL(aConfigData, KLicenseeTsyUid3);
    // only take the right 8 characters (ie discard the "0x")
    ASSERT(rawUid.Length() >= 8);
    TLex lex(rawUid.Right(8));   
    TUint uid;
    User::LeaveIfError(lex.Val(uid, EHex));
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_READUIDFROMCONFIGL_1, "TSY: Config file: LTSY UID3 = 0x%x", uid);
    return uid;
    }

/*static*/
void CMmPhoneFactoryTsy::ReadConfigFileL(RBuf* aConfigData)
/**
 * Reads config file from system drive or, if not present, from ROM
 *
 * @param aConfigData This buffer gets created and has the ini file contents loaded into it
 */
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_READCONFIGFILE_1, "TSY: Trying to load from %d (C) drive", fs.GetSystemDrive());
    fs.SetSessionToPrivate(fs.GetSystemDrive());

    RFile configFile;
    TInt ret = configFile.Open(fs, KLtsyIniFile, EFileShareExclusive);
    if (ret == KErrNotFound)
        {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_READCONFIGFILE_2, "TSY: Trying to load ini from %d (Z) drive, as read for system drive returned error %d", EDriveZ, ret);
        User::LeaveIfError(fs.SetSessionToPrivate(EDriveZ));
        User::LeaveIfError(configFile.Open(fs, KLtsyIniFile, EFileShareExclusive));
        }

OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEFACTORYTSY_READCONFIGFILE_3, "TSY: Reading ini file %S", KLtsyIniFile);
    
    CleanupClosePushL(configFile);

    TInt configFileSize;
    User::LeaveIfError(configFile.Size(configFileSize));

    RBuf8 fileBuf;
    User::LeaveIfError(fileBuf.Create(configFileSize));
    CleanupClosePushL(fileBuf);
    User::LeaveIfError(configFile.Read(fileBuf));

    aConfigData->CreateL(configFileSize);
    aConfigData->Copy(fileBuf);

    CleanupStack::PopAndDestroy(3, &fs); // fs, configFile, fileBuf
    }

/*static*/
TPtrC CMmPhoneFactoryTsy::GetValueForKeyL(const TDesC& aKeysValues, const TDesC& aKey)
/**
 * Gets the value for a specified key. The expected format is
 * <pre>
 * key1 value1
 * key2 value2
 * </pre>
 * 
 * However the parsing is flexible enough to allow "key1 =value1" as it allows an equals separator and extra whitespace
 * The value cannot include whitespace, even if quoted.
 */
    {
    // this matches the bracket expression in this regexp: \w+\s*=?\s*(\w+)[\s$]
    TInt startOfKey = aKeysValues.Find(aKey);
    TLex valueExtractor(aKeysValues);
    valueExtractor.Inc(startOfKey + aKey.Length());
    valueExtractor.SkipSpaceAndMark();
    // allow equals separating key and value
    if (valueExtractor.Peek() == '=')
        {
        valueExtractor.Inc();
        valueExtractor.SkipSpaceAndMark();
        }
    
    TPtrC value = valueExtractor.NextToken();
    __ASSERT_ALWAYS(value.Length() > 0, User::Leave(KErrCorrupt));
    return value;
    }
