// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ctlbsrequester.h"
#include "mctlbsrequestobserver.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CT_LbsRequester* CT_LbsRequester::NewL(
    TUid                	aPsyUid,
    MCT_LbsRequestObserver*	aObserver,
    CTestExecuteLogger&		aLogger
    )
    {
    CT_LbsRequester* self = new (ELeave) CT_LbsRequester(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aPsyUid, aLogger);
    CleanupStack::Pop();
    return self;
    }

// Two-phased constructor.
CT_LbsRequester* CT_LbsRequester::NewL(
    MCT_LbsRequestObserver*   aObserver,
    TTimeIntervalMicroSeconds aInterval,
    CTestExecuteLogger&		aLogger
    )
    {
    CT_LbsRequester* self = new (ELeave) CT_LbsRequester(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aInterval, aLogger);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CT_LbsRequester::~CT_LbsRequester()
    {
    iObserver = NULL;
    Cancel();
    iPositioner.Close();
    iPosServer.Close();
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsRequester::CT_LbsRequester(
    MCT_LbsRequestObserver*   aObserver)
    : CActive(EPriorityStandard), iObserver(aObserver)
    {
    }

// EPOC default constructor can leave.
void CT_LbsRequester::ConstructL(
     TUid        aPsyUid,
     CTestExecuteLogger&		aLogger
     )
	{
    iLogger = aLogger;
    User::LeaveIfError(iPosServer.Connect());
    iModuleInfo = TPositionInfo();
    iCompletionCode = KErrNone;
    User::LeaveIfError(iPositioner.Open(iPosServer, aPsyUid));
    _LIT(KService, "SAAA");
    iPositioner.SetRequestor(CRequestor::ERequestorService,
                             CRequestor::EFormatApplication,
                             KService);
    CActiveScheduler::Add(this);
    }

void CT_LbsRequester::ConstructL(
    TTimeIntervalMicroSeconds aInterval,
    CTestExecuteLogger&		aLogger
    )
	{
	iLogger = aLogger;
    User::LeaveIfError(iPosServer.Connect());
    iModuleInfo = TPositionInfo();
    iCompletionCode = KErrNone;
    User::LeaveIfError(iPositioner.Open(iPosServer));
    TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(aInterval);
    User::LeaveIfError(iPositioner.SetUpdateOptions(posOption));
    _LIT(KService, "SAAA");
    iPositioner.SetRequestor(CRequestor::ERequestorService,
                             CRequestor::EFormatApplication,
                             KService);
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CT_LbsRequester::CompletionTimeUTC
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
const TTime& CT_LbsRequester::CompletionTimeUTC() const
    {
    return iCompletionTimeUTC;
    }

// ---------------------------------------------------------
// CT_LbsRequester::CompletionCode
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsRequester::CompletionCode() const
    {
    return iCompletionCode;
    }

// ---------------------------------------------------------
// CT_LbsRequester::StartRequest
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsRequester::SetUpdateIntervalL(TTimeIntervalMicroSeconds aInterval)
    {	
	TPositionUpdateOptions posOption;
	posOption.SetUpdateInterval(aInterval);	
    User::LeaveIfError(iPositioner.SetUpdateOptions(posOption));
    }

// ---------------------------------------------------------
// CT_LbsRequester::StartRequest
// 
// (other items were commented in a header).
// ---------------------------------------------------------
void CT_LbsRequester::StartRequest()
    {    
    iPositioner.NotifyPositionUpdate(iModuleInfo, iStatus);
    SetActive();
    }

// LOCAL FUNCTIONS
void CleanOpRequestorStack(TAny* aAnArray)
    {
    // Cleanup function the requestor stack
    RRequestorStack* theStack = reinterpret_cast <RRequestorStack*> (aAnArray);
    theStack->ResetAndDestroy();
    }

// ---------------------------------------------------------
// CT_LbsRequester::SetRequestorStackL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsRequester::SetRequestorStackL(const TDesC& aService, const TDesC& aContact)
	{
	CRequestor* contactId = 
		CRequestor::New(CRequestor::ERequestorContact, CRequestor::EFormatTelephone, 
                    aContact);
    
    CRequestor* servId = 
        CRequestor::New(CRequestor::ERequestorService, CRequestor::EFormatApplication, 
        aService);

    // Create a requestor stack containing one contact and one service (the service must be last added)
    RRequestorStack stack = RRequestorStack();
    CleanupStack::PushL(TCleanupItem(CleanOpRequestorStack ,&stack));
    stack.Append(contactId);
    stack.Append(servId);
    
    CleanupStack::PushL(contactId);
    CleanupStack::PushL(servId);

    TInt err = iPositioner.SetRequestor(stack);
        
    CleanupStack::PopAndDestroy(servId);
    CleanupStack::PopAndDestroy(contactId);
    stack.Close();
    CleanupStack::PopAndDestroy(1); // CleanOpTp226 stack

	return err;
	}

// ---------------------------------------------------------
// CT_LbsRequester::RunL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsRequester::RunL()
    {
    iCompletionCode = iStatus.Int();
    iCompletionTimeUTC.UniversalTime();
    RequestComplete();
    }

// ---------------------------------------------------------
// CT_LbsRequester::DoCancel
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsRequester::DoCancel()
    {
    iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
    iCompletionTimeUTC.UniversalTime();
    RequestComplete();
    }

// ---------------------------------------------------------
// CT_LbsRequester::RequestComplete
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsRequester::RequestComplete()
    {
    if (iObserver)
        {
        TRAPD(err, iObserver->RequestCompleteL());
        if(KErrNone != err)
        	{
        	_LIT(KRequestCompleteError, "Error in LbsRequester: %d");
        	ERR_PRINTF2(KRequestCompleteError, err);
        	}
        }
    }

void CT_LbsRequester::GetPositionInfo(
            TPositionInfo& aModuleInfo)
    {
    aModuleInfo = iModuleInfo;
    }

// ---------------------------------------------------------
// CT_LbsRequester::SetUpdateType
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsRequester::SetUpdateType(const TInt& aUpdateType)
    {	
    iModuleInfo.SetUpdateType(aUpdateType); 
    }

//  End of File  
