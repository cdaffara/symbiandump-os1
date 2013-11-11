// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "openwfcjobmanager.h"
#include "openwfcwrapper.h"
#include "openwfcthreadmanager.h"
#include "openwfcpanics.h"
#include <e32debug.h>

#include <graphics/eglsynchelper.h>

COpenWfcJobManger::TComposeRequestDetails::TComposeRequestDetails():
iCommitRequest(NULL),
iCallingThread(KNullThreadId)
    {
    
    }

void COpenWfcJobManger::TComposeRequestDetails::Set(TRequestStatus* aCommitRequest, TThreadId aCallingThread)
    {
    iCommitRequest = aCommitRequest;
    iCallingThread = aCallingThread;
    }

void COpenWfcJobManger::TComposeRequestDetails::Reset()
    {
    iCommitRequest = NULL;
    iCallingThread = KNullThreadId;
    }

COpenWfcJobManger::Guard::Guard(RMutex& aLock):
iLock(aLock)
    {
    iLock.Wait();
    }

COpenWfcJobManger::Guard::~Guard()
    {
    iLock.Signal();
    }

COpenWfcJobManger::COpenWfcJobManger(COpenWfcWrapper& aOpenWfcWrapper, 
                                           WFCDevice aDevice, 
                                           WFCContext aContext,
                                           TInt aManagerId):
iComposeDetailsPool(_FOFF(TComposeRequestDetails, iDlink)),
iComposeDetailsPoolIter(iComposeDetailsPool),
iComposeDetailsList(_FOFF(TComposeRequestDetails, iDlink)),
iComposeDetailsListIter(iComposeDetailsList),
iOpenWfcWrapper(aOpenWfcWrapper),
iDevice(aDevice),
iContext(aContext),
iCompositionPaused(ETrue),
iPausedComposePending(ETrue),
iThreadManager(NULL),
iManagerId(aManagerId),
iSync(EGL_NO_SYNC_KHR),
iEglDisplay(aOpenWfcWrapper.Display()),
iOutstandingJob(EOpenWfcInvalidJobId)
    {
    
    }

COpenWfcJobManger::~COpenWfcJobManger()
    {
    JQLOG(("** ENTER * COpenWfcJobManger::~COpenWfcJobManger()"));
    iCommandLock.Wait();
    iJobLock.Wait();
    
    CompleteComposeRequests(KErrNone);
    
    delete iThreadManager;

    
    TComposeRequestDetails* pDetail = NULL;
    iComposeDetailsListIter.SetToFirst();
    while ((pDetail = iComposeDetailsListIter++) != NULL)
        {
        pDetail->iDlink.Deque();
        delete pDetail;
        }

    iComposeDetailsPoolIter.SetToFirst();
    while ((pDetail = iComposeDetailsPoolIter++) != NULL)
        {
        pDetail->iDlink.Deque();
        delete pDetail;
        }
    
    eglDestroySyncKHR(iEglDisplay, iSync);
    iJobLock.Signal();
    iCommandLock.Signal();
    iJobLock.Close();
    iCommandLock.Close();
    JQLOG(("** EXIT * COpenWfcJobManger::~COpenWfcJobManger()"));
    }

COpenWfcJobManger* COpenWfcJobManger::NewLC(COpenWfcWrapper& aOpenWfcWrapper, 
                                                 WFCDevice aDevice, 
                                                 WFCContext aContext,
                                                 TInt aManagerId)
    {
    COpenWfcJobManger* self = new(ELeave) COpenWfcJobManger(aOpenWfcWrapper, aDevice, aContext, aManagerId);
    CleanupStack::PushL(self);
    self->ConstructL();
    return(self);
    }

COpenWfcJobManger* COpenWfcJobManger::NewL(COpenWfcWrapper& aOpenWfcWrapper, 
                                                  WFCDevice aDevice, 
                                                  WFCContext aContext,
                                                  TInt aManagerId)
    {
    COpenWfcJobManger* self = COpenWfcJobManger::NewLC(aOpenWfcWrapper, aDevice, aContext, aManagerId);
    CleanupStack::Pop(self);
    return self;
    }
    
