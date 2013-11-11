// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//




// INCLUDE FILES
#include <e32cmn.h>
#include "epos_posgenericinfouser.h"

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
const struct TFieldIdAndType KGenericInfoFields[]=
    {
        {EPositionFieldComment,                 EFieldTypeDes16,   EFalse},
        //Speed
        {EPositionFieldHorizontalSpeed,         EFieldTypeTReal32, EFalse},
        {EPositionFieldHorizontalSpeedError,    EFieldTypeTReal32, EFalse},
        {EPositionFieldVerticalSpeed,           EFieldTypeTReal32, EFalse},
        {EPositionFieldVerticalSpeedError,      EFieldTypeTReal32, EFalse},
        //Direction
        {EPositionFieldTrueCourse,              EFieldTypeTReal32, EFalse},
        {EPositionFieldTrueCourseError,         EFieldTypeTReal32, EFalse},
        {EPositionFieldMagneticCourse,          EFieldTypeTReal32, EFalse},
        {EPositionFieldMagneticCourseError,     EFieldTypeTReal32, EFalse},
        //Compass
        {EPositionFieldHeading,                 EFieldTypeTReal32, EFalse},
        {EPositionFieldHeadingError,            EFieldTypeTReal32, EFalse},
        {EPositionFieldMagneticHeading,         EFieldTypeTReal32, EFalse},
        {EPositionFieldMagneticHeadingError,    EFieldTypeTReal32, EFalse},
        //Address
        {EPositionFieldCountry,                 EFieldTypeDes16,   EFalse},
        {EPositionFieldCountryCode,             EFieldTypeDes16,   EFalse},
        {EPositionFieldState,                   EFieldTypeDes16,   EFalse},
        {EPositionFieldCity,                    EFieldTypeDes16,   EFalse},
        {EPositionFieldDistrict,                EFieldTypeDes16,   EFalse},
        {EPositionFieldStreet,                  EFieldTypeDes16,   EFalse},
        {EPositionFieldStreetExtension,         EFieldTypeDes16,   EFalse},
        {EPositionFieldLocationName,            EFieldTypeDes16,   EFalse},
        {EPositionFieldPostalCode,              EFieldTypeDes16,   EFalse},
        {EPositionFieldLocality,                EFieldTypeDes16,   EFalse},
        {EPositionFieldCrossing1,               EFieldTypeDes16,   EFalse},
        {EPositionFieldCrossing2,               EFieldTypeDes16,   EFalse},
        {EPositionFieldCounty,                  EFieldTypeDes16,   EFalse},
        //Media
        {EPositionFieldMediaLinks,              EFieldTypeTUint8,  ETrue },
        {EPositionFieldMediaLinksStart,         EFieldTypeDes8,    EFalse},
        //Building
        {EPositionFieldBuildingName,            EFieldTypeDes16,   EFalse},
        {EPositionFieldBuildingFloor,           EFieldTypeDes16,   EFalse},
        {EPositionFieldBuildingRoom,            EFieldTypeDes16,   EFalse},
        {EPositionFieldBuildingZone,            EFieldTypeDes16,   EFalse},
        {EPositionFieldBuildingTelephone,       EFieldTypeDes16,   EFalse},
        //NMEA
        {EPositionFieldNMEASentences,           EFieldTypeTUint8,  ETrue },
        {EPositionFieldNMEASentencesStart,      EFieldTypeDes8,    EFalse},
        //Satellite
        {EPositionFieldSatelliteNumInView,      EFieldTypeTInt8,   EFalse},
        {EPositionFieldSatelliteNumUsed,        EFieldTypeTInt8,   EFalse},
        {EPositionFieldSatelliteTime,           EFieldTypeTTime,   EFalse},
        {EPositionFieldSatelliteHorizontalDoP,  EFieldTypeTReal32, EFalse},
        {EPositionFieldSatelliteVerticalDoP,    EFieldTypeTReal32, EFalse},
        {EPositionFieldSatelliteTimeDoP,        EFieldTypeTReal32, EFalse},
        {EPositionFieldSatellitePositionDoP,    EFieldTypeTReal32, EFalse},
        {EPositionFieldSatelliteSeaLevelAltitude,   EFieldTypeTReal32, EFalse},
        {EPositionFieldSatelliteGeoidalSeparation,  EFieldTypeTReal32, EFalse}
        //Custome fields are not supported
    };


// =========================== == OCAL FUNCTIONS ===============================


// ========================== == EMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// PosGenericInfoUser::CopyHGenericInfo
// -----------------------------------------------------------------------------
//
TInt PosGenericInfoUser::CopyHGenericInfo( 
            const HPositionGenericInfo& aSrc,
            HPositionGenericInfo& aDes )
    {
    //TRACESTRING( "PosGenericInfoUser::CopyHGenericInfo start" )
    //TRACEHEX2( reinterpret_cast < const TUint8* >( &aSrc ), aSrc.PositionClassSize() )
    //Set position
    TPosition position;
    aSrc.GetPosition( position );
    aDes.SetPosition( position );
        
    TInt err( KErrNone );
    TInt fieldId = aDes.FirstRequestedFieldId();
    while ( fieldId != EPositionFieldNone )
        {
        //TRACESTRING2( "FieldId=%d", fieldId )

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
    //TRACESTRING( "PosGenericInfoUser::CopyHGenericInfo end" )
    return err;
    }

// -----------------------------------------------------------------------------
// PosGenericInfoUser::CopyField
// -----------------------------------------------------------------------------
//
TInt PosGenericInfoUser::CopyField( 
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
// PosGenericInfoUser::FiledDataType
// -----------------------------------------------------------------------------
//
TInt PosGenericInfoUser::FieldDataType( 
        TInt aFieldId,
        TInt& aFieldDataType, 
        TBool& aIsList )
    {
    TInt count = sizeof( KGenericInfoFields ) / sizeof( TFieldIdAndType );
    for ( TInt i = 0; i < count; i++ )
        {
        if ( KGenericInfoFields[ i ].iFieldId == aFieldId )
            {
            aIsList = KGenericInfoFields[i].iIsList;
            if( !aIsList )
                {
                aFieldDataType = KGenericInfoFields[i].iFieldType;
                }
            else
                {
                // coverity [overrun-local]
                // iIsList of last element in statically defined array must always be False, therefore this cannot cause an overrun.
                aFieldDataType = KGenericInfoFields[i+1].iFieldType;
                }
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// PosGenericInfoUser::CopyDataByType
// -----------------------------------------------------------------------------
//
TInt PosGenericInfoUser::CopyDataByType( 
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
        case EFieldTypeTUint8:
            err = CopyData < TUint8 >( aSrc, aDes, aFieldId );
            break;
        case EFieldTypeTInt8:
            err = CopyData < TInt8 >( aSrc, aDes, aFieldId );
            break;
        case EFieldTypeTTime:
            err = CopyData < TTime >( aSrc, aDes, aFieldId );
            break;
        case EFieldTypeDes16:
            err = CopyData < TPtrC16 > (aSrc, aDes, aFieldId );
            break;
        case EFieldTypeDes8:
            err = CopyData < TPtrC8 > (aSrc, aDes, aFieldId );
            break;
        default:
            err = KErrNotSupported;
            break;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// PosGenericInfoUser::CopyData
// -----------------------------------------------------------------------------
//
template < class T > TInt PosGenericInfoUser::CopyData( 
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
