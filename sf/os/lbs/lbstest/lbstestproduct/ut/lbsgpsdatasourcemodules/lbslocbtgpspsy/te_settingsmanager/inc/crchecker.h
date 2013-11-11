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
* CCRChecker declaration
*
*/



/**
 @file crchecker.h
 @internalTechnology
*/

#ifndef CCRCHECKER_H
#define CCRCHECKER_H

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>
#include <LbsCommon.h>
#include <LbsClassTypes.h>
#include <centralrepository.h>
#include <PositioningPluginFWInternalCRKeys.h>
#include <BtGpsPsyPrivateCRKeys.h>

/**
 *  CCRChecker
 * 
 * This utility class allow to delete and check the value of the publish and subscribe keys used by the CBTGPSSettingManager class.
 * 
 */
class CCRChecker : public CBase
	{
public: // Constructors and destructor

	/**
	 * Destructor.
	 */
	~CCRChecker();

	/**
	 * Two-phased constructor.
	 */
	static CCRChecker* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CCRChecker* NewLC();
	
	/**
	 * This method check the central repository keys for basic setup information
	 * related to the BTPSY. The keys are common to all PSY.
	 * see PositioningPluginFWInternalCRKeys.h
	 */
	TInt CheckCommonRepositorykeys();
	
	/**
	 * This method check the central repository keys specific to the BTPSY.
	 * BtGpsPsyPrivateCRKeys.h
	 */
	TInt CheckSpecificRepositorykeys();
	
	/**
	 * Check the value stored for this repository key (the value is a TInt).
	 */
	TInt CheckRepository(TUint32 aKey);
	
	/**
	 * Check the value stored for this repository key (the value is a TReal).
	 */
	TInt CheckRepositoryRealKey(TUint32 aKey);
	
	/**
	 * Check the value of repository entry with an expected value.
	 */
	TInt CheckRepository(TUint32 aKey, const TDesC& aValue);
	
private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CCRChecker();

	/**
	 * default constructor for performing second stage construction
	 */
	void ConstructL();
	
	
private:
	//CenRep Settings   
	CRepository* iSettings;
	};

#endif // CCRCHECKER_H
