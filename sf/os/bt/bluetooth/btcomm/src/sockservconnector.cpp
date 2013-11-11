// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// sockservconnector.cpp - all the active connector code
// 
//

#include <e32math.h>

#include "btcomm.h"
#include "btcommactive.h"
#include "btcommutil.h"


TInt StartConnector(RSocketServ& aSockServ)
	{
	TInt err = aSockServ.Connect();
	if(err == KErrNone)
		{
		err = aSockServ.ShareAuto();
		if(err != KErrNone)
			{
			aSockServ.Close();
			}
		}
		
	return err;
	}

TInt RunConnectorThread(TAny* aArg)
	{
	// Add the thread id to the end of the the thread name
	// to ensure it is unique.
	TName threadName(KConnectorThreadName);
	RThread thread;
	threadName.AppendNum(static_cast<TUint>(thread.Id()), EHex);

	// We can carry on even if this errors
	User::RenameThread(threadName);
	
	return StartConnector(*reinterpret_cast<RSocketServ*>(aArg));
	}
	
//==============================================================================

CSockServConnector* CSockServConnector::NewL(RSocketServ& aSockServ)
	{
	CSockServConnector* self = new(ELeave) CSockServConnector(aSockServ);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CSockServConnector::ConstructL()
	{
	}

CSockServConnector::~CSockServConnector()
	{
	Cancel();
	}

void CSockServConnector::SockServConnect(TRequestStatus& aStatus)
	{
	TThreadFunction connectorThreadFunction(RunConnectorThread);
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;

	// Once we've connected this thread it is renamed to ensure
	// its name is unique.
	TInt err = iConnectorThread.Create(KConnectorThreadName, connectorThreadFunction,KDefaultStackSize,KMinHeapSize,1024*1000, &iSockServ, EOwnerThread);

	if(err != KErrNone)
		{
		User::RequestComplete(iClientStatus, err);
		}
	else
		{
		// Set up the configuration of the thread
		iConnectorThread.SetPriority(RThread().Priority());
		
		// Stick ourselves on the scheduler jit so we're not
		// cluttering up the queue when we don't want to be 
		// run.
		CActiveScheduler::Add(this);
		
		// Set ourselves to be notified on death of connector thread.
		iConnectorThread.Logon(iStatus);
		SetActive();

		// Mark connector thread ready to run
		iConnectorThread.Resume();
		}
	}

void CSockServConnector::RunL()
	{
	// Remove ourselves from the active scheduler so we
	// don't get in the way of other active objects
	Deque();
	
	// if the thread returned something other than
	// KErrNone then the Esock connect & share didn't
	// succeed.  Go to RunError to notify client and
	// tidy up.
	User::LeaveIfError(iStatus.Int());
	
	// The exit reason was zero, but the thread 
	// terminated abnormally.
	if(iConnectorThread.ExitType() != EExitKill)
		{
		User::Leave(KErrGeneral);
		}

	NotifyAndClose(KErrNone);
	}
	
TInt CSockServConnector::RunError(TInt aError)
	{
	// let client know about result, and gulp
	NotifyAndClose(aError);
	return KErrNone;
	}
	
void CSockServConnector::NotifyAndClose(TInt aError)
	{
	// tell client the result
	User::RequestComplete(iClientStatus, aError);
	// close connector thread
	iConnectorThread.Close();	
	}

void CSockServConnector::DoCancel()
	{
	BTCommUtil::Panic(EBTCommESockSessionAttachNotDone);
	}

// We make this ao high priority because we want to get the session connected asap so we can
// make some progress servicing other requests.
CSockServConnector::CSockServConnector(RSocketServ& aSockServ) : CActive (EPriorityHigh), iSockServ(aSockServ)
	{
	}
