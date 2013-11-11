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
* Description: Declaration of CSsmPolicyBase class.
*
*/

#ifndef C_SSMPOLICYBASE_H
#define C_SSMPOLICYBASE_H

#include <e32base.h>
#include <f32file.h>
#include <ssm/ssmconditionalcallback.h>

class CConditionEvaluate;
class CSsmCommandListResourceReader;
class CSsmMapperUtility;
class TSsmState;

/**
*  Base class for SSM policy plugins
*
*/
class CSsmPolicyBase : public CBase, public MSsmConditionalCallback
    {

public:

    /**
    * First phase constructor.
    */
    IMPORT_C CSsmPolicyBase();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CSsmPolicyBase();

protected: // from MSsmConditionalCallback

    /**
    * @see MSsmConditionalCallback
    */
	IMPORT_C TBool ConditionalCommandAllowedL(
	    CResourceFile& aResourceFile,
	    TInt aResourceId );

protected:

    /**
    * Getter for policy-specific command list path.
    *
    * @param aCmdListPath On return contains the command list path.
    */
    virtual void GetCommandListPathL( TDes& aCmdListPath ) = 0;

protected:

    /**
    * Getter for current system state.
    *
    * @param aState On return contains current system state.
    */
    IMPORT_C TInt GetCurrentState( TSsmState& aState ) const;

protected:

    /**
    * Second phase constructor.
    */
    IMPORT_C void BaseConstructL();

    /**
    * Read, increment and update reset counter in Cenrep.
    *
    * @return ETrue if the reset counter in Cenrep is greater than or equal to
    * the reset limit.
    */
    IMPORT_C TBool ResetLimitReached();

private:

    TBool ResetLimitReachedL();

protected: // data

    /** File server session for the use of derived classes. */
	RFs iFs;

    /** 
    * Mapper utility instance for the use of base and all derived classes.
    * Own. Not NULL. 
    */
	CSsmMapperUtility* iUtil;

	/** Resource reader. Own. Not NULL. */
	CSsmCommandListResourceReader* iCommandListResourceReader;

private: // data

    /** Used to evaluate conditions in command lists. Not NULL. Own. */
    CConditionEvaluate* iEvaluator;

    };

#endif // C_SSMPOLICYBASE_H
