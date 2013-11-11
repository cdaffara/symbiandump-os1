/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Compensator for tapping
*
*/


#include <sensordatacompensatorplugintypes.h>
#include <sensordatacompensatorplugin.h>
#include <sensrvtappingsensor.h>
#include "sensordatacompensatortappingdatahandler.h"
#include "trace.h"


// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CSensorDataCompensatorTappingDataHandler::NewL
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorTappingDataHandler* CSensorDataCompensatorTappingDataHandler::NewL( 
                                          CSensorDataCompensatorPlugin& aPlugin )
    {
    FUNC_LOG;
    return new ( ELeave )CSensorDataCompensatorTappingDataHandler( aPlugin );
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorTappingDataHandler::~CSensorDataCompensatorTappingDataHandler
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorTappingDataHandler::~CSensorDataCompensatorTappingDataHandler()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorTappingDataHandler::Compensate
// ---------------------------------------------------------------------------
//
TInt CSensorDataCompensatorTappingDataHandler::Compensate( TDes8& aData )
    {
    FUNC_LOG;
    TInt err( KErrNone );

    TPckgBuf<TSensrvTappingData> pckg;
    if ( aData.MaxSize() == pckg.MaxSize() )
        {
        pckg.Copy( aData );
        // tapping direction compensation
        TUint32 direction( pckg().iDirection );
        TUint32 compDirX( 0 );

        compDirX |= ( direction & ( KSensrvAccelerometerDirectionXplus | KSensrvAccelerometerDirectionXminus ) );

        switch ( iXRotation )
            {
            case TCompensationTypeDirectionData::ECw90:
                {
                if ( direction & KSensrvAccelerometerDirectionZplus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionYminus;
                    }
                if ( direction & KSensrvAccelerometerDirectionZminus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionYplus;
                    }
                if ( direction & KSensrvAccelerometerDirectionYplus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionZplus;
                    }
                if ( direction & KSensrvAccelerometerDirectionYminus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionZminus;
                    }
                break;
                }
            case TCompensationTypeDirectionData::ECw180:
                {
                if ( direction & KSensrvAccelerometerDirectionZplus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionZminus;
                    }
                if ( direction & KSensrvAccelerometerDirectionZminus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionZplus;
                    }
                if ( direction & KSensrvAccelerometerDirectionYplus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionYminus;
                    }
                if ( direction & KSensrvAccelerometerDirectionYminus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionYplus;
                    }
                break;
                }
            case TCompensationTypeDirectionData::ECw270:
                {
                if ( direction & KSensrvAccelerometerDirectionZplus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionYplus;
                    }
                if ( direction & KSensrvAccelerometerDirectionZminus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionYminus;
                    }
                if ( direction & KSensrvAccelerometerDirectionYplus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionZminus;
                    }
                if ( direction & KSensrvAccelerometerDirectionYminus )
                    {
                    compDirX |= KSensrvAccelerometerDirectionZplus;
                    }
                break;
                }
            case TCompensationTypeDirectionData::ECw0: // Fall through
            default:
                {
                INFO( "No compensation required about x-axis" );

                compDirX = direction;

                break;
                }
            }

        TUint compDirXY( 0 );

		compDirXY |= ( compDirX & ( KSensrvAccelerometerDirectionYplus | KSensrvAccelerometerDirectionYminus ) );


        switch ( iYRotation )
            {
            case TCompensationTypeDirectionData::ECw90:
                {
                if ( compDirX & KSensrvAccelerometerDirectionXplus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionZminus;
                    }
                if ( compDirX & KSensrvAccelerometerDirectionXminus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionZplus;
                    }
                if ( compDirX & KSensrvAccelerometerDirectionZplus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionXplus;
                    }
                if ( compDirX & KSensrvAccelerometerDirectionZminus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionXminus;
                    }
                break;
                }
            case TCompensationTypeDirectionData::ECw180:
                {
                if ( compDirX & KSensrvAccelerometerDirectionXplus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionXminus;
                    }
                if ( compDirX & KSensrvAccelerometerDirectionXminus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionXplus;
                    }
                if ( compDirX & KSensrvAccelerometerDirectionZplus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionZminus;
                    }
                if ( compDirX & KSensrvAccelerometerDirectionZminus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionZplus;
                    }
                break;
                }
            case TCompensationTypeDirectionData::ECw270:
                {
                if ( compDirX & KSensrvAccelerometerDirectionXplus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionZplus;
                    }
                if ( compDirX & KSensrvAccelerometerDirectionXminus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionZminus;
                    }
                if ( compDirX & KSensrvAccelerometerDirectionZplus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionXminus;
                    }
                if ( compDirX & KSensrvAccelerometerDirectionZminus )
                    {
                    compDirXY |= KSensrvAccelerometerDirectionXplus;
                    }
                break;
                }
            case TCompensationTypeDirectionData::ECw0: // Fall through
            default:
                {
                INFO( "No compensation required about y-axis" );

                compDirXY = compDirX;

                break;
                }
            }

        TUint compDirXYZ( 0 );

		compDirXYZ |= ( compDirXY & ( KSensrvAccelerometerDirectionZplus | KSensrvAccelerometerDirectionZminus ) );

        switch ( iZRotation )
            {
            case TCompensationTypeDirectionData::ECw90:
                {
                if ( compDirXY & KSensrvAccelerometerDirectionXplus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionYplus;
                    }
                if ( compDirXY & KSensrvAccelerometerDirectionXminus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionYminus;
                    }
                if ( compDirXY & KSensrvAccelerometerDirectionYplus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionXminus;
                    }
                if ( compDirXY & KSensrvAccelerometerDirectionYminus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionXplus;
                    }
                break;
                }
            case TCompensationTypeDirectionData::ECw180:
                {
                if ( compDirXY & KSensrvAccelerometerDirectionXplus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionXminus;
                    }
                if ( compDirXY & KSensrvAccelerometerDirectionXminus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionXplus;
                    }
                if ( compDirXY & KSensrvAccelerometerDirectionYplus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionYminus;
                    }
                if ( compDirXY & KSensrvAccelerometerDirectionYminus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionYplus;
                    }
                break;
                }
            case TCompensationTypeDirectionData::ECw270:
                {
                if ( compDirXY & KSensrvAccelerometerDirectionXplus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionYminus;
                    }
                if ( compDirXY & KSensrvAccelerometerDirectionXminus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionYplus;
                    }
                if ( compDirXY & KSensrvAccelerometerDirectionYplus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionXplus;
                    }
                if ( compDirXY & KSensrvAccelerometerDirectionYminus )
                    {
                    compDirXYZ |= KSensrvAccelerometerDirectionXminus;
                    }
                break;
                }
            case TCompensationTypeDirectionData::ECw0: // Fall through
            default:
                {
                INFO( "No compensation required about z-axis" );

                compDirXYZ = compDirXY;

                break;
                }
            }

        // Nothing got compensated, but there was data,
        // return the original data
        if ( !compDirXYZ && direction )
            {
            compDirXYZ = direction;
            }
        pckg().iDirection = compDirXYZ;
        aData.Copy( pckg );
        }
    else
        {
        err = KErrArgument;
        }

    ERROR( err, "Tapping compensation error" );
    return err;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorTappingDataHandler::Update
// ---------------------------------------------------------------------------
//
TInt CSensorDataCompensatorTappingDataHandler::Update()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TInt count( iPlugin.GetCompensationItemCount() );
    INFO_1( "Tapping compensation item count %d", count );
    iXRotation = TCompensationTypeDirectionData::ECw0; // Reset compensation
    iYRotation = TCompensationTypeDirectionData::ECw0;
    iZRotation = TCompensationTypeDirectionData::ECw0;
    if ( count >= 0 )
        {
        // Only the first compensation item is used
        TPtrC8 data( iPlugin.GetCompensationItem( 0 ) );
        TPckgBuf<TCompensationTypeDirectionData> pckg;
        if ( data.Size() == pckg.MaxSize() )
            {
            pckg.Copy( data );
            iXRotation = pckg().iTappingAxisX;
            iYRotation = pckg().iTappingAxisY;
            iZRotation = pckg().iTappingAxisZ;
            }
        else
            {
            err = KErrCorrupt;
            }
        }
    else
        {
        // Error occured in plugin
        err = count;
        }
    iCompensate = ( count > 0 ) && ( err == KErrNone ); // Ready to compensate
    ERROR( err, "Tapping update error" );
    return err;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorTappingDataHandler::CSensorDataCompensatorTappingDataHandler
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorTappingDataHandler::CSensorDataCompensatorTappingDataHandler(
                                          CSensorDataCompensatorPlugin& aPlugin )
    : CSensorDataCompensatorDataHandlerBase(aPlugin)
    {
    FUNC_LOG;
    }
