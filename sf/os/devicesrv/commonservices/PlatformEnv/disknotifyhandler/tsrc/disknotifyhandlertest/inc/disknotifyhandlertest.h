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
* Cdisknotifyhandlertest class declaration.
*
*/


#ifndef DISKNOTIFYHANDLERTEST_H
#define DISKNOTIFYHANDLERTEST_H

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

// SYSTEM INCLUDES
#include <testscripterinternal.h>
#include <disknotifyhandler.h>

#include <e32std.h> 
// FORWARD DECLARATIONS
class MThreadDeathNotification;
class CThreadDeathObserver;

// FORWARD DECLARATIONS
class TCaseInfo;
class CDisknotifyHandlerTest;


class MThreadDeathNotification
    {
public:
    virtual void DeathNotification ( TExitType aExitType, TInt aExitReason, TExitCategoryName aExitCategory ) = 0;
    };


// DESCRIPTION
// This a Test Module interface template
// that does not really do anything.
typedef TInt ( CDisknotifyHandlerTest::*TestFunction )( TTestResult& );

NONSHARABLE_CLASS( CDisknotifyHandlerTest ) : public CScriptBase,
                                              public MDiskNotifyHandlerCallback,
                                              public MThreadDeathNotification
    {
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CDisknotifyHandlerTest* NewL( CTestModuleIf& aTestModuleIf );

    /**
    * Destructor.
    */
    virtual ~CDisknotifyHandlerTest();

public: // Functions from base classes

    TInt RunMethodL( CStifItemParser& aItem );
    
public:     // From observer interface
    void HandleNotifyDisk(
        TInt aError, const TDiskEvent& aEvent );

    void HandleNotifyDismount(
        TInt aError, const TDismountEvent& aEvent );

    void HandleNotifyDiskSpace(
        TInt aError, const TDiskSpaceEvent& aEvent );

    void HandleNotifyEntry(
        TInt aError, const TEntryEvent& aEvent );
    
    void HandleNotifyDismountFinished(
            TInt aError,
            const TDismountFinishedEvent& aEvent );

    // From MThreadDeathNotification
    void DeathNotification ( TExitType aExitType, TInt aExitReason, TExitCategoryName aExitCategory ); 
    
protected: // New functions

    TInt NotifyDiskAddedL( CStifItemParser& aItem );
    TInt NotifyDiskAddedWithCancelL( CStifItemParser& aItem );
    TInt NotifyDiskRemovedL( CStifItemParser& aItem );
    TInt NotifyDiskRemovedWithCancelL( CStifItemParser& aItem );
    TInt NotifyDiskChangedL( CStifItemParser& aItem );
    TInt NotifyDiskChangedWithCancelL( CStifItemParser& aItem );
    TInt NotifyDismountL( CStifItemParser& aItem );
    TInt NotifyDismountWithCancelL( CStifItemParser& aItem );
    TInt NotifyDismountErrorL( CStifItemParser& aItem );
    TInt NotifyDiskSpaceOverL( CStifItemParser& aItem );
    TInt NotifyDiskSpaceOverWithCancelL( CStifItemParser& aItem );
    TInt NotifyDiskSpaceUnderL( CStifItemParser& aItem );
    TInt NotifyDiskSpaceErrorL( CStifItemParser& aItem );
    TInt NotifyEntryAddedL( CStifItemParser& aItem );
    TInt NotifyEntryAddedWithCancelL( CStifItemParser& aItem );
    TInt NotifyEntryRemovedL( CStifItemParser& aItem );
    TInt NotifyEntryChangedL( CStifItemParser& aItem );
    TInt NotifyEntryErrorL( CStifItemParser& aItem );
    TInt NotifyMultipleEventsL( CStifItemParser& aItem );
    TInt NotifyMultipleEventsWithCancelAllL( CStifItemParser& aItem );
    TInt StartDismountL( CStifItemParser& aItem );
    TInt StartDismountWithCancelL( CStifItemParser& aItem );
    TInt StartDismountForcedL( CStifItemParser& aItem );
    TInt StartDismountForcedWithCancelL( CStifItemParser& aItem );

private:

    /**
    * C++ default constructor.
    */
    CDisknotifyHandlerTest( CTestModuleIf& aTestModuleIf );

    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();

    /**
    * Function returning test case name and pointer to test case function
    */
    const TCaseInfo Case( const TInt aCaseNumber ) const;
    
    // Thread functions
    enum TFunction
       {
       EAddDrive = 0,
       ERemoveDrive,
       EChangeDriveState,
       EDismountDrive,
       EFillDriveSpace,
       ECleanupDriveSpace,
       EAddEntry,
       ERemoveEntry,
       EChangeEntry,
       EWaitForever,
       EStartForcedDismount
       };
    
    void StartThreadFunctionL( TInt aThreadFunction );

    static TInt ThreadFunction( TAny* aPtr );
    
    void ResetCancelReqs();

    void CheckAndStopScheduler ();
    
private: // Data

    TestFunction                iMethod;
    
    TInt                        iThreadFunction;
    RFs                         iFs;
    CActiveSchedulerWait*       iWait;
    CDiskNotifyHandler*         iDiskNotifyHandler;

    TBool                       iNotifyDiskCancel;
    TInt                        iNotifyDiskError;
    TBool                       iNotifyDismountCancel;
    TInt                        iNotifyDismountError;
    TBool                       iNotifyDiskSpaceCancel;
    TInt                        iNotifyDiskSpaceError;
    TBool                       iNotifyEntryCancel;
    TInt                        iNotifyEntryError;
    TBool                       iNotifyCancelAll;

    TInt                        iStartDismountError;
    TBool                       iStartDismountCancel;
    TBool                       iStartForcedDismount;

    
    MDiskNotifyHandlerCallback::TDismountEvent              iNotifyDismountEvent;
    MDiskNotifyHandlerCallback::TDiskSpaceEvent             iNotifyDiskSpaceEvent;
    MDiskNotifyHandlerCallback::TEntryEvent                 iNotifyEntryEvent;
    MDiskNotifyHandlerCallback::TDiskEvent                  iNotifyDiskEvent;
    MDiskNotifyHandlerCallback::TDismountFinishedEvent      iNotifyDismountFinishedEvent;
    
    CThreadDeathObserver*       iStubThreadDeathObserver;
    TBool                       iCallbackCompleted;
    TBool                       iDriveRemovedHandled;
    };

