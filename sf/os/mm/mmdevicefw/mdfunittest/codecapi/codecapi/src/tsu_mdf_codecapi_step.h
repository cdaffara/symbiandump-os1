// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic tests for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_step.h
*/

#ifndef TSU_MDF_CODECAPI_STEP_H
#define TSU_MDF_CODECAPI_STEP_H

#include <testframework.h>
#include <mmf/server/mmfhwdevice.h>
#include "tsu_mdf_codecapi_constants.h"

/**
 *
 * Timeout timer, for those tests which need to timeout
 * and continue without receiving a callback
 *
 * @class CTimeOutTimer 
 *
 */
class CTimeOutTimer : public CTimer
    {
public:
    static CTimeOutTimer* NewL();
    ~CTimeOutTimer();
protected:
    virtual void RunL();
private:
    CTimeOutTimer();
    void ConstructL();
    };

/**
 * Abstract base class for all test steps.
 *
 * @class RTestStepCodecApi 
 * @brief Test class that enables tests.
 *
 */
class RTestStepCodecApi : public RTestStep
	{
public:
	RTestStepCodecApi();
	~RTestStepCodecApi();
	
	// will be used by the audio play/record test cases
	virtual void SetVerdict(TPtrC16 aText, TVerdict aVerdict = EPass){	
		INFO_PRINTF3(_L("Text: %s, Verdict:%d"), aText.Ptr(), aVerdict);
	}
protected:
	virtual TVerdict OpenL();
	virtual void CleanupAfterOpenFail();
	virtual void Close();
	
	void StartActive(TInt aTimeout);
	void StartActive();
	
	virtual TVerdict DoTestStepL() = 0;
private:
	CActiveScheduler* iActiveScheduler;
	};

#endif // TSU_MDF_CODECAPI_STEP_H
