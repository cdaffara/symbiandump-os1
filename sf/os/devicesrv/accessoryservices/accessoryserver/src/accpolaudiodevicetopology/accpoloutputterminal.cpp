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
* Description:  Accessory audio stream format.
*
*/

#include <accpoloutputterminal.h>
#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolOutputTerminal::CAccPolOutputTerminal()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolOutputTerminal::CAccPolOutputTerminal( CAccPolObjectCon* aObject ) :
    CAccPolTerminalBase( EAccPolAudioOutputTerminalObject, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::CAccPolOutputTerminal() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::CAccPolOutputTerminal() - Return" );
    }

// -----------------------------------------------------------------------------
// CAccPolOutputTerminal::CAccPolOutputTerminal()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolOutputTerminal::CAccPolOutputTerminal( const TUint8 aUnitId,
    const TOutputTerminalType aType,
    const TUint8 aSourceId ) :
    CAccPolTerminalBase( EAccPolAudioOutputTerminalObject, aUnitId, aSourceId )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::CAccPolOutputTerminal() - Enter" );
    iOuputTerminalType = aType;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::CAccPolOutputTerminal() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolOutputTerminal* CAccPolOutputTerminal::NewL(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::NewL() - Enter" );
    CAccPolOutputTerminal* self = CAccPolOutputTerminal::NewLC( aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolOutputTerminal* CAccPolOutputTerminal::NewLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::NewLC() - Enter" );
    CAccPolOutputTerminal* self =
        new ( ELeave ) CAccPolOutputTerminal( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolOutputTerminal* CAccPolOutputTerminal::NewL(
    const TUint8 aUnitId,
    const TOutputTerminalType aType,
    const TUint8 aSourceId )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::NewL() - Enter" );
    CAccPolOutputTerminal* self = CAccPolOutputTerminal::NewLC( aUnitId, aType,
        aSourceId );

    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::NewL()- Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolOutputTerminal* CAccPolOutputTerminal::NewLC(
    const TUint8 aUnitId,
    const TOutputTerminalType aType,
    const TUint8 aSourceId )

    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::NewLC() - Enter" );
    CAccPolOutputTerminal* self = new ( ELeave ) CAccPolOutputTerminal(
        aUnitId, aType, aSourceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::ConstructL()
// ---------------------------------------------------------------------------
//
void CAccPolOutputTerminal::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::ConstructL() - Return" );
    }

// Destructor
EXPORT_C CAccPolOutputTerminal::~CAccPolOutputTerminal()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::~CAccPolOutputTerminal() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::~CAccPolOutputTerminal() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::operator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolOutputTerminal& CAccPolOutputTerminal::operator=(
    const CAccPolOutputTerminal& aOutputTerminalObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::operator() - Enter" );
    CAccPolTerminalBase::operator=(
        ( CAccPolTerminalBase& )aOutputTerminalObject );
    iOuputTerminalType = aOutputTerminalObject.OutputTerminalType();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::operator() - Return" );
    return ( *this );
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::OutputTerminalType()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolOutputTerminal::TOutputTerminalType CAccPolOutputTerminal::OutputTerminalType() const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::OutputTerminalType() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::OutputTerminalType() - Return" );
    return iOuputTerminalType;
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::SetOutputTerminalType()
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAccPolOutputTerminal::SetOutputTerminalType(
    TOutputTerminalType aOutputTerminalType )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::SetOutputTerminalType() - Enter" );
    iOuputTerminalType = aOutputTerminalType;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::SetOutputTerminalType() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::ExternalizeL()
// ---------------------------------------------------------------------------
//   
void CAccPolOutputTerminal::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::ExternalizeL() - Enter" );

    //Add object type at the beginning
    aStream.WriteUint8L( ObjectType() );

    //Externalize base class.    
    ExternalizeTerminalBaseL( aStream );

    //Object data    
    aStream.WriteUint8L( OutputTerminalType() );

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::InternalizeL()
// ---------------------------------------------------------------------------
//  
void CAccPolOutputTerminal::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::InternalizeL() - Enter" );

    // Internalize base class
    InternalizeTerminalBaseL( aStream );

    // Object data
    iOuputTerminalType
        = ( CAccPolOutputTerminal::TOutputTerminalType )aStream.ReadUint8L();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolOutputTerminal::DublicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolOutputTerminal::DuplicateLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::DublicateLC() - Enter" );
    CAccPolOutputTerminal* aOutput = CAccPolOutputTerminal::NewLC( aObject );
    *aOutput = ( CAccPolOutputTerminal& )*this;
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolOutputTerminal::DublicateLC() - Return" );
    return aOutput;
    }
