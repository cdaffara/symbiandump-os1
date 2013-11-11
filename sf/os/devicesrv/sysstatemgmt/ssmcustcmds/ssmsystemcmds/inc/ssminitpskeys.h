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
* Declaration of CSsmInitPsKeys class.
*
*/

#ifndef SSMINITPSKEYS_H
#define SSMINITPSKEYS_H

#include "ssmsystemcommandsbase.h"
#include <e32base.h>
#include <ssm/ssmcustomcommand.h>

/**
 * Initializes startup related Publish and Subscribe keys.
 */
NONSHARABLE_CLASS( CSsmInitPsKeys ) : public CSsmSystemCommandsBase, 
                                      public MSsmCustomCommand
    {

public:

    /**
     * Two-phased constructor
     */
	static CSsmInitPsKeys* NewL();

    /**
     * Destructor
     */
    virtual ~CSsmInitPsKeys();

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
     * Defines and sets values to the startup related Publish and Subscribe
     * keys.
     */
    void InitializeKeysL();
 
private: // data
    };

#endif // SSMINITPSKEYS_H
