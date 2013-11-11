// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "ctlbsportedstepbase.h"
#include "ctlbsclientrequestor.h"
#include "mctlbsrequestobserver.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CT_LbsClientRequestor* CT_LbsClientRequestor::NewL(
	TUid                	aPsyUid,
	MCT_LbsRequestObserver*	aObserver,
	const TDesC&			aClientName,
    CTestExecuteLogger&		aLogger
    )
	{
    CT_LbsClientRequestor* self = new (ELeave) CT_LbsClientRequestor(aObserver, aPsyUid);
    CleanupStack::PushL(self);
    self->ConstructL(aClientName, aLogger);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CT_LbsClientRequestor::~CT_LbsClientRequestor()
    {
    iObserver = NULL;
    Cancel();
    iPosServer.Close();
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsClientRequestor::CT_LbsClientRequestor(
	MCT_LbsRequestObserver*   aObserver,
	TUid				aPsyUid)
    : CActive(EPriorityStandard), iObserver(aObserver), iCompletionCode(KNoEvent),
	iPsyUid(aPsyUid), iContinueEventRequesting(FALSE)
    {
    }

// EPOC default constructor can leave.
void CT_LbsClientRequestor::ConstructL(
	const TDesC& aClientName,
    CTestExecuteLogger&		aLogger
    )
	{
	iName = aClientName;
    iLogger = aLogger;
    User::LeaveIfError(iPosServer.Connect());
    iModuleInfo = TPositionInfo();
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CRequester::CompletionTimeUTC
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TTime& CT_LbsClientRequestor::CompletionTimeUTC() const
    {
    return iCompletionTimeUTC;
    }

// ---------------------------------------------------------
// CRequester::CompletionCode
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsClientRequestor::CompletionCode() const
    {
    return iCompletionCode;
    }

// ---------------------------------------------------------
// CT_LbsClientRequestor::ResetCompletionCode
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientRequestor::ResetCompletionCode()
	{
	iCompletionCode = KNoEvent;
	}

// ---------------------------------------------------------
// CRequester::StartRequest
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientRequestor::StartRequestL(TBool aDefaultPsy)
    {    
    _LIT(KService, "SAAA");
	if (aDefaultPsy == FALSE)
		{
		User::LeaveIfError(iPositioner.Open(iPosServer, iPsyUid));    
		}
	else 
		{
		User::LeaveIfError(iPositioner.Open(iPosServer));
		}
	
	
	iPositioner.SetRequestor(CRequestor::ERequestorService,
                             CRequestor::EFormatApplication,
                             KService);

    iPositioner.NotifyPositionUpdate(iModuleInfo, iStatus);
    SetActive();
    }

// ---------------------------------------------------------
// CRequester::RunL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientRequestor::RunL()
    {
    iCompletionCode = iStatus.Int();
    RequestComplete();
	if (iContinueEventRequesting)
		{
		iContinueEventRequesting = FALSE;
		RequestModuleStatusEvent();
		}
    }

// ---------------------------------------------------------
// CT_LbsClientRequestor::DoCancel
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientRequestor::DoCancel()
    {
	iPosServer.CancelRequest(EPositionServerNotifyModuleStatusEvent/*EPositionServerGetModuleStatus*/);
    }

// ---------------------------------------------------------
// CT_LbsClientRequestor::RequestComplete
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientRequestor::RequestComplete()
    {
	if (iObserver)
        {
        TRAPD(err, iObserver->RequestCompleteL());
        if(KErrNone != err)
        	{
        	_LIT(KClientRequestor, "Error in LbsClientRequestor: %d");
        	ERR_PRINTF2(KClientRequestor, err);
        	}
        }
    }

// ---------------------------------------------------------
// CT_LbsClientRequestor::RequestModuleStatusEvent
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientRequestor::RequestModuleStatusEvent()
	{
	istatusEvent.SetRequestedEvents(TPositionModuleStatusEvent::EEventAll);
	iPosServer.NotifyModuleStatusEvent(istatusEvent, iStatus, iPsyUid);
	if (!IsActive())
		{
		SetActive();
		}
	}

// ---------------------------------------------------------
// CT_LbsClientRequestor::CancelModuleStatusEvent
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientRequestor::CancelModuleStatusEvent()
	{
	iPosServer.CancelRequest(EPositionServerGetModuleStatus);
	}

// ---------------------------------------------------------
// CT_LbsClientRequestor::ReadModuleStatusEvent
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsClientRequestor::ReadModuleStatusEvent()
	{
	TPositionModuleStatus modstatus = TPositionModuleStatus();
	istatusEvent.GetModuleStatus(modstatus);
	return modstatus.DeviceStatus();
	}

// ---------------------------------------------------------
// CT_LbsClientRequestor::ReadModuleStatusClientApi
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsClientRequestor::ReadModuleStatusClientApi(TPositionModuleId aModuleId)
	{
	//Module status from Client API
	TPositionModuleStatus moduleStatus = TPositionModuleStatus();
	iPosServer.GetModuleStatus(moduleStatus, aModuleId);
	return moduleStatus.DeviceStatus();
	}

// ---------------------------------------------------------
// CT_LbsClientRequestor::ReadModuleStatusModulesApi
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CT_LbsClientRequestor::ReadModuleStatusModulesApi(CPosModules* aDb, TPositionModuleId aModuleId)
	{
	//Module info from Modules API
	TPositionModuleInfo moduleInfo;
	CPosModuleIdList* prioList = aDb->ModuleIdListLC();
	TInt modnr = prioList->Find(aModuleId);
	aDb->GetModuleInfoL(prioList->At(modnr), moduleInfo);
	CleanupStack::PopAndDestroy(prioList);
	return moduleInfo.IsAvailable();
	}

// ---------------------------------------------------------
// CT_LbsClientRequestor::ContinueRequesting
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientRequestor::ContinueRequesting()
	{
	iContinueEventRequesting = TRUE;;
	}

// ---------------------------------------------------------
// CT_LbsClientRequestor::ClosePositioner
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientRequestor::ClosePositioner()
	{
	iPositioner.Close();
	}

//  End of File  
