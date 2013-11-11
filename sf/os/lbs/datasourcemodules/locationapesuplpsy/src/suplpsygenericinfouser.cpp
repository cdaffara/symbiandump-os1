/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is SUPL PSY HGenericPositionInfo helper class
*
*/



// INCLUDE FILES
#include <e32cmn.h>
#include "suplpsygenericinfouser.h"
#include "suplpsylogging.h"
#include "suplpsypanic.h"
#include "suplpsy.hrh"

// Defines data type
enum TFieldDataType
    {
    EFieldTypeTInt8,
    EFieldTypeTInt16,
    EFieldTypeTInt32,
    EFieldTypeTInt64,
    EFieldTypeTUint8,
    EFieldTypeTUint16,
    EFieldTypeTUint32,
    EFieldTypeTReal32,
    EFieldTypeTReal64,
    EFieldTypeTTime,
    EFieldTypeTTimeIntervalMicroSeconds,
    EFieldTypeDes8,
    EFieldTypeDes16
    };

// Defines struct of fields and data type
struct TFieldIdAndType
    {
    TInt  iFieldId;   //Id
    TInt  iFieldType; //Field type
    TBool iIsList;    //Field is a list
    };

// Defines all the fields that requestd by SUPL PSY
const struct TFieldIdAndType KSuplPsyFields[]=
    {
        {EPositionFieldHorizontalSpeed,         EFieldTypeTReal32, EFalse},
        {EPositionFieldHorizontalSpeedError,    EFieldTypeTReal32, EFalse},
        {EPositionFieldVerticalSpeed,           EFieldTypeTReal32, EFalse},
        {EPositionFieldVerticalSpeedError,      EFieldTypeTReal32, EFalse}
    };


// =========================== == OCAL FUNCTIONS ===============================


// ========================== == EMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// SuplPsyGenericInfoUser::SupportedFieldsCount
// -----------------------------------------------------------------------------
//
TInt SuplPsyGenericInfoUser::SupportedFieldsCount()
    {
    return sizeof( KSuplPsyFields ) / sizeof( TFieldIdAndType );
    }

// -----------------------------------------------------------------------------
// SuplPsyGenericInfoUser::SupportedFieldAt
// -----------------------------------------------------------------------------
//
TInt SuplPsyGenericInfoUser::SupportedFieldAt( TInt aIndex )
    {
    return KSuplPsyFields[aIndex].iFieldId;
    }

// -----------------------------------------------------------------------------
// SuplPsyGenericInfoUser::CopyHGenericInfo
// -----------------------------------------------------------------------------
//
TInt SuplPsyGenericInfoUser::CopyHGenericInfo( 
            const HPositionGenericInfo& aSrc,
            HPositionGenericInfo& aDes )
    {
    TRACESTRING( "SuplPsyGenericInfoUser::CopyHGenericInfo start" )
    TRACEHEX2( reinterpret_cast < const TUint8* >( &aSrc ), aSrc.PositionClassSize() )
    //Set position
    TPosition position;
    aSrc.GetPosition( position );
    aDes.SetPosition( position );
        
    TInt err( KErrNone );
    TInt fieldId = aDes.FirstRequestedFieldId();
    while ( fieldId != EPositionFieldNone )
        {
        TRACESTRING2( "FieldId=%d", fieldId )

        if ( aSrc.IsFieldAvailable( fieldId ) )
            {
            err = CopyField( aSrc, aDes, fieldId );
            if ( err != KErrNone )
                {
                return err;
                }
            }
        fieldId = aDes.NextRequestedFieldId( fieldId );
        }
    TRACESTRING( "SuplPsyGenericInfoUser::CopyHGenericInfo end" )
    return err;
    }

// -----------------------------------------------------------------------------
// SuplPsyGenericInfoUser::CopyField
// -----------------------------------------------------------------------------
//
TInt SuplPsyGenericInfoUser::CopyField( 
            const HPositionGenericInfo& aSrc,
            HPositionGenericInfo& aDes,
            TInt aFieldId )
    {
    //Get field data type
    TInt dataType;
    TBool isList;
    TInt err( KErrNone );
    err = FieldDataType( aFieldId, dataType, isList );
    if ( err != KErrNone )
        {
        return err;
        }
        
    if ( isList )
        {
        //copy list
        TUint8 count; //length of the list
        err = aSrc.GetValue( aFieldId, count );
        if ( err != KErrNone )
            {
            return err;
            }
        for ( TInt i = 0; i < count; i++ )
            {
            err = CopyDataByType( aSrc, aDes, aFieldId + 1 + i, dataType );
            if ( err != KErrNone )
                {
                return err;
                }
            }
            
        err = aDes.SetValue( aFieldId, count );
        }
    else
        {
        err = CopyDataByType( aSrc, aDes, aFieldId, dataType );
        }
        
    return err;
    }

// -----------------------------------------------------------------------------
// SuplPsyGenericInfoUser::FiledDataType
// -----------------------------------------------------------------------------
//
TInt SuplPsyGenericInfoUser::FieldDataType( 
        TInt aFieldId,
        TInt& aFieldDataType, 
        TBool& aIsList )
    {
    TInt count = SupportedFieldsCount();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( SupportedFieldAt( i ) == aFieldId )
            {
            aFieldDataType = KSuplPsyFields[i].iFieldType;
            aIsList = KSuplPsyFields[i].iIsList;
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// SuplPsyGenericInfoUser::CopyDataByType
// -----------------------------------------------------------------------------
//
TInt SuplPsyGenericInfoUser::CopyDataByType( 
            const HPositionGenericInfo& aSrc,
            HPositionGenericInfo& aDes,
            TInt aFieldId,
            TInt aDataType )
    {
    TInt err( KErrNone );
    switch ( aDataType )
        {
        case EFieldTypeTReal32:
            err = CopyData < TReal32 >( aSrc, aDes, aFieldId );
            break;
        default:
            err = KErrNotSupported;
            break;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// SuplPsyGenericInfoUser::CopyData
// -----------------------------------------------------------------------------
//
template < class T > TInt SuplPsyGenericInfoUser::CopyData( 
            const HPositionGenericInfo& aSrc,
            HPositionGenericInfo& aDes,
            TInt aFieldId )
    {
    T data;
    TInt err( KErrNone );
    err = aSrc.GetValue( aFieldId, data );
    if ( err != KErrNone )
        {
        return err;
        }
    return aDes.SetValue( aFieldId, data );
    }


//  End of File  