void COpenWfcJobManger::ConstructL()
    {
    User::LeaveIfError(iJobLock.CreateLocal());
    User::LeaveIfError(iCommandLock.CreateLocal());
    User::LeaveIfNull(iThreadManager = COpenWfcMonitorThread::NewL(iManagerId, *this));
    
    TComposeRequestDetails* pDetail = NULL;
    for (TInt i = 0; i < KComposeDetailsPoolSize; i++)
        {
        pDetail = new (ELeave) TComposeRequestDetails();
        iComposeDetailsPool.AddLast(*pDetail);
        }
    
    EGLint attrib_list[1] = { EGL_NONE };
    
    iSync = eglCreateSyncKHR(iEglDisplay,
                             EGL_SYNC_REUSABLE_KHR,
                             attrib_list);
    
    if (iSync == EGL_NO_SYNC_KHR)
        {  //code inspection reveals the only reason eglsync will fail is for memory...
        User::Leave(KErrNoMemory);
        }
    }


COpenWfcJobManger::TComposeRequestDetails* COpenWfcJobManger::AppendDetailsFromPool()
    {
    iComposeDetailsPoolIter.SetToFirst();
    TComposeRequestDetails* pDetails = iComposeDetailsPoolIter;
    if (pDetails)
        {
        pDetails->iDlink.Deque();
        iComposeDetailsList.AddLast(*pDetails);
        }
    return pDetails;
    }


void COpenWfcJobManger::ComposeRequest(TRequestStatus* aCompleted)
    {
    JQLOG(("** ENTER * COpenWfcJobManger::ComposeRequest(0x%x) WAIT FOR LOCKS", aCompleted));
    Guard g(iCommandLock);
    
    JQLOG(("** ENTER * COpenWfcJobManger::ComposeRequest(0x%x) COMMAND LOCK ACQUIRED", aCompleted));
    {
    Guard g(iJobLock);
    JQLOG(("** ENTER * COpenWfcJobManger::ComposeRequest(0x%x) JOB LOCK ACQUIRED", aCompleted));
    if (iCompositionPaused)
        {
        JQLOG(("** COpenWfcJobManger::ComposeRequest() EOpenWfcComposeJobId TRIGER compose job when resumed"));
        iPausedComposePending = ETrue;
        }
    else
        {
        // in this point we expect that the oustanding command is compose if any
        JQLOG(("** COpenWfcJobManger::ComposeRequest() EOpenWfcComposeJobId AUTONOMUS compose job"));
        OPENWFC_ASSERT_DEBUG(iOutstandingJob == EOpenWfcComposeJobId || iOutstandingJob == EOpenWfcInvalidJobId, 
                            EPanicWfcThreadManagerCannotQueueJob);
        iOutstandingJob = EOpenWfcComposeJobId;
        }
        
    // if pJob is NULL it means we have to add a Compose job to the job queue
    
    TInt err = wfcGetError(iDevice);
    if (err)
        {
        JQLOG(("** COpenWfcJobManger::ComposeRequest() : ENTER FlushSceneElementChanges Error(%d)", err));
        }
    iOpenWfcWrapper.FlushSceneElementChanges();
    // cleaning, also the "dirt"
    err = wfcGetError(iDevice);
    if (err)
        {
        JQLOG(("** COpenWfcJobManger::ComposeRequest() : EXIT FlushSceneElementChanges Error(%d)", err));
        }
    
    if (aCompleted)
        {
        // Let's store whatever compose complete notifications we may have
        TComposeRequestDetails* pDetails = AppendDetailsFromPool();
        
        if (pDetails)
            {
            // set the details if we could find a detail
            JQLOG(("** COpenWfcJobManger::ComposeRequest(): StoreReq(0x%x))", aCompleted));
            pDetails->Set(aCompleted, RThread().Id());
            *aCompleted = KRequestPending;
            }
        else
            {
            // signal that enough is enough
            JQLOG(("** COpenWfcJobManger::ComposeRequest(): OverflowReq(0x%x))", aCompleted));
            RThread().RequestComplete(aCompleted, KErrOverflow);
            }
        }
    
    JQLOG(("** COpenWfcJobManger::ComposeRequest(0x%x) JOB LOCK RELEASED", aCompleted));
    }
    
    // if a job has been queued, trigger the composition (it can happen only in autonomous mode)
    if (iOutstandingJob == EOpenWfcComposeJobId)
        {
        JQLOG(("** COpenWfcJobManger::ComposeRequest(): iThreadManager->Signal()"));
        iThreadManager->Signal();
        }
    
    JQLOG(("** EXIT * COpenWfcJobManger::ComposeRequest()"));
    }

