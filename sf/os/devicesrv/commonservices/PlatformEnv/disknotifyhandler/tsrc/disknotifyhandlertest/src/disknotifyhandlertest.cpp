/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CDisknotifyHandlerTest class implementation.
*
*/

#include <e32debug.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <bautils.h>
#include <e32property.h>
#include <coreapplicationuisdomainpskeys.h>


#include "disknotifyhandlertest.h"
#include "trace.h"


const TInt KFillBlockSize = 4096 * 100;
const TInt KWaitTearDown = 2000000;
const TInt KWaitMount = 2000000;
//const TInt KWaitTearDown = 0;
//const TInt KWaitMount = 0;

const TInt KMemoryCardDrive = EDriveE;
const TInt KPhoneMemoryDrive = EDriveC;
_LIT( KFSName, "Fat" );
_LIT( KTestFileName, "e:\\disknotifyhandlertest\\file.txt" );
_LIT( KTestFileNameN, "e:\\disknotifyhandlertest\\file%d.txt" );
_LIT( KTestFolderName, "e:\\disknotifyhandlertest\\" );

const TInt KForcedTimeOut( 500000 );


#define LEAVE( expression ) if( !( expression ) )\
                                {\
                                User::Leave( KErrGeneral );\
                                }\

//  INTERNAL INCLUDES
NONSHARABLE_CLASS( TWaitInfo )
    {
    public:

        CPeriodic* iPeriodic;
        CActiveSchedulerWait* iWait;
    };

/**
* Call back method when we need to stop active scheduler wait.
*/
LOCAL_C TInt WaitCallBack( TAny* aSelf )
    {
    if( aSelf )
        {
        TWaitInfo* info = static_cast<TWaitInfo*>( aSelf );
        if( info->iPeriodic )
            {
            info->iPeriodic->Cancel();
            }
        if( info->iWait )
            {
            if( info->iWait->IsStarted() )
                {
                info->iWait->AsyncStop();
                }
            }
        }

    return KErrNone;
    }

/**
* Helper method to wait current scheduler before teardown is completed.
*/
LOCAL_C void Wait( TInt aIntervalInMicorseconds )
    {
    TWaitInfo info;

    // Construct periodic
    CPeriodic* periodic = CPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( periodic );
    info.iPeriodic = periodic;

    // Construct active scheduler wait
    CActiveSchedulerWait* wait = new( ELeave ) CActiveSchedulerWait;
    CleanupStack::PushL( wait );
    info.iWait = wait;

    // Start timer and wait
    TCallBack cb( WaitCallBack, &info );
    periodic->Start( aIntervalInMicorseconds, aIntervalInMicorseconds, cb );
    wait->Start();

    // Cleanup
    CleanupStack::PopAndDestroy( wait );
    CleanupStack::PopAndDestroy( periodic );
    }


