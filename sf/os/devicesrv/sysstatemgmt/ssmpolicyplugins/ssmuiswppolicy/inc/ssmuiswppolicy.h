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
* Description: Declaration of CSsmUiSwpPolicy class.
*
*/

#ifndef C_SSMUISWPPOLICY_H
#define C_SSMUISWPPOLICY_H

#include "ssmswppolicybase.h"

/**
 *  Policy plugin for RF status SWP.
 */
NONSHARABLE_CLASS( CSsmUiSwpPolicy ) : public CSsmSwpPolicyBase
    {

public:

    /**
     * Constructs a CSsmUiSwpPolicy object.
     *
     * @return The new object
     */
    IMPORT_C static MSsmSwpPolicy* NewL();

    /**
     * Destructor.
     */
    virtual ~CSsmUiSwpPolicy();

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
    CSsmUiSwpPolicy();

    };

#endif // C_SSMUISWPPOLICY_H
