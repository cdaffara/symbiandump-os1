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
* Description:  Server-side sub-session base class
*
*/



#ifndef CACCSRVSUBBASE_H
#define CACCSRVSUBBASE_H

// INCLUDES
#include "AsyProxyAPI.h"
#include "AccSrvConnectionController.h"
#include "AccSrvServerModel.h"
#include "AccSrvQueueObserver.h"//MQueueObserver
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvMainSession;

// CLASS DECLARATION

/**
*  Base class implementation for all server side sub-session.
*
*  AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSubBase ) : public CObject,
                                      public MQueueObserver
    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        ~CAccSrvSubBase();

    public: // New functions

        /**
        * Handles received messages from client. This function is pure virtual
        * so all subclasses must implement it.
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        virtual void DispatchMessageL( const RMessage2& aMessage ) = 0;

    public: // Functions from base classes

        /**
        * From MQueueObserver
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

        /**
        * From MQueueObserver
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for NotifyProcessCommand() completion.
        *
        * @since S60 3.1
        * @param aCmdn Type of command in question.
        * @param aTrId Transaction Id of the operation.
        * @param aASYCommandParamRecord  Data for NotifyProcessCommand() completion.
        * @param aPtrBuf Pointer to CBufFlat buffer where CAccPolObjectCon object
        *                is externalized to.
        * @return ETrue if the message was consumpted by a observer.
        *               Call-back order must be renewed after consumption
        *               if further call-backs are needed.
        */
        virtual TBool CompleteProcessCommandL( TProcessCmdId aCmdn,
                                               TInt aTrId,
                                               TASYCommandParamRecord& aASYCommandParamRecord,
                                               TPtr8* aPtrBuf=NULL );

        /**
        * From MQueueObserver
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
        * From MQueueObserver
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
        * From MQueueObserver
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
        * From MQueueObserver
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
        CAccSrvSubBase( CASYProxyAPI* aSession,
                        CAccSrvConnectionController* aConnectionController,
                        CAccSrvServerModel* aServerModel );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Updateds iFlags parameter in client side.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @param aFlag    This bit is turned to zero.
        * @return void
        */
        void Write( const RMessage2& aMessage, TInt aFlag );

    private:

    public:     // Data

    protected:  // Data

        // Handle to Connection Controller
        CAccSrvConnectionController* iConnectionController; //Not owned

        // Handle to Server Model
        CAccSrvServerModel*          iServerModel;          //Not Owned

    private:    // Data

        CASYProxyAPI* iProxy; //Not Owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVSUBBASE_H

// End of File
