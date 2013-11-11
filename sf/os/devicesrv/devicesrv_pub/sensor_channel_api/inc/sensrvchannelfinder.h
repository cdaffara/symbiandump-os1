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
* Description:  Sensor Channel API
*
*/



#ifndef SENSRVCHANNELFINDER_H
#define SENSRVCHANNELFINDER_H

//  INCLUDES
#include <e32base.h>
#include <sensrvtypes.h>
#include <sensrvchannelinfo.h>

// CLASS DECLARATION
class MSensrvChannelListener;

/**
* The CSensrvChannelFinder class provides an API that allows clients to discover the sensor
* channels supported by Sensor Services and receive notifications when channels are installed
* and uninstalled.
* 
* The channel specific header files e.g. for orientation, acceleration, proximity, will document
* whether a secure id, vendor id or a set of capabilities, if any, is required by the client to
* be able to view a channel or receive notifications for it. See the channel specific header
* files for these requirements.
* 
* On instantiation of this class a session connection to the Sensor Server is made. The Sensor
* server is a transient server and is started, if not already running, when a session connection
* is made. The sensor server is shutdown when a device-configured timeout period expires after
* the last session connection has been dropped. The Sensor Server will not shutdown if there is
* a session connection to it.
* 
* Each item in RSensrvChannelInfoList returned by FindChannelsL() contains a channel Id which is
* only valid for the lifetime of the sensor server. If a client requires that this id remains
* unchanged then the client must ensure that an instance of either CSensrvChannelFinder or
* CSensrvChannel remains instantiated. This keeps a session connection with the server open which
* prevents the sensor server from shutting down.
*
* @see CSensrvChannel
* @lib sensrvclient.lib
* @since S60 5.0
*/
NONSHARABLE_CLASS( CSensrvChannelFinder ): public CBase
    {
    public:
    /**
    * Two-phased constructor.
    *
    * @since S60 5.0
    * @return Pointer to created CSensrvChannelFinder object
    * @leave  KErrNoMemory
    * @leave  One of the system-wide error codes
    */  
    IMPORT_C static CSensrvChannelFinder* NewL();

    /**
    * Two-phased constructor.
    *
    * @since S60 5.0
    * @return Pointer to created CSensrvChannelFinder object that is placed on the cleanup stack.
    * @leave  KErrNoMemory
    * @leave  One of the system-wide error codes
    */  
    IMPORT_C static CSensrvChannelFinder* NewLC();
  
    public:
    /**
    * Retrieves a list of channels that meet the supplied search parameters. Only
    * channels for which the client has the required secure id, vendor id and
    * capabilities will be returned. See class description for further information.
    *     
    * @since S60 5.0
    * @param  aChannelList List of all channels matching the supplied search 
    *         parameters. Each TSensrvChannelInfo instance in the list can be used
    *         to open a channel. Empty list is returned if no matching channels are
    *         found.
    * @param  aSearchParameters Parameters for which matching channels are to be
    *         found. To omit a field from the search use zero (if integer) or
    *         empty string (if descriptor). If all fields are zero or empty string
    *         all channels provided by the system will be returned. ChannelId,
    *         DataItemSize and Reserved data will be ignored if set.
    * @leave  One of the system-wide error codes
    */
    virtual void FindChannelsL( RSensrvChannelInfoList& aChannelList, 
                                const TSensrvChannelInfo& aSearchParameters ) = 0;
    
    /**
    * Listens for channels that meet the supplied search parameters. Only channels
    * for which the client has the required secure id, vendor id and capabilities
    * will be returned. See class description. Listening can be stopped by providing
    * a NULL parameter for the aChannelListener argument.
    * 
    * @since S60 5.0
    * @param  aChannelListener Pointer to channel listener callback instance. The
    *         channel listener must be valid until the CSensrvChannelFinder object
    *         is destroyed or listening has been stopped. Listening can be stopped
    *         using a NULL parameter.
    * @param  aSearchParameters Parameters for which matching channel notifications
    *         are to be provided. To omit a field from the search use zero (if integer)
    *         or empty string (if descriptor). If all fields are zero or empty string
    *         all channels provided by the system will be returned. ChannelId,
    *         DataItemSize and Reserved data will be ignored if set.
    * @leave  KErrAlreadyExists if channel listener has already been set
    * @leave  One of the system-wide error codes
    */
    virtual void SetChannelListenerL( MSensrvChannelListener* aChannelListener,
                                const TSensrvChannelInfo& aSearchParameters  ) = 0;

    public:
    /**
    * Default constructor. 
    */
    CSensrvChannelFinder();
    };


#endif //SENSRVCHANNELFINDER_H

// End of File
