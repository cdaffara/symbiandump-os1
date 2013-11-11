/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECOM listener for ATEXT
*
*/


#include <ecom/ecom.h>
#include "atextlisten.h"
#include "utils.h"
#include "debug.h"

const TInt KGranularity = 4;

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CATExtListen* CATExtListen::NewL( REComSession& aSession,
                                  MATExtListen* aCallback )
    {
    CATExtListen* self = NewLC( aSession, aCallback ); 
    CleanupStack::Pop( self );    
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CATExtListen* CATExtListen::NewLC( REComSession& aSession,
                                   MATExtListen* aCallback )
    {
    CATExtListen* self = new (ELeave) CATExtListen( aSession,
                                                    aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CATExtListen::~CATExtListen()
    {
    ResetData();
    }

// ---------------------------------------------------------------------------
// Resets data to initial values
// ---------------------------------------------------------------------------
//
void CATExtListen::ResetData()
    {
    // APIs affecting this:
    // IssueRequest()
    Stop();
    // NewL(), AddPluginUid()
    if ( iPluginUids )
        {
        iPluginUids->Reset();
        delete iPluginUids;
        iPluginUids = NULL;
        }
    // NewL(), AddInterfaceUid()
    DeleteLists( iCurrentLists );
    }

// ---------------------------------------------------------------------------
// Adds a new interface UID to the array of supported interfaces.
// Uses DoAddInterfaceUidL() to add the interface UID.
// ---------------------------------------------------------------------------
//
TInt CATExtListen::AddInterfaceUid( TUid& aIfUid )
    {
    TRACE_FUNC_ENTRY
    TRAPD( retTrap, DoAddInterfaceUidL(aIfUid) );
    TRACE_FUNC_EXIT
    return retTrap;
    }

// ---------------------------------------------------------------------------
// Adds a new plugin UID to the array of supported plugin UIDs.
// ---------------------------------------------------------------------------
//
TInt CATExtListen::AddPluginUid( TUid& aPluginUid )
    {
    TRACE_FUNC_ENTRY
    TRAPD( retTrap, iPluginUids->AppendL(aPluginUid) );
    TRACE_FUNC_EXIT
    return retTrap;
    }

// ---------------------------------------------------------------------------
// Starts waiting for changes in ECOM plugin registration data
// ---------------------------------------------------------------------------
//
TInt CATExtListen::IssueRequest()
    {
    TRACE_FUNC_ENTRY
    if ( IsActive() )
        {
        TRACE_FUNC_EXIT
        return KErrNotReady;
        }
    iStatus = KRequestPending;
    iSession.NotifyOnChange( iStatus );
    SetActive();
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Stops waiting for changes in ECOM plugin registration data
// ---------------------------------------------------------------------------
//
TInt CATExtListen::Stop()
    {
    TRACE_FUNC_ENTRY
    if ( !IsActive() )
        {
        TRACE_FUNC_EXIT
        return KErrNotReady;
        }
    Cancel();
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CATExtListen::CATExtListen
// ---------------------------------------------------------------------------
//
CATExtListen::CATExtListen( REComSession& aSession,
                            MATExtListen* aCallback ) :
    CActive( EPriorityNormal ),
    iSession( aSession ),
    iCallback( aCallback )
    {
    Initialize();
    }

// ---------------------------------------------------------------------------
// CATExtListen::ConstructL
// ---------------------------------------------------------------------------
//
void CATExtListen::ConstructL()
    {
    TRACE_FUNC_ENTRY
    if ( !iCallback )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    CArrayFixFlat<TUid>* pluginUids =
        new (ELeave) CArrayFixFlat<TUid>( KGranularity );
    CleanupStack::PushL( pluginUids );
    iCurrentLists = new (ELeave) CArrayFixFlat<TATExtIfData>( KGranularity );
    CleanupStack::Pop( pluginUids );
    iPluginUids = pluginUids;
    CActiveScheduler::Add( this );
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Initializes this class
// ---------------------------------------------------------------------------
//
void CATExtListen::Initialize()
    {
    // Don't initialize iSession here (it is set through NewL)
    // Don't initialize iCallback here (it is set through NewL)
    iPluginUids = NULL;
    iCurrentLists = NULL;
    }

// ---------------------------------------------------------------------------
// Deletes instances of lists associated with ECOM plugin registration data.
// Used to update iCurrentLists on ECOM changes and to delete in destructor.
// ---------------------------------------------------------------------------
//
TInt CATExtListen::DeleteLists( CArrayFixFlat<TATExtIfData>*& aLists )
    {
    TRACE_FUNC_ENTRY
    if ( !aLists )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt i;
    TInt count = aLists->Count();
    for ( i=0; i<count; i++ )
        {
        TATExtIfData& ifData = (*aLists)[i];
        ifData.iUidList.ResetAndDestroy();
        ifData.iUidList.Close();
        }
    aLists->Reset();
    delete aLists;
    aLists = NULL;
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Adds new interface UID and its implementation UIDs to iCurrentLists list of
// ECOM plugin registration data.
// ---------------------------------------------------------------------------
//
TInt CATExtListen::DoAddInterfaceUidL( TUid& aIfUid )
    {
    TRACE_FUNC_ENTRY
    // First get the implementations for this aIfUid
    TATExtIfData ifData;
    ifData.iIfUid = aIfUid;
    iSession.ListImplementationsL( aIfUid, ifData.iUidList );
    CleanupResetDestroyClosePushL( ifData.iUidList );
    // Add the implementations to iCurrentLists
    iCurrentLists->AppendL( ifData );
    CleanupStack::Pop( &ifData.iUidList );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Sorts one array of ECOM plugin registration data. Used to sort both the new
// and the old (iCurrentLists) array of plugin registration data when a change
// is detected by NotifyOnChange().
// ---------------------------------------------------------------------------
//
TInt CATExtListen::SortArrays( CArrayFixFlat<TATExtIfData>* aLists )
    {
    TRACE_FUNC_ENTRY
    if ( !aLists )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt i;
    TInt count = aLists->Count();
    for ( i=0; i<count; i++ )
        {
        TATExtIfData& ifData = (*aLists)[i];
        ifData.iUidList.SortIntoAddressOrder();
        }
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// New list of UID implementation UIDs. Uses DoGetNewImplementationListL() to
// get the new list and deletes if error detected. Used to get the new list
// after a change is detected by NotifyOnChange().
// ---------------------------------------------------------------------------
//
CArrayFixFlat<TATExtIfData>* CATExtListen::NewImplementationList()
    {
    TRACE_FUNC_ENTRY
    CArrayFixFlat<TATExtIfData>* newList = NULL;
    TRAPD( retTrap, DoGetNewImplementationListL(newList) );
    if ( retTrap != KErrNone )
        {
        DeleteLists( newList );
        TRACE_FUNC_EXIT
        return NULL;
        }
    TRACE_FUNC_EXIT
    return newList;
    }

// ---------------------------------------------------------------------------
// New list of UID implementation UIDs based on the iCurrentLists list of
// interface UIDs. Used by NewImplementationList().
// ---------------------------------------------------------------------------
//
void CATExtListen::DoGetNewImplementationListL(
    CArrayFixFlat<TATExtIfData>*& aNewList )
    {
    TRACE_FUNC_ENTRY
    TInt i;
    TInt count = iCurrentLists->Count();
    aNewList = NULL;
    aNewList = new (ELeave) CArrayFixFlat<TATExtIfData>( KGranularity );
    for ( i=0; i<count; i++ )
        {
        TATExtIfData newInfo;
        TATExtIfData& ifData = (*iCurrentLists)[i];
        newInfo.iIfUid = ifData.iIfUid;
        iSession.ListImplementationsL( newInfo.iIfUid, newInfo.iUidList );
        CleanupResetDestroyClosePushL( newInfo.iUidList );
        // Add the implementations to newList
        aNewList->AppendL( newInfo );
        CleanupStack::Pop( &newInfo.iUidList );
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Checks for existing plugin uninstallation. When a change is detected the
// change is notified by plugin uninstallation callback.
// ---------------------------------------------------------------------------
//
TBool CATExtListen::CheckPluginUninstall(
    CArrayFixFlat<TATExtIfData>* aNewLists )
    {
    TRACE_FUNC_ENTRY
    if ( !aNewLists || !iCurrentLists ||
         aNewLists->Count() != iCurrentLists->Count() )
        {
        TRACE_FUNC_EXIT
        return EFalse;
        }
    TInt i;
    TInt j;
    TInt ifDataCount = iCurrentLists->Count();
    for ( i=0; i<ifDataCount; i++ )
        {
        TATExtIfData& ifDataCurrent = (*iCurrentLists)[i];
        TATExtIfData& ifDataNew = (*aNewLists)[i];
        TInt pluginCountCurrent = ifDataCurrent.iUidList.Count();
        TInt pluginCountNew = ifDataNew.iUidList.Count();
        if ( pluginCountCurrent<=pluginCountNew ||
             ifDataCurrent.iIfUid!=ifDataNew.iIfUid )
            {
            continue;
            }
        for ( j=0; j<pluginCountCurrent; j++ )
            {
            TBool outOfBounds = EFalse;
            TBool differentUids = EFalse;
            if ( j >= pluginCountNew )
                {
                outOfBounds = ETrue;
                }
            CImplementationInformation* implCurrent = ifDataCurrent.iUidList[j];
            TUid pluginUidCurrent = implCurrent->ImplementationUid();
            if ( !outOfBounds )
                {
                CImplementationInformation* implNew = ifDataNew.iUidList[j];
                TUid pluginUidNew = implNew->ImplementationUid();
                if ( pluginUidNew != pluginUidCurrent )
                    {
                    differentUids = ETrue;
                    }
                }
            if ( outOfBounds || differentUids )
                {
                iCallback->NotifyPluginUninstallation( ifDataCurrent.iIfUid,
                                                       pluginUidCurrent );
                return ETrue;
                }
            }
        }
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Checks for new plugin installation. When a change is detected the change is
// is notified by plugin installation callback.
// ---------------------------------------------------------------------------
//
TBool CATExtListen::CheckPluginInstall(
    CArrayFixFlat<TATExtIfData>* aNewLists )
    {
    TRACE_FUNC_ENTRY
    if ( !aNewLists || !iCurrentLists ||
         aNewLists->Count() != iCurrentLists->Count() )
        {
        TRACE_FUNC_EXIT
        return EFalse;
        }
    TInt i;
    TInt j;
    TInt ifDataCount = iCurrentLists->Count();
    for ( i=0; i<ifDataCount; i++ )
        {
        TATExtIfData& ifDataCurrent = (*iCurrentLists)[i];
        TATExtIfData& ifDataNew = (*aNewLists)[i];
        TInt pluginCountCurrent = ifDataCurrent.iUidList.Count();
        TInt pluginCountNew = ifDataNew.iUidList.Count();
        if ( pluginCountNew<=pluginCountCurrent ||
             ifDataNew.iIfUid!=ifDataCurrent.iIfUid )
            {
            continue;
            }
        for ( j=0; j<pluginCountNew; j++ )
            {
            TBool outOfBounds = EFalse;
            TBool differentUids = EFalse;
            if ( j >= pluginCountCurrent )
                {
                outOfBounds = ETrue;
                }
            CImplementationInformation* implNew = ifDataNew.iUidList[j];
            TUid pluginUidNew = implNew->ImplementationUid();
            if ( !outOfBounds )
                {
                CImplementationInformation* implCurrent = ifDataCurrent.iUidList[j];
                TUid pluginUidCurrent = implCurrent->ImplementationUid();
                if ( pluginUidCurrent != pluginUidNew )
                    {
                    differentUids = ETrue;
                    }
                }
            if ( outOfBounds || differentUids )
                {
                iCallback->NotifyPluginInstallation( ifDataNew.iIfUid,
                                                     implNew );
                return ETrue;
                }
            }
        }
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Checks for plugin version upgrade/downgrade change. When a change is
// detected the change is notified by plugin uninstallation and installation
// callbacks.
// ---------------------------------------------------------------------------
//
TBool CATExtListen::CheckPluginVersionChange(
    CArrayFixFlat<TATExtIfData>* aNewLists )
    {
    TRACE_FUNC_ENTRY
    if ( !aNewLists || !iCurrentLists ||
         aNewLists->Count() != iCurrentLists->Count() )
        {
        TRACE_FUNC_EXIT
        return EFalse;
        }
    TInt i;
    TInt j;
    TInt ifDataCount = iCurrentLists->Count();
    for ( i=0; i<ifDataCount; i++ )
        {
        TATExtIfData& ifDataCurrent = (*iCurrentLists)[i];
        TATExtIfData& ifDataNew = (*aNewLists)[i];
        TInt pluginCountCurrent = ifDataCurrent.iUidList.Count();
        TInt pluginCountNew = ifDataNew.iUidList.Count();
        if ( pluginCountNew!=pluginCountCurrent ||
             ifDataNew.iIfUid!=ifDataCurrent.iIfUid )
            {
            continue;
            }
        for ( j=0; j<pluginCountNew; j++ )
            {
            CImplementationInformation* implCurrent = ifDataCurrent.iUidList[j];
            CImplementationInformation* implNew = ifDataNew.iUidList[j];
            TUid pluginUidCurrent = implCurrent->ImplementationUid();
            TUid pluginUidNew = implNew->ImplementationUid();
            if ( implCurrent->Version()!=implNew->Version() &&
                 pluginUidCurrent==pluginUidNew )
                {
                iCallback->NotifyPluginUninstallation( ifDataCurrent.iIfUid,
                                                       pluginUidCurrent );
                iCallback->NotifyPluginInstallation( ifDataNew.iIfUid,
                                                     implNew );
                TRACE_FUNC_EXIT
                return ETrue;
                }
            }
        }
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From CActive.
// Gets called when ECOM plugin registration data change is detected
// ---------------------------------------------------------------------------
//
void CATExtListen::RunL()
    {
    TRACE_FUNC_ENTRY
    IssueRequest();
    // First get the new list
    CArrayFixFlat<TATExtIfData>* newLists = NewImplementationList();
    if ( !newLists )
        {
        TRACE_FUNC_EXIT
        return;
        }
    SortArrays( newLists );
    SortArrays( iCurrentLists );
    // Next check the cases of plugin uninstall. This happens if an entry in the
    // iCurrentLists can't be found from the new list.
    TBool uninstalled = CheckPluginUninstall( newLists );
    // Next check the cases of plugin install. This happens if an entry in the
    // new list can't be found from the iCurrentList.
    TBool installed = EFalse;
    if ( !uninstalled )
        {
        installed = CheckPluginInstall( newLists );
        }
    if ( !uninstalled && !installed )
        {
        CheckPluginVersionChange( newLists );
        }
    // Delete existing iCurrentLists and set newLists
    DeleteLists( iCurrentLists );
    iCurrentLists = newLists;
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Gets called on cancel
// ---------------------------------------------------------------------------
//
void CATExtListen::DoCancel()
    {
    TRACE_FUNC_ENTRY
    iSession.CancelNotifyOnChange( iStatus );
    TRACE_FUNC_EXIT
    }
