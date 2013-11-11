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
// This program is designed to test the PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_step.cpp
*/

#include <e32base.h>
#include "tsu_mdf_codecapi_step.h"

/**
 * Timeout timer
 */
CTimeOutTimer* CTimeOutTimer::NewL()
    {
    CTimeOutTimer* self = new (ELeave) CTimeOutTimer();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CTimeOutTimer::CTimeOutTimer()
  : CTimer(0)
    {
    }
    
CTimeOutTimer::~CTimeOutTimer()
    {
    }

void CTimeOutTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }

void CTimeOutTimer::RunL()
    {
    CActiveScheduler::Stop();
    }
    
//

/**
 * RTestStepCodecApi Implementation
 */
RTestStepCodecApi::RTestStepCodecApi()
	{
	}
	
RTestStepCodecApi::~RTestStepCodecApi()
	{
	}
	
TVerdict RTestStepCodecApi::OpenL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( iActiveScheduler );
	CActiveScheduler::Install(iActiveScheduler);
	CleanupStack::Pop();
	return EPass;
	}
	
void RTestStepCodecApi::CleanupAfterOpenFail()
	{
	}
	
void RTestStepCodecApi::Close()
	{
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	}

void RTestStepCodecApi::StartActive(TInt aTimeout)
	{
	CTimeOutTimer* timer = NULL;
	if(aTimeout > 0) 
		{
		TRAPD(err, timer = CTimeOutTimer::NewL());
		if(!err) 
			{
			timer->After(aTimeout * 1000000L);
			}
		}
	CActiveScheduler::Start();
	if(timer != NULL) 
		{
		timer->Cancel();
		delete timer;
		}
	}

void RTestStepCodecApi::StartActive()
	{
	CActiveScheduler::Start();
	}
							
