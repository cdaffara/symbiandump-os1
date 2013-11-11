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
* Description:  Base class for all compensators
*
*/


#ifndef C_SENSORDATACOMPENSATORDATAHANDLERBASE_H
#define C_SENSORDATACOMPENSATORDATAHANDLERBASE_H

#include <e32base.h>

class CSensorDataCompensatorPlugin;

/**
 * Class implements base class for all compensators
 * 
 * @lib sensordatacompensator.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CSensorDataCompensatorDataHandlerBase ) : public CBase
    {  
public:
    /**
    * Compensates given data.
    *
    * @since S60 5.0
    * @param aData Data to compensate
    * @return KErrNone if compensation was succesful or system wide error code on fail.
    */
    virtual TInt Compensate( TDes8& aData ) = 0;

    /**
    * Updates the values used by compensation.
    *
    * @since S60 5.0
    * @return KErrNone if update was succesful or system wide error code on fail.
    */
    virtual TInt Update() = 0;

    /**
    * Indicates if handler is ready to compensate data.
    *
    * @since S60 5.0
    * @return ETrue if ready. Otherwise EFalse.
    */
    inline TBool ReadyToCompensate() const { return iCompensate; }

protected:
    /**
    * Constructor for derived classes.
    *
    * @since S60 5.0
    * @param aPlugin Plugin instance to be used.
    */
    CSensorDataCompensatorDataHandlerBase( CSensorDataCompensatorPlugin& aPlugin );

protected: // Data

    // Plugin instance for derived classes
    CSensorDataCompensatorPlugin& iPlugin;  

    /**
    * Indicates whether compensation values have been setup by Update() and handler is
    * ready to compensate. If compensation is not needed, value can be set to EFalse.
    * Derived classes must set this value to ETrue in successful Update().
    */
    TBool iCompensate;

    };


#endif // C_SENSORDATACOMPENSATORDATAHANDLERBASE_H
