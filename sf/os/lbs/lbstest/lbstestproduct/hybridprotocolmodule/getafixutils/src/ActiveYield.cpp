/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
// ActiveYield.cpp
//

#include <e32base.h>
#include <e32std.h>

#include <lbs/test/activeyield.h>

#include <test/TestExecuteStepBase.h>


EXPORT_C void CheckForObserverEventTestsL(TTimeIntervalMicroSeconds32 aTimeout, CTestStep& aStep)
	{
	class CTimeout : public CTimer
		{
		CTestStep& iStep;

		void RunL()  
	    	{
	    	iStep.SetTestStepError(KErrTimedOut);
	    	CActiveScheduler::Stop();
			}

		CTimeout(CTestStep& aTestStep) : CTimer(EPriorityLow), iStep(aTestStep) { ; }
		
	public:
		static CTimeout* NewL(CTestStep& aTestStep)
			{
			CTimeout* p = new (ELeave) CTimeout(aTestStep);
			CleanupStack::PushL(p);
			p->ConstructL();
			CActiveScheduler::Add(p);
			CleanupStack::Pop(p);
			return p;
			}
		};
		
	CTimeout *pTimeout = CTimeout::NewL(aStep);
	pTimeout->After(aTimeout);	
	
	CActiveScheduler::Start();

	if(pTimeout->iStatus == KRequestPending)
		{
		pTimeout->Cancel();
//		User::WaitForRequest(pTimeout->iStatus);
		}
	
	pTimeout->Deque();
	delete pTimeout;
	
	if(EPass != aStep.TestStepResult())
		{
		User::Leave(aStep.TestStepResult());
		}
	}
	

EXPORT_C void ReturnToTestStep()
	{
	CActiveScheduler::Stop();
	}


#ifdef __NEVER_DEFINED__
//-------------------------------------------------------------------------------------------------
// This code is NOT used at this time... but I'm keeping it just in case.


class CActiveYielder : public CBase
	{
private:
	friend class CYieldSliceTimer;
	
public:
	static CActiveYielder* NewL();
	~CActiveYielder();
	
	void Yield(TTimeIntervalMicroSeconds32 aTimeout);
	void Unyield();

protected:
	CActiveYielder();
	void ConstructL();
	
private:
	CActiveSchedulerWait* iNested;
	CYieldSliceTimer*     iTimeout;	
	};


CActiveYielder::CYieldSliceTimer::CYieldSliceTimer(CActiveYielder& aYielder)
:CTimer(EPriorityLow),
 iYielder(aYielder)
	{		
	;
	}
	

void CActiveYielder::CYieldSliceTimer::RunL()
	{
	__ASSERT_ALWAYS(iYielder.iNested->IsStarted(), User::Invariant());
	iYielder.iNested->AsyncStop();
	}    	

void CActiveYielder::CYieldSliceTimer::SetTimeout(TTimeIntervalMicroSeconds32  aTimeout)
	{
	__ASSERT_ALWAYS(!iYielder.iNested->IsStarted(), User::Invariant());
	if(!IsAdded())
		CActiveScheduler::Add(this);

	if(IsActive())
		Cancel();

	After(aTimeout);
	}
	
//-------------------------------------------------------------------------------------------------

CActiveYielder* CActiveYielder::NewL()
	{
	CActiveYielder *p = new(ELeave) CActiveYielder;
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop(p);
	return p;
	}

CActiveYielder::~CActiveYielder()
	{
	if(iTimeout)
		{
		if(iTimeout->IsActive())
			iTimeout->Cancel();
			
		if(iTimeout->IsAdded())
			iTimeout->Deque();
			
		delete iTimeout;
		}
	
	if(iNested && iNested->IsStarted())
		iNested->AsyncStop();
		
	delete iNested;
	}
	
void CActiveYielder::Yield(TTimeIntervalMicroSeconds32 aTimeout)
	{
	__ASSERT_ALWAYS(iNested, User::Invariant());
	__ASSERT_ALWAYS(!iNested->IsStarted(), User::Invariant());
	__ASSERT_ALWAYS(iTimeout, User::Invariant());
	__ASSERT_ALWAYS(iTimeout->IsAdded(), User::Invariant());
	
	iTimeout->SetTimeout(aTimeout);
	iNested->Start();
	}
	
	
void CActiveYielder::Unyield()
	{
	__ASSERT_ALWAYS(iNested, User::Invariant());
	__ASSERT_ALWAYS(iNested->IsStarted(), User::Invariant());
	__ASSERT_ALWAYS(iTimeout, User::Invariant());
	__ASSERT_ALWAYS(iTimeout->IsAdded(), User::Invariant());
	
	if(iTimeout->IsActive())
		iTimeout->Cancel();
	
	iNested->AsyncStop();
	}

CActiveYielder::CActiveYielder()
	{
	;
	}
	
void CActiveYielder::ConstructL()
	{
	iNested = new (ELeave) CActiveSchedulerWait;
	iTimeout = new (ELeave) CYieldSliceTimer(*this);
	}

#endif // __NEVER_DEFINED__
