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
* Description:  Audio stream format class.
*
*/
 
#include <accpolaudiostreamformat.h>

#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolAudioStreamFormat::CAccPolAudioStreamFormat()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolAudioStreamFormat::CAccPolAudioStreamFormat(
    const TStreamConObjectType aObjectType,
    CAccPolObjectCon* aObject ) :
    CAccPolObjectBase( aObjectType, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::CAccPolAudioStreamFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::CAccPolAudioStreamFormat() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolAudioStreamFormat::CAccPolAudioStreamFormat()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolAudioStreamFormat::CAccPolAudioStreamFormat(
    const TStreamConObjectType aObjectType, const TUint8 aUnitId,
    const TUid aAudioFormat, const TStreamType aStreamType,
    const TBitResolution aBitResolution, const TUint32 aSamFreq ) :
    CAccPolObjectBase( aObjectType ), iUnitId( aUnitId ),
        iAudioFormat( aAudioFormat ), iStreamType( aStreamType ),
        iBitResolution( aBitResolution ), iSamFreq( aSamFreq )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::CAccPolAudioStreamFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::CAccPolAudioStreamFormat() - Return" );
    }
// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioStreamFormat* CAccPolAudioStreamFormat::NewL(
    const TStreamConObjectType aObjectType, CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::NewL() - Enter" );
    CAccPolAudioStreamFormat* self =
        CAccPolAudioStreamFormat::NewLC( aObjectType, aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioStreamFormat* CAccPolAudioStreamFormat::NewLC(
    const TStreamConObjectType aObjectType, CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::NewLC() - Enter" );
    CAccPolAudioStreamFormat* self =
        new ( ELeave ) CAccPolAudioStreamFormat( aObjectType, aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioStreamFormat* CAccPolAudioStreamFormat::NewL(
    const TStreamConObjectType aObjectType, const TUint8 aUnitId,
    const TUid aAudioFormat, const TStreamType aStreamType,
    const TBitResolution aBitResolution, const TUint32 aSamFreq )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::NewL() - Enter" );
    CAccPolAudioStreamFormat* self =
        CAccPolAudioStreamFormat::NewLC( aObjectType,
            aUnitId,
            aAudioFormat,
            aStreamType,
            aBitResolution,
            aSamFreq );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioStreamFormat* CAccPolAudioStreamFormat::NewLC(
    const TStreamConObjectType aObjectType, const TUint8 aUnitId,
    const TUid aAudioFormat, const TStreamType aStreamType,
    const TBitResolution aBitResolution, const TUint32 aSamFreq )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::NewLC() - Enter" );
    CAccPolAudioStreamFormat* self =
        new ( ELeave ) CAccPolAudioStreamFormat( aObjectType,
            aUnitId,
            aAudioFormat,
            aStreamType,
            aBitResolution,
            aSamFreq );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolAudioStreamFormat::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::ConstructL() - Return" );
    }

// Destructor
CAccPolAudioStreamFormat::~CAccPolAudioStreamFormat()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::~CAccPolAudioStreamFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::~CAccPolAudioStreamFormat() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioStreamFormat& CAccPolAudioStreamFormat::operator=(
    const CAccPolAudioStreamFormat& aStreamObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::operator() - Enter" );
    CAccPolObjectBase::operator=( ( CAccPolObjectBase& )aStreamObject );
    iUnitId = aStreamObject.UnitId();
    iAudioFormat = aStreamObject.AudioFormat();
    iStreamType = aStreamObject.StreamType();
    iBitResolution = aStreamObject.BitResolution();
    iSamFreq = aStreamObject.SamFreq();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::operator() - Return" );
    return ( *this );
    }
// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::SetUnitId()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioStreamFormat::SetUnitId( const TUint8 aUnitId )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetUnitId() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetUnitId() - Unit Id=%d", aUnitId );
    iUnitId = aUnitId;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetUnitId()" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::SetAudioFormat()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioStreamFormat::SetAudioFormat( const TUid aAudioFormat )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetAudioFormat() - Enter" );
    iAudioFormat = aAudioFormat;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetAudioFormat() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::SetStreamType()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioStreamFormat::SetStreamType(
    const TStreamType aStreamType )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetStreamType() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetStreamType() - Stream Type=%d", aStreamType );
    iStreamType = aStreamType;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetStreamType() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::SetBitResolution()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioStreamFormat::SetBitResolution(
    const TBitResolution aBitResolution )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetBitResolution() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetBitResolution() - Bit resolution=%d", aBitResolution );
    iBitResolution = aBitResolution;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetBitResolution() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::SetSamFreq()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioStreamFormat::SetSamFreq( const TUint32 aSamFreq )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetSamFreq() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetSamFreq() - aSamFreq=%d", aSamFreq );
    iSamFreq = aSamFreq;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SetSamFreq() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::UnitId()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CAccPolAudioStreamFormat::UnitId() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::UnitId() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::UnitId() - Unit Id=%d", iUnitId );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::UnitId() - Return" );
    return iUnitId;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::AudioFormat()
