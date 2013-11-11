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
*/

#include "Te_SmsTestSteps.h"

CForwardSystemTime::CForwardSystemTime()
    {
    iPartOfMultiStepTestCase = ETrue;
    }

/**
  Forwards the system time in minutes.
*/
TVerdict CForwardSystemTime::doTestStepL()
	{	
	//Get the current system time
	TTime utcTime;
	utcTime.UniversalTime();
	
	//Get value from Ini file
    TInt  forwardTimeInMinutes;
	GetIntFromConfig(ConfigSection(), _L("forwardTime"), forwardTimeInMinutes);

	//Forward the system time 
	TTimeIntervalMinutes forwardMinutes(forwardTimeInMinutes);
	User::SetUTCTime(utcTime+forwardMinutes);

	//Allow sometime for the SMS Stack to spot the time change
	User::After(5*1000000);
	
	return TestStepResult();
	}
//-----------------------------------------------------------------------------







