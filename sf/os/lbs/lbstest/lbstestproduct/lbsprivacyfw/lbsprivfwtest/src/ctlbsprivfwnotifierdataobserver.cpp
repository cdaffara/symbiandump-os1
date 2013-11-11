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
 @file ctlbsprivfwnrhobserverao.cpp
*/


#include "ctlbsprivfwnotifierdataobserver.h"
#include <lbs.h>
#include <lbs/lbsadmin.h>


//_LIT(KSendResponseAO, "SendResponseAO");

CT_LbsPrivFwNotifierDataObserver::~CT_LbsPrivFwNotifierDataObserver()
/**
 * 	Destructor
 */
	{
//	DeleteProperties();
	Cancel();
	}
	

CT_LbsPrivFwNotifierDataObserver::CT_LbsPrivFwNotifierDataObserver() : CActive(EPriorityNormal)
/**
 * 	Constructor - will not leave
 */
 	{ 	
 	CActiveScheduler::Add(this);
	}


CT_LbsPrivFwNotifierDataObserver* CT_LbsPrivFwNotifierDataObserver::NewL(MNotifierDataObserver* aTestStepObserver)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwNotifierDataObserver* me = new(ELeave)CT_LbsPrivFwNotifierDataObserver();
	CleanupStack::PushL(me);
	me->ConstructL(aTestStepObserver);
	CleanupStack::Pop(me);
	return me;
	}


void CT_LbsPrivFwNotifierDataObserver::ConstructL(MNotifierDataObserver* aTestStepObserver)
	{
		iTestStepObserver = aTestStepObserver;
	}
	

void CT_LbsPrivFwNotifierDataObserver::Start()
	{	
	iStatus = KRequestPending;
	SetActive();
	}



void CT_LbsPrivFwNotifierDataObserver::DoCancel()
	{	
	SelfComplete();
	}
	
 
void CT_LbsPrivFwNotifierDataObserver::RunL()
	{
	if(iRequestComplete)
		{
		INFO_PRINTF3(_L("CT_LbsPrivFwNotifierDataObserver ReqId:%i, CompCode:%i"), iRequestId, iCompletionCode);
		iTestStepObserver->ProcessRequestComplete(iRequestId, iCompletionCode);
		}
	else
		{
		INFO_PRINTF2(_L("CT_LbsPrivFwNotifierDataObserver ReqId:%i, iPrivNotifierData:i"), iRequestId);
		iTestStepObserver->ProcessNotifierData(iRequestId, iPrivNotifierData);
		}
	
	iStatus = KRequestPending;
	SetActive();
	}


TInt CT_LbsPrivFwNotifierDataObserver::RunError(TInt aError)
	{	// called if RunL leaves. aError contains the error code
	return aError;
	}


void CT_LbsPrivFwNotifierDataObserver::NotifierDataAvailable(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData)
	{
	iRequestComplete = EFalse;
	iRequestId = aRequestId;
	iPrivNotifierData = aPrivNotifierData;
	iTestStepObserver->ProcessNotifierData(iRequestId, iPrivNotifierData);
	}


void CT_LbsPrivFwNotifierDataObserver::NotifierRequestComplete(TInt aRequestId, TInt aCompletionCode)
	{
	iRequestComplete = ETrue;
	iRequestId = aRequestId;
	iCompletionCode = aCompletionCode;
	SelfComplete();
	}

void CT_LbsPrivFwNotifierDataObserver::NotiferCtrlCloseDown()
	{
	iTestStepObserver->ProcessNotiferCtrlCloseDown();
	}

void CT_LbsPrivFwNotifierDataObserver::SelfComplete()
	{
	TRequestStatus *h=&iStatus;
	User::RequestComplete(h, KErrNone);
	}
