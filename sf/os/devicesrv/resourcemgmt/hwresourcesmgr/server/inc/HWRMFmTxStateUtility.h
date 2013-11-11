/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef HWRMFMTXSTATEUTILITY_H
#define HWRMFMTXSTATEUTILITY_H

//  INCLUDES
#include <e32base.h>
#include "HWRMPSChangeObserver.h"
#include "HWRMPluginHandler.h"
#include "HWRMFmTxPowersaveTimer.h"
#include "HWRMFmTxData.h"

// FORWARD DECLARATIONS
class CHWRMFmTxService;
class CHWRMFmTxCommonData;
class CHWRMPluginHandler;
class CHWRMReservationHandler;


// CLASS DECLARATION

/**
* Handles FM Tx and Audio Routing state changes.  
* Some events require plug-in requests to be sent automatically.
*/	
class CHWRMFmTxStateUtility : public CBase, 
                              public MHWRMPSChangeObserver,
                              public MHWRMIndicationHandler,
                              public MHWRMFmTxPowerSaveTimerObserver
    {
    public:
    
        // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        *
        * @param aPluginHandler Pointer to plugin handler for FM Tx
        * @param aWatcherPluginHandler Pointer to watcher plugin handler for FM Tx
        * @param aReservationHandler Pointer to reservation handler for FM Tx
        * @param aFmTxCommonData Pointer to FM Tx common data storage
        */
        static CHWRMFmTxStateUtility* NewL(CHWRMPluginHandler* aPluginHandler,
        								   CHWRMPluginHandler* aWatcherPluginHandler,
                                           CHWRMReservationHandler* aReservationHandler,
                                           CHWRMFmTxCommonData*     aFmTxCommonData);
        
        /**
        * Destructor.
        */
        virtual ~CHWRMFmTxStateUtility();
        
    public:  // Functions from base classes
    
        // From MHWRMPSChangeObserver
        void PSValueChanged(const TUid& aUid, TUint32 aKey, TInt aValue);                
        
        // From MHWRMIndicationHandler
        virtual void ProcessIndicationL( const TUint32 aId, TDesC8& aData );
        
        /**
         * HWRMFmTxPowerSaveTimer calls this method when timer expires
         */
        void TimerExpired();

    private:
    
        /**
        * C++ default constructor.
        */
        CHWRMFmTxStateUtility();

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aPluginHandler Pointer to plugin handler for FM Tx
        * @param aWatcherPluginHandler Pointer to watcher plugin handler for FM Tx
        * @param aReservationHandler Pointer to reservation handler for FM Tx        
        * @param aFmTxCommonData Pointer to FM Tx common data storage
        */
        void ConstructL(CHWRMPluginHandler* aPluginHandler,
        				CHWRMPluginHandler* aWatcherPluginHandler,
                        CHWRMReservationHandler* aReservationHandler,
                        CHWRMFmTxCommonData*     aFmTxCommonData);

        /**
        * Stops and deletes power save timer.
        *
        */                
        void CancelPowerSaveTimer();

    private:

        CHWRMFmTxService*      iFmTxService;         // Used to notify plugin about Audio Policy events.  Owned.
        CHWRMFmTxCommonData*   iFmTxCommonData;      // Pointer to FM Tx common data storage.  Not owned.

        TFmTxState             iPreviousState;       // Cached FM Tx state

        CHWRMPSChangeObserver* iFmTxStateObserver;   // Observes FM Tx status.  Owned.
        CHWRMPSChangeObserver* iPowerSaveObserver;   // Observes power save flag.  Owned.
        CHWRMPSChangeObserver* iAudioRoutedObserver; // Observes audio routed flag.  Owned.
        
        /* 
        * Power save timer for audio routing inactivity.
        * This variable is usually NULL, the timer is created and started when
        * audio output is no longer routed to FM Tx chip, which is still active.
        * Owned.
        */
        CHWRMFmTxPowerSaveTimer* iPowerSaveTimer;
		
		// Used to determine whether standby or off state entered from audio policy point of view
		TBool iEnableStandby;
    };

#endif  // HWRMFMTXSTATEUTILITY_H
            
// End of File
