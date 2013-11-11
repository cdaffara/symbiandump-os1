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
* Description:  SSY plug-ins callback interface
*
*/



#ifndef SSYCALLBACK_H
#define SSYCALLBACK_H

//  INCLUDES
#include <sensrvtypes.h>
#include <sensrvchannelinfo.h>
#include <e32base.h>

// FORWARD DECLARATIONS

class MSsyPropertyProvider;
class MSsyChannelDataProvider;

/** 
The UID of the ssy callback API.
*/
static const TUid KSsyCallbackInterface1 = {0x200159EE};

// CLASS DECLARATION

/**
* Baseclass for a callback instance. SSY communicates with a sensor server
* through this interface. 
*/
class MSsyCallback
    {
public:
    /**
    * Register all channels implemented by SSY. 
    * Must initially be called during CSsyControl construction or it cannot be
    * ensured that first clients will find the channels provided by SSY.
    * 
    * This method must also be called after CSsyControl construction, 
    * if the set of channels provided by SSY changes. All currently provided
    * channels must be listed at each re-registration.
    *
    * Note: If this method is called multiple times in rapid succession, 
    * it is not guaranteed that clients will get notifications about all 
    * intermediate changes. However, it is guaranteed that clients will 
    * know the final registered set.
    *
    * @since S60 5.0
    * @param[in, out] aChannelInfoList Channel info list where SSY must add all
    *    channel info objects of channels it provides. The sensor server assigns
    *    the channel id to all channel info objects in the list. If a channel
    *    is exact match to a previously registered channel, it will be assigned
    *    the same channel id as previously.
    *    If the list is empty, this method leaves with KErrArgument, 
    *    as SSY with no channels is not usable.
    */  
    virtual void RegisterChannelsL( RSensrvChannelInfoList& aChannelInfoList ) = 0;

    /**
    * Response to the channel open request. SSY calls this function when
    * SSY initiatation is done after the channel open request.
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel Id of the channel for which this response 
    *            is delivered.
    * @param[in] aErrorCode Symbian OS error code for the operation.
    * @param[in] aChannelDataProvider Pointer to channel data provider instance. 
    *            Must be a non-NULL, unless there was an error.
    * @param[in] aSensorPropertyProvider Pointer to sensor property provider 
    *            instance. Must be non-NULL, unless there was an error.
    */  
    virtual void ChannelOpened
                ( TSensrvChannelId aChannelId, 
                  TInt aErrorCode, 
                  MSsyChannelDataProvider* aChannelDataProvider,
                  MSsyPropertyProvider* aSensorPropertyProvider ) = 0;

    /**
    * Response to the channel close request. SSY call this function when channel 
    * is closed after the channel close request.
    * 
    * @since S60 5.0
    * @param[in] aChannelId Channel Id of the channel for which this response is 
    *            delivered.
    */  
    virtual void ChannelClosed( TSensrvChannelId aChannelId ) = 0;

    /**
    * SSY notifies a sensor server that a write buffer is filled. SSY receives 
    * a new buffer and its size as a reference parameter.  
    *
    * @since S60 5.0
    * @param[in] aChannelId Idenfies the channel this notification is related to.
    * @param[in] aWriteCount Number of objects written to buffer. 
    *     A negative number indicates Symbian OS error code. 
    *     If there is error channel will be closed by the sensor server, and
    *     the contents of aNewBuffer and aNewCount return parameters are 
    *     undefined.
    * @param[out] aNewBuffer Pointer to a new data buffer 
    * @param[out] aNewCount Indicates a new data buffer size as a count of the 
    *     data objects 
    */  
    virtual void BufferFilled( TSensrvChannelId aChannelId, 
                               TInt aWriteCount, 
                               TUint8*& aNewBuffer, 
                               TInt &aNewCount ) = 0;
    
    /**
    * SSY notifies that a sensor property value has changed and it has effect to 
    * specific channels which are listed in the parameter. 
    *
    * @since S60 5.0
    * @param[in] aChannelId Channel Id to be linked to this property changes 
    *            message
    * @param[in] aAffectedChannels Channel Ids of the channels that are linked 
    *            to the changed property. If the property change was initiated by 
    *            sensor server using MSsyPropertyProvider::SetPropertyL, the channel 
    *            linked to that property provider must not be added to this list. 
    *            If this omission would leave aAffectedChannels list empty, there 
    *            is no need to call PropertyChanged at all.
    * @param[in] aProperty New values of the all changed properties.
    */  
    virtual void PropertyChanged( const TSensrvChannelId aChannelId, 
                                  const RSensrvChannelList& aAffectedChannels, 
                                  const TSensrvProperty& aProperty ) = 0;       

    /** 
    * Returns a pointer to a specified interface - to allow future extension
    * of this class without breaking binary compatibility
    *
    * @since S60 5.0
    * @param aInterfaceUid Identifier of the interface to be retrieved
    * @param aInterface A reference to a pointer that retrieves the specified interface.
    */
    virtual void GetCallbackInterfaceL( TUid aInterfaceUid, 
                                        TAny*& aInterface ) = 0;
    };

#endif //SSYCALLBACK_H

// End of File


