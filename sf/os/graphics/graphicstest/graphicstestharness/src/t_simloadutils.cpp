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

#include <e32math.h>
#include <e32debug.h>
#include <iniparser.h>
#include "t_simload.h"
#include "t_simloadutils.h"

EXPORT_C TBool CTSimLoadAppUtils::IsStaticLoadL()
	{
	CIniData * iniData = CIniData::NewL(KSimLoadConfigFile);
	CleanupStack::PushL(iniData);

	TPtrC value;
	User::LeaveIfError(iniData->FindVar(KDefaultSectionName, KConfigSimLoadType(), value));
	HBufC* simLoadType = value.Alloc();

	CleanupStack::PopAndDestroy(iniData);

	TBool ret = EFalse;

	if (simLoadType!=NULL && simLoadType->Des().Compare(_L("KSimLoadStatic")) == KErrNone)
		{
		ret = ETrue;
		}
	else
		{
		if (simLoadType!=NULL && simLoadType->Des().Compare(_L("KSimLoadSpiked")) != KErrNone)
			{
			User::Leave(KErrAbort);
			}
		}

	return ret;
	}

EXPORT_C TInt CTSimLoadAppUtils::SimLoadL()
	{
	CIniData * iniData = CIniData::NewL(KSimLoadConfigFile);
	CleanupStack::PushL(iniData);

	TInt ret;
	User::LeaveIfError(iniData->FindVar(KDefaultSectionName, KConfigSimLoadValue(), ret));
	CleanupStack::PopAndDestroy(iniData);
	return ret;
	}

EXPORT_C void CTSimLoadAppUtils::SimulateStaticLoadL(TInt aLoadPercentage, TInt aDelay)
	{
	if (aLoadPercentage>=0)
		{
		// assuming the figure read in was '%' calculate the loop count
		// required to achieve the simulated load required
		const TInt mySimLoad = CalculateSimLoadL(aLoadPercentage);

		TInt internalDelay = aDelay;

		if (aLoadPercentage==0)
			{
			// set delay to large number, so that simload process is still
			// running at end of test, and therefore not cause issue with
			// TEF execution determining the test has failed because
			// the sim load app is not running at end of test.
			internalDelay = 30000000; // 30s
			}

		SimulateTestLoad(mySimLoad, internalDelay);
		}
	}

EXPORT_C void CTSimLoadAppUtils::SimulateSpikedLoad()
	{
	SimulateTestLoad(1500000, 100000);
	}

void CTSimLoadAppUtils::SimulateTestLoad(TInt aSimLoad, TInt aDelay)
	{
	FOREVER
		{
		for (TInt i = 0; i <= aSimLoad; i++)
			{
			//Load the CPU by forcing it to execute the NOP The load value
			//is dependant upon the value of aSimLoad and the load type (either
			//static or spiked) by the ratio of aSimload to aDelay
			//Tuned to work with the H4 only
			#if !defined(__WINS__) && !defined(__X86GCC__)
			asm{nop;};
			#endif
			}

		User::After(aDelay);
		}
	}

TInt CTSimLoadAppUtils::CalculateSimLoadL(TInt aPercentage)
	{
	TInt32 calculatedLoad = 0;

	if (aPercentage>0)
		{
		// based on measurements performed using Symbian Sample Profiler
		// .../base/e32utils/analyse/profiler.rtf
		// and then utilising curve fit (http://www.ebicom.net/~dhyams/cftp.htm)
		// the following formula was determined to best calculate the load
		// based on a required percentage
		// simload = a + b*aPercentage + c*aPercentage^2 + d*aPercentage^3 + e*aPercentage^4
		const TReal a = 92600;
		const TReal b = -32000;
		const TReal c = 4700;
		const TReal d = -114;
		const TReal e = 0.91;
		TReal tempValue = a + (b*aPercentage) + (c*aPercentage*aPercentage)
		                    + (d*aPercentage*aPercentage*aPercentage)
		                    + (e*aPercentage*aPercentage*aPercentage*aPercentage);
		User::LeaveIfError(Math::Int(calculatedLoad, tempValue));
		}

	return calculatedLoad;
	}
