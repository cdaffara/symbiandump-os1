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


#ifndef C_SENSORDATACOMPENSATORTAPPINGDATAHANDLER_H_
#define C_SENSORDATACOMPENSATORTAPPINGDATAHANDLER_H_

#include <sensordatacompensatorplugintypes.h>
#include "sensordatacompensatordatahandlerbase.h"

/**
 * Class implements tapping data compensator
 *  
 * @lib sensordatacompensator.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CSensorDataCompensatorTappingDataHandler ) :
        public CSensorDataCompensatorDataHandlerBase            
    {
public:
    /**
    * Two-phase constructor
    *
    * @since S60 5.0
    * @param aPlugin Plugin instance to be used.
    * @return Pointer to created object
    */
    static CSensorDataCompensatorTappingDataHandler* NewL(
        CSensorDataCompensatorPlugin& aPlugin );

    /**
    * Destructor.
    */
    virtual ~CSensorDataCompensatorTappingDataHandler();

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
    CSensorDataCompensatorTappingDataHandler( 
        CSensorDataCompensatorPlugin& aPlugin );

private: // Data

    // Stores compensation for x-axis
    TInt iXRotation;

    // Stores compensation for y-axis
    TInt iYRotation;

    // Stores compensation for z-axis
    TInt iZRotation;

    };

#endif // C_SENSORDATACOMPENSATORTAPPINGDATAHANDLER_H
