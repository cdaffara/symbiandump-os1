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
* Description:  Provides methods to open and control a sensor channel.
*
*/




#ifndef SENSRVCHANNELIMPL_H
#define SENSRVCHANNELIMPL_H

#include <e32base.h>
#include "sensrvchannel.h"
#include "sensrvtypes.h"
#include "sensrvclient.h"
#include "sensrvdatahandlerobserver.h"
#include "sensrvpropertyhandlerobserver.h"
#include "sensrvconditionhandlerobserver.h"


class MSensrvPropertyListener;
class CSensrvPropertyHandler;
class MSensrvDataListener;
class CSensrvDataHandler;
class MSensrvConditionListener;
class CSensrvConditionHandler;
class CSensrvChannelConditionSetImpl;


/**
 *  Realizes a sensor channel. Provides methods to open and control a sensor channel.
 *  Class provides operations to initiates a data transmittion,
 *  set channel conditions and property providers.
 *
 *  @lib SensrvClient.lib
 *  @since S60 5.0
 */

NONSHARABLE_CLASS(CSensrvChannelImpl) : public CSensrvChannel,
                                        public MSensrvDataHandlerObserver,
                                        public MSensrvPropertyHandlerObserver,
                                        public MSensrvConditionHandlerObserver
    {
    
    friend class CSensrvChannel;
    
    enum THandlerId
        {
        EDataHandler,
        EPropertyHandler,
        EConditionHandler,
        EStartDataListening,
        EStartConditionListening
        };
    
    public:
        /**
        * Destructor.
        */
        virtual ~CSensrvChannelImpl();

    public:
        /**
        * Open and initiates the channel. After a channel is opened a client can 
        * set and get channel properties, add channel conditions and the listen 
        * sensor data from a sensor.
        *
        * @since S60 5.0
        */
        virtual void OpenChannelL();

        /**
        * Closes the channel.
        *
        * @since S60 5.0
        * @return TInt KErrNone if channel closing successful
        */
        virtual TInt CloseChannel();

        /**
        * Returns a channel info object related to this channel.
        *
        * @since S60 5.0
        * @return TSensrvChannelInfo& Reference to channel info object
        */
        virtual const TSensrvChannelInfo& GetChannelInfo() const;

        /**
        * Start the channel data listening. Channel data is received into the
        * receiving buffer and it can be read using the GetData-function. When new
        * data is available in the receiving buffer, a notification is delivered
        * via the data listener callback interface (MSensrvDataListener).
        *
        * Note that if channel is listening for both data and conditions, the
        * lower desired count and buffering period defined in start calls will be
        * used for both.
        *
        * For best performance, always use as high desired and maximum counts as 
        * the use case allows to reduce overhead caused by IPC calls.
        * However, keep in mind that higher counts reserve more memory.
        *
        * @since S60 5.0
        * @param[in] aDataListener Data listener for the channel. Must be valid
        *        until data listening is stopped.
        * @param[in] aDesiredCount Desired receiving buffer size defined in a 
        *        count of the channel data objects. Maximum value is determined 
        *        by sensor server internal channel buffer size, and minimum value 
        *        is one. Any value outside these limits is interpreted as maximum 
        *        possible value. Actual count of channel data objects received 
        *        each time may be smaller than aDesiredCount, if a non-zero 
        *        buffering period has been specified, or larger, if aMaximumCount 
        *        is bigger than aDesiredCount. If sensor supplies data sporadically, 
        *        and every data object is interesting, aDesiredCount should be set 
        *        to one to ensure that no data objects are missed.
        * @param [in] aMaximumCount Maximum receiving buffer size defined in a 
        *        count of the channel data object. Maximum value is determined by
        *        sensor server internal channel buffer size, and minimum value is 
        *        aDesiredCount. Any value outside these limits is interpreted as 
        *        maximum possible value. It is recommended that whenever 
        *        aDesiredCount needs to be low and sensor has relatively high 
        *        possible data rate, aMaximumCount is set to significantly higher 
        *        than aDesiredCount to reduce IPC traffic and data loss.
        * @param[in] aBufferingPeriod Specifies the maximum time in milliseconds
        *        that sensor server waits between updating new data into the
        *        receiving buffer. It is possible that no new data is available
        *        when notification comes. Zero timeout means that only desired 
        *        and maximum counts are used in getting data.
        *        Using buffering period results in lot of unnecessary IPC traffic,
        *        if sensor only supplies data sporadically, so it is recommended 
        *        to be used only when it is absolutely vital to have data at 
        *        specific intervals and using low desired count is too expensive 
        *        performance-wise.
        */
        virtual void StartDataListeningL( MSensrvDataListener* aDataListener,
                                          const TInt aDesiredCount,
                                          const TInt aMaximumCount,
                                          const TInt aBufferingPeriod );

        /**
        * Stop channel data listening.
        *
        * @since S60 5.0
        * @return TInt KErrNone if stopping successful
        */
        virtual TInt StopDataListening();

        /**
        * Get next data item. A client can use this function to get a received 
        * data from the data buffer when a data listening is on.
        *
        * @since S60 5.0
        * @param[out] aData Reference to descriptor where data should be copied 
        *              to. Typically this is a package buffer defined for data 
        *              type of this channel.
        * @return TInt KErrNotFound if data buffer is empty.
        *              KErrBadDescriptor if aData is wrong size for data item.
        */
        virtual TInt GetData( TDes8& aData );

        /**
        * Set a property. Set property to the sensor. If the property is not 
        * valid error code will be returned. Property ID and item index related 
        * to the operation are added to the property parameter itself.
        *
        * @since S60 5.0        
        * @param[in] aProperty New property to be set. Contains also information 
        *         about property ID and itemindex.
        * @return TInt KErrNotFound Property ID is not found
        *              KErrAccessDenied A client has no right or priority to 
        *                      change the property value.
        */
        virtual TInt SetProperty(  const TSensrvProperty& aProperty);

        /**
        * Get the channel property value.
        *
        * @since S60 5.0        
        * @param[in] aPropertyId Id of the property to be get
        * @param[in] aItemIndex Item index of the property. Mapped to 
        *         a specific item in channel data structure.
        * @param[out] aProperty Reference to property value to be get.
        */
        virtual void GetPropertyL( const TSensrvPropertyId aPropertyId,
                                   const TInt aItemIndex,
                                   TSensrvProperty& aProperty );
                                   
        /**
        * Get a channel property value. 
        *
        * @since S60 5.0
        * @param[in] aPropertyId Id of the property to be get
        * @param[in] aItemIndex Item index of the property. Mapped to a specific 
        *            item in channel data structure.
        * @param[in] aArrayIndex Array index of the property. If property is an array
        *            parameter references to array index.
        * @param[out] aProperty Reference to a property object where property data 
        *             is copied to.
        * @leave KErrNotFound - No open channel with that id for this client found,
        *                       or property does not exist.
        */  
        virtual void GetPropertyL( const TSensrvPropertyId aPropertyId,
                                   const TInt aItemIndex,
                                   const TInt aArrayIndex,
                                   TSensrvProperty& aProperty );

        /**
        * Get all channel properties.
        *
        * @since S60 5.0        
        * @param[out] aChannelPropertyList List of the properties the channel have.
        */
        virtual void GetAllPropertiesL( RSensrvPropertyList& aChannelPropertyList );
        
        /**
        * Get all channel properties. 
        *
        * @since S60 5.0
        * @param[in] aPropertyId Id of the property to be get
        * @param[out] aChannelPropertyList List of all properties the channel has. 
        *    Any old data in list will be lost.
        * @leave KErrNotFound - No open channel with that id for this client found
        *                       or property does not exist.
        */  
        virtual void GetAllPropertiesL(
                     const TSensrvPropertyId aPropertyId, 
                     RSensrvPropertyList& aChannelPropertyList );

        /**
        * Set property listener. A client can use a property listener to get 
        * notification about a changed property. If a client itself changes 
        * a property value notification is not received. Note that changing 
        * a property value may effect to other properties on the channel and 
        * notification about these is occured.
        *
        * @since S60 5.0        
        * @param[in] aChannelPropertyListener Pointer to channel property listener
        */
        virtual void SetPropertyListenerL( 
                     MSensrvPropertyListener* aChannelPropertyListener);

        /**
        * Add a channel condition set for a channel. Added channel condition 
        * will be used in the channel condition listening. A client has ownership 
        * to added object and CSensrvChannel will not destroy 
        * CSensrvChannelConditionSet-object in any case. Client has to ensure 
        * the CSensrvChannelConditionSet-object is valid until the condition
        * set is removed from the channel or the channel is destroyed.
        *
        *
        * @since S60 5.0
        * @param[in] aChannelConditionSet Channel condition to be added
        */
        virtual void AddConditionL( 
                     CSensrvChannelConditionSet& aChannelConditionSet );

        /**
        * Remove channel condition set. Removed channel condition set is not 
        * used anymore in the channel condition listening. A client have to 
        * take care of destroying channel condition set object.
        *
        * @since S60 5.0
        * @param[in] aChannelConditionSet Channel condition set to be removed
        */
        virtual void RemoveConditionL( 
                     CSensrvChannelConditionSet& aChannelConditionSet );

        /**
        * Set and start channel condition listener. If any one of the channel 
        * conditions is met callback function in listener object is called.
        *
        * Note that if channel is listening for both data and conditions, 
        * the lower object count and buffering period defined in start calls 
        * will be used for both.
        *
        * @since S60 5.0
        * @param[in] aChannelConditionListener Channel condition listener to be set.
        *    The listener must be valid until condition listening is stopped.
        * @param[in] aObjectCount Listening buffer size defined in a count of the 
        *        channel objects. Each channel condition is evaluated at once 
        *        for a block which size is an aObjectCount. Maximum count is 
        *        determined by sensor server internal channel buffer size, and 
        *        minimum count is one. Any count outside these limits is interpreted
        *        as maximum possible count.
        * @param[in] aBufferingPeriod Specifies the maximum time in milliseconds 
        *        that sensor server waits between getting new data to evaluate 
        *        conditions against. Zero timeout means that only aObjectCount 
        *        is used in getting data.
        */
        virtual void StartConditionListeningL( 
                     MSensrvChannelConditionListener* aChannelConditionListener,
                     const TInt aObjectCount,
                     const TInt aBufferingPeriod );

        /**
        * Stop condition listening.
        *
        * @since S60 5.0
        * @return TInt KErrNone if stopping successful
        */
        virtual TInt StopConditionListening();

    public: // From MSensrvDataHandlerObserver
        /**
        * Indicates data is available in the receiving buffer
        *
        * @since S60 5.0
        * @param aCount Data count in receiving buffer
        * @param aDataLost Data lost count
        */
        void DataReceived( TInt aCount, TInt aDataLost );

        /**
        * Data listening failed.
        * If error was fatal, channel has also been closed and sensor server 
        * session terminated.
        * If error was minor, some data has potentially been lost.
        *
        * @since S60 5.0        
        * @param aError Error code.
        */
        void DataError( TSensrvErrorSeverity aError );

        /**
        * Reopen channel.
        * Property listening and condition listening need restarting too
        *
        * @since S60 5.0        
        * @param aMaxBufferCount New maximum client buffer count
        */
        void ReopenChannelForDataListeningL( TInt& aMaxBufferCount );

    public: //From MSensrvPropertyHandlerObserver

        /**
        * Notification about the changed value of a property.
        *
        * @since S60 5.0
        * @param[in] aChangedProperty Changed property.
        */
        void PropertyChanged( const TSensrvProperty& aChangedProperty );

        /**
        * Property listening failed.
        * If error was fatal, channel has also been closed and sensor server 
        * session terminated.
        * If error was minor, some property changes have potentially been missed.
        *
        * @since S60 5.0
        * @param[in] aError Error code.
        */
        void PropertyError( TSensrvErrorSeverity aError );

        /**
        * Reopen channel.
        * Data listening and condition listening need restarting
        *
        * @since S60 5.0        
        * @param aMaxBufferCount New maximum client buffer count
        */
        void ReopenChannelForPropertyListeningL();
        
        /**
        * Notification about change in property setting avaibility.
        *
        * @since S60 5.0
        * @param[in] aIndication holds the new indicator value.
        */
        void SetPropertySuccessIndicationChanged( TSetPropertySuccessIndicator aIndication );
        
    public: //From MSensrvConditionHandlerObserver

        /**
        * Notification about the condition met.
        *
        * @since S60 5.0
        * @param[in] aSetId Condition set id that was met.
        * @param[in] aValue The data value that caused meeting of this condition.
        */
        void ConditionMet( TInt aSetId, 
                           TDesC8& aValue );

        /**
        * Condition listening failed.
        * If error was fatal, channel has also been closed and sensor server 
        * session terminated.
        * If error was minor, some condition meetings have potentially been missed.
        *
        * @since S60 5.0
        * @param[in] aError Error code.
        */
        void ConditionError( TSensrvErrorSeverity aError );

        /**
        * Reopen channel.
        * Data listening and property listening need restarting
        *
        * @since S60 5.0        
        */
        void ReopenChannelForConditionListeningL();

    private:

        /**
        * C++ constructor
        *
        * @since S60 5.0
        */
        CSensrvChannelImpl( const TSensrvChannelInfo& aChannelInfo );

        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL();

        /**
        * Open Channel.
        * Must reset listeners here instead of close channel 
        * because listener pointer is needed after fatal error triggered
        * closing of channel to notify this fatal error to listener.
        *
        * @param aResetListeners ETrue set listeners to NULL,
        *        and remove all conditions.
        */
        void DoOpenChannelL( TBool aResetListeners );
        
        /**
        * Reopen channel for listening when prompted by a handler
        *
        * @param aInitiator The listener than prompted this
        */
        void ReopenChannelForListeningL( THandlerId aInitiator );

    private: // Data

        /**
        * Channel Id
        */
        TSensrvChannelInfo iChannelInfo;

        /**
        * Max buffer count
        */
        TInt iMaxBufferCount;

        /**
        * Data listening parameters
        */
        TSensrvStartListeningParameters iDataListeningParameters;

        /**
        * Sensor server client
        */
        RSensrvClient iSensrvClient;

        /**
        * Data handler
        * Own        
        */
        CSensrvDataHandler* iDataHandler;

        /**
         * Data listener
         * Not own        
         */
        MSensrvDataListener* iDataListener;

        /**
         * channel open flag
         */ 
        TBool iChannelOpen;

        /**
         * Property handler
         * Own         
         */
        CSensrvPropertyHandler* iChannelPropertyHandler;

        /**
         * Property listener
         * Not own         
         */
        MSensrvPropertyListener* iChannelPropertyListener;

        /**
        * Condition handler
        * Own         
        */
        CSensrvConditionHandler* iConditionHandler;

        /**
        * Condition listener
        * Not own         
        */
        MSensrvChannelConditionListener* iConditionListener;
        
        /**
        * List of added condition sets
        * Sets in list are not owned.
        */
        TSglQue<CSensrvChannelConditionSetImpl> iConditionSetList;

        /**
        * Iterator for condition list
        */        
        TSglQueIter<CSensrvChannelConditionSetImpl> iConditionSetIter;

        /**
        * Condition set id counter
        */
        TInt iConditionSetIdCounter;
    
    };


#endif //SENSRVCHANNELIMPL_H