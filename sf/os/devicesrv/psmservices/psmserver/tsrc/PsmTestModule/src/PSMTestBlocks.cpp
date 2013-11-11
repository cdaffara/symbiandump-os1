/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <StifItemParser.h>
#include <psmclient.h>
#include <psmsettings.h>
#include <psmtypes.h>

#include <psmsrvdomaincrkeys.h>
#include <centralrepository.h>
#include "psmclientimpl.h"
#include "PSMTest.h"


    // CONSTANT DEFINITIONS

    // For Settings API tests
    const TUint32 KCenRepUid2 = 0x10200EE4;
    const TUint32 KCenRepUid5 = 0x10200EE5;

    _LIT( KKey1FullPsm, "FullPowerSaveMode" );
    _LIT( KKey1PartialPsm, "PartialPowerSaveMode" );
    _LIT( KKey1NormalMode, "NormalMode" );

    const TInt KKey2FullPsm = 600;
    const TInt KKey2NormalMode = 450;
    
    const TInt KKey3FullPsm = 10;
    const TInt KKey3NormalMode = 50;

    const TInt KKey4FullPsm = 1500;
    const TInt KKey4PartialPsm = 2475;
    const TInt KKey4NormalMode = 5250;

    const TReal KKey5FullPsm = 2.75;
    const TReal KKey5PartialPsm = 2.435;
    const TReal KKey5NormalMode = 1.65;

    // For CenRep change checkings
    const TUint32 KCenRepUid = 0x10200EE3;
    TUid KCenRepUidValue = { KCenRepUid };

    _LIT8( KOriginalValue4, "ThirdFour" );

    enum TPsmPluginFirstKeys
        {
        EPsmPluginSecondKey1 = 1,
        EPsmPluginSecondKey2,
        EPsmPluginSecondKey3,
        EPsmPluginSecondKey4,
        EPsmPluginSecondKey5
        };




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPSMTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CPSMTest::Delete() 
    {
    }

// -----------------------------------------------------------------------------
// CPSMTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CPSMTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "InitTest", CPSMTest::InitializeTest ),
        ENTRY( "FullPsmChange", CPSMTest::FullPowerSaveModeChange ),
        ENTRY( "ChangePsmInit", CPSMTest::NotifyPowerSaveModeChangeInit ),
        ENTRY( "ChangePsmWaitComplete", CPSMTest::PsmWaitComplete ),
        ENTRY( "ChangePsmEnd", CPSMTest::NotifyPowerSaveModeChangeEnd ),
        ENTRY( "CancelPsm", CPSMTest::CancelPowerSaveModeChange ),

        ENTRY( "CheckPsmMode", CPSMTest::CheckPsmMode ),
        ENTRY( "GetCurrentSettings", CPSMTest::GetCurrentSettings ),
        ENTRY( "BackupSettings", CPSMTest::BackupSettings ),
        ENTRY( "PSMNotificationInit", CPSMTest::PsmNotificationInit ),
        ENTRY( "PSMNotificationCancel", CPSMTest::PsmNotificationCancel ),
        ENTRY( "PSMNotificationEnd", CPSMTest::PsmNotificationEnd ),

        ENTRY( "LoadUnload", CPSMTest::LoadAndUnload ),
        ENTRY( "CenRepChange", CPSMTest::CenRepChange ),
        ENTRY( "VerifyCenRepChange", CPSMTest::VerifyCenRepChangeL ),
        ENTRY( "ChangeSettingsInit", CPSMTest::ChangeSettingsInit ),
        ENTRY( "ChangeSettings", CPSMTest::ChangeSettings ),
        ENTRY( "ChangeSettingsEnd", CPSMTest::ChangeSettingsEnd ),
        ENTRY( "OpenClosePerformanceInit", CPSMTest::OpenClosePerformanceInit ),
        ENTRY( "OpenClosePerformanceRequest", CPSMTest::OpenClosePerformanceRequest ),
        ENTRY( "OpenClosePerformanceCancel", CPSMTest::OpenClosePerformanceCancel ),
        ENTRY( "OpenClosePerformanceChange", CPSMTest::OpenClosePerformanceChange ),
        ENTRY( "OpenClosePerformanceEnd", CPSMTest::OpenClosePerformanceEnd ),
        ENTRY( "ChangePsmPerformance", CPSMTest::ChangePsmPerformance ),

