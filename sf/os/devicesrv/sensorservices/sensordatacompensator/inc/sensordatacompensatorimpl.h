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
* Description:  Sensor data compensation implementation class.
*
*/


#ifndef C_SENSORDATACOMPENSATORIMPL_H
#define C_SENSORDATACOMPENSATORIMPL_H

#include <sensordatacompensator.h>
#include <sensordatacompensatorplugin.h>


class CSensorDataCompensatorDataHandlerBase;

/**
 * Class implements sensor data compensation.
 *  
 * @lib sensordatacompensator.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CSensorDataCompensatorImpl ): 
    public CSensorDataCompensator, 
    public MSensorDataCompensatorObserver
    {
    public:
    
    /**
    * Two-phase constructor
    *
    * @since S60 5.0
    * @param aDataType Specifies the data type to be compensated.
    * @param aType Specifies the compensation type.
    * @return Pointer to created object
    */  
    static CSensorDataCompensatorImpl* NewL( 
        TSensrvChannelDataTypeId aDataType,
        TSensorCompensationType aType );

    /**
    * Destructor. 
    */
    virtual ~CSensorDataCompensatorImpl();

    private:
    
    // From CSensorDataCompensator

    /**
     * @see CSensorDataCompensator
     */
    TInt Compensate( TDes8& aData );

    /**
     * @see CSensorDataCompensator
     */
    TSensrvChannelDataTypeId GetChannelDataType() const;

    /**
     * @see CSensorDataCompensator
     */
    TSensorCompensationType GetCompensationType() const;

    private:        
    // From MSensorDataCompensatorObserver

    /**
     * @see MSensorDataCompensatorObserver
     */
    void CompensationValuesUpdated();
    
    private:
    
    /**
    * Constructor
    */
    CSensorDataCompensatorImpl( 
        TSensrvChannelDataTypeId aDataType,
        TSensorCompensationType aType );

    /**
    * 2nd phase of construction
    */
    void ConstructL();
    
    private: // Data

    // Requested data type
    TSensrvChannelDataTypeId iDataType;

    // Requested compensation type
    TSensorCompensationType iType;

    /**
    * Compensation handler instance. Can be NULL when compensation of requested
    * data type is not supported.
    * Own.
    */
    CSensorDataCompensatorDataHandlerBase* iCompHandler;

    // Indication that matrices must be recalculated
    TBool iValuesUpdated;

    /**
    * Plugin instance. Can be NULL when plugin is missing.
    * Own.
    */
    CSensorDataCompensatorPlugin *iPlugin;

    // Plugin library
    RLibrary iLibrary;

    /**
    * Stores compensation error occured on compensation value update.
    * This error code is returned to the client until the update is successful and
    * compensation can be performed.
    */
    TInt iCompError;

    };


#endif // C_SENSORDATACOMPENSATORIMPL_H

// End of File
