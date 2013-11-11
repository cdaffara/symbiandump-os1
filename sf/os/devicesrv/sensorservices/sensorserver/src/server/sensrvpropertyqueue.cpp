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
* Description:  Sensor server property queue.
*
*/



#include "sensrvdefines.h"
#include "sensrvtrace.h"
#include "sensrvpropertyqueue.h"

// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvPropertyQueue* CSensrvPropertyQueue::NewL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::NewL()" ) ) );

    CSensrvPropertyQueue* self = new( ELeave ) CSensrvPropertyQueue();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvPropertyQueue::CSensrvPropertyQueue()
    : iPropertyPtrList(_FOFF(TLinkablePropertyPtr,iLink)),
      iPropertyPtrIter(iPropertyPtrList)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::CSensrvPropertyQueue()" ) ) );

    // Nothing to do

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::CSensrvPropertyQueue - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvPropertyQueue::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::ConstructL()" ) ) );

    iHeap = &User::Heap();
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::ConstructL - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvPropertyQueue::~CSensrvPropertyQueue()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::~CSensrvPropertyQueue()" ) ) );

    RemoveAll();
       
    // iHeap is not owned
        
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::~CSensrvPropertyQueue - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Adds property to the end of the queue
// ---------------------------------------------------------------------------
//
TInt CSensrvPropertyQueue::Append( const TSensrvProperty& aProperty )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::Append(Property ID:%d)" ), aProperty.GetPropertyId() ) );

    TInt err(KErrNone);

    // Allocate linkable transaction pointer in same heap as queue
    TLinkablePropertyPtr* newPtr = reinterpret_cast<TLinkablePropertyPtr*>(iHeap->Alloc(sizeof(TLinkablePropertyPtr)));
    
    if (newPtr)
        {
        Mem::Copy(&(newPtr->iProperty), &aProperty, sizeof(TSensrvProperty));
        iPropertyPtrList.AddLast(*newPtr);                
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CCSensrvPropertyQueue::Append - ERROR: No memory to add item" ) ) );
        err = KErrNoMemory;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::Append - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Gets the first property
// ---------------------------------------------------------------------------
//
TSensrvProperty* CSensrvPropertyQueue::First()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::First()" ) ) );

    TSensrvProperty* property = NULL;
    if (!iPropertyPtrList.IsEmpty())
        {
        TLinkablePropertyPtr* ptr = iPropertyPtrList.First();
        
        if( ptr )
            {        
            property = &(ptr->iProperty);
            }    
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::First - return %d" ), property ) );
    
    return property;
    }

// ---------------------------------------------------------------------------
// Remove the first property
// ---------------------------------------------------------------------------
//
void CSensrvPropertyQueue::RemoveFirst()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::RemoveFirst()" ) ) );

    if (!iPropertyPtrList.IsEmpty())
        {
        TLinkablePropertyPtr* ptr = iPropertyPtrList.First();
        
        if( ptr )
            {        
            iPropertyPtrList.Remove(*ptr);        
                    
            // Pointers are allocated directly on heap, so free them directly also just to be on safe side.
            if (iHeap)
                {
                iHeap->Free(ptr);
                ptr = NULL;
                }
            }    
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::RemoveFirst - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Remove all properties from queue
// ---------------------------------------------------------------------------
//
void CSensrvPropertyQueue::RemoveAll()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPropertyQueue::RemoveAll()" ) ) );

    if (!iPropertyPtrList.IsEmpty())
        {
        TLinkablePropertyPtr* ptr = NULL;
        iPropertyPtrIter.SetToFirst(); 
        
        while ((ptr = iPropertyPtrIter++) != NULL)
            {        
            iPropertyPtrList.Remove(*ptr);
            iHeap->Free(ptr);
            ptr = NULL;
            };
        }
        
    iPropertyPtrList.Reset();
    }

    

