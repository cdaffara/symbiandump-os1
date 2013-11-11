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
 @file ctlbsprivfwreqsubmitter.cpp
*/

#include "ctlbsprivfwreqsubmitter.h"
#include "ctlbsprivfwperftimingsrequest.h"


_LIT( KTxtRequestorName,	"Requester" );
_LIT( KTxtLcsClientName,	"LcsClient" );


CT_LbsPrivFwReqSubmitter::~CT_LbsPrivFwReqSubmitter()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwReqSubmitter::~CT_LbsPrivFwReqSubmitter"));
	Cancel();
	iReqInfoArray.Reset();
	}

CT_LbsPrivFwReqSubmitter::CT_LbsPrivFwReqSubmitter(RPointerArray<CT_LbsPrivFwBaseRequest>& aReqArray) 
    : CActive(EPriorityIdle), iReqArray(aReqArray)
/**
 * 	Constructor - will not leave
 */
 	{
 	CActiveScheduler::Add(this);
	}

CT_LbsPrivFwReqSubmitter* CT_LbsPrivFwReqSubmitter::NewL(CPrivFwProxy* aProxy, 
		CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aApiObserver, 
		TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep,
		RPointerArray<CT_LbsPrivFwBaseRequest>& aReqArray)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwReqSubmitter* me = new(ELeave)CT_LbsPrivFwReqSubmitter(aReqArray);
	CleanupStack::PushL(me);
	me->ConstructL(aProxy, aNetPrivacy, aApiObserver, aPrivacyHandler, aStep);
	CleanupStack::Pop(me);
	return me;
	}


void CT_LbsPrivFwReqSubmitter::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, 
		CT_LbsPrivFwApiDataObserver* aApiObserver, TPrivacyHandlerType aPrivacyHandler, 
		CT_LbsPrivFwStepBase* aStep)
	{
	iProxy = aProxy;
	iNetPrivacy = aNetPrivacy;
	iApiObserver = aApiObserver;
	iPrivacyHandler = aPrivacyHandler;
	iStep = aStep;
	
	INFO_PRINTF1(_L("CT_LbsPrivFwReqSubmitter::ConstructL()"));
	}

void CT_LbsPrivFwReqSubmitter::DoCancel()
	{	
	SelfComplete();
	}
	
 
void CT_LbsPrivFwReqSubmitter::RunL()
	{
	// create a new request info object
	iReqInfoArray.AppendL(CPosGSMPrivacyRequestInfo::NewL());

	TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
	const TInt maxNameLen = 20;
	TBuf<maxNameLen> reqInfoName;

	// Load request info with unique requestor and client name
	reqInfoName = KTxtRequestorName;
	reqInfoName.AppendNumFixedWidth(iCurrentRequest, EDecimal, 2);
	iReqInfoArray[iCurrentRequest]->SetRequestorL(reqInfoName, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);

	reqInfoName = KTxtLcsClientName;
	reqInfoName.AppendNumFixedWidth(iCurrentRequest, EDecimal, 2);
	iReqInfoArray[iCurrentRequest]->SetLCSClientL(reqInfoName, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
	iReqInfoArray[iCurrentRequest]->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);

	//Create and initialise the request object
	CT_LbsPrivFwPerfTimingsRequest* req = CT_LbsPrivFwPerfTimingsRequest::NewL(iProxy, iNetPrivacy, iApiObserver, iPrivacyHandler, iStep);		
	req->SetRequestInfo(iReqInfoArray[iCurrentRequest], gsmInfoType);
	req->SetDesiredResponse(EResponseTypeAccepted);
	req->SetDecision(CPosNetworkPrivacy::EDecisionAccepted);
	//Issue the verification request
	req->Start();
	
	// Add the request to the request array (which takes ownership)
	iReqArray.AppendL(req);
	
	// Start the time after the very first request has been submitted
	if (iCurrentRequest == 0)
		{
		iStartTime.UniversalTime();
		}
	
	// Keep the object active while there are more requests to submit
	if (++iCurrentRequest < iRequestCount)
		{
		iStatus = KRequestPending;
		SetActive();
		SelfComplete();
		}
	}


TInt CT_LbsPrivFwReqSubmitter::RunError(TInt aError)
	{	// called if RunL leaves. aError contains the error code
	INFO_PRINTF1(_L("CT_LbsPrivFwReqSubmitter::RunError()"));
	return aError;
	}


void CT_LbsPrivFwReqSubmitter::StartL(TInt aRequestCount)
	{
	iRequestCount = aRequestCount;
	iCurrentRequest = 0;
	RunL();
	}


void CT_LbsPrivFwReqSubmitter::SelfComplete()
	{
	TRequestStatus *h=&iStatus;
	User::RequestComplete(h, KErrNone); 
	}


// Getters
TTime CT_LbsPrivFwReqSubmitter::StartTime()
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwReqSubmitter::StartTime()"));
	return iStartTime;
	}


// Enables TEF PRINT macros
CTestExecuteLogger& CT_LbsPrivFwReqSubmitter::Logger()
	{
	return iStep->Logger();
	}

