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
//

#include <centralrepository.h>
#include "lbsrootapi.h"
#include "lbssystemcontroller.h"
#include "lbsrootcenrepdefs.h"
#include "tprocessstartparams.h"
#include "cprocesslaunch.h"
#include "lbsdevlogger.h"
#include "lbsdevloggermacros.h"

const TInt32 KRootProcessResponseTimeout =  15000000; //15 s
const TInt KRootProcessMaxRetries = 60;

EXPORT_C RLbsSystemController::RLbsSystemController()
    {
    }
    
EXPORT_C void RLbsSystemController::InitializeL()
    {
	const TSecurityPolicy KReadPolicy(ECapabilityLocation);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);
	const TSecurityPolicy KStartupCompleteReadPolicy(ECapability_None);
	const TSecurityPolicy KStartupWritePolicy(ECapability_None);		

	TInt err = RProperty::Define(KLbsStartupRequestKey, 
	                             RProperty::EByteArray, 
	                             KReadPolicy, 
	                             KStartupWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}

	err = RProperty::Define(KLbsStartupCompleteKey, 
							RProperty::EInt, 
							KStartupCompleteReadPolicy, 
							KWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
		
	err = RProperty::Define(KLbsCloseDownRequestKey, 
							RProperty::EInt, 
							KReadPolicy, 
							KWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
		
	err = RProperty::Define(KLbsCloseDownCompleteKey, 	
							RProperty::EInt, 
							KReadPolicy, 
							KWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}

	// Save the Uid of the process which defined the properties
	// in the cenrep file owned by LbsRoot.
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Set(KSystemControllerCategoryKey, 
								TInt(RProcess().SecureId())));
	CleanupStack::PopAndDestroy(rep);
    }

EXPORT_C void RLbsSystemController::ShutDownL()
    {
	RProperty::Delete(KLbsStartupRequestKey);
	RProperty::Delete(KLbsStartupCompleteKey);
	RProperty::Delete(KLbsCloseDownRequestKey);
	RProperty::Delete(KLbsCloseDownCompleteKey);
    }

TBool RLbsSystemController::CheckLbsRootProcessIsUp()
	{
    // Check that the root process is present by attempting to open it.
    // If it's not, then there's no point going any further, as the attempt
    // to communicate will just hang.

	TInt err = KErrNone;
	RProcess process;
	TFullName fullName;
	TFindProcess findProcess;
	TBool running = EFalse;
	
	findProcess.Find(KLbsRootProcessSearchString);
	while (err = findProcess.Next(fullName), err == KErrNone)
		{
		TInt processOpenErr = process.Open(findProcess);
		if (processOpenErr == KErrNone)
			{
			TExitType exitType = process.ExitType();
			if (exitType == EExitPending)
				{
				// found a running instance of the process,
				// so assume everything is ok; return ETrue
				running = ETrue;
				process.Close();
				break;
				}
			}
			process.Close();
		}

	if (!running)
		{
		// LBSRoot process not running, so attempt to start it up now
		TProcessStartParams params;
		params.SetProcessFileName(KLbsRootFileName);
		params.SetProcessName(KLbsRootProcessName);
		params.SetRendezvousRequired(ETrue);
		
		err = CProcessLaunch::ProcessLaunch(params);
		running = (err == KErrNone);
		}
	
	return running;	
	}

/*
 * Called from one of the system components to request that the LBS
 * system be started
 */
EXPORT_C TInt RLbsSystemController::RequestSystemStartup()
    {
	LBSLOG(ELogP1, "RLbsSystemController::RequestSystemStartup()\n");
    
	__ASSERT_DEBUG(iLbsStartupComplete.Handle() != NULL, User::Invariant());
    __ASSERT_DEBUG(iLbsStartupRequest.Handle() != NULL, User::Invariant());

    TInt err = KErrNone;
	RTimer timer;
	timer.CreateLocal();

	for (TInt retries=0; retries<KRootProcessMaxRetries; ++retries)
    	{
		if(!CheckLbsRootProcessIsUp())
			{
			err = ELbsStartupRootNotFound;
			break;
			}
	    
		TRequestStatus status;
		TRequestStatus timerStatus;
			
		timer.After(timerStatus, static_cast<TTimeIntervalMicroSeconds32>(KRootProcessResponseTimeout));
		
		// Need to pass the process handle to lbsroot, so
		// that lbsroot can check this process has the correct
		// capabilities to start LBS.
		RProcess process;
		iLbsStartupComplete.Subscribe(status);
		TPckg<TProcessId> pckg(process.Id());
		err = iLbsStartupRequest.Set(pckg);
		process.Close();

		if(err != KErrNone)
			{
			iLbsStartupComplete.Cancel();
			User::WaitForRequest(status);
			
			timer.Cancel();
			User::WaitForRequest(timerStatus);

			LBSLOG2(ELogP1, "iLbsStartupRequest.Set() failed with %d\n", err);

			return(err);
			}
			
		User::WaitForRequest(status, timerStatus);
		
		if (timerStatus != KRequestPending)
			{
			// when timeout occurs
			iLbsStartupComplete.Cancel();
			User::WaitForRequest(status);
			err = KErrTimedOut;
			}
		else
			{
			// root process has responded
			timer.Cancel();
			User::WaitForRequest(timerStatus);
			iLbsStartupComplete.Get(err);

			break;
			}
    	}

    timer.Close();
	return err;
    }

