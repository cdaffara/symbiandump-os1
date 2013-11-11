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
* Description:  Sensor server client-server API. 
*
*/


#ifndef SENSRVCLIENTSERVER_H
#define SENSRVCLIENTSERVER_H

#include <e32base.h>
#include <sensrvchannelinfo.h>
#include "sensrvdefines.h"
#include "sensrvtypes.h"

/**
* Data holder class for ESensrvSrvReqAsyncChannelData return values.
* @since S60 5.0
*/
class TSensrvAsyncChannelDataCountsRetval
    {
    public:
        TSensrvAsyncChannelDataCountsRetval()
            {}
            
        TSensrvAsyncChannelDataCountsRetval(TInt aDataItemCount,
                                            TInt aDataLostCount)
         : iDataItemCount(aDataItemCount),
           iDataLostCount(aDataLostCount)
            {
            }
        
        /** 
        * Amount of data items written to message 
        */
        TInt iDataItemCount;
        
        /** 
        * Amount of data items lost due buffer overwrite
        */
        TInt iDataLostCount;
    };

/**
* Data holder class for ESensrvSrvReqStartListening parameter values.
* @since S60 5.0
*/
class TSensrvStartListeningParameters
    {
    public:
        TSensrvStartListeningParameters()
            {}
            
        TSensrvStartListeningParameters(TInt aDesiredBufferingCount,
                                        TInt aMaximumBufferingCount,
                                        TInt aBufferingPeriod)
         : iDesiredBufferingCount(aDesiredBufferingCount),
           iMaximumBufferingCount(aMaximumBufferingCount),
           iBufferingPeriod(aBufferingPeriod)
            {
            }
        
        /** 
        * The minimum desired count of items in each request.
        * Count must be less than half of the buffer size assigned
        * to channel.
        * Count less than one or count over maximum means maximum
        * allowed count is used.
        */
        TInt iDesiredBufferingCount;
        
        /** 
        * The maximum count of items in each request.
        * Count must be less than half of the buffer size assigned
        * to channel and more than iDesiredBufferingCount.
        * Count less than iDesiredBufferingCount or count over
        * maximum means maximum allowed count is used.
        */
        TInt iMaximumBufferingCount;

        /** 
        * Buffering period in milliseconds. 
        * If buffering period is very short, it can
        * easily be affected by system load. 
        * This value will be converted into 32-bit microseconds       
        * value on server side, so maximum period is 30 minutes.
        * Period over maximum means maximum period is used.
        * Zero or negative period means no buffering period is used.
        */
        TInt iBufferingPeriod;
    };

/**
* Data holder class for ESensrvSrvReqAddConditionSet parameter values.
* @since S60 5.0
*/
class TSensrvAsyncConditionRetval
    {
    public:
        TSensrvAsyncConditionRetval()
            {}
            
        TSensrvAsyncConditionRetval(
            TInt aConditionSetId,
            TInt aConditionId )
         : iConditionSetId(aConditionSetId),
           iConditionId(aConditionId)
            {
            }
        
        /** 
        * An identifier for condition set which was met. Must be unique within 
        * currently active condition sets for this client.
        */
        TInt iConditionSetId;
        
        /** 
        * An identifier for the specific condition that was met.
        * If multiple conditions in a set were met by the same data,
        * only the first match is indicated here.
        * Must be unique within currently active conditions for this client.
        */
        TInt iConditionId;
    };

// Package buffers for return values    
typedef TPckgBuf<TSensrvAsyncChannelDataCountsRetval> TSensrvAsyncChannelDataCountsRetvalPckgBuf;  
typedef TPckgBuf<TSensrvStartListeningParameters> TSensrvStartListeningParametersPckgBuf;  
typedef TPckgBuf<TSensrvAsyncConditionRetval> TSensrvAsyncConditionRetvalPckgBuf;
typedef TPckgBuf<TInt> TSensrvTIntPckgBuf;
typedef TPckgBuf<TSensrvChannelInfo> TSensrvChannelInfoPckgBuf;
typedef TPckg<TSensrvProperty> TPropertyPckg;
typedef TPckgBuf<TSensrvProperty> TPropertyPckgBuf;
typedef TPckgBuf<TSensrvChannelChangeType> TSensrvTSensrvChannelChangeTypePckgBuf;

