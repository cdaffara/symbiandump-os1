/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class implements the shutdown timer that is used by the
*                SensorServer to shutdown the server after the last client has
*                disconnected.
*
*/



#ifndef SENSRVSHUTDOWN_H
#define SENSRVSHUTDOWN_H

//  INCLUDES
#include <e32base.h>

//  CONSTANTS
const TInt KSensrvDefaultTerminationPeriod = 30000000;


// Forward declarations
class CSensrvProxyManager;

// CLASS DECLARATION

/**
*  Shutdown timer after the last client has disconnected.
*/
class CSensrvShutdown : public CTimer
    {
public:  // Constructors and destructor

	/**
	 * Two-phased constructor.
	 */
	static CSensrvShutdown* NewL( CSensrvProxyManager& aProxyManager );

	/**
	 * Destructor.
	 */
	virtual ~CSensrvShutdown();

public: // New functions

	/**
	 * Starts the shutdown count down.
	 */
	void Start();

private:  // Functions from base classes

	/**
	 * From CTimer
	 * Cancel the shutdown timer.
	 */
	void DoCancel();

	/**
	 * From CTimer
	 * Timer has expired, stop the RadioServer.
	 **/
	void RunL();

private:

	/**
	 * C++ default constructor.
	 */
	CSensrvShutdown( CSensrvProxyManager& aProxyManager );

	/**
	 * By default Symbian 2nd phase constructor is private.
	 */
	void ConstructL();
	
	CSensrvProxyManager& iProxyManager;
	
    };

#endif // SENSRVSHUTDOWN_H

// End of File
