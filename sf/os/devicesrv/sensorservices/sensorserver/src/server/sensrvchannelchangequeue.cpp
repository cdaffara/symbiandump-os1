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



#include "sensrvdefines.h"
#include "sensrvtrace.h"
#include "sensrvchannelchangequeue.h"

// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelChangeQueue* CSensrvChannelChangeQueue::NewL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::NewL()" ) ) );

    CSensrvChannelChangeQueue* self = new( ELeave ) CSensrvChannelChangeQueue();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelChangeQueue::CSensrvChannelChangeQueue()
    : iChangesList(_FOFF(TChannelChangeNotification,iLink)),
      iChangesListIter(iChangesList)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::CSensrvChannelChangeQueue()" ) ) );

    // Nothing to do

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::CSensrvChannelChangeQueue - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvChannelChangeQueue::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::ConstructL()" ) ) );
    
    // Nothing to do

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::ConstructL - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvChannelChangeQueue::~CSensrvChannelChangeQueue()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::~CSensrvChannelChangeQueue()" ) ) );

    RemoveAll();
       
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::~CSensrvChannelChangeQueue - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Adds property to the end of the queue
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelChangeQueue::Append( const TSensrvChannelInfo& aChannelInfo,
                                        TSensrvChannelChangeType aChangeType )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::Append(Channel ID:%d)" ), aChannelInfo.iChannelId ) );

    TInt err(KErrNone);

    // Allocate linkable transaction pointer in same heap as queue
    TChannelChangeNotification* newPtr = new TChannelChangeNotification();
    
    if (newPtr)
        {
        newPtr->iChannelInfo = aChannelInfo;
        newPtr->iChangeType = aChangeType;
        iChangesList.AddLast(*newPtr);                
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CCSensrvChannelChangeQueue::Append - ERROR: No memory to add item" ) ) );
        err = KErrNoMemory;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::Append - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Gets the first property
// ---------------------------------------------------------------------------
//
CSensrvChannelChangeQueue::TChannelChangeNotification* CSensrvChannelChangeQueue::First()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::First()" ) ) );

    TChannelChangeNotification* ptr = NULL;
    
    if (!iChangesList.IsEmpty())
        {
        ptr = iChangesList.First();
        }
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::First - return %d" ), ptr ) );
    
    return ptr;
    }

// ---------------------------------------------------------------------------
// Remove the first property
// ---------------------------------------------------------------------------
//
void CSensrvChannelChangeQueue::RemoveFirst()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::RemoveFirst()" ) ) );

    TChannelChangeNotification* ptr = NULL;
    
    if (!iChangesList.IsEmpty())
        {
        ptr = iChangesList.First();
        iChangesList.Remove(*ptr);
        delete ptr;
        }
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::RemoveFirst - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Remove all properties from queue
// ---------------------------------------------------------------------------
//
void CSensrvChannelChangeQueue::RemoveAll()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannelChangeQueue::RemoveAll()" ) ) );

    if (!iChangesList.IsEmpty())
        {
        TChannelChangeNotification* ptr = NULL;
        iChangesListIter.SetToFirst(); 
        
        while ((ptr = iChangesListIter++) != NULL)
            {        
            iChangesList.Remove(*ptr);
            delete ptr;
            };
        }
        
    iChangesList.Reset();
    }

    

