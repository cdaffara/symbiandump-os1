/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor server channel implemetation
*
*/


#ifndef SENSERVERCHANNEL_H
#define SENSERVERCHANNEL_H

#include <e32base.h>
#include "sensrvtypes.h"
#include "sensrvchannelinfo.h"
#include "sensrvresourcechannelinfo.h"
#include "sensrvmessage.h"
#include "ssypropertyprovider.h"
#include "ssychanneldataprovider.h"
#include "sensrvgeneralproperties.h"

class CSensrvTransaction;
class CSensrvPluginProxy;
class CSensrvTransactionQueue;
class CSensrvPropertyQueue;
class CSensrvChannelListener;
class CSensrvChannelBuffer;
class CSensrvConditionEvaluator;

/**
* Implements channel related handling.
*
* @since S60 5.0
*/
class CSensrvChannel : public CBase
    {
    public:

        /**
        * Channel state.
        */
        enum TSensrvChannelState
            {
            EChannelStateClosed,             ///< Channel is closed
            EChannelStateOpen,               ///< Channel is open to at least one client
            EChannelStateListening           ///< Channel is listening for data
            };

        /**
        * Two phase constructor
        *
        * @since S60 5.0
        * @param aInfo Channel info.
        * @param aProxy Callback proxy.
        * @return New CSensrvChannel instance
        * @exception KErrNoMemory Out of memory
        */
		static CSensrvChannel* NewL( const TSensrvResourceChannelInfo& aInfo,
									 CSensrvPluginProxy& aProxy );

        /**
        * Destructor.
        */
        virtual ~CSensrvChannel();

        /**
        * Returns the channel id
        *
        * @since S60 5.0
        * @return Channel id.
        */
        inline TSensrvChannelId Id() { return iId; };

        /**
        * Gets channel state.
        * Called from both threads.
        *
        * @since S60 5.0
        * @return Current channel state
        */
        inline TSensrvChannelState State() const { return iState; };

        /**
        * Gets parent proxy reference.
        * Called from both threads.
        *
        * @since S60 5.0
        * @return Current channel state
        */
        inline CSensrvPluginProxy& Proxy() { return iProxy; };

        /**
        * Gets data provider pointer.
        * Called from SSY thread.
        *
        * @since S60 5.0
        * @return Current data provider pointer. Can be NULL.
        */
        inline MSsyChannelDataProvider* DataProvider() { return iDataProvider; };

        /**
        * Gets property provider pointer.
        * Called from SSY thread.
        *
        * @since S60 5.0
        * @return Current property provider pointer. Can be NULL.
        */
        inline MSsyPropertyProvider* PropertyProvider() { return iPropertyProvider; };

        /**
        * Gets data buffer pointer.
        * Called from SSY thread.
        *
        * @since S60 5.0
        * @return Data buffer pointer. Can be NULL.
        */
        inline CSensrvChannelBuffer* Buffer() { return iChannelBuffer; };

        /**
        * Gets maximum allowed buffering count.
        * Called from server thread.
        *
        * @since S60 5.0
        * @return Maximum allowed buffering count
        */
        inline TInt MaxBufferingCount() { return iMaxBufferingCount; };

        /**
        * Gets the persistent ETransTypeMediatorNewDataAvailable transaction.
        * Called from SSY thread.
        *
        * @since S60 5.0
        * @return ETransTypeMediatorNewDataAvailable transaction
        */
        inline CSensrvTransaction* DataAvailableTransaction() const { return iDataAvailableTransaction; };

       /**
        * Gets the persistent ETransTypeMediatorPropertyChanged transaction.
        * Called from SSY thread.
        *
        * @since S60 5.0
        * @return ETransTypeMediatorPropertyChanged transaction.
        */
        inline CSensrvTransaction* PropertyChangedTransaction() const { return iPropertyChangedTransaction; };

        /**
        * Gets the persistent ETransTypeMediatorForceBufferFilled transaction.
        * Called from server thread.
        *
        * @since S60 5.0
        * @return ETransTypeMediatorForceBufferFilled transaction
        */
        inline CSensrvTransaction* ForceBufferFilledTransaction() const { return iForceBufferFilledTransaction; };

        /**
        * Gets the persistent ETransTypeMediatorForceChannelClose transaction.
        * Called from SSY thread.
        *
        * @since S60 5.0
        * @return ETransTypeMediatorForceChannelClose transaction
        */
        inline CSensrvTransaction*  ForceChannelCloseTransaction() const { return iForceChannelCloseTransaction; };

        /**
        * Gets the listener count.
        * Called from server thread.
        *
        * @since S60 5.0
        * @return Listener count.
        */
        inline TInt ListenerCount() const { return iListenerList.Count(); };

        /**
        * Gets the data item size
        * Called from server thread.
        *
        * @since S60 5.0
        * @return Data item size
        */
        inline TInt DataItemSize() const { return iChannelInfo.iDataItemSize; };

        /**
        * Handles client message.
        * Message completion responsibility transfers.
        * Called only within server thread.
        *
        * @since S60 5.0
        * @param aMessage The message containing the client request.
        */
        void DispatchMessage( CSensrvMessage& aMessage );

        /**
        * Indicates channel has been opened by SSY
        * Called from SSY thread only.
        *
        * @since S60 5.0
        * @param aErrorCode Indicates any error that occurred during channel opening.
        * @param aDataProvider Pointer to channel data provider instance
        * @param aSensorPropertyProvider Pointer to sensor property provider instance
        */
        void ChannelOpened( TInt aErrorCode,
                            MSsyChannelDataProvider* aDataProvider,
                            MSsyPropertyProvider* aSensorPropertyProvider );

        /**
        * Indicates channel has been closed by SSY
        * Called from SSY thread only.
        *
        * @since S60 5.0
        */
        void ChannelClosed();

        /**
        * Store a changed property provided by SSY.
        * Called from SSY thread only.
        *
        * @since S60 5.0
        * @param aProperty Property that has changed
        * @param aIgnoreSession Do not store change for this session. Can be NULL.
        * @return ETrue if property was stored to any listener.
        */
        TBool StoreChangedProperty( const TSensrvProperty& aProperty,
                                    const CSensrvSession* aIgnoreSession );

        /**
        * Completes transaction after it has been handled by SSY.
        * Only called from server thread.
        *
        * @since S60 5.0
        * @param aTransaction The transaction to complete
        */
        void CompleteTransaction( CSensrvTransaction* aTransaction );

        /**
        * Cleans up session related data.
        *
        * @since S60 5.0
        * @param aSession The session that was deleted
        */
        void SessionTerminated( CSensrvSession* aSession );

        /**
        * Handles notify failure at SSY
        *
        * @since S60 5.0
        */
        void HandleSsyNotifyFailure();

        /**
        * Forces buffer filled at SSY
        *
        * @since S60 5.0
        */
        void ForceBufferFilled();

        /**
        * Recalculates buffering count, if there is more than one
        * client.
        *
        * @since S60 5.0
        * @return The new buffering count. If returned value is zero,
        *         it means there are no listeners in channel.
        */
        TInt RecalculateBufferingCount();

        /**
        * Creates a new condition evaluator or returns an existing one.
        *
        * @since S60 5.0
        * @return The condition evaluator reference.
        */
        CSensrvConditionEvaluator* ConditionEvaluatorL();

        /**
        * Gets listener for session.
        *
        * @since S60 5.0
        * @param aSession Session for which the listener is needed.
        * @return Listener pointer or NULL if not found.
        */
        CSensrvChannelListener* GetListener(CSensrvSession* aSession);

        /**
        * Takes the property set control.
        * Required priority checks must be done by client before using this method.
        *
        * @since S60 5.0
        * @param[in] aListener The listener that takes control or NULL if control is freed.
        */
        void TakePropertySetControl( CSensrvChannelListener* aListener );

        /**
        * Gets the listener that has taken the property set control.
        *
        * @since S60 5.0
        * @return The listener that has taken the property set control or NULL if control has not been taken.
        */
        inline CSensrvChannelListener* TakenPropertySetControl() const { return iCurrentSetPropertyControl; };

        /**
        * Gets the highest priority listener and its multiples from this channel.
        *
        * @since S60 5.0
        * @param[out] aHighestPriority Stores the highest priority.
        * @return Multiples of highest listener priority
        */
        TInt GetHighestListenerPriority( TInt& aHighestPriority );

        /**
        * Gets channel group
        * Called from server thread.
        *
        * @since S60 5.0
        * @return Channel group.
        */
        TSensrvResourceChannelInfo::TSensrvChannelGroup ChannelGroup();

    private:

        /**
        * C++ constructor
        *
        * @since S60 5.0
        * @param aInfo Channel info.
        * @param aProxy Callback proxy.
        */
        CSensrvChannel(const TSensrvChannelInfo& aInfo,
                       CSensrvPluginProxy& aProxy);

        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL();

        /**
        * Handles next transaction in queue.
        * If first transaction is still executing
        * or the queue is empty, does nothing.
        *
        * @since S60 5.0
        */
        void HandleNextTransaction();

        /**
        * Deletes a listener.
        *
        * @since S60 5.0
        * @param aSession Session for which the listener is deleted.
        */
        void DeleteListener(CSensrvSession* aSession);

        /**
        * Finds a listener by session.
        *
        * @since S60 5.0
        * @param aSession Session for which the listener is found.
        */
		CSensrvChannelListener* FindListener( CSensrvSession* aSession );

        /**
        * Closes the channel to SSY if there are no listeners.
        *
        * @since S60 5.0
        */
        void CloseIfNeeded();

        /**
        * Handles start data and condition listening transactions
        *
        * @since S60 5.0
        * @param aTransaction Reference to the transaction.
        * @return Error code
        */
        TInt HandleStartListeningTransaction(CSensrvTransaction& aTransaction);

        /**
        * Handles stop data and condition listening transactions
        *
        * @since S60 5.0
        * @param aTransaction Reference to the transaction.
        * @param[out] aSsyTransactionNeeded Returns ETrue if SSY transaction is needed.
        * @return Error code
        */
        TInt HandleStopListeningTransaction(CSensrvTransaction& aTransaction,
                                            TBool& aSsyTransactionNeeded);

        /**
        * Gets the highest priority listener and its multiples from all affected channels.
        *
        * @since S60 5.0
        * @param[out] aHighestPriority Stores the highest priority.
        * @return Multiples of highest listener priority
        */
        TInt GetHighestListenerPriorityOfAffectedChannels( TInt& aHighestPriority );

        /**
        * Updated set property success indication to channel listeners
        *
        * @since S60 5.0
        * @param[in] aChannel Channel to update
        * @param aHighestOfAffectedChannels Highest priority of affected channels
        * @param aMultiplesOnAffectedChannels Multiples of highest priority listeners
        */
        static void UpdateSetPropertySuccessIndToChannelListeners(
            CSensrvChannel& aChannel, TInt aHighestOfAffectedChannels, TInt aMultiplesOnAffectedChannels );

        /**
        * Handles set property success indications on property change
        *
        * @since S60 5.0
        */
        void HandleSetPropertySuccessIndOnPropertyChange();

        /**
        * Handles set property success indications on listener creation or deletion
        *
        * @since S60 5.0
        */
        void HandleSetPropertySuccessIndOnCreateOrDelete();


        /**
        * Handles dependent channels on channel open
        *
        * @since S60 5.0
        */
		void HandleDependencyOnOpen();

        /**
        * Handles dependent channels on channel close
        *
        * @since S60 5.0
        */
		void HandleDependencyOnClose( const TInt& aPriority );

        /**
        * Sets dependent channel availability
        *
        * @since S60 5.0
        */
		TInt SetAvailability( TSensrvChannelId aChannelId, TSensrvProperty& aProperty, TSensrvAvailability aValue, const TSecurityInfo& aSecurityInfo );

        /**
        * Sets dependent channel availability
        *
        * @since S60 5.0
        */
		TInt SetChannelAvailability( TSensrvChannelId aChannelIdToEnable, TSensrvChannelId aChannelIdToDisable, TSensrvProperty& aProperty );

        /**
        * Sets dependent channel availability
        *
        * @since S60 5.0
        */
		TInt SetChannelAvailability( TSensrvChannelId aChannelId, TSensrvProperty& aProperty, TSensrvAvailability aValue );

        /**
        * Get channel from affected channels list with the lowest priority
        *
        * @since S60 5.0
        */
		TInt GetLowestChannelPriority( const RSensrvChannelList* aAffectedChannelsList, TSensrvChannelId& aChannelId );


        /**
        * Get channel from affected channels list with the highest priority
        *
        * @since S60 5.0
        */
		TInt GetHighestChannelPriority( const RSensrvChannelList* aAffectedChannelsList, TSensrvChannelId& aChannelId );

        /**
        * Gets the affected channel list from SSY
        *
        * @since S60 5.0
        * @return Affected channel list or NULL
        */
        RSensrvChannelList* GetAffectedChannels() const;

        /**
        * Gets the affected channel list from SSY
        *
        * @since S60 5.0
        * @return Affected channel list or NULL
        */
        RSensrvChannelList* GetAvailabilityAffectedChannels( TSensrvAvailability aValue ) const;

        /**
        * Reads channel group
        *
        * @since S60 5.0
        * @return Channel group.
        */
        TSensrvResourceChannelInfo::TSensrvChannelGroup ReadChannelGroup();

        /**
        * Handles channel availability property change
        *
        * @since S60 5.0
        */
        void HandleAvailabilityChange( const TSensrvProperty& aProperty );

        /**
        * Handles channel availability property change
        *
        * @since S60 5.0
        */
		void HandleAvailabilityChange( const TSensrvAvailability& value );

        /**
        * Handles channel availability property change to unavailabe
        *
        * @since S60 5.0
        */
		void CSensrvChannel::SetChannelUnavailable( );

        /**
        * Handles channel availability property change to availabe
        *
        * @since S60 5.0
        */
		void CSensrvChannel::SetChannelAvailable( );

    private: // Member variables

        /**
        * Channel identifier of this channel.
        */
        TSensrvChannelId iId;

        /**
        * Transaction queue for transactions targeting this channel.
        * Own. iChannelQueue owns the queued transactions.
        */
        CSensrvTransactionQueue* iChannelQueue;

        /**
        * Current channel state
        */
        TSensrvChannelState iState;

        /**
        * Callback proxy
        */
        CSensrvPluginProxy& iProxy;

        /**
        * Channel listener array for open clients
        * Listed listeners are owned.
        */
        RPointerArray<CSensrvChannelListener> iListenerList;

        /**
        * Channel buffer.
        * Own.
        */
        CSensrvChannelBuffer* iChannelBuffer;

        /**
        * Data provider residing in SSY thread heap.
        * Can safely be accessed only from SSY thread.
        * Not Own, owned by SSY.
        */
        MSsyChannelDataProvider* iDataProvider;

        /**
        * Sensor property provider residing in SSY thread heap.
        * Can safely be accessed only from SSY thread.
        * Not Own, owned by SSY.
        */
        MSsyPropertyProvider* iPropertyProvider;

        /**
        * Transaction for notifying new data from SSY to Server.
        * It is stored and reused to both avoid repeatedly
        * constructing and deleting it and to ensure it always
        * exists, as the transaction is initiated by SSY.
        * Own.
        */
        CSensrvTransaction* iDataAvailableTransaction;

        /**
        * Transaction for notifying buffer filled must be forced.
        * It is stored and reused to avoid repeatedly
        * constructing and deleting it.
        * Own.
        */
        CSensrvTransaction* iForceBufferFilledTransaction;

        /**
        * Transaction for notifying channel close must be forced.
        * It is stored and reused to  ensure it always
        * exists, as the transaction is initiated by SSY.
        * Own.
        */
        CSensrvTransaction* iForceChannelCloseTransaction;

        /**
        * Indicates outstanding explicit buffer fill request.
        */
        TBool iForcingBufferFilled;

        /**
        * Indicates maximum buffering count.
        */
        TInt iMaxBufferingCount;

        /**
        * Transaction for notifying property change from SSY to Server
        * Own.
        */
        CSensrvTransaction* iPropertyChangedTransaction;

        /**
        * Condition evaluator plugin.
        * Own.
        */
        CSensrvConditionEvaluator* iConditionEvaluator;

        /**
        * Channel info of this channel.
        */
        TSensrvResourceChannelInfo iChannelInfo;

        /**
        * The channel listener that has taken the property set control.
        * Not own.
        */
        CSensrvChannelListener* iCurrentSetPropertyControl;

        /**
        * Highest listener priority of the channel.
        */
        TInt iHighestListenerPriority;

        /**
        * Multiples of highest listener priority.
        * If there are no listeners on this channel, value is zero.
        * If there is single listener with the highest priority, value is one, etc.
        */
        TInt iHighestListenerPriorityMultiples;

    };



#endif // SENSERVERCHANNEL_H
