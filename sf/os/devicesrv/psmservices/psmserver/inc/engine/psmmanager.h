/*
* Copyright (c) 2007 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM Manager class.
*
*/


#ifndef PSMMANAGER_H
#define PSMMANAGER_H

#include <e32base.h>

// FORWARD DECLARATIONS
class CPsmPluginLoader;
class CPsmSettingsHandler;
class MPsmModeChangeObserver;
class MPsmSettingsProvider; // Interface for Settings handler

/**
 *  PSM Manager class
 *
 *  Main class in PSM server. Controls the power save mode change chain
 *
 *  @since S60 5.0
 */
class CPsmManager : public CActive
    {
    public:  // Constructors and destructor

        /**
         * Creates a new PSM manager.
         *
         * @return A pointer to the created object.
         */
        static CPsmManager* NewL();

        /**
         * Creates a new PSM manager. Newly created instance is left in the cleanup stack.
         *
         * @return A pointer to the created object.
         */
        static CPsmManager* NewLC();

        /**
         * Destructor.
         */
        virtual ~CPsmManager();

        /**
         * Starts the mode change actions
         *
         * @param aPsmMode Mode to switch to.
         */
        void NotifyPowerSaveModeChangeL( TInt aPsmMode );

        /**
         * Cancels the mode change actions and restores previous mode.
         */
        void CancelPowerSaveModeChangeL();

        /**
         * Register new PSM observer. Adds observer to the list of registered observers
         */
        void RegisterObserverL( MPsmModeChangeObserver* aObserver );

        /**
         * Unreqister observer. Removes observer from the list of registered observers
         */
        void UnregisterObserver( MPsmModeChangeObserver* aObserver );

        /**
         * Returns reference to settings handler
         */
        MPsmSettingsProvider& SettingsProvider();

        /**
         * Makes NotifyPowerSaveModeChangeL to asynchronous
         */
        void IssueRequest();

#ifdef _DEBUG        
        /**
         * Delete plugin loader. Used only for testing purpose
         */         
        void ClosePluginLoader();

        /**
         * Compress ModeObservers Array. Used only for testing purpose
         */        
        void CompressModeObserversArray();

#endif //_DEBUG     

    protected: // From CActive

        /**
         * From CActive.
         */
        void RunL();

        /**
         * From CActive.
         */
        void DoCancel();

        /**
         * From CActive.
         */
        TInt RunError( TInt aError );

    private:

        /**
        * C++ constructor.
        */
        CPsmManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
         * Notifies PSM mode change observers
         */
        void NotifyObservers( const TInt aError ) const;

    private:    // Data

        /**
         * Plugin loader
         */
        CPsmPluginLoader* iPluginLoader;

        /**
         * Settings handler
         */
        CPsmSettingsHandler* iSettingsHandler;

        /**
         * Array of registered PSM mode observers
         */
        RArray<MPsmModeChangeObserver*> iModeObservers;

        /**
         * Indicates is mode change active
         */
        TBool iModeChangeCancel;

        /**
         * indicates that power save mode change is ongoing
         */
        TBool iPowerSaveModeChangeActive;
    };

#endif // PSMMANAGER_H 