void COpenWfcJobManger::CompositionPauseRequest()
    {
    JQLOG(("** ENTER * COpenWfcJobManger::CompositionPauseRequest() WAIT FOR LOCKS"));
    Guard g(iCommandLock);
    JQLOG(("** ENTER * COpenWfcJobManger::CompositionPauseRequest() COMMAND LOCK ACQUIRED"));
    TBool newJob = EFalse; 
        {
        Guard g(iJobLock);
        JQLOG(("** ENTER * COpenWfcJobManger::CompositionPauseRequest() JOB LOCK ACQUIRED"));
        if (!iCompositionPaused)
            {
            iCompositionPaused = ETrue;
            if (iOutstandingJob == EOpenWfcComposeJobId)
                {
                JQLOG(("** COpenWfcJobManger::CompositionPauseRequest() found outstanding composition request"));
                iPausedComposePending = ETrue;
                }
        
            iOutstandingJob = EOpenWfcPauseCompositionJobId;
            iPauseResumeRequestStatus = KRequestPending;
            iPauseResumeThread = RThread().Id();
            newJob = ETrue;
            JQLOG(("** COpenWfcJobManger::CompositionPauseRequest() : trigger new job"));
            }
        JQLOG(("** ENTER * COpenWfcJobManger::CompositionPauseRequest() JOB LOCK RELEASED"));
        }
    
    if(newJob)
        {
        JQLOG(("** COpenWfcJobManger::CompositionPauseRequest() : Signal()"));
        iThreadManager->Signal();
        User::WaitForRequest(iPauseResumeRequestStatus);
        }
    
    JQLOG(("** EXIT * COpenWfcJobManger::CompositionPauseRequest()"));
    }

void COpenWfcJobManger::CompositionResumeRequest()
    {
    JQLOG(("** ENTER * COpenWfcJobManger::CompositionResumeRequest() WAIT FOR LOCK"));
    Guard g(iCommandLock);
    JQLOG(("** ENTER * COpenWfcJobManger::CompositionResumeRequest() COMMAND LOCK ACQUIRED"));
    TBool newJob = EFalse;
        {
        Guard g(iJobLock);
        JQLOG(("** ENTER * COpenWfcJobManger::CompositionResumeRequest() JOB LOCK ACQUIRED"));
        if (iCompositionPaused)
            {
            iOutstandingJob = EOpenWfcResumeCompositionJobId;
            iPauseResumeRequestStatus = KRequestPending;
            iPauseResumeThread = RThread().Id();
            newJob = ETrue;
            JQLOG(("** COpenWfcJobManger::CompositionResumeRequest() : trigger new job"));
            }
        JQLOG(("** COpenWfcJobManger::CompositionResumeRequest() JOB LOCK RELEASED"));
        }
        
    if(newJob)
        {
        JQLOG(("** COpenWfcJobManger::CompositionResumeRequest() : Signal"));
        iThreadManager->Signal();
        User::WaitForRequest(iPauseResumeRequestStatus);
        }
    
    JQLOG(("** EXIT * COpenWfcJobManger::CompositionResumeRequest()"));
    }

