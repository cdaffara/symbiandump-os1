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
* Description:  Sensor server channel listener implemetation
*
*/


#ifndef SENSERVERCHANNELLISTENER_H
#define SENSERVERCHANNELLISTENER_H

#include <e32base.h>
#include "sensrvtimer.h"

class CSensrvChannel;
class CSensrvPluginProxy;
class CSensrvSession;
class TSensrvTailIndexItem;
class CSensrvTimer;
class CSensrvPropertyQueue;
class CSensrvConditionEvaluator;
class CSensrvChannelConditionSetImpl;

/**
* Number of data parst in TChannelDataParts
*/
const TInt KSensrvDataPartCount(2);

/**
* Implements channel listener related handling.
*
* @since S60 5.0
*/
class CSensrvChannelListener : public CBase, 
                               public MSensrvTimerCallback
    {
    public:
        
        /**
        * Channel listener state. State is a bitmask, as listener may be 
        * listening for both conditions and data simultaneously.
        * EChannelListenerStateInactive state is an exception, and it
        * may not be masked with other states.
        */
        enum TSensrvChannelListenerState
            {
            EChannelListenerStateInactive = 0,           ///< Client has channel open but is not listening for data
            EChannelListenerStateConditionListening = 1, ///< Client is listening for channel condition, but not data
            EChannelListenerStateDataListening = 2,      ///< Client is listening for data (can also be listening for condition)
            EChannelListenerStatePropertyListening = 4   ///< Client is listening for property changes (can also be listening for condition and/or data)
            };
         
         /**
         * Two part data holder
         */
         class TChannelDataParts
            {
            public:
                TChannelDataParts() 
                    {
                    }
                /**
                * First part of data. 
                */        
                TPtrC8 iFirstDataPart;
                
                /**
                * Second part of data.
                */
                TPtrC8 iSecondDataPart;
            };
            
         /**
         * Contains return message data for ESensrvSrvReqAsyncChannelData
         */   
         class TChannelDataMessage : public TChannelDataParts
            {
            public:
                TChannelDataMessage()
                    : iDataItemCount(0),
                      iDataLostCount(0)
                    {
                    }
                
                inline TBool IsEmpty() const { return ( !iDataItemCount && !iDataLostCount ); }
                
                /**
                * Count of data items contained in data parts
                */
                TInt   iDataItemCount;
                
                /**
                * Number of known lost data items
                */
                TInt   iDataLostCount;
            };

        class TPropertySetSuccessEvaluationData
            {
            public:
                TPropertySetSuccessEvaluationData()
                    : iHighestPriorityOfAffectedChannels(KMinTInt),
                      iMultiplesOnAffectedChannels(0),
                      iHighestPriorityOfChannel(KMinTInt),
                      iMultiplesOnChannel(0)
                    {
                    }
                
                TPropertySetSuccessEvaluationData(
                        TInt aHighestPriorityOfAffectedChannels,
                        TInt aMultiplesOnAffectedChannels,
                        TInt aHighestPriorityOfChannel,
                        TInt aMultiplesOnChannel )
                    : iHighestPriorityOfAffectedChannels(aHighestPriorityOfAffectedChannels),
                      iMultiplesOnAffectedChannels(aMultiplesOnAffectedChannels),
                      iHighestPriorityOfChannel(aHighestPriorityOfChannel),
                      iMultiplesOnChannel(aMultiplesOnChannel)
                    {
                    }
                /**
                * The highest priority of all affected channels
                */
                TInt iHighestPriorityOfAffectedChannels;
                
                /**
                * The multiples of highest priority of all affected channels
                */
                TInt iMultiplesOnAffectedChannels;
                
                /**
                * The highest priority of this channel
                */
                TInt iHighestPriorityOfChannel;
                
                /**
                * The multiples of highest priority of this channel
                */
                TInt iMultiplesOnChannel;
            };

        /**
        * Two phase constructor
        *
        * @since S60 5.0
        * @param aSession Related client session.
        * @param aChannel Parent channel.
        * @param aProxy Plugin proxy.
        * @return New CSensrvChannelListener instance
        * @exception KErrNoMemory Out of memory
        */
        static CSensrvChannelListener* NewL(CSensrvSession& aSession,
                                            CSensrvChannel& aChannel,
                                            CSensrvPluginProxy& aProxy);

        /**
        * Destructor.
        */
        virtual ~CSensrvChannelListener();
        
        /**
        * Gets session related to this listener.
        *
        * @since S60 5.0
        * @return Session pointer. 
        */
        inline CSensrvSession& Session() const { return iSession; };

        /**
        * Gets listener state.
        *
        * @since S60 5.0
        * @return Current listener state
        */
        inline TInt State() const { return iState; };

        /**
        * Gets listener desired buffering count.
        *
        * @since S60 5.0
        * @return Desired listener buffering count
        */
        inline TInt DesiredBufferingCount() const { return iDesiredBufferingCount; };

        /**
        * Gets listener maximum buffering count.
        *
        * @since S60 5.0
        * @return Maximum listener buffering count
        */
        inline TInt MaximumBufferingCount() const { return iMaximumBufferingCount; };

        /**
        * Starts listening actively for data and/or conditions. 
        * Buffering period and buffering count are same for 
        * both condition and data listening. If listener is doing
        * both, lower values for each are used.
        *
        * @since S60 5.0
        * @param TInt aDesiredBufferingCount Desired buffering count 
        *             specified by the client. If zero, negative or over 
        *             allowed maximum, the maximum count is used.
        * @param TInt aMaximumBufferingCount Maximum buffering count
        *             specified by the client. If less than desired count 
        *             or over allowed maximum, the maximum count is used.
        * @param TInt aBufferingPeriod  Buffering period specified by
        *             the client. If zero, negative or over 
        *             allowed maximum, the maximum period is used.
        *             In milliseconds.
        * @param aState Indicates if starting listening for data or
        *        (EChannelListenerStateDataListening) or conditions 
        *        (EChannelListenerStateConditionListening).
        * @return Symbian error code. 
        *         KErrArgument if both period and count are zero.
        */
        TInt StartListening(TInt aDesiredBufferingCount,
                            TInt aMaximumBufferingCount,
                            TInt aBufferingPeriod,
                            TSensrvChannelListenerState aState);

        /**
        * Stops listening actively for data and/or conditions. 
        *
        * @since S60 5.0
        * @param aState Indicates if stopping listening for data or
        *        (EChannelListenerStateDataListening) or conditions 
        *        (EChannelListenerStateConditionListening).
        * @param aDeleteConditions If ETrue, indicates that any
        *        added conditions should also be deleted.
        *        This is only evaluated if aState is 
        *        EChannelListenerStateConditionListening.
        */
        void StopListening(TSensrvChannelListenerState aState,
                           TBool aDeleteConditions);
        
        /**
        * Notifies listener that there is new data available in buffer.
        *
        * @since S60 5.0
        * @return Symbian error code. If listener is not active, will
        *         always return KErrNone.
        */
        TInt NewDataAvailable();
        
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
        * Gets the remaining buffering count still required to fill data
        * request from client.
        *
        * @since S60 5.0
        * @return Remaining buffering count.
        */
        TInt RemainingBufferingCount();
        
    // From MSensrvTimerCallback
    
        /** 
        * Called when the timer fires
        *
        * @since S60 5.0
        * @param aTimerId Timer identifier to id which timer fired.
        */
        void TimerFired(TInt aTimerId);
        
        
        /**
        * Stores changed property to the property queue.
        * Called from SSY thread only.
        *
        * @since S60 5.0
        * @return Symbian error code. 
        */
        TInt StoreChangedProperty(const TSensrvProperty& aProperty);

        /**
        * Notifies client about any changed properties in property queue.
        *
        * @since S60 5.0
        */
        void HandleNextProperty();
        
        /**
        * Get the priority of the client process which this listener is serving.
        *
        * @since S60 5.0
        * @return Priority of the process.
        */
        TInt Priority();
        
        /**
        * Read the priority of the process which this listener is serving from cenrep.
        *
        * @since S60 5.0
        * @return Priority of the process.
        */                   
        TInt ReadPriority();
        
        /**
        * Handles the property set success status update and client notification
        *
        * @since S60 5.0
        * @param[in] aEvaluationData The evaluation data for the indication logic.
        */
        void HandleSetPropertySuccessInd( const TPropertySetSuccessEvaluationData& aEvaluationData );
        
    private:

        /**
        * C++ constructor
        *
        * @since S60 5.0
        * @param aSession Related client session.
        * @param aChannel Parent channel.
        * @param aProxy Plugin proxy.
        */
        CSensrvChannelListener(CSensrvSession& aSession,
                               CSensrvChannel& aChannel,
                               CSensrvPluginProxy& aProxy);

        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL();

        /**
        * Sets data to message to be returned to client.
        * 
        * @since S60 5.0
        * @param aData Data to set to the message. 
        * @return KErrBadHandle Message is NULL.
        *         KErrBadDescriptor Message parameters are not descriptor type.
        *         KErrOverflow Message descriptor too small.
        */
        TInt SetMessageData(TChannelDataMessage& aData);
        
        /**
        * Completes asynchronous condition message.     
        * Message must exist.   
        * 
        * @since S60 5.0
        * @param aSet Set that was met. There must be met data in this set.
        *        The set is removed from met sets list and deleted if
        *        completion of message is successful. Otherwise
        *        the set remains in the list.
        */
        void CompleteConditionMessageD(CSensrvChannelConditionSetImpl& aSet);
        
        /**
        * Evaluates conditions for new data.
        *
        * @since S60 5.0
        */
        void EvaluateConditionsForNewData();
        
        /**
        * Clears condition queues.
        *
        * @since S60 5.0
        */
        void ClearConditionQueues();
        
        /**
        * Completes the property set success notification to client.
        *
        * @since S60 5.0
        */
        void CompleteSetPropertySuccessInd();
        
        /**
        * Handles the property set success status logic
        *
        * @since S60 5.0
        * @param[in] aEvaluationData The evaluation data for the indication logic.
        * @return New success indicator value.
        */
        TSetPropertySuccessIndicator EvaluatePriorities( const TPropertySetSuccessEvaluationData& aEvaluationData );
        
    private: // Member variables
    
        /**
        * Related client session.
        */
        CSensrvSession& iSession;
        
        /**
        * Parent channel.
        */
        CSensrvChannel& iChannel;
        
        /**
        * Callback proxy
        */
        CSensrvPluginProxy& iProxy;

        /**
        * Current channel listener state
        */
        TInt iState;
        
        /**
        * Listener desired buffering count
        */
        TInt iDesiredBufferingCount;
        
        /**
        * Listener maximum buffering count
        */
        TInt iMaximumBufferingCount;

        /**
        * Listener desired buffering period, in microseconds.
        */
        TTimeIntervalMicroSeconds32 iBufferingPeriod;
        
        /**
        * Indicates buffering period has expired,
        * so any amount of data is enough.
        */
        TBool iBufferingPeriodExpired;
        
        /**
        * Tail index item.
        * Not own, owned by channel buffer
        */
        TSensrvTailIndexItem* iTailItem;
        
        /**
        * Pending message for sending data to client
        * Not own.
        */
        CSensrvMessage* iClientDataMessage;
        
        /**
        * Pending message for sending changed property data to client
        * Not own.
        */
        CSensrvMessage* iClientPropertyMessage;
        
        /**
        * Pending message for sending condition met notifications to client
        * Not own.
        */
        CSensrvMessage* iClientConditionMessage;
        
        /**
        * Buffering period timer
        * Own.
        */
        CSensrvTimer* iBufferingPeriodTimer;
        
        /**
        * Stores changed properties informed by channel until they can be handled.
        */
        CSensrvPropertyQueue* iUnnotifiedProperties;
        
        /**
        * Condition evaluator.
        * Not own.
        */
        CSensrvConditionEvaluator* iConditionEvaluator;
        
        /**
        * List of added condition sets
        * Sets in list owned.
        */
        TSglQue<CSensrvChannelConditionSetImpl> iAddedConditionSetList;

        /**
        * Iterator for condition list
        */        
        TSglQueIter<CSensrvChannelConditionSetImpl> iAddedConditionSetIter;

        /**
        * List of met condition sets that are not yet notified.
        * Sets in list owned.
        */
        TSglQue<CSensrvChannelConditionSetImpl> iMetConditionSetList;

        /**
        * Iterator for condition list
        */        
        TSglQueIter<CSensrvChannelConditionSetImpl> iMetConditionSetIter;
        
        /**
        * Priority of the client process.
        * KErrUnknown means that the priority is not yet fetched from CentralRepository.
        */
        TInt iPriority;
        
        /**
        * Indicates if initial data has been sent
        */
        TBool iInitialDataSent;
        
        /**
        * Indicates pending property set success notification
        */ 
        TBool iSetPropertySuccessIndPending;
        
        /**
        * The current status of property set success indicator
        */         
        TSetPropertySuccessIndicator iSetPropertySuccessInd;
    };



#endif // SENSERVERCHANNELLISTENER_H
