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
* CPNSChecker declaration
*
*/



/**
 @file pnschecker.h
 @internalTechnology
*/

#ifndef PNSCHECKER_H
#define PNSCHECKER_H

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>
#include <centralrepository.h>
#include <BtGpsPsyPrivatePSKeys.h>

/**
 *  CPNSChecker
 * 
 * This utility class allow to delete and check the value of the publish and subscribe keys used by the CBTGPSSettingManager class.
 * 
 */
class CPNSChecker : public CBase
	{
public: // Constructors and destructor

	/**
	 * Destructor.
	 */
	~CPNSChecker();

	/**
	 * Two-phased constructor.
	 */
	static CPNSChecker* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CPNSChecker* NewLC();
	
	/**
	 * Check the validity of ALL the P&S keys.
	 */
	void CheckPNSKeys(TBool &areAvailableKeys,TBool &isBTPSYLoaded, TBool &areValidKeys);
	
	/**
	 * Check the value of the P&S key with an expected value.
	 */
	TInt CheckKey(TUint aKey, TInt aValue);
	
	/**
	 * Check the value of the P&S key with an expected value.
	 */
	TInt CheckKey(TUint aKey, const TDesC8& aValue);
	
	/**
	 * Delete the specified P&S key.
	 */
	void DeleteKey(TUint aKey);
	
	/**
	 * Delete All P&S key.
	 */
	void DeleteAllKeys();

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CPNSChecker();

	/**
	 * default constructor for performing second stage construction
	 */
	void ConstructL();
	
	/**
	 * Check that the P&S keys value is inside it's expected range.
	 */
	void CheckResults(TBool &areAvailableKeys, TBool &isBTPSYLoaded, TBool &areValidKeys, TInt aErr, TInt aValue, TUint aKey);
	
	/**
	 * Check that the P&S keys value is inside it's expected range.
	 */
	void CheckResults(TBool &areAvailableKeys, TBool &areValidKeys, TInt aErr, TUint aKey);

private:

	//P&S Keys settings
	RProperty	iEvents;
	};

#endif // PNSCHECKER_H
