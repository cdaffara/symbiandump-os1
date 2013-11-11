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

/**
 @file ctlbsprivfwapidataobserver.cpp
*/


#include "ctlbsprivfwapidataobserver.h"
#include <lbs.h>
#include <lbs/lbsadmin.h>
#include "ctlbsprivfwbaserequest.h"


//_LIT(KSendResponseAO, "SendResponseAO");

CT_LbsPrivFwApiDataObserver::~CT_LbsPrivFwApiDataObserver()
/**
 * 	Destructor
 */
	{
	Cancel();
	}
	

CT_LbsPrivFwApiDataObserver::CT_LbsPrivFwApiDataObserver() : CActive(EPriorityHigh)
/**
 * 	Constructor - will not leave
 */
 	{ 	
 	CActiveScheduler::Add(this);
	}


CT_LbsPrivFwApiDataObserver* CT_LbsPrivFwApiDataObserver::NewL(MApiDataObserver* aTestStepObserver)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsPrivFwApiDataObserver* me = new(ELeave)CT_LbsPrivFwApiDataObserver();
	CleanupStack::PushL(me);
	me->ConstructL(aTestStepObserver);
	CleanupStack::Pop(me);
	return me;
	}

void CT_LbsPrivFwApiDataObserver::ConstructL(MApiDataObserver* aTestStepObserver)
	{
	iTestStepObserver = aTestStepObserver;
	}
	

	
void CT_LbsPrivFwApiDataObserver::DoCancel()
	{	
	SelfComplete();
	}
	
 
void CT_LbsPrivFwApiDataObserver::RunL()
	{
	iTestStepObserver->ProcessApiData(iRequestId, iCompletionCode);
	iStatus = KRequestPending;
	SetActive();	
	}

TInt CT_LbsPrivFwApiDataObserver::RunError(TInt aError)
	{	// called if RunL leaves. aError contains the error code
	return aError;
	}


void CT_LbsPrivFwApiDataObserver::Start()
	{	
	iStatus = KRequestPending;
	SetActive();
	}

//From MPosVerificationObserver
void CT_LbsPrivFwApiDataObserver::HandleVerifyComplete(TInt aRequestId, TInt aCompletionCode)
	{
	iRequestId = aRequestId;
	iCompletionCode = aCompletionCode;
	SelfComplete();
	}

void CT_LbsPrivFwApiDataObserver::SelfComplete()
	{
	TRequestStatus *h=&iStatus;
	User::RequestComplete(h, KErrNone); 
	}
