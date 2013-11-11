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
* Description:  Haptic server shutdown timer.
*
*/


#ifndef C__HWRMHAPTICSSHUTDOWN_H
#define C__HWRMHAPTICSSHUTDOWN_H

#include <e32base.h>

/**
 *  Haptics server shutdown timer.
 */
class CHWRMHapticsShutdown : public CTimer
    {
public:
    
    /**
     * Two phased constructor.
     *
     * @return Created instance of the shutdown timer.
     */ 
    static CHWRMHapticsShutdown* NewL();

    /**
     * Destructor.
     */ 
    virtual ~CHWRMHapticsShutdown();
    
    /**
     * Starts the timer.
     */ 
    void Start();

private:

    /**
     * C++ constructor.
     */ 
    CHWRMHapticsShutdown();
    
    /**
     * Second phase construction.
     */ 
    void ConstructL();
    
    /**
     * From base class CActive.
     * @see CActive.
     * 
     * Executed, when the timer expires; Shuts down
     * the server by closing the active scheduler.
     */ 
    void RunL();
    };

#endif // C_HWRMHAPTICSSHUTDOWN_H
            
// End of File
