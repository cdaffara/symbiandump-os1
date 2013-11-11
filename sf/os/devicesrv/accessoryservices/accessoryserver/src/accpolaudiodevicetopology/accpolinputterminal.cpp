/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for audio input terminal object.
*
*/

#include <accpolinputterminal.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolInputTerminal::CAccPolInputTerminal()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolInputTerminal::CAccPolInputTerminal( CAccPolObjectCon* aObject ) :
    CAccPolTerminalBase( EAccPolAudioInputTerminalObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::CAccPolInputTerminal() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::CAccPolInputTerminal() - Return void" );
    }

// -----------------------------------------------------------------------------
// CAccPolInputTerminal::CAccPolInputTerminal()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolInputTerminal::CAccPolInputTerminal( const TUint8 aUnitId,
    const TInputTerminalType aType,
    TUint8 aChannelCount,
    TUint16 aChannelConfig ) :
    CAccPolTerminalBase( EAccPolAudioInputTerminalObject, aUnitId ) // JNI type removed
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::CAccPolInputTerminal() -  Enter" );
    iInputTerminalType = aType;
    iChannelCount = aChannelCount;
    iChannelConfig = aChannelConfig;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::CAccPolInputTerminal() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputTerminal* CAccPolInputTerminal::NewL(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::NewL() - Enter" );
    CAccPolInputTerminal* self = CAccPolInputTerminal::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputTerminal* CAccPolInputTerminal::NewLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::NewLC() - Enter" );

    CAccPolInputTerminal* self = new ( ELeave ) CAccPolInputTerminal( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputTerminal* CAccPolInputTerminal::NewL(
    const TUint8 aUnitId,
    const TInputTerminalType aType,
    TUint8 aChannelCount,
    TUint16 aChannelConfig )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::NewL() - Enter" );
    CAccPolInputTerminal* self = CAccPolInputTerminal::NewLC( aUnitId, aType,
        aChannelCount, aChannelConfig );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputTerminal* CAccPolInputTerminal::NewLC(
    const TUint8 aUnitId,
    const TInputTerminalType aType,
    TUint8 aChannelCount,
    TUint16 aChannelConfig )

    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::NewLC() - Enter" );
    CAccPolInputTerminal* self = new ( ELeave ) CAccPolInputTerminal( aUnitId,
        aType, aChannelCount, aChannelConfig );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolInputTerminal::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::ConstructL() - Return" );
    }

// Destructor
EXPORT_C CAccPolInputTerminal::~CAccPolInputTerminal()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::CAccPolInputTerminal() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::~CAccPolInputTerminal() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputTerminal& CAccPolInputTerminal::operator=(
    const CAccPolInputTerminal& aInputTerminalObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::operator() - Enter" );
    CAccPolTerminalBase::operator=(
        ( CAccPolTerminalBase& )aInputTerminalObject );
    iInputTerminalType = aInputTerminalObject.InputTerminalType();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::~operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolInputTerminal::SetChannelCount( TUint8 aChannelCount )
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::SetChannelCount() - Enter %d", aChannelCount );
    iChannelCount = aChannelCount;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::SetChannelCount() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CAccPolInputTerminal::ChannelCount() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::ChannelCount() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::ChannelCount() - Return %d", iChannelCount );
    return iChannelCount;
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolInputTerminal::SetChannelConfig( TUint32 aChannelConfig )
    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::SetChannelConfig() - Enter, 0x%x", aChannelConfig );
    iChannelConfig = aChannelConfig;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::SetChannelConfig() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolInputTerminal::ChannelConfig() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::ChannelConfig() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::ChannelConfig() - Return, 0x%x", iChannelConfig );
    return iChannelConfig;
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::SetSupportedControls()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolInputTerminal::ChannelConfig(
    TUint32 aChannelConfigBitMask ) const

    {
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::SupportedControls() 0x%x", aChannelConfigBitMask );

    TUint32 result( iChannelConfig );
    result = result & aChannelConfigBitMask;

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolFeatureUnit::SupportedControls() - return %d",
        ( result == aChannelConfigBitMask ) );

    return ( result == aChannelConfigBitMask );
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::InputTerminalType()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolInputTerminal::TInputTerminalType CAccPolInputTerminal::InputTerminalType() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::InputTerminalType() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::InputTerminalType() - Return" );
    return iInputTerminalType;
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::SetInputTerminalType()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolInputTerminal::SetInputTerminalType(
    TInputTerminalType aInputTerminalType )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::SetInputTerminalType() - Enter" );
    iInputTerminalType = aInputTerminalType;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::SetInputTerminalType() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolInputTerminal::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::ExternalizeL() - Enter" );
    aStream.WriteUint8L( ObjectType() );
    //Externalize base class.
    ExternalizeTerminalBaseL( aStream );

    //Object data
    aStream.WriteUint8L( iInputTerminalType );
    aStream.WriteUint8L( iChannelCount );
    aStream.WriteUint16L( iChannelConfig );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormat::InternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolInputTerminal::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::InternalizeL() - Enter" );
    //Externalize base class.
    InternalizeTerminalBaseL( aStream );

    // Object data
    iInputTerminalType
        = ( CAccPolInputTerminal::TInputTerminalType )aStream.ReadUint8L();
    iChannelCount = aStream.ReadUint8L();
    iChannelConfig = aStream.ReadUint16L();

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolInputTerminal::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolInputTerminal::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::DuplicateLC() - Enter" );
    CAccPolInputTerminal* aInput = CAccPolInputTerminal::NewLC( aObject );
    *aInput = ( CAccPolInputTerminal& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolInputTerminal::DuplicateLC() - Return" );
    return aInput;
    }
