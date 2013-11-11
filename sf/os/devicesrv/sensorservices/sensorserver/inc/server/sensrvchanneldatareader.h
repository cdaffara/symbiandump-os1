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
* Description:  TSensrvChannelDataReader declaration.
*
*/


#ifndef SENSRVCHANNELDATAREADER_H
#define SENSRVCHANNELDATAREADER_H

#include <sensrvtypes.h>
#include "sensrvresourcechannelinfo.h"
class CImplementationInformation;

/**
 *  An utility class for parsing channel data from
 *  SSY plugin ECom registration data.
 *
 *  @lib None.
 *  @since S60 5.0
 */
class TSensrvChannelDataReader
    {

public:
    /**
    * Bitmask enumeration for channel flags.
    */
    enum TChannelFlags
        {
        EChannelFlagDynamic 				= 0x01,
        EChannelFlagLocationPresent 		= 0x02,
        EChannelFlagVendorIdPresent 		= 0x04,
        EChannelFlagSecPolicyPresent 		= 0x08,
        EChannelFlagChannelGroupReserved 	= 0x30
        };

public:

    /**
    * Reads the channel data from SSY plugin registration data.
    *
    * @param aSsyInfo            plugin implementation information containing the channel
    *                            data in the default_data field of the plugin resource file.
    * @param aChannelList        used for storing the channel information. Contents of the
    *                            list are cleared by this method.
    * @param aDynamicChannelList used for storing the dynamic channel information.
    *                            Contents of the list are cleared by this method.
    *
    *
    * @return KErrNone                   channel data read was successful
    * @return KErrCorrupt                channel data is invalid
    * @return a Symbian wide error code  only in case RArray::Append fails
    */
    TInt ReadChannels( const CImplementationInformation& aSsyInfo,
                       RSensrvResourceChannelInfoList& aChannelList,
                       RSensrvResourceChannelInfoList& aDynamicChannelList );

    };

#endif // SENSRVCHANNELDATAREADER_H
