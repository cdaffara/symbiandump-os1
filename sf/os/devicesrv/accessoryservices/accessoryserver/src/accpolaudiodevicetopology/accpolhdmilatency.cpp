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
 * Description:  Latency object
 *
 */

#include <accpolhdmilatency.h>
#include <accpolobjecttypes.h>

#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolHdmiLatency::CAccPolHdmiLatency()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolHdmiLatency::CAccPolHdmiLatency( CAccPolObjectCon* aObject ) :
    CAccPolObjectBase( EAccPolHdmiLatencyObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::CAccPolHdmiLatency() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::CAccPolHdmiLatency() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolHdmiLatency::CAccPolHdmiLatency()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolHdmiLatency::CAccPolHdmiLatency( const TUid aLatencyType,
    const TUint32 aAudioLatency,
    const TUint32 aVideoLatency ) :
        CAccPolObjectBase( EAccPolHdmiLatencyObject ),
        iLatencyType( aLatencyType ),
        iAudioLatency( aAudioLatency ),
        iVideoLatency( aVideoLatency )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::CAccPolHdmiLatency() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::CAccPolHdmiLatency() - Return" );
    }
// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiLatency* CAccPolHdmiLatency::NewL( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::NewL() - Enter" );
    CAccPolHdmiLatency* self = CAccPolHdmiLatency::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiLatency* CAccPolHdmiLatency::NewLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::NewLC() - Enter" );
    CAccPolHdmiLatency* self = new ( ELeave ) CAccPolHdmiLatency( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiLatency* CAccPolHdmiLatency::NewL( const TUid aLatencyType,
    const TUint32 aAudioLatency,
    const TUint32 aVideoLatency )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::NewL() - Enter" );
    CAccPolHdmiLatency* self = CAccPolHdmiLatency::NewLC( aLatencyType,
        aAudioLatency,
        aVideoLatency );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiLatency* CAccPolHdmiLatency::NewLC( const TUid aLatencyType,
    const TUint32 aAudioLatency,
    const TUint32 aVideoLatency )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::NewLC() - Enter" );
    CAccPolHdmiLatency* self = new ( ELeave ) CAccPolHdmiLatency( aLatencyType,
        aAudioLatency,
        aVideoLatency );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiLatency::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::ConstructL() - Return" );
    }

// Destructor
CAccPolHdmiLatency::~CAccPolHdmiLatency()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::~CAccPolHdmiLatency() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::~CAccPolHdmiLatency() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiLatency& CAccPolHdmiLatency::operator=( const CAccPolHdmiLatency& aStreamObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::operator() - Enter" );

    CAccPolObjectBase::operator=( ( CAccPolObjectBase& )aStreamObject );

    iLatencyType = aStreamObject.LatencyType();
    iAudioLatency = aStreamObject.AudioLatency();
    iVideoLatency = aStreamObject.VideoLatency();

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::SetLatencyType()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiLatency::SetLatencyType( const TUid aLatencyType )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::SetLatencyType() - Enter" );
    iLatencyType = aLatencyType;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::SetLatencyType() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::SetAudioLatency()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiLatency::SetAudioLatency( const TUint32 aAudioLatency )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::SetAudioLatency() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::SetAudioLatency() - Latency=%d", aAudioLatency );
    iAudioLatency = aAudioLatency;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::SetAudioLatency() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::SetVideoLatency()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiLatency::SetVideoLatency( const TUint32 aVideoLatency )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::SetVideoLatency() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::SetVideoLatency() - Latency=%d", aVideoLatency );
    iVideoLatency = aVideoLatency;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::SetVideoLatency() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::LatencyType()
// ---------------------------------------------------------------------------
//
EXPORT_C const TUid CAccPolHdmiLatency::LatencyType() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::LatencyType() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::LatencyType() - Return" );
    return iLatencyType;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::AudioLatency()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolHdmiLatency::AudioLatency() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::AudioLatency() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::AudioLatency() - Latency=%d", iAudioLatency );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::AudioLatency() - Return" );
    return iAudioLatency;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::VideoLatency()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolHdmiLatency::VideoLatency() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::VideoLatency() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::VideoLatency() - Latency=%d", iVideoLatency );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::VideoLatency() - Return" );
    return iVideoLatency;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiLatency::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::ExternalizeL() - Enter" );

    //Add object type at the beginning
    aStream.WriteUint8L( ObjectType() );

    //Add data
    aStream.WriteInt32L( iLatencyType.iUid );
    aStream.WriteUint32L( iAudioLatency );
    aStream.WriteUint32L( iVideoLatency );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::ExternalizeL - Object Type     = %d", ObjectType() );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::ExternalizeL - LatencyType Uid = %d", iLatencyType.iUid );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::ExternalizeL - Audio Latency   = %d", iAudioLatency );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::ExternalizeL - Video Latency   = %d", iVideoLatency );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::InternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiLatency::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::InternalizeL() - Enter" );

    //Read data
    iLatencyType.iUid = aStream.ReadInt32L();
    iAudioLatency = aStream.ReadUint32L();
    iVideoLatency = aStream.ReadUint32L();

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::InternalizeL - Object Type     = %d", ObjectType() );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::InternalizeL - LatencyType Uid = %d", iLatencyType.iUid );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::InternalizeL - Audio Latency   = %d", iAudioLatency );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::InternalizeL - Video Latency   = %d", iVideoLatency );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiLatency::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolHdmiLatency::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::DuplicateL() - Enter" );
    CAccPolHdmiLatency* aStream = CAccPolHdmiLatency::NewLC( aObject );
    *aStream = ( CAccPolHdmiLatency& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiLatency::DuplicateL() - Return" );
    return aStream;
    }
