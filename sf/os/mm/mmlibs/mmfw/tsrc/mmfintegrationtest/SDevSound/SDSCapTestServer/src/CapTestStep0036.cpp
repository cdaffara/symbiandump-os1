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

//#include "captestinfoserver.h"
#include "CapTestStep0036.h"

CSecDevSndTS0036* CSecDevSndTS0036::NewL(TThreadId aClientTid)
	{
	CSecDevSndTS0036* self = new (ELeave) CSecDevSndTS0036(aClientTid);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSecDevSndTS0036::CSecDevSndTS0036(TThreadId aClientTid)
	{
	iClientTid = aClientTid;
	}

void CSecDevSndTS0036::ConstructL()
	{
	//iInfoServer = CCapTestInfoServer::NewL();
	RThread serverThread;
	iServerTid = serverThread.Id();
	serverThread.Close();
	}
	
void CSecDevSndTS0036::StartProcessing(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

TVerdict CSecDevSndTS0036::EndProcessingAndReturnResult(TDes8& aMessage)
	{
	TPckgBuf<TThreadId> serverTId(iServerTid);
	aMessage.Copy(serverTId);
	return EPass;
	}

CSecDevSndTS0036::~CSecDevSndTS0036()
	{
	//delete iInfoServer;
	}