/*
 * Called from one of the system components to request that the LBS
 * system be closed down.
 * This is the asynchronous vesion which returns a status response.
 */
EXPORT_C TInt RLbsSystemController::RequestSystemCloseDown()
	{
	return RequestSystemCloseDown(EFalse);
	}

/*
 * Called from one of the system components to request that the LBS
 * system be closed down.
 * This is the asynchronous vesion which returns a status response.
 * 
 * If aNoWaitForResponse is set, the method returns without waiting for
 * the system to be closed down (though it does wait until lbsroot has
 * acknowledged the closedown request).
 */
EXPORT_C TInt RLbsSystemController::RequestSystemCloseDown(TBool aNoWaitForResponse)
    {
    __ASSERT_DEBUG(iLbsCloseDownComplete.Handle() != NULL, User::Invariant());
    __ASSERT_DEBUG(iLbsCloseDownRequest.Handle() != NULL, User::Invariant());

    TInt err = KErrNone;
	RTimer timer;
	timer.CreateLocal();

	for (TInt retries=0; retries<KRootProcessMaxRetries; ++retries)
    	{
		if(!CheckLbsRootProcessIsUp())
			{
			err = ELbsCloseDownRootNotFound;
			break;
			}
	    
		TRequestStatus status;
		TRequestStatus timerStatus;
			
		timer.After(timerStatus, static_cast<TTimeIntervalMicroSeconds32>(KRootProcessResponseTimeout));
		
		iLbsCloseDownComplete.Subscribe(status);
		err = iLbsCloseDownRequest.Set(ELbsCloseDownAndConfirm);
		if(err != KErrNone)
			{
			iLbsCloseDownComplete.Cancel();
			User::WaitForRequest(status);
			
			timer.Cancel();
			User::WaitForRequest(timerStatus);
			return(err);
			}
			
		User::WaitForRequest(status, timerStatus);
		
		if (timerStatus != KRequestPending)
			{
			// when timeout occurs
			iLbsCloseDownComplete.Cancel();
			User::WaitForRequest(status);
			err = KErrTimedOut;
			}
		else
			{
			// root process has responded
			timer.Cancel();
			User::WaitForRequest(timerStatus); // completes when rproperty is deleted
			iLbsCloseDownComplete.Get(err);
			if (aNoWaitForResponse || (!aNoWaitForResponse && err != ELbsCloseDownInProgress))
				{
				break;
				}
			}
    	}

    timer.Close();
	return err;
    }
    
/*
 * Called from one of the system components to request that the LBS
 * processes be reset. (Used mainly for test purposes).
 */
EXPORT_C void RLbsSystemController::RequestSystemReset()
    {
	if(CheckLbsRootProcessIsUp())
		{
	    iLbsCloseDownRequest.Set(ELbsResetOnly);
		}    
    }

/*
 * Called from one of the system components to request that the LBS
 * Root process be shut down along with the processes it started.
 */
EXPORT_C TInt RLbsSystemController::RequestCompleteCloseDown()
    {
    __ASSERT_DEBUG(iLbsCloseDownComplete.Handle() != NULL, User::Invariant());
    __ASSERT_DEBUG(iLbsCloseDownRequest.Handle() != NULL, User::Invariant());

	if(!CheckLbsRootProcessIsUp())
		{
		return(ELbsCloseDownRootNotFound);
		}

    TRequestStatus status;
	iLbsCloseDownComplete.Subscribe(status);
	iLbsCloseDownRequest.Set(ELbsTotalCloseDown);
	User::WaitForRequest(status);
			
	TInt closeDownResult;
	iLbsCloseDownComplete.Get(closeDownResult);
	
	if (closeDownResult == ELbsCloseDownInProgress)
		{
		// Close down in progress, wait for it to finish before returning
		iLbsCloseDownComplete.Subscribe(status);
		User::WaitForRequest(status);
		iLbsCloseDownComplete.Get(closeDownResult);
		}
	
	return (closeDownResult);
    }

