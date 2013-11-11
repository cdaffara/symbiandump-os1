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
* Description:  Server-side sub-session of Accessory Connection Base
*
*/



#ifndef CACCSRVSUBCONNECTIONBASE_H
#define CACCSRVSUBCONNECTIONBASE_H

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
class CAccPolObjectCon;

// CLASS DECLARATION

/**
*  Accessory Connection Base server-side sub-session implementation.
*
*  AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSubConnectionBase ) : public CAccSrvSubBase
    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        ~CAccSrvSubConnectionBase();

    public: // New functions

    public: // Functions from base classes
    
        /**
        * From CAccSrvSubBase
        * Handles received messages from client. Sub classes of this base class
        * may or may not implement this also. However, if message is not handled
        * in sub class, it should call this base class to handle it.
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        virtual void DispatchMessageL( const RMessage2& aMessage );

        /**
        * From MQueueObserver.
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for GetValue() and SetValue() type of operations.
        *
        * @since S60 3.1
        * @param aMsgID       Message in question.
        * @param aCmdResponse Value of the operation, used in GetValue()
        *                     type of operations.
        * @param aErrorCode   Error Code of the operation.
        * @param aTrId        Transaction Id of the operation.
        * @return void
        */
        virtual void HandleValueMessageL( TMsgID aMsgID,
                                          TAccValueTypeTBool aCmdResponse,
                                          TInt aErrorCode,
                                          TInt aTrId );

        /**
        * From MQueueObserver.
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for GetValue() and SetValue() type of operations.
        *
        * @since S60 3.1
        * @param aMsgID       Message in question.
        * @param aCmdResponse Value of the operation, used in GetValue()
        *                     type of operations.
        * @param aErrorCode   Error Code of the operation.
        * @param aTrId        Transaction Id of the operation.
        * @return void
        */
        virtual void HandleValueMessageL( TMsgID aMsgID,
                                          TAccValueTypeTInt aCmdResponse,
                                          TInt aErrorCode,
                                          TInt aTrId );

        /**
        * From MQueueObserver.
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for GetValue() and SetValue() type of operations.
        *
        * @since S60 3.1
        * @param aMsgID       Message in question.
        * @param aCmdResponse Value of the operation, used in GetValue()
        *                     type of operations.
        * @param aErrorCode   Error Code of the operation.
        * @param aTrId        Transaction Id of the operation.
        * @return void
        */
        virtual void HandleValueMessageL( TMsgID aMsgID,
                                          TDesC8& aCmdResponse,
                                          TInt aErrorCode,
                                          TInt aTrId );

        /**
        * From MQueueObserver.
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for GetValue() and SetValue() type of operations.
        *
        * @param aMsgID       Message in question.
        * @param aTrId        Transaction Id of the operation.
        * @param aPtrBuf      Value of the operation, used in GetValue()
        *                     type of operations.
        * @param aErrorCode   Error Code of the operation.
        * @return void
        */
        virtual void HandleValueMessageL( TMsgID aMsgID,
                                          TInt aTrId,
                                          TPtr8* aPtrBuf,
                                          TInt aErrorCode );

    protected: // New functions

        /**
        * C++ default constructor.
        */
        CAccSrvSubConnectionBase( CASYProxyAPI* aSession,
                                  CAccSrvConnectionController* aConnectionController,
                                  CAccSrvServerModel* aServerModel );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private: // New functions

        /**
        * Handles GetSubblockNameArray() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        virtual void GetSubblockNameArrayL( const RMessage2& aMessage );

        /**
        * Handles GetValueL() TBool client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetValueTBoolL( const RMessage2& aMessage );

        /**
        * Handles GetValueL() TInt client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetValueTIntL( const RMessage2& aMessage );

        /**
        * Handles GetValueL() TDesC8 client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetValueTDes8L( const RMessage2& aMessage );
                
        /**
        * Handles GetValueL() CAccPolObjectBase client request.
        * If leave occurs, the client request is completed with that error code.
        *        
        * @param aMessage Client request.
        * @return void
        */
        void GetValueObjectBaseL( const RMessage2& aMessage );

        /**
        * Stores the value for the name to server model.
        *
        * @since S60 3.1
        * @param aGenericID  Generic ID in question.
        * @param aNameRecord Capability name which value is stored.
        * @param aValue      Value to be stored.
        * @return void
        */
        void StoreValueL( const TAccPolGenericID& aGenericID,
                          TAccPolNameRecord& aNameRecord,
                          TInt aValue );

        /**
        * Handles GetValueL() CAccPolObjectBase client request.
        * If leave occurs, the client request is completed with that error code.
        *        
        * @param aMessage Client request.
        * @return void
        */
        void GetObjectValueL( const RMessage2& aMessage );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Messages for later completion are stored here
        // Only one iMessage needed as long as RAccessoryConnectionBase has
        // only synchronous requests.
        RMessage2 iMessage;

        // Data buffer where object container is streamed.
        CBufFlat* iDataBuf; // Owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVSUBCONNECTIONBASE_H

// End of File
