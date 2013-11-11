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


#include "tetelrequeststatus.h"

/**
*/
void TEtelRequestBase::DoCleanup(TAny* aPtr)
	{
	TEtelRequestBase* me = static_cast<TEtelRequestBase*>(aPtr);
	me->Cleanup();
	}

/**
*/
void TExtEtelRequestStatus::Cancel()
	{
	iSubSess.CancelAsyncRequest(iCancelIpc);
	User::WaitForRequest(iReqStatus);
	}
	

/*
TExtEtelRequest phoneInitrequest(phone, 123);
phone.Initialise(phoneInitrequest);
err = WaitForRequestWithTimout(phoneInitrequest,K1s);


TExtEtelRequest phoneInitrequest(phone, 123);
CleanupStack::PushL(phoneInitrequest);
phone.Initialise(phoneInitrequest);
DoSomethingThatLeaveL();
*/

void TConsoleReadRequestStatus::Cancel()
	{
	iSubSess.ReadCancel();
	User::WaitForRequest(iReqStatus);
	}