void COpenWfcJobManger::CompleteComposeRequests(TInt aResult)
    {
    TComposeRequestDetails* pDetail = NULL;
    iComposeDetailsListIter.SetToFirst();
    
    while ((pDetail = iComposeDetailsListIter++) != NULL)
        {
        RThread thread;
        if (pDetail->iCommitRequest && (thread.Open(pDetail->iCallingThread) == KErrNone))
            {
            JQLOG(("** COpenWfcJobManger::CompleteComposeRequests(): CompleteReq(0x%x) Result(%d))", pDetail->iCommitRequest, aResult));
            thread.RequestComplete(pDetail->iCommitRequest, aResult);
            thread.Close();
            }
        
        pDetail->iDlink.Deque();
        iComposeDetailsPool.AddLast(*pDetail);
        pDetail->Reset();
        }
    }

void COpenWfcJobManger::CompletePauseResumeRequest(TInt aResult)
    {
    RThread thread;
    if (thread.Open(iPauseResumeThread) == KErrNone)
        {
        TRequestStatus* rs= &iPauseResumeRequestStatus;
        JQLOG(("** COpenWfcJobManger::CompletePauseResumeRequest(): %d)", aResult));
        thread.RequestComplete(rs, aResult);
        thread.Close();
        }
    }

void COpenWfcJobManger::DoExecuteJob()
    {
    JQLOG(("** ENTER * COpenWfcJobManger::DoExecuteJob() Enter Wait for Job Lock"));
    iJobLock.Wait();
    JQLOG(("** COpenWfcJobManger::DoExecuteJob(%d)", iOutstandingJob));
    EOpenWfcJobId job = iOutstandingJob;
    iOutstandingJob = EOpenWfcInvalidJobId;
    switch (job)
        {
        case EOpenWfcComposeJobId:
            DoComposeJob();
            break;
        case EOpenWfcPauseCompositionJobId:
            DoPauseCompositionJob();
            break;
        case EOpenWfcResumeCompositionJobId:
            DoResumeCompositionJob();
            break;
        default:
            iJobLock.Signal();
            break;
        }
    
    JQLOG(("** ENTER * COpenWfcJobManger::DoExecuteJob() Exit"));
    }

void COpenWfcJobManger::DoComposeJob()
    {
    JQLOG(("** ENTER * COpenWfcJobManger::DoComposeJob()"));
    Commit();
    
    Fence();
    WaitForSync();
    
    // complete all the notifications stored until this momment
    CompleteComposeRequests(KErrNone);
    JQLOG(("** COpenWfcJobManger::DoComposeJob() LOCK RELEASED"));
    
    iJobLock.Signal();
    JQLOG(("** EXIT * COpenWfcJobManger::DoComposeJob()"));
    }

void COpenWfcJobManger::DoPauseCompositionJob()
    {
    JQLOG(("** ENTER * COpenWfcJobManger::DoPauseCompositionJob()"));
    Deactivate();
    Fence();
    WaitForSync();
    CompletePauseResumeRequest(KErrNone);
    JQLOG(("** COpenWfcJobManger::DoPauseCompositionJob() RELEASE LOCK"));
    
    iJobLock.Signal();
    JQLOG(("** EXIT * COpenWfcJobManger::DoPauseCompositionJob() job lock released"));
    }

void COpenWfcJobManger::DoResumeCompositionJob()
    {
    JQLOG(("** ENTER * COpenWfcJobManger::DoResumeCompositionJob()"));
    
    iComposeDetailsListIter.SetToFirst();
    TComposeRequestDetails* pQueuedRequest = iComposeDetailsListIter;
    
    if (iPausedComposePending || pQueuedRequest)
        {
        JQLOG(("** COpenWfcJobManger::Commit()"));
        Commit();
        }
    
    Activate();
    
    Fence();
    WaitForSync();
    
    if (iPausedComposePending || pQueuedRequest)
        {
        CompleteComposeRequests(KErrNone);
        iPausedComposePending = EFalse;
        }
    
    iCompositionPaused = EFalse;
    CompletePauseResumeRequest(KErrNone);
    JQLOG(("** COpenWfcJobManger::DoResumeCompositionJob() RELEASE LOCK"));
    
    iJobLock.Signal();
    JQLOG(("** EXIT * COpenWfcJobManger::DoResumeCompositionJob() job lock released"));
    }
