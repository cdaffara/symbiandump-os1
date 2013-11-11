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
* Description:  Haptic server active scheduler definition.
*
*/


#ifndef CHWRMHAPTICSSERVERACTIVESCHEDULER_H
#define CHWRMHAPTICSSERVERACTIVESCHEDULER_H

#include <e32base.h>

/**
 * Overridden implementation for the active scheduler. The only 
 * difference in the implementation is the error handling in
 * Error(); Instead of the defaul implementation (panic), the 
 * method ignores the error, thus never panicing the server thread.
 */
class CHWRMHapticsServerActiveScheduler : public CActiveScheduler
    {
public:

    /**
     * Symbian two-phased constructor.
     */
    static CHWRMHapticsServerActiveScheduler* NewL();

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsServerActiveScheduler();

protected:  // New functions

    /**
     * Empty error handling implementation.
     * 
     * @param anError Error code
     */
    void Error( TInt anError ) const;

private:

    /**
     * C++ constructor.
     */
    CHWRMHapticsServerActiveScheduler();
    
    };

#endif // CHWRMHAPTICSSERVERACTIVESCHEDULER_H   
            
// End of File
