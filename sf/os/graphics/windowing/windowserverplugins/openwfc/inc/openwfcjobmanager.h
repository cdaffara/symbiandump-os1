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

#ifndef OPENWFCJOBMANAGER_H_
#define OPENWFCJOBMANAGER_H_

#include <e32std.h>
#include <e32base.h>
#include <e32debug.h>

#include <EGL/egl.h>
#include <graphics/eglsynchelper.h>
#include <WF/wfc.h>

class COpenWfcWrapper;
class COpenWfcMonitorThread;
class TOpenWfcJob;

#if defined(ENABLE_JQ_LOGGING)
#define JQLOG(X)  RDebug::Printf X
#else
#define JQLOG(X)
#endif

/**
 * Controls the OpenWF composition
 */
class COpenWfcJobManger : public CBase
	{
private:

    enum EOpenWfcJobId
        {
            EOpenWfcInvalidJobId = 0,
            EOpenWfcComposeJobId,
            EOpenWfcPauseCompositionJobId,
            EOpenWfcResumeCompositionJobId,
        };
    
	/**
	 * Stores the parameters associated with a composition
	 */
	struct TComposeRequestDetails
		{
		TComposeRequestDetails();
		void Set(TRequestStatus* aCommitRequest, TThreadId aCallingThread);
		void Reset();
		TRequestStatus* iCommitRequest;
		TThreadId iCallingThread;
		TDblQueLink iDlink;
		};
	
	/**
	 * Auxilliary class to insure the release of a lock
	 */
	class Guard
		{
		public:
		Guard(RMutex& aLock);
		~Guard();
		private:
		    RMutex& iLock;
		};
	
public:
	/**
	 * Two-phased constructor.
	 */
	static COpenWfcJobManger* NewL(COpenWfcWrapper& aOpenWfcWrapper, 
                                    WFCDevice aDevice, 
                                    WFCContext aContext,
                                    TInt aManagerId);
	
	/**
	 * Two-phased constructor.
	 */
	static COpenWfcJobManger* NewLC(COpenWfcWrapper& aOpenWfcWrapper, 
                                    WFCDevice aDevice, 
                                    WFCContext aContext,
                                    TInt aManagerId);
	/**
	 * Destructor.
	 */
	virtual ~COpenWfcJobManger();
	
	/**
	 * Request to compose the specified scene 
	 * 
	 * @param aCompleted The notification to be returned when the composition completes
	 * @param aUpdatePolicy Update policy to be used
	 * @param the Pointer to the first element from the element list containing the scene to be used by the next commit
	 * 
	 */
	void ComposeRequest(TRequestStatus* aCompleted);
	
	/**
	 * Request to pause the composition
	 */
	void CompositionPauseRequest();
	/**
	 * Request to resume the composition
	 */
	void CompositionResumeRequest();
	
    /**
     * Appends to the compose details list the first detail object from the compose detail list
     * 
     * @return NULL if the compose detail pool is empty or a pointer to the appended detail object
     */
	TComposeRequestDetails* AppendDetailsFromPool();

    /**
     * Executes an outstanding command, if any.
     * 
     */
    void DoExecuteJob();
    
private:
    /**
     * Executes a compose request.
     * Intended to be invoked by a job
     * 
     * @param Reference to the invoking job
     */
    void DoComposeJob();
    
    /**
     * Executes a pause composition request
     * Intended to be invoked by a job
     * 
     * @param Reference to the invoking job
     */
    void DoPauseCompositionJob();
    
    /**
     * Executes a resume composition request.
     * Intended to be invoked by a job
     * 
     * @param Reference to the invoking job
     */
    void DoResumeCompositionJob();
        
	/**
	 * Private constructor
	 */
	COpenWfcJobManger(COpenWfcWrapper& aOpenWfcWrapper, 
						 WFCDevice aDevice, 
						 WFCContext aContext,
						 TInt aManagerId);
	
	/**
	 * Symbian constructor used with the two stage construction pattern
	 */
	void ConstructL();
	
	/**
	 * Implements a fence operation
	 */
	void Fence();
	
	/**
	 * Waits until the sync is signalled
	 */
	void WaitForSync();
	
	/**
	 * Activates the composition on the current context
	 */
	void Activate();
	
	/**
	 * Deactivates the composition on the current context
	 */
	void Deactivate();
	
	/**
	 * Execute a composition on the current context
	 */
	void Compose();
	
	/**
	 * Execute a commit on the current context
	 */
	void Commit();
	
	/**
	 * Completes all the outstanding requests stored locally
	 */
	void CompleteComposeRequests(TInt aResult);
	
    /**
     * Completes all the outstanding pause/resumerequests requests
     */
    void CompletePauseResumeRequest(TInt aResult);
    
private:
	// 
	TDblQue<TComposeRequestDetails> iComposeDetailsPool;
	TDblQueIter<TComposeRequestDetails> iComposeDetailsPoolIter;
	TDblQue<TComposeRequestDetails> iComposeDetailsList;
	TDblQueIter<TComposeRequestDetails> iComposeDetailsListIter;
    RMutex iJobLock;
    RMutex iCommandLock;
    COpenWfcWrapper& iOpenWfcWrapper;
	WFCDevice iDevice;
	WFCContext iContext;
	volatile TBool iCompositionPaused;
	volatile TBool iPausedComposePending;
	COpenWfcMonitorThread*	iThreadManager;
	TInt iManagerId;
	EGLSyncKHR iSync;
	EGLDisplay iEglDisplay;
    TRequestStatus iPauseResumeRequestStatus;
    TThreadId iPauseResumeThread;
    EOpenWfcJobId iOutstandingJob;
    static const TInt KComposeDetailsPoolSize = 16;
	};

inline void COpenWfcJobManger::Fence()
	{
    wfcFence(iDevice, iContext, iEglDisplay, iSync);
	}

inline void COpenWfcJobManger::WaitForSync()
	{
	EGLTimeKHR timeout = (EGLTimeKHR) EGL_FOREVER_KHR;
	eglClientWaitSyncKHR(iEglDisplay, iSync, 0, timeout);
	}

inline void COpenWfcJobManger::Activate()
	{
	wfcActivate(iDevice, iContext);
	}

inline void COpenWfcJobManger::Deactivate()
	{
	wfcDeactivate(iDevice, iContext);
	}

inline void COpenWfcJobManger::Compose()
	{
	wfcCompose(iDevice, iContext, WFC_TRUE);
	}

inline void COpenWfcJobManger::Commit()
	{
	wfcCommit(iDevice, iContext, WFC_TRUE);
	}

#endif /* OPENWFCJOBMANAGER_H_ */
