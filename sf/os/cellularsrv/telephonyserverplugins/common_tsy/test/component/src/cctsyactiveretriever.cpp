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
// Implementation of CFilteringActiveScheduler and CActiveRetriever
// 
//

/**
 @file 
*/

#include "cctsyactiveretriever.h"


TInt CActiveRetriever::iSimultaneousRetrieveRequestsNumber(0);

//----------------------------------------------------------------------------
// Implementation of CFilteringActiveScheduler
//----------------------------------------------------------------------------
CFilteringActiveScheduler::~CFilteringActiveScheduler()
	{
	iRetrievers.Reset();
	}

//
void CFilteringActiveScheduler::AddRetrieverL(CActiveRetriever& aRetriever)
	{
	iRetrievers.AppendL(&aRetriever);
	}

//	
void CFilteringActiveScheduler::StartScheduler()
	{

	for (TInt i = 0; i<iRetrievers.Count(); i++)
		{
		if (iRetrievers[i]->IsActive())
			{
			// only if there is Active Retriever then actually starts Scheduler
			return CActiveScheduler::Start();
			}
		}
	}

//
void CFilteringActiveScheduler::WaitForAnyRequest()
	{
	do 	
		{
		CActiveScheduler::WaitForAnyRequest();
		}
	while (!IsOneOfMyRequestsComplete()); // WaitForAnyRequest while one of my requests is not complete 
	}


// check whether thera is any Retriever's request complete
TBool CFilteringActiveScheduler::IsOneOfMyRequestsComplete()
	{
	TBool ret(EFalse);
	
	for (TInt i = 0; i<iRetrievers.Count() && !ret; i++)
		{
		if (iRetrievers[i]->IsRequestComplete())
			{
			ret = ETrue;
			iRetrievers[i]->PerformCancelIfNeeded();
			}
		}
	
	return ret;
	}

//----------------------------------------------------------------------------
// Implementation of CActiveRetriever
//----------------------------------------------------------------------------
CActiveRetriever* CActiveRetriever::
			NewLC(CAsyncRetrieveVariableLengthBufferV2& aAsynchRetrieveBuffer)
    {
	CActiveRetriever* self =  new (ELeave) CActiveRetriever(aAsynchRetrieveBuffer);
	CleanupStack::PushL(self);
	return self;		
    }
    
//
CActiveRetriever* CActiveRetriever::
			NewL(CAsyncRetrieveVariableLengthBufferV2& aAsynchRetrieveBuffer)
    {
	return new (ELeave) CActiveRetriever(aAsynchRetrieveBuffer);
    }
    
//
CActiveRetriever::CActiveRetriever(CAsyncRetrieveVariableLengthBufferV2& aAsynchRetrieveBuffer) : 
    CActive(EPriorityStandard),
    iAsynchRetrieveBuffer(aAsynchRetrieveBuffer),
    iTestCase(ECaseNotSpecified),
    iPhase1Passed(EFalse)
    {
    CActiveScheduler::Add(this);
    }
    
//
CActiveRetriever::~CActiveRetriever()
    {
    Cancel();
    }
//
TBool CActiveRetriever::IsRequestComplete()
	{
	TBool ret(EFalse);
	
	if ( ( AsynchRetrieveBuffer().IsActive() && 
	       AsynchRetrieveBuffer().iStatus!=KRequestPending ) ||
	     ( IsActive() && iStatus!=KRequestPending) )
		{
		ret = ETrue;
		}
	
	return ret;
	}

//
void CActiveRetriever::PerformCancelIfNeeded()
	{
	if ( AsynchRetrieveBuffer().IsActive() && 
	     AsynchRetrieveBuffer().iStatus!=KRequestPending )
		{
		switch(TestCase())
			{
			case CActiveRetriever::ECasePhase2Cancel:
				if ( ! Phase1Passed() ) // cancel request after passing of 1st phase
					{
					SetPhase1Passed(ETrue);
					break;
					}
			case CActiveRetriever::ECasePhase1Cancel:
				AsynchRetrieveBuffer().Cancel();
				SetTestCase(CActiveRetriever::ECaseNotSpecified);
				break;
			case CActiveRetriever::ECaseNotSpecified:
			default:
				break;
			}
		}
	}

// resets pending requests number, STATIC 
TInt CActiveRetriever::ResetRequestsNumber()
	{
	TInt ret(iSimultaneousRetrieveRequestsNumber); 
	iSimultaneousRetrieveRequestsNumber = 0;
	return ret;
	}
//
TRequestStatus& CActiveRetriever::Status()
	{
	return iStatus;
	}

// AO's request function with test case parameter
void CActiveRetriever::Activate(TTestCase aTestCase)
    {
    if ( ECaseGeneralCancelCase != aTestCase )
    	{
        iTestCase = aTestCase;
        iSimultaneousRetrieveRequestsNumber++;
        SetActive();
    	}
    else
    	{
    	iAsynchRetrieveBuffer.Cancel();
	    iTestCase = ECaseNotSpecified;
    	}
    }
    
//   
void CActiveRetriever::RunL()
    {
    DoComplete(); // stops the Scheduler
    }

//
void CActiveRetriever::DoCancel()
    {
    if (IsActive())
	    {
	    iAsynchRetrieveBuffer.Cancel();
	    DoComplete();
	    }
    }
    
//
CActive& CActiveRetriever::AsynchRetrieveBuffer()
	{
	return iAsynchRetrieveBuffer;
	}
	
//
void CActiveRetriever::SetTestCase(CActiveRetriever::TTestCase aCase)
	{
	iTestCase = aCase;
	}

//	
CActiveRetriever::TTestCase CActiveRetriever::TestCase()
	{
	return iTestCase;
	}

//
void CActiveRetriever::SetPhase1Passed(TBool aPassed)
	{
	iPhase1Passed = aPassed;
	}

//	
TBool CActiveRetriever::Phase1Passed()
	{
	return iPhase1Passed;
	}

//
void CActiveRetriever::DoComplete()
	{
    if (!(--iSimultaneousRetrieveRequestsNumber))
	    {
    	CActiveScheduler::Stop();
    	}
	}