#ifdef _DEBUG 
        //OOM tests for psm server
        ENTRY( "OOMChangePsmInit", CPSMTest::OOMNotifyPowerSaveModeChangeInitL ),
        ENTRY( "OOMCancelPsm", CPSMTest::OOMCancelPowerSaveModeChangeL ),
        ENTRY( "OOMBackupAndGetSettings", CPSMTest::OOMPsmBackupAndGetSettingsL ),
        ENTRY( "OOMGetSettings", CPSMTest::OOMPsmGetSettingsL ),
        ENTRY( "OOMBackupSettings", CPSMTest::OOMPsmBackupSettingsL ),
#endif  //_DEBUG       
        ENTRY( "ChangeSettingsPerformance", CPSMTest::ChangeSettingsPerformance )    
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CPSMTest::InitializeTest
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::InitializeTest( CStifItemParser& aItem )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: InitializeTest") );

    FullPowerSaveModeChange( aItem );

    // reset error code
    iErrorCode = KErrNone;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPSMTest::LoadAndUnload
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::LoadAndUnload( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: LoadAndUnload") );

__UHEAP_MARK;
    // Create DLL and notify PSM change
    iPsmClient = CPsmClient::NewL( *this );

    delete iPsmClient;
__UHEAP_MARKEND;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPSMTest::CenRepChange
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::CenRepChange( CStifItemParser& aItem )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: CenRepChange") );
    
    CRepository* cenrep = CRepository::NewLC( KCenRepUidValue );

    TInt value1 = 0;
    TInt value2 = 0;
    TInt value3 = 0;
    TInt value4 = 0;
    TInt value5 = 0;

    cenrep->Get( EPsmPluginSecondKey1, value1 );
    cenrep->Get( EPsmPluginSecondKey2, value2 );
    cenrep->Get( EPsmPluginSecondKey3, value3 );
    cenrep->Get( EPsmPluginSecondKey4, value4 );
    cenrep->Get( EPsmPluginSecondKey5, value5 );

    CleanupStack::PopAndDestroy( cenrep );
    
    // Print old values from array
    RDebug::Print( _L( "PSMTest::CenRepChange(): oldValue info1: %d" ), value1 );
    RDebug::Print( _L( "PSMTest::CenRepChange(): oldValue info2: %d" ), value2 );
    RDebug::Print( _L( "PSMTest::CenRepChange(): oldValue info3: %d" ), value3 );
    RDebug::Print( _L( "PSMTest::CenRepChange(): oldValue info4: %d" ), value4 );
    RDebug::Print( _L( "PSMTest::CenRepChange(): oldValue info5: %d" ), value5 );

    NotifyPowerSaveModeChangeInit( aItem );
    PsmWaitComplete( aItem );

    return iErrorCode;
    }

// -----------------------------------------------------------------------------
// CPSMTest::VerifyCenRepChangeL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::VerifyCenRepChangeL( CStifItemParser& aItem )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: VerifyCenRepChangeL") );

    // Get mode 
    TInt mode( 0 );
    TInt err = aItem.GetNextInt( mode );

    CRepository* cenrep = CRepository::NewLC( KCenRepUidValue );

    TInt value1 = 0;
    TInt value2 = 0;
    TInt value3 = 0;
    TInt value4 = 0;
    TInt value5 = 0;

    cenrep->Get( EPsmPluginSecondKey1, value1 );
    cenrep->Get( EPsmPluginSecondKey2, value2 );
    cenrep->Get( EPsmPluginSecondKey3, value3 );
    cenrep->Get( EPsmPluginSecondKey4, value4 );
    cenrep->Get( EPsmPluginSecondKey5, value5 );

    CleanupStack::PopAndDestroy( cenrep );

    // Print new values from array
    RDebug::Print( _L( "PSMTest::CenRepChange(): newValue info1: %d" ), value1 );
    RDebug::Print( _L( "PSMTest::CenRepChange(): newValue info2: %d" ), value2 );
    RDebug::Print( _L( "PSMTest::CenRepChange(): newValue info3: %d" ), value3 );
    RDebug::Print( _L( "PSMTest::CenRepChange(): newValue info4: %d" ), value4 );
    RDebug::Print( _L( "PSMTest::CenRepChange(): newValue info5: %d" ), value5 );

    // Checks
    switch ( mode )
        {
        case EPsmsrvModeNormal:
            {
            if ( value1 == 5 && value2 == 6 &&
                 value3 == 7 && value4 == 8 &&
                 value5 == 9 )
                {
                err = KErrNone;
                }
            else
                {
                err = KErrArgument;
                }
            break;            
            }
        case EPsmsrvModePowerSave:
            {
            if ( value1 == 55 && value2 == 66 &&
                 value3 == 77 && value4 == 88 &&
                 value5 == 99 )
                {
                err = KErrNone;
                }
            else
                {
                err = KErrArgument;
                }
            break;            
            }
        case EPsmsrvPartialMode:
            {
            if ( value1 == 555 && value2 == 666 &&
                 value3 == 777 && value4 == 888 &&
                 value5 == 999 )
                {
                err = KErrNone;
                }
            else
                {
                err = KErrArgument;
                }
            break;            
            }
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CPSMTest::FullPowerSaveModeChange
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::FullPowerSaveModeChange( CStifItemParser& aItem )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: FullPowerSaveModeChange") );

    NotifyPowerSaveModeChangeInit( aItem );
    PsmWaitComplete( aItem );
    return NotifyPowerSaveModeChangeEnd( aItem );
    }

