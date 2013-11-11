/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor server transaction base class
*
*/


#ifndef SENSRVTRANSACTION_H
#define SENSRVTRANSACTION_H

#include <e32base.h>
#include "sensrvmessage.h"

class CSensrvSession;
class CSensrvProxyManager;
class CSensrvPluginProxy;
class CSensrvChannel;

/**
 *  Base class for Sensor Server transactions
 *
 *  @since S60 5.0
 */
class CSensrvTransaction : public CBase
    {
    public:
        
        /** 
        * Transaction types
        *
        * Mediator transactions are never owned by transaction queues and are reused, 
        * even if the regular transactions are not. They are used for passing events 
        * internally between SSY and Server main thread.
        */
        enum TSensrvTransactionType
            {
            ETransTypeNone,                      ///< Uninitialized type.
            ETransTypeLoadSsy = 1,               ///< Loads SSY.
            ETransTypeDelayedInitialize,         ///< Reloads plugin after unloading is complete
            ETransTypeDelayedOpenChannel,        ///< Reattempts Open channel after loading is complete
            ETransTypeClientQueryChannels,       ///< Waiting transaction for channel info queries
            ETransTypeOpenChannel,               ///< Opens channel
            ETransTypeCloseChannel,              ///< Closes channel
            ETransTypeStartListening,            ///< Starts listening to a channel
            ETransTypeStopListening,             ///< Stops listening to a channel
            ETransTypeGetProperty,               ///< Gets single property of a channel
            ETransTypeGetAllProperties,          ///< Gets all properties of a channel  
            ETransTypeSetProperty,               ///< Sets single property of a channel
            ETransTypeStartConditionListening,   ///< Starts condition listening
            ETransTypeStopConditionListening,    ///< Stops condition listening
            ETransTypeMediatorFirst = 10000,     ///< Just here to indicate first internal transaction.  
            ETransTypeMediatorNewDataAvailable,  ///< SSY mediator notifies channel about new data with this transaction.
            ETransTypeMediatorForceBufferFilled, ///< Channel listener notifies SSY that buffer fill needs to be forced.
            ETransTypeMediatorForceChannelClose, ///< SSY mediator notifies channel that it needs to be closed because of error situation.
            ETransTypeMediatorPropertyChanged,    ///< SSY mediator notifies channels about changed property.
            ETransTypeMediatorChannelsChanged    ///< SSY mediator notifies proxy about channel registration
            };
            
         /**
         * Transactino states
         */
         enum TSensrvTransactionState
            {
            ETransStateUninitialized,      ///< Initial state of transaction
            ETransStateQueued,             ///< Transaction has been queued
            ETransStateExecuting,          ///< Transaction is executing
            ETransStateHandledAtSsy,       ///< Transaction has been handled at SSY.
            ETransStateNotifyFailed,       ///< Transaction notifying to a mediator failed
            ETransStateCompleted           ///< Transaction has been completed and will be deleted shortly.
            };
            
         /**
         * Channel open message data
         */
        class TChannelOpenMessageData
            {
        public:
            inline TChannelOpenMessageData( TInt aMaxBufferingCount, TInt aDataItemSize ) : 
                iMaxBufferingCount(aMaxBufferingCount), iDataItemSize(aDataItemSize) {}

            TInt iMaxBufferingCount;
            TInt iDataItemSize;
            };
        
        /**
        * Two phase constructor
        *
        * @since S60 5.0
        * @param aMessage The message related to this transaction. 
        *         Can be NULL if transaction has no client.
        *         Ownership of the message passes to this transaction.
        * @param aProxy The plugin proxy related to this transaction. 
        *         Can be NULL if transaction is not related to a single proxy.
        * @param aChannel The channel related to this transaction.
        *         Can be NULL if transaction is not related to a channel.
        * @param aType The transaction type.
        * @return New CSensrvTransaction instance
        * @exception KErrNoMemory Out of memory
        */
        static CSensrvTransaction* NewL(CSensrvMessage* aMessage, 
                                        CSensrvPluginProxy* aProxy, 
                                        CSensrvChannel* aChannel,
                                        TSensrvTransactionType aType);

        /**
        * Two phase constructor, leaves newly created object to cleanupstack.
        *
        * @since S60 5.0
        * @param aMessage The message related to this transaction. 
        *         Can be NULL if transaction has no client.
        *         Ownership of the message passes to this transaction.
        * @param aProxy The plugin proxy related to this transaction. 
        *         Can be NULL if transaction is not related to a single proxy.
        * @param aChannel The channel related to this transaction.
        *         Can be NULL if transaction is not related to a channel.
        * @param aType The transaction type.
        * @return New CSensrvTransaction instance
        * @exception KErrNoMemory Out of memory
        */
        static CSensrvTransaction* NewLC(CSensrvMessage* aMessage, 
                                        CSensrvPluginProxy* aProxy, 
                                        CSensrvChannel* aChannel,
                                        TSensrvTransactionType aType);

        /**
        * Destructor.
        */
        virtual ~CSensrvTransaction();

        /**
        * Completes the transaction and related message, if any. 
        *
        * @since S60 5.0
        */
        void Complete();

        /**
        * Gets session related to this transaction.
        *
        * @since S60 5.0
        * @return Session pointer. NULL if transaction has no client.
        */
        inline CSensrvSession* Session() const { return iMessage ? iMessage->Session() : NULL; };

        /**
        * Gets message related to this transaction.
        * 
        * @since S60 5.0
        * @return Message pointer. NULL if transaction has no client.
        */
        inline CSensrvMessage* Message() const { return iMessage; };

        /**
        * Gets the plugin proxy related to transaction.
        *
        * @since S60 5.0
        * @return Related proxy. NULL if transaction is not related to a single proxy.
        */
        inline CSensrvPluginProxy* Proxy() const { return iProxy; };

        /**
        * Gets related channel pointer.
        *
        * @since S60 5.0
        * @return Pointer to related channel. NULL if transaction is not related to a channel.
        */
        inline CSensrvChannel* Channel() const { return iChannel; };

        /**
        * Gets the transaction type.
        *
        * @since S60 5.0
        * @return Transaction type.
        */
        inline TSensrvTransactionType Type() const { return iType; };

        /**
        * Checks if transaction is a mediator transaction.
        *
        * @since S60 5.0
        * @return ETrue if transaction is a mediator transaction
        */
        inline TBool IsMediatorType() const { return iType > ETransTypeMediatorFirst ? ETrue : EFalse; };

        /**
        * Gets transaction timestamp.
        *
        * @since S60 5.0
        * @return TTime Transaction creation timestamp
        */
        inline const TTime& TimeStamp() const { return iTimeStamp; };

        /**
        * Resets transaction timestamp to current time.
        *
        * @since S60 5.0
        * @return TTime New transaction timestamp
        */
        inline const TTime& ResetTimeStamp() { iTimeStamp.HomeTime(); return iTimeStamp; };
        
        /**
        * Gets transaction error.
        *
        * @since S60 5.0
        * @return Error code.
        */
        inline TInt ErrorCode() const { return iErrorCode; };

        /**
        * Gets transaction state.
        *
        * @since S60 5.0
        * @return Transaction state.
        */
        inline TInt State() const { return iState; };

        /**
        * Sets transaction state.
        *
        * @since S60 5.0
        * @param aState The new state for transaction. 
        */
        void SetState(TSensrvTransactionState aState);

        /**
        * Sets data to message to be returned to client.
        * 
        * @since S60 5.0
        * @param aData Data to set to the message. 
        *              How TAny pointer is interpreted depends on transaction type:
        *              ETransTypeClientQueryChannels - RSensrvChannelInfoList*,
        *              ETransTypeOpenChannel - TChannelOpenMessageData*
        * @return KErrBadHandle Message is NULL.
        *         KErrBadDescriptor Message parameters are not descriptor type.
        *         KErrOverflow Message descriptor too small.
        */
        TInt SetMessageData(TAny* aData);

        /**
        * Sets transaction error.
        *
        * @since S60 5.0
        * @param aErrorCode Error code
        */
        void SetErrorCode(TInt aErrorCode);

        /**
        * Extracts message from transaction. 
        * Message completion responsibility transfers to caller.
        *
        * @since S60 5.0
        * @return The message previously owned by transaction.
        */
        CSensrvMessage* ExtractMessage();

    private:

        /**
         * C++ constructor
         *
         * @since S60 5.0
         * @param aMessage The message related to this transaction. 
         *         Can be NULL if transaction has no client.
         * @param aProxy The plugin proxy related to this transaction. 
         *         Can be NULL if transaction is not related to a single proxy.
         * @param aChannel The channel related to this transaction.
         *         Can be NULL if transaction is not related to a channel.
         * @param aType The transaction type.
         */
        CSensrvTransaction(CSensrvMessage* aMessage, 
                           CSensrvPluginProxy* aProxy, 
                           CSensrvChannel* aChannel,
                           TSensrvTransactionType aType);

    private:

        /**
        * Message related to transaction.
        * Can be NULL for clientless internal transactions.
        * Not own, but is responsible for completion. 
        * All messages are owned by session objects.
        */
        CSensrvMessage* iMessage;

        /**
        * Plugin proxy related to transaction.
        * Can be NULL for transactions not related to a single proxy.
        * Not Own.
        */
        CSensrvPluginProxy* iProxy;

        /**
        * Channel related to transaction.
        * Can be NULL for transactions not related to a channel.
        * Not Own.
        */
        CSensrvChannel* iChannel;

        /**
        * Transaction type.
        */
        TSensrvTransactionType iType;

        /**
        * Transaction state.
        */
        TSensrvTransactionState iState;

        /**
        * Transaction creation time. 
        */
        TTime iTimeStamp;

        /**
        * Error related to transaction.
        */
        TInt iErrorCode;
    };



#endif // SENSRVTRANSACTION_H 
