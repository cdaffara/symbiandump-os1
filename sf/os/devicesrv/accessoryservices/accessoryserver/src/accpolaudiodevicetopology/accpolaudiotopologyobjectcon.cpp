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
* Description:  Container implementation for accessory policy name/value pair objects.
*
*/

#include <accpolaudiotopologyobjectcon.h>
#include <accpolmixerunit.h>
#include <accpolfeatureunit.h>
#include <accpolselectorunit.h>
#include <accpolvolumecontrol.h>
#include <accpolmutecontrol.h>
#include <accpolinputcontrol.h>

#include "acc_debug.h"

// -----------------------------------------------------------------------------
// CAccPolObjectCon::CAccPolObjectCon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccPolAudioTopologyObjectCon::CAccPolAudioTopologyObjectCon(
    CAccPolObjectCon* aObject ) :
    CAccPolObjectCon( EAccPolTopologyObjectCon, aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::CAccPolAudioTopologyObjectCon() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::CAccPolAudioTopologyObjectCon() - Return" );
    }

// Destructor
CAccPolAudioTopologyObjectCon::~CAccPolAudioTopologyObjectCon()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::~CAccPolAudioTopologyObjectCon() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::~CAccPolAudioTopologyObjectCon() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::ConstructL
// ---------------------------------------------------------------------------
//
void CAccPolAudioTopologyObjectCon::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::ConstructL() - Enter" );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::ConstructL() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::GetAudioUnits()
// ---------------------------------------------------------------------------
//
void CAccPolAudioTopologyObjectCon::GetAudioUnitsL(
    RPointerArray<CAccPolAudioUnitBase>& aAudioUnits ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::GetAudioUnitsL() - Enter" );
    TInt count( Count() );
    CAccPolObjectBase* ptr = NULL;
    for( TInt i = 0; i < count; i++ )
        {
        ptr = AtL( i );
        if(    ptr->ObjectType() == EAccPolAudioInputTerminalObject
            || ptr->ObjectType() == EAccPolAudioOutputTerminalObject
            || ptr->ObjectType() == EAccPolAudioMixerUnitObject
            || ptr->ObjectType() == EAccPolAudioFeatureUnitObject
            || ptr->ObjectType() == EAccPolAudioSelectorUnitObject )
            {
            aAudioUnits.AppendL( reinterpret_cast<CAccPolAudioUnitBase*> ( ptr ) );
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::GetAudioUnitsL() - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolObjectCon::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioTopologyObjectCon* CAccPolAudioTopologyObjectCon::NewL(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::NewL() - Enter" );
    CAccPolAudioTopologyObjectCon* self = CAccPolAudioTopologyObjectCon::NewLC(
        aObject );
    CleanupStack::Pop( self );
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::NewL() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolObjectCon::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CAccPolAudioTopologyObjectCon* CAccPolAudioTopologyObjectCon::NewLC(
    CAccPolObjectCon* aObject )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::NewLC() - Enter" );
    CAccPolAudioTopologyObjectCon* self =
        new ( ELeave ) CAccPolAudioTopologyObjectCon( aObject );
    CleanupStack::PushL( self );
    self->ConstructL();
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::NewLC() - Return" );
    return self;
    }

// ---------------------------------------------------------------------------
// CAccPolObjectCon::GetObjects()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioTopologyObjectCon::GetObjectsByTypeL(
    TTopologyConObjectType aObjectType,
    RPointerArray<CAccPolObjectBase>& aObjects ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::GetObjectsByTypeL() - Enter" );

    CAccPolObjectBase* ptr = NULL;
    TInt count( Count() );

    for( TInt i = 0; i < count; i++ )
        {
        ptr = AtL( i );
        if( ptr->ObjectType() == aObjectType )
            {
            API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::GetObjectsByTypeL() - add object - type=%d ", ptr->ObjectType() );
            aObjects.AppendL( ptr );
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::GetObjectsByTypeL() - return void" );
    }

// ---------------------------------------------------------------------------
// CAccPolObjectCon::InputTerminalL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolAudioTopologyObjectCon::InputTerminalL(
    CAccPolInputTerminal::TInputTerminalType aType,
    CAccPolAudioUnitBase*& aObject ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::InputTerminalL() - Enter " );

    TInt count( Count() );
    TInt index( 0 );
    TBool bFound( EFalse );

    CAccPolObjectBase* ptr = NULL;
    for( index = 0; index < count; index++ )
        {
        ptr = AtL( index );
        if( ptr->ObjectType() == EAccPolAudioInputTerminalObject
            && reinterpret_cast<CAccPolInputTerminal*> ( ptr )->InputTerminalType()
                == aType )
            {
            API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::InputTerminalL() - Object found" );
            aObject = reinterpret_cast<CAccPolInputTerminal*> ( ptr );
            bFound = ETrue;
            break;
            }
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::InputTerminalL() - Return" );

    return bFound;
    }

// ---------------------------------------------------------------------------
// CAccPolObjectCon::OuputTerminalL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolAudioTopologyObjectCon::OutputTerminalL(
    CAccPolOutputTerminal::TOutputTerminalType aType,
    CAccPolAudioUnitBase*& aObject ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::OuputTerminalL() - Enter " );

    TInt count( Count() );
    TInt index( 0 );
    TBool bFound( EFalse );

    CAccPolObjectBase* ptr = NULL;
    for( index = 0; index < count; index++ )
        {
        ptr = AtL( index );
        if( ptr->ObjectType() == EAccPolAudioOutputTerminalObject
            && reinterpret_cast<CAccPolOutputTerminal*> ( ptr )->OutputTerminalType()
                == aType )
            {
            API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::OuputTerminalL() - Object found" );
            aObject = reinterpret_cast<CAccPolOutputTerminal*> ( ptr );
            bFound = ETrue;
            break;
            }
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::OuputTerminalL() - Return" );

    return bFound;
    }

// ---------------------------------------------------------------------------
// CAccPolObjectCon::ObjectL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAccPolAudioTopologyObjectCon::ObjectL( TUint8 aUnitId,
    CAccPolAudioUnitBase*& aObject ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::ObjectL() - Enter " );

    TInt count( Count() );
    CAccPolObjectBase* ptr = NULL;
    TInt index = 0;
    TBool bFound( EFalse );

    if( count == 0 )
        {
        API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::ObjectL - Container is empty!" );
        User::Leave( KErrGeneral );
        }
    else
        {
        for( index = 0; index < count; index++ )
            {
            ptr = AtL( index );

            if(    ptr->ObjectType() == EAccPolAudioInputTerminalObject
                || ptr->ObjectType() == EAccPolAudioOutputTerminalObject
                || ptr->ObjectType() == EAccPolAudioMixerUnitObject
                || ptr->ObjectType() == EAccPolAudioFeatureUnitObject
                || ptr->ObjectType() == EAccPolAudioSelectorUnitObject )
                {
                CAccPolAudioUnitBase* audioUnitPtr =
                    reinterpret_cast<CAccPolAudioUnitBase*> ( ptr );
                if( audioUnitPtr->UnitId() == aUnitId )
                    {
                    aObject = audioUnitPtr;
                    bFound = ETrue;
                    break;
                    }
                }
            }
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::ObjectL() - Return" );
    return bFound;
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::InternalizeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioTopologyObjectCon::InternalizeL( RReadStream& aStream )
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolObjectCon::InternalizeL() - Enter" );

    TUint8 objectType;
    TUint32 count;

    // This object
    SetObjectType( aStream.ReadUint8L() );

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolObjectBase::InternalizeBaseL - Object Type=%d", ObjectType() );

    count = aStream.ReadUint8L();//Number of objects inside this container
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolObjectCon::InternalizeL - object count = %d",count );

    for( TInt i = 0; i < count; i++ )
        {
        objectType = aStream.ReadUint8L();
        API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolObjectCon::InternalizeL - object type  = %d",objectType );

        switch( objectType )
            {
            case EAccPolAudioInputTerminalObject:
                {
                CAccPolInputTerminal *inputTerminal =
                    CAccPolInputTerminal::NewLC();
                inputTerminal->InternalizeL( aStream );
                AddL( *inputTerminal );
                CleanupStack::PopAndDestroy( inputTerminal );
                }
                break;

            case EAccPolAudioOutputTerminalObject:
                {
                CAccPolOutputTerminal *outputTerminal =
                    CAccPolOutputTerminal::NewLC();
                outputTerminal->InternalizeL( aStream );
                AddL( *outputTerminal );
                CleanupStack::PopAndDestroy( outputTerminal );
                }
                break;

            case EAccPolAudioMixerUnitObject:
                {
                CAccPolMixerUnit *mixerUnit = CAccPolMixerUnit::NewLC();
                mixerUnit->InternalizeL( aStream );
                AddL( *mixerUnit );
                CleanupStack::PopAndDestroy( mixerUnit );
                }
                break;

            case EAccPolAudioFeatureUnitObject:
                {
                CAccPolFeatureUnit *featureUnit = CAccPolFeatureUnit::NewLC();
                featureUnit->InternalizeL( aStream );
                AddL( *featureUnit );
                CleanupStack::PopAndDestroy( featureUnit );
                }
                break;

            case EAccPolAudioSelectorUnitObject:
                {
                CAccPolSelectorUnit *selectorUnit =
                    CAccPolSelectorUnit::NewLC();
                selectorUnit->InternalizeL( aStream );
                AddL( *selectorUnit );
                CleanupStack::PopAndDestroy( selectorUnit );
                }
                break;

            case EAccPolAudioVolumeControlObject:
                {
                CAccPolVolumeControl *volumeControl =
                    CAccPolVolumeControl::NewLC();
                volumeControl->InternalizeL( aStream );
                AddL( *volumeControl );
                CleanupStack::PopAndDestroy( volumeControl );
                }
                break;

            case EAccPolAudioMuteControlObject:
                {
                CAccPolMuteControl *muteControl = CAccPolMuteControl::NewLC();
                muteControl->InternalizeL( aStream );
                AddL( *muteControl );
                CleanupStack::PopAndDestroy( muteControl );
                }
                break;
            case EAccPolAudioInputControlObject:
                {
                CAccPolInputControl *inputControl =
                    CAccPolInputControl::NewLC();
                inputControl->InternalizeL( aStream );
                AddL( *inputControl );
                CleanupStack::PopAndDestroy( inputControl );
                }
                break;
            default:
                {
                User::Leave( KErrNotSupported );
                }
                break;
            }
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolObjectCon::InternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::ExternalizeL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAccPolAudioTopologyObjectCon::ExternalizeL(
    RWriteStream& aStream ) const
    {
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolObjectCon::ExternalizeL() - Enter" );
    TInt count( Count() );

    aStream.WriteUint8L( ObjectType() );// Type of object
    aStream.WriteUint8L( Count() );//Count

    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolObjectCon::ExternalizeL - object type  = %d", ObjectType() );
    API_TRACE_1( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolObjectCon::ExternalizeL - object count = %d", count );

    for( TInt i = 0; i < count; i++ )
        {
        AtL( i )->ExternalizeL( aStream );
        }

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolObjectCon::ExternalizeL - Return" );
    }

// ---------------------------------------------------------------------------
// CAccPolAudioTopologyObjectCon::DuplicateLC()
// ---------------------------------------------------------------------------
//
CAccPolObjectBase* CAccPolAudioTopologyObjectCon::DuplicateLC(
    CAccPolObjectCon* aObject )
    {

    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::DublicateL() - Enter" );
    CAccPolAudioTopologyObjectCon* aContainer =
        CAccPolAudioTopologyObjectCon::NewLC( aObject );
    TInt count( Count() );
    for( TInt i = 0; i < count; i++ )
        {
        aContainer->AddL( *AtL( i ) );
        }
    API_TRACE_( "[AccFW: ACCPOLAUDIODEVICETOPOLOGY] CAccPolAudioTopologyObjectCon::DublicateL() - Return" );
    return aContainer;
    }

//  End of File
