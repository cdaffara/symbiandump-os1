// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <simulprocserver.h>

EXPORT_C void CSimulProcServer::SessionClosed()
	{
	iSessionCount--;
	if (iSessionCount == 0)
		CActiveScheduler::Stop();
	}
	

EXPORT_C CSimulProcServer::CSimulProcServer()
	: CServer2(EPriorityNormal)
	{
	}
	
	
EXPORT_C CSimulProcServer::~CSimulProcServer()
	{
	}	


EXPORT_C CSimulProcSession::CSimulProcSession()
/**
 * Constructor
 */
	{
	}

EXPORT_C CSimulProcSession::~CSimulProcSession()
/**
 * Destructor
 */
	{
	const CSimulProcServer* p=reinterpret_cast<const CSimulProcServer*>(Server());
//	// Shuts Down the server if this is the last open session
	const_cast<CSimulProcServer*>(p)->SessionClosed();
	}
	

	

	
EXPORT_C CSession2* CSimulProcServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& /*aMessage*/) const
/**
 * @param RMessage - RMessage for the session open
 * Secure version
 */
	{
	CSimulProcSession* session = new (ELeave) CSimulProcSession();
	const_cast<CSimulProcServer*>(this)->iSessionCount++;
	return session;
	}
	
	

EXPORT_C void CSimulProcSession::ServiceL(const RMessage2& aMessage)
/**
 * @param aMessage - Function and data for the session
 * Session was created by pure virtual CTestServer::NewSessionL()
 * Message Functions defined in TestExecuteClient.h
 * 
 * EOpenTestStep - Creates a new subsession
 * ERunTestStep - Executes the test step asynchronously
 * EAbortTestStep - Kill()'s the executing test step
 * ECloseTestStep - Free's the resource
 *
 * Secure and non-secure variants
 * There are two modes of operation:
 * Test step is opened with the shared data boolean set to FALSE:
 *		Create a new CStepControl instance and hence a new thread in its own heap
 *		Consecutive or Concurrent operation
 * Test step is opened with the shared data boolean set to TRUE:
 *		Create a CPersistentStepControl and keep reusing it, and its thread
 *		Consecutive operation only
 */
	{
	switch(aMessage.Function())
		{
		case EMMTSOpenTestStep :
			{
			TBuf<256> stepName;
			// Read the step name from the descriptor
			aMessage.ReadL(0,stepName);
			const CSimulProcServer* p=reinterpret_cast<const CSimulProcServer*>(Server());
			iTestStep = p->CreateTestStep(stepName);
			aMessage.Complete(KErrNone);
			}
			
			break;
		case EMMTSStartProcessing:
			{
			if (!iTestStep)
				aMessage.Complete(KErrNotReady);
			else
				{
				if (iActiveCallback)
					iActiveCallback->Cancel();
				delete iActiveCallback;
				iActiveCallback = NULL;
				iActiveCallback = new (ELeave) CActiveCallback(aMessage);
				iTestStep->StartProcessing(iActiveCallback->ActiveStatus());
				}
			}

			break;
		case EMMTSStopProcessing :
			{
			if (!iTestStep)
				aMessage.Complete(KErrNotReady);
			else
				{
				TBuf8<256> message;
				TVerdict verdict = iTestStep->EndProcessingAndReturnResult(message);
				TPckgBuf<TVerdict> v(verdict);
				aMessage.WriteL(0,v);
				aMessage.WriteL(1,message);
				aMessage.Complete(KErrNone);
				}			
			break;
			}
		case EMMTSClose :
			{
			aMessage.Complete(KErrNone);
			}

			break;
		default:
			break;
		}
	}

void CSimulProcSession::CActiveCallback::RunL()
	{
	iMessage.Complete(iStatus.Int());
	}
	
void CSimulProcSession::CActiveCallback::DoCancel()
	{
	
	}
	
CSimulProcSession::CActiveCallback::CActiveCallback(const RMessage2& aMessage)
	: CActive(EPriorityNormal), iMessage(aMessage)
	{
	CActiveScheduler::Add(this);
	}
	
TRequestStatus& CSimulProcSession::CActiveCallback::ActiveStatus()
	{
	SetActive();
	iStatus = KRequestPending;
	return iStatus;
	}
	
	
EXPORT_C CSimulProcTestStep::CSimulProcTestStep()
	{
	}
	
EXPORT_C CSimulProcTestStep::~CSimulProcTestStep()
	{
	}
