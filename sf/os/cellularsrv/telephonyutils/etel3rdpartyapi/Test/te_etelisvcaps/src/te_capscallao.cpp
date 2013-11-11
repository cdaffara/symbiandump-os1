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
// Implementation of AO to establish/hangup/hold calls with Etel3rdParty.
// 
//

/**
 @file 
 @internalTechnology
*/
 
#include "te_capscallao.h"

/**
Dial New Call Active Object
**/
CTestDialNewCallAct* CTestDialNewCallAct::NewLC(CTestStep* aTestStep, CTelephony* aTelephony)
	{
	CTestDialNewCallAct* obj = new(ELeave) CTestDialNewCallAct(aTestStep, aTelephony, NULL);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestDialNewCallAct* CTestDialNewCallAct::NewL(CTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent)
	{
	CTestDialNewCallAct* obj = new(ELeave) CTestDialNewCallAct(aTestStep, aTelephony, aParent);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop(obj);
	return obj;
	}
	
CTestDialNewCallAct::CTestDialNewCallAct(CTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent)
	: CEtelIsvActBase(aTestStep, aTelephony), iParent(aParent)
	{  
	}

void CTestDialNewCallAct::ConstructL()
	{	
	CActiveScheduler::Add(this);
	}

CTestDialNewCallAct::~CTestDialNewCallAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	}

void CTestDialNewCallAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EDialNewCallCancel);
	}

void CTestDialNewCallAct::TestDialNewCall(TTestCallParams& aTestCallParams)
	{
 	iTestCallParams = &aTestCallParams;

	// Make a new call
	iTelephony->DialNewCall(iStatus, *(iTestCallParams->iCallParamsV1Pckg) ,iTestCallParams->iTelAddress.iTelNumber, iTestCallParams->iCallId, iTestCallParams->iLine);
	SetActive(); 
	}
	
void CTestDialNewCallAct::RunL()
	{
	if(!iParent)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		iParent->ActionParent(iStatus.Int(), MActionParent::EDialNewCall);
		}
	}


/**
Hangup Active Object
**/
CTestHangupAct* CTestHangupAct::NewLC(CTestStep* aTestStep, CTelephony* aTelephony)
	{
	CTestHangupAct* obj = new(ELeave) CTestHangupAct(aTestStep, aTelephony, NULL);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestHangupAct* CTestHangupAct::NewL(CTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent)
	{
	CTestHangupAct* obj = new(ELeave) CTestHangupAct(aTestStep, aTelephony, aParent);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop();//obj
	return obj;
	}
	
CTestHangupAct::CTestHangupAct(CTestStep* aTestStep, CTelephony* aTelephony, MActionParent* aParent)
	: CEtelIsvActBase(aTestStep, aTelephony), iParent(aParent)
	{  
	}

void CTestHangupAct::ConstructL()
	{	
	CActiveScheduler::Add(this);
	}

CTestHangupAct::~CTestHangupAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	}

void CTestHangupAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EHangupCancel);
	}

void CTestHangupAct::TestHangup(TTestCallParams& aTestCallParams) 
			   
	{
 	iTestCallParams = &aTestCallParams;

	// Hangup existing call
	iTelephony->Hangup(iStatus, iTestCallParams->iCallId);
	SetActive(); 
	}
	
void CTestHangupAct::RunL()
	{ 
	if(!iParent)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		iParent->ActionParent(iStatus.Int(), MActionParent::EHangup);
		}
	}


/**
Hold Active Object
*/ 
CTestHoldAct* CTestHoldAct::NewLC(CTestStep* aTestStep, CTelephony* aTelephony)
//Factory constructor
	{
	CTestHoldAct* obj = new(ELeave) CTestHoldAct(aTestStep, aTelephony);
	CleanupStack::PushL(obj);
	obj->ConstructL();
	return obj;
	}
	
CTestHoldAct::CTestHoldAct(CTestStep* aTestStep, CTelephony* aTelephony)
	: CEtelIsvActBase(aTestStep, aTelephony)	 
	{  
	}

void CTestHoldAct::ConstructL()
	{	
	CActiveScheduler::Add(this);
	}

CTestHoldAct::~CTestHoldAct()
	{  
	Cancel();	// if any Req outstanding, calls DoCancel() to cleanup.
	}

void CTestHoldAct::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EHoldCancel);
	}

TInt CTestHoldAct::TestHold(TTestCallParams& aTestCallParams) 
	{
	iTelephony->Hold(iStatus, aTestCallParams.iCallId);
	SetActive(); 
	return iStatus.Int();
	}
	
void CTestHoldAct::RunL()
	{ 
	CActiveScheduler::Stop();
	}