// Message slot constants
const TInt KSensrvChannelIdSlot(0); // Channel id must always be in slot zero.

const TInt KSensrvQueryChannelsQueryParametersSlot(0);
const TInt KSensrvQueryChannelsChannelInfoArraySlot(1);
const TInt KSensrvQueryChannelsCountSlot(2);

const TInt KSensrvOpenChannelBufferSizeSlot(1);
const TInt KSensrvOpenChannelDataItemSizeSizeSlot(2);

const TInt KSensrvStartListeningParametersSlot(1); // Used for data and condition listening

const TInt KSensrvAsyncChannelDataBufferSlot(1);
const TInt KSensrvAsyncChannelDataCountSlot(2);

const TInt KSensrvGetPropertyPropertySlot(1);
const TInt KSensrvGetAllPropertiesPropertyArraySlot(1);
const TInt KSensrvGetAllPropertiesPropertyCountSlot(2);
const TInt KSensrvSetPropertyPropertySlot(1);
const TInt KSensrvAsyncPropertyDataPropertySlot(1);

const TInt KSensrvAddConditionSetDataSlot(1);

const TInt KSensrvRemoveConditionSetIdSlot(1);

const TInt KSensrvAsyncConditionMetReturnDataSlot(1);
const TInt KSensrvAsyncConditionMetReturnIdSlot(2);

const TInt KSensrvAsyncChannelChangeInfoSlot(0);
const TInt KSensrvAsyncChannelChangeTypeSlot(1);
const TInt KSensrvAsyncChannelSearchParamsSlot(2);

// Completion codes for the property set success indicator notification
const TInt KSensrvErrPropertySetAvailable(-500000);
const TInt KSensrvErrPropertySetPossible(-500001);
const TInt KSensrvErrPropertySetUnavailable(-500002);

