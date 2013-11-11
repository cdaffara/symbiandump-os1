/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CTCChannel class declaration.
*
*/



#ifndef TCSENDEVENTTIMER_H
#define TCSENDEVENTTIMER_H

// INCLUDES
#include <e32base.h>
#include "tcsendeventtimer.h"

// FORWARD DECLARATIONS
class MSendDataCallBack;

// CLASS DECLARATION

/**
*  This class implements a timer object that is used to create a sequence for
*  the tilt channel data send.
*
*  @lib None.
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCSendEventTimer ): public CActive
    {
    public:
    
        // Default C++ destructor
        ~CTCSendEventTimer();
    
    public: // New methods
    
        // Default C++ constructor
        CTCSendEventTimer( MSendDataCallBack& aPrtToCallback );
        
        /**
        * Creates a pointer to the timer
        *
        * @param aSetTimerToHzLevel the herz level the timer is set to
        * @param aPrtToCallback callback function of the caller
        * @return CTCSendEventTimer
        */
        static CTCSendEventTimer* NewL( TInt aSetTimerToHzLevel,
                                        MSendDataCallBack& aPrtToCallback );
        
    protected: // Methods from base classes
        
        // Constructor
        void ConstructL( TInt aSetTimerToHzLevel );
        
        // @see CActive
    	void RunL();
    	
    	// @see CActive
        void DoCancel();
        
        // @see CActive
        TInt RunError( TInt aError );
	public:
		void Start(TInt aHrz );
    private: // Private data for base channel
    
    // The timer object
    RTimer iTimer;
    
    // The timing sequence that is used in herz
    TInt iTimerToHzLevel;
    
    // The call back function that is called when the timer expires
    MSendDataCallBack& iCallBack;
    
    TBool iIsCanceled;
    
    };

class MSendDataCallBack
    {
public:
    
    /**
    * A callback method for the timer to call. Is implemented by the client.
    */
    virtual void SendDataAfterTimer() = 0;
    
    };

#endif // TCSENDEVENTTIMER_H

// End of File
