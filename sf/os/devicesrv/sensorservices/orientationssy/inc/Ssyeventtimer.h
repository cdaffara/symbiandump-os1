/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSsyOrientationEventTimer class declaration.
*
*/



#ifndef SSYORIENTATIONEVENTTIMER_H
#define SSYORIENTATIONEVENTTIMER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MSendDataCallBack;

// CLASS DECLARATION

/**
* This class implements a timer object that is used to create a sequence for
* configurable orientation events.
*/
NONSHARABLE_CLASS( CSsyOrientationEventTimer ): public CActive
    {
public:

    // Default C++ destructor
    ~CSsyOrientationEventTimer();

    // Default C++ constructor
    CSsyOrientationEventTimer( MSendDataCallBack& aPrtToCallback );
    
    /**
    * Creates a pointer to the timer
    *
    * @param aSetTimerToMilSeconds timer in milliseconds.
    * @param aPrtToCallback callback function of the caller.
    * @return CSsyOrientationEventTimer.
    */
    static CSsyOrientationEventTimer* NewL( TInt aSetTimerToMilSeconds,
                                    MSendDataCallBack& aPrtToCallback );
    
protected: // Methods from base classes
    
    /**
    * Constructor
    *
    * @param aSetTimerToSeconds timer in milliseconds.
    */
    void ConstructL( TInt aSetTimerToSeconds );
    
    // @see CActive
	void RunL();
	
	// @see CActive
    void DoCancel();
    
    // @see CActive
    TInt RunError( TInt aError );
    
private: // Private data for base channel
    
    /**
    * The timer object
    */
    RTimer iTimer;
    
    /**
    * The call back function that is called when the timer expires.
    */
    MSendDataCallBack& iCallBack;
    
    };

/**
* An interface class the is implemented by CSsyOrientation class. This method
* called when the timer expires.
*/
class MSendDataCallBack
    {
public:
    
    /**
    * A callback method for the timer to call. Is implemented by the client.
    */
    virtual void SendDataAfterTimer() = 0;
    
    };

#endif // SSYORIENTATIONEVENTTIMER_H

// End of File
