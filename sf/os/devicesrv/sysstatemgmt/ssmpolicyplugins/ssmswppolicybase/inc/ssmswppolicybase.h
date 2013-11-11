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
* Description: Declaration of CSsmSwpPolicyBase class.
*
*/

#ifndef C_SSMSWPPOLICYBASE_H
#define C_SSMSWPPOLICYBASE_H

#include <ssm/ssmswppolicy.h>

#include "ssmpolicybase.h"

/**
*  Base class for SSM SWP policy plugins
*
*/
class CSsmSwpPolicyBase : public CSsmPolicyBase, public MSsmSwpPolicy
    {

public:

    /**
    * First phase constructor.
    */
    IMPORT_C CSsmSwpPolicyBase();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CSsmSwpPolicyBase();

protected: // From MSsmSwpPolicy

	/**
	 * @see MSsmSwpPolicy.
     */
    IMPORT_C virtual void Initialize( TRequestStatus& aStatus );

	/**
	 * @see MSsmSwpPolicy.
     */
    IMPORT_C virtual void InitializeCancel();

	/**
	 * @see MSsmSwpPolicy.
     */
    IMPORT_C virtual void PrepareCommandList(
        const TSsmSwp& aSwp,
        TRequestStatus& aStatus );

	/**
	 * @see MSsmSwpPolicy.
     */
    IMPORT_C virtual void PrepareCommandListCancel();

	/**
	 * @see MSsmSwpPolicy.
     */
    IMPORT_C virtual CSsmCommandList* CommandList();

	/**
	 * @see MSsmSwpPolicy.
     */
	IMPORT_C virtual void HandleCleReturnValue(
	    const TSsmSwp& aSwp,
	    TInt aError,
	    TInt aSeverity,
	    TRequestStatus& aStatus );

	/**
	 * @see MSsmSwpPolicy.
     */
	IMPORT_C virtual void HandleCleReturnValueCancel();

	/**
	 * @see MSsmSwpPolicy.
     */
    IMPORT_C virtual void Release();

    };

#endif // C_SSMSWPPOLICYBASE_H
