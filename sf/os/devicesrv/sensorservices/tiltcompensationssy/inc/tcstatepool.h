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
* Description:  MTCStatePool interface declaration.
*
*/



#ifndef TCSTATEPOOL_H
#define TCSTATEPOOL_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CTCState;

// CLASS DECLARATION

/**
*  State pool interface.
*  State classes can use this interface to manipulate
*  current state.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( MTCStatePool )
    {
    public:
    
        /**
        * Returns current state. NULL if state not found.
        *
        * @return TTCState
        */
        virtual CTCState* CurrentState() const = 0;

        /**
        * Returns prvious state. NULL if state not found.
        * 
        * @return TTCState
        */
        //virtual CTCState* PreviousState() const = 0;

        /**
        * Changes the current state.
        * New state will be notified about state entry.
        * Current state will be notified about state exit.
        * 
        * @param aStateId The ID of the new state.
        */
        virtual void ChangeStateL( TInt aStateId ) = 0;
};

#endif // TCSTATEPOOL_H

// End of File
