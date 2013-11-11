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
* Description:  Implementation for audio selector unit object.
*
*/

#include <accpolselectorunit.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolSelectorUnit::CAccPolSelectorUnit()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolSelectorUnit::CAccPolSelectorUnit( CAccPolObjectCon* aObject ) :
    CAccPolAudioUnitBase( EAccPolAudioSelectorUnitObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::CAccPolSelectorUnit() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::CAccPolSelectorUnit() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolSelectorUnit::CAccPolSelectorUnit()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolSelectorUnit::CAccPolSelectorUnit( const TUint8 aUnitId,
    const TUint8 aNrInPins ) :
    CAccPolAudioUnitBase( aUnitId, EAccPolAudioSelectorUnitObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::CAccPolSelectorUnit() - Enter" );
    iNrInPins = aNrInPins;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::CAccPolSelectorUnit() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolSelectorUnit::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolSelectorUnit* CAccPolSelectorUnit::NewL(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::NewL() - Enter" );
    CAccPolSelectorUnit* self = CAccPolSelectorUnit::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolSelectorUnit::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolSelectorUnit* CAccPolSelectorUnit::NewLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::NewLC() - Enter" );
    CAccPolSelectorUnit* self = new ( ELeave ) CAccPolSelectorUnit( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolSelectorUnit::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolSelectorUnit* CAccPolSelectorUnit::NewL( const TUint8 aUnitId,
    const TUint8 aNrInPins )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::NewL() - Enter" );
    CAccPolSelectorUnit* self = CAccPolSelectorUnit::NewLC( aUnitId, aNrInPins );

    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolSelectorUnit::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolSelectorUnit* CAccPolSelectorUnit::NewLC( const TUint8 aUnitId,
    const TUint8 aNrInPins )

    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::NewLC() - Enter" );
    CAccPolSelectorUnit* self = new ( ELeave ) CAccPolSelectorUnit( aUnitId,
        aNrInPins );

    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolSelectorUnit::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolSelectorUnit::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::ConstructL() - Return" );
    }

// Destructor
EXPORT_C CAccPolSelectorUnit::~CAccPolSelectorUnit()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::~CAccPolSelectorUnit() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::¨CAccPolSelectorUnit() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolSelectorUnit::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolSelectorUnit& CAccPolSelectorUnit::operator=(
    const CAccPolSelectorUnit& aAudioSelectorUnitObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::operator() - Enter" );
    CAccPolAudioUnitBase::operator=(
        ( CAccPolAudioUnitBase& )aAudioSelectorUnitObject );
    iNrInPins = aAudioSelectorUnitObject.NrInPins();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolSelectorUnit::NrInPins()
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CAccPolSelectorUnit::NrInPins() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::NrInPins() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::NrInPins() - Return" );
    return iNrInPins;
    }

// ---------------------------------------------------------------------------
// CAccPolSelectorUnit::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolSelectorUnit::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::ExternalizeL() - Enter" );

    //Add object type at the beginning
    aStream.WriteUint8L( ObjectType() );
    ExternalizeAudioUnitBaseL( aStream );
    aStream.WriteUint8L( iNrInPins );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolSelectorUnit::InternalizeL()
// ---------------------------------------------------------------------------
//
void CAccPolSelectorUnit::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::InternalizeL() - Enter" );
    InternalizeAudioUnitBaseL( aStream );
    iNrInPins = aStream.ReadUint8L();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolFeatureUnit::DuplicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolSelectorUnit::DuplicateLC( CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::DuplicateLC() - Enter" );
    CAccPolSelectorUnit* aSelector = CAccPolSelectorUnit::NewLC( aObject );
    *aSelector = ( CAccPolSelectorUnit& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolSelectorUnit::DuplicateLC() - Return" );
    return aSelector;
    }

