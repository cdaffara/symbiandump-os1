/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration of CSsmNonCriticalSwpPolicy class.
*
*/

#ifndef SSMNONCRITICALSWPPOLICY_H
#define SSMNONCRITICALSWPPOLICY_H

#include "ssmswppolicybase.h"

/**
 *  Policy plugin for noncritical phase SWP.
 */
NONSHARABLE_CLASS( CSsmNonCriticalSwpPolicy ) : public CSsmSwpPolicyBase
    {

public:

    /**
     * Constructs a CSsmNonCriticalSwpPolicy object.
     *
     * @return The new object
     */
    IMPORT_C static MSsmSwpPolicy* NewL();

    /**
     * Destructor.
     */
    virtual ~CSsmNonCriticalSwpPolicy();

private: // From CSsmPolicyBase

    void GetCommandListPathL( TDes& aCmdListPath );

private: // From MSsmSwpPolicy

	/**
	 * @see MSsmSwpPolicy.
     */
    TResponse TransitionAllowed(
	    const TSsmSwp& aSwp,
	    const RMessagePtr2& aMessage );

private:

    /**
    * First phase constructor.
    */
    CSsmNonCriticalSwpPolicy();

    };

#endif // SSMNONCRITICALSWPPOLICY_H
