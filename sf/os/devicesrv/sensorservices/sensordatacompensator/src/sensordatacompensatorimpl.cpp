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
* Description:  Sensor data compensation implementation class
*
*/


#include <sensrvaccelerometersensor.h>
#include <sensrvmagnetometersensor.h>
#include <sensrvmagneticnorthsensor.h>
#include <sensrvtappingsensor.h>
#include "sensordatacompensatorimpl.h"
#include "sensordatacompensatoraxisdatahandler.h"
#include "sensordatacompensatormagneticnorthdatahandler.h"
#include "sensordatacompensatortappingdatahandler.h"
#include "trace.h"

const TInt KSensorDataCompensatorPluginFuncOrdinal( 1 );

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CSensorDataCompensatorImpl::NewL
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorImpl* CSensorDataCompensatorImpl::NewL(
    TSensrvChannelDataTypeId aDataType,
    TSensorCompensationType aType )
    {
    FUNC_LOG;
    CSensorDataCompensatorImpl* self = new ( ELeave ) CSensorDataCompensatorImpl( aDataType, aType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorImpl::~CSensorDataCompensatorImpl
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorImpl::~CSensorDataCompensatorImpl()
    {
    FUNC_LOG;
    delete iCompHandler;
    delete iPlugin;
    iLibrary.Close();
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorImpl::Compensate
// ---------------------------------------------------------------------------
//
TInt CSensorDataCompensatorImpl::Compensate( TDes8& aData )
    {
    FUNC_LOG;
    TInt err( KErrNone );

    if ( iPlugin && iCompHandler )
        {    
        // Update compensation values if update request has been notified by plugin
        if ( iValuesUpdated )
            {
            iValuesUpdated = EFalse; // Confirm the update to be started
            iCompError = iCompHandler->Update();
            }
        if ( iCompError == KErrNone ) // Try to compensate only if the update was ok
            {
            if ( iCompHandler->ReadyToCompensate() )
                {
                err = iCompHandler->Compensate( aData );
                }
            else
                {
                INFO_1( "Compensation not ready or needed (datatype 0x%x)", iDataType );
                }
            }
        else
            {
            err = iCompError; // Return compensation update error to the client
            }
        }
    else
        {
        INFO( "Compensator unavailable" );
        err = KErrNotSupported;
        }

    ERROR_1( err, "Compensation error (datatype 0x%x)", iDataType );

    return err; 
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorImpl::GetChannelDataType
// ---------------------------------------------------------------------------
//
TSensrvChannelDataTypeId CSensorDataCompensatorImpl::GetChannelDataType() const
    {
    FUNC_LOG;
    return iDataType;
    }


// ---------------------------------------------------------------------------
// CSensorDataCompensatorImpl::GetCompensationType
// ---------------------------------------------------------------------------
//
TSensorCompensationType CSensorDataCompensatorImpl::GetCompensationType() const
    {
    FUNC_LOG;
    return iType;
    }
    
// ---------------------------------------------------------------------------
// CSensorDataCompensatorImpl::CompensationValuesUpdated
// ---------------------------------------------------------------------------
//
void CSensorDataCompensatorImpl::CompensationValuesUpdated()
    {
    FUNC_LOG;
    iValuesUpdated = ETrue;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorImpl::CSensorDataCompensatorImpl
// ---------------------------------------------------------------------------
//
CSensorDataCompensatorImpl::CSensorDataCompensatorImpl( 
    TSensrvChannelDataTypeId aDataType, TSensorCompensationType aType ) :
    iDataType( aDataType ),
    iType( aType ),
    iValuesUpdated( ETrue )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CSensorDataCompensatorImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CSensorDataCompensatorImpl::ConstructL()
    {
    FUNC_LOG;
    // Load plug-in library
    _LIT( KCompensationPluginName, "sensordatacompensatorplugin.dll" );
    
    TInt error( iLibrary.Load( KCompensationPluginName  ) );
    ERROR( error, "Unable to load compensator plugin" );
    if ( KErrNone == error )
        {
        // Function at ordinal 1 creates CSensorDataCompensatorPlugin instance
        TLibraryFunction entry = iLibrary.Lookup( KSensorDataCompensatorPluginFuncOrdinal );
        
        if ( entry )
            {
            CreateSensorDataCompensatorFuncL createL = reinterpret_cast<CreateSensorDataCompensatorFuncL>( entry );
            
            if ( createL )
                {
                TRAP( error, iPlugin = ( *createL )( iDataType, iType, *this ) );
                ERROR( error, "Failed to create plugin" );
                }
            else
                {
                error = KErrCorrupt;
                ERROR( error, "Failed to cast plugin entry" );
                }
            }
        else
            {
            error = KErrCorrupt;
            ERROR( error, "Function lookup failed" );
            }
        }
   
    if ( error == KErrNone )
        {   
        switch ( iDataType )
            {
            case TSensrvAccelerometerAxisData::KDataTypeId:
            case TSensrvMagnetometerAxisData::KDataTypeId:
                {
                iCompHandler = CSensorDataCompensatorAxisDataHandler::NewL( *iPlugin,
                                                                            iDataType );
                break;
                }
            case TSensrvMagneticNorthData::KDataTypeId:
                {
                iCompHandler = CSensorDataCompensatorMagneticNorthDataHandler::NewL( *iPlugin );
                break;
                }
            case TSensrvTappingData::KDataTypeId:
                {
                iCompHandler = CSensorDataCompensatorTappingDataHandler::NewL( *iPlugin );
                break;
                }
            default:
                {
                INFO_1( "Compensator unavailable for datatype 0x%x", iDataType );
                break;
                }
            }
        }
    }