// ---------------------------------------------------------
// CDisknotifyHandlerTest::LibEntryL
// Returns: Poiter to CDisknotifyHandlerTest class
// ---------------------------------------------------------
EXPORT_C CDisknotifyHandlerTest* LibEntryL( CTestModuleIf& aTestModuleIf )
    {
    FUNC_LOG;
    
    CDisknotifyHandlerTest* libEntry( CDisknotifyHandlerTest::NewL( aTestModuleIf ) );
    return libEntry;
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::CDisknotifyHandlerTest
// ---------------------------------------------------------
CDisknotifyHandlerTest::CDisknotifyHandlerTest( CTestModuleIf& aTestModuleIf ) :
    CScriptBase( aTestModuleIf )
    {
    FUNC_LOG;    
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::ConstructL
// ---------------------------------------------------------
void CDisknotifyHandlerTest::ConstructL()
    {
    FUNC_LOG;    
    
    User::LeaveIfError( iFs.Connect() );
    iWait = new ( ELeave )CActiveSchedulerWait;

    TRAP_IGNORE( BaflUtils::EnsurePathExistsL( iFs, KTestFolderName ) );

    iDiskNotifyHandler = CDiskNotifyHandler::NewL( *this, iFs );
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::NewL
// ---------------------------------------------------------
CDisknotifyHandlerTest* CDisknotifyHandlerTest::NewL( CTestModuleIf& aTestModuleIf )
    {
    FUNC_LOG;
    
    CDisknotifyHandlerTest* self = new (ELeave) CDisknotifyHandlerTest( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::~CDisknotifyHandlerTest
// ---------------------------------------------------------
CDisknotifyHandlerTest::~CDisknotifyHandlerTest()
    {
    FUNC_LOG;    
    
    delete iWait;
    delete iDiskNotifyHandler;
    iFs.Close();

    delete iStubThreadDeathObserver;
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::RunMethodL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::RunMethodL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    
    const TStifFunctionInfo KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "NotifyDiskAddedL", CDisknotifyHandlerTest::NotifyDiskAddedL ),
        ENTRY( "NotifyDiskAddedWithCancelL", CDisknotifyHandlerTest::NotifyDiskAddedWithCancelL ),
        ENTRY( "NotifyDiskRemovedL", CDisknotifyHandlerTest::NotifyDiskRemovedL ),
        ENTRY( "NotifyDiskRemovedWithCancelL", CDisknotifyHandlerTest::NotifyDiskRemovedWithCancelL ),
        ENTRY( "NotifyDiskChangedL", CDisknotifyHandlerTest::NotifyDiskChangedL ),
        ENTRY( "NotifyDiskChangedWithCancelL", CDisknotifyHandlerTest::NotifyDiskChangedWithCancelL ),                      
        ENTRY( "NotifyDismountL", CDisknotifyHandlerTest::NotifyDismountL ),
        ENTRY( "NotifyDismountWithCancelL", CDisknotifyHandlerTest::NotifyDismountWithCancelL ),        
        ENTRY( "NotifyDismountErrorL", CDisknotifyHandlerTest::NotifyDismountErrorL ),
        ENTRY( "NotifyDiskSpaceOverL", CDisknotifyHandlerTest::NotifyDiskSpaceOverL ),
        ENTRY( "NotifyDiskSpaceOverWithCancelL", CDisknotifyHandlerTest::NotifyDiskSpaceOverWithCancelL ),
        ENTRY( "NotifyDiskSpaceUnderL", CDisknotifyHandlerTest::NotifyDiskSpaceUnderL ),
        ENTRY( "NotifyDiskSpaceErrorL", CDisknotifyHandlerTest::NotifyDiskSpaceErrorL ),
        ENTRY( "NotifyEntryAddedL", CDisknotifyHandlerTest::NotifyEntryAddedL ),
        ENTRY( "NotifyEntryAddedWithCancelL", CDisknotifyHandlerTest::NotifyEntryAddedWithCancelL ),
        ENTRY( "NotifyEntryRemovedL", CDisknotifyHandlerTest::NotifyEntryRemovedL ),
        ENTRY( "NotifyEntryChangedL", CDisknotifyHandlerTest::NotifyEntryChangedL ),
        ENTRY( "NotifyEntryErrorL", CDisknotifyHandlerTest::NotifyEntryErrorL ),
        ENTRY( "NotifyMultipleEventsL", CDisknotifyHandlerTest::NotifyMultipleEventsL ),
        ENTRY( "NotifyMultipleEventsWithCancelAllL", CDisknotifyHandlerTest::NotifyMultipleEventsWithCancelAllL ),
        ENTRY( "StartDismountL", CDisknotifyHandlerTest::StartDismountL ),
        ENTRY( "StartDismountWithCancelL", CDisknotifyHandlerTest::StartDismountWithCancelL ),
        ENTRY( "StartDismountForcedL", CDisknotifyHandlerTest::StartDismountForcedL ),
        ENTRY( "StartDismountForcedWithCancelL", CDisknotifyHandlerTest::StartDismountForcedWithCancelL )        
        };
    const TInt count( sizeof( KFunctions ) / sizeof( TStifFunctionInfo ) );
    TInt ret( RunInternalL( KFunctions, count, aItem ) );
    return ret;
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::HandleNotifyDisk
// ---------------------------------------------------------

void CDisknotifyHandlerTest::HandleNotifyDisk( TInt aError, 
                                               const TDiskEvent& aEvent )
    {
    FUNC_LOG;
    
    iNotifyDiskError = aError;
    iNotifyDiskEvent = aEvent;

    TBool stop( EFalse );

    if ( iNotifyDiskEvent.iType == EDiskError )
        {
        stop = ETrue;
        }
    else if ( iThreadFunction == EAddDrive &&
        iNotifyDiskEvent.iType == EDiskAdded )
        {
        stop = ETrue;
        }
    else if ( iThreadFunction == ERemoveDrive &&
        iNotifyDiskEvent.iType == EDiskRemoved )
        {
        stop = ETrue;
        iDriveRemovedHandled = ETrue;
        }
    else if ( iThreadFunction == EChangeDriveState &&
        iNotifyDiskEvent.iType == EDiskStatusChanged )
        {
        stop = ETrue;
        }

    if ( stop && iWait->IsStarted() )
        {
        if ( iNotifyDiskCancel )
            {
            iDiskNotifyHandler->CancelNotifyDisk();
            }
        if ( iNotifyCancelAll )
            {
            delete iDiskNotifyHandler;
            iDiskNotifyHandler = NULL;
            }
        
        CheckAndStopScheduler ();        
        iCallbackCompleted = ETrue;
        }
    // For better API test coverage
    MDiskNotifyHandlerCallback::HandleNotifyDisk( aError, aEvent );
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::HandleNotifyDismount
// ---------------------------------------------------------

void CDisknotifyHandlerTest::HandleNotifyDismount( TInt aError, 
                                                  const TDismountEvent& aEvent )
    {
    FUNC_LOG;
    
    iNotifyDismountError = aError;
    iNotifyDismountEvent = aEvent;

    if ( iStartForcedDismount )
        {
        return;
        }
    
    
    iDiskNotifyHandler->AllowDismount( aEvent.iDrive );

    if ( iNotifyDismountCancel )
        {
        iDiskNotifyHandler->CancelNotifyDismount();
        }
    
    CheckAndStopScheduler ();

    // For better API test coverage
    MDiskNotifyHandlerCallback::HandleNotifyDismount( aError, aEvent );
    
    iCallbackCompleted = ETrue;
    }

void CDisknotifyHandlerTest::CheckAndStopScheduler ()
    {
    if ( iWait->IsStarted() )
        {
        if ( iStubThreadDeathObserver )
            {
            if ( iStubThreadDeathObserver->Completed () )
                {
                iWait->AsyncStop();
                }
            }
        else
            {
            iWait->AsyncStop();
            }        
        }
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::HandleNotifyDiskSpace
// ---------------------------------------------------------

void CDisknotifyHandlerTest::HandleNotifyDiskSpace( TInt aError, 
                                                 const TDiskSpaceEvent& aEvent )
    {
    FUNC_LOG;
    
    iNotifyDiskSpaceError = aError;
    iNotifyDiskSpaceEvent = aEvent;

    if ( iNotifyDiskSpaceCancel )
        {
        iDiskNotifyHandler->CancelNotifyDiskSpace();
        }

    CheckAndStopScheduler ();
    
    // For better API test coverage
    MDiskNotifyHandlerCallback::HandleNotifyDiskSpace( aError, aEvent );
    
    iCallbackCompleted = ETrue;
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::HandleNotifyEntry
// ---------------------------------------------------------

void CDisknotifyHandlerTest::HandleNotifyEntry( TInt aError, 
                                                    const TEntryEvent& aEvent )
    {
    FUNC_LOG;
    
    iNotifyEntryError = aError;
    iNotifyEntryEvent = aEvent;

    if ( iNotifyEntryCancel )
        {
        iDiskNotifyHandler->CancelNotifyEntry();
        }

    CheckAndStopScheduler ();
    
    // For better API test coverage
    MDiskNotifyHandlerCallback::HandleNotifyEntry( aError, aEvent );
    
    iCallbackCompleted = ETrue;
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::HandleNotifyDismountFinished
// ---------------------------------------------------------


void CDisknotifyHandlerTest::HandleNotifyDismountFinished(
        TInt aError,
        const TDismountFinishedEvent& aEvent )
    {
    FUNC_LOG;
    
    INFO_1( "HandleNotifyDismountFinished aError %d", aError );
    
    iStartDismountError = aError;
    iNotifyDismountFinishedEvent = aEvent;
    
    CheckAndStopScheduler ();
    
    // For better API test coverage
    MDiskNotifyHandlerCallback::HandleNotifyDismountFinished( aError, aEvent );
    
    iCallbackCompleted = ETrue;
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDiskAddedL
// ---------------------------------------------------------

TInt CDisknotifyHandlerTest::NotifyDiskAddedL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ( void )aItem;
    
    iNotifyDiskError = KErrGeneral;
    
    TInt ret( iDiskNotifyHandler->NotifyDisk() );
    User::LeaveIfError( ret );
    ret = iDiskNotifyHandler->NotifyDisk();
    
    if ( KErrAlreadyExists != ret )
        {
        User::LeaveIfError( ret );
        }

    StartThreadFunctionL( EAddDrive );
    iWait->Start();
    
    User::LeaveIfError( iNotifyDiskError );
    
    if ( iNotifyDiskEvent.iType != EDiskAdded )
        {
        User::LeaveIfError( KErrGeneral );
        }
    
    return KErrNone;

    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDiskAddedWithCancelL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDiskAddedWithCancelL( CStifItemParser& aItem )
    {
    FUNC_LOG;
        
    ResetCancelReqs();
    iNotifyDiskCancel = ETrue;
    TRAPD( err, NotifyDiskAddedL( aItem ) );
    ResetCancelReqs();
    User::LeaveIfError( err );
    NotifyDiskAddedL( aItem );
    
    return KErrNone;

    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDiskRemovedL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDiskRemovedL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ( void )aItem;
    
    iDriveRemovedHandled = EFalse;
    
    // Ensure mount before dismount
    iFs.MountFileSystem( KFSName, KMemoryCardDrive );
    User::After( KWaitMount );

    iNotifyDiskError = KErrGeneral;
    TInt ret( iDiskNotifyHandler->NotifyDisk() );
    User::LeaveIfError( ret );
    ret = iDiskNotifyHandler->NotifyDisk();
    
    if ( KErrAlreadyExists != ret )
        {
        User::LeaveIfError( ret );
        }

    StartThreadFunctionL( ERemoveDrive );
    iWait->Start();

    User::LeaveIfError( iNotifyDiskError );
    if ( iNotifyDiskEvent.iType != EDiskRemoved && !iDriveRemovedHandled )
        {
        User::LeaveIfError( KErrGeneral );
        }
    return KErrNone;
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDiskRemovedWithCancelL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDiskRemovedWithCancelL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ResetCancelReqs();
    iNotifyDiskCancel = ETrue;
    TRAPD( err,NotifyDiskRemovedL( aItem ) );
    ResetCancelReqs();
    User::LeaveIfError( err );
    NotifyDiskRemovedL( aItem );
    return KErrNone;
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDiskChangedL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDiskChangedL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    iNotifyDiskError = KErrGeneral;
        
    LEAVE( iDiskNotifyHandler->NotifyDisk() == KErrNone );
    LEAVE( iDiskNotifyHandler->NotifyDisk( ) == KErrAlreadyExists )

    StartThreadFunctionL( EChangeDriveState );
    
    User::After( KWaitMount ); // Drive locking does not work, just test setup of change notification
    if ( iNotifyDiskCancel )
        {
        iDiskNotifyHandler->CancelNotifyDisk();
        }
/*
    iWait->Start();

    LEAVE(
        iNotifyDiskError == KErrNone &&
        iNotifyDiskEvent.iType == EDiskStatusChanged );
*/  
    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDiskChangedWithCancelL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDiskChangedWithCancelL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ResetCancelReqs();
    iNotifyDiskCancel = ETrue;
    TRAPD( err, NotifyDiskChangedL( aItem ) );
    ResetCancelReqs();
    User::LeaveIfError( err );
    NotifyDiskChangedL( aItem );

    return KErrNone;
    
    }    

// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDismountL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDismountL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ( void )aItem;
    // Ensure mount before dismount
    iFs.MountFileSystem( KFSName, KMemoryCardDrive );
    User::After( KWaitMount );

    iNotifyDismountError = KErrGeneral;
    
    TInt ret( iDiskNotifyHandler->NotifyDismount( KMemoryCardDrive ) );
    User::LeaveIfError( ret );
    ret = iDiskNotifyHandler->NotifyDismount( KMemoryCardDrive );
    
    if ( KErrAlreadyExists != ret )
        {
        User::LeaveIfError( ret );
        }


    StartThreadFunctionL( EDismountDrive );
    iWait->Start();

    LEAVE(
        iNotifyDismountError == KErrNone &&
        iNotifyDismountEvent.iDrive == KMemoryCardDrive );
    return KErrNone;

    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDismountWithCancelL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDismountWithCancelL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ResetCancelReqs();
    iNotifyDismountCancel = ETrue;
    TRAPD( err, NotifyDismountL( aItem ) );
    ResetCancelReqs();
    User::LeaveIfError( err );
    NotifyDismountL( aItem );
    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDismountErrorL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDismountErrorL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ( void )aItem;
    
    iNotifyDismountError = KErrNone;
    User::LeaveIfError( iDiskNotifyHandler->NotifyDismount( EDriveZ + 1 ) );
    iWait->Start();

    if ( iNotifyDismountError == KErrNone )
        {
        User::LeaveIfError( KErrGeneral );
        }
    
    return KErrNone;
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDiskSpaceOverL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDiskSpaceOverL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    
    ( void )aItem;
    
    // Ensure that file to fill is deleted
    iFs.Delete( KTestFileName );

    iNotifyDiskSpaceError = KErrGeneral;
    TVolumeInfo volInfo;
    User::LeaveIfError( iFs.Volume( volInfo, KMemoryCardDrive ) );

    TInt64 threshold = volInfo.iFree - KFillBlockSize / 2;

    TInt err( iDiskNotifyHandler->NotifyDiskSpace( threshold, 
                                                   KMemoryCardDrive ) );
    User::LeaveIfError( err );
    err = iDiskNotifyHandler->NotifyDiskSpace( threshold, KMemoryCardDrive );
    if ( KErrAlreadyExists != err )
        {
        User::LeaveIfError( KErrGeneral );
        }

    StartThreadFunctionL( EFillDriveSpace );
    iWait->Start();

    User::LeaveIfError( iFs.Volume( volInfo, KMemoryCardDrive ) );

    LEAVE( iNotifyDiskSpaceError == KErrNone &&
        iNotifyDiskSpaceEvent.iDrive == KMemoryCardDrive &&
        iNotifyDiskSpaceEvent.iThreshold == threshold );

    User::LeaveIfError( iFs.Volume( volInfo, KMemoryCardDrive ) );
    LEAVE( volInfo.iFree < threshold );
    
    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDiskSpaceOverWithCancelL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDiskSpaceOverWithCancelL( 
                                                        CStifItemParser& aItem )
    {
    FUNC_LOG;
    ResetCancelReqs();
    iNotifyDiskSpaceCancel = ETrue;
    TRAPD( err, NotifyDiskSpaceOverL( aItem ) );
    ResetCancelReqs();
    User::LeaveIfError( err );
    NotifyDiskSpaceOverL( aItem );

    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDiskSpaceUnderL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDiskSpaceUnderL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ( void )aItem;
    // Ensure that file to free exists
    HBufC8* buf = HBufC8::NewLC( KFillBlockSize );
    RFile file;
    User::LeaveIfError( file.Replace( iFs, KTestFileName, EFileShareReadersOrWriters ) );
    CleanupClosePushL( file );
    TPtr8 ptr( buf->Des() );
    ptr.FillZ( ptr.MaxSize() );
    User::LeaveIfError( file.Write( ptr ) );
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( buf );

    iNotifyDiskSpaceError = KErrGeneral;
    TVolumeInfo volInfo;
    User::LeaveIfError( iFs.Volume( volInfo, KMemoryCardDrive ) );

    TInt64 threshold = volInfo.iFree + KFillBlockSize / 2;

    LEAVE(
        iDiskNotifyHandler->NotifyDiskSpace( threshold, KMemoryCardDrive ) == KErrNone );
    LEAVE(
        iDiskNotifyHandler->NotifyDiskSpace( threshold, KMemoryCardDrive ) == KErrAlreadyExists );

    StartThreadFunctionL( ECleanupDriveSpace );
    iWait->Start();

    LEAVE( iNotifyDiskSpaceError == KErrNone &&
        iNotifyDiskSpaceEvent.iDrive == KMemoryCardDrive &&
        iNotifyDiskSpaceEvent.iThreshold == threshold );

    User::LeaveIfError( iFs.Volume( volInfo, KMemoryCardDrive ) );
    LEAVE( volInfo.iFree > threshold );    
    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyDiskSpaceErrorL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyDiskSpaceErrorL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    
    ( void )aItem;
    iNotifyDismountError = KErrNone;

    LEAVE(
        iDiskNotifyHandler->NotifyDiskSpace( 0, EDriveZ + 1 ) == KErrNone );

    iWait->Start();

    LEAVE( iNotifyDiskSpaceError != KErrNone );

    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyEntryAddedL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyEntryAddedL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    
    ( void )aItem;
    // Ensure that entry is removed
    iFs.Delete( KTestFileName );

    iNotifyEntryError = KErrGeneral;
    LEAVE(
        iDiskNotifyHandler->NotifyEntry( ENotifyAll, KTestFileName ) == KErrNone );
    LEAVE(
        iDiskNotifyHandler->NotifyEntry( ENotifyAll, KTestFileName ) == KErrAlreadyExists );
    StartThreadFunctionL( EAddEntry );
    iWait->Start();

    LEAVE( iNotifyEntryError == KErrNone &&
        BaflUtils::FileExists( iFs, KTestFileName ) );
    
    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyEntryAddedWithCancelL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyEntryAddedWithCancelL( 
                                                        CStifItemParser& aItem )
    {
    FUNC_LOG;
    ResetCancelReqs();
    iNotifyEntryCancel = ETrue;
    TRAPD( err, NotifyEntryAddedL( aItem ) );
    ResetCancelReqs();
    User::LeaveIfError( err );
    NotifyEntryAddedL( aItem );
    
    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyEntryRemovedL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyEntryRemovedL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    
    ( void )aItem;
    // Create entry to be removed
    RFile file;
    User::LeaveIfError( file.Replace( iFs, KTestFileName, EFileShareReadersOrWriters ) );
    file.Close();

    iNotifyEntryError = KErrGeneral;

    LEAVE(
        iDiskNotifyHandler->NotifyEntry( ENotifyEntry, KTestFileName ) == KErrNone );
    LEAVE(
        iDiskNotifyHandler->NotifyEntry( ENotifyEntry, KTestFileName ) == KErrAlreadyExists );

    StartThreadFunctionL( ERemoveEntry );
    iWait->Start();

    LEAVE( iNotifyEntryError == KErrNone &&
        !BaflUtils::FileExists( iFs, KTestFileName ) );
    
    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyEntryChangedL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyEntryChangedL( CStifItemParser& aItem )
    {
    FUNC_LOG;
        
    ( void )aItem;
    iNotifyEntryError = KErrGeneral;

    LEAVE(
        iDiskNotifyHandler->NotifyEntry( ENotifyAll, KTestFolderName ) == KErrNone );
    LEAVE(
        iDiskNotifyHandler->NotifyEntry( ENotifyAll, KTestFolderName ) == KErrAlreadyExists );

    StartThreadFunctionL( EChangeEntry );
    iWait->Start();

    LEAVE( iNotifyEntryError == KErrNone );
    
    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyEntryErrorL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyEntryErrorL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    
    ( void )aItem;
    iNotifyEntryError = KErrNone;
    LEAVE(
        iDiskNotifyHandler->NotifyEntry( ENotifyDir, KTestFileName ) == KErrNone );
    iWait->Start();

    LEAVE( iNotifyEntryError != KErrNone );
    
    return KErrNone;
    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyMultipleEventsL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyMultipleEventsL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    
    ( void )aItem;
    iNotifyDiskError = KErrGeneral;
    iNotifyDismountError = KErrNone;
    iNotifyDiskSpaceError = KErrNone;
    iNotifyEntryError = KErrNone;

    LEAVE( iDiskNotifyHandler->NotifyDisk() == KErrNone );
    LEAVE( iDiskNotifyHandler->NotifyDisk() == KErrAlreadyExists );

    LEAVE( iDiskNotifyHandler->NotifyDismount( KPhoneMemoryDrive ) == KErrNone );
    LEAVE( iDiskNotifyHandler->NotifyDismount( KPhoneMemoryDrive ) == KErrAlreadyExists );

    LEAVE( iDiskNotifyHandler->NotifyDismount( KMemoryCardDrive ) == KErrNone );
    LEAVE( iDiskNotifyHandler->NotifyDismount( KMemoryCardDrive ) == KErrAlreadyExists );

    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 1, KMemoryCardDrive ) == KErrNone );
    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 1, KMemoryCardDrive ) == KErrAlreadyExists );

    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 10000, KMemoryCardDrive ) == KErrNone );
    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 10000, KMemoryCardDrive ) == KErrAlreadyExists );

    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 1000000, KMemoryCardDrive ) == KErrNone );
    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 1000000, KMemoryCardDrive ) == KErrAlreadyExists );

    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 1, KPhoneMemoryDrive ) == KErrNone );
    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 1, KPhoneMemoryDrive ) == KErrAlreadyExists );

    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 10000, KPhoneMemoryDrive ) == KErrNone );
    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 10000, KPhoneMemoryDrive ) == KErrAlreadyExists );

    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 1000000, KPhoneMemoryDrive ) == KErrNone );
    LEAVE( iDiskNotifyHandler->NotifyDiskSpace( 1000000, KPhoneMemoryDrive ) == KErrAlreadyExists );

    LEAVE( iDiskNotifyHandler->NotifyEntry( ENotifyDir, KTestFolderName ) == KErrNone );

    TFileName file;
    for (TInt i( 0 ); i < 10; i++ )
        {
        file.Format( KTestFileNameN, i );

        LEAVE( iDiskNotifyHandler->NotifyEntry( ENotifyAll, file ) == KErrNone );
        LEAVE( iDiskNotifyHandler->NotifyEntry( ENotifyEntry, file ) == KErrNone );
        LEAVE( iDiskNotifyHandler->NotifyEntry( ENotifyFile, file ) == KErrNone );
        LEAVE( iDiskNotifyHandler->NotifyEntry( ENotifyAttributes, file ) == KErrNone );
        LEAVE( iDiskNotifyHandler->NotifyEntry( ENotifyWrite, file ) == KErrNone );
        LEAVE( iDiskNotifyHandler->NotifyEntry( ENotifyDisk, file ) == KErrNone );
        }
