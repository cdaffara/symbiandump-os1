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
* Description:  Interface for channel listener
*
*/



#ifndef SENSRVCHANNELLISTENER_H
#define SENSRVCHANNELLISTENER_H


#include <e32base.h>
#include <sensrvchannelfinder.h>
#include <sensrvtypes.h>
#include <sensrvchannelinfo.h>

/**
* Channel listener callback interface to indicate when channels have been added or removed.
* 
* This class is for use with the CSensrvChannelFinder::SetChannelListenerL() method. Clients wishing
* to find channels must inherit from this class and provide a reference to an instance of it in
* CSensrvChannelFinder::SetChannelListenerL().
* 
* @see CSensrvChannelFinder::SetChannelListenerL()
* @lib sensrvclient.lib
* @since S60 5.0
*/
class MSensrvChannelListener
    {
    public:

    /**
    * Callback implemented by a client so that they can be notified when channels are added
    * or removed from the system when new software is installed or uninstalled.
    * 
    * Clients providing an implementation for this callback must ensure that the operation does
    * not leave. If a leave does occur then the behaviour is undefined.
    * 
    * @since S60 5.0
    * @param aDetectedChannel Information for the channel that has been added or removed
    * @param  aChangeType If ESensrvChannelAdded, channel is a newly available channel.
    * If ESensrvChannelRemoved, channel has been removed and is no longer available.
    */
    virtual void ChannelChangeDetected( const TSensrvChannelInfo& aDetectedChannel,
                                        TSensrvChannelChangeType aChangeType ) = 0;

    /**
    * Callback implemented by a client so that they can be notified when channel listening
    * has failed. If the error is fatal the sensor server session has been terminated and
    * the channel finder is no longer useable. If the error is minor some notifications have
    * potentially been lost, however listening is still active.
    * 
    * Clients providing an implementation for this callback must ensure that the operation does
    * not leave. If a leave does occur then the behaviour is undefined.
    * 
    * @since S60 5.0
    * @param  aChannelFinder Reference to the related channelfinder object
    * @param  aError The error severity
    */
    virtual void ChannelDetectionError( CSensrvChannelFinder& aChannelFinder,
                                        TSensrvErrorSeverity aError ) = 0;

    /** 
    * Callback to future proof this API so that additional callbacks can be added in the future 
    * without breaking binary compatibility.
    * 
    * @since S60 5.0
    * @param  aInterfaceUid Identifier for the interface to be retrieved
    * @param  aInterface A reference to a pointer for the specified interface. Implementation sets
    * aInterface to a valid pointer if the M-class identified by aInterfaceUid is supported, otherwise
    * it is set to NULL on exit.
    * @leave  One of the system-wide error codes
    */
	virtual void GetChannelListenerInterfaceL( TUid aInterfaceUid, TAny*& aInterface ) = 0;

    };



#endif //SENSRVCHANNELLISTENER_H

// End of File

