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
* Description:  Server-side sub-session of Accessory Mode
*
*/



#ifndef CACCSRVSUBMODE_H
#define CACCSRVSUBMODE_H

// INCLUDES
#include "AsyProxyAPI.h"
#include "AccSrvSubBase.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvMainSession;

// CLASS DECLARATION

/**
*  Accessory Mode server-side sub-session implementation.
*
*  AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSubMode ) : public CAccSrvSubBase
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
        static CAccSrvSubMode* NewL( CASYProxyAPI* aSession,
                                     CAccSrvConnectionController* aConnectionController,
                                     CAccSrvServerModel* aServerModel );

        /**
        * Destructor.
        */
        ~CAccSrvSubMode();

    public: // New functions

    public: // Functions from base classes
        
        /**
        * From CAccSrvSubBase
        * Handles received messages from client.
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void DispatchMessageL( const RMessage2& aMessage );

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

    protected:  // Functions from base classes

    private: // New functions

        /**
        * C++ default constructor.
        */
        CAccSrvSubMode( CASYProxyAPI* aSession,
                        CAccSrvConnectionController* aConnectionController,
                        CAccSrvServerModel* aServerModel );

        /**
        * By default Symbian 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Handles GetAccessoryMode() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetAccessoryModeL( const RMessage2& aMessage );

        /**
        * Handles GetAccessoryMode() asynchronous client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetAccessoryModeAsyncL( const RMessage2& aMessage );

        /**
        * Handles CancelGetAccessoryMode() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelGetAccessoryMode( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryModeChanged() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyAccessoryModeChangedL( const RMessage2& aMessage );

        /**
        * Handles CancelNotifyAccessoryModeChanged() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelNotifyAccessoryModeChanged( const RMessage2& aMessage );

        /**
        * Completes Accessory Mode request.
        *
        * @since S60 3.1
        * @param aMessage       RMessage2 to be completed.Client request.
        * @param aErrorCode     Error code for message completion.
        * @param aAccessoryMode Most recent accessory mode what client have.
        * @return void
        */
        void Complete( const RMessage2& aMessage,
                       TInt aErrorCode,
                       TAccPolAccessoryMode& aAccessoryMode );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Stored NotifyAccessoryModeChangedL() client request for later completion.
        RMessage2 iNotifyAccessoryModeChanged;

        // Stored GetAccessoryModeAsyncL() client request for later completion.
        RMessage2 iGetAccessoryModeAsync;

        // NotifyAccessoryModeChanged() is not reordered fast enough 
        // (e.g. due to different process sceduling by kernel).
        // When notification is re-ordered, iRecentAccessoryMode is used 
        // to check such situation.
        // This is also most recent accessory mode what client have.
        TAccPolAccessoryMode iRecentAccessoryMode;

        //First notification order is not completed directly.
        TBool                iFirstOrder;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVSUBMODE_H

// End of File
