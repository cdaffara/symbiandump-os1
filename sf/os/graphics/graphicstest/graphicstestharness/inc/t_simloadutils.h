// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @test
*/

#ifndef __T_SIM_LOAD_UTILS_H__
#define __T_SIM_LOAD_UTILS_H__

#include <e32base.h>

 _LIT(KDefaultSectionName, "default");

class CTSimLoadAppUtils
	{
public:
	/**
	* Read in sim load type parameter from configuration file
	* created by test step, and indicate whether the type is
	* static.
	*
	* @return   ETrue if static load specified.
				EFalse if spiked load specified
	* @leave	KErrAbort if the load type cannot be determined,
				otherwise a system error code.
	*/
	IMPORT_C static TBool IsStaticLoadL();

	/**
	* Read in the simload (as a percentage) config parameter from
	* configuration file created by test step.
	*
	* @return	The simulated load that the app (in
	*			combination with 2 other apps) is to achieve.
	* @leave	System error code.
	*/
	IMPORT_C static TInt SimLoadL();

	/**
	* Given a load percentage, and seed value for delay, create a load that when
	* combined with 2 other sim load apps produce the load percentage passed in.
	*
	* @param	aLoadPercentage The simulated load that the app (in
	*			combination with 2 other apps) is to achieve.
	* @param    aDelay Seed value which harmonises the load pattern.
	* @leave	System wide error code
	*/
	IMPORT_C static void SimulateStaticLoadL(TInt aLoadPercentage, TInt aDelay);

	/**
	* Simulates a spiked load, e.g. 100% load for short period of time, at regular
	* spaced intervals, to simulate the load produced when for example a phone
	* call is received.
	*/
	IMPORT_C static void SimulateSpikedLoad();

private:
	CTSimLoadAppUtils() {}
	static void SimulateTestLoad(TInt, TInt);
	static TInt CalculateSimLoadL(TInt);
	};

#endif // __T_SIM_LOAD_UTILS_H__

