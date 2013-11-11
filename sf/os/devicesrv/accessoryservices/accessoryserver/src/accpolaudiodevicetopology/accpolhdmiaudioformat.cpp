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
 * Description:  HDMI audio format object
 *
 */

#include <accpolhdmiaudioformat.h>
#include <accpolobjecttypes.h>

#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::CAccPolHdmiAudioFormat()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolHdmiAudioFormat::CAccPolHdmiAudioFormat( CAccPolObjectCon* aObject ) :
    CAccPolObjectBase( EAccPolHdmiAudioFormatObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::CAccPolHdmiAudioFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::CAccPolHdmiAudioFormat() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::CAccPolHdmiAudioFormat()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolHdmiAudioFormat::CAccPolHdmiAudioFormat( const TUid aAudioFormat,
    const TUint32 aMaxChannelCount,
    const TUint32 aBitResolution,
    const TUint32 aMaxBitResolution,
    const TUint32 aSamFreq,
    const TUint32 aFormatDependentValue ) :
        CAccPolObjectBase( EAccPolHdmiAudioFormatObject ),
        iAudioFormat( aAudioFormat ),
        iMaxChannelCount( aMaxChannelCount ),
        iBitResolution( aBitResolution ),
        iMaxBitResolution( aMaxBitResolution ),
        iSamFreq( aSamFreq ),
        iFormateDependentValue( aFormatDependentValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::CAccPolHdmiAudioFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::CAccPolHdmiAudioFormat() - Return" );
    }
// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiAudioFormat* CAccPolHdmiAudioFormat::NewL(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::NewL() - Enter" );
    CAccPolHdmiAudioFormat* self = CAccPolHdmiAudioFormat::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiAudioFormat* CAccPolHdmiAudioFormat::NewLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::NewLC() - Enter" );
    CAccPolHdmiAudioFormat* self =
        new( ELeave ) CAccPolHdmiAudioFormat( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiAudioFormat* CAccPolHdmiAudioFormat::NewL( const TUid aAudioFormat,
    const TUint32 aMaxChannelCount,
    const TUint32 aBitResolution,
    const TUint32 aMaxBitResolution,
    const TUint32 aSamFreq,
    const TUint32 aFormatDependentValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::NewL() - Enter" );
    CAccPolHdmiAudioFormat* self = CAccPolHdmiAudioFormat::NewLC( aAudioFormat,
        aMaxChannelCount,
        aBitResolution,
        aMaxBitResolution,
        aSamFreq,
        aFormatDependentValue );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiAudioFormat* CAccPolHdmiAudioFormat::NewLC( const TUid aAudioFormat,
    const TUint32 aMaxChannelCount,
    const TUint32 aBitResolution,
    const TUint32 aMaxBitResolution,
    const TUint32 aSamFreq,
    const TUint32 aFormatDependentValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::NewLC() - Enter" );
    CAccPolHdmiAudioFormat* self =
        new ( ELeave ) CAccPolHdmiAudioFormat( aAudioFormat,
            aMaxChannelCount,
            aBitResolution,
            aMaxBitResolution,
            aSamFreq,
            aFormatDependentValue );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiAudioFormat::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ConstructL() - Return" );
    }

// Destructor
CAccPolHdmiAudioFormat::~CAccPolHdmiAudioFormat()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::~CAccPolHdmiAudioFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::~CAccPolHdmiAudioFormat() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiAudioFormat& CAccPolHdmiAudioFormat::operator=( const CAccPolHdmiAudioFormat& aStreamObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::operator() - Enter" );
    CAccPolObjectBase::operator=( ( CAccPolObjectBase& )aStreamObject );
    iAudioFormat = aStreamObject.AudioFormat();
    iMaxChannelCount = aStreamObject.ChannelCount();
    iBitResolution = aStreamObject.BitResolution();
    iMaxBitResolution = aStreamObject.MaxBitResolution();
    iSamFreq = aStreamObject.SamFreq();
    iFormateDependentValue = aStreamObject.FormatDependentValue();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::SetAudioFormat()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiAudioFormat::SetAudioFormat( const TUid aAudioFormat )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetAudioFormat() - Enter" );
    iAudioFormat = aAudioFormat;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetAudioFormat() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::SetStreamType()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiAudioFormat::SetChannelCount( const TUint32 aMaxChannelCount )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetChannelCount() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetChannelCount() - Channel count=%d", aMaxChannelCount );
    iMaxChannelCount = aMaxChannelCount;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetChannelCount() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::SetBitResolution()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiAudioFormat::SetBitResolution( const TUint32 aBitResolution )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetBitResolution() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetBitResolution() - Bit resolution=%d", aBitResolution );
    iBitResolution = aBitResolution;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetBitResolution() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::SetBitResolution()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiAudioFormat::SetMaxBitResolution( const TUint32 aMaxBitResolution )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetMaxBitResolution() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetMaxBitResolution() - Max Bit resolution=%d", aMaxBitResolution );
    iMaxBitResolution = aMaxBitResolution;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetMaxBitResolution() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::SetFormatDependentValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiAudioFormat::SetFormatDependentValue( const TUint32 aFormatDependentValue )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetFormatDependentValue() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetFormatDependentValue() - Format Dependent Value=%d", aFormatDependentValue );
    iFormateDependentValue = aFormatDependentValue;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetFormatDependentValue() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::SetSamFreq()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiAudioFormat::SetSamFreq( const TUint32 aSamFreq )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetSamFreq() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetSamFreq() - Sampling Freq=%d", aSamFreq );
    iSamFreq = aSamFreq;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SetSamFreq() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::AudioFormat()
// ---------------------------------------------------------------------------
//
EXPORT_C const TUid CAccPolHdmiAudioFormat::AudioFormat() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::AudioFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::AudioFormat() - Return" );
    return iAudioFormat;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::ChannelCount()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolHdmiAudioFormat::ChannelCount() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ChannelCount() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ChannelCount() - Channel count=%d", iMaxChannelCount );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ChannelCount() - Return" );
    return iMaxChannelCount;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::BitResolution()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolHdmiAudioFormat::BitResolution() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::BitResolution() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::BitResolution() - Bit resolution=%d", iBitResolution );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::BitResolution() - Return" );
    return iBitResolution;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::BitResolution()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiAudioFormat::BitResolution( TBitResolution aBitResolution ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::BitResolution() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::BitResolution() - Bit resolution=%d", iBitResolution );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::BitResolution() - Return" );
    return ( iBitResolution & aBitResolution );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::MaxBitResolution()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolHdmiAudioFormat::MaxBitResolution() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::MaxBitResolution() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::MaxBitResolution() - Max Bit resolution=%d", iMaxBitResolution );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::MaxBitResolution() - Return" );
    return iMaxBitResolution;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::SamFreq()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolHdmiAudioFormat::SamFreq() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SamFreq() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SamFreq() - Sampling Freq=%d", iSamFreq );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SamFreq() - Return" );
    return iSamFreq;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::SamFreq()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiAudioFormat::SamFreq( TSamplingFrequency aSamFreq ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SamFreq() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SamFreq() - Sampling Freq=%d", iSamFreq );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::SamFreq() - Return" );
    return ( iSamFreq & aSamFreq );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::FormatDependentValue()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolHdmiAudioFormat::FormatDependentValue() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::FormatDependentValue() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::FormatDependentValue() - Format Dependent Value=%d", iFormateDependentValue );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::FormatDependentValue() - Return" );
    return iFormateDependentValue;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiAudioFormat::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ExternalizeL() - Enter" );

    //Add object type at the beginning
    aStream.WriteUint8L( ObjectType() );

    aStream.WriteInt32L( iAudioFormat.iUid );
    aStream.WriteUint32L( iMaxChannelCount );
    aStream.WriteUint32L( iBitResolution );
    aStream.WriteUint32L( iMaxBitResolution );
    aStream.WriteUint32L( iSamFreq );
    aStream.WriteUint32L( iFormateDependentValue );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ExternalizeL - Object Type             = %d", ObjectType() );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ExternalizeL - AudioFormat Uid         = %d", iAudioFormat.iUid );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ExternalizeL - Channel count           = %d", iMaxChannelCount );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ExternalizeL - Bit Resolution          = %d", iBitResolution );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ExternalizeL - Maximum Bit Resolution  = %d", iMaxBitResolution );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ExternalizeL - Sam Freq                = %d", iSamFreq );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ExternalizeL - Format Dependen tValue  = %d", iFormateDependentValue );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::InternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolHdmiAudioFormat::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::InternalizeL() - Enter" );

    iAudioFormat.iUid = aStream.ReadInt32L();
    iMaxChannelCount = aStream.ReadUint32L();
    iBitResolution = aStream.ReadUint32L();
    iMaxBitResolution = aStream.ReadUint32L();
    iSamFreq = aStream.ReadUint32L();
    iFormateDependentValue = aStream.ReadUint32L();

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::InternalizeL - Object Type            = %d", ObjectType() );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::InternalizeL - AudioFormat Uid        = %d", iAudioFormat.iUid );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::InternalizeL - Channel Count          = %d", iMaxChannelCount );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::InternalizeL - Bit Resolution         = %d", iBitResolution );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::InternalizeL - Maximum Bit Resolution = %d", iMaxBitResolution );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::InternalizeL - Sam Freq               = %d", iSamFreq );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::InternalizeL - Format Dependent Value = %d", iFormateDependentValue );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiAudioFormat::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolHdmiAudioFormat::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::DuplicateL() - Enter" );
    CAccPolHdmiAudioFormat* aStream =
        CAccPolHdmiAudioFormat::NewLC( aObject );
    *aStream = ( CAccPolHdmiAudioFormat& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiAudioFormat::DuplicateL() - Return" );
    return aStream;
    }
