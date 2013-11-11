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
* Declaration of CSsmWaitPsKey class.
*
*/

#ifndef SSMWAITPSKEY_H
#define SSMWAITPSKEY_H

#include <e32base.h>
#include <ssm/ssmcustomcommand.h>


class CSsmPsKeyObserver;

/**
 *  Waits for a P&S key.
 */
NONSHARABLE_CLASS( CSsmWaitPsKey ) : public CBase,
                                     public MSsmCustomCommand
    {

public:
    
    /**
    * Wait PS key type. Defines whether to wait for a specific value
    * or a value in a range.
    */

    enum TSsmWaitPsKeyType
        {
        ESsmWaitPsExactValue = 1,
        ESsmWaitPsRange
        };

    /**
     * Two-phased constructor
     */
	static CSsmWaitPsKey* NewL( TSsmWaitPsKeyType aPsKeyWaitType );

    /**
     * Destructor
     */
    virtual ~CSsmWaitPsKey();

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
    
    CSsmWaitPsKey( TSsmWaitPsKeyType aPsKeyWaitType );
        
    CSsmPsKeyObserver* ExtractParamsL( const TDesC8& aParams );
    
	
private: // data
    
    // P&S key observer instance, owned.
    CSsmPsKeyObserver* iPsObserver;
    
    TSsmWaitPsKeyType iWaitType;

    };

#endif // SSMWAITPSKEY_H
