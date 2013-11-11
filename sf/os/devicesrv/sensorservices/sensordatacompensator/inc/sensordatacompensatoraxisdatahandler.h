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


#ifndef C_SENSORDATACOMPENSATORAXISDATAHANDLER_H_
#define C_SENSORDATACOMPENSATORAXISDATAHANDLER_H_

#include "sensordatacompensatordatahandlerbase.h"
#include "sensordatacompensatormatrix.h"

/**
 * Class implements axis data compensator
 *
 * @lib sensordatacompensator.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CSensorDataCompensatorAxisDataHandler ) : 
        public CSensorDataCompensatorDataHandlerBase
    {
public:
    /**
    * Two-phase constructor
    *
    * @since S60 5.0
    * @param aPlugin Plugin instance to be used.
    * @param aDataType Specifies the data type to be compensated.
    * @return Pointer to created object
    */
    static CSensorDataCompensatorAxisDataHandler* NewL( CSensorDataCompensatorPlugin& aPlugin,
                                                        TSensrvChannelDataTypeId aDataType );

    /**
    * Destructor. 
    */
    virtual ~CSensorDataCompensatorAxisDataHandler();

private:
// From CSensorDataCompensatorDataHandlerBase

    /**
     * @see CSensorDataCompensatorDataHandlerBase
     */
    TInt Compensate( TDes8& aData );

    /**
     * @see CSensorDataCompensatorDataHandlerBase
     */
    TInt Update();

private:
    /**
    * Constructor
    */
    CSensorDataCompensatorAxisDataHandler( CSensorDataCompensatorPlugin& aPlugin,
                                           TSensrvChannelDataTypeId aDataType );

private: // Data
    // Stores the data type to be compensated
    TSensrvChannelDataTypeId iDataType;

    // Stores the matrix used in compensation
    TSensorDataCompensatorMatrix  iMatrix;

    };

#endif // C_SENSORDATACOMPENSATORAXISDATAHANDLER_H
