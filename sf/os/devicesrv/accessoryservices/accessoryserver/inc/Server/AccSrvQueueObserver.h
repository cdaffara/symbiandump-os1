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
* Description:  Interface for notification queue
*
*/



#ifndef MACCSRVQUEUEOBSERVER_H
#define MACCSRVQUEUEOBSERVER_H

//  INCLUDES
#include "AccSrvNotificationQueue.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Interface class to be used for Notification Queue callbacks.
*
*  @lib AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( MQueueObserver )
    {
    public:

        /**
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
                                        TInt aUniqueID ) = 0;

        /**
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for NotifyProcessCommand() completion.
        *
        * @since S60 3.1
        * @param aCmdn                   Type of command in question.
        * @param aTrId                   Transaction Id of the operation.
        * @param aASYCommandParamRecord  Data for NotifyProcessCommand() completion.
        * @param aPtrBuf Pointer to CBufFlat buffer where CAccPolObjectCon object
        *                is externalized to.
        * @return ETrue if the message was consumpted by a observer.
        *               Call-back order must be renewed after consumption
        *               if further call-backs are needed.
        */
        virtual TBool CompleteProcessCommandL( 
                        TProcessCmdId aCmdn,
                        TInt aTrId,
                        TASYCommandParamRecord& aASYCommandParamRecord,
                        TPtr8* aPtrBuf=NULL ) = 0;

        /**
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
                                          TInt aTrId ) = 0;

        /**
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
                                          TInt aTrId ) = 0;

        /**
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
                                          TInt aTrId ) = 0;
                                          
        /**
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for GetValue() and SetValue() type of operations.
        *
        * @since S60 5.0
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
                                          TInt aErrorCode ) = 0;                                          
                                          
    };

#endif // MACCSRVQUEUEOBSERVER_H

// End of File