// Function pointer related internal definitions

// Hack around known GCC bug.
#ifndef __GCC32__
    #define GETPTR
#else
    #define GETPTR &
#endif


// An internal structure containing a test case name and
// the pointer to function doing the test
class TCaseInfoInternal
    {
    public:
        const TText* iCaseName;
        TestFunction iMethod;
    };

// An internal structure containing a test case name and
// the pointer to function doing the test
class TCaseInfo
    {
    public:
        TPtrC iCaseName;
        TestFunction iMethod;

    TCaseInfo( const TText* a ) : iCaseName( ( TText* ) a )
        {
        };
    };

// ---------------------------------------------------------
// Thread Death Observer
// ---------------------------------------------------------
class CThreadDeathObserver: public CActive
    {
public:
    static CThreadDeathObserver* NewL ( TThreadId aThreadId, MThreadDeathNotification& aNotificationObserver );
    ~CThreadDeathObserver ();
    
    void ObserveDeath ();
    TBool Completed ()
        {
        return !IsActive ();
        }
    
private:
    CThreadDeathObserver ( MThreadDeathNotification& aNotificationObserver );
    void ConstructL ( TThreadId aThreadId );
    
    // From CActive
    void RunL ();
    TInt RunError ( TInt aError );
    void DoCancel ();
    
private:
    RThread                     iThreadToObserve;
    MThreadDeathNotification&   iObserver;
    };


#endif // DISKNOTIFYHANDLERTEST_H

