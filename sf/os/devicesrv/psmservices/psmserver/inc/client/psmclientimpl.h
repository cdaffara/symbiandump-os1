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
* Description:  PSM client implementation class.
*
*/


#ifndef PSMCLIENTIMPL_H
#define PSMCLIENTIMPL_H

#include <e32base.h>
#include <psmclient.h>
#include "psmsession.h"

// FORWARD DECLARATIONS
class MPsmClientObserver;
class CPsmSettings;
class CPsmClientActive;

/**
 *  PSM client implementation class.
 *
 *  Implementation of power save mode client API.
 *
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CPsmClientImpl ) : public CPsmClient
    {

    friend class CPsmClient;

    public:  // Destructor

        /**
         * Destructor.
         */
        virtual ~CPsmClientImpl();

    public: // API methods

        /**
         * From CPsmClient.
         */
        void ChangePowerSaveMode( const TInt aPsmMode );

        /**
         * From CPsmClient.
         */
        void CancelPowerSaveModeChange();

        /**
         * From CPsmClient.
         */
        void RequestPowerSaveModeNotification();

        /**
         * From CPsmClient.
         */
        void CancelPowerSaveModeNotificationRequest();

        /**
         * From CPsmClient.
         */
        CPsmSettings& PsmSettings() const;

#ifdef _DEBUG  
        // these functions are used only for testing   
        virtual TInt HeapMark();
        virtual TInt HeapMarkEnd();
        virtual TInt HeapReset();
        virtual TInt SetHeapFailure(TInt aFailureRate);		
#endif //_DEBUG     

    private:

        /**
         * C++ constrcutor.
         */
        CPsmClientImpl( MPsmClientObserver& aObserver );

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();

    private: // data

        /**
         * Observer of the power save mode completion
         */
        MPsmClientObserver& iObserver;

        /**
         * PSM Client Settings API
         */
        CPsmSettings* iSettings;

        /**
         * Client-Server session to PSM server
         */
        RPsmServer iPsmServer;

        /**
         * PSM Client's active object.
         */
        CPsmClientActive* iActive;

    };

#endif // PSMCLIENTIMPL_H 
