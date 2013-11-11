/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMGENERICASYNCOBSERVER_H
#define HWRMGENERICASYNCOBSERVER_H

#include <e32base.h>
#include "HWRMClient.h"


/**
* Callback interface for async notification
*/
class MHWRMGenericAsyncObserver
    {
    public:
    
        /**
        * Notifies async response.
        *
        * @param aCommand Command for which the status value was obtained
        * @param aStatus Status value
        */
        virtual void AsyncRequestCompleted(TInt aCommand, TInt aStatusValue)=0;
    };


/**
* Active object wrapper for async HWRM client-server calls.
* Listens for status completion and notifies the callback.
* Also resubscribes the notification automatically.
*
*/
class CHWRMGenericAsyncObserver : public CActive
    {
    public:

        /**
        * This is a two-phase constructor method that is used to
        * create a new instance of CHWRMGenericAsyncObserver.
        * A newly created instance immediately starts listening to vibra status changes.
        *
        * @param aClient Reference to RHWRMClient instance that is used to (re)subscribe notification.
        * @param aCommand Command ID to be used in subscribing.
        * @param aInitialCommand Command ID to get initial value. 
        *         Initial value is not get if this parameter is negative.
        *         This initial command can only take one TInt parameter on slot 0,
        *         which is used to return the initial value.
        * @param aCallback Pointer to a callback instance. Can be NULL.
        * @return          A pointer to a new instance of the CHWRMGenericAsyncObserver class.
        */
        IMPORT_C static CHWRMGenericAsyncObserver* NewL(RHWRMClient& aClient, 
                                               TInt aCommand,
                                               TInt aInitialCommand,
                                               MHWRMGenericAsyncObserver* aCallback);

        /**
        * Destructor.
        */
        virtual ~CHWRMGenericAsyncObserver();

        /**
        * This method retrieves the most recently received status value.
        *
        * @return Integer indicating the latest status
        */
        TInt LatestStatus() const;

    protected:

        // From CActive
        void RunL();
        void DoCancel();
        
        // No need for RunError, as RunL will never leave

    private:

        /**
        * Constructor
        *
        * @param aClient Reference to RHWRMClient instance that is used to (re)subscribe notification.
        * @param aCommand Command ID to be used in subscribing.
        * @param aCallback Pointer to a callback instance. Must be non-NULL.
        */
        CHWRMGenericAsyncObserver(RHWRMClient& aClient, 
                                  TInt aCommand,
                                  MHWRMGenericAsyncObserver* aCallback);
                                                   
        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aInitialCommand Command ID to get initial value. 
        *         Initial value is not get if this parameter is negative.
        *         This initial command can only take one TInt parameter on slot 0,
        *         which is used to return the initial value.
        */
        void ConstructL(TInt aInitialCommand);

        /*
        * Order notification
        */
        void OrderNotification();
       
    private:
        
        MHWRMGenericAsyncObserver* iCallback;   // Not owned by this class. Can be NULL.
        RHWRMClient& iClient;                   // HWRM Client, not owned
        TInt iCommand;                          // Stores command
        TInt iLatestStatus;                     // Stores status
    };

#endif      // HWRMGENERICASYNCOBSERVER_H

// End of File
