// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the implementation of the test cases that predominately use NT RAS
// as the default interface. These test the use host resolvers under bad username
// and password conditions.
// 
//

/**
 @file
*/

#include "TE_RConnectionBadNTRASTestSteps.h"
#include <in_iface.h>

/*
	Test for fix of DEF016690
*/
TE_RConnectionTest500::~TE_RConnectionTest500() {}

enum TVerdict TE_RConnectionTest500::doTestStepL(void)
{
	TInt err;

	RSocketServ socketServer;
	RHostResolver hostResolver;

	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);

	err = OpenHostResolver(hostResolver, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hostResolver);

	err = MakeSynchronousNameRequest(hostResolver, iTestName); // this will use GetByName()
	TESTEL(KErrIfAuthenticationFailure == err, err);

	DestroyHostResolver(hostResolver);
	CleanupStack::Pop();

	CloseSocketServer(socketServer);
	CleanupStack::Pop(&socketServer);	// just sanity check our pops

	return TestStepResult();
}

/*
Test that an RConnection, attached to an underlying connection in the process of
starting up, will receive a progress with an associated error if the underlying
connection fails to start up.  The old behaviour was that the progress with error
would fail to materialise.  Associated defect is INC100281.
*/
TE_RConnectionTest501::~TE_RConnectionTest501() {}

enum TVerdict TE_RConnectionTest501::doTestStepL(void)
{
	SetTestStepResult(EFail);
	TInt err;

	RSocketServ socketServer;
	RConnection startConn;
	RConnection monitorConn;

	err = OpenSocketServer(socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(socketServer);

	err = OpenConnection(startConn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(startConn);

	err = OpenConnection(monitorConn, socketServer);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(monitorConn);

	// start up connection (destined to fail startup)
	TRequestStatus startStatus;
	StartConnectionAsynchronous(startConn, startStatus);
	
	// wait for completion of selection, after which we can attach to the connection
	TNifProgressBuf progressBuf;
	TRequestStatus progressStatus;
	ProgressNotification(startConn, progressStatus, progressBuf, KFinishedSelection);
	User::WaitForRequest(progressStatus);

	// Attach to the connection from another RConnection
	//
	// It can take a finite time from KFinishedSelection progress to the point where the
	// connection actually appears in the enumeration list, so take this into account by looping
	// waiting for the connection to enumerate.  We are only interested in obtaining the connection
	// info so that we can attach to the starting connection - we are not testing connection
	// enumeration here.

	TInt i = 0;
	TUint count;
	do
		{
		err = EnumerateConnections(startConn, count);
		TESTEL(KErrNone == err, err);
		if (count == 0)
			{
			User::After(50000);
			}
		}
	while (count == 0 && ++i <= 5);
	TESTEL(1 == count, count);

	TPckgBuf<TConnectionInfo> info;
	err = GetConnectionInfo(startConn, 1, info);
	TESTEL(KErrNone == err, err);

	err = AttachMonitor(monitorConn, info);
	TESTEL(KErrNone == err, err);

	ProgressNotification(monitorConn, progressStatus, progressBuf, KLinkLayerClosed);
	
	// wait for the startup to fail
	User::WaitForRequest(startStatus);	
	TESTEL(KErrIfAuthenticationFailure == startStatus.Int(), startStatus.Int());

	// Wait for KLinkLayerClosed or any progress which has an error associated.  Actually,
	// we are more interested in waiting for a progress with an error than KLinkLayerClosed.
	// Use a timer to prevent waiting forever and fail with an error if the progress never arrives.
	RTimer timer;
	err = timer.CreateLocal();
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(timer);

	TRequestStatus timerStatus;
	const TUint KMaxProgressWait = 5;
	timer.After(timerStatus, KMaxProgressWait * 1000000);

	User::WaitForRequest(progressStatus, timerStatus);
	if (progressStatus.Int() == KRequestPending)
		{
		// failure - timer fired
		CancelProgressNotification(monitorConn);
		User::WaitForRequest(progressStatus);
		}

	if (timerStatus.Int() == KRequestPending)
		{
		// success - progress arrived
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	// check that we correctly received a progress with an associated error on the RConnection
	// that attached to the underlying connection.
	TESTEL(KErrNone == progressStatus.Int(), progressStatus.Int());
	TESTEL(KErrIfAuthenticationFailure == progressBuf().iError, progressBuf().iError);
	SetTestStepResult(EPass);

	// ************************************************************************************
	// NOTE:
	// The following pause is apparently necessary to prevent WinTunnel from intermittently
	// entering an endless cpu loop and hanging the emulator.  The cause needs to be
	// investigated and fixed and then this pause can be removed.
	// ************************************************************************************
	User::After(1000000);

	CleanupStack::PopAndDestroy(&timer);
	CloseConnection(monitorConn);
	CleanupStack::Pop(&monitorConn);
	CloseConnection(startConn);
	CleanupStack::Pop(&startConn);
	CloseSocketServer(socketServer);
	CleanupStack::Pop(&socketServer);	// just sanity check our pops

	return TestStepResult();
}

