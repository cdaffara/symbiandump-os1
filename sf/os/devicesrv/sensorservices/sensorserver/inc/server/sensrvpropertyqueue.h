/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Property queue implementation
*
*/


#ifndef SENSRVTPROPERTYQUEUE_H
#define SENSRVTPROPERTYQUEUE_H

#include <e32base.h>
#include <sensrvproperty.h>

/**
 * Property queue. 
 * This class ensures that queue and queued items (TSensrvProperty)
 * are allocated from same heap.
 *
 * @since S60 5.0
 */
class CSensrvPropertyQueue : public CBase
    {
    public:
       
        /**
        * Linkable property pointer.
        */
        class TLinkablePropertyPtr
            {
            public:
                TLinkablePropertyPtr()
                    {
                    }

                /**
                * Link item for queueing property pointers
                */
                TSglQueLink iLink;
                
                /**
                * Property
                */
                TSensrvProperty iProperty;
            };


        /**
        * Two phase constructor.       
        */
        static CSensrvPropertyQueue* NewL();

        /**
        * Destructor.
        */
        virtual ~CSensrvPropertyQueue();

        /**
        * Adds a copy of a property to the the queue.
        *
        * @since S60 5.0
        * @param aProperty The property to be added.
        * @return Symbian OS standard error code.
        */
        TInt Append( const TSensrvProperty& aProperty );

        /**
        * Gets pointer to first property from queue.
        *
        * @since S60 5.0
        * @return First property in queue.
        */
        TSensrvProperty* First();

        /**
        * Removes and deletes first property from queue.
        *
        * @since S60 5.0
        */
        void RemoveFirst();

        /**
        * Removes and deletes all properties.
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
        inline TBool IsEmpty() {return iPropertyPtrList.IsEmpty(); };
        

    private:

        /**
        * C++ constructor
        */
        CSensrvPropertyQueue();

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
        TSglQue<TLinkablePropertyPtr> iPropertyPtrList;

        /**
        * Iterator for property queue
        */        
        TSglQueIter<TLinkablePropertyPtr> iPropertyPtrIter;
                
        /**
        * Heap where the queue resides. 
        * Transactions must reside in the same heap.
        * Not own.
        */
        RHeap* iHeap;
    };



#endif // SENSRVTPROPERTYQUEUE_H