EXPORT_C void RLbsSystemController::OpenL(TUid /* aModuleId */)    
	{ 
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	TInt category;
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	TInt err = rep->Get(KSystemControllerCategoryKey, category);
	User::LeaveIfError(err);
	categoryUid = TUid::Uid(category);
	CleanupStack::PopAndDestroy(rep);

	User::LeaveIfError(iLbsStartupRequest.Attach(categoryUid, 
													KLbsStartupRequestKey));
	User::LeaveIfError(iLbsStartupComplete.Attach(categoryUid, 
													KLbsStartupCompleteKey));
	User::LeaveIfError(iLbsCloseDownRequest.Attach(categoryUid, 
													KLbsCloseDownRequestKey));
	User::LeaveIfError(iLbsCloseDownComplete.Attach(categoryUid, 
													KLbsCloseDownCompleteKey));
	}
    
EXPORT_C void RLbsSystemController::Close()    
    {
	if(iLbsStartupRequest.Handle() != NULL)
		{
	    iLbsStartupRequest.Cancel();
		iLbsStartupRequest.Close();
		}
	if(iLbsStartupComplete.Handle() != NULL)
		{
		iLbsStartupComplete.Cancel();
		iLbsStartupComplete.Close();
		}
	if(iLbsCloseDownRequest.Handle() != NULL)
		{
	    iLbsCloseDownRequest.Cancel();
		iLbsCloseDownRequest.Close();
		}
	if(iLbsCloseDownComplete.Handle() != NULL)
		{
	    iLbsCloseDownComplete.Cancel();
		iLbsCloseDownComplete.Close();
		}
    }
    
EXPORT_C void RLbsSystemController::NotifySystemStartRequest(TRequestStatus& aStatus)
    {
    __ASSERT_DEBUG(iLbsStartupRequest.Handle() != NULL, User::Invariant());
	iLbsStartupRequest.Subscribe(aStatus);
    }

/* The startup data is the RProcess.Handle() of the calling process.
*/
TInt RLbsSystemController::GetStartupData(TDes8& aData)
	{
	return iLbsStartupRequest.Get(aData);
	}

EXPORT_C void RLbsSystemController::SystemStartComplete(TInt aStartupResult)
    {
    __ASSERT_DEBUG(iLbsStartupComplete.Handle() != NULL, User::Invariant());
	iLbsStartupComplete.Set(aStartupResult);
    }
    
EXPORT_C void RLbsSystemController::NotifySystemCloseDownRequest(TRequestStatus& aStatus)
    {
    __ASSERT_DEBUG(iLbsCloseDownRequest.Handle() != NULL, User::Invariant());
	iLbsCloseDownRequest.Subscribe(aStatus);
    }

EXPORT_C void RLbsSystemController::SystemCloseDownResponse(TInt aResponse)
    {
    __ASSERT_DEBUG(iLbsCloseDownComplete.Handle() != NULL, User::Invariant());
	iLbsCloseDownComplete.Set(aResponse);
    }

EXPORT_C TInt RLbsSystemController::GetCloseDownType()
    {
    __ASSERT_DEBUG(iLbsCloseDownRequest.Handle() != NULL, User::Invariant());
    TInt closeDownType;
    iLbsCloseDownRequest.Get(closeDownType);
	return(closeDownType);
    }

EXPORT_C CLbsSystemStartupRequestHandler* CLbsSystemStartupRequestHandler::NewL(
                                              MLbsSystemStartupObserver* aCloseDown, 
                                                              TUid aProcessUid)
    {
	CLbsSystemStartupRequestHandler* self = new (ELeave)CLbsSystemStartupRequestHandler(aCloseDown);
	CleanupStack::PushL(self);
	self->ConstructL(aProcessUid);
	CleanupStack::Pop(self);
	return(self);
    }
    
EXPORT_C CLbsSystemStartupRequestHandler::~CLbsSystemStartupRequestHandler()
    {
	iLbsSystemController.Close();
    Cancel();
    }

