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
* Description:  Server side client session implementation.
*
*/


#ifndef SENSRVSESSION_H
#define SENSRVSESSION_H

#include <e32base.h>
#include "sensrvtypes.h"
#include "sensrvresourcechannelinfo.h"

//Forward declarations
class CSensrvServer;
class CSensrvProxyManager;
class CSensrvMessage;
class CSensrvChannelChangeQueue;

/**
*  Represents a session for a client thread on the server-side.
*  A session acts as a channel of communication between the client 
*  and the server. 
*
*  @since S60 5.0
*/
class CSensrvSession : public CSession2
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSensrvSession* NewL( CSensrvServer& aServer, 
                                     CSensrvProxyManager& aProxyManager,
                                     TSecureId aSecureId );
        
        /**
        * Destructor.
        */
        virtual ~CSensrvSession();
        
        /**
        * Get the Secure ID of the client process.
        *
        * @since S60 5.0
        * @return Secure ID of the process.
        */
        TSecureId SecureId();
        
        /**
        * Notifies client if it is interested in channel change notifications
        *
        * @since S60 5.0
        * @param[in] aChangedChannel Channel information of the channel.
        * @param[in] aChangeType If ESensrvChannelAdded, channel is a newly available
        *                        channel. 
        *                        If ESensrvChannelRemoved, channel was removed and is  
        *                        no longer available.        
        */
        void NotifyChannelChange( const TSensrvResourceChannelInfo& aChangedChannel,
                                  TSensrvChannelChangeType aChangeType );
        
    protected:  // Functions from base classes
        
        /**
        * From CSession2
        *
        * @since S60 5.0
        * @param aMessage The message containing the client request
        */
        virtual void ServiceL( const RMessage2& aMessage );

    private:

        /**
        * C++ default constructor.
        */
        CSensrvSession( CSensrvServer& aServer, 
                        CSensrvProxyManager& aProxyManager,
                        TSecureId aSecureId );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Completes asynchronous change notification message.     
        * Message must exist and have valid handle
        * 
        * @since S60 5.0
        * @param aSet Set that was met. There must be met data in this set.
        *        The set is removed from met sets list and deleted if
        *        completion of message is successful. Otherwise
        *        the set remains in the list.
        */
        void CompleteChangeNotificationMessage(const TSensrvResourceChannelInfo& aChangedChannel,
                                               TSensrvChannelChangeType aChangeType);

    private:    // Data

        /**
        * Server reference. 
        * Not own.
        */
        CSensrvServer& iServer; 
        
        /**
        * Proxy manager reference.
        * Not own.
        */
        CSensrvProxyManager& iProxyManager;
        
        /**
        * Reuse synchronous messages to improve OOM cleanup.
        * Own
        */
        CSensrvMessage* iSyncMessage;
        
        /**
        * Reuse data listening messages to improve cleanup and performance.
        * Own
        */
        CSensrvMessage* iAsyncDataMessage;
        
        /**
        * Reuse property listening messages to improve cleanup and performance.
        * Own
        */
        CSensrvMessage* iAsyncPropertyMessage;

        /**
        * Reuse condition listening messages to improve cleanup and performance.
        * Own
        */
        CSensrvMessage* iAsyncConditionMessage;
        
        /**
        * Reuse channel change messages to improve cleanup and performance.
        * Own
        */
        CSensrvMessage* iAsyncChannelChangeMessage;

        /**
        * Secure ID of the process which has been created this session.
        */
        TSecureId iSecureId; 
        
        /**
        * Queue for channel change notifications.
        * Own
        */
        CSensrvChannelChangeQueue* iChannelChangeQueue;
        
        /**
        * Search params of the active channel change notification.
        */
        TSensrvResourceChannelInfo iChannelChangeSearchParams;

    };


#endif // SENSRVSESSION_H 
