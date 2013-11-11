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
* Declaration of CSsmActivityCmd class.
*
*/

#ifndef SSMACTIVITYCMD_H
#define SSMACTIVITYCMD_H

#include <e32base.h>
#include <ssm/ssmcustomcommand.h>

class CUserActivityManager;

/**
 *  A custom command for monitoring user in/activity used in start-up list 
 *  execution phasing. 
 */
NONSHARABLE_CLASS( CSsmActivityCmd ) : public CBase, public MSsmCustomCommand
    {

public:

    /**
     * Two-phased constructor
     */
	static CSsmActivityCmd* NewL();

    /**
     * Destructor
     */
    virtual ~CSsmActivityCmd();

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
	
private: // New functions.	
	
    /**
    * Called when CUserActivityManager detects inactivity.
    */
    static TInt InactiveCallback( TAny* aActivityMonitor );

    /**
    * Called when CUserActivityManager detects inactivity.
    */
    static TInt ActiveCallback( TAny* aActivityMonitor );
    
    /**
    * Completes the client request with provided error code.
    */
    void Complete( TInt aErrorCode );
    
    /**
    * Extracts the parameters for the custom command.
    * @aParams The parameters to be parsed.
    * @return The timeout value parsed from the params.
    */
    TInt ExtractParamsL( const TDesC8& aParams );
    
private: // data

    /**
    * Client request status, may be NULL.
    * Not own.
    */
    TRequestStatus* iClientStatus;
    
    /**
    * User activity manager. Instantiated in Initialize and released in Close.
    * Own.
    */
    CUserActivityManager* iActivityMgr;	

    };

#endif // SSMACTIVITYCMD_H
