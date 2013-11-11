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
* Declaration of CSsmInitCriticalLevels class.
*
*/

#ifndef SSMINITCRITICALLEVELS_H
#define SSMINITCRITICALLEVELS_H

#include "ssmsystemcommandsbase.h"
#include <e32base.h>
#include <ssm/ssmcustomcommand.h>


/**
 *  Defines critical level values for memory and disk space.
 */
NONSHARABLE_CLASS( CSsmInitCriticalLevels ) : public CSsmSystemCommandsBase,
                                              public MSsmCustomCommand
    {

public:

    /**
     * Two-phased constructor
     */
	static CSsmInitCriticalLevels* NewL();

    /**
     * Destructor
     */
    virtual ~CSsmInitCriticalLevels();

private: // From MSsmCustomCommand

    /**
     * @see MSsmCustomCommand
     */
    TInt Initialize( CSsmCustomCommandEnv* aCmdEnv );

    /**
     * @see MSsmCustomCommand
     */
	void Execute( const TDesC8& aParams, TRequestStatus& aRequest );

    /**
     * @see MSsmCustomCommand
     */
	void ExecuteCancel();

    /**
     * @see MSsmCustomCommand
     */
	void Close();

    /**
     * @see MSsmCustomCommand
     */
	void Release();
	
private:

    /**
     * Defines critical level values for memory and disk space.
     */
    void InitCriticalLevelKeysL();

private: // data
    };

#endif // SSMINITCRITICALLEVELS_H
