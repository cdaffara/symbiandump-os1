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
* Declaration of CSsmSaShutdown class.
*
*/

#ifndef SSMSASHUTDOWN_H
#define SSMSASHUTDOWN_H

#include <e32base.h>
#include <ssm/ssmcustomcommand.h>
#include <ssm/ssmadaptationcli.h>

/**
 *  Each command is implemented in a class derived from MSsmCustomCommand.
 *  It is also possible to implement multiple similar command in the same
 *  class and differentiate between them based on constructor parameters
 *  from the factory method.
 */
NONSHARABLE_CLASS( CSsmSaShutdown ) : public CActive, public MSsmCustomCommand
    {

public:

    /**
     * Two-phased constructor
     */
	static CSsmSaShutdown* NewL();

    /**
     * Destructor
     */
    virtual ~CSsmSaShutdown();

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
	
private: // From CActive
    
    /**
     * @see CActive
     */
    void RunL();
    /**
     * @see CActive
     */
    void DoCancel();
	
private:
    CSsmSaShutdown();

private: // data

    // Handle to adaptation.
    RSsmStateAdaptation iAdaptation;
    // Client request status.
    TRequestStatus* iClientStatus;
    };

#endif // SSMSASHUTDOWN_H
