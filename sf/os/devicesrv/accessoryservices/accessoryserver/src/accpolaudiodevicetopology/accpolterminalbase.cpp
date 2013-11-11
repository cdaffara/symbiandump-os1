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
* Description:  Implementation for terminal base class.
*
*/
 
#include <accpolterminalbase.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolTerminalBase::CAccPolTerminalBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolTerminalBase::CAccPolTerminalBase(
    const TTopologyConObjectType aObjecType,
    CAccPolObjectCon* aObject ) :
    CAccPolAudioUnitBase( aObjecType, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::CAccPolTerminalBase() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::CAccPolTerminalBase() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolTerminalBase::CAccPolTerminalBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolTerminalBase::CAccPolTerminalBase(
    const TTopologyConObjectType aObjecType,
    const TUint8 aUnitId ) :
    CAccPolAudioUnitBase( aUnitId, aObjecType )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::CAccPolTerminalBase() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::CAccPolTerminalBase() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolTerminalBase::CAccPolTerminalBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccPolTerminalBase::CAccPolTerminalBase(
    const TTopologyConObjectType aObjecType,
    const TUint8 aUnitId,
    const TUint8 aSourceId ) :
    CAccPolAudioUnitBase( aUnitId, aSourceId, aObjecType )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::CAccPolTerminalBase() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::CAccPolTerminalBase() - Return" );
    }

// Destructor
EXPORT_C CAccPolTerminalBase::~CAccPolTerminalBase()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::~CAccPolTerminalBase() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::~CAccPolTerminalBase() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolTerminalBase::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolTerminalBase& CAccPolTerminalBase::operator=(
    const CAccPolTerminalBase& aTerminalBaseObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::operator() - Enter" );
    CAccPolAudioUnitBase::operator=(
        ( CAccPolAudioUnitBase& )aTerminalBaseObject );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolTerminalBase::ExternalizeBaseLd
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolTerminalBase::ExternalizeTerminalBaseL(
    RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::ExternalizeTerminalBaseL() - Enter" );

    ExternalizeAudioUnitBaseL( aStream );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::ExternalizeTerminalBaseL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolTerminalBase::InternalizeBaseL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolTerminalBase::InternalizeTerminalBaseL(
    RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::InternalizeTerminalBaseL() - Enter" );

    InternalizeAudioUnitBaseL( aStream );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolTerminalBase::InternalizeTerminalBaseL - Return" );
    }
