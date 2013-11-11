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


#ifndef C_ATEXTLISTEN_H
#define C_ATEXTLISTEN_H

#include <e32base.h>

class REComSession;

/**
 *  Notification interface class for ECOM plugin install/uninstall/version
 *  changes in the ECOM registration data.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( MATExtListen )
    {

public:

    /**
     * Notifies about an installed plugin
     *
     * @since S60 5.0
     * @param aPluginUid UID of installed plugin's interface
     * @param aPluginUid UID of installed plugin
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt NotifyPluginInstallation(
        TUid& aIfUid,
        CImplementationInformation* aImplInfo ) = 0;

    /**
     * Notifies about an uninstalled plugin
     *
     * @since S60 5.0
     * @param aPluginUid UID of uninstalled plugin's interface
     * @param aPluginUid UID of uninstalled plugin
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt NotifyPluginUninstallation(
        TUid& aIfUid,
        TUid& aPluginUid ) = 0;

    };

/**
 *  Class to store data needed for one ECOM plugin interface's array of
 *  implementation UIDs.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( TATExtIfData )
    {

public:

    /**
     * Interface UID for the iUidList implementation UIDs
     */
    TUid iIfUid;

    /**
     * Array of implementation UIDs for iIfUid interface UID
     */
    RImplInfoPtrArray iUidList;

    };

/**
 *  Class for listening to ECOM plugin interface changes.
 *  When a plugin implementation UID addition/removal is detected then one
 *  plugin installation/uninstallation callback is called. When a plugin
 *  implementation UID stays the same but its version number chages then
 *  plugin installation callback is called first and after that plugin
 *  uninstallation callback is called.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CATExtListen ) : public CActive
    {

public:

    /**
     * Two-phased constructor.
     * @param aSession ECOM session; used for getting ECOM implementation
     *                 information.
     * @param aCallback Callback to call when ECOM plugin installation or
     *                  uninstallation detected.
     * @return Instance of self
     */
	static CATExtListen* NewL( REComSession& aSession,
	                           MATExtListen* aCallback );

    /**
     * Two-phased constructor.
     * @param aSession ECOM session; used for getting ECOM implementation
     *                 information.
     * @param aCallback Callback to call when ECOM plugin installation or
     *                  uninstallation detected.
     * @return Instance of self
     */
    static CATExtListen* NewLC( REComSession& aSession,
                               MATExtListen* aCallback );
	
    /**
    * Destructor.
    */
    virtual ~CATExtListen();

    /**
     * Resets data to initial values
     *
     * @since S60 5.0
     * @return None
     */
    void ResetData();

    /**
     * Adds a new interface UID to the array of supported interfaces.
     * Uses DoAddInterfaceUidL() to add the interface UID.
     *
     * @since S60 5.0
     * @return None
     */
    TInt AddInterfaceUid( TUid& aIfUid );

    /**
     * Adds a new plugin UID to the array of supported plugin UIDs.
     *
     * @since S60 5.0
     * @return None
     */
    TInt AddPluginUid( TUid& aPluginUid );

    /**
     * Starts waiting for changes in ECOM plugin registration data
     *
     * @since S60 5.0
     * @return None
     */
    TInt IssueRequest();

    /**
     * Stops waiting for changes in ECOM plugin registration data
     *
     * @since S60 5.0
     * @return None
     */
    TInt Stop();

private:

    CATExtListen( REComSession& aSession,
                  MATExtListen* aCallback );

    void ConstructL();

    /**
     * Initializes this class
     *
     * @since S60 5.0
     * @return None
     */
    void Initialize();

    /**
     * Deletes instances of lists associated with ECOM plugin registration data.
     * Used to update iCurrentLists on ECOM changes and to delete in destructor.
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt DeleteLists( CArrayFixFlat<TATExtIfData>*& aLists );

    /**
     * Adds new interface UID and its implementation UIDs to iCurrentLists list
     * of ECOM plugin registration data.
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt DoAddInterfaceUidL( TUid& aIfUid );

    /**
     * Sorts one array of ECOM plugin registration data. Used to sort both the
     * new and the old (iCurrentLists) array of plugin registration data when a
     * change is detected by NotifyOnChange().
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt SortArrays( CArrayFixFlat<TATExtIfData>* aNewLists );

    /**
     * New list of UID implementation UIDs. Uses DoGetNewImplementationListL()
     * to get the new list and deletes if error detected. Used to get the new
     * list after a change is detected by NotifyOnChange().
     *
     * @since S60 5.0
     * @return New implementation list, NULL if error.
     */
    CArrayFixFlat<TATExtIfData>* NewImplementationList();

    /**
     * New list of UID implementation UIDs based on the iCurrentLists list of
     * interface UIDs. Used by GeNewImplementationList().
     *
     * @since S60 5.0
     * @param aNewList New implementation list
     * @return None
     */
     void DoGetNewImplementationListL( CArrayFixFlat<TATExtIfData>*& aNewList );

    /**
     * Checks for existing plugin uninstallation. When a change is detected the
     * change is notified by plugin uninstallation callback.
     *
     * @since S60 5.0
     * @param aNewLists New array of plugin ECOM registration data
     * @return ETrue if plugin uninstall detected, EFalse otherwise
     */
    TBool CheckPluginUninstall( CArrayFixFlat<TATExtIfData>* aNewLists );

    /**
     * Checks for new plugin installation. When a change is detected the change
     * is notified by plugin installation callback.
     *
     * @since S60 5.0
     * @param aNewLists New array of plugin ECOM registration data
     * @return ETrue if plugin install detected, EFalse otherwise
     */
    TBool CheckPluginInstall( CArrayFixFlat<TATExtIfData>* aNewLists );

    /**
     * Checks for plugin version upgrade/downgrade change. When a change is
     * detected the change is notified by plugin uninstallation and installation
     * callbacks.
     *
     * @since S60 5.0
     * @param aNewLists New array of plugin ECOM registration data
     * @return ETrue if plugin version change detected, EFalse otherwise
     */
    TBool CheckPluginVersionChange( CArrayFixFlat<TATExtIfData>* aNewLists );

// from base class CActive

    /**
     * From CActive.
     * Gets called when ECOM plugin registration data change is detected
     *
     * @since S60 5.0
     * @return None
     */
    void RunL();

    /**
     * From CActive.
     * Gets called on cancel
     *
     * @since S60 3.2
     * @return None
     */
    void DoCancel();

private:  // data

    /**
     * ECOM session; used for getting ECOM implementation information.
     */
    REComSession& iSession;

    /**
     * Callback to call when ECOM plugin installation or uninstallation
     * detected.
     * Now own.
     */
    MATExtListen* iCallback;

    /**
     * Array of supported plugin UIDs for the selected interfaces.
     * Own.
     */
    CArrayFixFlat<TUid>* iPluginUids;

    /**
     * Array of supported UIDs and their implementation UIDs.
     * Own.
     */
    CArrayFixFlat<TATExtIfData>* iCurrentLists;

    };

#endif  // C_ATEXTLISTEN_H
