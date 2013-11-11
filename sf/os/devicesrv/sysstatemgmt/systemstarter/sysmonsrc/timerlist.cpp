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
// timerlist.h
// 
//

/**
 @file
 @internalComponent 
*/

#include "timerlist.h"
#include "../src/SysStartDebug.h"

CTimerList* CTimerList::NewL(TInt aPriority)
	{
	CTimerList* self = new(ELeave) CTimerList(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTimerList::CTimerList(TInt aPriority) : CTimer(aPriority)
	{ 
	CActiveScheduler::Add(this);		
	}

CTimerList::~CTimerList()
	{
	Cancel();
	iTimerList.ResetAndDestroy();
	}

void CTimerList::ConstructL()
	{
	CTimer::ConstructL();
	}

TInt32 CTimerList::AddL(const TTimeIntervalMicroSeconds32& aInterval, const TCallBack& aCallBack)
	{
	TTimerInfo* info = new(ELeave) TTimerInfo;
	
	info->iCallBack = aCallBack;
	info->iTime.UniversalTime();
	info->iTime += TTimeIntervalMicroSeconds32(aInterval);
	
	AddL(info);
	
	return reinterpret_cast<TInt32>(info);
	}

TInt32 CTimerList::AddL(const TTime& aTime, const TCallBack& aCallBack)
	{
	TTimerInfo* info = new(ELeave) TTimerInfo;
	
	info->iCallBack = aCallBack;
	info->iTime = aTime;

	AddL(info);
	
	return reinterpret_cast<TInt32>(info);
	}

void CTimerList::AddL(const TTimerInfo* aInfo)
	{
	TInt64 last = 0;
	
	TInt count = iTimerList.Count();
	if (count > 0)
		{
		last = iTimerList[count-1]->iTime.Int64();
		}
	
	TLinearOrder<TTimerInfo> timerSortOrder(CTimerList::TimerSortOrder);
	
	iTimerList.InsertInOrderAllowRepeatsL(aInfo, timerSortOrder);
	
	// Only reset timer if necessary.
	count++;
	if (last != iTimerList[count-1]->iTime.Int64())
		{
		// reset timer to the new last-of-the-list
		Cancel();
		AtUTC(iTimerList[count-1]->iTime);
		}
	}
	
void CTimerList::Remove(TInt32 aTimer)
	{
	TInt index = iTimerList.FindInAddressOrder(reinterpret_cast<TTimerInfo*>(aTimer));
	
	if (index != KErrNotFound)
		{
		delete reinterpret_cast<TTimerInfo*>(aTimer);
		iTimerList.Remove(index);
		}
	}

void CTimerList::RunL()
	{ // Leavescan, this implementation of this method does not leave but further derived implementations may if they choose
	TTime now;

	now.UniversalTime();
	
	TInt count;

	for (count = iTimerList.Count(); 
		((count > 0) && (iTimerList[count-1]->iTime <= now));
		count--)
		{
		// Always remove from the end of the sorted list
		
		iTimerList[count-1]->iCallBack.CallBack();
		delete iTimerList[count-1];
		iTimerList.Remove(count-1);
		};
		
	// reschedule to run again at the expiry date of next repository on the list, if any
	count = iTimerList.Count();
	if (count > 0)
		{
		AtUTC(iTimerList[count-1]->iTime);
		}
	}

TInt CTimerList::TimerSortOrder(const TTimerInfo &aInfo1, const TTimerInfo &aInfo2)
	{
	return static_cast<TInt>(aInfo2.iTime.Int64() - aInfo1.iTime.Int64());
	}

#ifdef _DEBUG
TInt CTimerList::RunError(TInt aError)
#else
TInt CTimerList::RunError(TInt /*aError*/)
#endif
	{
	DEBUGPRINT2(_L("CTimerList: RunError called with error=%d"), aError);

	return KErrNone;
	}
