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
* Description:  The channel basic information with knowledge of which implementation
*                this channel belongs to.
*
*/


#ifndef SENSRVRESOURCECHANNELINFO_H
#define SENSRVRESOURCECHANNELINFO_H

#include <e32base.h>
#include <sensrvtypes.h>
#include <sensrvchannelinfo.h>

#include "sensrvmessage.h"
#include "sensrvtrace.h"

/**
 *  The channel info contains channel basic information with knowledge of
 *  which implementation this channel belongs to.
 *
 *  @lib None.
 *  @since S60 5.0
 */
class TSensrvResourceChannelInfo : public TSensrvChannelInfo
    {
public:

        /**
        * Channel group.
        */
        enum TSensrvChannelGroup
            {
            ESensrvChannelGroupNotSolved = -1, ///< Channel group is not solved yet
            ESensrvChannelGroupNotDefined,     ///< Channel does not belong to any group
            ESensrvChannelGroupData,           ///< Channel belongs to group of data channels
            ESensrvChannelGroupEvent,          ///< Channel belongs to group of event channels
            ESensrvChannelGroupState           ///< Channel belongs to group of state channels
            };

    /**
    * Constructor of the TSensrvResourceChannelInfo
    */
    TSensrvResourceChannelInfo();

    /**
    * Copy constructor of the TSensrvResourceChannelInfo.
    *
    * @param[in] aChannelInfo Object to be copied to constructed object
    */
    TSensrvResourceChannelInfo( const TSensrvResourceChannelInfo& aChannelInfo );

    /**
    * Copy constructor of the TSensrvResourceChannelInfo.
    *
    * @param[in] aChannelInfo Object to be copied to constructed object
    */
    TSensrvResourceChannelInfo( const TSensrvChannelInfo& aChannelInfo );

    /**
    * Checks if parameter info matches the old info.
    * Channel Id and any value that is zero in aInfo are ignored.
    *
    * @param[in] aInfo Object to be matched agains this object.
    * @return ETrue if specified info is a match.
    */
    TBool IsMatch( const TSensrvResourceChannelInfo &aInfo ) const;

    /**
    * Checks if parameter info matches the old info.
    * Channel Id and any value that is zero in aInfo are ignored.
    *
    * @param[in] aInfo Object to be matched agains this object.
    * @param[in] aMessage The message containing the client request.
    * @return ETrue if specified info is a match.
    */
    TBool IsMatch( const TSensrvResourceChannelInfo &aInfo, const RMessage2& aMessage ) const;

    /**
    * Checks that channel info is complete, i.e. no zero/empty fields, except
    * channel Id, channel visibility and reserved member, which are ignored.
    *
    * @return ETrue if this info is complete.
    */
    TBool IsComplete() const;

    /**
    * Compare function for match functionality
    *
    * @param[in] aChannelInfoToMatchFrom Channel info to match from. Cannot be NULL.
    * @param[in] aChannelInfoToMatchWith Channel info to match with.
    * @return Comparison result.
    */
    static TInt CompareFindMatch( const TSensrvChannelInfo* aChannelInfoToMatchFrom,
                                  const TSensrvResourceChannelInfo& aChannelInfoToMatchWith );

    /**
    * Compare function for match functionality
    *
    * @param[in] aChannelInfoToMatchWith Channel info to match with. Cannot be NULL.
    * @param[in] aChannelInfoToMatchFrom Channel info to match from.
    * @return Comparison result.
    */
    static TInt CompareFindMatchReverse( const TSensrvChannelInfo* aChannelInfoToMatchWith,
                                         const TSensrvChannelInfo& aChannelInfoToMatchFrom );

private:
    #ifdef COMPONENT_TRACE_DEBUG

    /**
    * Prints client capabilities
    *
    */
    void TraceCapabilities( const RMessage2& aMessage ) const;

    #endif //COMPONENT_TRACE_DEBUG
public:
    /**
    * UID of the SSY implementation which this channel belongs to.
    */
    TUid iSsyImplementationUid;

    /**
    * Security policy of the channel.
    */
    TSecurityPolicy iPolicy;

    /**
    * Indicates if channel is a dynamic channel or a static channel.
    */
    TBool iDynamic;

	/**
	* Channel group of this channel.
	*/
	TSensrvChannelGroup iChannelGroup;

    };

typedef RArray<TSensrvResourceChannelInfo> RSensrvResourceChannelInfoList;



#endif // SENSRVRESOURCECHANNELINFO_H
