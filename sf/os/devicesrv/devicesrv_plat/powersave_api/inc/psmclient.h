/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM Client API class.
*
*/


#ifndef PSMCLIENT_H
#define PSMCLIENT_H

#include <e32base.h>

// FORWARD DECLARATIONS
class MPsmClientObserver;
class CPsmSettings;

/**
 *  PSM Client API class
 *
 *  Allows clients to change power save mode. Provides also a reference to 
 *  PSM Client Settings API. 
 *
 *  @lib psmclient.lib
 */
class CPsmClient : public CBase
    {
   public:  // Constructors and destructor

        /**
         * Creates a new PSM client.
         *
         * @param aObserver Observer to observe the power save mode change
         */
        IMPORT_C static CPsmClient* NewL( MPsmClientObserver& aObserver );
        IMPORT_C static CPsmClient* NewLC( MPsmClientObserver& aObserver );

    public: // API methods

        /**
         * Enable power save mode. It is possible to get notification
         * of mode change by requesting a notification using
         * RequestPowerSaveModeNotification. On successful change, 
         * client gets indication via PowerSaveModeChanged callback.
         * Failure in change is indicated to client via 
         * PowerSaveModeChangeError callback.
         *
         * @see 	       MPsmClientObserver
         * @param[in] 	   aPsmMode The power save mode to change to
         */
        virtual void ChangePowerSaveMode( const TInt aPsmMode ) = 0;

        /**
         * Cancels ongoing power save mode change and restores previous 
         * power save mode back. If there is no ongoing change, 
         * current mode remains.
         */
        virtual void CancelPowerSaveModeChange() = 0;

        /**
         * Requests notification of power save mode change. Client is
         * notified about mode changes until request cancelled using
         * CancelPowerSaveModeNotificationRequest.
         *
         * @see 	       MPsmClientObserver
         */
        virtual void RequestPowerSaveModeNotification() = 0;

        /**
         * Cancels notification request of power save mode change.
         *
         * @see 	       MPsmClientObserver
         */
        virtual void CancelPowerSaveModeNotificationRequest() = 0;

        /**
         * Reference to PSM Client Settings API.
         *
         * @return Reference to PSM Client Settings API
         */
        virtual CPsmSettings& PsmSettings() const = 0;

    };

#endif // PSMCLIENT_H 
