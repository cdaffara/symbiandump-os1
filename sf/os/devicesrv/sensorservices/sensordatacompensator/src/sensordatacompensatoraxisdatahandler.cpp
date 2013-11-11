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
* Description:  Compensator for axis data
*
*/


#include <sensordatacompensatorplugintypes.h>
#include <sensordatacompensatorplugin.h>
#include <sensrvaccelerometersensor.h>
#include <sensrvmagnetometersensor.h>
#include "trace.h"
#include "sensordatacompensatoraxisdatahandler.h"
#include "sensordatacompensatormatrix.h"


// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CSensorDataCompensatorAxisDataHandler::NewL
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorAxisDataHandler* CSensorDataCompensatorAxisDataHandler::NewL( 
                                         CSensorDataCompensatorPlugin& aPlugin,
                                         TSensrvChannelDataTypeId aDataType )
    {
	FUNC_LOG;
    return new ( ELeave )CSensorDataCompensatorAxisDataHandler( aPlugin, aDataType );
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorAxisDataHandler::~CSensorDataCompensatorAxisDataHandler
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorAxisDataHandler::~CSensorDataCompensatorAxisDataHandler()
    {
	FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorAxisDataHandler::Compensate
// ---------------------------------------------------------------------------
//
TInt CSensorDataCompensatorAxisDataHandler::Compensate( TDes8& aData )
    {
	FUNC_LOG;
	TInt err( KErrNone );
    TInt x( 0 );
    TInt y( 0 );
    TInt z( 0 );

    TRACE_MATRIX( "Axis data compensation matrix", iMatrix );

    if ( KSensrvChannelTypeIdMagnetometerXYZAxisData == iDataType )
        {
        INFO( "Compensating magnetometer data" );
        TPckgBuf<TSensrvMagnetometerAxisData> pckg;
        if ( aData.MaxSize() == pckg.MaxSize() )
            {
            pckg.Copy( aData );
            iMatrix.Transform( x, y, z, pckg().iAxisXRaw, pckg().iAxisYRaw,  pckg().iAxisZRaw );
            INFO_3( "Compensated data Magnetometer raw [X: %d], [Y: %d], [Z: %d]", x, y, z );

            pckg().iAxisXRaw = x;
            pckg().iAxisYRaw = y;
            pckg().iAxisZRaw = z;                
            iMatrix.Transform( x, y, z, pckg().iAxisXCalibrated, pckg().iAxisYCalibrated, pckg().iAxisZCalibrated );
            INFO_3( "Compensated data Magnetometer calib [X: %d], [Y: %d], [Z: %d]", x, y, z );

            pckg().iAxisXCalibrated = x;
            pckg().iAxisYCalibrated = y;
            pckg().iAxisZCalibrated = z;
            aData.Copy( pckg );    
            }
        else
            {
            err = KErrArgument;
            }
        }
    else
        {        
        INFO( "Compensating accelerometer data" );
        TPckgBuf<TSensrvAccelerometerAxisData> pckg;
        if ( aData.MaxSize() == pckg.MaxSize() )
            {
            pckg.Copy( aData );
            iMatrix.Transform( x, y, z, pckg().iAxisX, pckg().iAxisY, pckg().iAxisZ );
            INFO_3( "Compensated data accelerometer[X: %d], [Y: %d], [Z: %d]", x, y, z  );
            pckg().iAxisX = x;
            pckg().iAxisY = y;
            pckg().iAxisZ = z;
            aData.Copy( pckg );
            }
        else
            {
            err = KErrArgument;
            }
        }

    ERROR( err, "Axis data compensation error" );
    return err;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorAxisDataHandler::Update
// ---------------------------------------------------------------------------
//
TInt CSensorDataCompensatorAxisDataHandler::Update()
    {
	
	FUNC_LOG;
    TInt err( KErrNone );
    TInt count = iPlugin.GetCompensationItemCount();
    INFO_1( "Axis compensation item count %d", count );
    iMatrix.SetIdentity(); // Reset compensation
    if ( count >= 0 )
        {
        // Calculate final compensation angles and create compensation matrix
        TPckgBuf<TCompensationTypeAxisData> pckg;
        TInt x( 0 );
        TInt y( 0 );
        TInt z( 0 );
        TSensorDataCompensatorMatrix m;
        TSensorDataCompensatorMatrix m2;
        for ( TInt i( 0 ) ; i < count; i++ )
            {
            TPtrC8 data( iPlugin.GetCompensationItem( i ) );
            if ( data.Size() == pckg.MaxSize() )
                {
                pckg.Copy( data );
                x = pckg().iAxisX;
                y = pckg().iAxisY;
                z = pckg().iAxisZ;
                }
            else
                {
                err = KErrCorrupt;
                break;
                }
            m.SetRotateX( x );
            m2.SetRotateY( y );
            m *= m2; // xy
            m2.SetRotateZ( z );
            m *= m2; // xy * z
            iMatrix *= m;
            }
        if ( err != KErrNone )
            {
            iMatrix.SetIdentity();
            }
        }
    else
        {
        // Error occured in plugin
        err = count;
        }
    iCompensate = ( count > 0 ) && ( err == KErrNone ); // Ready to compensate
    ERROR( err, "Axis update error" );
    return err;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorAxisDataHandler::CSensorDataCompensatorAxisDataHandler
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorAxisDataHandler::CSensorDataCompensatorAxisDataHandler( 
                                         CSensorDataCompensatorPlugin& aPlugin,
                                         TSensrvChannelDataTypeId aDataType ) 
    : CSensorDataCompensatorDataHandlerBase( aPlugin ), iDataType( aDataType )
    {
	FUNC_LOG;
    }
