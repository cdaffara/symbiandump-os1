// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "rsdisplaychangeao.h"
#include "displaypolicy.h"

CRsDisplayChangeNotifier::CRsDisplayChangeNotifier(MWsDisplayControl* aNextLevelInterface, CDisplayPolicy* aDisplayPolicy)
:CActive(EPriorityHigh),iWsStatus(NULL)
	{
	CActiveScheduler::Add(this);
	iNextLevelInterface = aNextLevelInterface;
	iDisplayPolicy = aDisplayPolicy;
	}

CRsDisplayChangeNotifier* CRsDisplayChangeNotifier::NewL(MWsDisplayControl* aNextLevelInterface, CDisplayPolicy* aDisplayPolicy)
	{
	CRsDisplayChangeNotifier* self = new(ELeave) CRsDisplayChangeNotifier(aNextLevelInterface, aDisplayPolicy);
	return self;
	}

void CRsDisplayChangeNotifier::IssueNotificationRequest()
	{
	iNextLevelInterface->NotifyOnDisplayChange(iStatus);
	SetActive();
	}
void CRsDisplayChangeNotifier::RegisterActiveStatus(TRequestStatus &aStatus)
	{
	iWsStatus = &aStatus;
	}

void CRsDisplayChangeNotifier::CancelNotificationRequest()
	{
	Cancel();
	}

void CRsDisplayChangeNotifier::RunL()
	{
	//IssueNotificationRequest will overwrite iStatus, save a copy first;
	TInt result = iStatus.Int();
	IssueNotificationRequest();
	
	//calculate min UI buffer size depending on physical resolutions
	TInt ret = iNextLevelInterface->NumberOfResolutions();
	RArray<MDisplayControlBase::TResolution> resolutions;
	CleanupClosePushL(resolutions);
	if(ret > 0)
		{
		//just ignore the error. if there's an error, an empty resolution list is passed in, and uibuffer
		//will remain the previous size
		iNextLevelInterface->GetResolutions(resolutions);
		}
	iDisplayPolicy->CalculateMinBufferSize(resolutions, ret);				
	CleanupStack::PopAndDestroy(&resolutions);
	
	if(iWsStatus && *iWsStatus == KRequestPending)
		{
		User::RequestComplete(iWsStatus, result);
		}
	}

void CRsDisplayChangeNotifier::DoCancel()
	{
	iNextLevelInterface->NotifyOnDisplayChangeCancel();
	if(iWsStatus && *iWsStatus == KRequestPending)
		User::RequestComplete(iWsStatus, KErrCancel);
	}

CRsDisplayChangeNotifier::~CRsDisplayChangeNotifier()
	{
	Cancel();
	}

CRsConfigChangeNotifier::CRsConfigChangeNotifier(MWsDisplayControl* aNextLevelInterface)
:CActive(EPriorityHigh), iWsStatus(NULL)
	{
	CActiveScheduler::Add(this);
	iNextLevelInterface = aNextLevelInterface;
	}

CRsConfigChangeNotifier* CRsConfigChangeNotifier::NewL(MWsDisplayControl* aNextLevelInterface)
	{
	CRsConfigChangeNotifier* self = new(ELeave) CRsConfigChangeNotifier(aNextLevelInterface);
	return self;
	}

void CRsConfigChangeNotifier::IssueNotificationRequest()
	{
	iNextLevelInterface->NotifyOnConfigChange(iStatus);
	SetActive();
	}

void CRsConfigChangeNotifier::RegisterActiveStatus(TRequestStatus &aStatus)
	{
	iWsStatus = &aStatus;
	}

void CRsConfigChangeNotifier::CancelNotificationRequest()
	{
	Cancel();
	}

void CRsConfigChangeNotifier::RunL()
	{
	//IssueNotificationRequest will overwrite iStatus, save a copy first;
	TInt result = iStatus.Int();
	IssueNotificationRequest();
	if(iWsStatus && *iWsStatus == KRequestPending)
		{
		User::RequestComplete(iWsStatus, result);
		}
	}

void CRsConfigChangeNotifier::DoCancel()
	{
	iNextLevelInterface->NotifyOnConfigChangeCancel();
	if(iWsStatus && *iWsStatus == KRequestPending)
		User::RequestComplete(iWsStatus, KErrCancel);
	}

CRsConfigChangeNotifier::~CRsConfigChangeNotifier()
	{
	Cancel();
	}
