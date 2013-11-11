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
* Description:  Server-side sub-session of Accessory Control
*
*/



#ifndef CACCSRVSUBCONTROL_H
#define CACCSRVSUBCONTROL_H

// INCLUDES
#include "AccSrvSubConnection.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvMainSession;

// CLASS DECLARATION

/**
*  Accessory Control server-side sub-session implementation
*
*  AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSubControl ) : public CAccSrvSubConnection
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
        static CAccSrvSubControl* NewL( CASYProxyAPI* aSession,
                                        CAccSrvConnectionController* aConnectionController,
                                        CAccSrvServerModel* aServerModel );

        /**
        * Destructor.
        */
        virtual ~CAccSrvSubControl();

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
                                          TAccValueTypeTBool aValueTBool,
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
                                          TAccValueTypeTInt aValueTInt,
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
        CAccSrvSubControl( CASYProxyAPI* aSession,
                           CAccSrvConnectionController* aConnectionController,
                           CAccSrvServerModel* aServerModel );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:  // New functions
        
        /**
        * Handles ConnectAccessory() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void ConnectAccessory2L( const RMessage2& aMessage );

        /**
        * Handles DisconnectAccessory() client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void DisconnectAccessoryL( const RMessage2& aMessage );

        /**
        * Handles SetValueL() TBool client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void SetValueTBoolL( const RMessage2& aMessage );

        /**
        * Handles SetValueL() TInt client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void SetValueTIntL( const RMessage2& aMessage );

        /**
        * Handles SetValueL() CAccPolObjectBase client request.
        * If leave occurs, the client request is completed with that error code.
        *        
        * @param aMessage Client request.
        * @return void
        */
        void SetValueObjectBaseL( const RMessage2& aMessage );

        /**
        * Handles AccessoryValueChangedNotify() TBool client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void AccessoryValueChangedTBoolNotifyL( const RMessage2& aMessage );

        /**
        * Handles AccessoryValueChangedNotify() TInt client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void AccessoryValueChangedTIntNotifyL( const RMessage2& aMessage );

        /**
        * Handles AccessoryValueChangedNotify() object client request.
        * If leave occurs, the client request is completed with that error code.
        *
        * @since S60 5.2
        * @param aMessage Client request.
        * @return void
        */
        void AccessoryValueChangedObjectNotifyL( const RMessage2& aMessage );

        /**
        * 
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void HandleSetValueTIntCompletionL( TInt aErrorCode, TInt aTrId );

        /**
        * 
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void HandleSetValueTBoolCompletionL( TInt aErrorCode, TInt aTrId );

        /**
        *         
        * @param aMessage Client request.
        * @return void
        */
        void HandleSetValueObjectCompletion( TInt aErrorCode, TInt aTrId );

    public:     // Data

    protected:  // Data

    private:    // Data

       //Stored ConnectAccessory() client request for later completion.
        RMessage2 iConnectAccessoryMsg;

        //Stored SetValueL() TBool client request for later completion.
        RMessage2 iSetValueTBoolMsg;

        //Stored SetValueL() TInt client request for later completion.
        RMessage2 iSetValueTIntMsg;
        
        //Stored SetValueL() Object client request for later completion.
        RMessage2 iSetValueObjectMsg;

        //Transaction id for outstanding SetValueL()
        TInt      iOutstandingTrId;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVSUBCONTROL_H

// End of File
