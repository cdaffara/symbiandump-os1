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
* Description:  Channel change notifications queue implementation
*
*/


#ifndef SENSRVCHANNELCHANGEQUEUE_H
#define SENSRVCHANNELCHANGEQUEUE_H

#include <e32base.h>
#include "sensrvchannelinfo.h"

/**
 * Channel change notifications queue. 
 *
 * @since S60 5.0
 */
class CSensrvChannelChangeQueue : public CBase
    {
    public:
       
        /**
        * Linkable data about channel change notification
        */
        class TChannelChangeNotification
            {
            public:
                TChannelChangeNotification()
                    {
                    }

                /**
                * Link item for queueing channel change notifications
                */
                TSglQueLink iLink;
                
                /**
                * Channel info
                */
                TSensrvChannelInfo iChannelInfo;
                
                /**
                * Change type
                */
                TSensrvChannelChangeType iChangeType;
            };


        /**
        * Two phase constructor.       
        */
        static CSensrvChannelChangeQueue* NewL();

        /**
        * Destructor.
        */
        virtual ~CSensrvChannelChangeQueue();

        /**
        * Creates new TChannelChangeNotification and adds it to the the queue.
        *
        * @since S60 5.0
        * @param aChannelInfo The channel info related to change
        * @param aChangeType Type of the change
        * @return Symbian OS standard error code.
        */
        TInt Append( const TSensrvChannelInfo& aChannelInfo,
                     TSensrvChannelChangeType aChangeType );

        /**
        * Gets pointer to first change notification from queue.
        *
        * @since S60 5.0
        * @return First change notification in queue, or NULL if empty.
        */
        TChannelChangeNotification* First();

        /**
        * Removes and deletes first change notification from queue,
        * if queue has any notifications.
        *
        * @since S60 5.0
        */
        void RemoveFirst();

        /**
        * Removes and deletes all queued change notifications.
        *
        * @since S60 5.0
        */
        void RemoveAll();
        
        /**
        * Checks if queue is empty
        *
        * @since S60 5.0
        * @return ETrue if queue is empty.
        */
        inline TBool IsEmpty() {return iChangesList.IsEmpty(); };
        

    private:

        /**
        * C++ constructor
        */
        CSensrvChannelChangeQueue();

        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL();            

    private:
    
        /**
        * Holds queued properties.
        * Queued objects are owned.
        */
        TSglQue<TChannelChangeNotification> iChangesList;

        /**
        * Iterator for property queue
        */        
        TSglQueIter<TChannelChangeNotification> iChangesListIter;
    };



#endif // SENSRVCHANNELCHANGEQUEUE_H
