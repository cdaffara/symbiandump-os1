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


#ifndef C_SENSORDATACOMPENSATORMAGNETICNORTHDATAHANDLER_H_
#define C_SENSORDATACOMPENSATORMAGNETICNORTHDATAHANDLER_H_

#include "sensordatacompensatordatahandlerbase.h"

/**
 * Class implements magnetic north compensator
 *
 * @lib sensordatacompensator.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CSensorDataCompensatorMagneticNorthDataHandler ) : 
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
    static CSensorDataCompensatorMagneticNorthDataHandler* NewL(
        CSensorDataCompensatorPlugin& aPlugin );

    /**
    * Destructor.
    */
    virtual ~CSensorDataCompensatorMagneticNorthDataHandler();

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
    CSensorDataCompensatorMagneticNorthDataHandler(
        CSensorDataCompensatorPlugin& aPlugin );
    
private: // Data
    // Stores the angle used in compensation
    TInt iAngle;

    };

#endif // C_SENSORDATACOMPENSATORMAGNETICNORTHDATAHANDLER_H_
