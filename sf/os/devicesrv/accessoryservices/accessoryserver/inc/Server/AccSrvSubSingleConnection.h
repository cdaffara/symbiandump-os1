/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server-side sub-session of Accessory Single Connection
*
*/



#ifndef CACCSRVSUBSINGLECONNECTION_H
#define CACCSRVSUBSINGLECONNECTION_H

// INCLUDES
#include "AsyProxyAPI.h"
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
*  Accessory Single Connection server-side sub-session implementation
*
*  AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSubSingleConnection ) : public CAccSrvSubConnectionBase
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
        static CAccSrvSubSingleConnection* NewL( 
                    CASYProxyAPI* aSession,
                    CAccSrvConnectionController* aConnectionController,
                    CAccSrvServerModel* aServerModel );

        /**
        * Destructor.
        */
        ~CAccSrvSubSingleConnection();

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
        TBool CompleteMessageL( TMsgID aMsgID, TInt aErrorCode, TInt aUniqueID );

    protected: // New functions

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CAccSrvSubSingleConnection( CASYProxyAPI* aSession,
                                    CAccSrvConnectionController* aConnectionController,
                                    CAccSrvServerModel* aServerModel );

    private:  // new functions

        /**
        * Handles NotifyNewAccessoryConnected() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyNewAccessoryConnected( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryDisconnected() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyAccessoryDisconnected( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryValueChanged() TBool client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyAccessoryValueChangedTBoolL( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryValueChanged() TInt client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void NotifyAccessoryValueChangedTIntL( const RMessage2& aMessage );

        /**
        * Handles NotifyAccessoryValueChanged() Object client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 5.2
        * @param aMessage Client request.
        * @return void
        */
        void NotifyAccessoryValueChangedObjectL( const RMessage2& aMessage );

        /**
        * Handles CancelNotifyNewAccessoryConnected() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelNotifyNewAccessoryConnected( const RMessage2& aMessage );

        /**
        * Handles CancelNotifyAccessoryDisconnected() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelNotifyAccessoryDisconnected( const RMessage2& aMessage );

        /**
        * Handles CancelNotifyAccessoryValueChanged() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void CancelNotifyAccessoryValueChangedL( const RMessage2& aMessage );

        /**
        * Checks if NotifyAccessoryValueChanged() request was issued for
        * Accesory Capability Name and GID in question i.e. from SetValueL().
        *
        * @since S60 3.1
        * @return TBool indicating should this subsession be removed from queue
        */
        TBool HandleValueChangedTIntCompletionL();

        /**
        * Checks if NotifyAccessoryValueChanged() request was issued for
        * Accesory Capability Name and GID in question i.e. from SetValueL().
        * 
        * @since S60 3.1
        * @return TBool indicating should this subsession be removed from queue
        */
        TBool HandleValueChangedTBoolCompletionL();

        /**
        * Checks if NotifyAccessoryValueChanged() request was issued for
        * Accesory Capability Name and GID in question i.e. from SetValueL().
        * 
        * @since S60 5.2
        * @return TBool indicating should this subsession be removed from queue
        */
        TBool HandleValueChangedObjectCompletionL();

    public:     // Data

    protected:  // Data

    private:    // Data

        //Stored NotifyNewAccessoryConnected() client request for later completion.
        RMessage2 iNotifyNewAccessoryConnected;

        //Stored NotifyAccessoryDisconnected() client request for later completion.
        RMessage2 iNotifyAccessoryDisconnected;

        //Stored NotifyAccessoryValueChanged() TBool client request for later completion.
        RMessage2 iNotifyAccessoryValueChangedTBool;

        //Stored NotifyAccessoryValueChanged() TInt client request for later completion.
        RMessage2 iNotifyAccessoryValueChangedTInt;

        //Stored NotifyAccessoryValueChanged() Object client request for later completion.
        RMessage2 iNotifyAccessoryValueChangedObject;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVSUBSINGLECONNECTION_H

// End of File
