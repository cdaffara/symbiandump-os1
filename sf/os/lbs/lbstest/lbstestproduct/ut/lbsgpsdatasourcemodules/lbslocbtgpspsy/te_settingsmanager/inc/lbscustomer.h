/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CLBSCustomer declaration
*
*/



/**
 @file lbscustomer.h
 @internalTechnology
*/


#ifndef LBSCUSTOMER_H
#define LBSCUSTOMER_H

#include <e32std.h>
#include <e32base.h>
#include <lbs.h>

/**
 *  CLBSCustomer
 * 
 * This utility class is a wrapper around the position server.
 * It's allows to start the location position server an to load the bluetooth psy.
 * 
 */
class CLBSCustomer : public CBase
	{
public: // Constructors and destructor

	/**
	 * Destructor.
	 */
	~CLBSCustomer();

	/**
	 * Two-phased constructor.
	 */
	static CLBSCustomer* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CLBSCustomer* NewLC();
	
	/**
	 * Start the location server.
	 */
	TInt ConnectToPositionServer();
	
	/**
	 * Load the Bluetooth PSY
	 */
	TInt OpenBTPSYPositioner();

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CLBSCustomer();
	
private:
	RPositionServer iPosServer;
	RPositioner iPositioner;
	TBool iBuildPosServer;
	TBool iBuildPositioner;
	};

#endif // LBSCUSTOMER_H
