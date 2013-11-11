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
* Description:  Compensator for magnetic north
*
*/


#include <sensordatacompensatorplugintypes.h>
#include <sensordatacompensatorplugin.h>
#include <sensrvmagneticnorthsensor.h>
#include "sensordatacompensatormagneticnorthdatahandler.h"
#include "trace.h"

const TInt KDeg360 = 360;

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CSensorDataCompensatorMagneticNorthDataHandler::NewL
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorMagneticNorthDataHandler* CSensorDataCompensatorMagneticNorthDataHandler::NewL( 
                CSensorDataCompensatorPlugin& aPlugin )
    {
	
	FUNC_LOG;
    return new ( ELeave )CSensorDataCompensatorMagneticNorthDataHandler( aPlugin );
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorMagneticNorthDataHandler::~CSensorDataCompensatorMagneticNorthDataHandler
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorMagneticNorthDataHandler::~CSensorDataCompensatorMagneticNorthDataHandler()
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorMagneticNorthDataHandler::Compensate
// ---------------------------------------------------------------------------
//
TInt CSensorDataCompensatorMagneticNorthDataHandler::Compensate( TDes8& aData )
    {    
    FUNC_LOG;
    TInt err( KErrNone );

    TPckgBuf<TSensrvMagneticNorthData> pckg;
    if ( aData.MaxSize() == pckg.MaxSize() )
        {
        pckg.Copy( aData );
        // angle compensation
        TInt angle( pckg().iAngleFromMagneticNorth );
        INFO_1( "Used magnetic north compesation angle %d", angle );
        angle -= iAngle;
        angle %= KDeg360;
        if ( angle < 0 )
            {
            angle += KDeg360;
            }
        INFO_1( "Compensated magnetic north %d", angle );
        pckg().iAngleFromMagneticNorth = angle;
        aData.Copy( pckg );
        }
    else
        {
        err = KErrArgument;
        }

    ERROR( err, "Magnetic north compensation error" );
    return err;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorTappingDataHandler::Update
// ---------------------------------------------------------------------------
//
TInt CSensorDataCompensatorMagneticNorthDataHandler::Update()
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TInt count( iPlugin.GetCompensationItemCount() );
    INFO_1( "Magnetic north compensation item count %d", count );
    iAngle = 0; // Reset angle
    if ( count >= 0 )
        {
        TPckgBuf<TCompensationTypeAngleData> pckg;
        for ( TInt i( 0 ) ; i < count; i++ )
            {
            TPtrC8 data( iPlugin.GetCompensationItem( i ) );
            if ( data.Size() == pckg.MaxSize() )
                {
                pckg.Copy( data );
                iAngle += pckg().iAngle;
                }
            else
                {
                err = KErrCorrupt;
                break;
                }
            }
        }
    else
        {
        // Error occured in plugin
        err = count;
        }
    iCompensate = ( count > 0 ) && ( err == KErrNone ); // Ready to compensate
    ERROR( err, "Magnetic north update error" );
    return err;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorTappingDataHandler::CSensorDataCompensatorMagneticNorthDataHandler
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorMagneticNorthDataHandler::CSensorDataCompensatorMagneticNorthDataHandler(
                        CSensorDataCompensatorPlugin& aPlugin) :
    CSensorDataCompensatorDataHandlerBase( aPlugin )
    {
    FUNC_LOG;
    }
