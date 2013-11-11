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
* Description:  Container implementation for accessory policy name/value pair objects.
*
*/

#include <accpolaudiostreamformatcon.h>
#include <accpolobjecttypes.h>
#include <accpolaudiostreamformat.h>

#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolObjectCon::CAccPolObjectCon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolAudioStreamFormatCon::CAccPolAudioStreamFormatCon(
    CAccPolObjectCon* aObject ) :
    CAccPolObjectCon( EAccPolStreamObjectCon, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::CAccPolAudioStreamFormatCon() - Enter " );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::CAccPolAudioStreamFormatCon() - Return" );
    }

// Destructor
CAccPolAudioStreamFormatCon::~CAccPolAudioStreamFormatCon()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::~CAccPolAudioStreamFormatCon() - Enter " );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::~CAccPolAudioStreamFormatCon() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormatCon::ConstructL
// ---------------------------------------------------------------------------
//
void CAccPolAudioStreamFormatCon::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::ConstructL() - Enter " );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::ConstructL() - Return" );
    }
// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormatCon::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioStreamFormatCon* CAccPolAudioStreamFormatCon::NewL(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::NewL() - Enter " );
    CAccPolAudioStreamFormatCon* self = CAccPolAudioStreamFormatCon::NewLC(
        aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormatCon::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioStreamFormatCon* CAccPolAudioStreamFormatCon::NewLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::NewLC() - Enter " );
    CAccPolAudioStreamFormatCon* self =
        new ( ELeave ) CAccPolAudioStreamFormatCon( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormatCon::GetInputStreamFormatsL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioStreamFormatCon::GetInputStreamFormatsL(
    RPointerArray<CAccPolAudioStreamFormat>& aInputStreamFormats ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::GetInputStreamFormatsL() - Enter" );

    CAccPolObjectBase* ptr = NULL;
    TInt count( Count() );

    for( TInt i = 0; i < count; i++ )
        {
        ptr = AtL( i );
        if( ptr->ObjectType() == EAccPolAudioInputStreamObject )
            {
            aInputStreamFormats.AppendL(
                reinterpret_cast<CAccPolAudioStreamFormat*> ( ptr ) );
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::GetInputStreamFormatsL() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioStreamFormatCon::GetOutputStreamFormatsL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioStreamFormatCon::GetOutputStreamFormatsL(
    RPointerArray<CAccPolAudioStreamFormat>& aOuputStreamFormats ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::GetOutputStreamFormatsL() - Enter" );

    CAccPolObjectBase* ptr = NULL;
    TInt count( Count() );

    for( TInt i = 0; i < count; i++ )
        {
        ptr = AtL( i );
        if( ptr->ObjectType() == EAccPolAudioOutputStreamObject )
            {
            aOuputStreamFormats.AppendL(
                reinterpret_cast<CAccPolAudioStreamFormat*> ( ptr ) );
            }
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::GetOutputStreamFormatsL() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::InternalizeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioStreamFormatCon::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::InternalizeL() - Enter" );

    TUint8 objectType;
    TUint32 count;

    // This object
    SetObjectType( aStream.ReadUint8L() );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::InternalizeBaseL - Object Type=%d", ObjectType() );

    count = aStream.ReadUint8L();//Number of objects inside this container
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::InternalizeL - object count = %d",count );

    for( TInt i = 0; i < count; i++ )
        {
        objectType = aStream.ReadUint8L();
        API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::InternalizeL - object type  = %d",objectType );

        switch( objectType )
            {
            case EAccPolAudioInputStreamObject:
                {
                CAccPolAudioStreamFormat *audioStream =
                    CAccPolAudioStreamFormat::NewLC(
                        EAccPolAudioInputStreamObject );
                audioStream->InternalizeL( aStream );
                AddL( *audioStream );
                CleanupStack::PopAndDestroy( audioStream );
                }
                break;

            case EAccPolAudioOutputStreamObject:
                {
                CAccPolAudioStreamFormat *audioStream =
                    CAccPolAudioStreamFormat::NewLC(
                        EAccPolAudioOutputStreamObject );
                audioStream->InternalizeL( aStream );
                AddL( *audioStream );
                CleanupStack::PopAndDestroy( audioStream );
                }
                break;
            default:
                {
                User::Leave( KErrNotSupported );
                }
                break;
            }
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::ExternalizeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioStreamFormatCon::ExternalizeL( RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::ExternalizeL() - Enter" );
    TInt count( Count() );

    aStream.WriteUint8L( ObjectType() );// Type of object
    aStream.WriteUint8L( count );//Count

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::ExternalizeL - object type  = %d", ObjectType() );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::ExternalizeL - object count = %d", count );

    for( TInt i = 0; i < count; i++ )
        {
        AtL( i )->ExternalizeL( aStream );
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::DublicateL()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolAudioStreamFormatCon::DuplicateLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::DublicateL()" );
    CAccPolAudioStreamFormatCon* aContainer =
        CAccPolAudioStreamFormatCon::NewLC( aObject );
    TInt count( Count() );
    for( TInt i = 0; i < count; i++ )
        {
        aContainer->AddL( *AtL( i ) );
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioStreamFormatCon::DublicateL()" );
    return aContainer;
    }

//  End of File    
