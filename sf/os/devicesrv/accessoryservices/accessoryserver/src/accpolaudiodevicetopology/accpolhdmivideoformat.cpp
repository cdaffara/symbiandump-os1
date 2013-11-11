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
 * Description:
 * CAccPolHdmiVideoFormat class implementation.
 *
 */

// SYSTEM INCLUDES
#include <accpolhdmivideoformat.h>
#include <accpolobjecttypes.h>

#include "acc_debug.h"

// USER INCLUDES

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CAccPolHdmiVideoFormat::CAccPolHdmiVideoFormat( CAccPolObjectCon* aObject ):
    CAccPolObjectBase( EAccPolHdmiVideoFormatObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::CAccPolHdmiVideoFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::CAccPolHdmiVideoFormat() - Return" );
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CAccPolHdmiVideoFormat::CAccPolHdmiVideoFormat( TFixedModeCea aCeaMode,
    TFixedModeDmt aDmtMode,
    TUint8 aPixelRepeat,
    TBool aInterlaced ):
        CAccPolObjectBase( EAccPolHdmiVideoFormatObject ),
        iFixedModeCea( aCeaMode ),
        iFixedModeDmt( aDmtMode ),
        iPixelRepeat( aPixelRepeat ),
        iInterlaced( aInterlaced )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::CAccPolHdmiVideoFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::CAccPolHdmiVideoFormat() - Return" );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CAccPolHdmiVideoFormat::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::ConstructL() - Return" );
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiVideoFormat* CAccPolHdmiVideoFormat::NewL(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::NewL() - Enter" );
    CAccPolHdmiVideoFormat* self = CAccPolHdmiVideoFormat::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiVideoFormat* CAccPolHdmiVideoFormat::NewL( TFixedModeCea aCeaMode,
    TFixedModeDmt aDmtMode,
    TUint8 aPixelRepeat,
    TBool aInterlaced )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::NewL() - Enter" );
    CAccPolHdmiVideoFormat* self = CAccPolHdmiVideoFormat::NewLC( aCeaMode,
        aDmtMode,
        aPixelRepeat,
        aInterlaced );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor.
// Leaves pointer in the cleanup stack.
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiVideoFormat* CAccPolHdmiVideoFormat::NewLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::NewLC() - Enter" );
    CAccPolHdmiVideoFormat* self = new ( ELeave ) CAccPolHdmiVideoFormat( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor.
// Leaves pointer in the cleanup stack.
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiVideoFormat* CAccPolHdmiVideoFormat::NewLC( TFixedModeCea aCeaMode,
    TFixedModeDmt aDmtMode,
    TUint8 aPixelRepeat,
    TBool aInterlaced )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::NewLC() - Enter" );
    CAccPolHdmiVideoFormat* self = new ( ELeave ) CAccPolHdmiVideoFormat( aCeaMode,
        aDmtMode,
        aPixelRepeat,
        aInterlaced );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::operator=
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolHdmiVideoFormat& CAccPolHdmiVideoFormat::operator=(
    const CAccPolHdmiVideoFormat& aVideoFormat )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::operator=() - Enter" );
    CAccPolObjectBase::operator=( ( CAccPolObjectBase& )aVideoFormat );
    iFixedModeCea = aVideoFormat.CeaFixedMode();
    iFixedModeDmt = aVideoFormat.DmtFixedMode();
    iPixelRepeat = aVideoFormat.PixelRepeat();
    iInterlaced = aVideoFormat.Interlaced();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::operator=() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CAccPolHdmiVideoFormat::~CAccPolHdmiVideoFormat()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::~CAccPolHdmiVideoFormat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::~CAccPolHdmiVideoFormat() - Return" );
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::SetCeaFixedMode
//------------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiVideoFormat::SetCeaFixedMode( TFixedModeCea aMode )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SetCeaFixedMode() - Enter" );
    iFixedModeCea = aMode;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SetCeaFixedMode() - Return" );
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::SetDmtFixedMode
//------------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiVideoFormat::SetDmtFixedMode( TFixedModeDmt aMode )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SetDmtFixedMode() - Enter" );
    iFixedModeDmt = aMode;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SetDmtFixedMode() - Return" );
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::SetPixelRepeat
//------------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiVideoFormat::SetPixelRepeat( TUint8 aRepeat )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SetPixelRepeat() - Enter" );
    iPixelRepeat = aRepeat;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SetPixelRepeat() - Return" );
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::SetInterlaced
//------------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiVideoFormat::SetInterlaced( TBool aInterlaced )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SetScanMode() - Enter" );
    iInterlaced = aInterlaced;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SetScanMode() - Return" );
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::SetSupportedPixelRepetitions
//------------------------------------------------------------------------------
//
EXPORT_C void CAccPolHdmiVideoFormat::SetSupportedPixelRepetitions( TUint16 aPixelRepeatBitMask )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SetSupportedPixelRepetitions() - Enter" );
    iPixelRepeatBitMask = aPixelRepeatBitMask;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SetSupportedPixelRepetitions() - Return" );
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::CeaFixedMode
//------------------------------------------------------------------------------
//
EXPORT_C TFixedModeCea CAccPolHdmiVideoFormat::CeaFixedMode() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::CeaFixedMode() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::CeaFixedMode() - Return" );
    return iFixedModeCea;
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::DmtFixedMode
//------------------------------------------------------------------------------
//
EXPORT_C TFixedModeDmt CAccPolHdmiVideoFormat::DmtFixedMode() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::DmtFixedMode() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::DmtFixedMode() - Return" );
    return iFixedModeDmt;
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::PixelRepeat
//------------------------------------------------------------------------------
//
EXPORT_C TUint8 CAccPolHdmiVideoFormat::PixelRepeat() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::PixelRepeat() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::PixelRepeat() - Return" );
    return iPixelRepeat;
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::Interlaced
//------------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolHdmiVideoFormat::Interlaced() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::ScanMode() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::ScanMode() - Return" );
    return iInterlaced;
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::SupportedPixelRepetitions
//------------------------------------------------------------------------------
//
EXPORT_C TUint16 CAccPolHdmiVideoFormat::SupportedPixelRepetitions() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SupportedPixelRepetitions() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::SupportedPixelRepetitions() - Return" );
    return iPixelRepeatBitMask;
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::DuplicateLC
//------------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolHdmiVideoFormat::DuplicateLC(
    CAccPolObjectCon* /*aObject*/ )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::DuplicateLC() - Enter" );
    CAccPolHdmiVideoFormat* self = CAccPolHdmiVideoFormat::NewLC();
    *self = *this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::DuplicateLC() - Return" );
    return self;
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::InternalizeL
//------------------------------------------------------------------------------
//
void CAccPolHdmiVideoFormat::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::InternalizeL() - Enter" );

    // Object data
    iFixedModeCea = static_cast<TFixedModeCea>( aStream.ReadInt16L() );
    iFixedModeDmt = static_cast<TFixedModeDmt>( aStream.ReadInt16L() );
    iPixelRepeat = aStream.ReadUint8L();
    iInterlaced = aStream.ReadUint8L();

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::InternalizeL() - Return" );
    }

//------------------------------------------------------------------------------
// CAccPolHdmiVideoFormat::ExternalizeL
//------------------------------------------------------------------------------
//
void CAccPolHdmiVideoFormat::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::ExternalizeL() - Enter" );

    // Add object type at the beginning
    aStream.WriteUint8L( ObjectType() );

    // Object data
    aStream.WriteInt16L( iFixedModeCea );
    aStream.WriteInt16L( iFixedModeDmt );
    aStream.WriteUint8L( iPixelRepeat );
    aStream.WriteUint8L( iInterlaced );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolHdmiVideoFormat::ExternalizeL() - Return" );
    }

// End of file
