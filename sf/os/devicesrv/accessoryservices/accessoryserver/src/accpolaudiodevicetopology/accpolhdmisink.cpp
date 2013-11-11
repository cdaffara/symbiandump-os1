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
 * Description:  HDMI sink object
 *
 */

#include <accpolhdmisink.h>
#include <accpolobjecttypes.h>

#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolHdmiSink::CAccPolHdmiSink()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolHdmiSink::CAccPolHdmiSink( CAccPolObjectCon* aObject ) :
    CAccPolObjectBase( EAccPolHdmiSinkObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::CAccPolHdmiSink() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::CAccPolHdmiSink() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolHdmiSink::CAccPolHdmiSink()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolHdmiSink::CAccPolHdmiSink( const TBool aBasicAudioSupport,
    const TBool aHDCPSupport ):
        CAccPolObjectBase( EAccPolHdmiSinkObject ),
        iBasicAudioSupport( aBasicAudioSupport ),
        iHdcpSupport( aHDCPSupport )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::CAccPolHdmiSink() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::CAccPolHdmiSink() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSink* CAccPolHdmiSink::NewL( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::NewL() - Enter" );
    CAccPolHdmiSink* self = CAccPolHdmiSink::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSink* CAccPolHdmiSink::NewLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::NewLC() - Enter" );
    CAccPolHdmiSink* self = new ( ELeave ) CAccPolHdmiSink( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSink* CAccPolHdmiSink::NewL( const TBool aBasicAudioSupport,
    const TBool aHDCPSupport )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::NewL() - Enter" );
    CAccPolHdmiSink* self = CAccPolHdmiSink::NewLC( aBasicAudioSupport,
        aHDCPSupport );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::NewL()- Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSink* CAccPolHdmiSink::NewLC( const TBool aBasicAudioSupport,
    const TBool aHDCPSupport )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::NewLC() - Enter" );
    CAccPolHdmiSink* self = new ( ELeave ) CAccPolHdmiSink( aBasicAudioSupport,
        aHDCPSupport );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiSink::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::ConstructL() - Return" );
    }

// Destructor
CAccPolHdmiSink::~CAccPolHdmiSink()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::~CAccPolHdmiSink() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::~CAccPolHdmiSink() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiSink& CAccPolHdmiSink::operator=( const CAccPolHdmiSink& aHdmiSink )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::operator() - Enter" );
    CAccPolObjectBase::operator=( ( CAccPolObjectBase& )aHdmiSink );
    iBasicAudioSupport = aHdmiSink.BasicAudioSupport();
    iHdcpSupport = aHdmiSink.HdcpSupport();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::BasicAudioSupport()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSink::BasicAudioSupport() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::BasicAudioSupport() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::BasicAudioSupport() - Return" );
    return iBasicAudioSupport;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::HdcpSupport()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiSink::HdcpSupport() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::HdcpSupport() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::HdcpSupport() - Return" );
    return iHdcpSupport;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::SetBasicAudioSupport()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSink::SetBasicAudioSupport( TBool aBasicAudioSupport )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::SetBasicAudioSupport() - Enter" );
    iBasicAudioSupport = aBasicAudioSupport;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::SetBasicAudioSupport() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::SetHdcpSupport()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiSink::SetHdcpSupport( TBool aHdcpSupport )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::SetHdcpSupport() - Enter" );
    iHdcpSupport = aHdcpSupport;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::SetHdcpSupport() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiSink::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::ExternalizeL() - Enter" );

    // Add object type at the beginning
    aStream.WriteUint8L( ObjectType() );

    // Object data
    aStream.WriteUint8L( BasicAudioSupport() );
    aStream.WriteUint8L( HdcpSupport() );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::ExternalizeL - BasicAudioSupport  = %d", iBasicAudioSupport );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::ExternalizeL - HdcpSupport        = %d", iHdcpSupport );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::InternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiSink::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::InternalizeL() - Enter" );

    // Object data
    iBasicAudioSupport = ( TBool )aStream.ReadUint8L();
    iHdcpSupport = ( TBool )aStream.ReadUint8L();

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::InternalizeL - BasicAudioSupport  = %d", iBasicAudioSupport );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::InternalizeL - HdcpSupport        = %d", iHdcpSupport );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiSink::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolHdmiSink::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::DuplicateLC() - Enter" );
    CAccPolHdmiSink* aOutput = CAccPolHdmiSink::NewLC( aObject );
    *aOutput = ( CAccPolHdmiSink& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiSink::DuplicateLC() - Return" );
    return aOutput;
    }
