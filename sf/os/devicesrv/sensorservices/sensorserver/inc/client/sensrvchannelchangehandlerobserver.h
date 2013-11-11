/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel change observer.
*
*/


#ifndef MSENSRVCHANNELCHANGEHANDLEROBSERVER_H
#define MSENSRVCHANNELCHANGEHANDLEROBSERVER_H

#include <e32base.h>
#include "sensrvtypes.h"


/**
 * Channel change observer. Callback interface to indicate when a channel has been 
 * added or removed
 *  
 * @lib SensrvClient.lib
 * @since S60 5.0 
 */

class MSensrvChannelChangeHandlerObserver
    {
    public:
        /**
        * Indicates channel availability change caused by installation or removal
        * of new software. 
        *
        * @since S60 5.0
        * @param[in] aDetectedChannel Channel information of the channel.
        * @param[in] aChangeType If ESensrvChannelAdded, channel is a newly available
        *                        channel. 
        *                        If ESensrvChannelRemoved, channel was removed and is  
        *                        no longer available.
        */
        virtual void ChannelChangeDetected( const TSensrvChannelInfo& aDetectedChannel,
                                            TSensrvChannelChangeType aChangeType ) = 0;

        /**
        * Channel change listening failed.
        * If error was fatal, sensor server session has also been terminated, and
        * related finder is no longer usable.
        * If error was minor, some notifications have potentially been lost.
        *
        * @since S60 5.0
        * @param[in] aError Error code.
        */
        virtual void ChannelDetectionError( TSensrvErrorSeverity aError ) = 0;
    };

#endif //MSENSRVCHANNELCHANGEHANDLEROBSERVER_H
