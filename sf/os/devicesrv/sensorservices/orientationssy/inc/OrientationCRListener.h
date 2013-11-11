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
* Description:  CTCChannel class declaration.
*
*/



#ifndef ORIENTATIONCRLISTENER_H
#define ORIENTATIONCRLISTENER_H

// INCLUDES
#include <e32base.h>
#include <centralrepository.h>
#include "internalorientationcrkeys.h"

// FORWARD DECLARATIONS
class MCenRepCallBack;
// CLASS DECLARATION

/**
* This class implements a central repository listener and is used for testing
* purposes only.
*/
NONSHARABLE_CLASS( CSsyOrientationCRListener ): public CActive
    {
    public:
    
        // Default C++ destructor
        ~CSsyOrientationCRListener();
    
    public: // New methods
    
        // Default C++ constructor
        CSsyOrientationCRListener( MCenRepCallBack& aPrtToCallback );
        
        /**
        * Creates a pointer to the timer
        *
        * @param aSetTimerToHzLevel the herz level the timer is set to
        * @param aPrtToCallback callback function of the caller
        * @return CTCSendEventTimer
        */
        static CSsyOrientationCRListener* NewL( 
            MCenRepCallBack& aPrtToCallback,
            CRepository* aRepository );
        
    protected: // Methods from base classes
        
        void ConstructL( CRepository* aRepository );
        
        // @see CActive
    	void RunL();
    	
    	// @see CActive
        void DoCancel();
        
        // @see CActive
        TInt RunError( TInt aError );
        
        void SetCenRepListener();
        
    private: // Private data for base channel
    
    // Pointer to 
    CRepository* iRepository;
    
    MCenRepCallBack& iCallBack;
    
    };

/**
* A callback for creating the configurations each time the value has changed.
* This is used for testing purposes only so no need to optimize.
*/
class MCenRepCallBack
    {
public:
    virtual TInt CreateConfigurations() = 0;
    };

#endif // ORIENTATIONCRLISTENER_H

// End of File
