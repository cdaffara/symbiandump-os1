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



#ifndef HWRMFMTXSTATUSOBSERVER_H
#define HWRMFMTXSTATUSOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <hwrm/hwrmfmtx.h>
#include <e32property.h>

// CLASS DECLARATION

/**
* Active object wrapper for Publish and Subscribe one-shot notification handling.
* Listens for changes in FM Tx status and notifies the client.
*/
NONSHARABLE_CLASS(CHWRMFmTxStatusObserver) : public CActive
    {
    public:

        /**
        * This is a two-phase constructor method that is used to
        * create a new instance of CHWRMFmTxStatusObserver.
        * A newly created instance immediately starts listening to FM Tx status changes.
        *
        * @param aCallback Pointer to a callback instance. Can be NULL.
        * @return A pointer to a new instance of the CHWRMFmTxStatusObserver class.
        */
        static CHWRMFmTxStatusObserver* NewL(MHWRMFmTxObserver* aCallback);

        /**
        * Destructor.
        */
        virtual ~CHWRMFmTxStatusObserver();
        
        /**
        * This method retrieves the current FM Tx state. 
        *
        * @return TFMTxState indicating the current FM Tx state.
        */
        TFmTxState Status();

    protected:

        // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();

    private:

        /**
        * Constructor
        *
        * @param aCallback Pointer to a callback instance.
        */
        CHWRMFmTxStatusObserver(MHWRMFmTxObserver* aCallback);
                                                   
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /*
        * Order notification
        */
        void OrderNotification();

    private:
        
        MHWRMFmTxObserver* iCallback;      // Not owned by this class. Can be NULL.
        RProperty iProperty;               // P&S property
        
        TFmTxState iFmTxState; // Keeps track of last received status value
    };

#endif      // HWRMFMTXSTATUSOBSERVER_H

// End of File