// ---------------------------------------------------------------------------
//
EXPORT_C const TUid CAccPolAudioStreamFormat::AudioFormat() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::AudioFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::AudioFormat() - Return" );
    return iAudioFormat;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::StreamType()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioStreamFormat::TStreamType CAccPolAudioStreamFormat::StreamType() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::StreamType() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::StreamType() - Stream Type=%d", iStreamType );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::StreamType() - Return" );
    return iStreamType;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::BitResolution()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioStreamFormat::TBitResolution CAccPolAudioStreamFormat::BitResolution() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::BitResolution() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::BitResolution() - Bit resolution=%d", iBitResolution );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::BitResolution() - Return" );
    return iBitResolution;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::SamFreq()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolAudioStreamFormat::SamFreq() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SamFreq() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SamFreq() - aSamFreq=%d", iSamFreq );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::SamFreq() - Return" );
    return iSamFreq;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolAudioStreamFormat::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::ExternalizeL() - Enter" );

    //Add object type at the beginning
    aStream.WriteUint8L( ObjectType() );

    //Add data
    aStream.WriteUint8L( iUnitId );
    aStream.WriteInt32L( iAudioFormat.iUid );
    aStream.WriteUint8L( iStreamType );
    aStream.WriteUint8L( iBitResolution );
    aStream.WriteUint32L( iSamFreq );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::ExternalizeL - Object Type     = %d", ObjectType() );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::ExternalizeL - Unit id         = %d", iUnitId );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::ExternalizeL - AudioFormat Uid = %d", iAudioFormat.iUid );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::ExternalizeL - Stream Type    = %d", iStreamType );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::ExternalizeL - Bit Resolution  = %d", iBitResolution );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::ExternalizeL - Sam Freq        = %d", iSamFreq );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::InternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolAudioStreamFormat::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::InternalizeL() - Enter" );

    //Read data
    iUnitId = aStream.ReadUint8L();
    iAudioFormat.iUid = aStream.ReadInt32L();
    iStreamType = ( CAccPolAudioStreamFormat::TStreamType )aStream.ReadUint8L();
    iBitResolution
        = ( CAccPolAudioStreamFormat::TBitResolution )aStream.ReadUint8L();
    iSamFreq = aStream.ReadUint32L();

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::InternalizeL - Object Type     = %d", ObjectType() );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::InternalizeL - Unit id         = %d", iUnitId );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::InternalizeL - AudioFormat Uid = %d", iAudioFormat.iUid );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::InternalizeL - Stream Type    = %d", iStreamType );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::InternalizeL - Bit Resolution  = %d", iBitResolution );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::InternalizeL - Sam Freq        = %d", iSamFreq );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::DublicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolAudioStreamFormat::DuplicateLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::DublicateL() - Enter" );
    CAccPolAudioStreamFormat* aStream =
        CAccPolAudioStreamFormat::NewLC( ( TStreamConObjectType )ObjectType(),
            aObject );
    *aStream = ( CAccPolAudioStreamFormat& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormat::DublicateL() - Return" );
    return aStream;
    }
