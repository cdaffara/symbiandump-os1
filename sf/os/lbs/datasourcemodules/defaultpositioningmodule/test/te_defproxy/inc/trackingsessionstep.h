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
*
*/



/**
 @file TrackingSessionStep.h
 @internalTechnology
*/
#ifndef __TRACKINGSESSION_STEP_H__
#define __TRACKINGSESSION_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_defproxysuitestepbase.h"

class CTrackingSessionStep : public CTe_defproxySuiteStepBase
	{
public:
	CTrackingSessionStep();
	~CTrackingSessionStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KTrackingSessionStep,"TrackingSessionStep");

class TWatch 
{
    public:
        TWatch() { RestartWatch(); };
        void RestartWatch() { iTime.UniversalTime(); iLastTick = iTime; };
        void Tick() { iLastTick.UniversalTime(); };
        TTimeIntervalMicroSeconds ElapsedTotal()
        {
            TTime now;
            now.UniversalTime();
            return now.MicroSecondsFrom(iTime);
        };
        TTimeIntervalMicroSeconds ElapsedFromTick()
        {
            TTime now;
            now.UniversalTime();
            return now.MicroSecondsFrom(iLastTick);
        };
        TTime Time() { return iTime; };
        TTime LastTick() { return iLastTick; };
    private:
        TTime iTime;
        TTime iLastTick;
};

#endif
