/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*    Implementation of the CDosAsyncExecBase class.
*
*/


#include "dosasyncexecbase.h"

//
// ---------------------------------------------------------
// CDosAsyncExecBase Constructor
// ---------------------------------------------------------
//  

EXPORT_C CDosAsyncExecBase::CDosAsyncExecBase(const RMessagePtr2 aMessage) : CActive(EPriorityStandard), iMessage(aMessage)
{
	CActiveScheduler::Add(this);
}

//
// ---------------------------------------------------------
// CDosAsyncExecBase Desctructor
// ---------------------------------------------------------
//  

EXPORT_C  CDosAsyncExecBase::~CDosAsyncExecBase()
{
	Cancel();
}

//
// ---------------------------------------------------------
// CDosAsyncExecBase::RunL
//
// Inherited from CActive.
// ---------------------------------------------------------
//  

EXPORT_C void CDosAsyncExecBase::RunL()
{
	TBool autoDestroy = ETrue;
	TBool complete = ETrue;

	BeforeComplete(complete);
	if(complete)
	{
		iMessage.Complete(iStatus.Int());
		AfterComplete(autoDestroy);
		if(autoDestroy)
		{
			delete this;
		}
	}
}

//
// ---------------------------------------------------------
// CDosAsyncExecBase::BeforeComplete
//
// Default empty implementation. A derived class can override
// this.
// ---------------------------------------------------------
//  

EXPORT_C void CDosAsyncExecBase::BeforeComplete(TBool& /*aComplete*/)
{
}

//
// ---------------------------------------------------------
// CDosAsyncExecBase::AfterComplete
//
// Default empty implementation. A derived class can override
// this.
// ---------------------------------------------------------
//  

EXPORT_C void CDosAsyncExecBase::AfterComplete(TBool& /*aAutoDestroy*/)
{
}

//
// ---------------------------------------------------------
// CDosAsyncExecBase::DoCancel
//
// Inherited from CActive.
// ---------------------------------------------------------
//  

EXPORT_C void CDosAsyncExecBase::DoCancel()
{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,KErrCancel);
}