/**
* Opcodes used in message passing between client and server
*
* NOTE: If TSensrvServerRequest enumeration is changed, 
* Sensor Server's policy defined in sensrvserver.h must also be
* checked and changed correspondingly, if necessary.
*/
enum TSensrvServerRequest
    {
// ---------------------------------------------------------------
// Channel listening and property getting operations
// ---------------------------------------------------------------

    /**
    * Query channels
    *
    * parameters:
    * 0 - in  - TSensrvChannelInfoPckgBuf instance containing required channel properties.
    * 1 - out - TDes8 indicating the data area where fetched channels are written to.
    *           Note: Client must reserve enough space to contain all returned channels.
    *                 If there was not enough space, information in array is not valid,
    *                 and must be requested again with larger buffer. 
    *                 The count parameter indicates the actual channel count in that case.
    * 2 - out - TSensrvTIntPckgBuf, Count of returned channel infos.
    *
    * Errors:
    * KErrBadDescriptor - Invalid data descriptor for returning data
    * KErrOverflow - Not enough space to return all found channels.
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqQueryChannels,

    /**
    * Open a channel for the client 
    *
    * parameters:
    * 0 - in - TSensrvChannelID identifying the channel to open
    * 1 - out - TSensrvTIntPckgBuf Maximum client buffer count
    * 2 - out - TSensrvTIntPckgBuf Actual channel data item size
    *
    * Errors:
    * KErrNotFound - Invalid channel id or channel handler creation failure.
    * KErrAlreadyExists - Session already has channel open.
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqOpenChannel,
    
    /**
    * Close a channel for the client
    * Channel must be opened.
    *
    * parameters:
    * 0 - in - TSensrvChannelID identifying the channel to close
    *
    * Errors:
    * KErrNotFound - No channel with that id found.
    * KErrNotReady - Plugin is not in loaded state
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqCloseChannel,

    /**
    * Start listening for a channel.
    * Channel must be opened. 
    *
    * parameters:
    * 0 - in - TSensrvChannelID identifying the channel
    * 1 - in - TSensrvStartListeningParameters specifying
    *          listening parameters.
    *
    * Errors:
    * KErrNotFound - No open channel with that id for this client found.
    * KErrAlreadyExists - This client is already listening for this channel.
    * KErrNotReady - Plugin is not in loaded state
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqStartListening,
    
    /**
    * Stop listening for a channel.
    * Channel must be opened and listened to.
    *
    * parameters:
    * 0 - in - TSensrvChannelID identifying the channel
    *
    * Errors:
    * KErrNotFound - No open and listened channel with that id for this client found.
    * KErrNotReady - Plugin is not in loaded state
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqStopListening,
    
    /**
    * Request data from a channel.
    * Channel must be opened and listened to.
    * This request is asynchronous.
    *
    * Parameters:
    * 0 - in      - TSensrvChannelID identifying the channel 
    * 1 - out     - TDes8 indicating the data area where fetched data is written to.
    * 2 - out     - TSensrvAsyncChannelDataCountsRetvalPckgBuf 
    *               Actual number of returned items and lost items. 
    *               Rest of data area is invalid.
    *
    * Errors:
    * KErrNotFound - No open and listened channel with that id for this client found.
    * KErrBadDescriptor - Invalid data descriptor for returning data.
    * KErrOverflow - Not enough space to return data. All out parameters are invalid.
    * KErrCancel - Request was canceled
    * KErrNotReady - Plugin is not in loaded state
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqAsyncChannelData,

    /**
    * Get a channel property value synchronously. 
    *
    * parameters:
    * 0 - in     - TSensrvChannelID identifying the channel
    * 1 - in/out - TPropertyPckg, returned properties for a property.
    *              Name of the property(iPropertyId)
    *              and channel data type index number (iItemIndex)
    *              are transferred inside TSensrvProperty towards server.
    * Errors:
    * KErrNotFound - No open channel with that id for this client found.
    * KErrArgument - Property doesn't exist.
    * KErrCancel - Request was canceled
    * KErrNotReady - Plugin is not in loaded state
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqGetProperty,
    
    /**
    * Start listening for a channel property changes.
    * Channel must be opened and listened to.
    * This request is asynchronous.
    *
    * Parameters:
    * 0 - in  - TSensrvChannelID identifying the channel 
    * 1 - out - TPropertyPckgBuf, changed property.
    *
    * Errors:
    * KErrNotFound - No open channel with that id for this client found.
    * KErrCancel - Request was canceled
    * KErrNotReady - Plugin is not in loaded state
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqAsyncPropertyData,
    
    /**
    * Stop listening for a channel property changes.
    *
    * parameters:
    * 0 - in - TSensrvChannelID identifying the channel
    *
    * Errors:
    * KErrNotFound - No open channel with that id for this client found.
    * KErrCancel - Request was canceled
    * KErrNotReady - Plugin is not in loaded state
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqStopPropertyListening,
    
    /**
    * Get all channel properties.
    *
    * parameters:
    * 0 - in  - TSensrvChannelID identifying the channel
    * 1 - out - TPtr8 indicating the data area where changed properties are written to.
    *           Note: Client must reserve enough space to contain all returned properties.
    *                 If there was not enough space, information in array is not valid,
    *                 and must be requested again with larger buffer. 
    *                 The count parameter indicates the actual property count in that case.
    * 2 - out - TSensrvTIntPckgBuf, Count of returned properties.
    *
    * Errors:
    * KErrNotFound - No open channel with that id for this client found.
    * KErrCancel - Request was canceled
    * KErrNotReady - Plugin is not in loaded state
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqGetAllProperties,
    
    /**
    * Add a condition set for this channel. 
    * Channel must be opened. 
    *
    * parameters:
    * 0 - in - TSensrvChannelID identifying the channel
    * 1 - in - TDesC8 Externalized CSensrvChannelConditionSetImpl
    *
    * Errors:
    * KErrNotFound - No open channel with that id for this client found.
    * KErrAlreadyExists - This set is already active.
    * KErrNotReady - Plugin is not in loaded state
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqAddConditionSet,
    
    /**
    * Remove a condition set for this channel if one is found.
    * Channel must be opened. 
    *
    * parameters:
    * 0 - in - TSensrvChannelID identifying the channel
    * 1 - in - TSensrvTIntPckgBuf identifying the set to remove.
    *
    * Errors:
    * KErrNotFound - No open channel with specified id
    *                for this client found.
    * KErrNotReady - Plugin is not in loaded state
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqRemoveConditionSet,
       
    /**
    * Start listening for condition set.
    * Channel must be opened. 
    *
    * parameters:
    * 0 - in - TSensrvChannelID identifying the channel
    * 1 - in - TSensrvStartListeningParameters specifying
    *          listening parameters. Maximum count doesn't
    *          really matter in condition listening, as
    *          long as it is at least as much as desired count.
    *
    * Errors:
    * KErrNotFound - No open channel with that id for this client found.
    * KErrAlreadyExists - This client is already listening for this channel.
    * KErrNotReady - Plugin is not in loaded state
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqStartConditionListening,
    
    /**
    * Stop condition listening for a channel.
    * Channel must be opened and listened to.
    *
    * parameters:
    * 0 - in - TSensrvChannelID identifying the channel
    *
    * Errors:
    * KErrNotFound - No open and listened channel with that id for this client found.
    * KErrNotReady - Plugin is not in loaded state
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqStopConditionListening,
    
    /**
    * Request condition met notifications from a channel.
    * Channel must be opened and conditions listened to.
    * This request is asynchronous.
    *
    * Parameters:
    * 0 - in      - TSensrvChannelID identifying the channel 
    * 1 - out     - TDes8 indicating the data area where triggering data value is written to.
    * 2 - out     - TSensrvTIntPckgBuf Id of returned set
    *
    * Errors:
    * KErrNotFound - No open and listened channel with that id for this client found.
    * KErrBadDescriptor - Invalid data descriptor for returning data.
    * KErrOverflow - Not enough space to return data. All out parameters are invalid.
    * KErrCancel - Request was canceled
    * KErrNotReady - Plugin is not in loaded state
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqAsyncConditionMet,
    
    /**
    * Start listening for new and removed channels.
    * This request is asynchronous.
    *
    * Parameters:
    * 0 - out - TSensrvChannelInfoPckgBuf Channel information.
    * 1 - out - TSensrvTSensrvChannelChangeTypePckgBuf, Indicates if channel was removed or added,
    *           value must be one of those defined in TSensrvChannelChangeType enum.
    * 2 - in - TSensrvChannelInfoPckgBuf Search parameter.
    *
    * Errors:
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqAsyncChannelChangeNotification,
    
    /**
    * Stop listening for a channel changes.
    * If not listening, still completes without error.
    *
    * parameters:
    * None
    *
    * Errors:
    * KErrCancel - Request was canceled
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqStopChannelChangeListening,

// ---------------------------------------------------------------
// Property Writing
// ---------------------------------------------------------------

    /**
    * Set a property value.
    *
    * parameters:
    * 0 - in - TSensrvChannelID identifying the channel
    * 1 - in - TPropertyPckg, property to be set.
    *
    * Errors:
    * KErrNotFound - No open channel with that id for this client found, or property
    *                does not exist.
    * KErrPermissionDenied - Client capabilities do not match proprety set requirements.
    * KErrArgument - Property value is not valid.
    * KErrCancel - Request was canceled
    * KErrNotReady - Plugin is not in loaded state
    * KErrNoMemory - Not enough memory to fulfill request.
    */
    ESensrvSrvReqSetProperty,
    
// ---------------------------------------------------------------
// Server shutdown, 
// ---------------------------------------------------------------

    /**
    * Shuts down the server gracefully.
    * This function can only be issued by test framework (KSensrvTestFrameworkUid).
    *
    * parameters:
    * None
    *
    * Errors:
    * KErrPermissionDenied - Invalid client SID
    */
    ESensrvSrvReqShutdownServer,
    
// ---------------------------------------------------------------
// End of enum
// ---------------------------------------------------------------

    /**
    * Indicates end of enumeration values. 
    * Only used in security check and is not supported
    * as a real operation.
    */
    ESensrvSrvReqEnd 
    };


#endif // SENSRVCLIENTSERVER_H 
