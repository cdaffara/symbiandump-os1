/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor channel class definition
*
*/


#ifndef SENSRVCHANNEL_H
#define SENSRVCHANNEL_H

#include <e32base.h>
#include <sensrvtypes.h>
#include <sensrvchannelconditionset.h>
#include <sensrvchannelinfo.h>


class MSensrvPropertyListener;
class MSensrvDataListener;
class MSensrvChannelConditionListener;
class CSensrvChannelCondition;
class CSensrvChannelListener;


/**
* The CSensrvChannel class provides an API that allows clients to open, control, configure, 
* receive data and receive notifications from a sensor channel.
* 
* When instantiating CSensrvChannel an instance of TSensrvChannelInfo returned by
* CSensrvChannelFinder::FindChannelsL() must be used. This ensures that internal data used
* in identifying the chosen channel is passed back to Sensor Services in its entirety
* therefore ensuring correct operation. Not using the instance returned by
* CSensrvChannelFinder::FindChannelsL() will result in undefined behaviour.
* 
* On instantiation of this class a session connection to the Sensor Server is made. The Sensor
* server is a transient server and is started, if not already running, when a session connection
* is made. The sensor server is shutdown when a device-configured timeout period expires after
* the last session connection has been dropped. The Sensor Server will not shutdown if there is
* a session connection to it.
* 
* The instance of TSensrvChannelInfo returned by CSensrvChannelFinder::FindChannelsL()
* contains a channel Id which is only valid for the lifetime of the sensor server. If a client
* requires that this id remains unchanged then the client must ensure that an instance of either
* CSensrvChannelFinder or CSensrvChannel remains instantiated. This keeps a session connection with
* the server open which prevents the sensor server from shutting down.
* 
* After creating an instance of this class a client would typically:
* 1. Check the configuration of the channel using GetPropertyL() or GetAllPropertuies().
* 2. Change the configuration where necessary using SetPropertiesL()
* 3. If SetPropertiesL() failed, because of higher priority clients of this same channel or higher
* priority clients of impacted channels on the same sensor, listen for notifications indicating
* when it is able to set properties using SetPropertyListenerl().
* 4. Listen for property changes using SetPropertyListenerl(). This notifies the client of property
* changes made by other clients of this same channel.
* 5. Listen for data and retrieve data on the channel using StartDataListeningL() and GetData().
* 6. Set trigger conditions for a 1 off notification when the condition is met using AddConditionL()
* and SetConditionListeningL(). 
* 
* Its possible that there may be more than 1 client using the same sensor channel on the device. Each
* of these clients will have created their own instance of CSensrvChannel and can attempt to configure
* the sensor using the SetPropertyL() operation. To manage any contention between clients wishing to
* change the configuration of the sensor a concept of Client Priorities is employed. This means that a
* device creator assigns a pre-determined priority to a client. If a client has not been assigned a
* priority then it is assumed it has the lowest priority.
* 
* When the SetPropertyL() operation is called the sensor server checks to see if the client has the
* highest Client Priority of all clients using that channel or using an impacted channel on the same
* sensor. If it has the property is set and all other clients of the channel are notified of the property
* change. If it has not the SetPropertyL() operation fails with KErrAccessDenied. If clients have
* the same priority then property setting is done on a first come first served basis.
* 
* If a client is unable to set a property because of higher priority clients then the client should
* implement the MSensrvPropertyListener::SetPropertySuccessIndicationChanged() callback so that they can
* be notified when they are able to set properties.
* 
* All client applications should be designed in such a way so that they are able to handle the situation
* where they are unable to configure a channel using the SetPropertyL() operation.
* 
* For example:
* Should application features be disabled or should the application shutdown in such conditions?
* Should features be disabled under certain channel configurations?
* 
* This object is closed when deleted either by a call to operator delete or by the cleanup stack. No special
* cleanup code is necessary to close the object before deletion.
*
* @see CSensrvChannelFinder
* @lib sensrvclient.lib
* @since S60 5.0
*/