/*
    StartThreadFunctionL( ERemoveDrive );
    iWait->Start();
    LEAVE(
        iNotifyDiskError == KErrNone &&
        iNotifyDiskEvent.iType == EDiskRemoved );

    if ( iDiskNotifyHandler )
    */
    if ( iNotifyCancelAll )
        {
        iDiskNotifyHandler->CancelNotifyDisk();
        iDiskNotifyHandler->CancelNotifyDismount( KPhoneMemoryDrive );
        iDiskNotifyHandler->CancelNotifyDismount();
    
        iDiskNotifyHandler->CancelNotifyDiskSpace( 1, KPhoneMemoryDrive );
        iDiskNotifyHandler->CancelNotifyDiskSpace( KPhoneMemoryDrive );
        iDiskNotifyHandler->CancelNotifyDiskSpace();
    
        iDiskNotifyHandler->CancelNotifyEntry( ENotifyDir, KTestFolderName );
        file.Format( KTestFileNameN, 0 );
        iDiskNotifyHandler->CancelNotifyEntry( file );
        iDiskNotifyHandler->CancelNotifyEntry();
        }    
    return KErrNone;
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::NotifyMultipleEventsWithCancelAllL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::NotifyMultipleEventsWithCancelAllL( 
                                                        CStifItemParser& aItem )
    {    
    FUNC_LOG;
    
    ResetCancelReqs();
    iNotifyCancelAll = ETrue;
    TRAPD( err, NotifyMultipleEventsL( aItem ) );
    
    ERROR( err, "NotifyMultipleEventsL leaved" );
    ResetCancelReqs();
    User::LeaveIfError( err );
    
    return KErrNone;
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::StartDismountL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::StartDismountL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ( void )aItem;

    
    // Ensure mount before dismount
    iFs.MountFileSystem( KFSName, KMemoryCardDrive );
    User::After( KWaitMount );

    iStartDismountError = KErrGeneral;

    
    LEAVE( KErrNone == iDiskNotifyHandler->StartDismount( KMemoryCardDrive,
                                                          KForcedTimeOut ) );
    
    LEAVE( KErrAlreadyExists == iDiskNotifyHandler->StartDismount( 
                                                             KMemoryCardDrive,
                                                             KForcedTimeOut ) );
    
    iWait->Start();
    
    LEAVE( iStartDismountError == KErrNone && 
           iNotifyDismountFinishedEvent.iDrive == KMemoryCardDrive &&
           !iNotifyDismountFinishedEvent.iForcedDismount );
    
    return KErrNone;    
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::StartDismountWithCancelL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::StartDismountWithCancelL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ( void )aItem;
    
    // First dismount
    TRequestStatus stat;
    iFs.NotifyDismount( KMemoryCardDrive, stat, EFsDismountForceDismount );
    User::WaitForRequest( stat );
    User::After( KWaitMount );

    // Ensure mount before dismount
    TInt ret = iFs.MountFileSystem( KFSName, KMemoryCardDrive );
    User::After( KWaitMount );

    iStartDismountError = KErrGeneral;

    ret = iFs.MountFileSystem( KFSName, KMemoryCardDrive );
    
    LEAVE( KErrNone != ret );    
    
    LEAVE( KErrNone == iDiskNotifyHandler->StartDismount( KMemoryCardDrive,
                                                          KForcedTimeOut ) );

    // Cancel
    iDiskNotifyHandler->CancelStartedDismount( KMemoryCardDrive );
    
    // So the mount should succeed
    ret = iFs.MountFileSystem( KFSName, KMemoryCardDrive );
    User::After( KWaitMount );
    //LEAVE ( KErrNone == ret );
    
    return KErrNone;    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::StartDismountForcedL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::StartDismountForcedL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    ( void )aItem;
    // Ensure FileSystem Mounted
    iFs.MountFileSystem( KFSName, KMemoryCardDrive );
    User::After( KWaitMount );  
       
    iStartForcedDismount = ETrue;
    iDiskNotifyHandler->NotifyDismount( KMemoryCardDrive );
    
    LEAVE( KErrNone == iDiskNotifyHandler->StartDismount( KMemoryCardDrive,
                                                          KForcedTimeOut ) );

    iWait->Start();
    
    LEAVE( iStartDismountError == KErrNone && 
           iNotifyDismountFinishedEvent.iDrive == KMemoryCardDrive &&
           iNotifyDismountFinishedEvent.iForcedDismount );

    return KErrNone;    
    }
