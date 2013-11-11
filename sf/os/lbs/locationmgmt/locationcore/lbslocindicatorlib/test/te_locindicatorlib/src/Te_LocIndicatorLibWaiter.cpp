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
 @file Te_LocIndicatorLibWaiter.cpp
*/

#include <e32base.h>
#include <e32std.h>

#include <test/TestExecuteStepBase.h>


void WaitForEventL(TTimeIntervalMicroSeconds32 aTimeout, CTestStep& aStep)
	{
	class CTimeout : public CTimer
		{
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
					
		private:
			void RunL()  
		    	{
		    	iStep.SetTestStepError(KErrTimedOut);
		    	CActiveScheduler::Stop();
				}
			
			CTimeout(CTestStep& aTestStep) : CTimer(EPriorityLow), iStep(aTestStep) 
				{ 
				;
				}
			
		private:
			CTestStep& iStep;
		
		};
		
	CTimeout *pTimeout = CTimeout::NewL(aStep);
	pTimeout->After(aTimeout);	
	CActiveScheduler::Start();
	if(pTimeout->iStatus == KRequestPending)
		{
		pTimeout->Cancel();
		}
	pTimeout->Deque();
	delete pTimeout;
	if(EPass != aStep.TestStepResult())
		{
		User::Leave(aStep.TestStepResult());
		}
	}
	

void ReturnToTestStep()
	{
	CActiveScheduler::Stop();
	}
