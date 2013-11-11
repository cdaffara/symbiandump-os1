/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container implementation for accessory policy name/value pair objects.
*
*/
 
#include <accpolhdmiobjectcon.h>
#include <accpolobjecttypes.h>

#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolHdmiObjectCon::CAccPolHdmiObjectCon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolHdmiObjectCon::CAccPolHdmiObjectCon( CAccPolObjectCon* aObject ) :
    CAccPolObjectCon( EAccPolHdmiObjectCon, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::CAccPolHdmiObjectCon() - Enter " );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::CAccPolHdmiObjectCon() - Return" );
    }

// Destructor
CAccPolHdmiObjectCon::~CAccPolHdmiObjectCon()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::~CAccPolHdmiObjectCon() - Enter " );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::~CAccPolHdmiObjectCon() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiObjectCon::ConstructL
// ---------------------------------------------------------------------------
//
void CAccPolHdmiObjectCon::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::ConstructL() - Enter " );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::ConstructL() - Return" );
    }
// ---------------------------------------------------------------------------
// CAccPolHdmiObjectCon::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiObjectCon* CAccPolHdmiObjectCon::NewL( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::NewL() - Enter " );
    CAccPolHdmiObjectCon* self = CAccPolHdmiObjectCon::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiObjectCon::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiObjectCon* CAccPolHdmiObjectCon::NewLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::NewLC() - Enter " );
    CAccPolHdmiObjectCon* self = new ( ELeave ) CAccPolHdmiObjectCon( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::NewLC() - Return" );
    return self;
    }

//------------------------------------------------------------------------------
// CAccPolHdmiObjectCon::GetAudioFormatObjectsL
//------------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiObjectCon::GetAudioFormatObjectsL(
    RAccPolHdmiAudioFormatArray& aArray ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::GetFormatObjectsL() - Enter " );
    CAccPolObjectBase* object = NULL;
    TInt count = Count();
    for( TInt i = 0; i < count; i++ )
        {
        object = AtL( i );
        if( object->ObjectType() == EAccPolHdmiAudioFormatObject )
            {
            aArray.AppendL( static_cast<CAccPolHdmiAudioFormat*>( object ) );
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::GetFormatObjectsL() - Return " );    
    }

//------------------------------------------------------------------------------
// CAccPolHdmiObjectCon::GetLatencyObjectsL
//------------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiObjectCon::GetLatencyObjectsL(
    RAccPolHdmiLatencyArray& aArray ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::GetLatencyObjectsL() - Enter " );
    CAccPolObjectBase* object = NULL;
    TInt count = Count();
    for( TInt i = 0; i < count; i++ )
        {
        object = AtL( i );
        if( object->ObjectType() == EAccPolHdmiLatencyObject )
            {
            aArray.AppendL( static_cast<CAccPolHdmiLatency*>( object ) );
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::GetLatencyObjectsL() - Return " );    
    }

//------------------------------------------------------------------------------
// CAccPolHdmiObjectCon::GetHdmiSinkObjectsL
//------------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiObjectCon::GetHdmiSinkObjectsL(
    RAccPolHdmiSinkArray& aArray ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::GetHdmiSinkObjectsL() - Enter " );
    CAccPolObjectBase* object = NULL;
    TInt count = Count();
    for( TInt i = 0; i < count; i++ )
        {
        object = AtL( i );
        if( object->ObjectType() == EAccPolHdmiSinkObject )
            {
            aArray.AppendL( static_cast<CAccPolHdmiSink*>( object ) );
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::GetHdmiSinkObjectsL() - Return " );    
    }

//------------------------------------------------------------------------------
// CAccPolHdmiObjectCon::GetHdmiSpeakerAllocationObjectsL
//------------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiObjectCon::GetHdmiSpeakerAllocationObjectsL(
    RAccPolHdmiSpeakerAllocationArray& aArray ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::GetHdmiSpeakerAllocationObjectsL() - Enter " );
    CAccPolObjectBase* object = NULL;
    TInt count = Count();
    for( TInt i = 0; i < count; i++ )
        {
        object = AtL( i );
        if( object->ObjectType() == EAccPolHdmiSpeakerAllocationObject )
            {
            aArray.AppendL( static_cast<CAccPolHdmiSpeakerAllocation*>( object ) );
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::GetHdmiSpeakerAllocationObjectsL() - Return " );    
    }

//------------------------------------------------------------------------------
// CAccPolHdmiObjectCon::GetHdmiVideoFormatObjectsL
//------------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiObjectCon::GetHdmiVideoFormatObjectsL(
    RAccPolHdmiVideoFormatArray& aArray ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::GetHdmiSpeakerAllocationObjectsL() - Enter " );
    CAccPolObjectBase* object = NULL;
    TInt count = Count();
    for( TInt i = 0; i < count; i++ )
        {
        object = AtL( i );
        if( object->ObjectType() == EAccPolHdmiVideoFormatObject )
            {
            aArray.AppendL( static_cast<CAccPolHdmiVideoFormat*>( object ) );
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::GetHdmiSpeakerAllocationObjectsL() - Return " );    
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::InternalizeL
// ---------------------------------------------------------------------------
//
void CAccPolHdmiObjectCon::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::InternalizeL() - Enter" );

    TUint8 objectType = 0;
    TUint32 count = 0;

    // This object
    SetObjectType( aStream.ReadUint8L() );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::InternalizeBaseL - Object Type=%d", ObjectType() );

    count = aStream.ReadUint8L();//Number of objects inside this container
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::InternalizeL - object count = %d",count );

    for( TInt i = 0; i < count; i++ )
        {
        objectType = aStream.ReadUint8L();
        API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::InternalizeL - object type  = %d",objectType );

        CAccPolObjectBase* object = NULL;
        switch( objectType )
            {
            case EAccPolHdmiSinkObject:
                {
                object = CAccPolHdmiSink::NewLC();
                break;
                }
            case EAccPolHdmiAudioFormatObject:
                {
                object = CAccPolHdmiAudioFormat::NewLC();
                break;
                }
            case EAccPolHdmiLatencyObject:
                {
                object = CAccPolHdmiLatency::NewLC();
                break;
                }
            case EAccPolHdmiSpeakerAllocationObject:
                {
                object = CAccPolHdmiSpeakerAllocation::NewLC();
                break;
                }
            case EAccPolHdmiVideoFormatObject:
                {
                object = CAccPolHdmiVideoFormat::NewLC();
                break;
                }
            default:
                {
                User::Leave( KErrNotSupported );
                break;
                }
            }
        object->InternalizeL( aStream );
        AddL( *object );
        CleanupStack::PopAndDestroy( object );
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::ExternalizeL
// ---------------------------------------------------------------------------
//
void CAccPolHdmiObjectCon::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::ExternalizeL() - Enter" );

    TInt count( Count() );
    aStream.WriteUint8L( ObjectType() );// Type of object
    aStream.WriteUint8L( count );//Count

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::ExternalizeL - object type  = %d", ObjectType() );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::ExternalizeL - object count = %d", count );

    for( TInt i = 0; i < count; i++ )
        {
        AtL( i )->ExternalizeL( aStream );
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::DuplicateL
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolHdmiObjectCon::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::DuplicateL()" );
    CAccPolHdmiObjectCon* aContainer = CAccPolHdmiObjectCon::NewLC( aObject );
    TInt count( Count() );
    for( TInt i = 0; i < count; i++ )
        {
        aContainer->AddL( *AtL( i ) );
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiObjectCon::DuplicateL()" );
    return aContainer;
    }

//  End of File