// ---------------------------------------------------------
// CDisknotifyHandlerTest::StartDismountForcedWithCancelL
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::StartDismountForcedWithCancelL( CStifItemParser& aItem )
    {
    FUNC_LOG;
    
    ( void )aItem;
    
    iStartForcedDismount = ETrue;
    
    iDiskNotifyHandler->NotifyDismount( KMemoryCardDrive );
    
    LEAVE( KErrNone == iDiskNotifyHandler->StartDismount( KMemoryCardDrive,
                                                          KForcedTimeOut ) );

    iWait->Start();
    
    iDiskNotifyHandler->CancelStartedDismount( KMemoryCardDrive );
      
    //Ensure FileSystem Mounted
    TInt ret = iFs.MountFileSystem( KFSName, KMemoryCardDrive );
    User::After( KWaitMount );
    
    return KErrNone;    
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::StartThreadFunctionL
// ---------------------------------------------------------
void CDisknotifyHandlerTest::StartThreadFunctionL( TInt aThreadFunction )
    {
    FUNC_LOG;
    
    delete iStubThreadDeathObserver;
    iStubThreadDeathObserver = NULL;
    iCallbackCompleted = EFalse;
    
    RThread thread;
    User::LeaveIfError( thread.Create(
        KNullDesC, ThreadFunction, KDefaultStackSize, NULL, this ) );
    iThreadFunction = aThreadFunction;
    TThreadId threadId = thread.Id ();
    thread.SetPriority( EPriorityNormal );
    thread.Resume();
    thread.Close();
    
    iStubThreadDeathObserver = CThreadDeathObserver::NewL ( threadId, *this );
    iStubThreadDeathObserver->ObserveDeath ();
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::ThreadFunction
// ---------------------------------------------------------
TInt CDisknotifyHandlerTest::ThreadFunction( TAny* aPtr )
    {
    FUNC_LOG;
    
    CDisknotifyHandlerTest* self =
        static_cast<CDisknotifyHandlerTest*>( aPtr );
    RFs fs;
    TInt ret ( fs.Connect() );
    if ( ret != KErrNone )
        {
        return ret;
        }
    switch( self->iThreadFunction )
        {
        case EAddDrive:
            {
            // Prevent SysAp shutting down applications
            RProperty::Set(
                KPSUidCoreApplicationUIs,
                KCoreAppUIsMmcRemovedWithoutEject,
                ECoreAppUIsEjectCommandUsed );

            // Ensure dismount before dismount
            TRequestStatus stat;
            fs.NotifyDismount( KMemoryCardDrive, stat, EFsDismountForceDismount );
            User::WaitForRequest( stat );
            User::After( KWaitMount );
            ret = fs.MountFileSystem( KFSName, KMemoryCardDrive );
            break;
            }
        case ERemoveDrive:
            {
            // Prevent SysAp shutting down applications
            RProperty::Set(
                KPSUidCoreApplicationUIs,
                KCoreAppUIsMmcRemovedWithoutEject,
                ECoreAppUIsEjectCommandUsed );

            TRequestStatus stat;
            fs.NotifyDismount( KMemoryCardDrive, stat, EFsDismountForceDismount );
            User::WaitForRequest( stat );
            User::After( KWaitMount );
            fs.MountFileSystem( KFSName, KMemoryCardDrive );
            break;
            }
        case EChangeDriveState:
            {
            _LIT( KPsw, "A");
            TMediaPassword psw;
            psw.FillZ( psw.MaxLength( ) );
            psw.Copy( (TUint8*)KPsw().Ptr(), KPsw().Size() );

            TDriveInfo info;
            ret = fs.Drive( info, KMemoryCardDrive );
            if ( info.iMediaAtt & KMediaAttHasPassword )
                {
                fs.UnlockDrive( KMemoryCardDrive, psw, ETrue );
                fs.ClearPassword( KMemoryCardDrive, psw );
  //              User::After( KWaitMount );
                fs.ErasePassword( KMemoryCardDrive );
//                User::After( KWaitMount );
                }
            fs.LockDrive( KMemoryCardDrive, psw, psw, ETrue );
//            User::After( KWaitMount );
            fs.ClearPassword( KMemoryCardDrive, psw );
            break;
            }
        case EDismountDrive:
            {
            // Prevent SysAp shutting down applications
            RProperty::Set(
                KPSUidCoreApplicationUIs,
                KCoreAppUIsMmcRemovedWithoutEject,
                ECoreAppUIsEjectCommandUsed );

            TRequestStatus stat;
            fs.NotifyDismount( KMemoryCardDrive, stat, EFsDismountNotifyClients );
            User::WaitForRequest( stat );
            ret = stat.Int();
            User::After( KWaitMount );
            fs.MountFileSystem( KFSName, KMemoryCardDrive );
            break;
            }
        case EChangeEntry: // FALL THROUGH
        case EFillDriveSpace:
            {
            HBufC8* buf = HBufC8::New( KFillBlockSize );
            if ( buf )
                {
                RFile file;
                ret = file.Replace( fs, KTestFileName, EFileShareReadersOrWriters );
                if ( ret == KErrNone )
                    {
                    TPtr8 ptr( buf->Des() );
                    ptr.FillZ( ptr.MaxSize() );
                    ret = file.Write( ptr );
                    file.Close();
                    }
                delete buf;
                }
            else
                {
                ret = KErrNoMemory;
                }
            break;
            }
        case ECleanupDriveSpace: // FALL THROUGH
        case ERemoveEntry:
            {
            ret = fs.Delete( KTestFileName );
            break;
            }
        case EAddEntry:
            {
            RFile file;
            ret = file.Replace( fs, KTestFileName, EFileShareReadersOrWriters );
            file.Close();
            break;
            }
/*        case EStartForcedDismount:
            {
            TInt ret = fs.MountFileSystem( KFSName, KMemoryCardDrive );
            // Create a file, but don't close, will result to forced dismount.
            RFile file;
            ret = file.Replace( fs, KTestFileName, EFileShareExclusive );
            User::After( KWaitMount );
            file.Close();
            break;
            }
*/            
        default:
            {
            break;
            }
        }
    fs.Close();
    return ret;
    }

// ---------------------------------------------------------
// CDisknotifyHandlerTest::ResetCancelReqs
// ---------------------------------------------------------
void CDisknotifyHandlerTest::ResetCancelReqs()
    {
    iNotifyDiskCancel = EFalse;
    iNotifyDismountCancel = EFalse;
    iNotifyDiskSpaceCancel = EFalse;
    iNotifyCancelAll = EFalse;
    }

// From MThreadDeathNotification
void CDisknotifyHandlerTest::DeathNotification ( TExitType /*aExitType*/, TInt /*aExitReason*/, TExitCategoryName /*aExitCategory*/ )
    {
    if ( iCallbackCompleted && iWait->IsStarted () )
        {
        iWait->AsyncStop ();
        }
    }

// ---------------------------------------------------------
// Thread Death Observer
// ---------------------------------------------------------
CThreadDeathObserver* CThreadDeathObserver::NewL ( TThreadId aThreadId, MThreadDeathNotification& aNotificationObserver )
    {
    CThreadDeathObserver* self = new ( ELeave ) CThreadDeathObserver ( aNotificationObserver );
    CleanupStack::PushL ( self );
    self->ConstructL ( aThreadId );
    CleanupStack::Pop ();
    return self;
    }
    
CThreadDeathObserver::~CThreadDeathObserver ()
    {
    Cancel ();
    iThreadToObserve.Close ();
    }
    
void CThreadDeathObserver::ObserveDeath ()
    {
    if ( !IsActive () )
        {        
        iThreadToObserve.Logon ( iStatus );
        SetActive ();
        }
    }
    
CThreadDeathObserver::CThreadDeathObserver ( MThreadDeathNotification& aNotificationObserver )
    : CActive ( EPriorityStandard ),
    iObserver ( aNotificationObserver ) 
    {
    CActiveScheduler::Add ( this );
    }
    
void CThreadDeathObserver::ConstructL ( TThreadId aThreadId )
    {
    User::LeaveIfError ( iThreadToObserve.Open ( aThreadId ) );
    }
    
void CThreadDeathObserver::RunL ()
    {
    User::LeaveIfError ( iStatus.Int () );
    iObserver.DeathNotification ( iThreadToObserve.ExitType (), iThreadToObserve.ExitReason (), iThreadToObserve.ExitCategory () );
    }
    
TInt CThreadDeathObserver::RunError ( TInt aError )
    {
    return KErrNone;
    }
    
void CThreadDeathObserver::DoCancel ()
    {
    iThreadToObserve.LogonCancel ( iStatus );
    }
    