// -----------------------------------------------------------------------------
// CPSMTest::NotifyPowerSaveModeChangeInit
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::NotifyPowerSaveModeChangeInit( CStifItemParser& aItem )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: NotifyPowerSaveModeChangeInit") );

    TInt tomode( 0 );
    TInt err = aItem.GetNextInt( tomode );

    iErrorCode = KErrNone;

    // Store to-mode for later use
    iChangedMode = tomode;
    iPsmComplete = EFalse;

    iLog->Log( _L("PSMTest: NotifyPowerSaveModeChangeInit: parameter[%d] error: %d"), tomode, err );

    if ( !iPsmClient )
        {
        iPsmClient = CPsmClient::NewL( *this );
        }

    iPsmClient->ChangePowerSaveMode( tomode );

    return err;
    }

// -----------------------------------------------------------------------------
// CPSMTest::PsmWaitComplete
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::PsmWaitComplete( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: PsmWaitComplete") );

    if ( !iPsmComplete )
        {
        // Wait for PSM mode change complete
        iSchedulerWait.Start();
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPSMTest::NotifyPowerSaveModeChangeEnd
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::NotifyPowerSaveModeChangeEnd( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: NotifyPowerSaveModeChangeEnd") );

    delete iPsmClient;
    iPsmClient = NULL;

    return iErrorCode;
    }

// -----------------------------------------------------------------------------
// CPSMTest::CancelPowerSaveModeChange
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::CancelPowerSaveModeChange( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: CancelPowerSaveModeChange") );

    // Cancel PSM change
    iPsmClient->CancelPowerSaveModeChange();

    return iErrorCode;
    }

// -----------------------------------------------------------------------------
// CPSMTest::PsmNotification
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::PsmNotificationInit( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: PsmNotificationInit") );

    if ( !iPsmClient )
        {
        iPsmClient = CPsmClient::NewL( *this );
        }

    iPsmClient->RequestPowerSaveModeNotification();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPSMTest::PsmNotificationCancel
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::PsmNotificationCancel( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: PsmNotificationCancel") );

    TInt error( KErrNone );

    if ( !iPsmClient )
        {
        error = KErrNotFound;
        }
    else
        {
        iPsmClient->CancelPowerSaveModeNotificationRequest();
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CPSMTest::PsmNotification
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::PsmNotificationEnd( CStifItemParser& aItem )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: PsmNotificationEnd") );

    TInt currentmode( 0 );
    TInt err = aItem.GetNextInt( currentmode );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }

    if ( currentmode == iChangedMode )
        {
        // Test pass
        err = KErrNone;
        }
    else
        {
        err = KErrArgument;
        }

    delete iPsmClient;
    iPsmClient = NULL;

    return err;
    }

// -----------------------------------------------------------------------------
// CPSMTest::ChangeSettingsInit
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::ChangeSettingsInit( CStifItemParser& aItem )
    {
    // Print to log file
    iLog->Log( _L("PSMTest: ChangeSettingsInit") );

    TInt initmode( 0 );
    TInt err = aItem.GetNextInt( initmode );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }

    // Initialize config array with init mode. This is different than the mode we are 
    // changing to
    GenerateConfigArray( iPsmConfigArray, initmode, 0 );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPSMTest::GenerateConfigArray
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPSMTest::GenerateConfigArray( RConfigInfoArray& aPsmConfigArray, TInt aMode, TInt aIncrement )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: GenerateConfigArray") );

    // Initialize infos
    TPsmsrvConfigInfo info1;
    info1.iConfigId = 1 + aIncrement;
    info1.iConfigType = EConfigTypeBuf;
    
    TPsmsrvConfigInfo info2;
    info2.iConfigId = 2 + aIncrement;
    info2.iConfigType = EConfigTypeInt;

    TPsmsrvConfigInfo info3;
    info3.iConfigId = 3 + aIncrement;
    info3.iConfigType = EConfigTypeInt;

    TPsmsrvConfigInfo info4;
    info4.iConfigId = 4 + aIncrement;
    info4.iConfigType = EConfigTypeInt;

    TPsmsrvConfigInfo info5;
    info5.iConfigId = 5 + aIncrement;
    info5.iConfigType = EConfigTypeReal;

    switch ( aMode )
        {
        case EPsmsrvModeNormal:
            {
            info1.iDesValue.Append( KKey1NormalMode );
            info2.iIntValue = KKey2NormalMode;
            info3.iIntValue = KKey3NormalMode;
            info4.iIntValue = KKey4NormalMode;
            info5.iRealValue = KKey5NormalMode;
            break;
            }
        case EPsmsrvModePowerSave:
            {
            info1.iDesValue.Append( KKey1FullPsm );
            info2.iIntValue = KKey2FullPsm;
            info3.iIntValue = KKey3FullPsm;
            info4.iIntValue = KKey4FullPsm;
            info5.iRealValue = KKey5FullPsm;
            break;
            }
        case EPsmsrvPartialMode:
            {
            info1.iDesValue.Append( KKey1PartialPsm );
            info2.iIntValue = KKey2NormalMode;
            info3.iIntValue = KKey3NormalMode;
            info4.iIntValue = KKey4PartialPsm;
            info5.iRealValue = KKey5PartialPsm;
            break;
            }
        default:
            {
            iLog->Log( _L("PSMTest: GenerateConfigArray - Unknown mode") );
            }
        }

    aPsmConfigArray.AppendL( info1 );
    aPsmConfigArray.AppendL( info2 );
    aPsmConfigArray.AppendL( info3 );
    aPsmConfigArray.AppendL( info4 );
    aPsmConfigArray.AppendL( info5 );

    }

// -----------------------------------------------------------------------------
// CPSMTest::ChangeSettings
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::ChangeSettings( CStifItemParser& aItem )
    {
    // Print to log file
    iLog->Log( _L("PSMTest: ChangeSettings") );

    TBool useExistingApi( EFalse );
    TInt err = aItem.GetNextInt( useExistingApi );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }

    if ( !useExistingApi )
        {
        CPsmSettings* settings = CPsmSettings::NewLC();
        err = settings->BackupAndGetSettings( iPsmConfigArray, KCenRepUid2 );
        CleanupStack::PopAndDestroy( settings );
        }
    else
        {
        if ( iPsmClient )
            {
            iPsmClient->PsmSettings().BackupAndGetSettings( iPsmConfigArray, KCenRepUid2 );
            }
        else
            {
            err = KErrNotFound;
            }
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// CPSMTest::ChangeSettingsEnd
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::ChangeSettingsEnd( CStifItemParser& aItem )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: ChangeSettingsEnd") );

    TInt mode( 0 );
    TInt err = aItem.GetNextInt( mode );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }

    // First, generate array for changed mode settings
    RConfigInfoArray compareArray;
    GenerateConfigArray( compareArray, mode, 0 );

    // Compare iPsmConfigArray with changed mode settings
    for ( TInt i = 0; i < iPsmConfigArray.Count() && !err; i++ )
        {
        TBool found( EFalse );
        for ( TInt c = 0; c < compareArray.Count() && !found; c++ )
            {
            found = Compare( iPsmConfigArray[i], compareArray[c] );
            }

        if ( !found )
            {
            RDebug::Print( _L( " Compare failed with item: %i " ), i );
            err = KErrArgument;
            }
        }

    // Cleanup, no memory leaks
    compareArray.Close();
    iPsmConfigArray.Close();

    return err;
    }

// -----------------------------------------------------------------------------
// CPSMTest::Compare
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPSMTest::Compare( TPsmsrvConfigInfo aTarget, TPsmsrvConfigInfo aSource )
    {
    // Print to log file
    iLog->Log( _L("PSMTest: Compare") );
    TBool returnValue( EFalse );

    if ( aTarget.iConfigId == aSource.iConfigId &&
         aTarget.iConfigType == aSource.iConfigType )
        {
        iLog->Log( _L( "aTarget.iConfigId: %i, aSource.iConfigId: %i" ), aTarget.iConfigId, aSource.iConfigId );
        iLog->Log( _L( "aTarget.iConfigType: %i, aSource.iConfigType: %i" ), aTarget.iConfigType, aSource.iConfigType );
        switch ( aTarget.iConfigType )
            {
            case EConfigTypeInt:
                {
                iLog->Log( _L( "aTarget.iIntValue: %i, aSource.iIntValue: %i" ), aTarget.iIntValue, aSource.iIntValue );
                returnValue = aTarget.iIntValue == aSource.iIntValue;
                break;
                }
            case EConfigTypeReal:
                {
                iLog->Log( _L( "aTarget.iRealValue: %f, aSource.iRealValue: %f" ), aTarget.iRealValue, aSource.iRealValue );
                returnValue = aTarget.iRealValue == aSource.iRealValue;
                break;
                }
            case EConfigTypeBuf:
                {
                TBuf<256> targetBuf;
                targetBuf.Copy( aTarget.iDesValue );
                TBuf<256> sourceBuf;
                sourceBuf.Copy( aSource.iDesValue );
                iLog->Log( _L( "aTarget.iDesValue: %S, aSource.iDesValue: %S" ), &targetBuf, &sourceBuf );
                // Compare returns zero if both descroptors are the same
                if ( 0 == targetBuf.Compare( sourceBuf ) )
                    {
                    returnValue = ETrue;
                    }
                break;
                }
            default:
                {
                iLog->Log( ( _L( "PSMTest: Compare - Unknown type" ) ) );
                returnValue = EFalse;
                }
            }
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CPSMTest::OpenClosePerformanceInit
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::OpenClosePerformanceInit( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: OpenClosePerformanceInit") );

 /*   TInt amount( 0 );
    TInt err = aItem.GetNextInt( amount );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }

    // Open amount of client connections
    for ( TInt i = 0; i < amount; i++ )
        {
        CPSMTestObserver* observer = new ( ELeave ) CPSMTestObserver();
        CPsmNotifier* client = CPsmNotifier::NewL( *observer );
        iObserverArray.Append( observer );
        iNotifierArray.Append( client );
        }
*/
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CPSMTest::OpenClosePerformanceRequest
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::OpenClosePerformanceRequest( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: OpenClosePerformanceRequest") );

    // Open amount of client connections
  /*  for ( TInt i = 0; i < iNotifierArray.Count(); i++ )
        {
        iNotifierArray[ i ]->RequestPSMChangeNotification();
        }

    // Put member variable to NULL
    iPsmNotifier = NULL;
    */

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CPSMTest::OpenClosePerformanceChange
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::OpenClosePerformanceChange( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: OpenClosePerformanceChange") );

    // aItem has three parameters:
    //  - First indicates initialization mode, 
    //  - Second indicates use existing settings API and
    //  - Third indicates the mode to compare changed settings

    // Open amount of client connections
    /*for ( TInt i = 0; i < iNotifierArray.Count(); i++ )
        {
   //     iPsmNotifier = iNotifierArray[ i ];
        CStifItemParser* params = &aItem;
        ChangeSettingsInit( *params );
        ChangeSettings( *params );
        ChangeSettingsEnd( *params );
        }
    */
    // Put member variable to NULL
    // iPsmNotifier = NULL;

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CPSMTest::OpenClosePerformanceCancel
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::OpenClosePerformanceCancel( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: OpenClosePerformanceCancel") );

/*    if ( iNotifierArray.Count() != iObserverArray.Count() )
        {
        // Initialization error
        return KErrBadHandle;
        }

    // Open amount of client connections
    for ( TInt i = 0; i < iNotifierArray.Count(); i++ )
        {
        iPsmNotifier = iNotifierArray[ i ];
        iChangeObserver = iObserverArray[ i ];
        PsmNotificationCancel( aItem );
        }

    // Put member variable to NULL
    iPsmNotifier = NULL;
    iChangeObserver = NULL;
*/
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CPSMTest::OpenClosePerformanceEnd
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::OpenClosePerformanceEnd( CStifItemParser& /*aItem*/ )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: OpenClosePerformanceEnd") );

    /*for ( TInt i = 0; i < iNotifierArray.Count(); i++ )
        {
        delete iNotifierArray[i];
        iNotifierArray.Remove( i );
        i--;
        }

    for ( TInt i = 0; i < iObserverArray.Count(); i++ )
        {
        delete iObserverArray[i];
        iObserverArray.Remove( i );
        i--;
        }

    iNotifierArray.Close();
    iObserverArray.Close();
    */
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CPSMTest::CheckPsmMode
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::CheckPsmMode( CStifItemParser& aItem )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: CheckPsmMode") );


    TInt mode( 0 );
    TInt checkmode( 0 );

    TInt err = aItem.GetNextInt( checkmode );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }

    CPsmSettings* settings = CPsmSettings::NewLC();
    err = settings->GetCurrentMode( mode );
    CleanupStack::PopAndDestroy( settings );
    

   /* CRepository* cenrep = CRepository::NewLC( KCRUidPowerSaveMode );
    cenrep->Get( KPsmCurrentMode, mode );
    CleanupStack::PopAndDestroy( cenrep );
    */

    if ( KErrNone == err && mode != checkmode )
        {
        err = KErrArgument;
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CPSMTest::GetCurrentSettings
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::GetCurrentSettings( CStifItemParser& aItem )
    {
    // Print to log file
    iLog->Log( _L("PSMTest: GetCurrentSettings") );

    TInt initmode( 0 );
    TInt err = aItem.GetNextInt( initmode );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }

    // Initialize config array with init mode. This is different than the mode we are 
    // changing to
    RConfigInfoArray compareArray;
    GenerateConfigArray( compareArray, initmode, 0 );

    // Generate to normal mode settings
    RConfigInfoArray currentSettings;
    GenerateConfigArray( currentSettings, EPsmsrvModeNormal, 0 );

    CPsmSettings* settings = CPsmSettings::NewLC();
    err = settings->GetSettings( currentSettings, KCenRepUid2 );
    CleanupStack::PopAndDestroy( settings );

     // Compare arrays
    for ( TInt i = 0; i < compareArray.Count() && !err; i++ )
        {
        TBool found( EFalse );
        for ( TInt c = 0; c < currentSettings.Count() && !found; c++ )
            {
            found = Compare( currentSettings[i], compareArray[c] );
            }

        if ( !found )
            {
            err = KErrArgument;
            }
        }

    // Cleanup, no memory leaks
    compareArray.Close();
    currentSettings.Close();

    return err;
    }

// -----------------------------------------------------------------------------
// CPSMTest::BackupSettings
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::BackupSettings( CStifItemParser& aItem )
    {
    // Print to log file
    iLog->Log( _L("PSMTest: BackupSettings") );

    TInt initmode( 0 );
    TInt err = aItem.GetNextInt( initmode );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }

    // Initialize config array with init mode. This is different than the mode we are 
    // changing to
    RConfigInfoArray backupArray;
    GenerateConfigArray( backupArray, initmode, 5 );

    CPsmSettings* settings = CPsmSettings::NewLC();
    err = settings->BackupSettings( backupArray, KCenRepUid5 );
    CleanupStack::PopAndDestroy( settings );

    // Cleanup, no memory leaks
    backupArray.Close();

    return err;
    }

// -----------------------------------------------------------------------------
// CPSMTest::ChangePsmPerformance
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::ChangePsmPerformance( CStifItemParser& aItem )
    {

    // Print to log file
    iLog->Log( _L("PSMTest: ChangePsmPerformance") );

    // aItem has three parameters:
    //  - First one is the amount of actions to do
    //  - Second one is the first mode to change
    //  - Third one is the second mode to change
   
    TInt amount( 0 );
    TInt firstmode( 0 );
    TInt secondmode( 0 );

    TInt err = aItem.GetNextInt( amount );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }
    
    err = aItem.GetNextInt( firstmode );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }

    err = aItem.GetNextInt( secondmode );

    if ( KErrNone != err )
        {
        return KErrGeneral;
        }

    // Create local client, no need for memeber
    CPsmClient* client = CPsmClient::NewLC( *this );

    // Need to change mode in turns
    TBool changeFirstMode( ETrue );

    // Loop and change mode
    for ( TInt i = 0; i < amount && iErrorCode == KErrNone; i++ )
        {
        if ( changeFirstMode )
            {
            client->ChangePowerSaveMode( firstmode );
            changeFirstMode = EFalse;
            }
        else
            {
            client->ChangePowerSaveMode( secondmode );
            changeFirstMode = ETrue;
            }

        // Wait for mode to change
        iSchedulerWait.Start();

        // No need to do anything else, continue to next mode
        }

    CleanupStack::PopAndDestroy( client );

    return iErrorCode;
    }

// -----------------------------------------------------------------------------
// CPSMTest::ChangeSettingsPerformance
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPSMTest::ChangeSettingsPerformance( CStifItemParser& /*aItem*/ )
    {
    // Print to log file
    iLog->Log( _L("PSMTest: ChangeSettingsPerformance") );

    TInt increment = 0;
    TInt err = KErrNone;
    
    // Initialize config array with normal mode.
    for ( TInt i = 0; i < 10; i++ )
        {
        GenerateConfigArray( iPsmConfigArray, EPsmsrvModeNormal, increment );
        increment = iPsmConfigArray.Count();
        }

    // Change settings
    CPsmSettings* settings = CPsmSettings::NewLC();
    err = settings->BackupAndGetSettings( iPsmConfigArray, KCenRepUid5 );
    CleanupStack::PopAndDestroy( settings );

    // Generate array for changed mode settings
    increment = 0;
    RConfigInfoArray compareArray;
    for ( TInt k = 0; k < 10; k++ )
        {
        GenerateConfigArray( compareArray, EPsmsrvModePowerSave, increment );
        increment = compareArray.Count();
        }

    // Compare iPsmConfigArray with changed mode settings
    for ( TInt j = 0; j < iPsmConfigArray.Count() && !err; j++ )
        {
        TBool found( EFalse );
        for ( TInt c = 0; c < compareArray.Count() && !found; c++ )
            {
            found = Compare( iPsmConfigArray[j], compareArray[c] );
            }

        if ( !found )
            {
            err = KErrArgument;
            }
        }

    // Cleanup, no memory leaks
    compareArray.Close();
    iPsmConfigArray.Close();
    return err;
    }

// -----------------------------------------------------------------------------
// CPSMTest::PowerSaveModeChanged
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPSMTest::PowerSaveModeChanged( const TPsmsrvMode aMode )
    {
    // Print to log file
    iLog->Log( _L("PSMTest: PowerSaveModeChanged - mode: %d"), aMode );

    iErrorCode = KErrNone;

    iChangedMode = aMode;

    iPsmComplete = ETrue;

    // Stop scheduler wait
    if ( iSchedulerWait.IsStarted() )
        {
        iSchedulerWait.AsyncStop();    
        }
    }

// -----------------------------------------------------------------------------
// CPSMTest::PowerSaveModeChangeError
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPSMTest::PowerSaveModeChangeError( const TInt aError )
    {
    // Print to log file
    iLog->Log( _L("PSMTest: PowerSaveModeChangeError - Error: %d"), aError );

    iErrorCode = aError;

    // Stop scheduler wait
    if ( iSchedulerWait.IsStarted() )
        {
        iSchedulerWait.AsyncStop();    
        }
    }

/***************************************************************************
 * TestObserver functions
 ***************************************************************************/
// -----------------------------------------------------------------------------
// CPSMTest::PowerSaveModeChanged
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*void CPSMTestObserver::PowerSaveModeChanged( const TPsmsrvMode aMode )
    {
    // Print to log file
    RDebug::Print( _L("CPSMTestObserver: PowerSaveModeChanged - Mode: %d"), aMode );
    iNewMode = aMode;
    }

// -----------------------------------------------------------------------------
// CPSMTest::PowerSaveModeChangeError
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPSMTestObserver::PowerSaveModeChangeError( const TInt aError )
    {
    // Print to log file
    RDebug::Print( _L("CPSMTestObserver: PowerSaveModeChangeError - Error: %d"), aError );
    iNewMode = aError;
    }

// -----------------------------------------------------------------------------
// CPSMTest::CPSMTestObserver
// -----------------------------------------------------------------------------
//
CPSMTestObserver::CPSMTestObserver()
    {
    }

// -----------------------------------------------------------------------------
// CPSMTest::NotifyPowerSaveModeChange
// -----------------------------------------------------------------------------
//
CPSMTestObserver::~CPSMTestObserver()
    {
    }   
*/
#ifdef _DEBUG 
// -----------------------------------------------------------------------------
// CPSMTest::OOMNotifyPowerSaveModeChangeInit
// -----------------------------------------------------------------------------
//
TInt CPSMTest::OOMNotifyPowerSaveModeChangeInitL(CStifItemParser& /*aItem*/)
    {    
    CPsmClient* psmClient = CPsmClient::NewL( *this );
    CleanupStack::PushL(psmClient);
    CPsmClientImpl* psmImpl = static_cast<CPsmClientImpl*> (psmClient);
	psmImpl->HeapReset();
	psmImpl->HeapMark();
    for (TInt allocFailRate = 1;; allocFailRate++)
        {        
        psmImpl->SetHeapFailure( allocFailRate );                
        psmImpl->ChangePowerSaveMode(1);
        // Wait for mode to change
        iSchedulerWait.Start();
        
        if (iErrorCode == KErrNone)
            {
            iErrorCode = KErrNone;            
            break;            
            }     
        }
	psmImpl->HeapMarkEnd();
	psmImpl->HeapReset();   
    CleanupStack::PopAndDestroy(psmClient);
    return KErrNone;  
    }

// -----------------------------------------------------------------------------
// CPSMTest::OOMCancelPowerSaveModeChange
// -----------------------------------------------------------------------------
//
TInt CPSMTest::OOMCancelPowerSaveModeChangeL( CStifItemParser& /*aItem*/ )
    {
    CPsmClient* psmClient = CPsmClient::NewL( *this );
    CleanupStack::PushL(psmClient);
    CPsmClientImpl* psmImpl = static_cast<CPsmClientImpl*> (psmClient);
    psmImpl->ChangePowerSaveMode(0);
    for (TInt allocFailRate = 1;; allocFailRate++)
        {
        psmImpl->HeapReset();
        psmImpl->SetHeapFailure( allocFailRate );
        psmImpl->HeapMark();        
        psmImpl->CancelPowerSaveModeChange();        
        // Wait for mode to change
        iSchedulerWait.Start();
        psmImpl->HeapMarkEnd();
        if (iErrorCode == KErrNone )
            {
            iErrorCode = KErrNone;
            psmImpl->HeapReset();  
            break;            
            }       
        }
    CleanupStack::PopAndDestroy(psmClient); 
    return KErrNone;  
    }


// -----------------------------------------------------------------------------
// CPSMTest::OOMPsmBackupAndGetSettings
// -----------------------------------------------------------------------------
//
TInt CPSMTest::OOMPsmBackupAndGetSettingsL( CStifItemParser& /*aItem*/ )
    {
    CPsmClient* psmClient = CPsmClient::NewL( *this );
    CleanupStack::PushL(psmClient);
   
    CPsmClientImpl* psmImpl = static_cast<CPsmClientImpl*> (psmClient);
    psmImpl->HeapReset();
    psmImpl->HeapMark(); 
    for (TInt allocFailRate = 1;; allocFailRate++)
        {
        
        psmImpl->SetHeapFailure( allocFailRate );
              
        TInt err = psmImpl->PsmSettings().BackupAndGetSettings( iPsmConfigArray, KCenRepUid2 );
       
        if (err == KErrNone)
            {
            
            break;            
            }         
        }
    psmImpl->HeapMarkEnd();
    psmImpl->HeapReset();  
    CleanupStack::PopAndDestroy(psmClient);
    return KErrNone;  
    }

// -----------------------------------------------------------------------------
// CPSMTest::OOMPsmGetSettings
// -----------------------------------------------------------------------------
//
TInt CPSMTest::OOMPsmGetSettingsL( CStifItemParser& /*aItem*/ )
    {
    CPsmClient* psmClient = CPsmClient::NewL( *this );
    CleanupStack::PushL(psmClient);
    CPsmClientImpl* psmImpl = static_cast<CPsmClientImpl*> (psmClient);

    for (TInt allocFailRate = 1;; allocFailRate++)
        {
        psmImpl->HeapReset();
        psmImpl->SetHeapFailure( allocFailRate );
        psmImpl->HeapMark();       
        TInt err = psmImpl->PsmSettings().GetSettings( iPsmConfigArray, KCenRepUid2 );
        psmImpl->HeapMarkEnd();
        if (err == KErrNone)
            {
            psmImpl->HeapReset();  
            break;            
            }         
        }
    CleanupStack::PopAndDestroy(psmClient);
    return KErrNone;  
    }

// -----------------------------------------------------------------------------
// CPSMTest::OOMPsmBackupSettings
// -----------------------------------------------------------------------------
//
TInt CPSMTest::OOMPsmBackupSettingsL( CStifItemParser& /*aItem*/ )
    {
    CPsmClient* psmClient = CPsmClient::NewL( *this );
    CleanupStack::PushL(psmClient);
    CPsmClientImpl* psmImpl = static_cast<CPsmClientImpl*> (psmClient);
	//IGNORE the error of Powersave mode request. If Powersave mode is not enabled the Backupsettings will fail with KErrWrite.
    psmImpl->ChangePowerSaveMode(1);
    iSchedulerWait.Start();    
    RConfigInfoArray psmConfigArray;
    GenerateConfigArray( psmConfigArray, EPsmsrvModePowerSave, 0 ); 
    psmImpl->HeapReset();
    psmImpl->HeapMark();
    for (TInt allocFailRate = 1;; allocFailRate++)
        {       
        psmImpl->SetHeapFailure( allocFailRate );
        TInt err = psmImpl->PsmSettings().BackupSettings( psmConfigArray, KCenRepUid2 );
        iLog->Log( _L("PSMTest: BackupSettings- iPsmConfigArray.Count(): %d"), psmConfigArray.Count() );
        if (err == KErrNone)
            {            
            break;            
            }      
        }
    psmImpl->HeapMarkEnd();
    psmImpl->HeapReset(); 
    psmConfigArray.Close();   
    CleanupStack::PopAndDestroy(psmClient);
    return KErrNone; 
    }

#endif //_DEBUG     
//  End of File
