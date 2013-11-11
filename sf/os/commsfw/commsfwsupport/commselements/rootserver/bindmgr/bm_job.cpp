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

/**
 @file 
 @internalComponent
*/

#include <commschan.h>
#include <cfshared.h>
using namespace CommsFW;
#include "bm_defs.h"


CJobTimeout* CJobTimeout::NewL(MBMNotifyTimerShot* aNotifier)
/** Creates new CJobTimeout object
@param aNotifier the object to notify of timeout
@return Pointer to the new CJobTimeout object.
@leave KErrNoMemory
*/
    {
    CJobTimeout* pTimeout = new (ELeave) CJobTimeout(aNotifier);
    CleanupStack::PushL(pTimeout);
    pTimeout->ConstructL(); // base method - add this to scheduler
    RS_DETLOG((KLogSubSysRS, KLogCode, 
		_L8("CJobTimeout::NewL &iStatus %08X"), &(pTimeout->iStatus)));
    CleanupStack::Pop(pTimeout);
    return pTimeout;
    }

void CJobTimeout::RunL()
/** Active Object virutal RunL method called when the timer expires
@leave
*/
    {
    RS_DETLOG((KLogSubSysRS, KLogEvent, 
		_L8("CJobTimeout::RunL &iStatus %08X, status %d"), &iStatus, iStatus.Int()));
    if((iStatus == KErrNone) && (iNotifier != NULL))
        {
        iNotifier->ModuleTimerShot();
        }
    }


CJobTimeout::CJobTimeout(MBMNotifyTimerShot* aNotifier) :
	CTimer(EPriorityStandard)
/** Constrcutor for CJobTimeout
@param aNotifier the object to notify of timeout
*/
    {
    iNotifier = aNotifier;
    __CFLOG_1(KLogSubSysRS, KLogEvent, 
		_L8("CJobTimeout &iStatus %08X"), &iStatus);
    }


CJob* CJob::NewL(TRequestStatus& aRsStatus, MBindManagerNotify *aBindManager)
/** Creates new CJob object
@param aRsStatus the TRequestStatus to complete when the job is finished
@param aBindManager the object to notify of timeout
@return Pointer to the new CJob object.
@leave KErrNoMemory
*/
	{
    CJob* pJob = new (ELeave) CJob(aRsStatus, aBindManager);
    CleanupStack::PushL(pJob);
    pJob->ConstructL();
    CleanupStack::Pop(pJob);
    __CFLOG_2(KLogSubSysRS, KLogCode, 
		_L8("CJob::NewL(%08X) &iRsStatus %08X"), pJob, &aRsStatus);
    return pJob;
	}

CJob::CJob(TRequestStatus& aRsStatus, MBindManagerNotify *aBindManager) :
	iRsStatus(aRsStatus),
	iBindManager(aBindManager)
// Surplus initialisers left as documentation of deliberate intent
//	iCompletionCode(KErrNone)
//	iCancelled(EFalse)
//	iTimeout(NULL)
/** Constructor for CJob
@param aRsStatus the TRequestStatus to complete when the job is finished
@param aBindManager the object to notify of timeout
*/
	{
	iRsStatus = KRequestPending;
	}

CJob::~CJob()
/** Destructor for CJob
*/
	{
    __CFLOG_1(KLogSubSysRS, KLogCode, _L8("CJob::~CJob(%08X)"), this);
	Dequeue();
	delete iTimeout;
	iTaskDataList.Close();
	}

void CJob::ConstructL()
/** Second phase constructor for CJob
*/
	{
	iTimeout = CJobTimeout::NewL(this);
	CActiveScheduler::Add(iTimeout);
	}

void CJob::ModuleTimerShot()
/** Virtual method called by timeout member
*/
	{
    RS_DETLOG((KLogSubSysRS, KLogCode, 
		_L8("CJob(%08X)::ModuleTimeShot - informing bindmanager"), this));
	iCompletionCode = KErrRSRequestTimedOut;

	// now tell bindmanager about each task that must be considered to have failed
	TInt i;
	for (i=0; i<iTaskDataList.Count(); i++)
		{
		iBindManager->JobTimedOut(iTaskDataList[i].iModule, 
								  iTaskDataList[i].iTaskId);
		}
	}

void CJob::Dequeue()
/** Remove this from the linked list
*/
	{
	iLink.Deque();
	}

void CJob::AddTaskL(TUint aTaskId, const TCFModuleNameF &aModule)
/** Add a task to this job
@param aTaskId the task identifier to add
@param aModule the name of the module which owns the task
@leave KErrNoMemory
*/
	{
	TTaskData newTask(aTaskId, aModule);
	User::LeaveIfError(iTaskDataList.Append(newTask));
    RS_DETLOG((KLogSubSysRS, KLogCode, _L8("CJob(%08x)::AddTaskL(%d, %S, ptr=%X)"), this, aTaskId, &aModule, aModule.Ptr()));
	}

void CJob::StartTimeout(TUint aMicroseconds)
/** Start the timeout for this job
@param aMicroseconds the length of the timeout
*/
	{
	iTimeout->After(aMicroseconds);
	}

void CJob::TaskCompleted(const TCFModuleNameF& aModuleName, 
						 const TUint aTaskId,
						 const TInt aStatus)
/** Method called by bindmanager when a task is completed. If all of the tasks on 
a job complete then the client's request will be completed and the job 
deleted
@param aModuleName the name of the module that owns the completed task
@param aTaskId the identifier of the completed task
@param aStatus the completion code for the task
*/
	{
	// compare with all tasks
    RS_DETLOG((KLogSubSysRS, KLogCode, _L8("CJob(%08x)::TaskCompleted - checking for (%S, %d)"), this, &aModuleName, aTaskId));
	TInt i;
	for (i=0; i<iTaskDataList.Count(); i++)
		{
		if (iTaskDataList[i].iTaskId == aTaskId &&
			iTaskDataList[i].iModule == aModuleName)
			{
			__CFLOG_2(KLogSubSysRS, KLogCode, 
				_L8("CJob::TaskCompleted - removing task entry %S:%d"),
				&aModuleName, aTaskId);
		    RS_DETLOG((KLogSubSysRS, KLogCode, _L8("--- matched at %d"), i));
			iTaskDataList.Remove(i);

			// record any error status if one has't already been noted
			if (KErrNone == iCompletionCode)
				{
				iCompletionCode = aStatus;
				}
			break;
			}
		}
	if (0 == iTaskDataList.Count())
		{
		__CFLOG_1(KLogSubSysRS, KLogCode,
			_L8("CJob::TaskCompleted - completing request %08X"), &iRsStatus);
		iTimeout->Cancel();
		if (!iCancelled)
			{
			TRequestStatus *status = &iRsStatus;
			User::RequestComplete(status, iCompletionCode);
			}
		delete this;
		}
	}

void CJob::Cancel()
/** Cancels the job
*/
	{
	TRequestStatus *status = &iRsStatus;
	iRsStatus = KErrCancel;
	User::RequestComplete(status, KErrCancel);
	iCancelled = ETrue;
	}

TBool CJob::HasStatus(const TRequestStatus& aRsStatus) const
/** Indicates whether this job will complete the supplied status request when the
job completes
@param aRsStatus the comparator
@return true if the status match
*/
	{
	return (&aRsStatus == &iRsStatus);
	}


