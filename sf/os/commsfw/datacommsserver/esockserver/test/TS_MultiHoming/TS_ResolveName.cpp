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
 
#include "TS_MultiHomingStep.h"
#include "TS_ResolveName.h"

// From ../../networking/inhook6/inc/dnd_err.h
const TInt KErrDndNameNotFound = -5120; 

/*       
 * Resolve a hostname to an address
 */
CTS_ResolveName::CTS_ResolveName()
	{
	iTestStepName = KResolveName;
	}

CTS_ResolveName::~CTS_ResolveName()
	{	
	}


enum TVerdict CTS_ResolveName::doTestStepPreambleL(void)
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KResolveName, KOomTest, iIsOOMTest)))
		iIsOOMTest=EFalse;
	return EPass;
	}

enum TVerdict CTS_ResolveName::doTestStepL(void)
/**
 * Resolve a hostname to address
 * @return The test step verdict  
 */
	{			
	TInt err = KErrNone;
	TInt numOfResolvers = 0;

	
	err = GetIntFromConfig(KResolveName, KNumOfResolvers, numOfResolvers);
	if ((!err) || (numOfResolvers < 1))
		{
		return EInconclusive;
		}

	RArray<CResolveName*> resolverArray;
	RArray<TBool> expectResolveToSucceedArray;
	RArray<TBool> expectResolveToTimeoutArray;
	RArray<TBool> expectResolveToBeNotReadyArray;

	TInt i;
	for (i = 0; i < numOfResolvers; i++)
		{
				
		CResolveName* newResolver = CResolveName::NewL();
			 
		TConnDetails *theConn;
		TBool expectSuccess = ETrue;
		TBool expectTimeout = EFalse; 
		TBool expectNotReady = EFalse;
		TBool explicitResolve = EFalse;
		TInt protocol = KProtocolInetTcp;
		
		TName	hostName;  
		GetResolverConfig(i, hostName, protocol, expectSuccess, expectTimeout, expectNotReady, explicitResolve, &theConn);
				
		expectResolveToSucceedArray.Append(expectSuccess);   
		expectResolveToTimeoutArray.Append(expectTimeout); 
		expectResolveToBeNotReadyArray.Append(expectNotReady); 

		StepLog(_L("Trying to resolving[%i]: %S"), i, &hostName);			
		err = newResolver->DoResolution(hostName, theConn, protocol, this, explicitResolve);		
		if (err == KErrNone)
			{
			err = resolverArray.Append(newResolver);
			if (err != KErrNone)
				{
				newResolver->DoCancel();
				delete newResolver;
				newResolver = 0;
				//Log(KELogger,err);		// We can't keep a ref outside the loop...
				}
			}
//		else
//			Log (KELogger,err);
		}	// for (i<numOfSockets)
	
	CEnhancedScheduler::Start();	// And start them off...
	
	// Wait for all to finish / time out.	
	TInt result(0);
	for (i = 0; i < resolverArray.Count(); i++)
		{
		result = resolverArray[i]->GetResult();
		StepLog(_L("Resolving %i: returned:%i"), i, result);

		if(result == KErrNotFound  && !expectResolveToSucceedArray[i] 
			||	result == KErrDndNameNotFound && !expectResolveToSucceedArray[i]
			||  result == KErrNone && expectResolveToSucceedArray[i] 
			||  result == KErrNotReady && expectResolveToBeNotReadyArray[i]
			||  result == KErrTimedOut && expectResolveToTimeoutArray[i] 
			||  result == KErrDndNameNotFound && expectResolveToTimeoutArray[i] )
			{
		
			}
		else
			{
			// fail all case
			iTestStepResult = EFail;
			}
			
		
		delete resolverArray[i];
		}

	expectResolveToSucceedArray.Close();
	expectResolveToTimeoutArray.Close();    
	expectResolveToBeNotReadyArray.Close();    
	resolverArray.Close();
	return iTestStepResult;

	}


					
TInt CResolveName::DoResolution(TName aHostName, TConnDetails *aConn, TInt aProtocol, 
								CTS_ResolveName *aOwnerStep, TBool aExplicitResolve)
								
	{
	TInt err;
	iResolverHasSS = FALSE;	
	iHostName.Copy(aHostName.Ptr(), aHostName.Length());


	iOwnerStep = aOwnerStep;
	iTConnection = aConn;


	
	if (aExplicitResolve)		
		{		
		err = iResolver.Open(iTConnection->iSocketServ, KAfInet, 
			aProtocol, iTConnection->iConnection);		
		}
	else
		{						
		if (iTConnection != 0)
			// Use the SocketServ session of opened connection 
			err = iResolver.Open(iTConnection->iSocketServ, KAfInet, aProtocol);			
		else
			{
			// No connection specified to use so create session
			iResolverHasSS = TRUE;
			err = iResolverSS.Connect();
			if(err != KErrNone)
			    {
			    return err;
			    }
			err = iResolver.Open(iResolverSS, KAfInet, aProtocol);
			}
		}
		
	if (err != KErrNone)
		return err;			// No point going any further...
	
	iStatus = KRequestPending; 

	// Increment the counter before we start.
	iOwnerStep->iOwnerSuite->iScheduler->IncCount();
	SetActive();

	TRequestStatus* pS = &iStatus;
	User::RequestComplete(pS,KErrNone);

	return KErrNone;
	}


CResolveName::~CResolveName()
	{
	//Cancel();	
	if (iResolverHasSS)
	    {
	    iResolver.Close();
	    iResolverSS.Close();
	    }

	}

CResolveName* CResolveName::NewL()
	{
	
	CResolveName* self = new(ELeave) CResolveName;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CResolveName::ConstructL()
	{
	CEnhancedScheduler::Add(this);
	}

void CResolveName::DoCancel()
	{
	iResolver.Close();
	}

void CResolveName::RunL()
	{
	
	switch (iState)
		{
		case EResReady:
			{
			iResolver.GetByName(iHostName, iResultingName, iStatus);
			iState = EWaitingForResult;
			SetActive();
			}
			break;

		case EWaitingForResult:
			{
				iState = EResComplete;
				iResolverResult = iStatus.Int();
				iOwnerStep->iOwnerSuite->iScheduler->DecCount();
				if(0 == iOwnerStep->iOwnerSuite->iScheduler->GetCount())
						CEnhancedScheduler::Stop();
			}		
			break;
				
		default:
				iOwnerStep->StepLog(KTxtWhatHappened);
			break;
		}
		//iOwnerStep->StepLog(KState, iState, iOwnerStep->iOwnerSuite->iScheduler->GetCount());
	}

