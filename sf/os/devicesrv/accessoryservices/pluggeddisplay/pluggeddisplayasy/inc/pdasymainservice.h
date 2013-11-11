/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:
 * CPDAsyMainService class declaration.
 *
 */

#ifndef PDASYMAINSERVICE_H
#define PDASYMAINSERVICE_H

// SYSTEM INCLUDE FILES
#include <AsyMainServiceBase.h>

// USER INCLUDE FILES

// FORWARD DECLARATIONS
class CPDEngine;

// DATA TYPES

// CLASS DECLARATION

/**
 * CPDAsyMainService implements the ASY Plug-in API main service.
 *
 * @lib None.
 * @since S60 TB9.2
 */
NONSHARABLE_CLASS( CPDAsyMainService ): public CASYMainServiceBase
    {
public:

    /**
     * Symbian two phased constructors.
     *
     * @since S60 TB9.2
     * @param None.
     * @return CPDAsyMainService
     */
    static CPDAsyMainService* NewL();
    static CPDAsyMainService* NewLC();

    /**
     * C++ destructor.
     */
    virtual ~CPDAsyMainService();
    
public: // New methods
    
    /**
     * Get access to the Plugged Display Engine.
     * 
     * @since TB9.2
     * @param None.
     * @return const CPDEngine*
     */
    CPDEngine& Engine() const;

protected: // From base classes

    // @see CASYMainServiceBase
    virtual TInt StartL();

private:

    CPDAsyMainService();
    void ConstructL();

private: // Data
    
    /** Instance to the Plugged Display Engine */
    CPDEngine* iEngine;
    };

#endif // PDASYMAINSERVICE_H
