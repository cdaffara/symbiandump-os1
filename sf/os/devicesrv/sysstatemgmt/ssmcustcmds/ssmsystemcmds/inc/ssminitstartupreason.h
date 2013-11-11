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
* Declaration of CSsmInitStartupReason class.
*
*/

#ifndef SSMINITSTARTUPREASON_H
#define SSMINITSTARTUPREASON_H

#include "ssmsystemcommandsbase.h"
#include <e32base.h>
#include <ssm/ssmcustomcommand.h>

/**
 *  Reads startup reason from the Central Repository and sets it to Publish
 *  and Subscribe key.
 */
NONSHARABLE_CLASS( CSsmInitStartupReason ) : public CSsmSystemCommandsBase,
                                             public MSsmCustomCommand
    {

public:

    /**
     * Two-phased constructor
     */
	static CSsmInitStartupReason* NewL();

    /**
     * Destructor
     */
    virtual ~CSsmInitStartupReason();

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
     *  Initalizes the startup reason.
     */
    void InitStartupReasonL();
	
private: // data

    };

#endif // SSMINITSTARTUPREASON_H