NONSHARABLE_CLASS( CSensrvChannel ): public CBase
    {
    public:
    /**
    * Two-phased constructor.
    *
    * The Sensor Server uses aChannelInfo to identify the channel in the server that the returned object
    * represents.
    *
    * @since S60 5.0
    * @param  aChannelInfo The information that determines which channel object is created. This must
    *         be an object returned by CSensrvChannelFinder::FindChannelsL().
    * @return Pointer to created CSensorChannel object
    * @leave  KErrNoMemory
    * @leave  One of the system-wide error codes
    */  
    IMPORT_C static CSensrvChannel* NewL
            ( const TSensrvChannelInfo& aChannelInfo );

    /**
    * Two-phased constructor.      
    * 
    * The Sensor Server uses aChannelInfo to identify the channel in the server that the returned object
    * represents.
    * 
    * @since S60 5.0
    * @param  aChannelInfo The information that determines which channel object is created. This must
    *         be an object returned by CSensrvChannelFinder::FindChannelsL().
    * @return Pointer to created CSensorChannel object that is placed on the cleanup stack.      
    * @leave  KErrNoMemory
    * @leave  One of the system-wide error codes
    */  
    IMPORT_C static CSensrvChannel* NewLC
            ( const TSensrvChannelInfo& aChannelInfo );
  
    public:
    /**
    * Initializes and opens the channel. After the channel is opened, the client can set and get channel
    * properties, add channel conditions and listen for sensor data.
    * 
    * @since S60 5.0
    * @leave  KErrAlreadyExists if channel is already open
    * @leave  One of the system-wide error codes
    */
    virtual void OpenChannelL() = 0;

    /**
    * Closes the channel. 
    * 
    * This object is closed when deleted either by a call to operator delete or by the cleanup stack. No special
    * cleanup code is necessary to close the object before deletion.
    * 
    * @since S60 5.0
    * @return KErrNone if channel closing is successful, or one of the system-wide error codes
    */  
    virtual TInt CloseChannel() = 0;
    
    /**
    * Retrieves a channel information object related to this channel.
    *     
    * @since S60 5.0
    * @return Reference to the channel information object
    */  
    virtual const TSensrvChannelInfo& GetChannelInfo() const = 0;

    /**
    * Starts channel data listening on the channel.
    * 
    * To use this operation clients must provide an object that inherits from MSensrvDataListener.  
    * 
    * When new data is available to be read, the client is notified by the
    * MSensrvDataListener::DataReceived() callback. On receiving this notificaton the client should
    * retrieve the data using the GetData() operation.
    * 
    * Note that if a client is listening for both data and conditions on the channel then the lower
    * desired count and buffering period provided in either start listening operation arguments will be
    * used for both listeners. When either listener is stopped neither the desired count nor the
    * buffering period shall be changed.
    * 
    * For the best performance, always use the highest desired count and maximum count as the use case
    * allows. This will reduce the overhead caused by a high volume of IPC context switches. However,
    * be aware that higher counts need to reserve more memory.
    * 
    * @since S60 5.0
    * @param  aDataListener Data listener for the channel. Must be valid until data listening is stopped. 
    * @param  aDesiredCount Desired receiving buffer size defined as a count of the channel data objects.
    *         The maximum value is determined by sensor server internal channel buffer size, and minimum
    *         value is one. Any value outside these limits is interpreted as maximum possible value.
    *         The actual count of channel data objects received may be smaller than aDesiredCount, if a
    *         non-zero buffering period has been specified.
    *         The actual count of channel data objects received may larger, if aMaximumCount is bigger
    *         than aDesiredCount.
    *         If the sensor supplies data sporadically, and the clients requires every data object then
    *         aDesiredCount should be set to one to ensure that no data objects are missed. 
    * @param  aMaximumCount Maximum receiving buffer size defined as a count of the channel data object.
    *         The Maximum value is determined by sensor server internal channel buffer size, and minimum
    *         value is aDesiredCount. Any value outside these limits is interpreted as maximum possible
    *         value.
    *         It is recommended that whenever aDesiredCount needs to be low and sensor has relatively high
    *         data rate, aMaximumCount is set to significantly higher than aDesiredCount to reduce IPC
    *         context switches and data loss.
    * @param  aBufferingPeriod Specifies the maximum time in milliseconds that sensor server waits before
    *         notifying the client that new data is available. It is possible that no new data is will be
    *         available when the client is notified.
    *         Zero timeout means that only desired and maximum counts are used in getting data.
    *         Using a buffering period results in lot of unnecessary IPC context switches if a sensor
    *         channel supplies data sporadically. It is therefore recommended that a buffering period is
    *         used only when it is absolutely vital to have data at specific intervals and using a low
    *         desired count is otherwise too expensive performance-wise.
    * @leave  KErrArgument if aDataListener is NULL, or if a negative argument is supplied
    * @leave  KErrNotFound if the channel is not open
    * @leave  KErrAlreadyExists if data listening has already started on the channel
    * @leave  One of the system-wide error codes
    */  
    virtual void StartDataListeningL( MSensrvDataListener* aDataListener, 
                                      const TInt aDesiredCount, 
                                      const TInt aMaximumCount,
                                      const TInt aBufferingPeriod ) = 0;

    /**
    * Stop channel data listening. 
    * 
    * @since S60 5.0
    * @return KErrNone if stopping successful or one of the system-wide error codes
    */  
    virtual TInt StopDataListening() = 0;

    /**
    * Get data items when the client is notified by the MSensrvDataListener::DataReceived() callback
    * that new data is ready to be retrieved. The callback also indicates the number of data items
    * contained in aData. 
    * 
    * Example:
    *     
    * How to retrieve data using TPckg
    * 
    * @code
	  * void DataReceived(CSensrvChannel& aChannel, TInt aCount, TInt aDataLost )
		* {
    * if(aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdAccelerometerXYZAxisData)
		* 	{
		* 	TSensrvAccelerometerAxisData XYZData;
	  *       TPckg<TSensrvAccelerometerAxisData> XYZPackage( XYZData );
	  *       aChannel.GetData( XYZPackage );
		* 	}
		* }
    * @endcode
    *  
    * @since S60 5.0
    * @param  aData Reference to descriptor that contains the data. Typically this is a package buffer
    *         defined for data type of this channel. See channel specific data headers for details of
    *         the package types and data supported by a channel.
    * @return KErrNotFound if data buffer is empty, KErrOverflow if aData is wrong size for data
    *         item, or one of the system-wide error codes
    */  
    virtual TInt GetData( TDes8& aData ) = 0;
 
    /**
    * Sets a property value for this channel. To manage any contention between multiple clients wishing
    * to change the configuration of the sensor a concept of Client Priorities is employed. See class
    * description for further information.
    * 
    * The sensor plugin may police setting of properties by verifying that the client making the request
    * has the necessary capabilities. If the client does not then it will return with KErrAccessDenied.
    * @since S60 5.0
    * @param  aProperty Identifies the property that is to be set and its new value. Only the Property Id,
    *         Item Index and Property Value should be filled in. All other fields in TSensrvProperty are
    *         ignored.
    * @return KErrNotFound if channel is not open or if the property does not exist, KErrAccessDenied
    *         if there are other clients for this channel or impacted channel on the same sensor with a
    *         higher Client Priority, KErrAccessDenied if the client does not have the required
    *         capabilities or one of the system-wide error codes
    */  
    virtual TInt SetProperty(  const TSensrvProperty& aProperty ) = 0;
      
    /**
    * Gets a channel property value for this channel.
    * 
    * @since S60 5.0
    * @param  aPropertyId Id of the property that is to be retrieved
    * @param  aItemIndex The Item Index of the property to be retrieved. This is mapped to a specific item
    *         in channel data structure. See channel specific data headers.
    * @param  aProperty Reference for a property object where property data is to be copied
    * @leave  KErrNotFound if channel is not open
    * @leave  KErrNotFound if property does not exist
    * @leave  KErrArgument if a negative argument is supplied
    * @leave  One of the system-wide error codes
    * */
    virtual void GetPropertyL( const TSensrvPropertyId aPropertyId, 
                               const TInt aItemIndex,
                               TSensrvProperty& aProperty ) = 0;
                               
    /**
    * Get a channel property value for this channel
    * 
    * @since S60 5.0
    * @param  aPropertyId Id of the property that is to be retrieved
    * @param  aItemIndex Item Index of the property to be retrieved. This is mapped to a specific item
    *         in channel data structure. See channel specific data headers.
    * @param  aArrayIndex Array index of the property to be retrieved when the property is part of a
    *         property array.
    * @param  aProperty Reference for a property object where property data is to be copied
    * @leave  KErrNotFound if channel is not open
    * @leave  KErrNotFound if property does not exist
    * @leave  KErrArgument if a negative argument is supplied
    * @leave  One of the system-wide error codes
    */  
    virtual void GetPropertyL( const TSensrvPropertyId aPropertyId, 
                               const TInt aItemIndex,
                               const TInt aArrayIndex,
                               TSensrvProperty& aProperty ) = 0;

    /**
    * Get all properties for this channel
    * 
    * @since S60 5.0
    * @param  aChannelPropertyList Reference for a property list where all property data is to be
    *         copied. Any existing data in the list will be lost.
    * @leave  KErrNotFound if channel is not open
    * @leave  One of the system-wide error codes
    */  
    virtual void GetAllPropertiesL
            ( RSensrvPropertyList& aChannelPropertyList ) = 0;
            
    /**
    * Get all channel properties having the supplied Property Id. This is used when the property
    * is an array property.    
    * 
    * @since S60 5.0
    * @param  aPropertyId Id of the property to be retrieved
    * @param  aChannelPropertyList Reference for a property list where all property data is to be
    *         copied. Any existing data in the list will be lost.
    * @leave  KErrNotFound if channel is not open
    * @leave  KErrNotFound if property does not exist
    * @leave  One of the system-wide error codes
    */  
    virtual void GetAllPropertiesL
            ( const TSensrvPropertyId aPropertyId, 
              RSensrvPropertyList& aChannelPropertyList ) = 0;

    /**
    * Start property listening on this channel.
    * 
    * To use this operation clients must provide an object that inherits from MSensrvPropertyListener.
    *     
    * The client can use the property listener to get notifications when properties are changed and
    * to get a notification when the client is able/unable to set properties because of higher priority
    * clients have opened/closed the same sensor channel.
    * 
    * If the client itself changes a property value, no notification is received for that property by
    * the client. If another client changes a property on this channel or another channel that impacts
    * this channel a notification will be received by the client.
    * 
    * Some sensors support a number of channels with each channel having some common properties.
    * Therefore changing a common property may result in notifications on a number of other channels
    * supported by the sensor.
    * 
    * @since S60 5.0
    * @param  aChannelPropertyListener Pointer to channel property listener callback instance. The
    *         channel property listener must be valid until the CSensrvChannel object is destroyed or
    *         listening has been stopped. Listening can be stopped using a NULL parameter.
    * @leave  KErrAlreadyExists if channel property listener has already been set
    * @leave  KErrNotFound if channel is not open
    * @leave  One of the system-wide error codes
    */  
    virtual void SetPropertyListenerL
                  ( MSensrvPropertyListener* aChannelPropertyListener ) = 0;
 
    /**
    * Add a channel ConditionSet to the channel.
    * 
    * Channel ConditionSet's are used for channel condition listening and at least 1 ConditionSet must
    * be added before listening can begin.
    * 
    * When the conditions defined by the ConditionSet is met the client will be notified by the 
    * MSensrvChannelConditionListener::ConditionMet() callback and listening for this ConditionSet will
    * cease. If a client wishes to listen for this ConditionSet again it must be added again.
    * 
    * The client continues to own the ConditionSet object after a call to this function and must ensure
    * that it is destroyed when no longer required.
    * 
    * The client must ensure that the ConditionSet object is valid until it is no longer needed by the
    * Sensor Server. This is either after the condition set has been removed by the client using
    * RemoveConditionL() or the client has been notified that the condition has been met.
    * 
    * @since S60 5.0
    * @param  aChannelConditionSet The channel condition set to be added.
    * @leave  KErrNotFound if channel is not open
    * @leave  KErrNotSupported if a channel does not support conditions.
    * @leave  One of the system-wide error codes
    * @see CSensrvChannel::StartConditionListeningL()    
    */  
    virtual void AddConditionL( CSensrvChannelConditionSet& aChannelConditionSet ) = 0;

    /**
    * Remove a channel ConditionSet from the channel.
    * 
    * The same reference used to to add the Condition Set in AddConditionL(), must also be used to
    * remove it. 
    * 
    * The removed channel ConditionSet is no longer used for channel condition listening. 
    * 
    * The client continues to own the ConditionSet object after a call to this function and must ensure
    * that it is destroyed when no longer required.
    * 
    * @since S60 5.0
    * @param  aChannelConditionSet The channel condition set to be removed.
    * @leave  KErrNotFound if channel is not open
    * @leave  One of the system-wide error codes
    */  
    virtual void RemoveConditionL
                    ( CSensrvChannelConditionSet& aChannelConditionSet ) = 0;

    /**
    * Start channel condition listening.
    * 
    * To use this operation clients must provide an object that inherits from MSensrvChannelConditionListener.
    * 
    * If the conditions of an added ConditionSet is met the MSensrvChannelConditionListener::ConditionMet()
    * callback function will be called and listening for this ConditionSet will cease. If a client wishes
    * to listen for the same ConditionSet again it must be added again.
    * 
    * Note that if a client is listening for both data and conditions on the channel then the lower
    * desired count and buffering period provided in either start listening operation arguments will be
    * used for both listeners. When either listener is stopped neither the desired count nor the
    * buffering period shall be changed.
    * 
    * @since S60 5.0
    * @param  aChannelConditionListener Condition listener for the channel. Must be valid until data
    *         listening is stopped.
    * @param  aObjectCount Listening buffer size defined as a count of the channel data objects. When the
    *         desired object count is reached each ConditionSet is evaluated in turn. The maximum value is
    *         determined by sensor server internal channel buffer size, and minimum value is one. Any value
    *         outside these limits is interpreted as maximum possible value.
    * @param  aBufferingPeriod Specifies the maximum time in milliseconds that sensor server waits before
    *         evaluating new data.
    *         Zero timeout means that the sensor server only evaluates new data when the object count has been
    *         reached.
    * @leave  KErrArgument if aChannelConditionListener is NULL, or if a negative argument is supplied
    * @leave  KErrNotFound if the channel is not open
    * @leave  KErrAlreadyExists if condition listening has already started on the channel
    * @leave  One of the system-wide error codes  
    */  
    virtual void StartConditionListeningL
                    ( MSensrvChannelConditionListener* aChannelConditionListener,
                      const TInt aObjectCount,
                      const TInt aBufferingPeriod ) = 0;

    /**
    * Stop channel condition listening.
    * 
    * All remaining unmet ConditionSet's are removed and channel condition listening is stopped.
    * 
    * If the same ConditionSet's need to be used when condition listening has been restarted then they must be
    * re-added.
    * 
    * @since S60 5.0
    * @return KErrNone if condition listening is successfully stopped or one of the system-wide error codes
    */  
    virtual TInt StopConditionListening() = 0;
    
    public:
    /**
    * Default constructor. 
    */
    CSensrvChannel();
    };


#endif //SENSRVCHANNEL_H

// End of File
