// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of CStrtSecNoteRequestQueue class
//



#include "strtsecnoterequestqueue.h"
#include "ssmdebug.h"
#include "clayerpanic.h"

// Allocate queue space for this many items in advance.
const TInt KPreallocatedQueueSize = 8;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CStrtSecNoteRequestQueue::NewL
//
// ---------------------------------------------------------------------------
//
CStrtSecNoteRequestQueue* CStrtSecNoteRequestQueue::NewL()
    {
    CStrtSecNoteRequestQueue* self = new( ELeave ) CStrtSecNoteRequestQueue;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CStrtSecNoteRequestQueue::~CStrtSecNoteRequestQueue
//
// ---------------------------------------------------------------------------
//
CStrtSecNoteRequestQueue::~CStrtSecNoteRequestQueue()
    {
    iArray.Close();
    }

// ---------------------------------------------------------------------------
// CStrtSecNoteRequestQueue::Add
//
// ---------------------------------------------------------------------------
//
TInt CStrtSecNoteRequestQueue::Add( const TStrtSecurityNoteType aItem )
    {
	DEBUGPRINT2A("Item to add: %d", aItem);
	__ASSERT_DEBUG(aItem >= ESecCodePIN1 && aItem < ESecNoteTypeLimit, CLAYER_PANIC(ECLayerInvalidNoteType));

    TInt errorCode = iArray.Find( aItem );
    if ( errorCode == KErrNotFound )
        {
        errorCode = iArray.Append( aItem );
        }
    return errorCode;
    }

// ---------------------------------------------------------------------------
// CStrtSecNoteRequestQueue::Remove
//
// ---------------------------------------------------------------------------
//
void CStrtSecNoteRequestQueue::Remove( const TStrtSecurityNoteType aItem )
    {
	DEBUGPRINT2A("Item to remove: %d", aItem);
    TInt idx = iArray.Find( aItem );
    
    if ( idx >= 0 && idx < iArray.Count() )
        {
        iArray.Remove( idx );
        }
    }

// ---------------------------------------------------------------------------
// CStrtSecNoteRequestQueue::GetFirst
//
// ---------------------------------------------------------------------------
//
TStrtSecurityNoteType CStrtSecNoteRequestQueue::GetFirst()
    {
    TStrtSecurityNoteType item = ESecNoteNone;
    if ( iArray.Count() > 0 )
        {
        item = iArray[0];
        iArray.Remove( 0 );
        }
    return item;
    }

// ---------------------------------------------------------------------------
// CStrtSecNoteRequestQueue::CStrtSecNoteRequestQueue
//
// ---------------------------------------------------------------------------
//
CStrtSecNoteRequestQueue::CStrtSecNoteRequestQueue()
    {
    
    }

// ---------------------------------------------------------------------------
// CStrtSecNoteRequestQueue::ConstructL
//
// ---------------------------------------------------------------------------
//
void CStrtSecNoteRequestQueue::ConstructL()
    {
    iArray.ReserveL( KPreallocatedQueueSize );
    }
