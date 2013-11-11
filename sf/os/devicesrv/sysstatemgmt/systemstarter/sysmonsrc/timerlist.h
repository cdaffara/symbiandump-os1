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
 @internalComponent 
*/

#ifndef __TIMERLIST_H__
#define __TIMERLIST_H__

#include <e32std.h>
#include <e32base.h>

/**
This class is an active object that holds a list of timers. 
When a new timer item is added, this class compare the newly added timer with the other timers 
in the the list and find out the timer that will expire the earliest. It then start the timer 
that will expire the earliest (if applicable). After the first one expire, it will run a timer 
for the next timer, and so on.

@internalComponent
*/
class CTimerList : public CTimer
	{
public:
	static CTimerList* NewL(TInt aPriority = EPriorityStandard);
	~CTimerList();

	TInt32 AddL(const TTimeIntervalMicroSeconds32& aInterval, const TCallBack& aCallBack);
	TInt32 AddL(const TTime& aTime, const TCallBack& aCallBack);
	void Remove(TInt32 aTimer);

protected:
	class TTimerInfo
		{
	public:
		TTime iTime;
		TCallBack iCallBack;
		};

	void RunL();
	void ConstructL();
	TInt RunError(TInt aError);
		
private:
	CTimerList(TInt aPriority); 	

	void AddL(const TTimerInfo* aInfo);
	
	static TInt TimerSortOrder(const TTimerInfo &aInfo1, const TTimerInfo &aInfo2);
	
private:
	RPointerArray<TTimerInfo> iTimerList;
	};

#endif
