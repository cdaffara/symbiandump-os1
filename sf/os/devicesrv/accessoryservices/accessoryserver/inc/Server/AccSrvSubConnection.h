/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server-side sub-session of Accessory Connection
*
*/



#ifndef CACCSRVSUBCONNECTION_H
#define CACCSRVSUBCONNECTION_H

// INCLUDES
#include "AccSrvSubConnectionBase.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvMainSession;

// CLASS DECLARATION

/**
*  Accessory Connection server-side sub-session implementation.
*
*  AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSubConnection ) : public CAccSrvSubConnectionBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since S60 3.1
        * @param aSession              Handle to the ASY Proxy.
        * @param aConnectionController Handle to the Connection Controller.
        * @param aServerModel          Handle to the Server Model.
        * @return Pointer to a new subsession
        */
        static CAccSrvSubConnection* NewL( CASYProxyAPI* aSession,
                                           CAccSrvConnectionController* aConnectionController,
                                           CAccSrvServerModel* aServerModel );

        /**
        * Destructor.
        */
        ~CAccSrvSubConnection();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CAccSrvSubBase.
        * Handles received messages from client. Sub classes of this base class
        * may or may not implement this, however, if message is not handled
        * in sub class, it should call this base class to handle it.
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        virtual void DispatchMessageL( const RMessage2& aMessage );

        /**
        * From MQueueObserver.
        * Called when Queue passes message completion handling to registered observers.
        *
        * @since S60 3.1
        * @param aMsgID     Message in question.
        * @param aErrorCode Error Code of the operation.
        * @param aUniqueID  Unique ID of the Generic ID in question.
        * @return ETrue if the message was consumpted by a observer.
        *               Call-back order must be renewed after consumption
        *               if further call-backs are needed.
        */
        virtual TBool CompleteMessageL( TMsgID aMsgID,
                                        TInt aErrorCode,
                                        TInt aUniqueID );

    protected: // New functions

        /**
        * C++ default constructor.
        */
        CAccSrvSubConnection( CASYProxyAPI* aSession,
                              CAccSrvConnectionController* aConnectionController,
                              CAccSrvServerModel* aServerModel );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:  // New functions

        /**
        * Handles GetAccessoryConnectionStatus() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetAccessoryConnectionStatusL( const RMessage2& aMessage );

        /**
        * Handles GetAccessoryConnectionStatus() asynchronous client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetAccessoryConnectionStatusAsyncL( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryConnectionStatusChanged() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyAccessoryConnectionStatusChanged( const RMessage2& aMessage );

        /**
        * Handles CancelNotifyAccessoryConnectionStatusChanged() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelNotifyAccessoryConnectionStatusChanged( const RMessage2& aMessage );

        /**
        * Handles CancelGetAccessoryConnectionStatus() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelGetAccessoryConnectionStatus( const RMessage2& aMessage );


    public:     // Data

    protected:  // Data

    private:    // Data

        //Stored GetAccessoryConnectionStatus() client request for later completion.
        RMessage2 iGetAccessoryConnectionStatus;

        //Stored NotifyAccessoryConnectionStatusChanged() client request for later completion.
        RMessage2 iNotifyAccessoryConnectionStatusChanged;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVSUBCONNECTION_H

// End of File
