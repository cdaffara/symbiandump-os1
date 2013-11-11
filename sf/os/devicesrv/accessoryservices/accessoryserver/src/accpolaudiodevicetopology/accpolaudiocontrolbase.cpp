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
* Description:  Audio control base class.
*
*/

#include <accpolaudiocontrolbase.h>

#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolAudioControlBase::CAccPolAudioControlBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioControlBase::CAccPolAudioControlBase(
    TTopologyConObjectType aObjectType,
    CAccPolObjectCon* aObject ) :
    CAccPolObjectBase( aObjectType, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::CAccPolAudioControlBase() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::CAccPolAudioControlBase() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolAudioControlBase::CAccPolAudioControlBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioControlBase::CAccPolAudioControlBase(
    TTopologyConObjectType aObjectType, TUint32 aChannel ) :
    CAccPolObjectBase( aObjectType )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::CAccPolAudioControlBase() - Enter" );
    iChannel = aChannel;
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::CAccPolAudioControlBase() - Channel number=%d", iChannel );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::CAccPolAudioControlBase() - Return" );
    }

// Destructor
EXPORT_C CAccPolAudioControlBase::~CAccPolAudioControlBase()
    {
    }

// ---------------------------------------------------------------------------
// CAccPolAudioControlBase::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioControlBase& CAccPolAudioControlBase::operator=(
    const CAccPolAudioControlBase& aAudioControlBaseObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::operator() - Enter" );
    CAccPolObjectBase::operator=( ( CAccPolObjectBase& )aAudioControlBaseObject );
    iUnitId = aAudioControlBaseObject.UnitId();
    iChannel = aAudioControlBaseObject.Channel();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioControlBase::SetUnitId
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioControlBase::SetUnitId( TUint8 aUnitId )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::SetUnitId() - Enter" );
    iUnitId = aUnitId;
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::SetUnitId() - Unit Id=%d", iUnitId );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::SetUnitId() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolAudioControlBase::UnitId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint8 CAccPolAudioControlBase::UnitId() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::UnitId() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLICY] CAccPolAudioControlBase::UnitId() - Return unit Id=%d", iUnitId );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::UnitId() - Return" );
    return iUnitId;
    }

// -----------------------------------------------------------------------------
// CAccPolAudioControlBase::SetChannel
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioControlBase::SetChannel( TUint32 aChannel )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::SetChannel() - Enter" );
    iChannel = aChannel;
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::SetChannel() - Channel number=%d", iChannel );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::SetChannel() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolAudioControlBase::Channel
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CAccPolAudioControlBase::Channel() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::Channel() - Enter" );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::Channel() - Return channel number=%d", iChannel );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::Channel() - Return" );
    return iChannel;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioControlBase::ExternalizeAudioControlbaseL()
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAccPolAudioControlBase::ExternalizeAudioControlbaseL(
    RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW:ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::ExternalizeAudioControlbaseL() - Enter" );

    aStream.WriteUint8L( iUnitId );
    aStream.WriteUint16L( iChannel );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::ExternalizeAudioControlbaseL() - Unit Id=%d", iUnitId );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::ExternalizeAudioControlbaseL() - Channel number=%d", iChannel );

    API_TRACE_( "[AccFW:ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::ExternalizeAudioControlbaseL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioControlBase::InternalizeAudioControlbaseL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioControlBase::InternalizeAudioControlbaseL(
    RReadStream& aStream )
    {
    API_TRACE_( "[AccFW:ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::InternalizeAudioControlbaseL() - Enter" );

    iUnitId = aStream.ReadUint8L();
    iChannel = aStream.ReadUint16L();

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::InternalizeAudioControlbaseL() - Unit Id=%d", iUnitId );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::InternalizeAudioControlbaseL() - Channel number=%d", iChannel );

    API_TRACE_( "[AccFW:ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioControlBase::InternalizeAudioControlbaseL - Return" );
    }

//  End of File   