CLbsSystemStartupRequestHandler::CLbsSystemStartupRequestHandler(MLbsSystemStartupObserver* aObserver) 
: CActive(EPriorityStandard),
  iObserver(aObserver)
    {
    CActiveScheduler::Add( this );     
    }

void CLbsSystemStartupRequestHandler::ConstructL(TUid aProcessUid)
    {
	iLbsSystemController.OpenL(aProcessUid);
    iLbsSystemController.NotifySystemStartRequest(iStatus);
	SetActive();
    }
     
EXPORT_C void CLbsSystemStartupRequestHandler::ConfirmStartup(TInt sStartupStatus)
    {
	LBSLOG2(ELogP1, "ConfirmStartup returning %d\n", sStartupStatus);

    iLbsSystemController.SystemStartComplete(sStartupStatus);
    }
    
void CLbsSystemStartupRequestHandler::RunL()
    {
	LBSLOG(ELogP1, "CLbsSystemStartupRequestHandler::RunL()\n");

    iLbsSystemController.NotifySystemStartRequest(iStatus);

    TProcessId id;
    TPckg<TProcessId> pckg(id);
    TInt err = iLbsSystemController.GetStartupData(pckg);
    RProcess process;
    process.Open(id);
    
    // Check the capabilities of the calling process
    TBool isValid(EFalse);
    if (process.HasCapability(ECapabilityLocation))
    	{
    	isValid = ETrue;
    	}
    else if (process.HasCapability(ECapabilityReadDeviceData))
    	{
    	isValid = ETrue;
    	}
    else if (process.HasCapability(ECapabilityCommDD))
    	{
    	isValid = ETrue;
    	}
    
    if (isValid)
    	{
		LBSLOG(ELogP1, "Calling OnStartupRequest\n");

        iObserver->OnStartupRequest();    	
    	}
    else
    	{
    	ConfirmStartup(ELbsStartupFail);
    	}
    process.Close();
    
	SetActive();
    }
    
void CLbsSystemStartupRequestHandler::DoCancel()
    {
	
    }
//

EXPORT_C CLbsSystemCloseDownRequestHandler* CLbsSystemCloseDownRequestHandler::NewL(
                                              MLbsSystemCloseDownObserver* aObserver, 
                                                              TUid aProcessUid)
    {
	CLbsSystemCloseDownRequestHandler* self = new (ELeave)CLbsSystemCloseDownRequestHandler(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aProcessUid);
	CleanupStack::Pop(self);
	return(self);
    }
    
EXPORT_C CLbsSystemCloseDownRequestHandler::~CLbsSystemCloseDownRequestHandler()
    {
	iLbsSystemController.Close();
    Cancel();
    }

CLbsSystemCloseDownRequestHandler::CLbsSystemCloseDownRequestHandler(MLbsSystemCloseDownObserver* aObserver) 
: CActive(EPriorityStandard),
  iObserver(aObserver)
    {
	CActiveScheduler::Add(this);
    }

void CLbsSystemCloseDownRequestHandler::ConstructL(TUid aProcessUid)
    {
	iLbsSystemController.OpenL(aProcessUid);
    iLbsSystemController.NotifySystemCloseDownRequest(iStatus);
	SetActive();
    }
     

EXPORT_C void CLbsSystemCloseDownRequestHandler::ConfirmCloseDown(TInt aResponse)
    {
    iLbsSystemController.SystemCloseDownResponse(aResponse);
    }
    
void CLbsSystemCloseDownRequestHandler::RunL()
    {
    iLbsSystemController.NotifySystemCloseDownRequest(iStatus);
    switch(iLbsSystemController.GetCloseDownType())
    	{
    	case RLbsSystemController::ELbsCloseDown:
    		{
    		iObserver->OnCloseDownRequest(EFalse); // just close down
    		break;
    		}
    	case RLbsSystemController::ELbsCloseDownAndConfirm:
    		{
    		iObserver->OnCloseDownRequest(ETrue); // confirm when closedown cpmplete
    		break;
    		}

    	case RLbsSystemController::ELbsResetOnly:
    		{
    		iObserver->OnResetRequest(); // just get the processes to clear their cache
    		break;
    		}
    		
    	case RLbsSystemController::ELbsTotalCloseDown:
    		{
    		iObserver->OnCompleteCloseDownRequest(); // Stop the LBS Root process as well
    		break;
    		}
    		
    	default:
    	break;
    	}
	SetActive();
    }
    
void CLbsSystemCloseDownRequestHandler::DoCancel()
    {	
    }

