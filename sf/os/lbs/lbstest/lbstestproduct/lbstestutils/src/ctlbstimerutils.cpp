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
// @file ctlbstimerutils.cpp
// 
//


#include <lbs/test/ctlbstimerutils.h>


/**
  Function : NewL
  Description :  It performs the two construction and returns an object
  				 of type CT_MsgTimerUtils
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
EXPORT_C CT_LbsTimerUtils* CT_LbsTimerUtils::NewL(MT_LbsTimerUtilsObserver* aObserver, TInt aTimerId)
	{
	CT_LbsTimerUtils* self = new(ELeave) CT_LbsTimerUtils(aObserver, aTimerId);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}


/**
  Function : CT_MsgTimerUtils
  Description : Constructor
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsTimerUtils::CT_LbsTimerUtils(MT_LbsTimerUtilsObserver* aObserver, TInt aTimerId)
	:
	CTimer(CTimer::EPriorityHigh),
	iObserver(aObserver),
	iTimerId(aTimerId)
	{
	CActiveScheduler::Add(this);
	}


/**
  Function : ~CT_MsgTimerUtils
  Description : Destructor
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
EXPORT_C CT_LbsTimerUtils::~CT_LbsTimerUtils()
	{
	Cancel();
	}

void CT_LbsTimerUtils::ConstructL()
	{
	CTimer::ConstructL();
	}


/**
  Function : After
  Description : Calls the After function CTimer for the given number of secs
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
EXPORT_C void CT_LbsTimerUtils::SetTimer(const TTimeIntervalMicroSeconds32 aPeriod)
{
	iTargetTime = 0;
	HighRes(aPeriod);
}
	
	
EXPORT_C void CT_LbsTimerUtils::SetTimer(const TTime& aUtcTargetTime)
	{
	Cancel();
	
	iTargetTime = aUtcTargetTime;

	TTime zeroTime(0);

	if (aUtcTargetTime == zeroTime)
		{
		return;
		}
	
	TTime timeNow;
	timeNow.UniversalTime();
	
	TTimeIntervalMicroSeconds delay(0);
	
	if (timeNow < aUtcTargetTime)
		{
		delay = aUtcTargetTime.MicroSecondsFrom(timeNow);
		}

	TInt delay32 = static_cast<TInt>(I64LOW(delay.Int64()));
	
	if (delay32 < 0)
		{
		delay32 = 0;
		}

	TTimeIntervalMicroSeconds32 time32 = TTimeIntervalMicroSeconds32(delay32);
	
	HighRes(time32);
	}
	
EXPORT_C void CT_LbsTimerUtils::CancelTimer()
	{
	Cancel();
	}
	
/**
  Function : RunL
  Description : Gives the status of the operation 
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
void CT_LbsTimerUtils::RunL()
	{
	if (iObserver)
		{
		iObserver->HandleTimerL(iTimerId, iTargetTime);
		}
	}




